/*
 *
 * OCTEON SDK
 *
 * Copyright (c) 2011 Cavium Networks. All rights reserved.
 *
 * This file, which is part of the OCTEON SDK which also includes the
 * OCTEON SDK Package from Cavium Networks, contains proprietary and
 * confidential information of Cavium Networks and in some cases its
 * suppliers. 
 *
 * Any licensed reproduction, distribution, modification, or other use of
 * this file or the confidential information or patented inventions
 * embodied in this file is subject to your license agreement with Cavium
 * Networks. Unless you and Cavium Networks have agreed otherwise in
 * writing, the applicable license terms "OCTEON SDK License Type 5" can be found 
 * under the directory: $OCTEON_ROOT/components/driver/licenses/
 *
 * All other use and disclosure is prohibited.
 *
 * Contact Cavium Networks at info@caviumnetworks.com for more information.
 *
 */



#include "cvmcs-common.h"
#include "cvmcs-nic.h"
#include  <cvmx-atomic.h>



/* Enable this flag if you want to test peer-to-peer communication.
   Packets received on the GMX ports will be forwarded to the peer to be sent
   out from the same GMX port (e.g. pkt arriving from port 16 on a 56xx will
   be forwarded to the peer which will send it out on its port 16).
*/
//#define  ENABLE_NIC_PEER_TO_PEER


#ifdef ENABLE_NIC_PEER_TO_PEER
#include "cn56xx_ep_comm.h"
#endif




CVMX_SHARED uint32_t wqe_count[MAX_CORES] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};

CVMX_SHARED octnic_dev_t  *octnic;


extern CVMX_SHARED  uint64_t          cpu_freq;
extern CVMX_SHARED  cvm_oct_dev_t    *oct;
extern uint32_t  core_id;



#ifdef __linux__
void (*prev_sig_handler)(int);
#endif



void
cvmcs_print_compile_options()
{
	printf("Application compiled with: ");
#ifdef  CVMCS_DUTY_CYCLE
	printf("[ DUTY CYCLE ] ");
#endif
#ifdef ENABLE_NIC_PEER_TO_PEER
	printf("[ PEER TO PEER ] ");
#endif
}







static inline void
cvmcs_nic_print_stats(void)
{
	int i;
	oct_link_stats_t  *st;

	printf("-- RGMII driver stats --\n");
	printf(" (Rx pkts from wire; Tx pkts from host)\n");
	for(i = 0; i < octnic->nports; i++) {
		printf("Port%d: ", i); 
		st = &octnic->port[i].stats;

		printf(" Rx : %llu ", cast64(st->fromwire.total_rcvd));
		if(st->fromwire.err_pko)
			printf("(%llu PKO Err) ", cast64(st->fromwire.err_pko));
		if(st->fromwire.err_link)
			printf("(%llu Link Err) ", cast64(st->fromwire.err_link));
		if(st->fromwire.err_drop)
			printf("(%llu Drops) ", cast64(st->fromwire.err_drop));

		printf(" Tx : %llu ", cast64(st->fromhost.total_rcvd));
		if(st->fromhost.err_pko)
			printf("(%llu PKO Err) ", cast64(st->fromhost.err_pko));
		if(st->fromhost.err_link)
			printf("(%llu Link Err) ", cast64(st->fromhost.err_link));
		if(st->fromhost.err_drop)
			printf("(%llu Drops) ", cast64(st->fromhost.err_drop));
		printf("\n");
	}
}







/** Setup the FPA pools. The Octeon hardware, simple executive and
  * PCI core driver use  WQE and Packet pool. OQ pool is used to
  * allocate command buffers for Output queue by simple exec.
  * Test pool is used by this application. 
  */
