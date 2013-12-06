#ifndef __SE_AGENT_USER_H__
#define __SE_AGENT_USER_H__

#include "cvmx-spinlock.h"
#include "se_agent.h"
#include <linux/tipc.h>
#include "cvmx-rwlock.h"



/********************************************************
 *	user tables
 *********************************************************/
typedef struct  user_item_s{
	struct user_item_s *next;
	struct user_info_s user_info;
	//cvmx_spinlock_t      lock; /*only the first bucket lock is used*/
	cvmx_rwlock_wp_lock_t lock;
	uint16_t valid_entries;  /*only the first bucket valid_entries is used for the number of rule entry*/
	uint8_t reserved[2];	
}user_item_t;

typedef struct user_ipv4_info_s
{	
	uint64_t forward_up_bytes;
	uint64_t forward_up_packet;
	uint64_t forward_down_bytes;
	uint64_t forward_down_packet;
	
	uint32_t user_ip;	
	uint16_t meter_index;	/*the CAR index*/	
	uint16_t flow_number;

	/*below is the user car information*/
	uint32_t   depth;       /*CBS in bytes*/
	uint32_t   rate;        /* CIR in kbps*/

	uint64_t   rate_in_cycles_per_byte;
	uint64_t   depth_in_cycles;
	uint64_t   cycles_prev;

	uint8_t  user_state;
	uint8_t  reserved[7];	

	/*uint64_t	begin_time_stamp;*/
} user_ipv4_info_t;

/********************************************************
 *	user tables
 *********************************************************/
typedef struct  user_ipv4_item_s{
	struct user_ipv4_item_s *next;
	struct user_ipv4_info_s user_info;
	//cvmx_spinlock_t      lock; /*only the first bucket lock is used*/
	cvmx_rwlock_wp_lock_t lock;
	uint16_t valid_entries;  /*only the first bucket valid_entries is used for the number of rule entry*/
	uint8_t reserved[2];	
}user_ipv4_item_t;

typedef struct user_ipv6_info_s
{
	/* ipv6 flux */
	struct cvm_ip6_in6_addr user_ipv6; 
	uint64_t ipv6_forward_up_bytes;
	uint64_t ipv6_forward_up_packet;
	uint64_t ipv6_forward_down_bytes;
	uint64_t ipv6_forward_down_packet;

	uint8_t  user_state;
	uint8_t  reserved[7];
} user_ipv6_info_t;


typedef struct  user_ipv6_item_s{
	struct user_ipv6_item_s *next;
	struct user_ipv6_info_s user_info;
	//cvmx_spinlock_t      lock; /*only the first bucket lock is used*/
	cvmx_rwlock_wp_lock_t lock;
	uint16_t valid_entries;  /*only the first bucket valid_entries is used for the number of rule entry*/
	uint8_t reserved[2];	
}user_ipv6_item_t;



#define USER_TBL_RULE_NAME           "user_tbl"
#define USER_DYNAMIC_TBL_RULE_NAME   "user_dynamic_tbl"
#define USER_IPV6_TBL_RULE_NAME           "user_ipv6_tbl"
#define USER_IPV6_DYNAMIC_TBL_RULE_NAME   "user_ipv6_dynamic_tbl"


extern int se_agent_user_offline(char *buf,struct sockaddr_tipc *client_addr,unsigned int len);
extern int se_agent_user_online(char *buf,struct sockaddr_tipc *client_addr,unsigned int len);
extern int se_agent_get_user_flow_statistics(char *buf,struct sockaddr_tipc *client_addr,unsigned int len);
extern int se_agent_config_pure_payload_acct(char *buf,struct sockaddr_tipc *client_addr,unsigned int len);
extern void se_agent_show_fwd_user_stats(char *buf,struct sockaddr_tipc *client_addr,unsigned int len);
extern int se_agent_show_user_rule_by_ip(char *buf,struct sockaddr_tipc *client_addr,unsigned int len);
extern void se_agent_show_fwd_user_rule_all(char *buf,struct sockaddr_tipc *client_addr,unsigned int len);

#endif
