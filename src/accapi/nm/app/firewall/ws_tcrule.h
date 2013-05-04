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
* ws_tcrule.h
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
#ifndef WS_TCTULE_H
#define WS_TCTULE_H


#ifdef __cplusplus
extern "C"
{
#endif// end #ifdef __cplusplus

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpathInternals.h>
#include <sys/wait.h>
/*#include "ws_err.h"*/

#define TCRULES_XML_FILE "/opt/services/conf/traffic_conf.conf"
#define TCRULE_STATUS_FILE "/opt/services/status/traffic_status.status"
#define CMD_LEN		2048
#define TRANS_TO_CMD	0

//��������ģ�黹�����⣬�Ƚ�������Ϊ0
#define USE_P2P_TRAFFIC_CTROL	0
#define USE_RULE_TIME_CTROL		0


struct tcrule_offset_s {
    unsigned int ruleIndex;

    int     uplink_offset;
    int     downlink_offset;
};

//���һ�������Ե�����
/*
1����struct tcRule����Ӹ�����
2����addTCRuleToNode�д�������ӵ�����
3����tcParseXmlNode�д�������ӵ�����
4����tcFreeRule���ͷŸ�ָ��
5���ڲ��Գ����������Ӧ�Ĵ���
*/
typedef struct tcRule{
	char 	*name;		//�������ƣ���δ�õ�
	int		enable;	//�Ƿ����
	int		ruleIndex;
	char	*comment;	//����ע�ͣ���ע���ֹ������õ�ע�ͣ� ��δ�õ�

	char	*interface;//new
	char	*up_interface;//2008-10-30 10:14:11
	char	*protocol;//new
	
	char	*p2p_detail;//��δ�õ�
	
	//����ַ
	char	*addrtype;//new
	char	*addr_begin;//new
	char	*addr_end;//new
	char	*mode;//new ��addrtypeΪһ��rangeʱ�������������Ϊshare��noshare�ġ�
	
	//��������
	char	*uplink_speed;//new
	char	*downlink_speed;//new
	//p2p����
	int		useP2P;
	char	*p2p_uplink_speed;//new
	char	*p2p_downlink_speed;//new
	//ʱ�����
	int		time_begin;//new
	int 	time_end;//new
#if TRANS_TO_CMD	
	//����ת��Ϊ��Ӧ������
	char	*cmd_mark;
	char	*cmd_class;
	char	*cmd_filter;
#endif	
	char	*limit_speed;// ����

    struct tcrule_offset_s offset;
	
	struct tcRule *next;
}TCRule,*PTCRule;


int if_tcrule_file(char * fpath);
//����һ������
PTCRule	tcNewRule();

//�ͷ�һ������
void tcFreeRule(TCRule *rule);

//�ͷŹ�������
void tcFreeList( TCRule *root );

void tcFreeArray(TCRule **array, unsigned int count);

//�洢�����ļ�
int tcSaveDoc( char *path, TCRule *root );

//���������ļ�
PTCRule tcParseDoc( char *path );

//������ת��Ϊ����
int processRuleCmd( PTCRule ptcRule );

//�õ���ǰ��������

int getTcRuleNum( );

//�滻һ������
PTCRule replaceRule( PTCRule ptcRuleRoot, PTCRule ptcRuleNew, int index );

//���롢���һ������
PTCRule insertRule( PTCRule ptcRuleRoot, PTCRule ptcRuleNew, int index );

//ɾ��һ������
PTCRule deleteRule( PTCRule ptcRuleRoot, int index );

//�޸�һ�������˳��
PTCRule changeRuleIndex( PTCRule ptcRuleRoot, int iOldIndex, int iNewIndex );

//ִ�����й���
int tc_doAllRules( PTCRule ptcRuleRoot );

extern void tcrule_status_exist();


//����index�õ������еĹ���
PTCRule getTCRuleByIndex( PTCRule ptcRuleRoot, int iIndex );
#ifdef __cplusplus
}
#endif// end   #ifdef __cplusplus

#endif// end #ifndef WS_TCTULE_H

