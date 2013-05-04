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
#include "cvmcs-test.h"



int
cvmcs_test_process_wqe(cvmx_wqe_t  *wqe)
{
	cvmx_raw_inst_front_t  *front = (cvmx_raw_inst_front_t *)wqe->packet_data;

	switch(front->irh.s.opcode) {

		case CVMCS_REQRESP_OP:
			//printf("Received Test Request\n");
			cvmcs_process_instruction(wqe);
			break;

		case CVMCS_REQRESP_COMP_OP:
			cvm_free_host_instr(wqe);
			break;

		case CORE_DRV_TEST_SCATTER_OP:
			cvmcs_process_scatter_instruction(wqe);
			break;

#ifdef CVMCS_DMA_DEMO
		case CVMCS_DMA_OP:
			cvmcs_process_dma_demo(wqe);
			break;
#endif

#ifdef CN56XX_PEER_TO_PEER
		case EP_TO_EP_OP:
			cn56xx_process_ep_test_packet(wqe);
			break;
#endif

		default:
			printf("Opcode %x (param: %x) not supported (wqe->len: %d)\n",
			       front->irh.s.opcode, front->irh.s.param, cvmx_wqe_get_len(wqe));
			cvm_drv_print_wqe(wqe);
			return 1;
	}
	return 0;
}





uint8_t
cvmcs_sign_data(uint8_t  *buf, uint32_t  size, uint8_t  start)
{
	while(size--)
		*(buf++) = start++;

	return start;
}



void
cvmcs_test_print_options()
{
#if defined(CVMCS_DMA_DEMO)
	printf(" [ DMA DEMO ] ");
#endif
#ifdef CVMCS_TEST_PKO
	printf(" [ TEST PKO ] ");
#endif
	printf("\n");
}





void
cvmcs_test_global_init(void)
{
#if  defined(CVMCS_TEST_PKO) || defined(MAX_PACKET_RATE_TEST)
	cvmcs_test_pko_global_init();
#endif
}

