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
* ws_portal_container.h
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
*$Source: /rdoc/AuteCS/cgic205/portal_multi_ins/ws_portal_container.h,v $
*$Author: chensheng $
*$Date: 2010/02/22 06:47:56 $
*$Revision: 1.2 $
*$State: Exp $
*
*
*/

#ifndef _WS_PORTAL_CONTAINER_H
#define _WS_PORTAL_CONTAINER_H
#include "cgic.h"
#include "ws_err.h"
#include "ws_ec.h"
#include "ws_usrinfo.h"
#include "ws_module_container.h"

#define WS_ERR_PORTAL_MEMORY			-1
#define WS_ERR_PORTAL_ILLEGAL_USER	-2
#define WS_ERR_PORTAL_PORINTER		-3

extern FILE *cgiOut;

#define	LIST_FILE_PATH	"../htdocs/text/"
#define LIST_FILE_PUBLIC	LIST_FILE_PATH"public.txt"
#define LIST_FILE_LOCAL		LIST_FILE_PATH"authentication.txt"
#define LIST_FILE_CONTROL	LIST_FILE_PATH"control.txt"

typedef struct{
	STModuleContainer *pstModuleContainer;
	struct list *lpublic;/*����public.txt�ļ�������ͷ*/
	struct list *llocal;/*������ģ����ַ����ļ�������ͷ*/
	struct list *lcon;
	char encry[BUF_LEN];
	char *username_encry;	         /*�洢���ܺ�ĵ�ǰ��½�û���*/
	int iUserGroup;	//Ϊ0ʱ��ʾ�ǹ���Ա��
	FILE *fp;

	int formProcessError;
} STPortalContainer;

typedef struct{
	char *cgi_name;
	char *name_key;
	int  user_group_mask;//Ϊ1��ʾֻ�й���Ա���ܿ�����label
}STLabelHelper;


int init_portal_container( STPortalContainer **pp_stSnmpdContainer );
int release_portal_container( STPortalContainer **pp_stSnmpdContainer );


#endif
