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
* Auth.h
*
*
* CREATOR:
* autelan.software.WirelessControl. team
*
* DESCRIPTION:
* asd module
*
*
*******************************************************************************/

#ifndef _AUTH_H
#define _AUTH_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifdef x86
#include <linux/if.h>
#endif
//#include <linux/wireless.h>
//#include <linux/netlink.h>

#ifdef xscale
#include <linux/if.h>
#endif
//#include "typedef.h"
#include "structure.h"
#include "config.h" 


/*����WAI������IOCTL������*/
#define P80211_IOCTL_SETWAPI_INFO		(0x8BE0 + 26)//0x8BFA

/*����STA״̬��IOCTL������*/
#define IEEE80211_IOCTL_SETMLME			(0x8BE0+6)//SIOCIWFIRSTPRIV

/*����WAPI������IOCTL��������,��ʾ����WAPI״̬*/
#define P80211_PACKET_WAPIFLAG			(u16)0x0001

/*����WAPI������IOCTL��������,��ʾ��װ��Կ*/

#define P80211_PACKET_SETKEY     			(u16)0x0003
/*����ͳ����Ϣ*/
#define P80211_PACKET_SETSTASTATS     			(u16)0x0006
/**/
//#define WLAN_DEVNAMELEN_MAX   	16
//#define MSG_BUFF_LEN          	4000
//#define SEND_BUF_LEN   		96
//#define MAX_RESEND_BUF		1000

#define WLAN_ADDR_LEN  	6		/*MAC��ַ����*/


/*���ջ������Ĵ�С*/
#define  FROM_MT_LEN 		3000		
#define  FROM_AS_LEN 		3000		
#define  FROM_CGI_LEN 	3000		

#define  MAX_AUTH_MT_SIMU  	32	/*������м���STA�ĸ���*/

#define KEY_LEN 			16  	
#define MULTI_KEY_LEN  	KEY_LEN


/*����ģʽ*/
#define AUTH_MODE 		(u8)0
#define PRE_AUTH_MODE 	(u8)1

/* ASUE״̬����*/
#define NO_AUTH		  					(u16)0		/*δ����*/
#define MT_WAITING_ACCESS_REQ 			(u16)1		/*�ȴ������������*/
#define MT_WAITING_AUTH_FROM_AS 		(u16)2		/*�ȴ�֤�������Ӧ*/
#define MT_WAITING_SESSION		 		(u16)3		/*�ȴ���ԿЭ��*/
#define MT_WAITING_GROUPING	 			(u16)4		/*�ȴ��鲥ͨ��*/
#define MT_SESSIONKEYING  				(u16)5		/*������ԿREKEY*/
#define MT_GROUPNOTICEING    				(u16)6		/*�鲥��ԿREKEY*/
#define MT_SESSIONGROUPING 				(u16)7		/*�������鲥ͨ���ٷ���̬��ԿЭ������*/
#define MT_WAITING_DYNAMIC_SESSION		(u16)8		/*������ԿREKEY�����еĵ�����Կ��Ӧ*/
#define MT_WAITING_DYNAMIC_GROUPING	(u16)9		/*�ȴ����鲥��ԿREKEY�������鲥��Կ��Ӧ*/
#define MT_AUTHENTICATED  				(u16)10 	/*����ɹ�*/
#define MT_PRE_AUTH_OVER  				(u16)11 	/**/


/*Driver ���͵���Ϣ������*/
#define PACKET_AUTH_TYPE					(u16)0x3a	/*����*/
#define PACKET_SESSIONKEY_TYPE			(u16)0x3b	/*USK Rekey*/
#define PACKET_GROUPKEY_TYPE			(u16)0x3c	/*MSK Rekey*/

/*80211Э����sta ״̬*/
#define	IEEE80211_MLME_ASSOC			1	/* associate station */
#define	IEEE80211_MLME_DISASSOC		2	/* disassociate station */
#define	IEEE80211_MLME_DEAUTH			3	/* deauthenticate station */
#define	IEEE80211_MLME_AUTHORIZE		4	/* authorize station */
#define	IEEE80211_MLME_UNAUTHORIZE	5	/* unauthorize station */

/*WAI���*/
#define MAX_AP_NODE 					20	/*Ԥ�������ڵ���*/
#define PRE_AUTH_SAVE_TIME			1800 /*Ԥ����ʱʱ��*/
#define TIMEOUT						1	/*WAI���鳬ʱʱ��*/
#define MAX_RESEND_COUNT 			3	/*WAI��������ش�����*/
#define GROUP_NOTICE_RESULT 			0xFF/*�鲥��Կͨ������ʶ*/
#define WAI_HLEN						12	/*WAI����ͷ����*/

                            	
/*����80211 Э���е�sta״̬ */
struct ieee80211req_mlme {
	u8	 	im_op;		/* operation to perform */
	u16	im_reason;	/* 802.11 reason code */
	u8 	im_macaddr[WLAN_ADDR_LEN];
};