int
cvmcs_nic_setup_memory()
{

	cvmx_fpa_enable();

	if( cvmcs_app_mem_alloc("Packet Buffers", CVMX_FPA_PACKET_POOL,
	                         CVMX_FPA_PACKET_POOL_SIZE, FPA_PACKET_POOL_COUNT))
		return 1;

	if( cvmcs_app_mem_alloc("Work Queue Entries", CVMX_FPA_WQE_POOL,
	                         CVMX_FPA_WQE_POOL_SIZE, FPA_WQE_POOL_COUNT))
		return 1;

	if( cvmcs_app_mem_alloc("PKO Command Buffers",CVMX_FPA_OUTPUT_BUFFER_POOL,
	                     CVMX_FPA_OUTPUT_BUFFER_POOL_SIZE, FPA_OQ_POOL_COUNT))
		return 1;

	if(OCTEON_IS_MODEL(OCTEON_CN68XX) && (cvmx_helper_initialize_sso(FPA_WQE_POOL_COUNT)))
       return -1;

	cvm_common_fpa_display_all_pool_info();

	return 0;
}







extern int __get_active_pci_oq_count(void);


/** Global initialization. Performed by the boot core only. */
int
cvmcs_nic_init_global()
{
	if(cvmcs_nic_setup_memory())
		return 1;

	if(cvmcs_app_init_global())
		return 1;

	cvmx_helper_ipd_and_packet_input_enable();
	
	/* overwriting the SLI_TX_PIPE Register with active queue count of 4 */ 
	if(OCTEON_IS_MODEL(OCTEON_CN68XX)) {
		uint32_t 	    activeqcnt;
		cvmx_sli_tx_pipe_t  slitxpipe;

		slitxpipe.u64 = cvmx_read_csr(CVMX_PEXP_SLI_TX_PIPE);
		activeqcnt = __get_active_pci_oq_count();
		slitxpipe.s.nump = activeqcnt;
		cvmx_write_csr(CVMX_PEXP_SLI_TX_PIPE, slitxpipe.u64);
		printf("[ NIC APP ] Active PCI Queues: %d (derived from checking queue registers)\n", activeqcnt);
	}

	/* Enable IPD only after sending the START indication packet to host. */
	cvmx_ipd_disable();

	return cvmcs_nic_setup_interfaces();

}





/** Local initialization. Performed by all cores. */
int
cvmcs_nic_init_local()
{
	if(cvmcs_app_init_local())
		return 1;

	CVMX_SYNCW;
	return 0;
}





#ifdef ENABLE_NIC_PEER_TO_PEER
int
cvmcs_nic_forward_pkt_to_ep(cvmx_wqe_t  *wqe, int ifidx)
{
	cn56xx_ep_packet_t  pkt;
	int                 retval;

	cvmx_atomic_add64(&octnic->port[ifidx].stats.fromwire.total_rcvd, 1);

	memset(&pkt, 0, sizeof(cn56xx_ep_packet_t));

	pkt.bufcount = 1;

	pkt.buf[0].s.addr = wqe->packet_ptr.s.addr;
	pkt.buf[0].s.size = cvmx_wqe_get_len(wqe);
	pkt.buf[0].s.pool = CVMX_FPA_PACKET_POOL;
	pkt.buf[0].s.i    = 1;

	pkt.tag     = 0x11001100;
	pkt.tagtype = CVMX_POW_TAG_TYPE_ORDERED;
	pkt.param   = cvmx_wqe_get_port(wqe);
	pkt.opcode  = EP_TO_EP_OP;
	
	DBG("Sending test packet with opcode: %x param: %x\n", pkt.opcode, pkt.param);


	retval = cn56xx_send_ep_packet(&pkt);

	/* If packet was sent successfully, the packet buffers would be freed by the
	   core driver EP communication code. Else we need to free it here. */
	if(retval == 0) {
		wqe->word2.s.bufs   = 0;
		wqe->packet_ptr.u64 = 0;
		cvmx_atomic_add64(&octnic->port[ifidx].stats.fromwire.total_fwd, 1);
	} else {
		cvmx_atomic_add64(&octnic->port[ifidx].stats.fromwire.err_drop, 1);
	}

	cvm_free_host_instr(wqe);

	return retval;
}
#endif











