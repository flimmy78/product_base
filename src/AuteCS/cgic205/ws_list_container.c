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
* capture.c
*
*
* CREATOR:
* autelan.software.Network Dep. team
*
* DESCRIPTION:
*
*
*
*******************************************************************************/

/*
* Copyright (c) 2008,Autelan - AuteCS
* All rights reserved.
*
*$Source: /rdoc/AuteCS/cgic205/ws_list_container.c,v $
*$Author: chensheng $
*$Date: 2010/02/22 06:48:37 $
*$Revision: 1.3 $
*$State: Exp $
*$Modify:	$
*$Log $
*
*/
#include "ws_list_container.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"



/***************************************************************
*USEAGE:	add an element to container.
*Param: 	p_c->a pointer to a container. ������ָ��һ��list_container_base�������ָ��
		p_e->a pointer to a element. ������ָ��һ��list_element_base���������ָ��
		idx->position that you want to insert.�����ֵΪ����,���������ĩβ,�����idx�����˵�ǰ�ܸ���,�򷵻�ʧ��.0��ʾ���ڵ�һ��.
*Return:	0 ->  success
		other -> error
*Auther:	shao jun wu
*Date:		2009-3-9 14:15
*Modify:	(include modifyer,for what resease,date)
****************************************************************/
int add_element_f( list_container_base *p_c, list_element_base *p_e, int idx )
{
	list_element_base *p_e_insert_prev;
	
	if( NULL == p_c || NULL == p_e )
	{
		//debugprint("p_c or p_e is NULL!\n");
		return LC_ERR_POINTER_NULL;	
	}
	
	if( NULL == p_c->head )
	{
		p_c->head = p_e;
	}
	else if( 0 == idx  )
	{
		p_e->next = p_c->head;
		p_c->head = p_e;
	}
	else
	{
		if( idx < 0 )
		{
			get_last_element( p_c, &p_e_insert_prev );
		}
		else
		{
			get_element_by_idx( p_c, idx-1, &p_e_insert_prev );

		}
		
		if( NULL == p_e_insert_prev )//û���ҵ���ǰ����,���ش���.
		{
			return LC_ERR_THIS_IDX_NULL;
		}		
		
		p_e->next = p_e_insert_prev->next;
		p_e_insert_prev->next = p_e;
	}
	//debugprint("add item ok p_e = %x\n", p_e);
	return RTN_OK;
}


/***************************************************************
*USEAGE:	add an element to container.by cmp.���մ�element_compare�����ķ���С�����˳��Ԫ�ز��뵽����
*Param: 	p_c->a pointer to a container. ������ָ��һ��list_container_base�������ָ�롣
		p_e->a pointer to a element. ������ָ��һ��list_element_base���������ָ�롣
		idx->position that you want to insert.�����ֵΪ����,���������ĩβ,�����idx�����˵�ǰ�ܸ���,�򷵻�ʧ��.0��ʾ���ڵ�һ����
*Return:	0 ->  success
		other -> error
*Auther:	shao jun wu
*Date:		2009-3-9 16:51
*Modify:	(include modifyer,for what resease,date)
****************************************************************/
int add_element_by_cmp_f( list_container_base *p_c, list_element_base *p_e )
{
	list_element_base *p_e_insert_prev;

	if( NULL == p_c || NULL == p_e )
	{
		//debugprint("p_c or p_e is NULL!\n");
		return LC_ERR_POINTER_NULL;	
	}
	
	if( NULL == p_c->element_compare )
	{
		return LC_ERR_CB_NULL;
	}
	
	if( NULL == p_c->head )
	{
		p_c->head = p_e;
	}
	else if( p_c->element_compare( p_e , p_c->head ) < 0 )//���뵽head
	{
		p_e->next = p_c->head;
		p_c->head = p_e;
	}
	else
	{
		for( p_e_insert_prev = p_c->head; p_e_insert_prev->next; p_e_insert_prev = p_e_insert_prev->next )
		{
			if( p_c->element_compare( p_e_insert_prev, p_e ) <= 0 && 
				p_c->element_compare( p_e_insert_prev->next, p_e ) > 0 )
			{
				break;
			}			
		}
		p_e->next = p_e_insert_prev->next;
		p_e_insert_prev->next = p_e;		
	}
	return RTN_OK;
}


