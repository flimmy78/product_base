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
*$Source: /rdoc/accapi/nm/public/ws_list_container.h,v $
*$Author: shaojunwu $
*$Date: 2010/05/31 11:51:50 $
*$Revision: 1.1 $
*$State: Exp $
*$Modify:	$
*$Log $
*
*/
/*
list  container ��һ�������������������װ�����е����������

*/
#ifndef _LIST_CONTAINER_H
#define _LIST_CONTAINER_H


#define __DEBUG	0	//�ϴ�cvsʱ����Ϊ0

#if __DEBUG
//�������Ķ���,�õ�ʱ���Ǻܶ�,����ʱ�������.
#define debugprint(...) printf("%s  %d   ",__FILE__,__LINE__);printf(stderr,__VA_ARGS__)
#else
#define debugprint(...)
#endif


/******************************
���������������ͣ�һ��container��һ��element
�ڵ��õĵط�,ͳһʹ�� LEBase  LCBase ���������Ͷ���
*******************************/
typedef struct element_base_t {
	struct element_base_t *next;
	int (*release_self)( struct element_base_t *this );//�����ͷ��������Ķ�����malloc�Ŀռ�.ͬʱҪ�ͷ�thisָ��
}list_element_base, LEBase;
//����LEBase��LCBase��Ϊ�˷���ʹ��,Ҫ���������������øýṹʱ����ʹ���������.

typedef int (*REL_ELE_SELF_FUNC )( struct element_base_t *this );


typedef struct list_container_t {
	list_element_base ele_base;//����Ҫ��container��Ϊһ��element��������һ��container��ʱ��Ҫʹ�á����ڶ�ά����ͨ���Ĳ��������Ա��
	list_element_base *head;
	int (*release_self)( struct list_container_t *this );//�������ֻ��Ҫ�ͷ��������з���Ŀռ�,�����еĿռ��Լ��������������ͷ�.ͬʱҪ���ͷ�thisָ��
	int (*element_compare)( list_element_base *p1, list_element_base *p2 );//�������򣬻��߲��ҡ���Ŀǰ���ǵ�ϵͳ���ԣ�������ò��š�
	int (*proc_element)( list_element_base *this );//���ﴦ�����container�е�ÿһ��Ԫ��,���Բ������ݵ���element��ָ��.
}list_container_base, LCBase;

typedef int (*RELS_CONT_SELF_FUNC)( list_container_base *this );
typedef int (*ELE_CMP_FUNC)( list_element_base *p1, list_element_base *p2 );
typedef int (*PROC_ALL_ELE_FUNC)( list_element_base* this );




/****************************************
��������,�ⲿ��������ʱ����LC_ERR_EXTEND_BASE��ʼ
*****************************************/
#define RTN_OK			0
#define LC_ERR_NONE		-1
#define LC_ERR_MALLOC_FAILED	-2
#define LC_ERR_POINTER_NULL	-3
#define LC_ERR_NO_THIS_ELE	-4
#define LC_ERR_CB_NULL		-5
#define LC_ERR_THIS_IDX_NULL	-6
#define LC_ERR_INDEX		-7

#define LC_ERR_EXTEND_BASE	-1000


/*************************************
����һ��container  type��     ������    �����ͣ���ʼ����ɺ�,headָ��ָ��null.
**********************************/
#define NEW_CONTAINER(_p,_type,_release)	{\
	_p=(_type*)malloc(sizeof(_type));\
	if( NULL != _p )\
	{\
		list_container_base *p_tmp=(list_container_base *)_p;\
		memset( p_tmp,0,sizeof(_type));\
		p_tmp->release_self = (RELS_CONT_SELF_FUNC)_release;\
	}\
}

/***********************************
�ͷ�һ��container��
�������е�����Ԫ��
************************************/
#define DELETE_CONTAINER(p)	{\
	if( NULL != p )\
	{\
		list_container_base *p_container_base;\
		list_element_base *p_head,*p_temp;\
		p_container_base = (list_container_base *)p;\
		p_head = p_container_base->head;\
		/*debugprint("p_head = %x\n", p_head );*/\
		while( NULL != p_head )\
		{\
			/*debugprint("relese element in container p_head=%x\n", p_head);*/\
			p_temp = p_head;\
			p_head = p_head->next;\
			if( NULL != p_temp->release_self )\
			{\
				p_temp->release_self( p_temp );\
			}\
			else\
			{\
				free( p_temp );\
			}\
			p_temp = NULL;\
		}\
		if( NULL != p_container_base->release_self)\
		{\
			p_container_base->release_self( p_container_base );\
		}\
		else\
		{\
			free( p );\
		}\
	}\
}
	

/*************************************
����һ��element,���е����������������������
************************************/
#define NEW_ELEMENT(_p,_type,_release)	{\
	_p = (_type*)malloc(sizeof(_type));\
	if( NULL != _p )\
	{\
		list_element_base *p_tmp = (list_element_base *)_p;\
		memset( p_tmp, 0, sizeof(_type));\
		p_tmp->release_self = (REL_ELE_SELF_FUNC)_release;\
	}\
}
	
/**********************************
ɾ��һ��element,
************************************/
#define DELETE_ELEMENT(_p) {\
	if( NULL != _p )\
	{\
		list_element_base *p_e_base;\
		p_e_base = (list_element_base *)_p;\
		if( NULL !=  p_e_base->release_self )\
		{\
			p_e_base->release_self(p_e_base);\
		}\
		else\
		{\
			free(p_e_base);\
		}\
	}\
}