void
cvmcs_nic_dump_ptrs(cvmx_buf_ptr_t  *ptr, int numbufs)
{
	int              i, total=0;
	cvmx_buf_ptr_t  *next;

	for(i = 0; i < numbufs; i++) {
		next = (cvmx_buf_ptr_t *)CVM_DRV_GET_PTR(ptr->s.addr - 8);
		printf("ptr[%d]: 0x%016llx  size: %d pool %d\n", i, CAST64(ptr->s.addr),
		       ptr->s.size, ptr->s.pool);
		total += ptr->s.size;
		ptr = next;
	}

	printf("Total Bytes: %d\n", total);
}

















/* Direction: 0 - to wire, 1 - to host */
static int
cvmcs_nic_send_to_pko(cvmx_wqe_t  *wqe,  int dir, int port, int queue, int offload)
{
	cvmx_pko_command_word0_t    pko_command;

	CVMX_SYNCWS;

	/* Prepare to send a packet to PKO. */
	if (dir && octeon_has_feature(OCTEON_FEATURE_PKND))
		cvmx_pko_send_packet_prepare_pkoid(port, queue, 1);
	else
		cvmx_pko_send_packet_prepare(port, queue, 1);

	/* Build a PKO pointer to this packet */
	pko_command.u64           = 0;

	/* Setting II = 0 and DF = 0 will free all buffers whose I bit is not set.
	   Since I bit is not set by default in pkt buffer ptrs, this setting allows
	   packets to be forwarded to host/wire without having to touch each pkt
	   ptr to set the I bit. */
	pko_command.s.ignore_i    = 0;
	pko_command.s.dontfree    = 0;
	pko_command.s.segs        = wqe->word2.s.bufs;
	pko_command.s.total_bytes = cvmx_wqe_get_len(wqe);
	if(offload)
		pko_command.s.ipoffp1 = offload;

	DBG_PRINT(DBG_FLOW,"pko cmd: %016llx lptr: %016llx PORT: %d Q: %d\n",
	          cast64(pko_command.u64), cast64(wqe->packet_ptr.u64), port,
	          queue);

	/* Send a packet to PKO. */
	if (dir && octeon_has_feature(OCTEON_FEATURE_PKND))
		cvmx_pko_send_packet_finish_pkoid(port, queue, pko_command, wqe->packet_ptr, 1);
	else
		cvmx_pko_send_packet_finish(port, queue, pko_command, wqe->packet_ptr, 1);

	return 0;
}







