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
* eag_statistics.h
*
*
* CREATOR:
* autelan.software.Network Dep. team
*
* DESCRIPTION:
* eag statistics
*
*
*******************************************************************************/

#ifndef _EAG_STATISTICS_H
#define _EAG_STATISTICS_H

#include <stdint.h>
#include "eag_def.h"

struct eag_bss_stat_data {
/*	������					����˵��		��Ӧ���� */

/*  ������Ϣ: */		
	uint8_t apmac[6];			//AP��MAC��ַ	
	uint8_t wlanid;				//WLANID	
	uint8_t radioid;			//RADIOID	
			
/* ͳ����Ϣ: */
	uint32_t online_user_num;		//�����û���
	uint32_t user_connected_total_time;	//���ߵ��û���ʱ��
	uint32_t user_connecting_total_time;	//���ߵ��û���ʱ��
	/* macauth */
	uint32_t macauth_online_user_num;
	uint32_t macauth_user_connected_total_time;
	uint32_t macauth_user_connecting_total_time;
#if 1
/* http��Ϣ: */	
	uint32_t http_redir_request_count;		//Portalҳ��������
	uint32_t http_redir_success_count;	//Portalҳ��ɹ�������
#endif
/* portal��Ϣ: */
	/* challenge: */
	uint32_t challenge_req_count;		//challenge�������		REQ_CHALLENGE
	  					//challenge�ظ�����	ACK_CHALLENGE
	uint32_t challenge_ack_0_count;		//����Challenge�ɹ�		ErrCode��0
	uint32_t challenge_ack_1_count;		//����Challenge���ܾ�	ErrCode��1
	uint32_t challenge_ack_2_count;		//�������ѽ���		ErrCode��2
	uint32_t challenge_ack_3_count;		//�û�������֤������	ErrCode��3
	uint32_t challenge_ack_4_count;		//����Challengeʧ��		ErrCode��4

	/* auth */
	uint32_t auth_req_count;		//��֤�������		REQ_AUTH
						//auth�ظ�����		ACK_AUTH
	uint32_t auth_ack_0_count;		//��֤����ɹ�		ErrCode��0
	uint32_t auth_ack_1_count;		//��֤���󱻾ܾ�		ErrCode��1
	uint32_t auth_ack_2_count;		//�������ѽ���		ErrCode��2
	uint32_t auth_ack_3_count;		//�û�������֤������	ErrCode��3
	uint32_t auth_ack_4_count;		//��֤ʧ��		ErrCode��4
	/* macauth */
	uint32_t macauth_req_count;		//��֤�������		REQ_AUTH
						//auth�ظ�����		ACK_AUTH
	uint32_t macauth_ack_0_count;		//��֤����ɹ�		ErrCode��0
	uint32_t macauth_ack_1_count;		//��֤���󱻾ܾ�		ErrCode��1
	uint32_t macauth_ack_2_count;		//�������ѽ���		ErrCode��2
	uint32_t macauth_ack_3_count;		//�û�������֤������	ErrCode��3
	uint32_t macauth_ack_4_count;		//��֤ʧ��		ErrCode��4
	#if 0
	/* logout */
						//�������ߴ���		REQ_LOGOUT
	uint32_t logout_req_0_count;		//�������ߴ���		ErrCode��0
	uint32_t logout_req_1_count;		//�������ߴ���		ErrCode��1
						//���߻ظ�����		ACK_LOGOUT
	uint32_t logout_ack_0_count;		//�û����߳ɹ�		ErrCode��0
	uint32_t logout_ack_1_count;		//�û����߱��ܾ�	ErrCode��1
	uint32_t logout_ack_2_count;		//�û�����ʧ��		ErrCode��2
	
	/* ntf logout */
	uint32_t ntf_logout_count;		//�쳣���Ĵ���		NTF_LOGOUT
	#endif
	/* logoff count */
	uint32_t normal_logoff_count;		//�������ߴ���
	uint32_t abnormal_logoff_count;		//�쳣���ߴ���
	uint32_t macauth_abnormal_logoff_count;
	/* ����ͳ�� */
	uint32_t challenge_timeout_count;	//Challenge��ʱ����
	uint32_t challenge_busy_count;		//Challengeæµ����	
	
