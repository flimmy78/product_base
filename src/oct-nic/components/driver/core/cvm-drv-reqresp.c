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


#include "cvm-driver-defs.h"
#include "cvm-drv-debug.h"
#include "cvm-drv.h"
#include "octeon-opcodes.h"
#include <errno.h>


extern  CVMX_SHARED  cvm_oct_dev_t    *oct;






static inline int
cvm_drv_wait_for_resp_completion(cvm_dma_comp_ptr_t  *comp)
{
	while(comp->comp_byte) {
		CVMX_SYNCWS;
		cvmx_wait(10);
	}

	return 0;
}






static inline int
cvm_drv_prepare_response_dma_header(DMA_ACTION                     action,
                                    cvmx_wqe_t                    *wqe,
                                    cvmx_oct_pci_dma_inst_hdr_t   *dma_hdr)
{
	/* Initialize the dma header. The FL and II bits remain set to 0.
	   ptr is NULL; DIR is outbound C is 0, CA is 0; */
	dma_hdr->u64 = 0;

	/* Use DMA channel 0 and force an interrupt. Speeds up request completion
	   on host. */
	dma_hdr->s.c  = 0;
	dma_hdr->s.fi = 1;

	if(action == DMA_NON_BLOCKING) {
		if(wqe == NULL) {
			printf("NULL WQE found for NON-BLOCKING instr response\n");
			return -EINVAL;
		}
		dma_hdr->s.wqp = 1;
		dma_hdr->s.ptr = CVM_DRV_GET_PHYS(wqe);
	} else {
		cvm_dma_comp_ptr_t  *comp = cvm_get_dma_comp_ptr();
		if(comp == NULL) {
			printf("[ DRV ] comptr alloc failed for BLOCKING response\n");
			return -ENOMEM;
		}
		comp->comp_byte = 0xff;
		dma_hdr->s.ptr   = CVM_DRV_GET_PHYS(&comp->comp_byte);
	}
	return 0;
}










static inline int
cvm_drv_post_response(DMA_ACTION                      action,
                      cvmx_oct_pci_dma_inst_hdr_t    *dma_hdr,
                      cvmx_oct_pci_dma_local_ptr_t   *lptr,
                      cvm_dma_remote_ptr_t           *rptr)
{
	int  retval;

	CVMX_SYNCW;


	if(OCTEON_IS_MODEL(OCTEON_CN56XX) || OCTEON_IS_MODEL(OCTEON_CN63XX) ||
	  OCTEON_IS_MODEL(OCTEON_CN66XX) )
		retval = cvm_pcie_dma_raw(0, dma_hdr, lptr, rptr);
	else
		retval = cvm_pci_dma_raw(dma_hdr, lptr, rptr);

	if(cvmx_unlikely(retval)) {
		return retval;
	} else {
 		if(action == DMA_BLOCKING) {
			cvm_dma_comp_ptr_t *comp = (cvm_dma_comp_ptr_t *)CVM_DRV_GET_PTR(dma_hdr->s.ptr);
			retval = cvm_drv_wait_for_resp_completion(comp);
			cvm_release_dma_comp_ptr(comp);
		}
	}
	return retval;
}







static inline  uint32_t
cvm_dma_fill_scatter_ptrs(cvm_dma_remote_ptr_t   *remote_ptr,
                          octeon_sg_entry_t      *sg_entry,
                          uint32_t                remote_segs)
{
	uint32_t   i, j, k, remote_bytes=0;

	for(i = 0, k = 0; i < remote_segs; k++) {
		for(j = 0; ((j < 4) && (i < remote_segs)); j++, i++)  {
			remote_ptr[i].s.size = sg_entry[k].u.size[3-j];
			remote_ptr[i].s.addr = sg_entry[k].ptr[j];
			DBG_PRINT(DBG_FLOW,"remote_ptr[%d] addr: 0x%llx size: %d\n", i,
			       cast64(remote_ptr[i].s.addr), remote_ptr[i].s.size);
			remote_bytes += remote_ptr[i].s.size;
		}
	}
	return remote_bytes;
}




