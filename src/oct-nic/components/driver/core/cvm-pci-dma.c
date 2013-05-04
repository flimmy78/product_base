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
#include <errno.h>
#include "octeon-model.h"
#include "cvm-pci-dma.h"
#include  "cavium-list.h"

/* Validate the PCI DMA command, local and remote pointers. */
#define CVM_PCI_VALIDATE_DMA_POINTERS


#ifdef PROFILE_PCI_DMA
/* Clock cycle value when the doorbell was last hit by each core. */
uint64_t   doorbell_hit_time[MAX_OCTEON_CORES];
#endif


extern  int          cvm_drv_core_id;
extern  CVMX_SHARED  cvm_oct_dev_t    *oct;

CVMX_SHARED  char  dmadirstring[4][10] =
	{ "OUTBOUND", "INBOUND", "EXTERNAL", "INTERNAL" };


#ifdef CVM_PCI_TRACK_DMA

/*
   If the CVM_PCI_TRACK_DMA flag is enabled, the core driver will create
   a  WQE  that records the contents of the DMA including the DMA header
   and the local & remote pointers. If the original command required  a
   WQE to be scheduled or a byte to be reset to 0, the core driver makes
   a copy of the original pointer.
   The WQE is maintained by the core driver in an internal list and the
   DMA  header  is modified to schedule this WQE on DMA completion. The
   application is required to call the cvm_pci_dma_remove_from_tracker_list()
   API if any work arrives with input port set to CVM_PCI_DMA_TRACKER_PORT.
   Application can call cvm_pci_dma_dump_tracker_list() to dump the
   internal list to see the list of DMA commands that have not been
   processed by the DMA engine yet.
*/

typedef struct {
	cavium_list_t   list;           // Byte 0 - 15
	uint64_t        cnt:8;          // Byte 16 - 23
	uint64_t        allbufs:8;
	uint64_t        pktnum:32;
	uint64_t        rsvd:16;
	uint64_t        dmawords[36];   // Byte 24 onwards
} cvm_pci_dma_tracker_t;



CVMX_SHARED struct __dma_tracker{
	cvmx_spinlock_t  lock;
	cavium_list_t    head;
	int              pool;
	int              maxwords;
	int              pktnum;
	int              rsvd;
	struct __dma_tracker_stats {
		uint64_t         dir[4];
		uint64_t         total;
		uint64_t         max_lptrs;
		uint64_t         max_rptrs;
		uint64_t         max_sumptrs;
	} stats;
} dmatracker;

#endif


static int  cvm_post_pcie_dma_command(int q_no,
                                      cvmx_oct_pci_dma_inst_hdr_t *dma_hdr,
                                      void *firstptrs, void *lastptrs);



/** Print the contents of the 64-bit DMA Command  */
void
cvm_pci_dma_print_header(cvmx_oct_pci_dma_inst_hdr_t  *dmahdr)
{
	printf("DMAHdr: 0x%016llx\n", cast64(dmahdr->u64));
	printf(" Dir: %s Fport: %d Lport: %d Counter: %d CntrAdd: %s;  wqe @ %lx\n",
		 dmadirstring[dmahdr->s.dir], dmahdr->s.fport, dmahdr->s.lport, 
		 dmahdr->s.c, (dmahdr->s.ca)?"Yes":"No", (uint64_t)dmahdr->s.ptr);
	printf(" PtrCount -> remote: %d local: %d (fi: %d ii: %d fl: %d)\n",
		dmahdr->s.nr, dmahdr->s.nl, dmahdr->s.fi, dmahdr->s.ii, dmahdr->s.fl);
}



/** Print the contents of the DMA chunk buffer. */
void
cvm_drv_print_dma_chunk_words(cvm_oct_dma_chunk_t *chunk)
{
	printf("Printing %d dma chunk words in buf @%p\n", chunk->current_word,
	       chunk->buf);
	cvm_drv_print_data(chunk->buf, (chunk->current_word << 3));
}




void
cvm_drv_print_dma_chunk(cvm_oct_dma_chunk_t *chunk)
{
  printf("\n Printing DMA chunk\n");
  printf("buf          : %p\n", chunk->buf);
  printf("chunk_size   : %d\n", chunk->chunk_size);
  printf("current_word : %d\n", chunk->current_word);
  printf("max_words    : %d\n", chunk->max_words);
  printf("pool         : %d\n", chunk->pool);
  printf("chunk_buf[%d]: 0x%016llx\n", chunk->max_words,
         cast64(chunk->buf[chunk->max_words]));
  printf("\n");
}
 


void
cvm_drv_debug_print_dmaq(int q_no)
{
	if (q_no >= oct->max_dma_qs) {
		printf("%s Invalid queue number (%d)\n", __FUNCTION__, q_no);
		return;
	}
	
	cvm_drv_print_dma_chunk(oct->chunk[q_no]);
	cvm_drv_print_dma_chunk_words(oct->chunk[q_no]);
}





#ifdef CVM_PCI_TRACK_DMA

static void
cvm_pci_dma_init_tracker_list(void)
{
	int   rsvd;

	memset(&dmatracker, 0, sizeof(struct __dma_tracker));
	CAVIUM_INIT_LIST_HEAD(&dmatracker.head);
	cvmx_spinlock_init(&dmatracker.lock);
	dmatracker.pool     = CVMX_FPA_PACKET_POOL;

	rsvd = (offsetof(cvmx_wqe_t, packet_data)
	      + offsetof(cvm_pci_dma_tracker_t, dmawords));
	dmatracker.maxwords = ((CVMX_FPA_PACKET_POOL_SIZE - rsvd) >> 3);
}




static void
cvm_pci_dma_postprocess_tracker_pkt(cvmx_oct_pci_dma_inst_hdr_t   *hdr)
{
	if(hdr->s.ptr == 0)
		return;

	if(hdr->s.wqp) {
		cvmx_wqe_t *wqe = (cvmx_wqe_t *)cvmx_phys_to_ptr(hdr->s.ptr);
		cvmx_pow_work_submit(wqe, wqe->tag, wqe->tag_type, wqe->qos, wqe->grp);
	} else {
		*((uint8_t *)cvmx_phys_to_ptr(hdr->s.ptr)) = 0;
		CVMX_SYNCWS;
	}
}



static void
cvm_pci_dma_tracker_update_stats(cvmx_oct_pci_dma_inst_hdr_t *hdr)
{
	dmatracker.stats.dir[hdr->s.dir]++;
	dmatracker.stats.total++;
	if(hdr->s.nr > dmatracker.stats.max_rptrs)
		dmatracker.stats.max_rptrs = hdr->s.nr;
	if(hdr->s.nl > dmatracker.stats.max_lptrs)
		dmatracker.stats.max_lptrs = hdr->s.nl;
	if( (hdr->s.nr + hdr->s.nl) > dmatracker.stats.max_sumptrs)
		dmatracker.stats.max_sumptrs = (hdr->s.nr + hdr->s.nl);
}




void
cvm_pci_dma_remove_from_tracker_list(cvmx_wqe_t  *wqe)
{
	cvm_pci_dma_tracker_t  *dmat = (cvm_pci_dma_tracker_t *)wqe->packet_data;
	cvmx_oct_pci_dma_inst_hdr_t   *hdr;
	static int count=10;

	if(count > 0) {
		printf("Count: %d Not freeing this entry\n", count);
		count --;
		return;
	}

	cvmx_spinlock_lock(&dmatracker.lock);
	cavium_list_del(&dmat->list);
	hdr = (cvmx_oct_pci_dma_inst_hdr_t *)dmat->dmawords;
	cvm_pci_dma_tracker_update_stats(hdr);
	cvm_pci_dma_postprocess_tracker_pkt(hdr);
	cvmx_spinlock_unlock(&dmatracker.lock);

	cvmx_fpa_free(wqe, dmatracker.pool, 0);
}





