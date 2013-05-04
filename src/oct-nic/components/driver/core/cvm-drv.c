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



#include "cvm-drv.h"
#include "cvmx-sysinfo.h"
#include "octeon-opcodes.h"
#include "cvm-pci-loadstore.h"


CVMX_SHARED  cvm_oct_dev_t   *oct;

#if !defined(DISABLE_PCIE14425_ERRATAFIX)
CVMX_SHARED  uint64_t         last_bp_check, bp_check_interval;
#endif

int          cvm_drv_core_id;



#ifdef  CN56XX_PEER_TO_PEER
int
cn56xx_alloc_peeriq_memory(void);

extern void
cn56xx_setup_peeriq_op_handler(void);
#endif


void
cvm_drv_wait(int time_in_us)
{
	cvmx_wait(oct->clocks_per_us * time_in_us);
}





void
cvm_drv_local_init()
{
  cvmx_scratch_write64(CVM_DRV_SCR_PACKET_BUF_PTR,  CVM_COMMON_ALLOC_INIT);
  CVMX_SYNCW;
  cvmx_fpa_async_alloc(CVM_DRV_SCR_PACKET_BUF_PTR, CVMX_FPA_PACKET_POOL);

  cvmx_scratch_write64(CVM_DRV_SCR_WQE_BUF_PTR,  CVM_COMMON_ALLOC_INIT);
  CVMX_SYNCW;
  cvmx_fpa_async_alloc(CVM_DRV_SCR_WQE_BUF_PTR, CVMX_FPA_WQE_POOL);

  cvm_drv_core_id = cvmx_get_core_num();

#if !defined(DISABLE_PCIE14425_ERRATAFIX)
  last_bp_check     = cvmx_get_cycle();
  bp_check_interval = oct->clocks_per_us * BP_CHECK_INTERVAL;
  CVMX_SYNCWS;
#endif

}


void
setup_pci_input_ports(void)
{
	int port;
	cvmx_pip_port_tag_cfg_t  tag_config;
	cvmx_pip_port_cfg_t      port_config;

	for (port = FIRST_PCI_PORT; port <= LAST_PCI_PORT ; port++)   {

		port_config.u64 = 0;

		/* Have each port go to a different POW queue */
		port_config.s.qos  = port - 32;
		port_config.s.mode = CVMX_PIP_PORT_CFG_MODE_NONE;

		/* setup the ports again for ATOMIC tag */
		tag_config.u64            = 0;
		tag_config.s.inc_prt_flag = 1;
		tag_config.s.non_tag_type = CVMX_POW_TAG_TYPE_ATOMIC;
		tag_config.s.grp          = 0;

		/* Set up the PCI Input port configuration */
		cvmx_pip_config_port(port, port_config, tag_config);
	}

}





extern void __cn68xx_qlm_gen2_fix(void);



