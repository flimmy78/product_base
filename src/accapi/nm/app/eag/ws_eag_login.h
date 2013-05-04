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
* ws_eag_login.h
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

#ifndef WS_EAG_LOGIN
#define WS_EAG_LOGIN
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "ws_user_manage.h"





/*************************************
�����������������
************************************/
#define INT8	char
#define UINT8	unsigned char
#define INT16	short
#define UINT16	unsigned short
#define INT32	int
#define UINT32	unsigned int
#define INT64	long long
#define UINT64	unsigned long long

/***********************************************
�������ƶ�portal Э����ص��������ͣ�
�ο����й��ƶ�WEB�������̲��Թ淶��
	���й��ƶ�WLAN����ϵͳ�豸���ܺ����ܲ��Թ淶v5.3.1��
	
************************************************/

typedef struct {
	UINT8	version;
	UINT8	pkg_type;
	UINT8	auth_type;/*pap or chap*/
	UINT8	rsv;/*�����ֶ�*/
	UINT16	serial_no;/*SerialNo�ֶ�Ϊ���ĵ����кţ�����Ϊ 2 �ֽڣ���Portal Server������ɣ�Portal Server���뾡����֤��ͬ��֤���̵�SerialNo��һ��ʱ���ڲ����ظ�����ͬһ����֤���������б��ĵ�SerialNo��ͬ*/
	UINT16	req_id;/*ReqID�ֶγ���Ϊ 2 ���ֽڣ���AC�豸������ɣ�����ʹ����һ��ʱ����ReqID���ظ���*//*������뷨�����һ��id��*/
	UINT32	user_ip;
	UINT16	user_port;
	UINT8	err_code;
	UINT8	attr_num;
	UINT8	attr[0];/*ռλ���š�����ʵ�ʵ���Ҫ���øò��ֵĳ��ȡ�*/
}STPortalPkg;

/*�������ݰ�������*/
typedef enum {
	REQ_CHALLENGE=0x01,/*Portal Server ��AC�豸���͵�����Challenge����*/
	ACK_CHALLENGE,/*0x02AC�豸��Portal Server����Challenge���ĵ���Ӧ����*/
	REQ_AUTH,/*0x03Portal Server��AC�豸���͵�������֤����*/
	ACK_AUTH,/*0x04AC�豸��Portal Server������֤���ĵ���Ӧ����*/
	REQ_LOGOUT,/*0x05��ErrCode�ֶ�ֵΪ0x00����ʾ�˱�����Portal Server��AC�豸���͵������û����߱��ģ���ErrCode�ֶ�ֵΪ0x01����ʾ�ñ�����Portal Server���͵ĳ�ʱ���ģ���ԭ����Portal Server�����ĸ��������ڹ涨ʱ����û���յ���Ӧ���ġ�*/
	ACK_LOGOUT,/*0x06AC�豸��Portal Server�������߱��ĵ���Ӧ����*/
	AFF_ACK_AUTH,/*0x07Portal Server���յ�����֤�ɹ���Ӧ���ĵ�ȷ�ϱ���*/
	NTF_LOGOUT,/*0x08�û���ǿ������֪ͨ����*/
	REQ_INFO,/*0x09��Ϣѯ�ʱ���*/
	ACK_INFO/*0x0A��Ϣѯ�ʵ�Ӧ����*/
}PKG_TYPE;

/*������֤��Э��*/
typedef enum{
	AUTH_CHAP=0x00,
	AUTH_PAP=0x01
}AUTH_TYPE;


/*���������ֶε�����*/
typedef enum{
	ATTR_USERNAME=1, 
	ATTR_PASSWORD,
	ATTR_CHALLENGE,/*chap��ʽ��ħ���֡�*/
	ATTR_CHAPPASSWORD,/*����chap��ʽ���ܺ�����롣*/
	ATTR_PORT_ID=0x08
}ATTR_TYPE;


typedef struct{
	UINT8 attr_type;/*����û��ֱ���� ATTR_TYPE��Ϊ�������ͣ�����Ϊ�ڲ�ͬ�ı������ϡ�enum�Ĵ�С���ܲ�һ�������ƶ��ĵ��й涨attr_typeռ��1��byte;*/
	UINT8 attr_len;
	UINT8 attr_value[1];/*ʵ�ʴ�С��attr_len��������*/
}STPkgAttr;

/*error code*/
/*ack challeng����(ac�ظ���portal server��challenge����ı���)��errcode ���塡��pkg_type=2(ACK_CHALLENGE)*/
typedef enum{
	CHALLENGE_SUCCESS=0,
	CHALLENGE_REJECT,/*challenge ���ܾ�*/
	CHALLENGE_CONNECTED,/*�������Ѿ�����*/
	CHALLENGE_ONAUTH,/*��ǰportal server���û�������֤�����У����Ժ�����*/
	CHALLENGE_FAILED/*�û���challenge����ʧ��*/
}ACK_CHALLENGE_ERRCODE;