static void
cvm_pci_dma_add_to_tracker_list(uint64_t   *dma_words, int words_reqrd)
{
	cvmx_wqe_t                    *wqe;
	cvm_pci_dma_tracker_t         *dmat;
	cvmx_oct_pci_dma_inst_hdr_t   *hdr;

	wqe = cvmx_fpa_alloc(dmatracker.pool);
	if(cvmx_unlikely(wqe == NULL)) {
		printf("%s wqe alloc failed\n", __FUNCTION__);
		return;
	}

	memset(wqe, 0, sizeof(cvmx_wqe_t));
	wqe->word2.s.software = 1;
	wqe->ipprt            = CVM_PCI_DMA_TRACKER_PORT;
	wqe->tag              = CVM_PCI_DMA_TRACKER_TAG;
	wqe->tag_type         = CVMX_POW_TAG_TYPE_ORDERED;

	dmat = (cvm_pci_dma_tracker_t *)wqe->packet_data;
	CAVIUM_INIT_LIST_HEAD(&dmat->list);
	dmat->cnt = (words_reqrd > dmatracker.maxwords)?dmatracker.maxwords:words_reqrd;
	dmat->allbufs = (dmat->cnt == words_reqrd);
	memcpy(dmat->dmawords, dma_words, (dmat->cnt * 8));

	hdr = (cvmx_oct_pci_dma_inst_hdr_t *)dma_words;
	hdr->s.wqp = 1;
	hdr->s.ptr = cvmx_ptr_to_phys(wqe);

	cvmx_spinlock_lock(&dmatracker.lock);
	dmat->pktnum = ++dmatracker.pktnum;
	cavium_list_add_tail(&dmat->list, &dmatracker.head);
	cvmx_spinlock_unlock(&dmatracker.lock);

	CVMX_SYNCWS;
}




void
cvm_pci_dma_dump_tracker_list(void)
{
	cvm_pci_dma_tracker_t  *dmat;
	cavium_list_t          *tmp, dhead;
	int                     count = 0;

	CAVIUM_INIT_LIST_HEAD(&dhead);

	cvmx_spinlock_lock(&dmatracker.lock);
	printf("DMA Tracker current pkt num: %u\n", dmatracker.pktnum);
	printf("dmatracker head @ %p dhead @ %p\n", &dmatracker.head, &dhead);
	cavium_list_move(&dhead, &dmatracker.head);
	cvmx_spinlock_unlock(&dmatracker.lock);

	cavium_list_for_each(tmp, &dhead) {
		int                            i;
		cvmx_wqe_t                    *wqe;
		cvmx_oct_pci_dma_inst_hdr_t   *hdr;

		cavium_list_del(tmp);
		dmat = (cvm_pci_dma_tracker_t  *)tmp;
		hdr = (cvmx_oct_pci_dma_inst_hdr_t *)dmat->dmawords;

		printf("\n DMA Packet #%d (%s bufs recorded)\n",
		       dmat->pktnum, (dmat->allbufs)?"All":"Not all");

		cvm_pci_dma_print_header(hdr);
		for(i = 1; i < dmat->cnt; i++)
			printf("0x%016lx\n", dmat->dmawords[i]);
		printf("\n");

		cvm_pci_dma_tracker_update_stats(hdr);
		cvm_pci_dma_postprocess_tracker_pkt(hdr);
		wqe = getstructptr(dmat, cvmx_wqe_t, packet_data);
		cvmx_fpa_free(wqe, dmatracker.pool, 0);
		count++;
	}

	printf("DMA Tracker Summary: Total %llu DMA tracked; %d pending DMA found\n",
	       cast64(dmatracker.stats.total), count);
	if(dmatracker.stats.dir[PCI_DMA_OUTBOUND])
		printf("Outbound DMA: %llu;  ", cast64(dmatracker.stats.dir[PCI_DMA_OUTBOUND]));
	if(dmatracker.stats.dir[PCI_DMA_INBOUND])
		printf("Inbound DMA: %llu;  ", cast64(dmatracker.stats.dir[PCI_DMA_INBOUND]));
	if(dmatracker.stats.dir[PCI_DMA_EXTERNAL])
		printf("External DMA: %llu;  ", cast64(dmatracker.stats.dir[PCI_DMA_EXTERNAL]));
	if(dmatracker.stats.dir[PCI_DMA_INTERNAL])
		printf("Internal DMA: %llu;  ", cast64(dmatracker.stats.dir[PCI_DMA_INTERNAL]));
	printf("\nMax LptrCnt: %llu RptrCnt: %llu SumPtrs: %llu\n",
	      cast64(dmatracker.stats.max_lptrs), cast64(dmatracker.stats.max_rptrs),
	      cast64(dmatracker.stats.max_sumptrs));
}

#endif




/** Async allocation routine for DMA queue instruction chunk.
  */
static inline  uint64_t 
cvm_drv_dma_chunk_async_alloc()
{
    uint64_t   buf;

    buf = CVM_DRV_GET_PHYS(cvmx_fpa_alloc(CVM_FPA_DMA_CHUNK_POOL));
    return buf;
}





static inline  uint32_t
___get_remote_word_count(int pcnt)
{
	return ( ((pcnt >> 2 ) * 5) + ((pcnt & 0x03)?((pcnt & 0x03) + 1):0) );
}


	

/**  Get a count of number of 64-bits required in the chunk for a DMA operation.
 *   This routine calculates the number of PCI DMA instruction chunk words
 *   required to create the instruction for the number of local and remote
 *   pointers given in the instrucion header.
 */
static inline   uint32_t
get_word_count(cvmx_oct_pci_dma_inst_hdr_t   *dma_hdr)
{
	uint32_t    words = 1;

	if(cvmx_likely(dma_hdr->s.dir != PCI_DMA_EXTERNAL))
		words += dma_hdr->s.nl;
	else
		words += ___get_remote_word_count(dma_hdr->s.nl);

	if(cvmx_likely(dma_hdr->s.dir != PCI_DMA_INTERNAL))
		words += ___get_remote_word_count(dma_hdr->s.nr);
	else
		words += dma_hdr->s.nr;

	return (words);
}







static inline  int
cvm_pci_dma_copy_local_ptrs(int                             nl,
                            uint32_t                       *localsize,
                            uint64_t                       *cmd,
                            cvmx_oct_pci_dma_local_ptr_t   *lptr)
{
	int j;

	*localsize = 0;

	/* Copy the local pointers. */
	for(j = 0; j < nl; j++) {
		cmd[j]      = lptr[j].u64;
		*localsize += lptr[j].s.size;
		DBG_PRINT(DBG_FLOW,"lptr[%d]: 0x%016llx\n",j, cast64(lptr[j].u64));
	}

	return j;
}






static inline  int
cvm_pci_dma_copy_remote_ptrs(int                    nr,
                             uint32_t              *remsize,
                             uint64_t              *cmd,
                             cvm_dma_remote_ptr_t  *rptr)
{
	int                    i = 0, j, k;
	cvm_dma_remote_len_t  *pci_len;

	*remsize = 0;

	/* Create the PCI components with length and addresses. */
	for(j = 0; j < nr; j+=4)  {
		pci_len      = (cvm_dma_remote_len_t *)&cmd[i++];
		pci_len->u64 = 0;
		for(k = 0; k < (((nr - j) >= 4)?4:(nr - j)); k++) {
			pci_len->l.len[k]  = rptr[j+k].s.size;
			*remsize          += rptr[j+k].s.size;
			cmd[i++]           = cast64(rptr[j+k].s.addr);
			DBG_PRINT(DBG_FLOW,"rptr[%d]: addr: 0x%llx size: %u\n", j+k,
			          CAST64(rptr[j+k].s.addr), rptr[j+k].s.size);
		}
	}
	return i;
}