	uint32_t req_auth_password_missing_count;//�û�������Ϣȱʧ����
	uint32_t req_auth_unknown_type_count;	//δ֪���ʹ������
	uint32_t ack_auth_busy_count;		//authæµ����
	uint32_t auth_disorder_count;		//auth������������
	
/* radius��Ϣ: */
	/* access */
	uint32_t access_request_count;		//Radius����֤�������	Access-Request
	uint32_t access_request_retry_count;	//Radius����֤�����ش�����
	uint32_t access_request_timeout_count;	//Radius����֤����ʱ����
	uint32_t access_accept_count;		//Radius����֤ͨ������	Access-Accept
	uint32_t access_reject_count;		//Radius����֤�ܾ�����	Access-Reject
	
		
	/* accounting */
	uint32_t acct_request_start_count;	//�Ʒѿ�ʼ�������		Accounting-Request(Start)
	uint32_t acct_request_start_retry_count;
	uint32_t acct_response_start_count;	//�Ʒѿ�ʼ������Ӧ����	Accounting-Response(Start)			
	
	uint32_t acct_request_update_count;	//�ƷѸ��´���		Accounting-Request(Interim-Update)
	uint32_t acct_request_update_retry_count;
	uint32_t acct_response_update_count;	//�ƷѸ�����Ӧ����		Accounting-Response(Interim-Update)
	
	uint32_t acct_request_stop_count;	//�Ʒ�ֹͣ�������		Accounting-Request(stop)
	uint32_t acct_request_stop_retry_count;
	uint32_t acct_response_stop_count;	//�Ʒ�ֹͣ������Ӧ����	Accounting-Response(stop)
};

struct eag_ap_stat_data {
/*	������					����˵��		��Ӧ���� */

/*  ������Ϣ: */		
	uint8_t apmac[6];			//AP��MAC��ַ
			
/* ͳ����Ϣ: */
	uint32_t online_user_num;		//�����û���
	uint32_t user_connected_total_time;	//���ߵ��û���ʱ��
	uint32_t user_connecting_total_time;	//���ߵ��û���ʱ��
	/* macauth */
	uint32_t macauth_online_user_num;
	uint32_t macauth_user_connected_total_time;
	uint32_t macauth_user_connecting_total_time;
#if 1
/* http��Ϣ: */	
	uint32_t http_redir_request_count;		//Portalҳ��������
	uint32_t http_redir_success_count;	//Portalҳ��ɹ�������
#endif
/* portal��Ϣ: */
	/* challenge: */
	uint32_t challenge_req_count;		//challenge�������		REQ_CHALLENGE
	  					//challenge�ظ�����	ACK_CHALLENGE
	uint32_t challenge_ack_0_count;		//����Challenge�ɹ�		ErrCode��0
	uint32_t challenge_ack_1_count;		//����Challenge���ܾ�	ErrCode��1
	uint32_t challenge_ack_2_count;		//�������ѽ���		ErrCode��2
	uint32_t challenge_ack_3_count;		//�û�������֤������	ErrCode��3
	uint32_t challenge_ack_4_count;		//����Challengeʧ��		ErrCode��4

	/* auth */
	uint32_t auth_req_count;		//��֤�������		REQ_AUTH
						//auth�ظ�����		ACK_AUTH
	uint32_t auth_ack_0_count;		//��֤����ɹ�		ErrCode��0
	uint32_t auth_ack_1_count;		//��֤���󱻾ܾ�		ErrCode��1
	uint32_t auth_ack_2_count;		//�������ѽ���		ErrCode��2
	uint32_t auth_ack_3_count;		//�û�������֤������	ErrCode��3
	uint32_t auth_ack_4_count;		//��֤ʧ��		ErrCode��4
	/* macauth */
	uint32_t macauth_req_count;		//��֤�������		REQ_AUTH
						//auth�ظ�����		ACK_AUTH
	uint32_t macauth_ack_0_count;		//��֤����ɹ�		ErrCode��0
	uint32_t macauth_ack_1_count;		//��֤���󱻾ܾ�		ErrCode��1
	uint32_t macauth_ack_2_count;		//�������ѽ���		ErrCode��2
	uint32_t macauth_ack_3_count;		//�û�������֤������	ErrCode��3
	uint32_t macauth_ack_4_count;		//��֤ʧ��		ErrCode��4
	#if 0
	/* logout */
						//�������ߴ���		REQ_LOGOUT
	uint32_t logout_req_0_count;		//�������ߴ���		ErrCode��0
	uint32_t logout_req_1_count;		//�������ߴ���		ErrCode��1
						//���߻ظ�����		ACK_LOGOUT
	uint32_t logout_ack_0_count;		//�û����߳ɹ�		ErrCode��0
	uint32_t logout_ack_1_count;		//�û����߱��ܾ�	ErrCode��1
	uint32_t logout_ack_2_count;		//�û�����ʧ��		ErrCode��2
	
