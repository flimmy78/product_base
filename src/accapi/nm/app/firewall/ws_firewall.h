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
* ws_firewall.h
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
#ifndef ATCS_FIREWALL
#define ATCS_FIREWALL 1

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define DORULE_PREFIX_CMD	system("sudo /usr/bin/setlinkstate.sh down");
#define DORULE_POSTFIX_CMD	system("sudo /usr/bin/setlinkstate.sh up");
//��������ת������
#define PEND_CVM_FILE	"/sys/module/cavium_ethernet/parameters/pend_cvm"

#define DORULE_ETHERNET_ON 	\
	do { \
		char cmd[256]; \
		sprintf(cmd, "sudo chmod a+rw %s; echo 0 > %s", PEND_CVM_FILE, PEND_CVM_FILE); \
		system(cmd); \
	} while (0);
	
#define DORULE_ETHERNET_OFF \
	do { \
		char cmd[256]; \
		sprintf(cmd, "sudo chmod a+rw %s; echo 1 > %s", PEND_CVM_FILE, PEND_CVM_FILE); \
		system(cmd); \
	} while (0);


#define NAT_UDP_TIMEOUT_MIN		30
#define NAT_UDP_TIMEOUT_MAX		1800
#define NAT_UDP_TIMEOUT_DEFAULT	1200

//����ı༭״̬����ɡ����޸ġ������
typedef enum{
FW_DONE,
FW_CHANGED,
FW_NEW,
}fwRuleStatus;

//����ƥ��ʱ�����������������ܾ�
typedef enum{
FW_ACCEPT,
FW_DROP,
FW_REJECT,
FW_TCPMSS
}fwRuleAct;

//Э������
typedef enum{
FW_PTCP=1,
FW_PUDP,
FW_PTCPANDUDP,
FW_PICMP,
}fwProtocl;

//�������;����
typedef enum{
FW_WALL,		//filter����
FW_DNAT,		//DNAT
FW_SNAT,		//SNAT
FW_INPUT	//input chain
}fwRuleType;

//ip��ַ�ı�����ʽ
typedef enum{
FW_MASQUERADE,
FW_IPSINGLE,
FW_IPHOST,
FW_IPMASK,
FW_IPNET,
FW_IPRANG,
}fwIPType;

//�˿ڵı�����ʽ
typedef enum{
FW_PTSINGLE=1,
FW_PTRANG,
FW_PTCOLLECT,
}fwPortType;


typedef struct fwRule{
fwRuleType	 type; 		//���������
unsigned int id;		//������(ͬһ���͵� idΨһ)
unsigned int ordernum;	//���򴴽�ʱ����ʷ˳���
char 		*name;		//��������
int		 enable;	//�Ƿ����
fwRuleStatus status;	//����״̬
char		*comment;	//����ע�ͣ���ע���ֹ������õ�ע��

char 		*ineth;		//���ݽ���ӿ�
char		*outeth;	//���������ӿ�
fwIPType	 srctype;
char		*srcadd;	//ԴIP��ַ
fwIPType	 dsttype;
char		*dstadd;	//Ŀ��IP��ַ

fwProtocl	 protocl;	//ƥ��ָ����Э��

fwPortType	 sptype;
char		*sport;		//Դ�˿�
fwPortType	 dptype;
char		*dport;		//Ŀ�Ķ˿�

/* connect limit */
char 		*connlimit;

fwRuleAct 	 act;		//������--�����ܾ�������
char		*tcpmss_var;
//��typeΪwallʱʹ��
char		*pkg_state;//
char		*string_filter;

//��type��WALLʱʹ��
fwIPType	 natiptype;
char 		*natipadd;	//��ַת���ĵ�ַ����
fwPortType	 natpttype;
char 		*natport;	//��ַת���Ķ˿ڲ���

//ָ����һ����
struct fwRule		*next;
}fwRule,*fwRulePtr;


//ȫ���������洢�����и�ʽ��һ���ṹ��
typedef struct{
fwRule *wall;
int	iWallTotalNum;
fwRule *snat;
int	iSNATTotalNum;
fwRule *dnat;
int	iDNATTotalNum;
fwRule *input;
int iInputTotalNum;
}fwRuleList;


void	firewall_free_ruleDate(fwRule *rule);

void	firewall_free_array(fwRule **array, unsigned int count);

//����iptables���� ����ֵ 0:�ɹ�	
int fwServiceStart();

//ֹͣiptables���� ����ֵ 0:�ɹ�	
int fwServiceStop();

//�������÷���ֵ 0:�ɹ�	
int fwServiceSaveConf();

//���������ʹ�õĹ��� ����ֵ 0:�ɹ�	
int fwServiceFlush();

#define firewall_chmod_conf_file() \
do { \
	system("sudo chmod a+rw /opt/services/conf/firewall_conf.conf >/dev/null 2>&1"); \
	system("sudo chmod a+rw /opt/services/status/iptables_status.status >/dev/null 2>&1"); \
	system("sudo chmod a+rw /opt/services/status/firewall_status.status >/dev/null 2>&1"); \
} while(0);

#ifdef __cplusplus
}
#endif
#endif


