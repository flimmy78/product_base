/* cgicTempDir is the only setting you are likely to need
	to change in this file. */

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
* ws_ec.h
*
*
* CREATOR:
* autelan.software.Network Dep. team
* qiaojie@autelan.com
* shaojw@autelan.com
* tangsq@autelan.com
* zhouym@autelan.com
*
* DESCRIPTION:
*
*
*
*******************************************************************************/


#include<stdio.h>                        /*�������ͷ�ļ�*/
#include<string.h>                       /*�ַ�����ͷ�ļ�*/
#include<stdlib.h>                       /*��̬�洢����ͷ�ļ�*/

#ifndef _WS_EC_H
#define _WS_EC_H

#define LN 128                            /*��ŵ�����������ʱ���鳤��*/
#define File_Len 30                            /*�ϴ��ļ�������󳤶�*/

#define LONG_SORT 600
#define SHORT_SORT 120

#define STR_NOT_FOND	"string no found"			/*can also defined as  NULL*/




struct list                              /*����ṹ*/
{
//  char val[LN];                           /*��ű��������Ӧֵ*/
	struct list *next;                     /*ָ����һ�ڵ��ָ��*/
	char val[0];					/*��ű��������Ӧֵ,�������ĳ����Ǹ���ʵ�ʵĳ�����malloc�ģ�����ΪLN��������*/
};                                /*leָ��Ӣ�������ͷָ��,lcָ�����������ͷָ��*/

int equal(char *des,char *src);        /*�Ƚ��ַ���src���ַ���des��ǰsizeof(src)���ַ��Ƿ���ȣ���ȷ���1������ȷ���0*/
int load(struct list *l);               /*�ָ��ļ�fp�еı�������������l*/
char *search(struct list *l,char *v);    /*������l�в��ұ���v������ָ�������ֵ��ָ�룬û�ҵ�����NULL*/
void release(struct list *l);            /*��������l���ͷſռ�*/
char *ser_var(char *v);                /*���ļ�en-ch.txt�в��ұ���var��ֵ*/
char *getFPath(char *v);  /*���������ַ�����������Ӧ���ļ�·�����ɹ�����ָ���ļ�·���ַ�����ָ�룬ʧ�ܷ���NULL*/
char * readproductID();
int getfpath(const char *v, char *fpath );
struct list * get_chain_head(char *file_name); /*file_name��ʾ��Դ�ļ������֣����������ͷ*/





char *replace_url(char *strbuf, char *sstr, char *dstr);



#endif