	/* ntf logout */
	uint32_t ntf_logout_count;		//�쳣���Ĵ���		NTF_LOGOUT
	#endif
	/* logoff count */
	uint32_t normal_logoff_count;		//�������ߴ���
	uint32_t abnormal_logoff_count;		//�쳣���ߴ���
	uint32_t macauth_abnormal_logoff_count;
	/* ����ͳ�� */
	uint32_t challenge_timeout_count;	//Challenge��ʱ����
	uint32_t challenge_busy_count;		//Challengeæµ����	
	
	uint32_t req_auth_password_missing_count;//�û�������Ϣȱʧ����
	uint32_t req_auth_unknown_type_count;	//δ֪���ʹ������
	uint32_t ack_auth_busy_count;		//authæµ����
	uint32_t auth_disorder_count;		//auth������������
	
/* radius��Ϣ: */
	/* access */
	uint32_t access_request_count;		//Radius����֤�������	Access-Request
	uint32_t access_request_retry_count;	//Radius����֤�����ش�����
	uint32_t access_request_timeout_count;	//Radius����֤����ʱ����
	uint32_t access_accept_count;		//Radius����֤ͨ������	Access-Accept
	uint32_t access_reject_count;		//Radius����֤�ܾ�����	Access-Reject
	
		
	/* accounting */
	uint32_t acct_request_start_count;	//�Ʒѿ�ʼ�������		Accounting-Request(Start)
	uint32_t acct_request_start_retry_count;
	uint32_t acct_response_start_count;	//�Ʒѿ�ʼ������Ӧ����	Accounting-Response(Start)			
	
	uint32_t acct_request_update_count;	//�ƷѸ��´���		Accounting-Request(Interim-Update)
	uint32_t acct_request_update_retry_count;
	uint32_t acct_response_update_count;	//�ƷѸ�����Ӧ����		Accounting-Response(Interim-Update)
	
	uint32_t acct_request_stop_count;	//�Ʒ�ֹͣ�������		Accounting-Request(stop)
	uint32_t acct_request_stop_retry_count;
	uint32_t acct_response_stop_count;	//�Ʒ�ֹͣ������Ӧ����	Accounting-Response(stop)	
};


struct eag_stat_data {
	uint32_t ap_num;
	uint32_t online_user_num;
	uint32_t user_connect_total_time;
	/* macauth */
	uint32_t macauth_online_user_num;
	uint32_t macauth_user_connect_total_time;	
#if 1
/* http��Ϣ: */	
	uint32_t http_redir_request_count;		//Portalҳ��������
	uint32_t http_redir_success_count;	//Portalҳ��ɹ�������
#endif
/* portal��Ϣ: */
	/* challenge: */
	uint32_t challenge_req_count;		//challenge�������		REQ_CHALLENGE
	  					//challenge�ظ�����	ACK_CHALLENGE
	uint32_t challenge_ack_0_count;		//����Challenge�ɹ�		ErrCode��0
	uint32_t challenge_ack_1_count;		//����Challenge���ܾ�	ErrCode��1
	uint32_t challenge_ack_2_count;		//�������ѽ���		ErrCode��2
	uint32_t challenge_ack_3_count;		//�û�������֤������	ErrCode��3
	uint32_t challenge_ack_4_count;		//����Challengeʧ��		ErrCode��4

	/* auth */
	uint32_t auth_req_count;		//��֤�������		REQ_AUTH
						//auth�ظ�����		ACK_AUTH
	uint32_t auth_ack_0_count;		//��֤����ɹ�		ErrCode��0
	uint32_t auth_ack_1_count;		//��֤���󱻾ܾ�		ErrCode��1
	uint32_t auth_ack_2_count;		//�������ѽ���		ErrCode��2
	uint32_t auth_ack_3_count;		//�û�������֤������	ErrCode��3
	uint32_t auth_ack_4_count;		//��֤ʧ��		ErrCode��4
	/* macauth */
	uint32_t macauth_req_count;		//��֤�������		REQ_AUTH
						//auth�ظ�����		ACK_AUTH
	uint32_t macauth_ack_0_count;		//��֤����ɹ�		ErrCode��0
	uint32_t macauth_ack_1_count;		//��֤���󱻾ܾ�		ErrCode��1
	uint32_t macauth_ack_2_count;		//�������ѽ���		ErrCode��2
	uint32_t macauth_ack_3_count;		//�û�������֤������	ErrCode��3
	uint32_t macauth_ack_4_count;		//��֤ʧ��		ErrCode��4
	#if 0
	/* logout */
						//�������ߴ���		REQ_LOGOUT
	uint32_t logout_req_0_count;		//�������ߴ���		ErrCode��0
	uint32_t logout_req_1_count;		//�������ߴ���		ErrCode��1
						//���߻ظ�����		ACK_LOGOUT
	uint32_t logout_ack_0_count;		//�û����߳ɹ�		ErrCode��0
	uint32_t logout_ack_1_count;		//�û����߱��ܾ�	ErrCode��1
	uint32_t logout_ack_2_count;		//�û�����ʧ��		ErrCode��2
	