/***************************************************************
*USEAGE:	delete an element from container. you must no the element's pointer.
*Param: 	p_c->a pointer to a container. ������ָ��һ��list_container_base�������ָ��
		p_e->a pointer to a element. ������ָ��һ��list_element_base���������ָ��,������Ԫ�ػ�û����ӵ�container���򷵻�LC_ERR_NO_THIS_ELE.
*Return:	0 ->  success
		other -> error
*Auther:	shao jun wu
*Date:		2009-3-9 14:20
*Modify:	(include modifyer,for what resease,date)
****************************************************************/
int del_element_f( list_container_base *p_c, list_element_base *p_e )
{
	list_element_base *p_e_prev;
	list_element_base *p_e_for_free;
	
	if( NULL == p_c || NULL == p_e )
	{
		//debugprint("p_c or p_e is NULL!\n");
		return LC_ERR_POINTER_NULL;	
	}
	
	for( p_e_prev = p_c->head; (p_e_prev != NULL)&&(p_e_prev->next!=p_e); p_e_prev = p_e_prev->next);
	
	if( NULL==p_e_prev )
	{
		//debugprint("this element is no in container! \n");
		return LC_ERR_NO_THIS_ELE;	
	}
	
	//�Ƚ�element��container�жϿ�,���ͷ�ָ��
	p_e_for_free = p_e_prev->next;
	p_e_prev->next = p_e_for_free->next;
	
	DELETE_ELEMENT( p_e_for_free );
	
	return RTN_OK;	
}

/***************************************************************
*USEAGE:	�õ����һ��Ԫ��.�ⲿ��Ҫʹ�á�
*Param: 	p_c->a pointer to a container. ������ָ��һ��list_container_base�������ָ��
*Return:	porinter to an element base�� which is the last element in the container.
*Auther:	shao jun wu
*Date:		2009-3-9 14:23
*Modify:	(include modifyer,for what resease,date)
****************************************************************/
int get_last_element_f( list_container_base *p_c, list_element_base **pp_e_base_get )
{
	list_element_base *p_ret;
	if( NULL == p_c || NULL == pp_e_base_get )
	{
		return LC_ERR_POINTER_NULL;	
	}
		
	for( p_ret = p_c->head; (p_ret && p_ret->next!=NULL); p_ret=p_ret->next );
	
	*pp_e_base_get = p_ret;
	
	return RTN_OK;
}



/***************************************************************
*USEAGE:	���������ţ��õ�һ��Ԫ�ء�
*Param: 	p_c->a pointer to a container. ������ָ��һ��list_container_base�������ָ��
		idx->the index that you want to get.if it is lager than the cont of the element, it will return NULL.
*Return:	porinter to an element base�� which is the last element in the container.
*Auther:	shao jun wu
*Date:		2009-3-9 14:23
*Modify:	(include modifyer,for what resease,date)
****************************************************************/
int get_element_by_idx_f( list_container_base *p_c, int idx, list_element_base **pp_e_base_get  )
{
	int i;
	list_element_base *p_ret;
	
	if( NULL == p_c  || NULL == pp_e_base_get )
	{
		return LC_ERR_POINTER_NULL;
	}
	
	if( index < 0 )
	{
		return LC_ERR_INDEX;	
	}
			
	for( i=0,p_ret=p_c->head; (p_ret && i<idx); p_ret=p_ret->next,i++);
	
	*pp_e_base_get = p_ret;
	
	return RTN_OK;
}




/********************************
����һ���ڵ�,����������࣬��Ҫ������Ĳ�������ǿ������ת��,���������ţ��������С��0����ʾû���ҵ�
pp_get�õ�
ע��,�����p_e_search,���ܲ�����container�е�Ԫ��,���������Ҫ����cmp�������Ƚ���������Ĵ�С.
//��ʱ����,�Ȳ�ʵ��.
*******************************/
int search_element_f( list_container_base *p_c, list_element_base *p_e_search, list_element_base **pp_e_get )
{
	return 0;
}