/* ENable this flag if only the DMA engines in CN63XX are used and the 
   DPI packet Input/Output will not be used.
   This will increase the FIFO space for the DMA engines and map more
   instruction queues to each engine.
*/
//#define CN6XXX_NO_PKT_ENGINE

static void
cvm_enable_cn63xx_dma(cvm_oct_dev_t  *oct)
{
	cvmx_dpi_dma_control_t   dma_ctl;
	int                      i;
#ifdef CN6XXX_NO_PKT_ENGINE
	int                      fifo_sizes[6]  = {2, 2, 2, 2, 2, 2};
	int                      engine_qmap[6] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20};
#else
	/* Fifo sizes are determined by the host driver when packet engines are
	   used. */
	int                      engine_qmap[6] = {0x1, 0x2, 0x4, 0x8, 0x10, 0};
#endif

#ifdef CN6XXX_NO_PKT_ENGINE
	for(i = 0; i < 6; i++) {
		cvmx_write_csr(CVMX_DPI_ENGX_BUF(i), fifo_sizes[i]);
	}
	dma_ctl.u64           = 0;
	dma_ctl.s.o_mode      = 1;
	dma_ctl.s.o_es        = 1;
	dma_ctl.s.dma_enb     = 0x3F;
#else
	dma_ctl.u64           = cvmx_read_csr(CVMX_DPI_DMA_CONTROL);
	dma_ctl.s.dma_enb     = 0x1F;
#endif

	dma_ctl.s.o_add1      = 1;
	dma_ctl.s.fpa_que     = CVM_FPA_DMA_CHUNK_POOL;
	dma_ctl.s.dwb_ichk    = 1;
	dma_ctl.s.dwb_denb    = 1;
	dma_ctl.s.commit_mode = 1;

	cvmx_write_csr(CVMX_DPI_DMA_CONTROL, dma_ctl.u64);

	for(i = 0; i < 6; i++) {
		cvmx_write_csr(CVMX_DPI_DMA_ENGX_EN(i), engine_qmap[i]);
	}

	cvmx_write_csr(CVMX_DPI_CTL, 1);

	printf("DPI_DMA_CONTROL: 0x%016lx\n", cvmx_read_csr(CVMX_DPI_DMA_CONTROL));

	for(i = 0; i < 6; i++) {
		printf("DPI_ENGX_BUF(%d): 0x%016lx\n", i, cvmx_read_csr(CVMX_DPI_ENGX_BUF(i)));
	}

}


static void
cvm_enable_cn66xx_dma(cvm_oct_dev_t  *oct)
{
	cvmx_dpi_dma_control_t   dma_ctl;
	int                      i;

	int                      fifo_sizes[6]  = {4, 4, 2, 1, 1, 4};
	int                      engine_qmap[6] = {0x21, 0x42, 0x84, 0x8, 0x10, 0};
	//int                      engine_qmap[6] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x0};

	for(i = 0; i < 6; i++) {
		cvmx_write_csr(CVMX_DPI_ENGX_BUF(i), fifo_sizes[i]);
	}

	dma_ctl.u64           = 0;
	dma_ctl.s.o_mode      = 1;
	dma_ctl.s.o_es        = 1;
	dma_ctl.s.dma_enb     = 0x1F; 
	dma_ctl.s.o_add1      = 1;
	dma_ctl.s.fpa_que     = CVM_FPA_DMA_CHUNK_POOL;
	dma_ctl.s.dwb_ichk    = 1;
	dma_ctl.s.dwb_denb    = 1;
	dma_ctl.s.commit_mode = 1;

	dma_ctl.s.pkt_en      = 1;

	/* CHK */
	//dma_ctl.s.pkt_en1     = 1;
	//dma_ctl.s.ffp_dis     = 1;
	
	cvmx_write_csr(CVMX_DPI_DMA_CONTROL, dma_ctl.u64);

	for(i = 0; i < 6; i++) {
		cvmx_write_csr(CVMX_DPI_DMA_ENGX_EN(i), engine_qmap[i]);
	}

	cvmx_write_csr(CVMX_DPI_CTL, 1);

	printf("DPI_DMA_CONTROL: 0x%016lx\n", cvmx_read_csr(CVMX_DPI_DMA_CONTROL));

	for(i = 0; i < 6; i++) {
		printf("DPI_ENGX_BUF(%d): 0x%016lx\n", i, cvmx_read_csr(CVMX_DPI_ENGX_BUF(i)));
	}

}




static void
cvm_enable_cn68xx_dma(cvm_oct_dev_t  *oct)
{
	cvmx_dpi_dma_control_t   dma_ctl;
	int                      i;
#ifdef CN6XXX_NO_PKT_ENGINE
	int                      fifo_sizes[6]  = {2, 2, 2, 2, 2, 2};
	int                      engine_qmap[6] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20};
#else
	/* Fifo sizes are determined by the host driver when packet engines are
	   used. */
//	int                      engine_qmap[6] = {0x1, 0x2, 0x4, 0x8, 0x10, 0};
	int                      engine_qmap[6] = {0x1, 0x2, 0x4, 0x8, 0x0, 0};
#endif

#ifdef CN6XXX_NO_PKT_ENGINE
	for(i = 0; i < 6; i++) {
		cvmx_write_csr(CVMX_DPI_ENGX_BUF(i), fifo_sizes[i]);
	}
	dma_ctl.u64           = 0;
	dma_ctl.s.o_mode      = 1;
	dma_ctl.s.o_es        = 1;
	dma_ctl.s.dma_enb     = 0x3F;
#else
	dma_ctl.u64           = cvmx_read_csr(CVMX_DPI_DMA_CONTROL);
	dma_ctl.s.dma_enb     = 0x0F;
#endif

	dma_ctl.s.o_add1      = 1;
	dma_ctl.s.fpa_que     = CVM_FPA_DMA_CHUNK_POOL;
	dma_ctl.s.dwb_ichk    = 1;
	dma_ctl.s.dwb_denb    = 1;
	dma_ctl.s.commit_mode = 1;

	dma_ctl.s.pkt_en1     = 1;
	dma_ctl.s.ffp_dis     = 1;
	
	cvmx_write_csr(CVMX_DPI_DMA_CONTROL, dma_ctl.u64);

	for(i = 0; i < 6; i++) {
		cvmx_write_csr(CVMX_DPI_DMA_ENGX_EN(i), engine_qmap[i]);
	}

	cvmx_write_csr(CVMX_DPI_CTL, 1);

	printf("DPI_DMA_CONTROL: 0x%016lx\n", cvmx_read_csr(CVMX_DPI_DMA_CONTROL));

	for(i = 0; i < 6; i++) {
		printf("DPI_ENGX_BUF(%d): 0x%016lx\n", i, cvmx_read_csr(CVMX_DPI_ENGX_BUF(i)));
	}

}





static void
cvm_enable_cn56xx_dma(uint32_t  chunk_size)
{
	cvmx_npei_dma_control_t  dma_ctl;

	/* Set instruction chunk size and free pool */
	dma_ctl.u64        = 0;
	dma_ctl.s.fpa_que  = CVM_FPA_DMA_CHUNK_POOL;
	dma_ctl.s.csize    = chunk_size;
	dma_ctl.s.o_add1   = 1;
	dma_ctl.s.o_es     = 1;
	dma_ctl.s.o_mode   = 1;
	dma_ctl.s.dwb_denb = 1;
	dma_ctl.s.dma0_enb = 1;
	dma_ctl.s.dma1_enb = 1;
	dma_ctl.s.dma2_enb = 1;
	dma_ctl.s.dma3_enb = 1;
	dma_ctl.s.dma4_enb = 1;
	cvmx_write_csr(CVMX_PEXP_NPEI_DMA_CONTROL, dma_ctl.u64);
	CVMX_SYNCW;
}