/************************************
���һ��Ԫ��
p_c   : container ��ʵ��
p_e   �� element��ʵ��
index ��     ��ӵ���λ�ã����Ϊ0�����ڵ�һ�������С��0������������index�����˵�ǰ�����������������
***********************************/
int add_element_f( list_container_base *p_c, list_element_base *p_e, int idx );
//�����������ĺ궨����Ϊ�˷����ҵ���,û��Ҫ��ÿ���ط�������ǿ������ת����.
#define add_element(p_c,p_e,idx)	add_element_f( (list_container_base *)p_c, (list_element_base *)p_e,idx )


/************************************
���һ��Ԫ��,����˳����뵽������С��������Ҫ������element_compare ������
p_c   : container ��ʵ��
p_e   �� element��ʵ��
index ��     ��ӵ���λ�ã����Ϊ0�����ڵ�һ�������С��0������������index�����˵�ǰ�����������������
***********************************/
int add_element_by_cmp_f( list_container_base *p_c, list_element_base *p_e );
#define add_element_by_cmp( p_c, p_e )	add_element_by_cmp_f( (list_container_base *)p_c, (list_element_base *)p_e )

/*********************************
���ݽڵ��ָ�� ɾ��һ���ڵ�, ����ƶ��ڵ��ָ�벻���ڣ��ͷ���<0  ɾ���ɹ�����0
***********************************/
int del_element_f( list_container_base *p_c, list_element_base *p_e );
#define del_element( p_c,p_e )		del_element_f( (list_container_base*)p_c, (list_element_base *)p_e )




/********************************
����һ���ڵ�,����������࣬��Ҫ������Ĳ�������ǿ������ת��,���������ţ��������С��0����ʾû���ҵ�
pp_get�õ�
*******************************/
int search_element_f( list_container_base *p_c, list_element_base *p_e_search, list_element_base **pp_e_get );
#define search_element(p_c,p_e_sharch,pp_e_get)	search_element_f((list_container_base *)p_c,(list_element_base *)p_e_search, (list_element_base **)pp_e_get )
/**********************************
�õ�container��Ԫ�صĸ���
**********************************/
int get_element_num_f( list_container_base *p_c );
#define get_element_num(p_c)	get_element_num_f((list_container_base *)p_c)

/**********************************
����Ԫ��ָ�룬�õ������š�
***********************************/
int get_idx_of_element_f( list_container_base *p_c, list_element_base *p_e );
#define get_idx_of_element(p_c,p_e)	get_idx_of_element_f((list_container_base *)p_c,(list_element_base *)p_e)



/*********************************
����Ԫ�صıȽϺ���.
************************************/
int set_element_cmp_func_f( list_container_base *p_c, ELE_CMP_FUNC ele_cmp_func );
#define set_element_cmp_func(p_c,ele_cmp_func)	set_element_cmp_func_f((list_container_base *)p_c,(ELE_CMP_FUNC)ele_cmp_func)


/*********************************************************************
���ô�������container��ÿ��Ԫ�صĺ����ص�����
********************************************************************/
int set_proc_all_func_f( list_container_base *p_c, PROC_ALL_ELE_FUNC proc_all_ele_func );
#define set_proc_all_func( p_c, func )	set_proc_all_func_f( (list_container_base *)p_c, (PROC_ALL_ELE_FUNC)func )


/***************************************************
�������е�Ԫ��, ���õĴ������� set_proc_all_func���õ� �ص�����.
******************************************************/
int proc_all_ele_in_cont_f( list_container_base *p_c );
#define proc_all_ele_in_cont( p_c )	proc_all_ele_in_cont_f( (list_container_base *)p_c )


/*****************************************************
������key���бȽϣ���ȵĶ���ŵ��ô�����
******************************************************/
int proc_eles_in_cont_by_key_f( list_container_base *p_c, list_element_base *p_e_key );
#define proc_eles_in_cont_by_key(p_c,p_e_key)	proc_eles_in_cont_by_key_f((list_container_base *)p_c,(list_element_base *)p_e_key )

/******************************************************
������Ԫ�ؽ�������
********************************************************/
int sort_elements_f( list_container_base *p_c );
#define sort_elements(p_c)	sort_elements_f( (list_container_base *)p_c )


/******************************************
�õ���һ��Ԫ��
��Ϊ�������µ���ʱ��û��nextָ�룬nextָ�����ڻ����ж���ģ������и�ר�ú����������һ����
������ʵ�ʲ�����ʱ�򣬶���Ҫ��next��ȡ����һ����ͳһʹ���������.
�ڵ����������,������ֵ��ֵ��һ���������ָ�����ʱ,����warning,��Ҫǿ��ת���������������,��ȥ��warning
*******************************************/

int get_next_element_f( list_element_base *p_e_cur, list_element_base **pp_e_base_get );
#define get_next_element(p_e_cur,pp_e_base_get)	get_next_element_f((list_element_base *)p_e_cur,(list_element_base **)pp_e_base_get)

/*****************************
�õ����һ��Ԫ��
ʹ��ע������: ����
********************************/
int get_last_element_f( list_container_base *p_c, list_element_base **pp_e_base_get );
#define get_last_element(p_c,pp_e_base_get)	get_last_element_f((list_container_base *)p_c,(list_element_base **)pp_e_base_get)

/***********************************
����index���õ�һ���ڵ�,���index < 0���߳�����Χ����NULL
************************************/
int get_element_by_idx_f( list_container_base *p_c, int idx, list_element_base **pp_e_base_get );
#define get_element_by_idx(p_c,idx,pp_e_base_get)	get_element_by_idx_f((list_container_base *)p_c,idx,(list_element_base **)pp_e_base_get);




#endif