	/* ntf logout */
	uint32_t ntf_logout_count;		//�쳣���Ĵ���		NTF_LOGOUT
	#endif
	/* logoff count */
	uint32_t normal_logoff_count;		//�������ߴ���
	uint32_t abnormal_logoff_count;		//�쳣���ߴ���
	uint32_t macauth_abnormal_logoff_count;
	/* ����ͳ�� */
	uint32_t challenge_timeout_count;	//Challenge��ʱ����
	uint32_t challenge_busy_count;		//Challengeæµ����	
	
	uint32_t req_auth_password_missing_count;//�û�������Ϣȱʧ����
	uint32_t req_auth_unknown_type_count;	//δ֪���ʹ������
	uint32_t ack_auth_busy_count;		//authæµ����
	uint32_t auth_disorder_count;		//auth������������
	
/* radius��Ϣ: */
	/* access */
	uint32_t access_request_count;		//Radius����֤�������	Access-Request
	uint32_t access_request_retry_count;	//Radius����֤�����ش�����
	uint32_t access_request_timeout_count;	//Radius����֤����ʱ����
	uint32_t access_accept_count;		//Radius����֤ͨ������	Access-Accept
	uint32_t access_reject_count;		//Radius����֤�ܾ�����	Access-Reject
	
		
	/* accounting */
	uint32_t acct_request_start_count;	//�Ʒѿ�ʼ�������		Accounting-Request(Start)
	uint32_t acct_request_start_retry_count;
	uint32_t acct_response_start_count;	//�Ʒѿ�ʼ������Ӧ����	Accounting-Response(Start)			
	
	uint32_t acct_request_update_count;	//�ƷѸ��´���		Accounting-Request(Interim-Update)
	uint32_t acct_request_update_retry_count;
	uint32_t acct_response_update_count;	//�ƷѸ�����Ӧ����		Accounting-Response(Interim-Update)
	
	uint32_t acct_request_stop_count;	//�Ʒ�ֹͣ�������		Accounting-Request(stop)
	uint32_t acct_request_stop_retry_count;
	uint32_t acct_response_stop_count;	//�Ʒ�ֹͣ������Ӧ����	Accounting-Response(stop)	
};



struct eag_ap_statistics {
	/* ap������Ϣ */
	uint8_t ap_mac[6];	

	/* ap���ݽṹ */
	struct list_head ap_node;		//ap����ڵ�
	struct hlist_node ap_hnode;		//��ϣ��ڵ�
	
	/* bss�����Ϣ */
	struct list_head bss_head;		//bss����
	int bss_num;				//bss�������ȶ��壬��ʹ��

	/* ������Ϣ */
	eag_statistics_t *eagstat;
};
	
struct eag_bss_statistics {
	/* bss���ݽṹ */
	struct list_head bss_node;		//bss����
	
	/* bss������Ϣ */
	/* bssͳ����Ϣ */
	struct eag_bss_stat_data data;		//ͳ����Ϣ
	
	/* ������Ϣ */
	struct eag_ap_statistics *ap_stat;	//����ap
	eag_statistics_t *eagstat;
};

