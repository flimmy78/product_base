/*******************************************************************************
Copyright (C) Autelan Technology

This software file is owned and distributed by Autelan Technology 
********************************************************************************

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************
* nam_utilus.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		APIs used in NAM for utilities such as allocate and manage table index
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.8 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdlib.h>
#include <string.h>

#include <syslog.h>

#include "nam_utilus.h"

tbl_index_t *nam_index_create
(
	unsigned int max
)
{
	tbl_index_t *number = NULL;
	unsigned short	i = 0;

	number = (tbl_index_t*)malloc(sizeof(tbl_index_t));
	if(NULL == number) {
		return NULL;
	}
	memset(number, 0, sizeof(tbl_index_t));
	number->table = (void *) malloc(sizeof(index_elem) * max);
	if(NULL == number->table) {
		free(number);
		return NULL;
	}
	memset(number->table, 0, sizeof(index_elem)*max);
	
	number->available 	= max;
	number->max 		= max;
	number->free		= 0;

	for(i=0 ;i< max; i++) {
		(*number->table)[i] = (index_elem)(i+1);
	}
	(*number->table)[max-1] = INDEX_END_OF_FREE;

	return number;
}

void nam_index_destroy
(
	tbl_index_t *number
)
{
	if((NULL == number)||(NULL == number->table)) {
		return;
	}

	free(number->table);
	number->table = NULL;
	free(number);
	number = NULL;
}

unsigned int nam_index_alloc
(
	tbl_index_t *number, 
	index_elem *val
)
{
	unsigned int status = 0;

	if(INDEX_END_OF_FREE == number->free) {
		status = 1; /* no number resource*/
	}
	else if((number->free >= number->max)||(0 == number->available)) {
		status = 2; /* no available resource or next free resource out of bound*/
	}
	else {
		number->available--;
		*val = number->free;
		number->free = (*number->table)[*val];
		(*number->table)[*val] = INDEX_ALLOCATED;
	}

	return status;
}

/***********************************************************************************
 *
 *	nam_index_get
 *
 *	DESCRIPTION:
 *		alloc a particular number from the free number list
 *
 *	INPUT:
 *		number - number list
 *
 *	OUTPUT:
 *		val - specific number to alloc
 *
 *	RETURN:
 *		0 - if no error occurred
 *		1 - if the given number has been allocated
 *		2 - if the given number is out of range
 * 		3 - if no number resource left
 *		4 - if other errors occur
 * 
 *
 *
 ***********************************************************************************/
unsigned int nam_index_get
(
	tbl_index_t   *number,
	index_elem	  val	
)
{
	unsigned int status = 0;
	index_elem 	cur = 0;

	if( val > number->max) {
		status = 2; /*number out of range*/
	}
	else if((*number->table)[val] == INDEX_ALLOCATED) {
		status = 1; /* number allocated before*/
	}
	else {
		cur = number->free;
		if(val == cur) {
			number->free = (*number->table)[val];

		}
		else {
			while((cur != INDEX_END_OF_FREE) &&
					((*number->table)[cur] != val)){
				cur = (*number->table)[cur];
			}
			if(INDEX_END_OF_FREE == cur) {
				status = 3;
			}
			else {
				(*number->table)[cur] = (*number->table)[val];
			}
		}
		number->available--;
		(*number->table)[val] = INDEX_ALLOCATED;
	}

	return status;
}

unsigned int nam_index_free
(
	tbl_index_t *number, 
	index_elem 	val
)

{
	unsigned int status = 0;

	if(val > number->max) {
		status = 1; /* value out of range*/
	}
	else if((*number->table)[val] != INDEX_ALLOCATED) {
		status = 2; /* value not allocated before*/
	}
	else {
		number->available++;
		(*number->table)[val] = number->free;
		number->free = val;
	}

	return status;
}

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
)
{
	unsigned long task_id = 0UL;
	pthread_t thread = 0;
	pthread_attr_t thread_attr;
/**
 * 	Here if we use marvell ASIC driver,all thread creating operation adapted to osTaskCreate(...)
 *	This encapsulation is caused by marvell Chip address-mapped register access via PCI interface
 *	Problem(always zero pointer access exception) occur when we access registers via PCI in thread 
 *	created by pthread_create(...) APIs.
 *	If thread need access address-mapped register via PCI,this thread need add to task_list in ltaskLib.c
 *	Others if no register access operation, we should use pthread_create(...) APIs.
 */
	if(TRUE == accessChip) {
#ifdef DRV_LIB_CPSS
		osTaskCreate(
					name,			/* task name */
					200, 			/* task priority */
					0x2000, 		/* task stack size */
					entry_point, 	/*task entry */
					arglist,		/* parameters */
					&task_id);
		if(TRUE == needJoin) {
			osTaskGetThreadId(task_id,&thread);
			if(thread && (thread != (~0UL))) {
				pthread_join(thread,NULL);
			}
		}
#endif
#ifdef DRV_LIB_BCM
		pthread_attr_init(&thread_attr);
		pthread_create(&thread,&thread_attr,(void *)entry_point,arglist);
		if(TRUE == needJoin) {
			pthread_join(thread,NULL);
		}
	   
       /*thread = (pthread_t) sal_thread_create(name,0x2000,200,entry_point,arglist);
       nam_syslog_dbg("create thread %s get thread id %#x\n",name,thread);
       if(TRUE == needJoin){
          pthread_join(thread,NULL);
	   */
#endif
	}
	else if(FALSE == accessChip){		
		pthread_attr_init(&thread_attr);
		pthread_create(&thread,&thread_attr,(void *)entry_point,arglist);
		if(TRUE == needJoin) {
			pthread_join(thread,NULL);
		}
	}

	return ;
}

#ifdef __cplusplus
}
#endif