static void
cvmcs_nic_forward_packet_to_host(cvmx_wqe_t  *wqe, octnic_port_info_t  *nicport)
{
	cvmx_resp_hdr_t     *resp_ptr;
	cvmx_buf_ptr_t      *lptr;
	int                  port, queue;

	cvmx_atomic_add64(&nicport->stats.fromwire.total_rcvd, 1);

	if(!nicport->state.rx_on) {
		cvmx_atomic_add64(&nicport->stats.fromwire.err_drop, 1);
		cvm_free_wqe_and_pkt_bufs(wqe);
		return;
	}

	wqe->word2.s.bufs += 1;
	cvmx_wqe_set_len(wqe, (cvmx_wqe_get_len(wqe) + CVMX_RESP_HDR_SIZE));

	/* We use the space in WQE starting at byte offset 40 for the response
	   header information. This allows the WQE to be used for data and let
	   PKO free the WQE buffer. */
	lptr      = (cvmx_buf_ptr_t *)&wqe->packet_data[40];
	lptr->u64 = wqe->packet_ptr.u64;

	resp_ptr = (cvmx_resp_hdr_t  *)&wqe->packet_data[48];
	resp_ptr->u64         = 0;
	resp_ptr->s.opcode    = CORE_NW_DATA_OP;
	resp_ptr->s.destqport = nicport->linfo.ifidx;

	/* lptr is reused */
	lptr         = &wqe->packet_ptr;
	lptr->u64    = 0;
	lptr->s.addr = CVM_DRV_GET_PHYS(resp_ptr);
	lptr->s.size = 8;
	lptr->s.pool = CVMX_FPA_WQE_POOL;

	DBG("wqe @ %p wqe bufs: %d len: %d pkt_ptr @ %lx\n", wqe,
	        wqe->word2.s.bufs, cvmx_wqe_get_len(wqe), wqe->packet_ptr.s.addr);

	port  = cvm_pci_get_oq_pkoport(nicport->linfo.rxpciq);
	queue = cvm_pci_get_oq_pkoqueue(nicport->linfo.rxpciq);

	if(cvmx_unlikely(port == -1 || queue == -1)) {
		cvmx_atomic_add64(&nicport->stats.fromwire.err_drop, 1);
		cvm_free_wqe_and_pkt_bufs(wqe);
		return;
	}

	DBG("to host: rxq: %d port: %d queue: %d\n", nicport->linfo.rxpciq, port,
	    queue);

	if(!cvmcs_nic_send_to_pko(wqe, 1, port, queue, 0)) {
		cvmx_atomic_add64(&nicport->stats.fromwire.total_fwd, 1);
		return;
	}

	cvmx_atomic_add64(&nicport->stats.fromwire.err_pko, 1);
	cvm_free_wqe_and_pkt_bufs(wqe);
}







static void
cvmcs_nic_forward_packet_to_wire(cvmx_wqe_t   *wqe, int ifidx, int offload)
{
	uint64_t             nextptr;
	octnic_port_info_t  *nicport = &octnic->port[ifidx];
	int                  port, queue;


	cvmx_atomic_add64(&nicport->stats.fromhost.total_rcvd, 1);

	if(cvmx_unlikely(nicport->linfo.link.s.status == 0) ) {
		cvmx_atomic_add64(&nicport->stats.fromhost.err_link, 1);
		cvm_free_wqe_and_pkt_bufs(wqe);
		return;
	}

	nextptr = *((uint64_t *)CVM_DRV_GET_PTR(wqe->packet_ptr.s.addr - 8));
	wqe->packet_ptr.s.addr += CVM_RAW_FRONT_SIZE;
	wqe->packet_ptr.s.size -= CVM_RAW_FRONT_SIZE;
	cvmx_wqe_set_len(wqe, (cvmx_wqe_get_len(wqe) - CVM_RAW_FRONT_SIZE));
	*((uint64_t *)CVM_DRV_GET_PTR(wqe->packet_ptr.s.addr - 8)) = nextptr;

	port  = nicport->linfo.gmxport;
	queue = cvmx_pko_get_base_queue(nicport->linfo.gmxport);

	if(cvmx_unlikely(port == -1 || queue == -1)) {
		cvmx_atomic_add64(&nicport->stats.fromhost.err_drop, 1);
		cvm_free_wqe_and_pkt_bufs(wqe);
		return;
	}

	DBG("to wire: port: %d queue: %d\n", port, queue);
	if(!cvmcs_nic_send_to_pko(wqe, 0, port, queue, offload)) {
		cvmx_atomic_add64(&nicport->stats.fromhost.total_fwd, 1);
		cvm_drv_fpa_free(wqe, CVMX_FPA_WQE_POOL, 0);
		return;
	}

	cvmx_atomic_add64(&nicport->stats.fromwire.err_pko, 1);
	cvm_free_wqe_and_pkt_bufs(wqe);
}







/** All work received by the application is forwarded to this routine.
  * All RAW packets with opcode=0x1234 and param=0x10 are test instructions
  * and handle by the application. All other RAW packets with opcode in
  * the range 0x1000-0x1FFF is given to the core driver. All other packets
  * are dropped.
  */