static void
cvm_enable_cn3xx_dma(uint32_t  chunk_size)
{
	cvmx_npi_dma_control_t   dma_ctl;

	/* Set instruction chunk size and free pool */
	dma_ctl.u64        = 0;
	dma_ctl.s.fpa_que  = CVM_FPA_DMA_CHUNK_POOL;
	dma_ctl.s.csize    = chunk_size;
	dma_ctl.s.o_add1   = 1;
	dma_ctl.s.o_es     = 1;
	dma_ctl.s.o_mode   = 1;
	dma_ctl.s.dwb_denb = 1;
	dma_ctl.s.hp_enb   = 1;
	dma_ctl.s.lp_enb   = 1;
	cvmx_write_csr(CVMX_NPI_DMA_CONTROL, dma_ctl.u64);
	CVMX_SYNCW;
}



void
cvm_enable_dma(cvm_oct_dev_t  *oct, int chunk_size)
{
	if(OCTEON_IS_MODEL(OCTEON_CN68XX))
		cvm_enable_cn68xx_dma(oct);

	if( OCTEON_IS_MODEL(OCTEON_CN66XX)) /* TBD */
		cvm_enable_cn66xx_dma(oct);

	if( OCTEON_IS_MODEL(OCTEON_CN63XX) )
		cvm_enable_cn63xx_dma(oct);

	if(OCTEON_IS_MODEL(OCTEON_CN56XX))
		cvm_enable_cn56xx_dma(chunk_size);

	if(OCTEON_IS_MODEL(OCTEON_CN58XX) || OCTEON_IS_MODEL(OCTEON_CN38XX))
		cvm_enable_cn3xx_dma(chunk_size);
}





void
cvm_free_dma_queue(cvm_oct_dev_t  *oct)
{
	int i;

	for( i = 0; i < oct->max_dma_qs; i++) {
		if(oct->chunk[i]) {
			if(oct->chunk[i]->buf)
				cvmx_fpa_free(oct->chunk[i]->buf, CVM_FPA_DMA_CHUNK_POOL, 0);
			cvmx_fpa_free(oct->chunk[i], CVMX_FPA_WQE_POOL, 0);
		}
	}
}




cvm_oct_dma_chunk_t  *
cvm_dma_alloc_chunk_space(int chunk_size)
{
	cvm_oct_dma_chunk_t   *c  = NULL;
	uint64_t   *first_buf = NULL, *second_buf = NULL, *third_buf = NULL;

	c          = cvmx_fpa_alloc(CVMX_FPA_WQE_POOL);
	first_buf  = (uint64_t *)cvmx_fpa_alloc(CVM_FPA_DMA_CHUNK_POOL);
	second_buf = (uint64_t *)cvmx_fpa_alloc(CVM_FPA_DMA_CHUNK_POOL);
	third_buf  = (uint64_t *)cvmx_fpa_alloc(CVM_FPA_DMA_CHUNK_POOL);

	if( (c == NULL) || (first_buf == NULL) || (second_buf == NULL) ||
	    (third_buf == NULL) ) {
		goto dma_chunk_alloc_failed;
	}

	memset(c, 0, sizeof(cvm_oct_dma_chunk_t));

	c->buf               = first_buf;
	c->chunk_size        = chunk_size;
	c->current_word      = 0;
	c->pool              = CVM_FPA_DMA_CHUNK_POOL;
	c->max_words         = chunk_size - 1;

	c->buf[c->max_words]      = CVM_DRV_GET_PHYS(second_buf);
	second_buf[c->max_words]  = 0;
	c->extra_buf              = CVM_DRV_GET_PHYS(third_buf);

#ifdef DMA_CHUNK_LOCK_USE_TAG
	c->tag = CVM_PCI_DMA_CHUNK_TAG(i);
#else
	cvmx_spinlock_init(&c->lock);
#endif
	CVMX_SYNCW;

	return c;


dma_chunk_alloc_failed:
	if(third_buf)  cvmx_fpa_free(third_buf, CVM_FPA_DMA_CHUNK_POOL, 0);
	if(second_buf) cvmx_fpa_free(second_buf, CVM_FPA_DMA_CHUNK_POOL, 0);
	if(first_buf)  cvmx_fpa_free(first_buf, CVM_FPA_DMA_CHUNK_POOL, 0);
	if(c)          cvmx_fpa_free(c, CVMX_FPA_WQE_POOL, 0);
	return NULL;
}







static int
cvm_dma_chip_specific_setup(cvm_oct_dev_t *oct, int q_no)
{
	cvm_oct_dma_chunk_t   *c = oct->chunk[q_no];


	oct->max_lptrs     = OCTEON_MAX_DMA_LOCAL_POINTERS;
	oct->max_rptrs     = OCTEON_MAX_DMA_REMOTE_POINTERS;
	oct->max_dma_ptrs  = OCTEON_MAX_DMA_POINTERS;

	if(OCTEON_IS_MODEL(OCTEON_CN63XX) || OCTEON_IS_MODEL(OCTEON_CN66XX) ||
	   OCTEON_IS_MODEL(OCTEON_CN68XX) ) {
		c->doorbell_reg    = CVMX_DPI_DMAX_DBELL(q_no);
		if(cvmx_read_csr(CVMX_DPI_DMAX_IBUFF_SADDR(q_no)) & (1ULL << 40)) {
			cvmx_write_csr(CVMX_DPI_DMAX_IBUFF_SADDR(q_no),
			  (((unsigned long long)c->chunk_size << 48) | CVM_DRV_GET_PHYS(c->buf)) );
			return 0;
		} else {
			printf("%s DMA Engine %d is not in idle state\n", __FUNCTION__, q_no);
			return 1;
		}
	}

	if(OCTEON_IS_MODEL(OCTEON_CN56XX)) {
		c->doorbell_reg    = CVMX_PEXP_NPEI_DMAX_DBELL(q_no);
		if(cvmx_read_csr(CVMX_PEXP_NPEI_DMAX_IBUFF_SADDR(q_no)) & (1ULL << 36)) {
			cvmx_write_csr(CVMX_PEXP_NPEI_DMAX_IBUFF_SADDR(q_no), CVM_DRV_GET_PHYS(c->buf));
			return 0;
		} else {
			printf("%s DMA Engine %d is not in idle state\n", __FUNCTION__, q_no);
			return 1;
		}
	}

	if(OCTEON_IS_MODEL(OCTEON_CN58XX) || OCTEON_IS_MODEL(OCTEON_CN38XX)){
		c->doorbell_reg    = CVM_PCI_DMA_DOORBELL_REG(q_no);
		cvmx_write_csr(CVM_PCI_DMA_START_ADDR_REG(q_no), CVM_DRV_GET_PHYS(c->buf));
		return 0;
	}

	return 1;
}