/***************************************************************
*USEAGE:	�õ�container��Ԫ�صĸ���
*Param: 	p_c->a pointer to a container. ������ָ��һ��list_container_base�������ָ��
*Return:	the num of the elements in the container
*Auther:	shao jun wu
*Date:		2009-3-9 14:27
*Modify:	(include modifyer,for what resease,date)
****************************************************************/
int get_element_num_f( list_container_base *p_c )
{
	int iret;
	list_element_base *p_e_cur;
	
	for(iret=0,p_e_cur=p_c->head; p_e_cur; iret++,p_e_cur=p_e_cur->next );
	
	return iret;
}

/***************************************************************
*USEAGE:	�õ�ĳ��Ԫ�ص�����ֵ
*Param: 	p_c->a pointer to a container. ������ָ��һ��   list_container_base������  ��ָ��
		p_e->a pointer to an element. ������ָ��һ��  list_element_base������ ��ָ��
*Return:	the index of this element in the container
*Auther:	shao jun wu
*Date:		2009-3-9 14:27
*Modify:	(include modifyer,for what resease,date)
****************************************************************/
int get_idx_of_element_f( list_container_base *p_c, list_element_base *p_e )
{
	int iret;
	list_element_base *p_e_cur;
	
	for( iret=0,p_e_cur=p_c->head; p_e_cur!=p_e; iret++,p_e_cur=p_e_cur->next );
	
	return iret;
}


/***************************************************************
*USEAGE:	�õ�ĳ��Ԫ�ص���һ��Ԫ�أ��������ⲿ���á�
*Param: 	p_e_cur->a pointer to an element. ������ָ��һ��  list_element_base������ ��ָ��
*Return:	a pointer to an element.
*Auther:	shao jun wu
*Date:		2009-3-9 14:29
*Modify:	(include modifyer,for what resease,date)
****************************************************************/
int get_next_element_f( list_element_base *p_e_cur, list_element_base **pp_e_base_get )
{
	if( NULL == p_e_cur || NULL == pp_e_base_get )
	{
		return LC_ERR_POINTER_NULL;	
	}
	
	*pp_e_base_get = p_e_cur->next;
	return RTN_OK;
}

/***************************************************************
*USEAGE:	����element�Ƚϵ�callback����������Ҫ��container�е�Ԫ�ؽ��������ʱ����Ҫ���øñ�����
*Param: 	p_c->a pointer to a container. ������ָ��һ��   list_container_base������  ��ָ��
		ele_cmp_func-> function pointer .ELE_CMP_FUNC define as:
				int (*ELE_CMP_FUNC)( list_element_base *p1, list_element_base *p2 );
				so you should define a function like this:
				int ELE_CMP_FUNC( list_element_base *p1, list_element_base *p2 );
				������Ҫָ��Ϊ ��Ӧ���������ָ�롣
				ע�⣬���callback������������������ elementָ��.
*Return:	a pointer to an element.
*Auther:	shao jun wu
*Date:		2009-3-9 14:29
*Modify:	(include modifyer,for what resease,date)
****************************************************************/
int set_element_cmp_func_f( list_container_base *p_c, ELE_CMP_FUNC ele_cmp_func )
{
	if( NULL == p_c || NULL == ele_cmp_func  )
	{
		//debugprint("p_c  or  ele_cmp_func is NULL!\n");
		return LC_ERR_POINTER_NULL;
	}
	
	p_c->element_compare = ele_cmp_func ;
	
	return RTN_OK;	
};


/***************************************************************
*USEAGE:	���ô���ÿ��Ԫ�ص�callback������
*Param: 	p_c->a pointer to a container. ������ָ��һ��   list_container_base������  ��ָ��
		proc_all_ele_func-> function pointer .PROC_ALL_ELE_FUNC define as:
				typedef int (*PROC_ALL_ELE_FUNC)( list_element_base* this );
				so you should define a function like this:
				int PROC_ALL_ELE_FUNC( list_element_base* this );
				������Ҫָ��Ϊ   ��Ӧ��������   ��ָ�롣
				ע�⣬���callback�����Ĳ����� element�� ���� container��
*Return:	0
*Auther:	shao jun wu
*Date:		2009-3-9 14:29
*Modify:	(include modifyer,for what resease,date)
****************************************************************/
int set_proc_all_func_f( list_container_base *p_c, PROC_ALL_ELE_FUNC proc_all_ele_func )
{
	if( NULL == p_c || NULL == proc_all_ele_func )
	{
		//debugprint( "p_c  or proc_all_ele_func is NULL!\n" );
		return LC_ERR_POINTER_NULL;
	}
	
	p_c->proc_element = proc_all_ele_func;
	
	return RTN_OK;
}