static inline int
cvmcs_nic_process_wqe(cvmx_wqe_t  *wqe)
{
	cvmx_raw_inst_front_t     *front;
	uint32_t                   opcode;

	front = (cvmx_raw_inst_front_t *)wqe->packet_data;


	if(wqe->word2.s.software)   {

		DBG("RawWQE @ %p ipprt: %d bufs: %d len: %d opcode: %x param: %d\n",
		    wqe, cvmx_wqe_get_port(wqe), wqe->word2.s.bufs,
		    cvmx_wqe_get_len(wqe), front->irh.s.opcode,  front->irh.s.param);

		front = (cvmx_raw_inst_front_t *)wqe->packet_data;
		opcode = front->irh.s.opcode;

		switch(opcode) {

#ifdef ENABLE_NIC_PEER_TO_PEER
			case EP_TO_EP_OP:
				cvmcs_nic_forward_packet_to_wire(wqe, front->irh.s.param);
				break;
#endif

			case OCT_NW_PKT_OP:
				cvmcs_nic_forward_packet_to_wire(wqe, front->irh.s.param,
				                                 front->irh.s.rlenssz);
				break;

			case OCT_NW_CMD_OP:
				cvmcs_nic_process_cmd(wqe);
				break;

			case HOST_NW_INFO_OP:
				cvmcs_nic_send_link_info(wqe);
				break;

			default: 
				if(opcode >= DRIVER_OP_START && opcode <= DRIVER_OP_END) {
					cvm_drv_process_instr(wqe);
				} else {
					cvm_free_host_instr(wqe);
				}
		} /* switch */

	} else  {

		int  ifidx = cvmcs_nic_find_idx(cvmx_wqe_get_port(wqe));

		DBG("PktWQE @ %p ipprt: %d ifidx: %d bufs: %d len: %d\n",
		    wqe, cvmx_wqe_get_port(wqe), ifidx, wqe->word2.s.bufs,
		    cvmx_wqe_get_len(wqe));

		if(ifidx == -1) {
			printf("No NIC idx for WQE from ipprt: %d bufs: %d len: %d\n",
			       cvmx_wqe_get_port(wqe), wqe->word2.s.bufs,
			       cvmx_wqe_get_len(wqe));
			goto unrecognized_pkt;
		}

		if(octnic->port[ifidx].state.active) {

			if(cvmx_likely(!wqe->word2.snoip.rcv_error)) {

				#ifdef ENABLE_NIC_PEER_TO_PEER
				cvmcs_nic_forward_pkt_to_ep(wqe, ifidx);
				#else
				cvmcs_nic_forward_packet_to_host(wqe, &octnic->port[ifidx]);
				#endif

			} else {

				octnic->port[ifidx].stats.fromwire.err_drop++;

				DBG("L2/L1 error from port %d. Error code=%x\n",
				    ipprt, wqe->word2.snoip.err_code);
				cvm_free_host_instr(wqe);

			}
			return 0;
		}

unrecognized_pkt:
		printf("Unsupported WQE format @ %p\n", wqe);
		cvm_drv_print_data(wqe, 64);
		cvm_free_host_instr(wqe);

	}

	return 0;
}








/** This loop is run by all cores running the application.
  * If any work is available it is processed. If there is no work
  * and 
  * If CVMCS_DUTY_CYCLE is enabled, it prints useful statistics
  * at intervals determined by DISPLAY_INTERVAL.
  * If CVMCS_TEST_PKO is enabled, then packets are sent to PKO 
  * port 32 at intervals determined by "cvmcs_run_freq".
  * If CVMCS_FIXED_SIZE_TEST is enabled, packets of fixed size
  * are sent. Else packet size can be of range 1-CVM_MAX_DATA
  * where CVM_MAX_DATA cannot be > 65520 bytes.
  */
