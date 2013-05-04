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
#include <linux/sched.h>


MODULE_AUTHOR("Cavium Networks");
MODULE_LICENSE("Cavium Networks");


//#define MODULATE_WAIT

struct ddoq_global_stats dstats;


volatile unsigned long   ddoq_create_cnt = 0, ddoq_create_cnt_total = 0;
volatile unsigned long   ddoq_delete_cnt = 0, ddoq_delete_cnt_total = 0;
volatile unsigned long   total_secs=0;
cavium_atomic_t          active_ddoqs;
cavium_atomic_t          ddoq_inactive_pkts;

/* The largest ddoq id number seen by this test program */
volatile uint32_t        max_ddoq_id = 0;


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
/* Store the pid's here. cleanup_module needs them */
pid_t    ddoq_create_pid=-1, ddoq_delete_pid=-1;
#else
static cavium_ostask_t    *ddoq_create_task = NULL, *ddoq_delete_task = NULL;
#endif




/* This variable tells the thread when to stop. */
int     ddoq_create_ok_to_run = 1, ddoq_delete_ok_to_run = 1;



struct ddoq_test_list test_list;
extern void  oct_test_init_ddoq_setup(octeon_ddoq_setup_t  *d);


unsigned long  life_36=0;



#ifndef   DDOQ_PKT_INPUT_TEST
static void
oct_test_display_stats(unsigned long last)
{
	unsigned long  create_cnt, delete_cnt, active, sec;

	create_cnt = ddoq_create_cnt;
	ddoq_create_cnt = 0;
	delete_cnt = ddoq_delete_cnt;
	ddoq_delete_cnt = 0;
	active     = (unsigned long)cavium_atomic_read(&active_ddoqs);
	sec        = (jiffies - last)/HZ;

	total_secs += sec;
	printk("DDOQ_TEST: Max DDOQ Id: %d\n", max_ddoq_id);
	printk("DDOQ_TEST: Last %lu sec> Created: %lu  Deleted: %lu Active: %lu\n",
	        sec, create_cnt, delete_cnt, active);
	printk("DDOQ_TEST: Total %lu sec> Created: %lu Deleted: %lu Average: %lu\n",
	        total_secs, ddoq_create_cnt_total,
	        ddoq_delete_cnt_total, ddoq_delete_cnt_total/total_secs);
}

#endif




static int
oct_test_create_ddoq(struct ddoq_test_list  *dtl, octeon_ddoq_setup_t  *dsetup)
{
	int                     ddoq_id;
	struct ddoq_test_node  *tn;
	struct test_ddoq_info  *dinfo;

	PRINT_DBG(" --- %s ----\n", __FUNCTION__);

	if(cavium_atomic_read(&active_ddoqs) >= MAX_DDOQS)
		return 0;

	tn = (struct ddoq_test_node *)get_next_node(dtl->il);
	if(tn == NULL) {
		PRINT_DBG("DDOQ_TEST (CREATE): No node in init list\n");
		return 0;
	}

	dsetup->fn_arg = (void *)&tn->ddoq;
	dsetup->tag    = ddoq_create_cnt_total;
	ddoq_id = octeon_ddoq_create(OCTEON_ID, dsetup);

	if(ddoq_id > 0) {

		if(ddoq_id == 36)
			life_36 = cavium_jiffies + (2 * HZ);

		if(ddoq_id > max_ddoq_id)
			max_ddoq_id = ddoq_id;

		//printk("ddoq created. id: %d\n", ddoq_id);
		cavium_atomic_inc(&active_ddoqs);

		dinfo          = &tn->ddoq;
		dinfo->ddoq_id = ddoq_id;
		dinfo->status  = DT_STATE_CREATE;
		dinfo->prev_time = dinfo->create_time = jiffies;
		dinfo->prev_pkt_cnt = dinfo->pkt_cnt  = 0;

		put_free_node(dtl->cl, (struct test_node *)tn);
		ddoq_create_cnt++;
		ddoq_create_cnt_total++;
		return 1;
	}

	return 0;
}