/*
 * API Function.
 * Send response to host scatter buffers. The response header, data and
 * completion code are available in the data buffers in "lptr".
 */

int
cvm_drv_send_scatter_response_direct(DMA_ACTION              action,
                                     cvmx_wqe_t             *wqe,
                                     cvm_pci_scatter_resp_t *resp,
                                     cvmx_buf_ptr_t          lptr,
                                     uint32_t                local_segs,
                                     cvm_ptr_type_t          ptr_type)
{
	uint32_t                       local_bytes=0, remote_bytes=0;
	cvmx_oct_pci_dma_local_ptr_t   local_ptr[16];
	cvm_dma_remote_ptr_t           remote_ptr[16];
	cvmx_oct_pci_dma_inst_hdr_t    dma_hdr;


	DBG_PRINT(DBG_FLOW,"----cvm_dma_send_scatter_response_direct----\n");

	if( (local_segs > oct->max_lptrs)
	     || (resp->remote_segs > oct->max_rptrs)) {
		printf("[ DRV ] Scatter direct response ptrs (local: %d) (remote: %d) exceeds max allowed\n", local_segs, resp->remote_segs);
		return -EINVAL;
	}


	/* Copy all the local data pointers. */
	local_bytes = cvm_dma_fill_local_ptrs(local_ptr,lptr,local_segs, ptr_type);
	if((int)local_bytes == -1) {
		printf("[ DRV ] Scatter Direct: Local buffer found with size > %d\n",
		        MAX_PCI_DMA_LOCAL_BUF_SIZE);
		return -EINVAL;
	}


	/* Copy all the remote data pointers. */
	remote_bytes = cvm_dma_fill_scatter_ptrs(remote_ptr, resp->sg_entry,
	                                         resp->remote_segs);

	if(local_bytes != remote_bytes) {
		printf("[ DRV ] Scatter Direct Response: Local (%d) & remote (%d) sizes do not match\n", local_bytes, remote_bytes);
		return -EINVAL;
	}

	if(cvm_drv_prepare_response_dma_header(action, wqe, &dma_hdr))
		return  -ENOMEM;

	dma_hdr.s.nl     = local_segs;
	dma_hdr.s.nr     = resp->remote_segs;
	dma_hdr.s.lport  = resp->pcie_port;

	return cvm_drv_post_response(action, &dma_hdr, local_ptr, remote_ptr);
}





/*
 * API Function.
 * Send response to host scatter buffers. The response header and completion
 * code are available from the "resp" parameter. Response data is present in
 * "lptr".
*/