int
cvm_dma_queue_init(cvm_oct_dev_t    *oct)
{
	int                           i, chunk_size;

	chunk_size = (cvmx_fpa_get_block_size(CVM_FPA_DMA_CHUNK_POOL)/8);

	for( i = 0; i < oct->max_dma_qs; i++) {

		oct->chunk[i] = cvm_dma_alloc_chunk_space(chunk_size);
		if(oct->chunk[i] == NULL) {
			printf("[ DRV ] Chunk alloc failed for DMA Queue[%d]\n", i);
			cvm_free_dma_queue(oct);
			return -ENOMEM;
		}
		printf("[ DRV ] DMA Q[%d]: Chunk @ %p size: %d max_words: %d\n", i,
	       oct->chunk[i], oct->chunk[i]->chunk_size, oct->chunk[i]->max_words);

		if(cvm_dma_chip_specific_setup(oct, i))
			return -EINVAL;
	}

	/* Allocate and initialize the completion words free pool */
	oct->comp_ptr = cvmx_bootmem_alloc_named(CVM_DMA_COMP_POOL_SIZE + sizeof(cvm_oct_comp_ptr_pool_t), CVMX_CACHE_LINE_SIZE, "comp_ptr_pool");
	if(oct->comp_ptr == NULL)
		return -ENOMEM;
	memset(oct->comp_ptr, 0, CVM_DMA_COMP_POOL_SIZE + sizeof(cvm_oct_comp_ptr_pool_t));

	oct->comp_ptr->list = (cvm_dma_comp_ptr_t *)((uint8_t *)oct->comp_ptr + sizeof(cvm_oct_comp_ptr_pool_t));
	cvmx_spinlock_init(&(oct->comp_ptr->lock));

#ifdef CVM_PCI_TRACK_DMA
	cvm_pci_dma_init_tracker_list();
#endif

	/* Now that all memory allocations are done, enable the PCI DMA engines. */
	cvm_enable_dma(oct, chunk_size);


	return 0;
}







/*-------  START Completion words pool functions --------------*/

/* Function to get the next free entry from the completion words free pool.
   The function is a blocking call in the sense that it does not return till
   it finds a free entry in the pool.
*/
cvm_dma_comp_ptr_t *
cvm_get_dma_comp_ptr(void)
{
	cvm_oct_comp_ptr_pool_t  *comp = oct->comp_ptr;
	cvm_dma_comp_ptr_t       *next_ptr=NULL;
	uint32_t                 i, retries=0;

#define MAX_COMP_PTR_RETRIES  100

	do {
		cvmx_spinlock_lock(&comp->lock);

		/* Start from the next location where a free entry is expected. Go on
		   till the end of the pool. If no entries were found, reset the index
		   and start from the beginning of free pool after a small wait time.
	 	   If memory is low, some entries would hopefully be freed in the
		   wait time.
		*/
		for(i = comp->idx; i < CVM_DMA_COMP_PTR_COUNT; i++) {

			if(!comp->list[i].in_use) {

				next_ptr = &comp->list[i];
				comp->list[i].in_use = 1;
				comp->idx = i;
				cvmx_spinlock_unlock(&comp->lock);
				return next_ptr;
			}
		}
		comp->idx = 0;
		cvmx_spinlock_unlock(&comp->lock);
		cvmx_wait(10);
	} while(++retries < MAX_COMP_PTR_RETRIES);
	
	return NULL;
}



/* Return a completion word entry into the free pool. The pointer passed is
   checked to ensure it is a completion word free pool entry.
*/
void
cvm_release_dma_comp_ptr(cvm_dma_comp_ptr_t *ptr)
{
	if( ((unsigned long)ptr >= (unsigned long)oct->comp_ptr->list)
        && ((unsigned long)ptr < ((unsigned long)oct->comp_ptr->list + CVM_DMA_COMP_POOL_SIZE))) {
		cvmx_spinlock_lock(&oct->comp_ptr->lock);
		ptr->in_use = 0;
		cvmx_spinlock_unlock(&oct->comp_ptr->lock);
	} else {
		printf("Invalid DMA_COMP_PTR %p returned\n", ptr);
	}
}

/*-------  END completion words pool functions ------------*/




/*-------  START DMA Chunk manipulation functions --------------*/

static inline int
get_dma_chunk_space(cvm_oct_dma_chunk_t  *chunk,  uint32_t words)
{
	uint64_t    *buf, *lastbuf = NULL;
	uint32_t     current_space = (chunk->max_words - chunk->current_word);

	DBG_PRINT(DBG_FLOW,"---get_dma_chunk_space, cur-space: %d words: %d---\n", current_space, words);
	if (current_space > words)
		return (current_space);

	lastbuf = (uint64_t *)cvmx_phys_to_ptr(chunk->buf[chunk->max_words]);
	if(lastbuf) {
		if (chunk->extra_buf) {
			buf = (uint64_t *)cvmx_phys_to_ptr(chunk->extra_buf);
			lastbuf[chunk->max_words] = chunk->extra_buf;
			buf[chunk->max_words] = 0;
			chunk->extra_buf = cvm_drv_dma_chunk_async_alloc();
		} else {
			/* The previous async alloc has not yet delivered a buf */
			/* But we need a chunk right away */
			buf = (uint64_t *)cvmx_fpa_alloc(chunk->pool);
			if(buf) {
				lastbuf[chunk->max_words] = CVM_DRV_GET_PHYS(buf);
				buf[chunk->max_words] = 0;
				chunk->extra_buf = cvm_drv_dma_chunk_async_alloc();
			} else {
				printf("[ DRV ] CVM_PCI_DMA:Cannot allocate chunk from pool\n");
				return 0;
			}
		}
	}  else { /*lastbuf = NULL */
		printf("[ DRV ] CVM_PCI_DMA: Warning! Next Chunk Pointer was empty\n");
		chunk->buf[chunk->max_words] = cvm_drv_dma_chunk_async_alloc();
		if(chunk->buf[chunk->max_words] == 0) {
			printf("[ DRV ] CVM_PCI_DMA: Warning! Retry failed! Next Chunk Pointer is NULL\n");
			/* We are not able to allocate memory. */
			return 0;
		}
	}  
	CVMX_SYNCW;
	DBG_PRINT(DBG_FLOW,"&chunk_buf: %p chunk_buf[%d]: 0x%016llx  extra_buf: 0x%016llx\n", chunk->buf, chunk->max_words, cast64(chunk->buf[chunk->max_words]), cast64(chunk->extra_buf));
	return current_space;
}






static inline uint64_t *
cvm_update_dma_chunk_ptr(cvm_oct_dma_chunk_t   *chunk,
                         uint32_t           consumed)
{
	if((chunk->max_words - chunk->current_word) > consumed) {
		chunk->current_word += consumed;
	} else {
		if((chunk->max_words - chunk->current_word) < consumed) {
			printf(" [ DRV ] ERROR!! %d words were consumed in DMA chunk with %d words\n", consumed, (chunk->max_words - chunk->current_word));
			return NULL;
		}
		/* Else all the available words were consumed; Move to the next chunk.*/
		chunk->buf = (uint64_t *)cvmx_phys_to_ptr(chunk->buf[chunk->max_words]);
		chunk->current_word = 0;
	}
	CVMX_SYNCW;
	return &(chunk->buf[chunk->current_word]);
}


/*-------  END   DMA Chunk manipulation functions --------------*/





#if  defined(CVM_PCI_VALIDATE_DMA_POINTERS)

/* Checks:
	   Buffers should be freed only for OUTBOUND DMA.
	   The address cannot be NULL.
	   The PCI DMA Local address can only be 36-bits long.
	   Data in Little-Endian form is not supported.
	   Data Size cannot be zero-bytes
*/
static int
cvm_pci_dma_validate_localptr(int dir, cvmx_oct_pci_dma_local_ptr_t  *lptr)
{
	int errornum = 0;

	if(dir != PCI_DMA_OUTBOUND && lptr->s.i)  {
		errornum = 1; goto  localptr_validate_error;
	}

	if(lptr->s.addr == 0) {
		errornum = 2; goto  localptr_validate_error;
	}

	if(lptr->s.addr & 0xf000000000ULL) {
		errornum = 3; goto  localptr_validate_error;
	}

	if(lptr->s.l) {
		errornum = 4; goto  localptr_validate_error;
	}

	if(lptr->s.size == 0) {
		errornum = 5; goto  localptr_validate_error;
	}
	return 0;

localptr_validate_error:
	printf("[ DRV ] DMA: Invalid local address: 0x%016llx ", cast64(lptr->u64));
	switch(errornum) {
		case 1: printf(": I-bit set for non-OUTBOUND traffic\n"); break;
		case 2: printf(": NULL Address\n"); break;
		case 3: printf(": Reserved field set\n"); break;
		case 4: printf(": Little Endian mode\n"); break;
		case 5: printf(": Zero-byte size field\n"); break;
	}
	return errornum;
}





