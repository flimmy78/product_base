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
* ws_secondary_container.h
*
*
* CREATOR:
* autelan.software.Network Dep. team
* shaojw@autelan.com
*
* DESCRIPTION:
* function for web
*
*
***************************************************************************/
#ifndef _WS_SECONDARY_CONTAINER_H
#define _WS_SECONDARY_CONTAINER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ws_list_container.h"
#include "ws_sndr_cfg.h"//��������ļ������˱����ʱ����Ҫ����Щģ����뵽ҳ��

#define _DEBUG_SNDR_CONTAINER	0


#define MAX_LABLE_STR_LEN		32
#define MAX_LABLE_URL_LEN		128
#define MAX_LABLE_IMG_NAME_LEN	32
#define MAX_SUMMARY_TITLE_LEN	32
#define MAX_SUMMARY_KEY			32
#define MAX_SUMMARY_KEY_INFO	32
#define MAX_SUMMARY_KEY_VALUE	32


typedef struct secondary_module_label{
	char szlabelname[MAX_LABLE_STR_LEN];
	char szlabelurl[MAX_LABLE_URL_LEN];
	char szlabelimgname[MAX_LABLE_IMG_NAME_LEN];	
	char szlabelfont[MAX_LABLE_STR_LEN];
}t_sndr_label;


typedef struct secondary_module_summary{
	char szsummarytitle[MAX_SUMMARY_TITLE_LEN];
	char szsummarykey[MAX_SUMMARY_KEY];
	char szsummarykeyinfo[MAX_SUMMARY_KEY_INFO];
	char szsummarykeyvalue[MAX_SUMMARY_KEY_VALUE];
	char szsummaryfont[MAX_SUMMARY_KEY_VALUE];
}t_sndr_summary;

typedef struct secondary_module_item {
	LEBase			st_list_element_base;//�̳�����������Ԫ�ػ���,
	t_sndr_label	st_sndr_label;
	t_sndr_summary 	st_sndr_summary;
	int (*ex_show_call_back)( struct secondary_module_item *this );//�Ǳ��룬�����õ���
	FILE *fp;
}t_sndr_item,STSndrItem;

typedef int (*EX_SHOW_CALL_BACK)( struct secondary_module_item *this );


typedef int (*EX_SHOW_CALL_BACK_N)( struct secondary_module_container *this );
typedef int (*EX_SHOW_CALL_BACK_NZ)( struct secondary_module_container *this );


typedef struct secondary_module_container{
	LCBase			st_list_container_base;//�̳������������������ࡣ
	//����������ҳ������ʾ��صı�������������Ҫʹ��txt�ļ���ָ�롣�ȡ�
	FILE	*fp;//ͨ������ΪcgiOut.	
	void *lpublic;
	void *local;
	char encry[50];
	unsigned int pid;
	EX_SHOW_CALL_BACK_N callback_content;
	void *callback_param;
	EX_SHOW_CALL_BACK_NZ callback_content_z;
	void *callback_param_z;
	int flag;
	
}t_sndr_module_container,STSndrContainer;

int MC_setPageCallBack( STSndrContainer *me, EX_SHOW_CALL_BACK_N callback,void *callback_param);
int MC_setPageCallBack_z( STSndrContainer *me, EX_SHOW_CALL_BACK_NZ callback,void *callback_param_z);

typedef struct {
	void *public;
	void *local;
	char encry[50];
	int plotid;
}STPubInfoForItem;




//����һ������ҳ���item
STSndrItem *create_sndr_module_item();
//ɾ��һ������ҳ���item
int release_sndr_module_item( STSndrItem *this );
//����lable name
int SI_set_label_name( STSndrItem *this, char *name );
//����img
int SI_set_label_img( STSndrItem *this, char *img );
//����url
int SI_set_label_url( STSndrItem *this, char *url );
//����encry
int SI_set_label_encry( STSndrItem *this, char *encry );
//����font
int SI_set_label_font( STSndrItem *this, char *font );

//����summary title
int SI_set_summary_title( STSndrItem *this, char *title );
//����summary key
int SI_set_summary_key( STSndrItem *this, char *key );
//����summary key info
int SI_set_summary_keyinfo( STSndrItem *this, char *keyinfo );
//����summary key value
int SI_set_summary_keyvalue( STSndrItem *this, char *keyvalue );
//����summary key font
int SI_set_summary_font( STSndrItem *this, char *font );

//����callback  �Ǳ���
int SI_set_show_callback( STSndrItem *this, EX_SHOW_CALL_BACK callback );

//����һ������ҳ�������
STSndrContainer *create_sndr_module_container();

//����һ������ҳ������
int release_sndr_module_container( STSndrContainer *this);

//��� item��container
int SC_add_item( STSndrContainer *this, STSndrItem *p_item );

//��container ���Ϊҳ��
int SC_writeHtml( STSndrContainer *this);

//�Ƿ���ʾͼƬ��ǰ��Ҫ�޸�����Ч��
int if_show_sndr_icon();

//ÿ������ģ�鶼Ҫ�ṩһ�����½ӿڵĺ�������ɣ��������ݵĻ�á�
//lpublic  llocalΪ����txt��������������˵Ӧ���к����ڲ��Լ�ȥ���ļ��������ģ����ǵ����container�л����Щ�ļ���ȡ��������ֱ�Ӵ����²㣬��Լ����ִ��ʱ�䡣�����������������ʹ���������ģ�����Ҫ�����Լ�ȥ���ˡ�
//e.g. int fill_summary_data_valn( void *public, void *local, STSndrItem *p_item );
//e.g. int fill_summary_data_port( void *public, void *local, STSndrItem *p_item );
typedef int (*FILL_SUMMARY_DATA)( STPubInfoForItem *p_pubinfo, STSndrItem *p_item );

//������������ҳ��Ľṹ������
//���ｫhelper����Ϊһ���ṹ�壬����Ϊ�������ܻ�����helper��������ԡ�
typedef struct sc_create_helper {
	FILL_SUMMARY_DATA fill_data_api;
}STSCCreateHelper;

STSndrContainer *create_sndr_module_container_helper( STPubInfoForItem *p_pubinfo, STSCCreateHelper pstSCCreateHelper[], int num );

#endif
