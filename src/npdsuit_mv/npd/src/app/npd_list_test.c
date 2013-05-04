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
* npd_list_test.c
*
*
* CREATOR:
*       chenb@autelan.com
*
* DESCRIPTION:
*       test for npd list.
*
* DATE:
*       01/24/2008
*
*  FILE REVISION NUMBER:
*       $Revision: 1.3 $
*******************************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "util/npd_list.h"

static unsigned int test_i=0;

#define DEBUG_SHOW do { printf("**************************\n"); \
	printf("list 1:\n"); \
	list_for_each(i, &list1) {\
  		mylist_t *ops = list_entry(i, mylist_t, list); \
		printf("pf is %d\n", ops->pf); \
	} \
	printf("list 2:\n"); \
	list_for_each(i, &list2) { \
		mylist_t *ops = \
				list_entry(i, \
				mylist_t, list); \
				printf("pf is %d\n", ops->pf); \
	} \
} while(0);



#if 1

typedef struct __mylist {
	int pf;
	char name[FILENAME_MAX];
	struct list_head list;
} mylist_t;

#else

typedef struct __mylist {
	struct list_head list;
	int pf;
	char name[FILENAME_MAX];
} mylist_t;

#endif


static LIST_HEAD(list1); /* define 2 list */
static LIST_HEAD(list2);

//struct list_head name= { &(name), &(name) }; 

void dump_list(struct list_head *list) {
//	printf("list.pf %d\n",list.pf);
	printf("list %p\n",list);
	printf("prev %p\n",list->prev);
	printf("next %p\n",list->next);
	
}

int main(int argc, char *argv[])

{

	struct list_head *i;
	mylist_t elem1;
	mylist_t elem2;


	test_i = 1;
	
//	INIT_LIST_HEAD(&list1);
//	INIT_LIST_HEAD(&list2);
	elem1.pf = 2008;
	elem2.pf = 8002;

	dump_list(&list1);
	dump_list(&list2);

//	list_add(&elem1.list, &list1);
//	list_del(&elem1.list);


	DEBUG_SHOW;

	list_add(&elem1.list, &list1);

	list_add_tail(&elem2.list, &list1);

	dump_list(&list1);
	dump_list(&list2);
	DEBUG_SHOW;

	/* move element to another list */

	list_move_tail(&elem2.list, &list2);

	list_splice_init(&list1, &list2); /* merge */

	dump_list(&list1);
	dump_list(&list2);
	DEBUG_SHOW;

	return 0;

}