static int
oct_test_delete_ddoq(struct ddoq_test_list  *dtl)
{
	int                       status;
	struct ddoq_test_node    *tn1, *tn;
	struct test_ddoq_info    *dinfo;

	PRINT_DBG(" --- %s ----\n", __FUNCTION__);

	tn1 = (struct ddoq_test_node *)peek_node(dtl->cl);
	if(tn1 == NULL)
		return 0;

	dinfo = &tn1->ddoq;
	if(!time_after(jiffies, (dinfo->create_time + DT_CREATE_WAIT_TIME)) )
		return 0;

	tn = (struct ddoq_test_node *)get_next_node(dtl->cl);
	if(tn != tn1) {
		printk(" >>> %s tn (%p) != tn1 (%p) <<<\n", __FUNCTION__, tn, tn1);
		return 0;
	}

	dinfo = &tn->ddoq;

	if( (dinfo->ddoq_id == 36) && !cavium_check_timeout(cavium_jiffies, life_36)) {
		put_free_node(dtl->cl, (struct test_node *)tn);
		return 0;
	}


	//printk("Found ddoq %d to delete\n", dinfo->ddoq_id);
	status = octeon_ddoq_delete(OCTEON_ID, dinfo->ddoq_id, NULL, 0);
	//printk("DDOQ[%d] deleted with status: %d\n", dinfo->ddoq_id, status);
	if(status) {
		printk("%s Delete failed for ddoq_id: %d\n",
		        __FUNCTION__, dinfo->ddoq_id);
		put_free_node(dtl->cl, (struct test_node *)tn);
		return 0;
	}
	dinfo->status = DT_STATE_DELETE;
	put_free_node(dtl->dl, (struct test_node *)tn);
	ddoq_delete_cnt++;
	ddoq_delete_cnt_total++;
	return 1;
}









static int
oct_test_cleanup_deleted_ddoq(struct ddoq_test_list  *dtl)
{
	int                        check_count=0;
	struct ddoq_test_node    *tdcn, *tdn;

	PRINT_DBG(" --- %s ----\n", __FUNCTION__);
 
	/* Get the next node from the delete confirmation list. */
	tdcn = (struct ddoq_test_node *)get_next_node(dtl->dcl);
	if(tdcn == NULL)
		return 0;

	while(tdcn) {

		/* Get the next node from the delete list. */
		tdn = (struct ddoq_test_node *)get_next_node(dtl->dl);
		check_count = 0;

		/* Till we find a match for the delete completion in the delete list. */
		while(tdn && (check_count < MAX_DDOQS)) {

			/* If the entries match ... */
			if(tdn->ddoq.ddoq_id == tdcn->ddoq.ddoq_id) {
				//printk("DDOQ[%d] delete confirmed\n", tdn->ddoq.ddoq_id);
				memset(&tdcn->ddoq, 0, sizeof(struct test_ddoq_info));
				/* Free the del compl node into del compl init list */
				put_free_node(dtl->dcil, (struct test_node *)tdcn);
				/* Free the del node into init list */
				put_free_node(dtl->il, (struct test_node *)tdn);

				/* That's one less DDOQ that's active */
				cavium_atomic_dec(&active_ddoqs);
				break;
			}

			/* No match. Continue searching the delete list. */
			check_count++;
			put_free_node(dtl->dl, (struct test_node *)tdn);
			tdn = (struct ddoq_test_node *)get_next_node(dtl->dl);
		}

		/* This should not happen. */
		if(check_count == MAX_DDOQS) {
			printk(">>>> %s DDOQ [%d] not found in delete list  <<<<\n",
			        __FUNCTION__, tdcn->ddoq.ddoq_id);
			break;
		}

		/* Get the next node in the delete completion list */
		tdcn = (struct ddoq_test_node *)get_next_node(dtl->dcl);
	}

	return 0;
}