/*ack auth����(ac�ظ�portal server����֤����ı���)pkg_type = 4(ACK_AUTH)*/
typedef enum{
	PORTAL_AUTH_SUCCESS=0,
	PORTAL_AUTH_REJECT,
	PORTAL_AUTH_CONNECTED,
	PORTAL_AUTH_ONAUTH,/*��ǰportal server���û�������֤�����У����Ժ�����*/
	PORTAL_AUTH_FAILED/*�û���challenge����ʧ��*/
}ACK_AUTH_ERRCODE;

/*pkg_type = 6(ACK_LOGOUT)*/
typedef enum{
	EC_ACK_LOGOUT_SUCCESS,
	EC_ACK_LOGOUT_REJECT,
	EC_ACK_LOGOUT_FAILED
}ACK_LOGOUT_ERRCODE;



/*pkg_type 5 req logout    portal---AC �����û�����*/
typedef enum{
	EC_REQ_LOGOUT, 			/*�˱�����Portal Server����AC�豸���������߱���*/
	EC_REV_TIME_OUT			/*Portal Serverû���յ�AC�豸�����ĶԸ����������Ӧ���ģ�����ʱ��ʱ�䵽������ʱ��ʱ��Portal Server����AC�豸�ı���*/
}REQ_LOGOUT_ERRCODE;



#define MAX_EAG_LOGIN_NAME_LEN 256
#define MAX_EAG_LOGIN_PASS_LEN 16

#define STATUS_NOTAUTH	0
#define STATUS_AUTHED	1
#define STATUS_FAILED	2
	


typedef struct {
	char	 	usrName[MAX_EAG_LOGIN_NAME_LEN];
	char 		usrPass[MAX_EAG_LOGIN_PASS_LEN];
	int 		usrOperation;/*1 --login,2--logout*/
	UINT32		usrStatus;
}STUserInfo;


typedef struct {
	int fd;
	UINT32 protocal;
	STPortalPkg *pSendPkg;
	STPortalPkg *pRevPkg;
	
}STAuthProcess;

#define MAX_ATTR_VALUE_LEN 253


typedef struct MD5Context {
  UINT32 buf[4];
  UINT32 bits[2];
  unsigned char in[64];
}MD5_CTX;

#define MD5LEN 16
#define EAGINS_ID_BEGIN	1
#define MAX_EAGINS_NUM	16
#define EAGINS_UNIXSOCK_PREFIX	"/var/run/eagins_user"



/****************************************************/
int cgi_auth_init(STAuthProcess * stAuProc, int port);
int  init_auth_socket(STAuthProcess * stAuProc, int port );
int suc_connect_unix_sock();
int get_authType_from_eag(STUserManagePkg *pkg, int fd,int wait, STUserManagePkg **pprsp);

int setPkgUserIP(STPortalPkg * pkg, UINT32 ip_addr);
int send_pkg(int sock,STPortalPkg * pkg);
STPortalPkg *createPortalPkg(PKG_TYPE pkg_type);
unsigned int  destroyPortalPkg(STPortalPkg *pstPortalPkg);
int getPortalPkg( int fd, int wait, STPortalPkg **pp_portal_pkg );
int sendPortalPkg( int fd, int wait, int port, char * addr, STPortalPkg *pkg );
static unsigned int getAttrNum(STPortalPkg *pstPortalPkg);
int addAttr(STPortalPkg **pp_stPortalPkg, ATTR_TYPE attr_type, void *attr_value, unsigned int attr_value_len );
STPkgAttr *getAttrByAttrType(STPortalPkg *pstPortalPkg, ATTR_TYPE attr_type);

int setRequireID(STPortalPkg *pstPortalPkg, unsigned short req_id);
unsigned short getRequireID(STPortalPkg *pstPortalPkg);

unsigned int getPkgSize(STPortalPkg *pstPortalPkg);
int setAuthType(STPortalPkg *pstPortalPkg, AUTH_TYPE auth_type);
int closePkgSock(STAuthProcess * stAuProc);
unsigned char getErrCode(STPortalPkg *pstPortalPkg);



/*MD5 function*/
void MD5Init(struct MD5Context *context);
void MD5Update(struct MD5Context *context, unsigned char const *buf, size_t len);
void MD5Final(unsigned char digest[16], struct MD5Context *context);
void MD5Transform(UINT32 buf[4], UINT32 const in[16]);







#endif