/*Driver ���͵Ĺ��ڹ���STA����Ϣ*/								
struct asso_mt_t
{
//	struct nlmsghdr hdr;		/**/
	u16		type;		/*��Ϣ����*/
	u16		data_len;		/*��Ϣ����*/
	u8 		ap_mac[6];
	u8 		pad1[2];
	u8		mac[6];		/*STA ��MAC��ַ*/
	u8 		pad[2];
	u8		gsn[16];		/*�鲥�������*/
	u8		wie[256];		/*wapi��ϢԪ��*/
};
typedef struct asso_mt_t asso_mt;

/*�ط��������ṹ*/
struct _resendbuf_st{
	u16		len;
	void 		*data;
}__attribute__ ((packed));
typedef struct _resendbuf_st resendbuf_st;

/*  resendinfo_st->direction   */
#define SENDTO_STA	0
#define SENDTO_AS	1

/*�ط���������Ϣ�ṹ*/
struct _resendinfo_st {
	u32		send_time;		/*����ʱ��*/
	u16		cur_count;		/*���ʹ�������*/
	u16		timeout;			/*��ʱʱ��*/
	u16		direction;			/* ���ͷ���( STA or AS)*/
	u16		pad_value;		//align data
}__attribute__ ((packed));
typedef struct _resendinfo_st	 resendinfo_st;

/*��Կ�ṹ*/
struct _sta_key
{
	u8  uek[16]; 			/*����������Կ*/
	u8  uck[16];				/*����������У����Կ*/
	u8  kek[16];				/*��Կ������Կ*/
	u8  mck[16];			/*��Ϣ������Կ*/
	u8	  valid_flag;			/*��Կ�Ƿ���Ч�ı�־*/
	u8	  pad[3];
};

/*PSK��������Կ�����׼���AE��BKSA*/
struct psk_bksa
{
	u8		psk_password[32];
	u8		bk[16];
};

/*ASUE��BKSA*/
struct sta_bksa
{
	u8		bkid[16];
	u8		bk[16];
	u8		ae_auth_flag[32];		/*��һ��֤�����ļ����ʶ*/
	u16	bk_update;			/*��ʾ�Ƿ���BK���²���*/
	u16	akm_no;				/*��������Կ�����׼����*/
	u8		akm[4*MAX_AKM_NO];	/*��������Կ������*/
};//__attribute__ ((packed));;

/*ASUE��USKSA*/
struct sta_usksa
{
	u8				uskid;				/*Key ID 0,1֮�䷭ת*/
	u8				dynamic_key_used;	/**/
	u8				pad[2];
	struct _sta_key	usk[2];				/*��Կ*/
	time_t				setkeytime;	//	xm0623
	//u16	uskkey_no;				/*���������׼����*/
	//u8	unicast_key[4*MAX_UNICAST_NO]; /*���������׼�*/
};

/*AE��MSKSA*/
struct sta_msksa
{
	u8		mskid;		/*Key ID 0,1֮�䷭ת*/
	u8		pad[3];
	u8		msk[16];		/*�鲥��Կ*/
	time_t		setkeytime;	//	xm0623
};
typedef struct _wapi_stats_Entry{
	u32 	wapi_version;
	u8		controlled_port_status;
	u8		selected_unicast_cipher[4];
	
	u32		wai_sign_errors;
	u32		wai_hmac_errors;
	u32		wpi_mic_errors;		// D8.2.3
	u32		wpi_replay_counters;
	u32		wpi_decryptable_errors;
	
	u32		wai_auth_res_fail;
	u32		wai_discard;
	u32		wai_timeout;
	u32		wai_format_errors;
	u32		wai_cert_handshake_fail;
	u32		wai_unicast_handshake_fail;
	u32		wai_multi_handshake_fail;
	
}wapi_stats_Entry;

/*ap ��Ϣ��¼��*/
struct _apdata_info {
/*��¼�ϼ���wapid_interfaces�ṹ*/
	void *user_data;
	u8		wai_policy;				/*ap ��WAI����*/
	u8		pre_auth_policy;			/*ap ��Ԥ�������*/
	u16		mtu;					/*���߿���mtu*/

	u8		macaddr[6];				/*���߿���mac ��ַ*/
	u8		pad;

	u8 	gsn[16];					/*�鲥�������*/
	u32	gnonce[4];				/*�鲥��Կͨ���ʶ*/

	int 		g_ifr;					/**/
	
	int		group_No;				/*�鲥ͨ�����*/
	int		group_status;				/*�鲥ͨ��״̬*/
	u8 		wie_ae[255];				/*WIE*/
	u8	 	ae_nonce[32];				/*AE ����ս*/

	struct psk_bksa 	psk;				/*Ԥ������Կ����*/
	struct sta_msksa	msksa;			/*�鲥��Կ����*/
	wai_fixdata_alg 	sign_alg;			/*ǩ���㷨*/
	struct auth_sta_info_t *wapi_sta_info;		/**/
	char iface[16+1];
	int ap_debug;
	wapi_stats_Entry	wapi_mib_stats;
};
typedef struct _apdata_info apdata_info;


