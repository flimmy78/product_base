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


#include "cavium_sysdep.h"
#include "cavium_defs.h"
#include "octeon-cntq-common.h"
#include "octeon_cntq_defs.h"
#include "octeon_cntq.h"
#include "octeon_ddoq_list.h"
#include "octeon_ddoq.h"
#include "octeon_stats.h"


static uint32_t  proc_ddoq_id[MAX_OCTEON_DEVICES];




static int
proc_write_ddoq_stats(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char      str[10], *strend;
	uint32_t  ddoq_id=0;
	int       octeon_id = get_octeon_device_id(data);

	if(octeon_id < 0)
		return 0;

	CVM_MOD_INC_USE_COUNT;
	if(cavium_copy_in(str, buffer, count)) {
		cavium_error("cavium_copy_in failed\n");
		CVM_MOD_DEC_USE_COUNT;
		return -EFAULT;
	}
	str[count] = '\0';

	ddoq_id = simple_strtoul(str, &strend, 0);
	if(ddoq_id >=  DDOQ_ID_OFFSET) 
		proc_ddoq_id[octeon_id] = ddoq_id;
	else
		cavium_error("Invalid DDOQ id %d in /proc/octeon%d/ddoq_stats\n", ddoq_id, octeon_id);
	CVM_MOD_DEC_USE_COUNT;

	return count;
}







static int
proc_read_ddoq_stats(char    *page,
                     char   **start,
                     off_t    off,
                     int      count,
                     int     *eof,
                     void    *data)
{
	int                   len = 0, octeon_id = get_octeon_device_id(data);
	oct_ddoq_stats_t      ddoq_stats;
	int                   ddoq_id, status;

	if(octeon_id < 0)
		return 0;

	CVM_MOD_INC_USE_COUNT;

	/* Check that the CNTQ queues are in ENABLE state before calling any
	   stats functions. */
	status = octeon_get_cntq_status(octeon_id, 0);
	if(status != CNTQ_ENABLE) { CVM_MOD_DEC_USE_COUNT; return len; }

	status = octeon_get_cntq_status(octeon_id, 1);
	if(status != CNTQ_ENABLE) { CVM_MOD_DEC_USE_COUNT; return len; }


	ddoq_id = proc_ddoq_id[octeon_id];

	if(octeon_ddoq_get_stats(octeon_id, ddoq_id, &ddoq_stats)) {
		proc_print(page, len, "DDOQ[%d] does not exist\n", ddoq_id);
		CVM_MOD_DEC_USE_COUNT;
		*eof = 1;
		return len;
	}

	proc_print(page, len, "Stats for DDOQ[%d] State: %llx\n", ddoq_id, CVM_CAST64(ddoq_stats.state) );
	proc_print(page, len, "Received: %llu packets/ %llu bytes  Dropped: %llu packets\n", CVM_CAST64(ddoq_stats.packets_recvd), CVM_CAST64(ddoq_stats.bytes_received), CVM_CAST64(ddoq_stats.packets_dropped));
	proc_print(page, len, "Credits: Sent %llu instructions/ %llu descriptors (%llu Failed)\n", CVM_CAST64(ddoq_stats.credit_instr_cnt), CVM_CAST64(ddoq_stats.credit_desc_cnt), CVM_CAST64(ddoq_stats.credit_instr_failed));
	proc_print(page, len, "Desc pending refill: %llu Desc for next credit: %llu\n", CVM_CAST64(ddoq_stats.desc_pending_refill), CVM_CAST64(ddoq_stats.desc_pending_credit));
	proc_print(page, len, "Packets pending: %llu\n", CVM_CAST64(ddoq_stats.pkts_pending));
	CVM_MOD_DEC_USE_COUNT;
	*eof = 1;
	return len;
}






int
octcntq_proc_read_statsb(int octeon_id, oct_stats_t  *stats)
{
	int    error = 0, status;
	
	/* Module usage count need not be incremented here since this function
	   will always be called from the main *_statsb function. */

	/* Check that the CNTQ queues are in ENABLE state before calling any
	   stats functions. */
	status = octeon_get_cntq_status(octeon_id, 0);
	if(status != CNTQ_ENABLE) {  return 1; }

	status = octeon_get_cntq_status(octeon_id, 1);
	if(status != CNTQ_ENABLE) {  return 1; }


	error  =  octeon_cntq_get_stats(octeon_id, 0, &stats->cntq[0]);
	error |=  octeon_cntq_get_stats(octeon_id, 1, &stats->cntq[1]);
	error |=  octeon_ddoq_list_get_stats(octeon_id, &stats->ddoq_list);

	if(error)
		return 1;

	stats->components |= OCTEON_CNTQ_STATS_ON;
	return 0;
}








