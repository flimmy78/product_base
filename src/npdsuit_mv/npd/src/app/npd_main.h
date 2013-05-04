#ifndef __NPD_MAIN_H__
#define __NPD_MAIN_H__

unsigned int npd_query_sw_version(void);
extern void npd_mirror_init(void);
extern void npd_fdb_static_init();
extern void npd_fdb_blacklist_init();

extern int npd_route_init();

extern void npd_init_arpsnooping();
/*******************************************************************************
* nam_thread_create
*
* DESCRIPTION:
*       Create Linux thread and start it.
*
* INPUTS:
*       name    - task name, valid when accessChip is TRUE
*       start_addr - task/thread Function to execute
*       arglist    - pointer to list of parameters for task/thread function
*	  accessChip - this thread/task need access chip register or not
*	  needJoin - this thread need pthread_join operation
*
* OUTPUTS:
*
* RETURNS:
*       0   - on success
*       1 - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
extern void nam_thread_create
(
	char	*name,
	unsigned (*entry_point)(void*),
	void	*arglist,
	unsigned char accessChip,
	unsigned char needJoin
);
#endif