/*sta ��Ϣ��¼��*/
struct auth_sta_info_t
{
	u16		packet_type;			/*���������Ϣ����*/
	u16		status;				/*ASUE״̬*/

	u8			mac[6];			
	u8			auth_mode;			/*����ģʽ0����ģʽ,1Ԥ����ģʽ*/
	u8			auth_result;			/*������*/
	
	u32		timestamp;			/*ʱ��*/
	u32		pre_auth_save_time;	/*Ԥ����ʱ��*/

	resendinfo_st	sendinfo;				/*�ط�����������*/
	resendbuf_st	buf0;				/*�ط�������0*/
	resendbuf_st	buf1;				/*�ط�������1*/
	
	u8 		gsn[16];				/*�鲥�������*/
	u8			asue_nonce[32];		/*ASUE��ս*/
	u8			asue_cert_hash[32];	/*ASUE ֤��HASH*/
	u8			ae_nonce[32];			/*AE��ս*/
	u8			wie[256];				/*WAPI��ϢԪ��*/
	byte_data		asue_key_data;		/*ASUE��Կ����*/
	byte_data		ae_key_data;			/*AE��Կ����*/
	para_alg  		ecdh;				/*ECDH�㷨����*/
	wai_fixdata_flag flag;				/*WAI ��ʶ*/
	wai_fixdata_id	asue_id;				/*ASUE ���*/
	struct sta_bksa 	bksa;			/*STA BKSA*/
	struct sta_usksa 	usksa;			/*STA USKSA*/
	u16		ae_group_sc;			/*AE���͵�WAI �������*/
	u16		asue_group_sc;		/*ASUE���͵�WAI �������*/
	apdata_info *pap;
	wapi_stats_Entry	wapi_mib_stats;

	byte_data   serial_no;					//mahz add 2010/11/22
};
	
/*��Driver������Ϣ����Ϣ�ṹ*/
struct ioctl_drv
{
	u16  io_packet;
	struct  _iodata
	{
		u16 wDataLen;
		u8 pbData[96];
	}iodata;
}__attribute__ ((packed));

struct param_element_t
{
	int debug;
	char iface[20];
	char fileconfig[256];
};
struct wapid_interfaces {
	struct wapid_interfaces *next;
/*ÿ���豸��������Ϣ*/
	char *identity;
	size_t identity_len;
	u8 *ssid;
	size_t ssid_len;
	int	ssid_method;
	u8	wapi_method;
	u8	psk_type;
	u8 *password;
	size_t password_len;
/*���vap�ṹ���ڴ洢��֤��Ϣ��*/
	apdata_info *wapid; 
/*��¼�ϼ���circle�ṹ*/
	void *circle_save;
};
struct circle_data_wapi_old
{
/*�����ļ�*/
	char fileconfig[256];
	int card_id;
	int debug;
/*�׽�������*/
	int 		socket_open;			/*�׽��ӵ�״̬*/
	u16		g_eth_proto;				/*Э��*/
	int 		wai_raw_sk;				/*���ͽ���0x88b4֡���׽���*/
	int 		netlink_raw_sk ; 		/*����driver��Ϣ���׽���*/
	int 		cgi_udp_sk ;				/*����CGI��Ϣ���׽���*/
	int 		snmpd_udp_sk ;			/*����CGI��Ϣ���׽���*/
	
	int 		as_udp_sk ;				/*��ASUͨ�ŵ��׽���*/
	int 		ioctl_fd ;				/*ʹ��IOCTL ϵͳ����ʱ���׽���*/
	int		max_fd;
	fd_set 	read_fds;
	fd_set 	ip_recverr_fds;
/*֤�����֤����������*/
	struct sockaddr_in 	as_addr;			/*asu ��IP��ַ*/
	struct _apcert_info 	cert_info;			/*AE��֤��*/
	wai_fixdata_cert 	ae_cert;			/**/
	wai_fixdata_id  	ae_id;			/*AE�����*/
/*�����豸�ӿ�*/
	struct wapid_interfaces *vap_user;
	struct wapid_interfaces *vap_11a_user;
		u8		has_cert;					/*ap�Ƿ���֤��*/

};

struct asd_wapi
{
	struct asd_data *wasd_bk;
	int		as_udp_sk;
	struct sockaddr_in 	as_addr;			/*asu ��IP��ַ*/
	struct _apcert_info 	cert_info;			/*AE��֤��*/
	wai_fixdata_cert 	ae_cert;			/**/
	wai_fixdata_id  	ae_id;			/*AE�����*/
	struct wapid_interfaces *vap_user;
		u8		has_cert;					/*ap�Ƿ���֤��*/
		u8		multi_cert;					/*ap�Ƿ����ö�֤��*/

};


#endif