typedef enum {
	BSS_ONLINE_USER_NUM = 1,
	BSS_USER_CONNECTED_TOTAL_TIME,
	BSS_USER_CONNECTING_TOTAL_TIME,
	/* MACAUTH */
	BSS_MACAUTH_ONLINE_USER_NUM,
	BSS_MACAUTH_USER_CONNECTED_TOTAL_TIME,
	BSS_MACAUTH_USER_CONNECTING_TOTAL_TIME,
#if 1		
/* http */	
	BSS_HTTP_REDIR_REQ_COUNT,
	BSS_HTTP_REDIR_SUCCESS_COUNT,
#endif	
/* PORTAL */	
	/* CHALLENGE: */
	BSS_CHALLENGE_REQ_COUNT,
	BSS_CHALLENGE_ACK_0_COUNT,		
	BSS_CHALLENGE_ACK_1_COUNT,		
	BSS_CHALLENGE_ACK_2_COUNT,		
	BSS_CHALLENGE_ACK_3_COUNT,		
	BSS_CHALLENGE_ACK_4_COUNT,		
	/* AUTH */
	BSS_AUTH_REQ_COUNT,
	BSS_AUTH_ACK_0_COUNT,		
	BSS_AUTH_ACK_1_COUNT,		
	BSS_AUTH_ACK_2_COUNT,		
	BSS_AUTH_ACK_3_COUNT,		
	BSS_AUTH_ACK_4_COUNT,		
    /* MACAUTH */
	BSS_MACAUTH_REQ_COUNT,
	BSS_MACAUTH_ACK_0_COUNT,		
	BSS_MACAUTH_ACK_1_COUNT,		
	BSS_MACAUTH_ACK_2_COUNT,		
	BSS_MACAUTH_ACK_3_COUNT,		
	BSS_MACAUTH_ACK_4_COUNT,
	#if 0
	/* LOGOUT */
	BSS_LOGOUT_REQ_0_COUNT,		
	BSS_LOGOUT_REQ_1_COUNT,
	BSS_LOGOUT_ACK_0_COUNT,	
	BSS_LOGOUT_ACK_1_COUNT,	
	BSS_LOGOUT_ACK_2_COUNT,	
	/* NTF LOGOUT */
	BSS_NTF_LOGOUT_COUNT,	
	#endif
	/* LOGOFF COUNT */
	BSS_NORMAL_LOGOFF_COUNT,		
	BSS_ABNORMAL_LOGOFF_COUNT,		
	BSS_MACAUTH_ABNORMAL_LOGOFF_COUNT,
	/* ����ͳ�� */
	BSS_CHALLENGE_TIMEOUT_COUNT,		
	BSS_CHALLENGE_BUSY_COUNT,
	BSS_REQ_AUTH_PASSWORD_MISSING_COUNT,	
	BSS_REQ_AUTH_UNKNOWN_TYPE_COUNT,
	BSS_ACK_AUTH_BUSY_COUNT,
	BSS_AUTH_DISORDER_COUNT,	
/* RADIUS */	
	/* ACCESS */
	BSS_ACCESS_REQUEST_COUNT,		
	BSS_ACCESS_REQUEST_RETRY_COUNT,	
	BSS_ACCESS_REQUEST_TIMEOUT_COUNT,	
	BSS_ACCESS_ACCEPT_COUNT,
	BSS_ACCESS_REJECT_COUNT,
	/* ACCOUNTING */
	BSS_ACCT_REQUEST_START_COUNT,	
	BSS_ACCT_REQUEST_START_RETRY_COUNT,
	BSS_ACCT_RESPONSE_START_COUNT,
	
	BSS_ACCT_REQUEST_UPDATE_COUNT,	
	BSS_ACCT_REQUEST_UPDATE_RETRY_COUNT,
	BSS_ACCT_RESPONSE_UPDATE_COUNT,
	
	BSS_ACCT_REQUEST_STOP_COUNT,	
	BSS_ACCT_REQUEST_STOP_RETRY_COUNT,
	BSS_ACCT_RESPONSE_STOP_COUNT
} bss_stat_info_type;

eag_statistics_t *
eag_statistics_create(uint32_t size);

int
eag_statistics_destroy(eag_statistics_t *eagstat);

int
eag_ap_statistics_count(eag_statistics_t *eagstat);

int
eag_bss_statistics_count(eag_statistics_t *eagstat);

int 
ins_bss_auth_count(eag_statistics_t *eagstat,
		uint8_t apmac[6], uint8_t wlanid, uint8_t radioid,
		bss_stat_info_type info_type, uint32_t value);

int
eag_bss_message_count(eag_statistics_t *eagstat,
		struct app_conn_t *appconn,
		bss_stat_info_type info_type, uint32_t value);

int
eag_statistics_clear(eag_statistics_t *eagstat);

int
eag_statistics_count_online_user_info(eag_statistics_t *eagstat);

int
eag_statistics_set_appdb(eag_statistics_t *eagstat,
		appconn_db_t *appdb);

struct list_head *
eag_statistics_get_ap_head(eag_statistics_t *eagstat);

#endif		/* _EAG_STATISTICS_H */