/***************************************************************
*USEAGE:	���ô���cotainer��ÿ��Ԫ�أ�ʹ�õ�����һ������ָ����callback������ ���������Ҫ�����þ��ǽ���������ķ���ͳһ.
		���û������ proc_all_func�Ļص���������᷵��LC_ERR_CB_NULL����
*Param: 	p_c->a pointer to a container. ������ָ��һ��   list_container_base������  ��ָ��
*Return:	0 ->  success
		other -> error
*Auther:	shao jun wu
*Date:		2009-3-9 14:29
*Modify:	(include modifyer,for what resease,date)
****************************************************************/
int proc_all_ele_in_cont_f( list_container_base *p_c )
{
	list_element_base *p_e;
	int err=RTN_OK;
	
	if( NULL == p_c )
	{
		//debugprint( "p_c   is NULL!\n" );
		return 	LC_ERR_POINTER_NULL;
	}
	
	if( NULL == p_c->proc_element )
	{
		//debugprint( "didn't set call back func for    proc_element!\n" );
		return LC_ERR_CB_NULL;	
	}
	
	//����ÿ��Ԫ��,����ÿ��Ԫ�ض�����
	for( p_e = p_c->head; p_e; p_e=p_e->next )
	{
		err = p_c->proc_element( p_e );
		if( err != RTN_OK )
		{
			break;
		}
	}
	
	return err;
}


/***************************************************************
*USEAGE:	���Զ�������ĳЩ����Ķ��󣬲Ž��д���
		����������ڱ������е�element�ǣ���element��p_e_key���бȽϣ�����ȽϷ��ص�ֵΪ0���Ż����proc_all_func�������������
		���û������element_compare�ص���������᷵��LC_ERR_CB_NULL����
		���û������ proc_element�Ļص���������᷵��LC_ERR_CB_NULL����
*Param: 	p_c->a pointer to a container. ������ָ��һ��   list_container_base������  ��ָ��
*Return:	0 ->  success
		other -> error
*Auther:	shao jun wu
*Date:		2009-3-9 14:29
*Modify:	(include modifyer,for what resease,date)
****************************************************************/
int proc_eles_in_cont_by_key_f( list_container_base *p_c, list_element_base *p_e_key )
{
	list_element_base *p_e_cur;
	int err=RTN_OK;
	
	if( NULL == p_c )
	{
		//debugprint( "p_c   is NULL!\n" );
		return 	LC_ERR_POINTER_NULL;
	}
	
	if( NULL == p_c->proc_element || NULL == p_c->element_compare )
	{
		return LC_ERR_CB_NULL;
	}
	
	//����ÿ��Ԫ��,����ÿ��Ԫ����key���бȽϣ������ص���0ʱ���ŵ���proc����
	for( p_e_cur = p_c->head; p_e_cur; p_e_cur=p_e_cur->next )
	{
		if( 0 != p_c->element_compare(p_e_cur,p_e_key) )
		{
			continue;
		}
		err = p_c->proc_element( p_e_cur );
		if( err != RTN_OK )
		{
			break;
		}
	}
	
	return err;		
}