int
cvm_drv_send_scatter_response(DMA_ACTION              action,
                              cvmx_wqe_t             *wqe, 
                              cvm_pci_scatter_resp_t *resp,
                              cvmx_buf_ptr_t          lptr,
                              uint32_t                local_segs,
                              cvm_ptr_type_t          ptr_type)
{
	uint32_t                        remote_bytes = 0, local_bytes = 0, idx=0;
	uint64_t                       *resp_buf = NULL;
	cvmx_oct_pci_dma_local_ptr_t    local_ptr[16];
	cvm_dma_remote_ptr_t            remote_ptr[16];
	cvmx_oct_pci_dma_inst_hdr_t     dma_hdr;
	int                             retval = -EINVAL;


	DBG_PRINT(DBG_FLOW,"----cvm_dma_send_scatter_response----\n");

	if((resp->status & 0xff) == 0xff) {
		printf("[ DRV ] Scatter Response: Byte 0 of status cannot be 0xff\n");
		return -EINVAL;
	}

	/* 1 Local Pointer reserved for resp_hdr, 1 for status. */
	if(local_segs > (oct->max_lptrs-2)) {
		printf("[ DRV ] Scatter Response supports max %d local ptrs (found %d)\n", (oct->max_lptrs-2), local_segs);
		return -EINVAL;
	}

	if(resp->remote_segs > oct->max_rptrs) {
		printf("[ DRV ] Scatter Response supports max %d remote segs (found %d)\n", oct->max_rptrs, resp->remote_segs);
		return -EINVAL;
	}


	resp_buf = (uint64_t *)cvm_drv_fpa_alloc_sync(CVMX_FPA_WQE_POOL);
	if(resp_buf == NULL)  {
		printf("[ DRV ] Buffer alloc failed to send scatter response\n");
		return -ENOMEM;
	}
	resp_buf[0] = resp->orh.u64;
	resp_buf[1] = resp->status;

	/* Prepare the local pointer list now. */
	idx = 0;

	/* The response header comes first. */
	local_ptr[idx].u64 = 0;
	local_ptr[idx].s.addr = CVM_DRV_GET_PHYS(resp_buf);
	local_ptr[idx].s.size = CVMX_RESP_HDR_SIZE;
	local_ptr[idx].s.pool = CVMX_FPA_WQE_POOL;
	idx++;

	/* Followed by all the local data pointers. */
	local_bytes = cvm_dma_fill_local_ptrs(&local_ptr[idx], lptr, local_segs, ptr_type);
	if((int)local_bytes == -1) {
		printf("[ DRV ] Scatter Response: Local buffer found with size > %d\n",
		        MAX_PCI_DMA_LOCAL_BUF_SIZE);
		goto scatter_response_fail;
	}

	idx += local_segs;

	/* Total local bytes = data + resp_hdr + status word */
	local_bytes += CVMX_RESP_HDR_SIZE + 8;
	CVMX_SYNCWS;


	/* And finally the status word. The I bit is set for resp_buf here. */
	local_ptr[idx].u64 = 0;
	local_ptr[idx].s.addr = CVM_DRV_GET_PHYS(&(resp_buf[1]));
	local_ptr[idx].s.size = 8;
	local_ptr[idx].s.pool = CVMX_FPA_WQE_POOL;
	local_ptr[idx].s.i    = 1;
	idx++;


	remote_bytes = cvm_dma_fill_scatter_ptrs(remote_ptr, resp->sg_entry, resp->remote_segs);

	/* The response hdr and status are added separately in this routine. */
	if(local_bytes != remote_bytes) {
		printf("[ DRV ] Scatter Response: Local (%d) & remote (%d) sizes do not match\n", local_bytes, remote_bytes);
		goto scatter_response_fail;
	}

	retval = cvm_drv_prepare_response_dma_header(action, wqe, &dma_hdr);
	if(retval)
		goto scatter_response_fail;

	dma_hdr.s.nl     = idx;
	dma_hdr.s.nr     = resp->remote_segs;
	dma_hdr.s.lport  = resp->pcie_port;

	retval = cvm_drv_post_response(action, &dma_hdr, local_ptr, remote_ptr);

scatter_response_fail:
	if(retval)
		cvm_drv_fpa_free(resp_buf, CVMX_FPA_WQE_POOL, 0);

	return retval;
}










/*
 * API Function.
 * Send response to a single host buffer. The response header, response data and
 * completion code are in the local buffers in "lptr" passed to this function. 
 */

int
cvm_drv_pci_instr_response_direct(DMA_ACTION               action,
                                  cvmx_raw_inst_front_t   *front,
                                  cvmx_buf_ptr_t           lptr,
                                  uint32_t                 segs,
                                  cvm_ptr_type_t           ptr_type,
                                  cvmx_wqe_t              *wqe)
{
	uint32_t                       total_bytes = 0;
	cvmx_oct_pci_dma_local_ptr_t   local_ptr[16];
	cvm_dma_remote_ptr_t           remote_ptr;
	cvmx_oct_pci_dma_inst_hdr_t    dma_hdr;
	int                            retval = -EINVAL;

	DBG_PRINT(DBG_FLOW,"----cvm_dma_send_instr_response_direct----\n");

	if(segs > oct->max_lptrs) {
		printf("[ DRV ] Direct Instr Response support max %d segs, found %d\n",
		       oct->max_lptrs, segs);
		return -EINVAL;
	}



	/* Copy all the local data pointers. */
	total_bytes = cvm_dma_fill_local_ptrs(local_ptr, lptr, segs, ptr_type);
	if((int)total_bytes == -1) {
		printf("[ DRV ] Direct Instr Response Local buffer size > %d\n",
		       MAX_PCI_DMA_LOCAL_BUF_SIZE);
	}


	if(total_bytes != front->irh.s.rlenssz) {
		printf("[ DRV ] Direct Instr Response bytes (%u) != IRH len (%u)\n",
		        total_bytes, front->irh.s.rlenssz);
		return -EINVAL;
	}

	/* The remote pointers are calculated from the rptr */
	/* Scatter mode is not supported right now. */
	remote_ptr.s.addr = front->rptr;
	remote_ptr.s.size = total_bytes;

	retval = cvm_drv_prepare_response_dma_header(action, wqe, &dma_hdr);
	if(retval)
		return retval;

	dma_hdr.s.nl     = segs;
	dma_hdr.s.nr     = 1;
	dma_hdr.s.lport  = front->irh.s.pcie_port;

	return cvm_drv_post_response(action, &dma_hdr, local_ptr, &remote_ptr);
}





