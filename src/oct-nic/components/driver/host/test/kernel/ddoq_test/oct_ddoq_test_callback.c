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

#include "oct_test_list.h"
#include "oct_ddoq_test.h"
#include "octeon-ddoq-testpattern.h"

extern  cavium_atomic_t          ddoq_inactive_pkts;
extern  struct ddoq_test_list    test_list;

extern struct ddoq_global_stats dstats;



static  void
free_recv_pkt_buffers(octeon_recv_pkt_t  *recv_pkt)
{
	int i;

	for( i = 0; i < recv_pkt->buffer_count; i++) {
		switch(recv_pkt->buf_type) {
			case OCT_BUFFER_TYPE_1:
				cavium_free_dma(recv_pkt->buffer_ptr[i]);
				break;
			case OCT_BUFFER_TYPE_2:
				free_recv_buffer(recv_pkt->buffer_ptr[i]);
				break;
			default:
				cavium_error("ddoq_test: %s: Unknown buf type \n", __FUNCTION__);
		}
	}
}



int
verify_data(octeon_recv_pkt_t  *recv_pkt)
{
	int i, j= 0, idx = 0;

	for( i = 0; i < recv_pkt->buffer_count; i++) {
		uint8_t  *buf = recv_pkt->buffer_ptr[i];
		for(j = 0; j < recv_pkt->buffer_size[i]; j++) {
			if(buf[j] != ddoqsign[idx]) {
				int k = (j > 16)?(j - 16):0;
				printk("Buffer mismatch at buf[%d][%d]\n", i, j);
				printk("Printing last %d bytes before mismatch\n, Expected ",
				       (j - k));
				for( ; k <= j; k++)
					printk(" %02x ", ddoqsign[k]);
				printk("\nFound ");
				k = (j > 16)?(j - 16):0;
				for( ; k <= j; k++)
					printk(" %02x ", buf[k]);
				printk("\n");
				return 1;
			}
			INCR_SIGNIDX(idx);
		}
	}
	
	return 0;
}




int  test_ddoq_dispatch(octeon_recv_info_t *recv_info, void *arg)
{
	octeon_recv_pkt_t      *recv_pkt = recv_info->recv_pkt;
	struct test_ddoq_info  *dinfo    = (struct test_ddoq_info *)arg;
	uint32_t                ddoq_id;

	if(dinfo->status != DT_STATE_CREATE) {
		cavium_atomic_inc(&ddoq_inactive_pkts);
		goto dispatch_free;
	}

	ddoq_id = recv_pkt->resp_hdr.dest_qport;
	dinfo->pkt_cnt++;
	dinfo->byte_cnt += recv_pkt->length;
	dstats.total_pkts++;
	dstats.total_bytes += recv_pkt->length;
	dstats.last_pkt_recv_time = jiffies;
	
//	printk("Received %d bytes\n", recv_pkt->length);

	if(time_after(jiffies, (dinfo->prev_time + DISPLAY_INTERVAL))) {
		uint32_t  sec = (jiffies - dinfo->prev_time)/HZ;
		unsigned long  avg_pkt, avg_bps, avg_pktsize;
		cavium_div64(CVM_CAST64(dinfo->pkt_cnt - dinfo->prev_pkt_cnt), (uint64_t)sec, &avg_pkt);
		cavium_div64((CVM_CAST64(dinfo->byte_cnt - dinfo->prev_byte_cnt) * 8), (uint64_t)sec, &avg_bps);
		cavium_div64((CVM_CAST64(dinfo->byte_cnt - dinfo->prev_byte_cnt)), CVM_CAST64(dinfo->pkt_cnt - dinfo->prev_pkt_cnt), &avg_pktsize);
		printk("ddoq[%d]: %llupkts (last %d sec avg Packets: %lu bps: %lu pktsize: %lu)\n",
		       ddoq_id, CVM_CAST64(dinfo->pkt_cnt), sec, avg_pkt, avg_bps, avg_pktsize);
		dinfo->prev_time = jiffies;
		dinfo->prev_pkt_cnt  = dinfo->pkt_cnt;
		dinfo->prev_byte_cnt = dinfo->byte_cnt;
	}

	if(verify_data(recv_pkt)) {
		dstats.error_pkts++;
	}
	
dispatch_free:
    free_recv_pkt_buffers(recv_pkt);
	kfree(recv_info);
	return 0;
}





void *
test_ddoq_buf_alloc(uint32_t  ddoq_id,
                    uint32_t  buf_size)
{
    void *buf;
    buf = kmalloc(buf_size, GFP_ATOMIC);
    PRINT_DBG("ddoq_test: allocating %d size buf for ddoq: %d @ 0x%lx\n",
            buf_size, ddoq_id, (unsigned long)buf );
    return buf;
}



void
test_ddoq_buf_dealloc(uint32_t ddoq_id,
                      void   *buf)
{
   PRINT_DBG("ddoq_test: Freeing buf @ 0x%lx for ddoq : %d\n",
           (unsigned long)buf, ddoq_id);
   kfree(buf);
}







void
test_ddoq_delete_conf(uint32_t  ddoq_id,
                      uint32_t  status)
{
	struct ddoq_test_node   *tdcn;

	if(status)
		printk("DDOQ[%d] delete conf status: %d\n", ddoq_id, status);

	//printk("DDOQTEST: Delete conf received for %d\n", ddoq_id);
	tdcn = (struct ddoq_test_node *)get_next_node(test_list.dcil);
	if(tdcn == NULL) {
		printk("%s Delete Confirmation Init list is empty\n", __FUNCTION__);
		return;
	}

	tdcn->ddoq.ddoq_id = ddoq_id;
	tdcn->ddoq.status  = status;
	put_free_node(test_list.dcl, (struct test_node *)tdcn);
}







void
oct_test_init_ddoq_setup(octeon_ddoq_setup_t  *d)
{
	int       i;
	uint8_t  *udd = (uint8_t *)d->ddoq_udd;

	cavium_memset(d, 0, sizeof(octeon_ddoq_setup_t));

	d->dispatch_fn      = test_ddoq_dispatch;
	d->buf_alloc_fn     = test_ddoq_buf_alloc;
	d->buf_dealloc_fn   = test_ddoq_buf_dealloc;
	d->delete_conf_fn   = test_ddoq_delete_conf;
	d->no_of_desc       = DT_DESC_COUNT;
	d->buffer_size      = DT_BUF_SIZE;
	d->refill_threshold = DT_REFILL_THRESHOLD;
	d->initial_fill     = DT_INITIAL_REFILL;
	d->type             = DDOQ_TYPE_TEST;

	if(udd) {
		for(i = 0; i < MAX_DDOQ_UDD_SIZE; i++)
			udd[i] = i;
		d->ddoq_udd = udd;
		d->ddoq_udd_size = MAX_DDOQ_UDD_SIZE;
	}
}


/* $Id: oct_ddoq_test_callback.c 63294 2011-08-09 21:48:01Z panicker $ */