/***************************************************************
*USEAGE:	��container�е�Ԫ�ؽ�������
		���û������element_compare�ص���������᷵��LC_ERR_CB_NULL����

*Param: 	p_c->a pointer to a container. ������ָ��һ��   list_container_base������  ��ָ��
*Return:	0 ->  success
		other -> error
*Auther:	shao jun wu
*Date:		2009-3-9 14:29
*Modify:	(include modifyer,for what resease,date)
****************************************************************/
int sort_elements_f( list_container_base *p_c )
{
	list_element_base *p_e_cur;
	list_element_base *p_head;
	int iret=RTN_OK;
	
	if( NULL == p_c )
	{
		return 	LC_ERR_POINTER_NULL;
	}
	
	if( NULL == p_c->element_compare )
	{
		return LC_ERR_CB_NULL;	
	}
	
	//����
	//
	p_head = p_c->head;
	p_c->head = NULL;
	
	//ÿ�ν�head��������ȡ�������ŵ�container�У�ͬʱ��headָ����һ��,��ʼ��ÿ��ѭ��Ҫ����������һ���ģ�
	p_e_cur=p_head;
	while( p_e_cur )
	{
		p_head=p_head->next;
		p_e_cur->next=NULL;//����ǰ��Ԫ�ش�ԭ���������жϵ���
		iret = add_element_by_cmp( p_c, p_e_cur );
		if( iret != RTN_OK )
		{
			break;	
		}
		p_e_cur=p_head;
	}
	
	return iret;
}






























/*************************************************************************
�������ڸ�ģ��ĵ�������,ͬʱ������Ϊģ���һ��ʹ��˵��!

��Ҫ,ÿ���˶���Ҫ��������ô�õ�, ���������������.

ʹ�õĻ�������:
1�����壺���������ڶ����Լ���Ҫ��������ݽṹ��ʱ������Ӧ��base����ڽṹ��Ŀ�ͷ������ΪʲôҪ���ڿ�ͷ�������ڿ�ͷ���𣿣�
2�����壺container��elemnet�����release���������������Ľṹ����û����Ҫ�ͷŵ�ָ�룬Ҳ���Բ����壩
3�����壺����ÿ��Ԫ�صĺ���ָ�롣�����Ҫ���Զ���������
4��������ʹ��NEW_CONTAINER����һ��container���󡣴�����Ӧ��release���������û�оʹ���NULL
5��������ʹ��NEW_ELEMENT����element���󣬸�����Ҫ�������������Ҫ��ֵ�ĵط����и�ֵ������Ԫ����ӵ�container�С�
6��Ӧ�ã�����proc_all_ele_in_cont�������������ж���

7��Ӧ�ã������Ҫ���������element���󣬿�������һ��key��������cmp������������key�Ķ���cmpʱ����0��Ȼ�����proc_eles_in_cont_by_key
8��Ӧ�ã������Ҫ���򣬿�������cmp��������������д�������Ĺؼ��֣�������ʱ���Ὣ�ȽϷ��صĽ�С�Ķ�������ǰ�档
**************************************************************************/
#if __DEBUG
#if 0//�ϴ�cvsʱ,�������������Ϊ0,������ܳ��ֶ��main���������벻����	����Ϊ1���Ե������Ը�ģ��.
//�̳�element base �� container base, 
typedef struct {
	LEBase ebase;//�̳�element �Ļ���
	int aaa;
	char *bbb;
}ST_E_Test;


typedef struct {
	LCBase cbase; 
	int xxx;
	char yyy[42];
}ST_C_Test;




//������չ�Ĵ������.�������  LC_ERR_EXTEND_BASE
#define LCTEST_ERR_MENBER_BBB_NULL	LC_ERR_EXTEND_BASE-1




int e_test_release_self( struct element_base_t *this )//����element�� release����
{
	ST_E_Test *p_e_test;//�̳�������
	int err = RTN_OK;
	
	p_e_test = (ST_E_Test *)this;
	if( NULL == this )
	{
		return LC_ERR_POINTER_NULL;	
	}
	
	if( NULL == p_e_test->bbb )
	{
		err = LCTEST_ERR_MENBER_BBB_NULL; 	
	}
	else
	{
		free( p_e_test->bbb );
	}
	
	free( p_e_test );
		
	return err;
}


//����ÿ��Ԫ�صĴ�����.
int proc_element_test( ST_E_Test *this )
{
	if( NULL != this )
	{
		printf( "proc func bbb = %s         ", this->bbb );
		printf( " aaa = %d\n", this->aaa );	
	}
	return 0;
}

