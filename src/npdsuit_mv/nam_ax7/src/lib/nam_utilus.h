#ifndef __NAM_UTILUS_H__
#define __NAM_UTILUS_H__

#include <pthread.h>

#define INDEX_ALLOCATED	0x7FFE
#define INDEX_END_OF_FREE	0x7FFF

#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0

typedef unsigned int index_elem;

typedef struct _tbl_index_s_ {
	unsigned int	max;
	unsigned int	available;
	index_elem		free;
	index_elem		(*table)[];	
}tbl_index_t;

tbl_index_t *nam_index_create
(
	unsigned int max
);

void nam_index_destroy
(
	tbl_index_t *number
);

unsigned int nam_index_get
(
	tbl_index_t   *number,
	index_elem	  val	
);

unsigned int nam_index_alloc
(
	tbl_index_t *number, 
	index_elem *val
);

unsigned int nam_index_free
(
	tbl_index_t *number, 
	index_elem 	val
);

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
void nam_thread_create
(
	char	*name,
	unsigned (*entry_point)(void*),
	void	*arglist,
	unsigned char accessChip,
	unsigned char needJoin
);

#ifdef DRV_LIB_CPSS
/*******************************************************************************
* osTaskCreate
*
* DESCRIPTION:
*       Create OS Task and start it.
*
* INPUTS:
*       name    - task name, string no longer then OS_MAX_TASK_NAME_LENGTH
*       prio    - task priority 1 - 64 => HIGH
*       stack   - task Stack Size
*       start_addr - task Function to execute
*       arglist    - pointer to list of parameters for task function
*
* OUTPUTS:
*       tid   - Task ID
*
* RETURNS:
*       0   - on success
*       1 - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
extern unsigned long osTaskCreate
(
    char    *name,
    unsigned long  prio,
    unsigned long  stack,
    unsigned (*start_addr)(void*),
    void    *arglist,
    unsigned long *tid
);

/*******************************************************************************
* osTaskGetThreadId
*
* DESCRIPTION:
*       get task's thread id via task id
*
* INPUTS:
*       tid - task id
*
* OUTPUTS:
*       pthrid -  the task's thread id
*
* RETURNS:
*       GT_OK - on success.
*       GT_FAIL - if parameter is invalid
*
* COMMENTS:
*       None
*
*******************************************************************************/
extern unsigned long osTaskGetThreadId
(
    unsigned long tid,
	pthread_t *pthrid
);
#endif
#endif