static int
oct_ddoq_create_test_thread(void* arg)
{
#ifndef DDOQ_PKT_INPUT_TEST
	unsigned long           last_display=jiffies;
#endif
	struct ddoq_test_list  *dtl = (struct ddoq_test_list *)arg;
	octeon_ddoq_setup_t     dsetup;
	uint8_t                 udd[MAX_DDOQ_UDD_SIZE];
	int                     create_count, create_wait=1;

#define MAX_CREATE  1600

	dsetup.ddoq_udd = udd;
	oct_test_init_ddoq_setup(&dsetup);

	PRINT_DBG(" %s starting loop\n", __FUNCTION__);

#ifdef DDOQ_PKT_INPUT_TEST
	create_wait = (HZ * 1);
#endif

	
	do {
	    create_count = 0;
		do {
			if(create_count == MAX_CREATE)  {
#if 0
				create_count = 0;
				cavium_sleep_timeout(create_wait);
				if (signal_pending(current)) {
					ddoq_create_ok_to_run = 0;
					break;
				}
#else
				break;
#endif
			}	
			create_count++;
		} while(oct_test_create_ddoq(dtl, &dsetup));

#ifdef MODULATE_WAIT
		if(create_count < (MAX_CREATE >> 1))
			create_wait += (create_wait >= HZ/10)?0:HZ/100;	
		else
			create_wait -= (create_wait <= HZ/100)?0:HZ/100;	
#endif
		cavium_sleep_timeout(create_wait);

#if  LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
		if (signal_pending(current)) {
			ddoq_create_ok_to_run = 0;
			break;
		}
#else
		if(kthread_should_stop()) {
            ddoq_create_ok_to_run = 0;
            break;
        }
#endif

#ifndef  DDOQ_PKT_INPUT_TEST
		if(time_after(jiffies,(last_display + DISPLAY_INTERVAL))) {
			oct_test_display_stats(last_display);
			last_display = jiffies;
		}
#endif

	} while(ddoq_create_ok_to_run);


	printk("Test thread: quitting create thread\n");
	return 0;
}






#ifndef DDOQ_PKT_INPUT_TEST

static int
oct_ddoq_delete_test_thread(void* arg)
{
	struct ddoq_test_list  *dtl = (struct ddoq_test_list *)arg;
	int                     delete_count, delete_wait=1;
#define MAX_DELETE 1600

	PRINT_DBG(" %s starting loop\n", __FUNCTION__);

	do {
		delete_count = 0;
		/* Delete all DDOQ's that are past the wait time. */
		do {
			if(delete_count == MAX_DELETE)  {
				delete_count = 0;
				cavium_sleep_timeout(delete_wait);
#if  LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
				if (signal_pending(current)) {
					ddoq_delete_ok_to_run = 0;
					break;
				}
#else
				if(kthread_should_stop()) {
					ddoq_delete_ok_to_run = 0;
					break;
				}
#endif
			}
			delete_count++;
		} while(oct_test_delete_ddoq(dtl));

#ifdef MODULATE_WAIT
		if(delete_count < (MAX_DELETE >> 1))
			delete_wait += (delete_wait >= HZ/10)?0:HZ/100;	
		else
			delete_wait -= (delete_wait <= HZ/100)?0:HZ/100;	
#endif

		/* Check for delete confirms that have arrived. */
		oct_test_cleanup_deleted_ddoq(dtl);

		/* Wait for 1/200 of a second before we continue. */
		cavium_sleep_timeout(delete_wait);
#if  LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
		if (signal_pending(current)) {
			ddoq_delete_ok_to_run = 0;
			break;
		}
#else
		if(kthread_should_stop())
			break;
#endif

	} while(ddoq_delete_ok_to_run);


	printk("Test thread: quitting delete thread\n");
	return 0;
}

#endif





int
oct_test_init_test_list(struct ddoq_test_list  *tl)
{
	int state = 0;

	/* Create a "initial" list with one node for each ddoq that can exist */
	tl->il = initialize_test_list(MAX_DDOQS, sizeof(struct test_ddoq_info));
	if(tl->il == NULL)
		return -ENOMEM;

	state = 1;

	/* Create a empty "create" list. Nodes will taken from the "initial" list
	   and added to this list as ddoq's are created.  */
	tl->cl = initialize_test_list(0, 0);
	if(tl->cl == NULL)
		goto  init_test_list_error;

	state = 2;

	/* Create a empty "delete" list. Nodes will taken from the "create" list
	   and added to this list as ddoq's are deleted.  */
	tl->dl = initialize_test_list(0, 0);
	if(tl->dl == NULL)
		goto  init_test_list_error;

	state = 3;

	/* Create a "delete confirm initial" list with one node for each ddoq
	   that can exist */
	tl->dcil = initialize_test_list(MAX_DDOQS, sizeof(struct test_ddoq_info));
	if(tl->dcil == NULL)
		goto  init_test_list_error;

	state = 4;

	/* Create a empty "delete confirm" list. Nodes will taken from the
	   "delete confirm initial" list" and added to this list as ddoq
	   delete confirm's are received from driver.  */
	tl->dcl = initialize_test_list(0, 0);
	if(tl->dcl)
		return 0;

	/* If any allocation fails, we come here to clean up. */
init_test_list_error:
	switch(state) {
		case 4:
		cleanup_test_list(tl->dcil, MAX_DDOQS);
		case 3:
		cleanup_test_list(tl->dl, 0);
		case 2:
		cleanup_test_list(tl->cl, 0);
		case 1:
		cleanup_test_list(tl->il, MAX_DDOQS);
	}
	return -ENOMEM;
}