//����ÿ��Ԫ�صĴ�����2.ʹ��set_proc_all_func������.
int proc_element_test2( ST_E_Test *this )
{
	if( NULL != this )
	{
		printf( "second proc func bbb = %s         ", this->bbb );
		printf( "aaa = %d\n", this->aaa );
		//����,���Ҫ��container�е�����Ԫ�������Ļ�,��������������һ��tr.Ҳ����ÿһ������һ��.
	}
	return 0;		
}


//����ȽϺ�����������Ҫע�⣬������һ��������ʱ�����ܱ����õĺ����ж�Ϊ p_e1 < p_e2
int element_cmp_test( ST_E_Test *p_e1, ST_E_Test *p_e2 )
{
	int iRet;
	
	if( NULL == p_e1 || NULL == p_e2 )
	{
		return LC_ERR_POINTER_NULL;	
	}
	
	
	return (p_e1->aaa<p_e2->aaa)?0:1;	
}


int element_cmp_test2( ST_E_Test *p_e1, ST_E_Test *p_e2 )
{
	int iRet;
	
	if( NULL == p_e1 || NULL == p_e2 )
	{
		return LC_ERR_POINTER_NULL;	
	}
	
	return -(p_e1->aaa-p_e2->aaa);
}



int main() 
{
	ST_C_Test *p_container;//ע��������������һ��ST_C_Test�ı���,��֮ǰд��<���ԭ��>�е�shap��ʾ����һ��,Ϊʲô?��ʲô����?
	ST_E_Test *p_element;
	ST_E_Test *p_element_key;
	int i;
	
	//������Ҫ�����ע��,���ܽ�container��element�������,���ʹ�õ���NEW_CONTAINER,�ڶ�����������д��element����������.
	NEW_CONTAINER(p_container,ST_C_Test,NULL);//��Ϊ�ṹ����û��ָ����Ҫ�ͷ�,���������release�����ƶ�ΪNULL;
	if( NULL == p_container )
	{
		//debugprint( "err  for malloc container!\n" );
		return -1;
	}
	
	for( i=0;i<10;i++ )//һ��ѭ���������element�ĳ�ʼ��
	{
		char test[20];
		NEW_ELEMENT(p_element,ST_E_Test,e_test_release_self);//��Ϊ�ṹ������һ��ָ��,������Ҫ�ͷ�,���������ﶨ����e_test_release_self����.
		sprintf( test, "test bbb!  %d", i );
		
		//strdupΪbbbָ������˿ռ�,��Ҫfree���ͷ�, freeд����e_test_release_self ����ص���������.����,������container��ʱ������Զ��ĵ�����.
		p_element->bbb = strdup(test);
		p_element->aaa = (i%5)*10;
		
		add_element( p_container, p_element, -1 );//-1��Ԫ�ؼӵ����.
	}
	
	set_proc_all_func( p_container, proc_element_test );
	
	proc_all_ele_in_cont( p_container );
	
	
	del_element(p_container,p_element);//test,   ɾ�����һ���ڵ�.
	

	set_proc_all_func( p_container, proc_element_test2 );//�����˻ص�����.
		
	proc_all_ele_in_cont( p_container );
	
	//�����˱ȽϺ���������������key��ֻ��ӡ aaa<5�Ķ���
	NEW_ELEMENT( p_element_key, ST_E_Test, e_test_release_self );//�������û����ӵ�container�У���Ҫ��ʹ���������ͷš�
	p_element_key->aaa = 5;
	set_element_cmp_func( p_container, element_cmp_test );
	proc_eles_in_cont_by_key( p_container, p_element_key );
	
	
	
	//���ñȽϺ�������element����aaa�Ľ������У�Ȼ���ӡ����
	//��aaa���մӴ�С��˳�����С�
	//ǰ�����õıȽϺ����Ѿ����������µ�Ҫ���ˣ���������cmp������
	set_element_cmp_func( p_container, element_cmp_test2 );
	sort_elements( p_container );//����
	proc_all_ele_in_cont( p_container );//����ÿһ��Ԫ�ء�
	
	
	DELETE_CONTAINER( p_container );
	
	DELETE_ELEMENT( p_element_key );//���keyֻ���������˵ģ�û�з���container�У������ͷš�
	
	return 0;
}

#endif
#endif