/*
 * API Function.
 * Send response to a single host buffer. The response header is passed in
 * "orh", response data in the local buffers in "lptr", and completion code is
 * in "status" parameter passed to this function. 
 */
int
cvm_drv_pci_instr_response(DMA_ACTION              action,
                           cvmx_raw_inst_front_t  *front,
                           cvmx_resp_hdr_t        *orh,
                           uint64_t                status,
                           cvmx_buf_ptr_t          lptr,
                           uint32_t                segs,
                           cvm_ptr_type_t          ptr_type,
                           cvmx_wqe_t             *wqe)
{
	uint32_t                       data_bytes = 0, total_bytes = 0, idx=0;
	uint64_t                      *resp_buf = NULL;
	cvmx_oct_pci_dma_local_ptr_t   local_ptr[16];
	cvm_dma_remote_ptr_t           remote_ptr;
	cvmx_oct_pci_dma_inst_hdr_t    dma_hdr;
	int                            retval = -EINVAL;

	DBG_PRINT(DBG_FLOW,"----cvm_dma_send_instr_response----\n");

	if((status & 0xff) == 0xff) {
		printf("[ DRV ] Byte 0 of status cannot be 0xff\n");
		return  -EINVAL;
	}


	/* 1 reserved for resp_hdr. 1 for status */
	if(segs > (oct->max_lptrs - 2)) {
		printf("[ DRV ] Instr Response does not support %d segs\n", segs);
		return  -EINVAL;
	}

	resp_buf = (uint64_t *)cvm_drv_fpa_alloc_sync(CVMX_FPA_WQE_POOL);
	if(resp_buf == NULL)  {
		printf("[ DRV ] resp_buf alloc failed\n");
		return -ENOMEM;
	}
	resp_buf[0] = orh->u64;
	resp_buf[1] = status;

	/* Prepare the local pointer list now. */
	idx = 0;

	/* The response header comes first. */
	local_ptr[idx].u64 = 0;
	local_ptr[idx].s.addr = CVM_DRV_GET_PHYS(resp_buf);
	local_ptr[idx].s.size = CVMX_RESP_HDR_SIZE;
	local_ptr[idx].s.pool = CVMX_FPA_WQE_POOL;
	idx++;

	/* Followed by all the local data pointers. */
	data_bytes = cvm_dma_fill_local_ptrs(&local_ptr[idx], lptr, segs, ptr_type);
	if((int)data_bytes == -1) {
		printf("[ DRV ] Instr Response: Local buffer found with size > %d\n",
		        MAX_PCI_DMA_LOCAL_BUF_SIZE);
		goto response_fail;
	}

	idx += segs;

	/* Total bytes = data + resp_hdr + status word */
	total_bytes = data_bytes + CVMX_RESP_HDR_SIZE + 8;
	CVMX_SYNCWS;

	/* The response hdr and status are added separately in this routine. */
	if(total_bytes != front->irh.s.rlenssz) {
		printf("[ DRV ] CVM_PCI_DMA: (1) total response bytes (%d) don't match length in IRH (IRH: 0x%016llx)\n", total_bytes, cast64(front->irh.u64));
		goto response_fail;
	}

	/* And finally the status word. The I bit is set for resp_buf here. */
	local_ptr[idx].u64 = 0;
	local_ptr[idx].s.addr = CVM_DRV_GET_PHYS(&(resp_buf[1]));
	local_ptr[idx].s.size = 8;
	local_ptr[idx].s.pool = CVMX_FPA_WQE_POOL;
	local_ptr[idx].s.i    = 1;
	idx++;


	/* The remote pointers are calculated from the rptr */
	/* Scatter mode is not supported right now. */
	remote_ptr.s.addr = front->rptr;
	remote_ptr.s.size = total_bytes;

	retval = cvm_drv_prepare_response_dma_header(action, wqe, &dma_hdr);
	if(retval)
		goto response_fail;

	dma_hdr.s.nl     = idx;
	dma_hdr.s.nr     = 1;
	dma_hdr.s.lport  = front->irh.s.pcie_port;


	retval = cvm_drv_post_response(action, &dma_hdr, local_ptr, &remote_ptr);
response_fail:
	if(retval)
		cvm_drv_fpa_free(resp_buf, CVMX_FPA_WQE_POOL, 0);
	return retval;
}