static struct proc_dir_entry    *ddoq_test_root;
static uint64_t                  last_total_bytes = 0, last_total_pkts = 0;

static int
proc_read_ddoq_test_stats(char *page, char **start, off_t off, int count,
                          int *eof, void *data)
{
	int len = 0;
	uint64_t  last_pkts = 0, last_bytes = 0;
	
	last_pkts = (dstats.total_pkts - last_total_pkts);
	last_bytes = (dstats.total_bytes - last_total_bytes);

	last_total_bytes = dstats.total_bytes;
	last_total_pkts  = dstats.total_pkts;
	
	proc_print(page, len, "TotalPkts: %llu TotalBytes: %llu ErrorPkts: %llu \n",
			   CVM_CAST64(dstats.total_pkts), CVM_CAST64(dstats.total_bytes),
			   CVM_CAST64(dstats.error_pkts));
	proc_print(page, len, "Since Last: Pkts: %llu Bytes: %llu\n", 
			   CVM_CAST64(last_pkts), CVM_CAST64(last_bytes));
	proc_print(page, len, "Last Pkt seen at %lu ticks (current tick: %lu)\n",
			   dstats.last_pkt_recv_time, jiffies);
	
	*eof = 1;
	return len;
}




int
oct_ddoq_test_create_proc(void)
{
	struct proc_dir_entry    *root, *node;

	/* create directory /proc/OcteonX */
	root = proc_mkdir("cavium_ddoq_test", NULL);
	if(root == NULL)
		return 1;
	SET_PROC_OWNER(root);

	node = create_proc_entry("stats", 0444, root);
	if(node == NULL)
		return 1;
	node->read_proc = proc_read_ddoq_test_stats;
	SET_PROC_OWNER(node);

	ddoq_test_root = root;
	
	return 0;
}





void 
oct_ddoq_test_delete_proc(void)
{
	remove_proc_entry("stats", ddoq_test_root);
	remove_proc_entry("cavium_ddoq_test", NULL);
}






int 
init_module()
{
	const char  *ddoq_test_cvs_tag = "$Name: PCI_CNTQ_RELEASE_2_2_0_build_84 ";
	char         ddoq_test_version[80];


	cavium_atomic_set(&active_ddoqs, 0);
	cavium_atomic_set(&ddoq_inactive_pkts, 0);


	if(oct_test_init_test_list(&test_list)) {
		printk("DDOQ_TEST: Test List initialization failed\n");
		return -ENOMEM;
	}

	/* Create the ddoq create thread */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
	ddoq_create_pid = kernel_thread(oct_ddoq_create_test_thread, (void *)&test_list, CLONE_FS|CLONE_FILES|CLONE_SIGHAND);
	if(ddoq_create_pid < 0) {
		printk("DDOQ_TEST: Test thread (DDOQ_CREATE) creation failed\n");
		goto return_with_error;
	}
#else
	ddoq_create_task = kthread_run(oct_ddoq_create_test_thread, (void *)&test_list, "DDOQ Test Create");
	if(ddoq_create_task == NULL) {
		printk("DDOQ_TEST: Test thread (DDOQ_CREATE) creation failed\n");
		goto return_with_error;
	}
#endif



#ifndef DDOQ_PKT_INPUT_TEST
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
	/* Create the ddoq delete thread */
	ddoq_delete_pid = kernel_thread(oct_ddoq_delete_test_thread, (void *)&test_list, CLONE_FS|CLONE_FILES|CLONE_SIGHAND);
	if(ddoq_delete_pid < 0) {
		printk("DDOQ_TEST: Test thread (DDOQ_DELETE) creation failed\n");
		kill_proc(ddoq_create_pid, SIGTERM, 1);
		goto return_with_error;
	}
#else
	/* Create the ddoq delete thread */
	ddoq_delete_task = kthread_run(oct_ddoq_delete_test_thread, (void *)&test_list, "DDOQ Test Delete");
	if(ddoq_delete_task == NULL) {
		printk("DDOQ_TEST: Test thread (DDOQ_DELETE) creation failed\n");
		kthread_stop(ddoq_create_task);
		goto return_with_error;
	}
#endif
#endif
	if(oct_ddoq_test_create_proc()) {
		printk("!! Error !! No /proc entries created\n");	
	}

	cavium_parse_cvs_string(ddoq_test_cvs_tag, ddoq_test_version, 80);
	cavium_print_msg("DDOQ_TEST: Octeon DDOQ test application (%s) loaded\n",
	                 ddoq_test_version);

	return 0;

return_with_error:
	cleanup_test_list(test_list.dl, 0);
	cleanup_test_list(test_list.cl, 0);
	cleanup_test_list(test_list.il, MAX_DDOQS);
	return -ENOMEM;
}