int
cvm_drv_init()
{

	printf("[ DRV ] Driver Initialization\n");
	if(OCTEON_IS_MODEL(OCTEON_CN56XX_PASS1)) {
		printf("[ DRV ] CN56XX Pass1 is not supported\n");
		return -1;
	}

	printf("[ DRV ] Allocating memory for octeon device\n");
	oct = cvm_drv_fpa_alloc_sync(CVMX_FPA_PACKET_POOL);
	if(oct == NULL) {
		printf("[ DRV ] octeon device alloc failed\n");
		return -1;
	}

	memset(oct, 0, sizeof(cvm_oct_dev_t));


	oct->clocks_per_us = (cvmx_sysinfo_get()->cpu_clock_hz/ (1000 * 1000));

	if(OCTEON_IS_MODEL(OCTEON_CN68XX)) {
		oct->max_dma_qs = 8;
		oct->npi_if     = cvmx_helper_get_interface_num(0x100);
	}

	if( (OCTEON_IS_MODEL(OCTEON_CN63XX)) || (OCTEON_IS_MODEL(OCTEON_CN66XX)) )
		oct->max_dma_qs = 8;

	if(OCTEON_IS_MODEL(OCTEON_CN56XX))
		oct->max_dma_qs = 5;

	if(OCTEON_IS_MODEL(OCTEON_CN58XX) || OCTEON_IS_MODEL(OCTEON_CN38XX))
		oct->max_dma_qs = 2;

	/* Make sure we are not in NULL_NULL POW state
	   (if we are, we cant output a packet) */
	cvmx_pow_work_request_null_rd();

	if(!OCTEON_IS_MODEL(OCTEON_CN68XX))
		setup_pci_input_ports();
	else
		__cn68xx_qlm_gen2_fix();


	if(setup_pci_pko_ports())
		return -1;
	printf("[ DRV ] PKO Done\n");

	/* To indicate a RAW mode packet in WQE word2 */
	cvmx_write_csr(CVMX_PIP_RAW_WORD, cvmx_read_csr(CVMX_PIP_RAW_WORD)|0x8000);
	
	printf("[ DRV ] Setting up DMA Engines\n");
	if(cvm_dma_queue_init(oct)) {
		printf("[ DRV ] DMA Queue initialization failed\n");
		return -1;
	}

	oct->state = CVM_DRV_INIT;

	cvm_setup_pci_load_store();

#ifdef  CN56XX_PEER_TO_PEER
	if(cn56xx_alloc_peeriq_memory())
		return 1;
#endif

	CVMX_SYNCW;
	return 0;
}








int
cvm_drv_change_state(cvmx_wqe_t  *wqe)
{
	cvmx_raw_inst_front_t  *f = (cvmx_raw_inst_front_t *)wqe->packet_data;
	uint32_t                op, status=0;
	uint8_t                 action;

	op = f->irh.s.opcode;


	if(op == DEVICE_STOP_OP) {
		printf(" \n\n [ DRV ] Received Driver STOP Command from Host--\n");

#ifdef CVM_PCI_TRACK_DMA
		cvm_pci_dma_dump_tracker_list();
#endif

		action = (f->irh.s.param & 0xf);

		if((action & DEVICE_PKO) && (oct->pko_state == CVM_DRV_PKO_READY)) {
			oct->pko_state = CVM_DRV_PKO_STOP;
			printf("[ DRV ] Packets to PCI Host will be discarded\n");
		}

		if(action & DEVICE_IPD) {
			cvmx_ipd_disable();
			printf("[ DRV ] IPD is now disabled\n");
		}

	}

	if(op == HOT_RESET_OP) {
		oct->state     = CVM_DRV_RESET;
		oct->pko_state = CVM_DRV_PKO_STOP;
		printf(" \n\n [ DRV ] Received Device Reset from Host--\n");
		printf("[ DRV ] Packets to PCI Host will be discarded\n");
	}

	if(op == DEVICE_START_OP) {
		printf(" \n\n [ DRV ] Received Driver START Command from Host--\n");

		action = (f->irh.s.param & 0xf);

		if(action & DEVICE_PKO) {
			if(oct->pko_state == CVM_DRV_PKO_STOP) {
				printf("[ DRV ] Packets to PCI Host will be forwarded\n");
				oct->pko_state = CVM_DRV_PKO_READY;
			} else {
				status = 0x11000011;
			}
		}

		if(action & DEVICE_IPD) {
			cvmx_ipd_enable();
			printf("[ DRV ] IPD is now enabled\n");
		}
	}

	/* On success, write 0 to host response address */
	if(f->rptr)
		cvm_pci_mem_writel(f->rptr, status);

	cvm_free_host_instr(wqe);
	CVMX_SYNCWS;
	return 0;
}





