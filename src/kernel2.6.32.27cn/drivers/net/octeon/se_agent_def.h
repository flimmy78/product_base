#ifndef SE_AGENT_DEF_H
#define SE_AGENT_DEF_H

#define HAND_CMD_SIZE  128   
#define CW_H_LEN       16

#define FCCP_L2_ETH_TYPE  0x8050
#define FCCP_ETH_HEADER_LENGTH  14

/* dst Module definition*/
#define FCCP_MODULE_ACL 1
#define FCCP_MODULE_NAT 2
#define FCCP_MODULE_CAR 3
#define FCCP_MODULE_DBG 4

/* src Module definition */
#define FCCP_MODULE_AGENT_ACL 101
#define FCCP_MODULE_AGENT_NAT 102
#define FCCP_MODULE_AGENT_CAR 103
#define FCCP_MODULE_AGENT_DBG 104

/* opcode definition */
#define FCCP_CMD_INSERT 1
#define FCCP_CMD_DELETE 2
#define FCCP_CMD_SET_AGEING_TIMER 3
#define FCCP_CMD_GET_AGEING_TIMER 4

#define FCCP_CMD_RULE_CNT 5

#define FCCP_RETURN_OK          0x12340000
#define FCCP_RETURN_ERROR       0x12340001  

#define AGENT_RETURN_OK         0x12340010
#define AGENT_RETURN_FAIL       0x12340011

/* ethernet headers*/
typedef struct eth_hdr_s 
{
    uint8_t	 h_dest[6];	   /*destination eth addr */
    uint8_t h_source[6];	   /* source ether addr	*/
    uint16_t h_vlan_proto;              /* Should always be 0x8100 */
}eth_hd_r;

typedef struct  capwap_cache_tbl_s
{
    uint32_t use_num;
    /* Extern IP header */
    uint32_t dip;
    uint32_t sip;
    uint16_t dport;
    uint16_t sport;
    uint8_t tos;
    /*current entry is valid or not*/
    /*uint8_t    valid_bit;	*/
    /* CAPWAP header */
    uint8_t cw_hd[CW_H_LEN];
}capwap_cache_t;


/***************************************
 * 114 Bytes, 
 * ACL rule structer 
 ***************************************/
typedef struct  rule_param_s{

	/*L2_header information  34Bytes*/
	uint64_t dsa_info;
	/*rpa head*/
	struct rpa_header{
		uint8_t rpa_type;
		uint8_t rpa_dnetdevNum;
		uint8_t rpa_snetdevNum;
		uint8_t rpa_d_s_slotNum;
	}rpa_header;
	uint8_t  ether_dhost[6];
	uint8_t  ether_shost[6];
	uint16_t out_ether_type;
	uint16_t out_tag;
	uint16_t in_ether_type;
	uint16_t in_tag;
	uint16_t ether_type;
	

	/*the tunnel internal L2 hader*/
	union {
		/*802.11WIFI header  22Bytes*/
		struct wifi_header {
			uint8_t fc[2];
			uint8_t qos[2];
			uint8_t addr[18];
		} wifi_header;

		/*802.3 ethernet header  14Bytes*/	
		struct eth_header {
			uint8_t  dmac[6];
			uint8_t  smac[6];
			uint16_t ether_type;
		} eth_header;
	}tunnel_l2_header;
#define	acl_tunnel_wifi_header_fc		tunnel_l2_header.wifi_header.fc
#define	acl_tunnel_wifi_header_qos		tunnel_l2_header.wifi_header.qos
#define	acl_tunnel_wifi_header_addr		tunnel_l2_header.wifi_header.addr
#define	acl_tunnel_eth_header_dmac		tunnel_l2_header.eth_header.dmac
#define	acl_tunnel_eth_header_smac		tunnel_l2_header.eth_header.smac
#define	acl_tunnel_eth_header_ether		tunnel_l2_header.eth_header.ether_type


	/*L3-4 header, the HASH key  14Bytes	*/	
	uint32_t  sip;
	uint32_t  dip;  
	uint16_t  sport;
	uint16_t  dport;
	uint8_t  protocol;
	uint8_t  direct_flag; /* flag to uplink downlink*/

	/*²ßÂÔÐÅÏ¢44 Bytes */
	uint16_t forward_port; 
	uint32_t action_type;	/*the action for drop/capwap tunnel/forward/etc*/
	uint32_t action_mask;	/*the action mask for meter/nat/etc*/
	uint64_t time_stamp;  /*the rule time stamp for age*/
	uint64_t extend_index;	    /*for future action extend "action_param_t *"  */
	uint16_t tunnel_index; /*if capwap packet, this is the capwap index*/
	uint16_t nat_index;	/*the NAT table index*/
	uint32_t user_index; /*identify the user index */
	uint16_t user_link_index; /*identify the user link index */
	uint8_t  rule_state; /* static/learning / invalid / learned*/
	volatile uint8_t  packet_wait; /*before acl learned successful, the number of packets forwarded to the linux*/
	/* maybe one rule belong to 2 users, e.g. sending a file between two users. add by zhaohan */
	uint32_t user2_index; /*identify the user2 index */	
	uint16_t user2_link_index; /*identify the user2 link index */
}__attribute__ ((packed)) rule_param_t ;


typedef union
{
    struct
	{
        rule_param_t rule_param;
        capwap_cache_t cw_cache;
    }rule_info;
}fccp_data_t;

typedef struct  control_cmd_s
{   
    uint16_t dest_module; 
    uint16_t src_module; 
    uint16_t cmd_opcode; 
    uint16_t cmd_len; 	
    uint32_t agent_pid;
    uint32_t ret_val;
    fccp_data_t fccp_data;
}control_cmd_t;


#endif