//extern void print_ddoq_delete_stats(void);


void
cleanup_module()
{
#define  CLEANUP_COUNT   250
	int  delete_wait_count;  /* Max wait of 5 seconds */

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0)
	/* SIGINT and SIGHUP both need to be disallowed for the time when 
	   we send out delete instructions for the existing DDOQ's. */
	disallow_signal(SIGINT);
	disallow_signal(SIGHUP);
#endif

	/* Stop the create thread. */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
		kill_proc(ddoq_create_pid, SIGTERM, 1);
#else
		kthread_stop(ddoq_create_task);
#endif

	oct_ddoq_test_delete_proc();

#ifndef DDOQ_PKT_INPUT_TEST

	delete_wait_count = CLEANUP_COUNT;
	while(cavium_atomic_read(&active_ddoqs) && delete_wait_count--)
		cavium_sleep_timeout(HZ/50);

	/* Wait 100ms to make sure there are no more ddoq's */
	cavium_sleep_timeout(HZ/10);

	cavium_print_msg("DDOQTEST: Active DDOQ: %d Waiting for deletion..\n",
	                 cavium_atomic_read(&active_ddoqs));

	delete_wait_count = CLEANUP_COUNT;
	/* Check to make sure all DDOQ's have been deleted. */
	while(cavium_atomic_read(&active_ddoqs) && delete_wait_count--) {
		oct_test_delete_ddoq(&test_list);
		oct_test_cleanup_deleted_ddoq(&test_list);
		cavium_sleep_timeout(HZ/50);
	}

	/* Kill the delete thread now. */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
		kill_proc(ddoq_delete_pid, SIGTERM, 1);
#else
		kthread_stop(ddoq_delete_task);
#endif

#else
	delete_wait_count = CLEANUP_COUNT;
	while(cavium_atomic_read(&active_ddoqs) && delete_wait_count--) {
		cavium_sleep_timeout(HZ/50);
		printk("jiffies: %lu ddoqs: %d count: %d\n", jiffies, cavium_atomic_read(&active_ddoqs), delete_wait_count);
		oct_test_delete_ddoq(&test_list);
		oct_test_cleanup_deleted_ddoq(&test_list);
	}

	cavium_sleep_timeout(HZ);

#endif



//	oct_test_display_stats();
	cavium_print_msg("DDOQ_TEST: Total: Created: %lu Deleted: %lu\n",
	                 ddoq_create_cnt_total, ddoq_delete_cnt_total);
		//print_ddoq_delete_stats();
	if(cavium_atomic_read(&active_ddoqs)) {
		printk("There is still %d ddoq active. Waiting for delete thread\n",
		       cavium_atomic_read(&active_ddoqs));
		delete_wait_count = CLEANUP_COUNT;
		while(ddoq_delete_ok_to_run && delete_wait_count--)
			cavium_sleep_timeout(HZ/50);
		//print_ddoq_delete_stats();
	}
}





/* $Id: oct_ddoq_test.c 54238 2010-10-22 19:40:58Z panicker $ */