int
cvm_drv_start()
{
    int                   retval = 0;
    uint8_t              *buf;
    octeon_core_setup_t  *core;
    cvmx_resp_hdr_t      *resp_hdr;
    cvmx_buf_ptr_t        lptr;
    int                   datalen = 0;

    /* Change to state READY temporarily to allow PKO to sent indication
       to host. Check return value to adjust state before exit. */
    oct->pko_state = CVM_DRV_PKO_READY;
    CVMX_SYNCW;

    buf = (uint8_t *) cvm_drv_fpa_alloc_sync(CVMX_FPA_PACKET_POOL);
    if(buf == NULL) {
       printf("\n\n[ DRV ] CRITICAL ERROR!!!\n");
       printf("[ DRV ] Failed to send core driver indicator!!!!\n\n");
       oct->state = CVM_DRV_INIT;
       oct->pko_state = CVM_DRV_PKO_STOP;
       CVMX_SYNCWS;
       return 1;
    }

    resp_hdr = (cvmx_resp_hdr_t *)buf;
    core = (octeon_core_setup_t *)(buf + sizeof(cvmx_resp_hdr_t));
    datalen = sizeof(octeon_core_setup_t);

    resp_hdr->u64         = 0;
    resp_hdr->s.opcode    = CORE_DRV_ACTIVE_OP;
    resp_hdr->s.destqport = oct->app_code;

    core->corefreq = oct->clocks_per_us * 1000 * 1000;

    lptr.u64    = 0;
    lptr.s.size = sizeof(cvmx_resp_hdr_t) + datalen;
    lptr.s.addr = CVM_DRV_GET_PHYS(buf);
    lptr.s.pool = CVMX_FPA_PACKET_POOL;
    lptr.s.i    = 1;
    printf("[ DRV ] Core app is active. Sending indication to host\n");
    CVMX_SYNCWS;

	/* Send the indication packet on the first output queue. */
   	retval = cvm_send_pci_pko_direct(lptr, CVM_DIRECT_DATA, 1, lptr.s.size, 0);

    /* Register the HOT RESET handler */
    printf("Registering HotReset Handler (Opcode: 0x%x)\n", HOT_RESET_OP);
    cvm_drv_register_op_handler(HOT_RESET_OP,  cvm_drv_change_state); 

    printf("Registering Output Queue Command Handler (Opcode: 0x%x)\n",
           DEVICE_STOP_OP);
    cvm_drv_register_op_handler(DEVICE_STOP_OP,  cvm_drv_change_state);

    printf("Registering Output Queue Command Handler (Opcode: 0x%x)\n",
           DEVICE_START_OP);
    cvm_drv_register_op_handler(DEVICE_START_OP,  cvm_drv_change_state);


#ifdef  CN56XX_PEER_TO_PEER
    cn56xx_setup_peeriq_op_handler();
#endif

    /* With SDK 1.8.1, check for PASS2 include 56xx 2.x devices.
       Keep Check for Pass2.1 here to be compatible with 1.8.0 */
    if( (OCTEON_IS_MODEL(OCTEON_CN56XX_PASS2)
        || OCTEON_IS_MODEL(OCTEON_CN56XX_PASS2_1)) ) {
        uint64_t  val64 = cvmx_read_csr(CVMX_IOB_INT_ENB);
        printf("Disabling NP_DAT NP_SOP & NP_EOP for 56xx Pass2\n");
        val64 &= ~(0x13ULL);
        cvmx_write_csr(CVMX_IOB_INT_ENB, val64);  
    }

    /* If pko send was not successful, reset the state to INIT */
    if(retval) {
        oct->pko_state = CVM_DRV_PKO_STOP;
    } else {
        oct->state = CVM_DRV_READY;
        printf(" [ DRV ] Octeon device is in READY state\n");
    }

    CVMX_SYNCW;

    return retval;
}





int
cvm_drv_process_instr(cvmx_wqe_t   *wqe)
{
	cvmx_raw_inst_front_t  *front = (cvmx_raw_inst_front_t *)wqe->packet_data;
	uint16_t   op_tail;

	DBG_PRINT(DBG_FLOW,"\n-----cvm_drv_process_instr-----\n");
	op_tail = (front->irh.s.opcode & 0xff);
	if((op_tail <= MAX_DRV_OPCODES)) {
		if(oct->optable[op_tail]) {
			oct->optable[op_tail](wqe);
			return 0;
		}
	} 
	printf("[ DRV ] Unsupported Opcode: %x in instruction\n", front->irh.s.opcode);
	cvm_free_host_instr(wqe);
	return 1;
}