int
octcntq_proc_read_stats(int octeon_id, char   *page)
{
	oct_cntq_stats_t        cntq_stats[MAX_OCTEON_DMA_QUEUES];
	oct_ddoq_list_stats_t   ddoq_list_stats;
	int                     i, error = 0, len=0, status;

	if(octeon_id < 0)
		return len;

	CVM_MOD_INC_USE_COUNT;

	/* Check that the CNTQ queues are in ENABLE state before calling any
	   stats functions. */
	status = octeon_get_cntq_status(octeon_id, 0);
	if(status != CNTQ_ENABLE) { CVM_MOD_DEC_USE_COUNT; return len; }

	status = octeon_get_cntq_status(octeon_id, 1);
	if(status != CNTQ_ENABLE) { CVM_MOD_DEC_USE_COUNT; return len; }

	error  =  octeon_cntq_get_stats(octeon_id, 0, &cntq_stats[0]);
	error |=  octeon_cntq_get_stats(octeon_id, 1, &cntq_stats[1]);
	error |=  octeon_ddoq_list_get_stats(octeon_id, &ddoq_list_stats);

	if(error)  { CVM_MOD_DEC_USE_COUNT; return len; }

	proc_print(page, len,  "--CNTQ--                     ");
	for(i = 0; i < MAX_OCTEON_DMA_QUEUES; i++)
		proc_print(page, len,"    CNTQ %d    ", i);

	proc_print(page, len,"\n Pkts Received             :");
	for(i = 0; i < MAX_OCTEON_DMA_QUEUES; i++)
		proc_print(page,len,"  %8llu  ", CVM_CAST64(cntq_stats[i].pkts_received));

	proc_print(page, len,"\n Bytes Received            :");
	for(i = 0; i < MAX_OCTEON_DMA_QUEUES;  i++)
	proc_print(page, len,"  %8llu  ", CVM_CAST64(cntq_stats[i].bytes_received));

	proc_print(page, len,"\n Pkts dropped ");
	proc_print(page, len,"\n        No dispatch        :");
	for(i = 0; i < MAX_OCTEON_DMA_QUEUES; i++)
		proc_print(page, len,"  %8llu  ", CVM_CAST64(cntq_stats[i].dropped_nodispatch));

	proc_print(page, len,"\n        No memory          :");
	for(i = 0; i < MAX_OCTEON_DMA_QUEUES; i++)
		proc_print(page, len,"  %8llu  ", CVM_CAST64(cntq_stats[i].dropped_nomem));

	proc_print(page, len,"\n Credits ");
	proc_print(page, len,"\n           Posted          :");
	for(i = 0; i < MAX_OCTEON_DMA_QUEUES; i++)
		proc_print(page, len,"  %8llu  ", CVM_CAST64(cntq_stats[i].credits_posted));

	proc_print(page, len,"\n           Failed          :");
	for(i = 0; i < MAX_OCTEON_DMA_QUEUES; i++)
		proc_print(page, len,"  %8llu  ", CVM_CAST64(cntq_stats[i].credits_failed));
	proc_print(page, len, "\n__________________________________________________________________________\n");


	proc_print(page, len,   "--DDOQ's--\n");
	proc_print(page, len,   "   Total DDOQ's Created       : %8llu\n", CVM_CAST64(ddoq_list_stats.ddoqs_created));
	proc_print(page, len,   "   DDOQ Creation failed       : %8llu\n", CVM_CAST64(ddoq_list_stats.create_failed));
	proc_print(page, len,   "   Current active DDOQ's      : %8llu\n", CVM_CAST64(ddoq_list_stats.active_ddoqs));
	proc_print(page, len,   "   DDOQ's pending Delete EOT  : %8llu\n", CVM_CAST64(ddoq_list_stats.ddoqs_pending_eot));
	proc_print(page, len,   "   DDOQ's Delete B4 create    : %8llu\n", CVM_CAST64(ddoq_list_stats.del_b4_crt));

	CVM_MOD_DEC_USE_COUNT;

	return len;
}








int
octcntq_init_proc(int octeon_id)
{
	octeon_proc_entry_t   entry;

	strcpy(entry.name, "ddoq_stats");
	entry.attributes = 0644;
	entry.read_proc  = proc_read_ddoq_stats;
	entry.write_proc = proc_write_ddoq_stats;
	entry.type       = OCTEON_PROC_READ | OCTEON_PROC_WRITE;
	if(octeon_add_proc_entry(octeon_id, &entry)) {
		cavium_error("OCTEON_CNTQ: Added proc entry for ddoq stats failed\n");
		return 1;
	}	
	return 0;
}




void
octcntq_delete_proc(int octeon_id)
{
	octeon_delete_proc_entry(octeon_id, "ddoq_stats");
}





/* $Id: octcntq_proc.c 42868 2009-05-19 23:57:52Z panicker $ */