/* Checks:
	   The address cannot be NULL.
	   The PCI DMA Local address can only be 36-bits long.
	   Data Size cannot be zero-bytes
*/
static int
cvm_pci_dma_validate_remoteptr(cvm_dma_remote_ptr_t *rptr)
{
	int errornum = 0;

	if(rptr->s.addr == 0) {
		errornum = 1; goto  remoteptr_validate_error;
	}

	if(rptr->s.size == 0) {
		errornum = 2; goto  remoteptr_validate_error;
	}
	return 0;

remoteptr_validate_error:
	printf("[ DRV ] DMA: Invalid Remote address: 0x%016llx or size: %d",
	       cast64(rptr->s.addr), rptr->s.size);
	switch(errornum) {
		case 1: printf(": NULL Address\n"); break;
		case 2: printf(": Zero-byte size field\n"); break;
	}
	return errornum;
}




static int
cvm_pci_dma_validate_ptrs(cvmx_oct_pci_dma_inst_hdr_t      *dma_hdr,
                          cvmx_oct_pci_dma_local_ptr_t     *lptr,
                          cvm_dma_remote_ptr_t             *rptr)
{
	int j;

	for(j = 0; j < dma_hdr->s.nl; j++)  {
		dbg_printf("lptr[%d]: 0x%016lx\n", j, lptr[j].u64);
		if(cvm_pci_dma_validate_localptr(dma_hdr->s.dir, &lptr[j]))
			goto dma_ptr_check_failed;
#ifdef SANITY_CHECKS
		pci_output_sanity_and_buffer_count_update((cvmx_buf_ptr_t)lptr[j].u64, CVM_DIRECT_DATA, 1, lptr[j].s.size, 32);
#endif
	}


	for(j = 0; j < dma_hdr->s.nr; j++) {
		dbg_printf("rptr[%d]: 0x%016lx\n", j, rptr[j].u64);
		if(cvm_pci_dma_validate_remoteptr(&rptr[j]))
			goto dma_ptr_check_failed;
	}
	return 0;

dma_ptr_check_failed:
	for(j = 0; j < dma_hdr->s.nl; j++)
		printf("lptr[%d]: 0x%016lx\n", j, lptr[j].u64);
	for(j = 0; j < dma_hdr->s.nr; j++)
		printf("rptr[%d]: 0x%016lx\n", j, rptr[j].u64);
	return -EINVAL;
}

#endif






int
cvm_pci_dma_chunk_write(cvm_oct_dma_chunk_t           *chunk,
                        CVMX_DMA_QUEUE_TYPE            q_no,
                        uint64_t                      *cmd,
                        uint32_t                       cnt)
{

	uint32_t    chunk_space;
	int         retval = 0;
#ifdef DMA_CHUNK_LOCK_USE_TAG
	cvmx_pow_tag_req_t   prev_tag;
#endif


#ifdef DMA_CHUNK_LOCK_USE_TAG
	prev_tag = cvmx_pow_get_current_tag();
	cvmx_pow_tag_sw_full((cvmx_wqe_t *)cvmx_phys_to_ptr(0x80), chunk->tag, CVMX_POW_TAG_TYPE_ATOMIC, 0);
	cvmx_pow_tag_sw_wait();
#else
	cvmx_spinlock_lock(&(chunk->lock));
#endif


	/* Check if the chunk has enough 64-bits words for this op. */
	chunk_space = get_dma_chunk_space(chunk, cnt);

	if(chunk_space) {

		if(chunk_space >= cnt) {

			memcpy(&(chunk->buf[chunk->current_word]), cmd, (cnt << 3));
			cvm_update_dma_chunk_ptr(chunk, cnt);

		}  else {

			memcpy(&(chunk->buf[chunk->current_word]), cmd,(chunk_space << 3));
			memcpy(cvm_update_dma_chunk_ptr(chunk, chunk_space),
			       &(cmd[chunk_space]), ((cnt - chunk_space) << 3) );
			cvm_update_dma_chunk_ptr(chunk, (cnt - chunk_space));
		}
		CVMX_SYNCWS;


		DBG_PRINT(DBG_FLOW,"DMA Q[%d] doorbell hit with %d words\n", q_no, cnt);
		cvmx_write_csr(chunk->doorbell_reg, cnt);
#ifdef PROFILE_PCI_DMA
		doorbell_hit_time[cvm_drv_core_id] = cvmx_get_cycle();
#endif
	} else {
		printf("[ DRV ] CVM_PCI_DMA: PCI Write Fail. No DMA chunk\n");
		retval = -ENOMEM;
	}

#ifdef DMA_CHUNK_LOCK_USE_TAG
	if(prev_tag.s.type != CVMX_POW_TAG_TYPE_NULL) {
		cvmx_pow_tag_sw_full((cvmx_wqe_t *)cvmx_phys_to_ptr(0x80), prev_tag.s.tag, prev_tag.s.type, 0);
		cvmx_pow_tag_sw_wait();
	} else
		cvmx_pow_tag_sw_null();
#else
	cvmx_spinlock_unlock(&(chunk->lock));
#endif
	return retval;
}




/**
 * Description:
 * Octeon software initiates a DMA write operation to transfer data between
 * the Octeon L2/DRAM and host memory. Checks and allocates required words
 * in the DMA chunk. Write the DMA header, followed by the local pointers
 * followed by the PCI components which are created on the fly.
 *
 * Input:
 *
 * Return Value:
 *      Success: 0
 */
static int
cvm_post_pci_dma_command(cvmx_oct_pci_dma_inst_hdr_t      *dma_hdr,
                         cvmx_oct_pci_dma_local_ptr_t     *lptr,
                         cvm_dma_remote_ptr_t             *rptr)
{
	uint32_t               words_reqrd, i, lsize, rsize;
	cvm_oct_dma_chunk_t   *chunk;
	uint64_t               dma_words[36];

	DBG_PRINT(DBG_FLOW,"----cvm_pci_dma_write----\n");
	CVMX_SYNCWS;

	dbg_printf("DMA Hdr: 0x%016lx\n", dma_hdr->u64);

	if( (!dma_hdr->s.nl || !dma_hdr->s.nr)
	    || (dma_hdr->s.nl > oct->max_lptrs) || (dma_hdr->s.nr > oct->max_rptrs)
	    || ((dma_hdr->s.nl + dma_hdr->s.nr) > oct->max_dma_ptrs) ) {
		printf("%s Invalid DMA Pointer count (local: %d remote: %d)\n",
		       __FUNCTION__, dma_hdr->s.nl, dma_hdr->s.nr);
		return -EINVAL;
	}

#if  defined(CVM_PCI_VALIDATE_DMA_POINTERS)
	if(cvm_pci_dma_validate_ptrs(dma_hdr, lptr, rptr)) {
		return -EINVAL;
	}
#endif

	/* Get a count of no. of 64-bits required in the chunk for this DMA op */
	words_reqrd  =  get_word_count(dma_hdr);


	i = 0;
	dma_words[i++] = dma_hdr->u64;

	i += cvm_pci_dma_copy_local_ptrs(dma_hdr->s.nl, &lsize, &dma_words[i], lptr);
	i += cvm_pci_dma_copy_remote_ptrs(dma_hdr->s.nr, &rsize, &dma_words[i],rptr);

	if(lsize != rsize)  {
		printf("[ DRV ] %s: local size (%u) != remote size (%u)\n", __FUNCTION__,
		       lsize, rsize);
		return -EINVAL;
	}

	chunk  = oct->chunk[dma_hdr->s.c];
	DBG_PRINT(DBG_FLOW,"words_reqrd: %d  nr: %d  nl: %d curr_word: %d\n",
	          words_reqrd, dma_hdr->s.nr, dma_hdr->s.nl, chunk->current_word);

#ifdef CVM_PCI_TRACK_DMA
	cvm_pci_dma_add_to_tracker_list(dma_words, words_reqrd);
#endif

	return cvm_pci_dma_chunk_write(chunk, dma_hdr->s.c, dma_words, words_reqrd);
}