int
cvmcs_nic_data_loop()
{
	cvmx_wqe_t    *wqe;
	uint64_t       last_display=0, last_link_check=0;
	uint64_t       idle_counter = 0, pko_test_counter=0;

	printf("# cvmcs: Data loop started on core[%d]\n", core_id);
	do {

#if !defined(DISABLE_PCIE14425_ERRATAFIX)
		if(is_display_core(core_id)) {
			cvmcs_sw_iq_bp();
		}
#endif

		wqe = cvmcs_app_get_work_sync(0);
		if (wqe) {
			wqe_count[core_id]++;
			cvmcs_nic_process_wqe(wqe);
			continue;
		}

		idle_counter++;
		pko_test_counter++;

	/* There is no definition of model PASS2_0 in SDK 1.8.0; check for 
	   model CN56XX_PASS2 returns true for CN56XX Pass2.0 parts only.
	   In SDK 1.8.1, check for model CN56XX_PASS2 returns true
	   for all CN56XX Pass 2.X parts, so use model CN56XX_PASS2_0 instead.
	*/
#if (OCTEON_SDK_VERSION_NUM < 108010000ULL)
		if(OCTEON_IS_MODEL(OCTEON_CN56XX_PASS2))
#else
		if(OCTEON_IS_MODEL(OCTEON_CN56XX_PASS2_0))
#endif
			cvm_56xx_pass2_update_pcie_req_num();


		if(is_display_core(core_id)) {

			if( (cvmx_get_cycle() - last_link_check)
				 >= (cpu_freq * LINK_CHECK_INTERVAL)) {
				cvmcs_nic_check_link_status();
				last_link_check = cvmx_get_cycle();
			}
			
#ifdef  CVMCS_DUTY_CYCLE
			if( (cvmx_get_cycle() - last_display)
				 >= (cpu_freq * DISPLAY_INTERVAL)) {

				cvmcs_app_duty_cycle_actions();
				cvmcs_nic_print_stats();
				last_display = cvmx_get_cycle();
			}
#endif
		}
	} while(1);
	printf("# cvmcs: Core %d Exited from data loop\n", core_id);
}






#ifdef __linux__
void signal_handler(int x)
{
        printf("# cvmcs: Received signal %d, quitting now!!\n", x);
        signal(SIGINT, prev_sig_handler );
        cvmcs_app_shutdown();
        exit(0);
}
#endif







static void
modify_ipd_settings(void)
{
	cvmx_ipd_ctl_status_t ipd_reg;
	ipd_reg.u64 = cvmx_read_csr(CVMX_IPD_CTL_STATUS);
	ipd_reg.s.len_m8 = 1;
	cvmx_write_csr(CVMX_IPD_CTL_STATUS, ipd_reg.u64);
}







/** MAIN */
int main()
{
	if(cvmcs_app_bringup())
		return 1;

	if(is_boot_core(core_id)) {

		printf("SDK Build Number: %s\n", SDKVER);
		cvmcs_print_compile_options();
		printf("\n# cvmcs-nic: Starting global init on core %d\n", core_id);

		if(cvmcs_nic_init_global()) 
			return 1;
		printf("# cvmcs-nic: Global initialization completed\n\n");
	}

	cvmcs_app_barrier();
	
	/* Initialization local to each core */
	cvmcs_nic_init_local();

	if(is_boot_core(core_id)) {

#ifdef __linux__
		prev_sig_handler = signal(SIGINT, signal_handler);
#endif

		cvmx_helper_setup_red(RED_HIGH_WMARK, RED_LOW_WMARK);

		cvm_drv_setup_app_mode(CVM_DRV_NIC_APP);
		cvm_drv_start();

		/* Modify default IPD settings */
		modify_ipd_settings();

		/* Enable IPD only after sending the START indication packet to host. */
		cvmx_ipd_enable();

		print_pool_count_stats();
	}

	cvmcs_app_barrier();


	/* Start the data processing loop on each core. */
	cvmcs_nic_data_loop();

	cvmcs_app_barrier();

	if(is_boot_core(core_id))
		cvmcs_app_shutdown();

	return 0;
}


/* $Id: cvmcs-nic-main.c 67436 2011-11-25 17:52:40Z mchalla $ */