/*
 * API Function.
 * Call this function to read a scatter list from the host. This function
 * should be called when an instruction from host requires the response to 
 * be copied into scatter buffers. 
 */
int
cvm_drv_read_scatter_list(DMA_ACTION              action,
                          uint64_t                host_hw_addr,
                          cvmx_raw_inst_front_t  *front,
                          octeon_sg_entry_t      *sptr,
                          cvmx_wqe_t             *wqe)
{
	cvm_dma_comp_ptr_t    *comp=NULL;
	cvm_pci_dma_cmd_t      pci_cmd;
	cvmx_buf_ptr_t         lptr;
	cvm_dma_remote_ptr_t   rptr;
	uint32_t               sg_cnt, sg_size;
	int                    retval;

	if((action == DMA_NON_BLOCKING) && (wqe == NULL)) {
		printf("[ DRV ] WQE cannot be NULL for non-blocking scatter read\n");
		return -EINVAL;
	}

	sg_cnt  = front->irh.s.rlenssz;
	sg_size = (ROUNDUP4(sg_cnt) >> 2) * OCT_SG_ENTRY_SIZE;

	pci_cmd.u64  = 0;
	pci_cmd.s.nr = 1;
	pci_cmd.s.nl = 1;

	pci_cmd.s.pcielport = front->irh.s.pcie_port;

	if(action == DMA_NON_BLOCKING) {
		pci_cmd.s.flags = PCI_DMA_INBOUND | PCI_DMA_PUTWQE;
		pci_cmd.s.ptr   = CVM_DRV_GET_PHYS(wqe);
	} else {
		comp = cvm_get_dma_comp_ptr();
		if(comp == NULL) {
			printf("[ DRV ] comptr alloc failed for scatter read\n");
			return -ENOMEM;
		}
		comp->comp_byte = 0xff;
		pci_cmd.s.flags = PCI_DMA_INBOUND | PCI_DMA_PUTWORD;
		pci_cmd.s.ptr   = CVM_DRV_GET_PHYS(&comp->comp_byte);
	}

	/* Read into sptr. Pass the physical address here. */
	lptr.u64    = 0;
	lptr.s.addr = CVM_DRV_GET_PHYS(sptr);
	lptr.s.size = sg_size;

	rptr.s.addr = host_hw_addr;
	rptr.s.size = sg_size;
	CVMX_SYNCWS;

	retval = cvm_pci_dma_recv_data(&pci_cmd, &lptr, &rptr);
	
	if(action == DMA_BLOCKING) {
		/* return error val right away if cvm_pci_dma_recv_data() failed. */
		if(retval)  return retval;
		while(comp->comp_byte) {
			CVMX_SYNCWS;
			cvmx_wait(10);
		}
		cvm_release_dma_comp_ptr(comp);
		return 0;
	}
	return retval;
}

/* $Id: cvm-drv-reqresp.c 66809 2011-11-04 23:26:58Z panicker $ */