/*
	Call this function to copy data from local buffers to the host/PCI-E memory
	space.
*/

int
cvm_pci_dma_send_data(cvm_pci_dma_cmd_t     *cmd,
                      cvmx_buf_ptr_t        *lptr,
                      cvm_dma_remote_ptr_t  *rptr)
{
	cvmx_oct_pci_dma_inst_hdr_t    dma_hdr;
	cvmx_oct_pci_dma_local_ptr_t   local_ptr[16];
	int i;

	if(oct->state != CVM_DRV_READY) {
		printf("[ DRV ] Core driver is in state (0x%x); DMA Send abort!\n",
		       oct->state);
		return 1;
	}

	dma_hdr.u64   = 0;

	if(OCTEON_IS_MODEL(OCTEON_CN56XX) || OCTEON_IS_MODEL(OCTEON_CN63XX) ||
	   OCTEON_IS_MODEL(OCTEON_CN66XX) || OCTEON_IS_MODEL(OCTEON_CN68XX) ) {
		dma_hdr.s.lport = cmd->s.pcielport;
	}

	dma_hdr.s.dir = PCI_DMA_OUTBOUND;
	dma_hdr.s.wqp = (cmd->s.flags & PCI_DMA_PUTWQE)?1:0;
	dma_hdr.s.ca  = (cmd->s.flags & PCI_DMA_CNTRADD)?1:0;
	dma_hdr.s.fi  = (cmd->s.flags & PCI_DMA_FORCEINT)?1:0;
	dma_hdr.s.fl  = (cmd->s.flags & PCI_DMA_FREELOCAL)?1:0;
	dma_hdr.s.ii  = (cmd->s.flags & PCI_DMA_IGNOREI)?1:0;
	dma_hdr.s.nr  = cmd->s.nr;
	dma_hdr.s.nl  = cmd->s.nl;


	/* CN56XX allows use of upto 5 queues. If queue 0 or 1 is used, we follow
	   the q_no as is. For the newer queues, the "Counter" field is set to 0.
	*/
	if(cmd->s.flags & PCI_DMA_CNTRADD)
		dma_hdr.s.c   = (cmd->s.q_no <= 1)?cmd->s.q_no:0;

	dma_hdr.s.ptr = cast64(cmd->s.ptr);  //cmd->s.ptr should be phys address
	for(i = 0; i < cmd->s.nl; i++) 
		local_ptr[i].u64 = (lptr[i].u64 &  CVM_PCI_DMA_LOCAL_PTR_MASK);
	
	CVMX_SYNCWS;

	if(OCTEON_IS_MODEL(OCTEON_CN56XX) || OCTEON_IS_MODEL(OCTEON_CN63XX) ||
	   OCTEON_IS_MODEL(OCTEON_CN66XX) || OCTEON_IS_MODEL(OCTEON_CN68XX) ) 
		return cvm_post_pcie_dma_command(cmd->s.q_no, &dma_hdr, local_ptr, rptr);
	else
		return cvm_post_pci_dma_command(&dma_hdr, local_ptr, rptr);

}





/*
	Call this function to copy data from known address(es) in the host/PCI-E
	memory to local buffers.
*/

int
cvm_pci_dma_recv_data(cvm_pci_dma_cmd_t     *cmd,
                      cvmx_buf_ptr_t        *lptr,
                      cvm_dma_remote_ptr_t  *rptr)
{
	cvmx_oct_pci_dma_inst_hdr_t    dma_hdr;
	cvmx_oct_pci_dma_local_ptr_t   local_ptr[16];
	int i;


	if(oct->state != CVM_DRV_READY) {
		printf("[ DRV ] Core driver is in state (0x%x); DMA Send abort!\n",
		       oct->state);
		return 1;
	}

	dma_hdr.u64   = 0;

	if(OCTEON_IS_MODEL(OCTEON_CN56XX) || OCTEON_IS_MODEL(OCTEON_CN63XX) ||
	   OCTEON_IS_MODEL(OCTEON_CN66XX) || OCTEON_IS_MODEL(OCTEON_CN68XX) ) {
		dma_hdr.s.lport = cmd->s.pcielport;
	}


	dma_hdr.s.dir = PCI_DMA_INBOUND;
	dma_hdr.s.wqp = (cmd->s.flags & PCI_DMA_PUTWQE)?1:0;
	dma_hdr.s.nr  = cmd->s.nr;
	dma_hdr.s.nl  = cmd->s.nl;

	/* For CN38xx/CN58XX, the DMA queue number is passed in the header itself.
	*/
	dma_hdr.s.c   = (cmd->s.q_no <= 1)?cmd->s.q_no:0;


	dma_hdr.s.ptr = cast64(cmd->s.ptr);  // cmd->s.ptr should be phys address
	for(i = 0; i < cmd->s.nl; i++) 
		local_ptr[i].u64 = (lptr[i].u64 &  CVM_PCI_DMA_LOCAL_PTR_MASK);
	
	CVMX_SYNCWS;

	DBG_PRINT(DBG_FLOW, "%s command from user: nl: %d nr: %d ptr: %llx flags: %x\n",
	 __FUNCTION__, cmd->s.nl, cmd->s.nr, CAST64(cmd->s.ptr), cmd->s.flags);
	DBG_PRINT(DBG_FLOW, "%s dma_hdr: 0x%016lx\n", __FUNCTION__, dma_hdr.u64);

	if(OCTEON_IS_MODEL(OCTEON_CN56XX) || OCTEON_IS_MODEL(OCTEON_CN63XX) ||
	   OCTEON_IS_MODEL(OCTEON_CN66XX) || OCTEON_IS_MODEL(OCTEON_CN68XX) ) 
		return cvm_post_pcie_dma_command(cmd->s.q_no, &dma_hdr, local_ptr, rptr);
	else
		return cvm_post_pci_dma_command(&dma_hdr, local_ptr, rptr);
}




/* 
	This is a wrapper around the cvm_post_pci_dma_command() function.
*/

int
cvm_pci_dma_raw(cvmx_oct_pci_dma_inst_hdr_t     *dma_hdr,
                cvmx_oct_pci_dma_local_ptr_t    *lptr,
                cvm_dma_remote_ptr_t            *rptr)
{
	if(oct->state != CVM_DRV_READY) {
		printf("[ DRV ] Core driver is in state (0x%x); DMA Send abort!\n",
		       oct->state);
		return 1;
	}

	return cvm_post_pci_dma_command(dma_hdr, lptr, rptr);
}
 