int
cvm_drv_register_op_handler(uint16_t   opcode, int (*handler)(cvmx_wqe_t *))
{
	uint16_t   op_head = ((opcode & 0xff00) >> 8);
	uint16_t   op_tail = (opcode & 0xff);

	if((op_head != 0x10 && op_head != 0x11) || (op_tail > MAX_DRV_OPCODES)) {
		printf("[ DRV ] OPCODE %x is not the supported range. Registration failed\n", opcode);
		return 1;
	}

	printf("[ DRV ] Registered handler @ %p for opcode: %x \n", handler, opcode);
	oct->optable[op_tail] = handler;
	return 0;
}





void
cvm_drv_setup_app_mode(int app_mode)
{
	oct->app_code |= app_mode;
}



void
cvm_56xx_pass2_update_pcie_req_num(void)
{
	uint64_t   mval = 8 + (cvmx_get_cycle() & 0x7);
	uint64_t   req_num = 0ULL;
	

	req_num = ( (1ULL << 63) |
				(mval << 48) |
				(mval << 32) |
				(mval << 24) |
				(mval << 16) |
				(mval << 8) |
				(0x10) );

	cvmx_write_csr(CVMX_ADD_IO_SEG(0x00011F00000085B0), req_num);
}







int
octdev_get_device_id(void)
{
	return oct->dev_id;
}


int
octdev_max_dma_localptrs(void)
{
	return oct->max_lptrs;
}


int
octdev_max_dma_remoteptrs(void)
{
	return oct->max_rptrs;
}

int
octdev_max_dma_sumptrs(void)
{
	return oct->max_dma_ptrs;
}

int
octdev_get_state(void)
{
	return oct->state;
}

int
octdev_get_pko_state(void)
{
	return oct->pko_state;
}

#ifdef CN56XX_PEER_TO_PEER
int
octdev_get_max_peers(void)
{
	return oct->max_peers;
}
#endif



/* Do not use this function within any application. This is kept here for
   test purposes only.
*/
void octdev_force_state_to_ready(void)
{
	oct->state = CVM_DRV_READY;
}




#if !defined(DISABLE_PCIE14425_ERRATAFIX)
void
cvmcs_sw_iq_bp(void)
{
	uint64_t pkt_pool_cnt, wqe_pool_cnt;
	uint64_t scratch_reg;

	if(cvmx_get_cycle() < (last_bp_check + bp_check_interval))
		return;

	if( !(OCTEON_IS_MODEL(OCTEON_CN56XX) || OCTEON_IS_MODEL(OCTEON_CN63XX) 
	       || OCTEON_IS_MODEL(OCTEON_CN66XX) || OCTEON_IS_MODEL(OCTEON_CN68XX)) )
		return;

	if(OCTEON_IS_MODEL(OCTEON_CN56XX))
		scratch_reg  = CVMX_PEXP_NPEI_SCRATCH_1;

	if(OCTEON_IS_MODEL(OCTEON_CN63XX) || OCTEON_IS_MODEL(OCTEON_CN66XX)
	   || OCTEON_IS_MODEL(OCTEON_CN68XX) )
		scratch_reg  = CVMX_PEXP_SLI_SCRATCH_1;

	pkt_pool_cnt = cvmx_read_csr(CVMX_FPA_QUEX_AVAILABLE(CVMX_FPA_PACKET_POOL));
	wqe_pool_cnt = cvmx_read_csr(CVMX_FPA_QUEX_AVAILABLE(CVMX_FPA_WQE_POOL));

	if( (pkt_pool_cnt > BP_HIGH_WMARK) && (wqe_pool_cnt > BP_HIGH_WMARK) )
		cvmx_write_csr(scratch_reg, 0);
	if( (pkt_pool_cnt < BP_LOW_WMARK) || (wqe_pool_cnt < BP_LOW_WMARK) )
		cvmx_write_csr(scratch_reg, 1);

	last_bp_check = cvmx_get_cycle();

	CVMX_SYNCWS;
}
#endif



/* $Id: cvm-drv.c 66446 2011-10-25 02:31:59Z mchalla $ */