static int
cvm_post_pcie_dma_command(int                              q_no,
                          cvmx_oct_pci_dma_inst_hdr_t     *dma_hdr,
                          void                            *firstptrs,
                          void                            *lastptrs)
{
	uint32_t           words_reqrd, i, firstsize, lastsize;
	cvm_oct_dma_chunk_t   *chunk;
	uint64_t           dma_words[36];
	int                retval;

	DBG_PRINT(DBG_FLOW,"----cvm_pcie_dma_write----\n");

	if(q_no > oct->max_dma_qs) {
		printf("[ DRV ]: Found PCI-E DMA Command with q_no: %d (Max: %d)\n",
		        q_no, oct->max_dma_qs);
		return -EINVAL;
	}

	CVMX_SYNCWS;

	if( (!dma_hdr->s.nl || !dma_hdr->s.nr)
	    || (dma_hdr->s.nl > oct->max_lptrs) || (dma_hdr->s.nr > oct->max_rptrs)
	    || ((dma_hdr->s.nl + dma_hdr->s.nr) > oct->max_dma_ptrs) ) {
		printf("%s Invalid DMA Pointer count (local: %d remote: %d)\n",
		       __FUNCTION__, dma_hdr->s.nl, dma_hdr->s.nr);
		return -EINVAL;
	}

#if  defined(CVM_PCI_VALIDATE_DMA_POINTERS)
	if( ((dma_hdr->s.dir == PCI_DMA_INBOUND) || (dma_hdr->s.dir == PCI_DMA_OUTBOUND))
	   && cvm_pci_dma_validate_ptrs(dma_hdr, (cvmx_oct_pci_dma_local_ptr_t *)firstptrs, (cvm_dma_remote_ptr_t *)lastptrs)) {
		return -EINVAL;
	}
#endif

	/* Get a count of no. of 64-bits required in the chunk for this DMA op */
	words_reqrd  =  get_word_count(dma_hdr);

	i = 0;
	dma_words[i++] = dma_hdr->u64;

	if(dma_hdr->s.dir != PCI_DMA_EXTERNAL)
		i += cvm_pci_dma_copy_local_ptrs(dma_hdr->s.nl, &firstsize, &dma_words[i],
                                    (cvmx_oct_pci_dma_local_ptr_t *)firstptrs);
	else
		i += cvm_pci_dma_copy_remote_ptrs(dma_hdr->s.nr, &firstsize, &dma_words[i],
                                    (cvm_dma_remote_ptr_t *)firstptrs);


	if(dma_hdr->s.dir == PCI_DMA_INTERNAL)
		i += cvm_pci_dma_copy_local_ptrs(dma_hdr->s.nl, &lastsize, &dma_words[i],
                                    (cvmx_oct_pci_dma_local_ptr_t *)lastptrs);
	else
		i += cvm_pci_dma_copy_remote_ptrs(dma_hdr->s.nr, &lastsize, &dma_words[i],
                                    (cvm_dma_remote_ptr_t *)lastptrs);

	if(firstsize != lastsize)  {
		printf("[ DRV ] %s: first size (%u) != last size (%u)\n", __FUNCTION__,
		       firstsize, lastsize);
		return -EINVAL;
	}


	chunk  = oct->chunk[q_no];


	DBG_PRINT(DBG_FLOW,"DMA %d chunk @ %p words_reqrd: %d  hdr: 0x%016lx nr: %d  nl: %d curr_word: %d\n",
	       q_no, chunk, words_reqrd, dma_hdr->u64, dma_hdr->s.nr, dma_hdr->s.nl, chunk->current_word);

	retval = cvm_pci_dma_chunk_write(chunk, q_no, dma_words, words_reqrd);
	return retval;
}


/*
	Call this function to copy data from one set of local addresses to another.
*/

int
cvm_pcie_dma_internal(cvm_pci_dma_cmd_t    *cmd,
                      cvmx_buf_ptr_t       *firstptrs,
                      cvmx_buf_ptr_t       *lastptrs)
{
	cvmx_oct_pci_dma_inst_hdr_t    dma_hdr;
	cvmx_oct_pci_dma_local_ptr_t   first_lptr[16];
	cvmx_oct_pci_dma_local_ptr_t   last_lptr[16];
	int i;


	if(oct->state != CVM_DRV_READY) {
		printf("[ DRV ] Core driver is in state (0x%x); DMA Send abort!\n",
		       oct->state);
		return 1;
	}

	if(CVM_PCI_DMA_DIR(cmd) != PCI_DMA_INTERNAL) {
		printf("[ DRV ] %s: Invalid Direction (0x%x) in cmd->flags\n",
		        __FUNCTION__, CVM_PCI_DMA_DIR(cmd) );
		return 1;
	} 

	dma_hdr.u64   = 0;
	dma_hdr.s.dir = PCI_DMA_INTERNAL;
	dma_hdr.s.wqp = (cmd->s.flags & PCI_DMA_PUTWQE)?1:0;
	dma_hdr.s.nr  = cmd->s.nr;
	dma_hdr.s.nl  = cmd->s.nl;
	dma_hdr.s.ptr = cast64(cmd->s.ptr);  // cmd->s.ptr should be phys address

	for(i = 0; i < cmd->s.nl; i++) 
		first_lptr[i].u64 = (firstptrs[i].u64 &  CVM_PCI_DMA_LOCAL_PTR_MASK);

	for(i = 0; i < cmd->s.nr; i++) 
		last_lptr[i].u64 = (lastptrs[i].u64 &  CVM_PCI_DMA_LOCAL_PTR_MASK);
	
	CVMX_SYNCWS;

	return cvm_post_pcie_dma_command(cmd->s.q_no, &dma_hdr, first_lptr, last_lptr);
}



/*
	Call this function to copy data from one set of PCI-Express addresses
     to another.
*/
int
cvm_pcie_dma_external(cvm_pci_dma_cmd_t     *cmd,
                      cvm_dma_remote_ptr_t  *firstptrs,
                      cvm_dma_remote_ptr_t  *lastptrs)
{
	cvmx_oct_pci_dma_inst_hdr_t    dma_hdr;

	if(oct->state != CVM_DRV_READY) {
		printf("[ DRV ] Core driver is in state (0x%x); DMA Send abort!\n",
		       oct->state);
		return 1;
	}

	if(CVM_PCI_DMA_DIR(cmd) != PCI_DMA_EXTERNAL) {
		printf("[ DRV ] %s: Invalid Direction (0x%x) in cmd->flags\n",
		        __FUNCTION__, CVM_PCI_DMA_DIR(cmd) );
		return 1;
	} 

	dma_hdr.u64     = 0;
	dma_hdr.s.fport = cmd->s.pciefport;
	dma_hdr.s.lport = cmd->s.pcielport;
	dma_hdr.s.dir   = PCI_DMA_EXTERNAL;
	dma_hdr.s.wqp   = (cmd->s.flags & PCI_DMA_PUTWQE)?1:0;
	dma_hdr.s.nr    = cmd->s.nr;
	dma_hdr.s.nl    = cmd->s.nl;
	dma_hdr.s.ptr   = cast64(cmd->s.ptr);  // cmd->s.ptr should be phys address

	CVMX_SYNCWS;
	return cvm_post_pcie_dma_command(cmd->s.q_no, &dma_hdr,firstptrs, lastptrs);
}





/* 
	This is a wrapper around the cvm_post_pcie_dma_command() function.
*/

int
cvm_pcie_dma_raw(int                              q_no,
                 cvmx_oct_pci_dma_inst_hdr_t     *dma_hdr,
                 void                            *firstptrs,
                 void                            *lastptrs)
{
	if(oct->state != CVM_DRV_READY) {
		printf("[ DRV ] Core driver is in state (0x%x); DMA Send abort!\n",
		       oct->state);
		return 1;
	}

	return cvm_post_pcie_dma_command(q_no, dma_hdr, firstptrs, lastptrs);
}

/* $Id: cvm-pci-dma.c 66890 2011-11-09 03:09:07Z mchalla $ */
