

#ifndef _SEM_FPGA_H
#define _SEM_FPGA_H

#define FPGA_BM_FILE_PATH	"/dev/bm0"


#define BM_IOC_MAGIC 0xEC 
#define BM_IOC_FPGA_WRITE 				_IOWR(BM_IOC_MAGIC,20,fpga_file) //FPGA online burning
#define BM_IOC_G_  				_IOWR(BM_IOC_MAGIC,1,bm_op_args) // read values
#define BM_IOC_X_ 				_IOWR(BM_IOC_MAGIC,2,bm_op_args) // write and readout wrote value
#define BM_IOC_FPGA_W 				_IOWR(BM_IOC_MAGIC,24,bm_op_args)//write fpga reg no delay

/*start for read car table counter */
typedef struct car_table_s{

	unsigned long long car_id;
	unsigned long long reload;
	unsigned long long linkup;
	unsigned long long car_valid;
	unsigned int usr_ip;
    unsigned long long credit;
	unsigned long long byte_drop_count;  
	unsigned long long byte_set_count;
	unsigned long long package_set_count;
	unsigned long long package_drop_count;
}car_table_t;

#define CAR_TABLE_CAR_VALID_REG	(0x800000001d070222ull)
/* end  */
#define FPGA_REG_BASE (0x800000001d070000ull)
#define DEVICE_ID (0x800000001d070000ull)
#define SYS_VERSION (0x800000001d070002ull)
#define SYS_CTL_1 (0x800000001d070004ull)
#define SYS_CTL_2 (0x800000001d07001cull)
#define SYS_STATE (0x800000001d070006ull)
#define HASH_AGE (0x800000001d07000cull)
#define HASH_UPDATA (0x800000001d07000eull)

#define HASH_AGING_TIME	(0x800000001d07000cull)
#define INDIRECT_ACCESS_CONTROL_REG (0x800000001d070040ull)
#define INDIRECT_ACCESS_ADDRESS_REG_L (0x800000001d070042ull)
#define INDIRECT_ACCESS_ADDRESS_REG_H (0x800000001d070044ull)
#define INDIRECT_ACCESS_STATE_REG (0x800000001d070046ull)
#define Data_Temp_Stor_Base_Addr  (0x800000001d070200ull)
#define Data_Temp_Stor_Hash_Ctl  (0x800000001d07021eull)

#define LED_CONTROL_REG  (0x800000001d07000aull)


#define READY_REG_VALUE (0x1)

#define HASH_TABLE_1_VALUE (0x10)
#define HASH_TABLE_2_VALUE (0x20)
#define CAR_TABLE_VALUE (0x30)
#define CAM_CORE_VALUE (0x40)
#define CAM_TABLE_VALUE (0x50)

#define	HASH_TABLE_ID_REG_0         (0x800000001d070200ull)
#define	HASH_TABLE_ID_REG_1         (0x800000001d070202ull)
/*start for write car table counter */

/* cam core */
#define	CAM_CORE_REG_0         (0x800000001d070200ull) 	
#define	CAM_CORE_REG_1	       (0x800000001d070202ull)					
#define	CAM_CORE_REG_2         (0x800000001d070204ull) 			
#define	CAM_CORE_REG_3	       (0x800000001d070206ull) 			
#define	CAM_CORE_REG_4         (0x800000001d070208ull)  			
#define	CAM_CORE_REG_5	       (0x800000001d07020aull) 			
#define	CAM_CORE_REG_6	       (0x800000001d07020cull)
#define	CAM_CORE_REG_7         (0x800000001d07020eull)
/**/

/*Cycle_1*/
#define	SOURCE_IP_ADDRESS_REG_H         (0x800000001d070210ull) 	/* 8 */
#define	SOURCE_IP_ADDRESS_REG_L	        (0x800000001d070212ull)	/* 9 */				
#define	DESTINATION_IP_ADDRESS_REG_H    (0x800000001d070214ull) 	/* 10 */		
#define	DESTINATION_IP_ADDRESS_REG_L	(0x800000001d070216ull) 	/* 11 */		
#define	SOURCE_PORT_REG                 (0x800000001d070218ull)  	/* 12 */		
#define	DESTINATION_PORT_REG	        (0x800000001d07021aull) 	/* 13 */		
#define	PROTOCOL_REG	                (0x800000001d07021cull)	/* 14 */			
#define	HASHCTL_REG                     (0x800000001d07021eull)    /* 15 */

/*Cycle_2*/
#define DESTINATION_MAC_ADDRESS_REG_H	(0x800000001d070220ull) 	/* 16 */		
#define DESTINATION_MAC_ADDRESS_REG_M	(0x800000001d070222ull)	/* 17 */			
#define DESTINATION_MAC_ADDRESS_REG_L	(0x800000001d070224ull) 	/* 18 */		
#define SOURCE_MAC_ADDRESS_REG_H	    (0x800000001d070226ull) 	/* 19 */		
#define SOURCE_MAC_ADDRESS_REG_M	    (0x800000001d070228ull) 	/* 20 */		
#define SOURCE_MAC_ADDRESS_REG_L	    (0x800000001d07022aull)   	/* 21 */
#define VLAN_OUT_REG_H	                (0x800000001d07022cull)	/* 22 */		
#define VLAN_OUT_REG_L      	        (0x800000001d07022eull)    /* 23 */

/*Cycle_3*/
#define VLAN_IN_REG_H	        (0x800000001d070230ull) 	/* 24 */			
#define VLAN_IN_REG_L	        (0x800000001d070232ull)	/* 25 */
#define	ETHERNET_PROTOCOL_REG   (0x800000001d070234ull)    /* 26 */
#define	IP_TOS_REG              (0x800000001d070236ull)    /* 27 */
#define	IP_LEN_REG	            (0x800000001d070238ull) 	/* 28 */		
#define	IP_ID_REG	            (0x800000001d07023aull) 	/* 29 */		
#define	IP_OFFSET_REG	        (0x800000001d07023cull)	/* 30 */	
#define	IP_TTL_REG              (0x800000001d07023eull)    /* 31 */

/*Cycle_4*/
#define	IP_CHECKSUM_REG	                    (0x800000001d070240ull)    /* 32 */ 			
#define	TUNNEL_SOURCE_IP_ADDRESS_REG_H	    (0x800000001d070242ull) 	/* 33 */		
#define	TUNNEL_SOURCE_IP_ADDRESS_REG_L	    (0x800000001d070244ull) 	/* 34 */		
#define	TUNNEL_DESTINATION_IP_ADDRESS_REG_H	(0x800000001d070246ull)	/* 35 */		
#define	TUNNEL_DESTINATION_IP_ADDRESS_REG_L	(0x800000001d070248ull)	/* 36 */			
#define	TUNNEL_SOURCE_PORT_REG	            (0x800000001d07024aull) 	/* 37 */
#define	TUNNEL_DESTINATION_PORT_REG	        (0x800000001d07024cull)	/* 38 */			
#define	UDP_LEN_REG	                        (0x800000001d07024eull)	/* 39 */			

/*Cycle_5*/
#define	UDP_CHECKSUM_REG    	(0x800000001d070250ull) 	/* 40 */			
#define	CAPWAP_HEADER_REG_B0   	(0x800000001d070252ull)	/* 41 */				
#define	CAPWAP_HEADER_REG_B2   	(0x800000001d070254ull) 	/* 42 */		
#define	CAPWAP_HEADER_REG_B4   	(0x800000001d070256ull) 	/* 43 */		
#define	CAPWAP_HEADER_REG_B6   	(0x800000001d070258ull) 	/* 44 */		
#define	CAPWAP_HEADER_REG_B8   	(0x800000001d07025aull)	/* 45 */	
#define	CAPWAP_HEADER_REG_B10  	(0x800000001d07025cull)	/* 46 */			
#define	CAPWAP_HEADER_REG_B12   (0x800000001d07025eull)    /* 47 */

/*Cycle_6*/
#define	CAPWAP_HEADER_REG_B14   	(0x800000001d070260ull) 	/* 48 */			
#define	HEADER_802_11_REG_B0   		(0x800000001d070262ull)	/* 49 */				
#define	HEADER_802_11_REG_B2	    (0x800000001d070264ull) 	/* 50 */		
#define	HEADER_802_11_REG_B4    	(0x800000001d070266ull) 	/* 51 */		
#define	HEADER_802_11_REG_B6    	(0x800000001d070268ull) 	/* 52 */		
#define	HEADER_802_11_REG_B8    	(0x800000001d07026aull) 	/* 53 */		
#define	HEADER_802_11_REG_B10       (0x800000001d07026cull)	/* 54 */		
#define	HEADER_802_11_REG_B12       (0x800000001d07026eull)    /* 55 */

/*Cycle_7*/
#define	HEADER_802_11_REG_B14   	(0x800000001d070270ull) 	/* 56 */			
#define	HEADER_802_11_REG_B16       (0x800000001d070272ull)	/* 57 */				
#define	HEADER_802_11_REG_B18	    (0x800000001d070274ull)	/* 58 */		
#define	HEADER_802_11_REG_B20	    (0x800000001d070276ull) 	/* 59 */		
#define	HEADER_802_11_REG_B22	    (0x800000001d070278ull)	/* 60 */		
#define	HEADER_802_11_REG_B24	    (0x800000001d07027aull) 	/* 61 */		
#define	HEADER_802_11_REG_B26	    (0x800000001d07027cull)	/* 62 */			
#define	CAPWAP_HEADER_PROTOCOL_REG	(0x800000001d07027eull)    /* 63 */
/* end */

/*for show port counter */
#define FPGA_LOCK_CLEAR_REG  (0x800000001d070016ull)
#define Mib_Xaui_Reg_Base_Addr  (0x800000001d070100ull)

typedef struct xaui_port_counter_s {

	unsigned long long tx_package_num;
	unsigned long long tx_bytenum;
	unsigned long long tx_multicast_package_num;
	unsigned long long tx_broadcast_package_num;
	unsigned long long tx_pause_package_num;

	unsigned long long rx_good_package_num;
	unsigned long long rx_good_byte_num;
	unsigned long long rx_bad_package_num;
	unsigned long long rx_fcserr_package_num;
	unsigned long long rx_multicast_package_num;
	unsigned long long rx_broadcast_package_num;
	unsigned long long rx_pause_package_num;
	unsigned long long rx_drop_num;
	unsigned long long rx_ttl_drop_num;
	unsigned long long rx_car_drop_num;
	unsigned long long rx_hash_col_num;
	unsigned long long rx_to_cpu_num;

    //unsigned long long hash_aging_num;
    //unsigned long long hash_update_num;
    //unsigned long long hash_learn_num;

}xaui_port_counter_t;

typedef struct hash_statistics_s
{
    unsigned long long hash_aging_num;
    unsigned long long hash_update_num;
    unsigned long long hash_learn_num;
}hash_statistics_t;

/*end*/
typedef struct fpga_file
{
	char fpga_bin_name[256];		//FPGA burning filename
	int result;
}fpga_file;

void dcli_fpga_init(void);

/*Indirect access to data temporary storage area*/

static long DataTempStorageArea[] = { 				

/*Cycle_1*/
	0x10, 	/* 8 */
	0x12,	/* 9 */				
	0x14, 	/* 10 */		
	0x16, 	/* 11*/		
	0x18,  	/* 12 */		
	0x1a, 	/* 13*/		
	0x1c,	/* 14 */			
	0x1e,   /* 15 */
/*Cycle_2*/
	0x20, 	/* 16 */		
	0x22,	/* 17 */				
	0x24, 	/* 18 */		
	0x26, 	/* 19 */		
	0x28, 	/* 20 */		
	0x2a,   /* 21 */	
	0x2c,	/* 22 */		
	0x2e,   /* 23 */
/*Cycle_3*/
	0x30, 	/* 24 */			
	0x32,	/* 25 */				
	0x34, 	/* 26 */		
	0x36, 	/* 27 */		
	0x38, 	/* 28 */		
	0x3a, 	/* 29 */		
	0x3c,	/* 30 */			
	0x3e,   /* 31 */
/*Cycle_4*/
	0x40, 	/* 32 */		
	0x42, 	/* 33 */		
	0x44, 	/* 34 */		
	0x46,	/* 35 */		
	0x48,	/* 36 */			
	0x4a, 	/* 37 */
	0x4c,	/* 38 */			
	0x4e,	/* 39 */			
/*Cycle_5*/
	0x50, 	/* 40 */			
	0x52,	/* 41 */				
	0x54, 	/* 42 */		
	0x56, 	/* 43 */		
	0x58, 	/* 44 */		
	0x5a,	/* 45 */	
	0x5c,	/* 46 */			
	0x5e,	/* 47 */			
/*Cycle_6*/
	0x60, 	/* 48 */			
	0x62,	/* 49 */				
	0x64, 	/* 50 */		
	0x66, 	/* 51 */		
	0x68, 	/* 52 */		
	0x6a, 	/* 53 */		
	0x6c,	/* 54 */		
	0x6e,	/* 55 */	
/*Cycle_7*/
	0x70, 	/* 56 */			
	0x72,	/* 57 */				
	0x74,	/* 58 */		
	0x76, 	/* 59 */		
	0x78,	/* 60 */		
	0x7a, 	/* 61 */		
	0x7c,	/* 62 */			
	0x7e,   /* 63 */
}; 
/*end*/

/*for counter register*/

static long MibXauiRegOffsetAll[] = { 				

    /*xaui112   k=0*/
	0x00,/* rx_good_pkt_num [31:16]*/ 			
	0x02,/* rx_good_pkt_num [15:0]*/ 			
	0x04,/* rx_good_byte_num[39:32]*/ 			
	0x06,/* rx_good_byte_num[31:16] */			
	0x08,/* rx_good_byte_num[15:0]	*/				
	0x0a,/* rx_bad_pkt_num[15:0] */ 	               
	0x0c,/* rx_fcserr_pkt_num[15:0] */				
	0x0e,/* rx_multicast_pkt_num[15:0] */				
	0x10,/* rx_broadcast_pkt_num[15:0] */ 				
	0x12,/* rx_pause_pkt_num[15:0] */
	0x14,/* rx_drop_num[15:0] */
	0x16,/* rx_ttl_drop_num[15:0] */ 			
	0x18,/* rx_car_drop_num[15:0] */ 			
	0x1a,/* rx_hash_col_num[31:16] */ 			
	0x1c,/* rx_hash_col_num[15:0] */ 			
	0x1e,/* rx_to_cpu_num[31:16] */				
	0x20,/* rx_to_cpu_num[15:0] */				
	0x22,/* tx_pkt_num[31:16] */ 				
	0x24,/* tx_pkt_num[15:0] */					
	0x26,/* tx_byte_num[39:32] */ 			
	0x28,/* tx_byte_num[31:16] */ 			
	0x2a,/* tx_byte_num[15:0] */ 			
	0x2c,/* tx_multicast_pkt_num[15:0] */ 			
	0x2e,/* tx_broadcast_pkt_num[15:0] */				
	0x30,/* tx_pause_pkt_num[15:0] */	
	
    /*xaui113   k=25*/
	0x40,/* rx_good_pkt_num [31:16]*/ 			
	0x42,/* rx_good_pkt_num [15:0]*/ 			
	0x44,/* rx_good_byte_num[39:32]*/ 			
	0x46,/* rx_good_byte_num[31:16] */			
	0x48,/* rx_good_byte_num[15:0]	*/				
	0x4a,/* rx_bad_pkt_num[15:0] */ 	
	0x4c,/* rx_fcserr_pkt_num[15:0] */				
	0x4e,/* rx_multicast_pkt_num[15:0] */				
	0x50,/* rx_broadcast_pkt_num[15:0] */ 				
	0x52,/* rx_pause_pkt_num[15:0] */
	0x54,/* rx_drop_num[15:0] */
	0x56,/* rx_ttl_drop_num[15:0] */ 			
	0x58,/* rx_car_drop_num[15:0] */ 			
	0x5a,/* rx_hash_col_num[31:16] */ 			
	0x5c,/* rx_hash_col_num[15:0] */ 			
	0x5e,/* rx_to_cpu_num[31:16] */				
	0x60,/* rx_to_cpu_num[15:0] */				
	0x62,/* tx_pkt_num[31:16] */ 				
	0x64,/* tx_pkt_num[15:0] */					
	0x66,/* tx_byte_num[39:32] */ 			
	0x68,/* tx_byte_num[31:16] */ 			
	0x6a,/* tx_byte_num[15:0] */ 			
	0x6c,/* tx_multicast_pkt_num[15:0] */ 			
	0x6e,/* tx_broadcast_pkt_num[15:0] */				
	0x70,/* tx_pause_pkt_num[15:0] */		
	
/*xaui115   k=50*/
	0x80,/* rx_good_pkt_num [31:16]*/ 			
	0x82,/* rx_good_pkt_num [15:0]*/ 			
	0x84,/* rx_good_byte_num[39:32]*/ 			
	0x86,/* rx_good_byte_num[31:16] */			
	0x88,/* rx_good_byte_num[15:0]	*/				
	0x8a,/* rx_bad_pkt_num[15:0] */ 	
	0x8c,/* rx_fcserr_pkt_num[15:0] */				
	0x8e,/* rx_multicast_pkt_num[15:0] */				
	0x90,/* rx_broadcast_pkt_num[15:0] */ 				
	0x92,/* rx_pause_pkt_num[15:0] */
	0x94,/* rx_drop_num[15:0] */
	0x96,/* rx_ttl_drop_num[15:0] */ 			
	0x98,/* rx_car_drop_num[15:0] */ 			
	0x9a,/* rx_hash_col_num[31:16] */ 			
	0x9c,/* rx_hash_col_num[15:0] */ 			
	0x9e,/* rx_to_cpu_num[31:16] */				
	0xa0,/* rx_to_cpu_num[15:0] */				
	0xa2,/* tx_pkt_num[31:16] */ 				
	0xa4,/* tx_pkt_num[15:0] */					
	0xa6,/* tx_byte_num[39:32] */ 			
	0xa8,/* tx_byte_num[31:16] */ 			
	0xaa,/* tx_byte_num[15:0] */ 			
	0xac,/* tx_multicast_pkt_num[15:0] */ 			
	0xae,/* tx_broadcast_pkt_num[15:0] */				
	0xb0,/* tx_pause_pkt_num[15:0] */			
	
/*xaui116  k=75*/
	0xc0,/* rx_good_pkt_num [31:16]*/ 			
	0xc2,/* rx_good_pkt_num [15:0]*/ 			
	0xc4,/* rx_good_byte_num[39:32]*/ 			
	0xc6,/* rx_good_byte_num[31:16] */			
	0xc8,/* rx_good_byte_num[15:0]	*/				
	0xca,/* rx_bad_pkt_num[15:0] */ 	
	0xcc,/* rx_fcserr_pkt_num[15:0] */				
	0xce,/* rx_multicast_pkt_num[15:0] */				
	0xd0,/* rx_broadcast_pkt_num[15:0] */ 				
	0xd2,/* rx_pause_pkt_num[15:0] */
	0xd4,/* rx_drop_num[15:0] */
	
/*hash	k=86*/
	0xd6,/* hash_aging_num[31:16] */ 			
	0xd8,/* hash_aging_num[15:0] */ 			
	0xda,/* hash_update_num [31:16] */ 			
	0xdc,/* hash_update_num [15:0] */				
	0xde,/* hash_learn_num [31:16] */				
	0xe0,/* hash_learn_num [15:0] */ 				

	0xe2,/* tx_pkt_num[31:16] */		
	0xe4,/* tx_pkt_num[15:0] */					
	0xe6,/* tx_byte_num[39:32] */ 			
	0xe8,/* tx_byte_num[31:16] */ 			
	0xea,/* tx_byte_num[15:0] */ 			
	0xec,/* tx_multicast_pkt_num[15:0] */ 			
	0xee,/* tx_broadcast_pkt_num[15:0] */				
	0xf0,/* tx_pause_pkt_num[15:0] */				
	
}; /* register address */

#include <linux/if_vlan.h>

#define SET_VLAN_QINQ_TYPE_CMD (GET_VLAN_VID_CMD+1)      /* for add a member to enum vlan_ioctl_cmds of compiler and kernel */
#define GET_VLAN_QINQ_TYPE_CMD  (SET_VLAN_QINQ_TYPE_CMD + 1) /* for get dev's qinq-type */
#define SET_VLAN_FLAG_CFI_CMD  (GET_VLAN_QINQ_TYPE_CMD + 1) /* for change dev's CFI */

#define HASH_MAX (2*1024*1024)
#define CAR_MAX (128*1024)
#define CAR_WHITE_MAX (2048)
#define WAN_IF_MAX (100)
#define WAN_IF_NAME_LEN	20
/*
typedef struct xaui_port_counter_s {

	unsigned long long tx_package_num;
	unsigned long long tx_bytenum;
	unsigned long long tx_multicast_package_num;
	unsigned long long tx_broadcast_package_num;
	unsigned long long tx_pause_package_num;

	unsigned long long rx_good_package_num;
	unsigned long long rx_good_byte_num;
	unsigned long long rx_bad_package_num;
	unsigned long long rx_fcserr_package_num;
	unsigned long long rx_multicast_package_num;
	unsigned long long rx_broadcast_package_num;
	unsigned long long rx_pause_package_num;
	unsigned long long rx_drop_num;
	unsigned long long rx_ttl_drop_num;
	unsigned long long rx_car_drop_num;
	unsigned long long rx_hash_col_num;
	unsigned long long rx_to_cpu_num;

    unsigned long long hash_aging_num;
    unsigned long long hash_update_num;
    unsigned long long hash_learn_num;

}xaui_port_counter_t;
*/
struct hash_cam_table_result_s {

	unsigned long long sip;
	unsigned long long sport;
	unsigned long long smac;
	unsigned long long tunnel_sip;
	unsigned long long tunnel_sport;

	unsigned long long dip;
	unsigned long long dport;
	unsigned long long dmac;
	unsigned long long tunnel_dip;
	unsigned long long tunnel_dport;

    unsigned long long protocol;
	
	unsigned long long setup_time;  /* table creation time */
	unsigned long long hash_ctl;
	unsigned long long vlan_out;
	unsigned long long vlan_in;
	
	unsigned long long ethernet_protocol;
	unsigned long long ip_tos;
	unsigned long long ip_len;
	unsigned long long ip_identification;
	unsigned long long ip_offset;
	unsigned long long ip_ttl;
	unsigned long long ip_protocol;
    unsigned long long ip_checksum;
	
	unsigned long long udp_len;
    unsigned long long udp_checksum;	
    unsigned long long capwap_header_b0;
	unsigned long long capwap_header_b2;
	unsigned long long capwap_header_b4;
	unsigned long long capwap_header_b6;
	unsigned long long capwap_header_b8;
	unsigned long long capwap_header_b10;
	unsigned long long capwap_header_b12;
	unsigned long long capwap_header_b14;
	
    unsigned long long header_802_11_b0;
	unsigned long long header_802_11_b2;
	unsigned long long header_802_11_b4;
	unsigned long long header_802_11_b6;
	unsigned long long header_802_11_b8;
	unsigned long long header_802_11_b10;
	unsigned long long header_802_11_b12;
	unsigned long long header_802_11_b14;
	unsigned long long header_802_11_b16;
	unsigned long long header_802_11_b18;
	unsigned long long header_802_11_b20;
	unsigned long long header_802_11_b22;
	unsigned long long header_802_11_b24;
	unsigned long long header_802_11_b26;
	unsigned long long capwap_protocol;

};


struct hash_cam_table_s {
	struct src_s 
	{
		unsigned short sip_H;
		unsigned short sip_L;
		unsigned short sport;
		unsigned short smac_H;
        unsigned short smac_M;
		unsigned short smac_L;
		unsigned short tunnel_sip_H;
		unsigned short tunnel_sip_L;
		unsigned short tunnel_sport;

	} src;
	struct dst_s 
	{
		unsigned short dip_H;
		unsigned short dip_L;
		unsigned short dport;
		unsigned short dmac_H;
        unsigned short dmac_M;
		unsigned short dmac_L;
		unsigned short tunnel_dip_H;
		unsigned short tunnel_dip_L;
		unsigned short tunnel_dport;

	} dst;
    unsigned short protocol;
	
	unsigned short setup_time;  /* table creation time */
	unsigned short hash_ctl;
	unsigned short vlan_out_H;
	unsigned short vlan_out_L;
	unsigned short vlan_in_H;
	unsigned short vlan_in_L;
	
	unsigned short ethernet_protocol;
	unsigned short ip_tos;
	unsigned short ip_len;
	unsigned short ip_identification;
	unsigned short ip_offset;
	unsigned short ip_ttl;
	unsigned short ip_protocol;
    unsigned short ip_checksum;
	
	unsigned short udp_len;
    unsigned short udp_checksum;	
    unsigned short capwap_header_b0;
	unsigned short capwap_header_b2;
	unsigned short capwap_header_b4;
	unsigned short capwap_header_b6;
	unsigned short capwap_header_b8;
	unsigned short capwap_header_b10;
	unsigned short capwap_header_b12;
	unsigned short capwap_header_b14;
	
    unsigned short header_802_11_b0;
	unsigned short header_802_11_b2;
	unsigned short header_802_11_b4;
	unsigned short header_802_11_b6;
	unsigned short header_802_11_b8;
	unsigned short header_802_11_b10;
	unsigned short header_802_11_b12;
	unsigned short header_802_11_b14;
	unsigned short header_802_11_b16;
	unsigned short header_802_11_b18;
	unsigned short header_802_11_b20;
	unsigned short header_802_11_b22;
	unsigned short header_802_11_b24;
	unsigned short header_802_11_b26;
	unsigned short capwap_protocol;

};

typedef struct hash_table_s {

	unsigned short sip_H;
	unsigned short sip_L;
	unsigned short sport;
	unsigned short smac_H;
    unsigned short smac_M;
	unsigned short smac_L;
	unsigned short tunnel_sip_H;
	unsigned short tunnel_sip_L;
	unsigned short tunnel_sport;
	unsigned short dip_H;
	unsigned short dip_L;
	unsigned short dport;
	unsigned short dmac_H;
    unsigned short dmac_M;
	unsigned short dmac_L;
	unsigned short tunnel_dip_H;
	unsigned short tunnel_dip_L;
	unsigned short tunnel_dport;
    unsigned short protocol;
	unsigned short setup_time;  /* table creation time */
	unsigned short hash_ctl;
	unsigned short vlan_out_H;
	unsigned short vlan_out_L;
	unsigned short vlan_in_H;
	unsigned short vlan_in_L;
	unsigned short ethernet_protocol;
	unsigned short ip_tos;
	unsigned short ip_len;
	unsigned short ip_identification;
	unsigned short ip_offset;
	unsigned short ip_ttl;
	unsigned short ip_protocol;
    unsigned short ip_checksum;
	unsigned short udp_len;
    unsigned short udp_checksum;	
    unsigned short capwap_header_b0;
	unsigned short capwap_header_b2;
	unsigned short capwap_header_b4;
	unsigned short capwap_header_b6;
	unsigned short capwap_header_b8;
	unsigned short capwap_header_b10;
	unsigned short capwap_header_b12;
	unsigned short capwap_header_b14;
    unsigned short header_802_11_b0;
	unsigned short header_802_11_b2;
	unsigned short header_802_11_b4;
	unsigned short header_802_11_b6;
	unsigned short header_802_11_b8;
	unsigned short header_802_11_b10;
	unsigned short header_802_11_b12;
	unsigned short header_802_11_b14;
	unsigned short header_802_11_b16;
	unsigned short header_802_11_b18;
	unsigned short header_802_11_b20;
	unsigned short header_802_11_b22;
	unsigned short header_802_11_b24;
	unsigned short header_802_11_b26;
	unsigned short capwap_protocol;
}hash_table_t;


typedef struct write_cam_param_s
{
    unsigned short Indirect_add_reg_H;
    unsigned short Indirect_add_reg_L;
	unsigned short SIP_H;
	unsigned short SIP_L;
	unsigned short DIP_H;
	unsigned short DIP_L;
	unsigned short SPORT;
	unsigned short DPORT;
	unsigned short PROTOCOL;
	unsigned short HASH_CTL;
	unsigned short DMAC_H;
	unsigned short DMAC_M;
	unsigned short DMAC_L;
	unsigned short SMAC_H;
	unsigned short SMAC_M;
	unsigned short SMAC_L;
	unsigned short VLAN_OUT_H;
	unsigned short VLAN_OUT_L;
	unsigned short VLAN_IN_H;
	unsigned short VLAN_IN_L;
	unsigned short FIRST_0800;//add for test DDR
	unsigned short IPTOS;
	unsigned short IP_LEN;//add for test DDR
	unsigned short IP_ID;//add for test DDR
	unsigned short IP_OFFSET;//add for test DDR
	unsigned short IP_TTL_IP_PROTOCOL;
	unsigned short IP_CHECKSUM;//add for test DDR
	unsigned short TUNNEL_SIP_H;
	unsigned short TUNNEL_SIP_L;
	unsigned short TUNNEL_DIP_H;
	unsigned short TUNNEL_DIP_L;
	unsigned short TUNNEL_SRC_PORT;
	unsigned short TUNNEL_DST_PORT;
	unsigned short UDP_LEN;//add for test DDR
	unsigned short UDP_CHECKSUM;//add for test DDR
	unsigned short CAPWAP_B0B1;
	unsigned short CAPWAP_B2B3;
	unsigned short CAPWAP_B4B5;
	unsigned short CAPWAP_B6B7;
	unsigned short CAPWAP_B8B9;
	unsigned short CAPWAP_B10B11;
	unsigned short CAPWAP_B12B13;
	unsigned short CAPWAP_B14B15;
	unsigned short H_802_11_B0B1;
	unsigned short H_802_11_B2B3;
	unsigned short H_802_11_B4B5;
	unsigned short H_802_11_B6B7;
	unsigned short H_802_11_B8B9;
	unsigned short H_802_11_B10B11;
	unsigned short H_802_11_B12B13;
	unsigned short H_802_11_B14B15;
	unsigned short H_802_11_B16B17;
	unsigned short H_802_11_B18B19;
	unsigned short H_802_11_B20B21;
	unsigned short H_802_11_B22B23;
	unsigned short H_802_11_B24B25;
	unsigned short H_802_11_B26B27;
	unsigned short SEC_0800;//add for test DDR
}write_cam_param_t;

typedef struct read_cam_result_s
{
	unsigned short cam_table_id;

	unsigned short sip_H;
	unsigned short sip_L;
	unsigned short sport;
	unsigned short smac_H;
    unsigned short smac_M;
	unsigned short smac_L;
	unsigned short tunnel_sip_H;
	unsigned short tunnel_sip_L;
	unsigned short tunnel_sport;

	unsigned short dip_H;
	unsigned short dip_L;
	unsigned short dport;
	unsigned short dmac_H;
    unsigned short dmac_M;
	unsigned short dmac_L;
	unsigned short tunnel_dip_H;
	unsigned short tunnel_dip_L;
	unsigned short tunnel_dport;

    unsigned short protocol;
	
	unsigned short setup_time;  /* table creation time */
	unsigned short hash_ctl;
	unsigned short vlan_out_H;
	unsigned short vlan_out_L;
	unsigned short vlan_in_H;
	unsigned short vlan_in_L;
	
	unsigned short ethernet_protocol;
	unsigned short ip_tos;
	unsigned short ip_len;
	unsigned short ip_offset;
	unsigned short ip_ttl;
	unsigned short ip_protocol;
    unsigned short ip_checksum;
	
	unsigned short udp_len;
    unsigned short udp_checksum;	
    unsigned short capwap_header_b0;
	unsigned short capwap_header_b2;
	unsigned short capwap_header_b4;
	unsigned short capwap_header_b6;
	unsigned short capwap_header_b8;
	unsigned short capwap_header_b10;
	unsigned short capwap_header_b12;
	unsigned short capwap_header_b14;
	
    unsigned short header_802_11_b0;
	unsigned short header_802_11_b2;
	unsigned short header_802_11_b4;
	unsigned short header_802_11_b6;
	unsigned short header_802_11_b8;
	unsigned short header_802_11_b10;
	unsigned short header_802_11_b12;
	unsigned short header_802_11_b14;
	unsigned short header_802_11_b16;
	unsigned short header_802_11_b18;
	unsigned short header_802_11_b20;
	unsigned short header_802_11_b22;
	unsigned short header_802_11_b24;
	unsigned short header_802_11_b26;
	unsigned short capwap_protocol;
}read_cam_result_t;

typedef struct read_car_result_s
{
	unsigned long long reload;
	unsigned long long linkup;
	unsigned long long car_valid;
	unsigned int usr_ip;
    unsigned long long credit;
	unsigned long long byte_drop_count;  
	unsigned long long byte_set_count;
	unsigned long long package_set_count;
	unsigned long long package_drop_count;
}read_car_result_t;

typedef struct read_cam_core_result_s
{
	int match_flag;
	int cam_id;    
}read_cam_core_result_t;

typedef struct write_cam_core_param_s
{
	unsigned short Indirect_add_reg_H;
	unsigned short Indirect_add_reg_L;
	unsigned short SIP_H;
	unsigned short SIP_L;
    unsigned short DIP_H;
	unsigned short DIP_L;
	unsigned short SPORT;
	unsigned short DPORT;
	unsigned short PROTOCOL;
}write_cam_core_param_t;

typedef struct read_cam_core_param_s
{
	unsigned short Indirect_add_reg_H;
	unsigned short Indirect_add_reg_L;
	unsigned short SIP_H;
	unsigned short SIP_L;
    unsigned short DIP_H;
	unsigned short DIP_L;
	unsigned short SPORT;
	unsigned short DPORT;
	unsigned short PROTOCOL;
	int match_flag;
	int cam_id;
}read_cam_core_param_t;

#define car_uplink 1
#define car_downlink 0
#define car_dynamic 1
#define car_static 0

/*car table link list*/
typedef struct wan_if_linklist_node_s
{
	char name[20];
    struct wan_if_linklist_node_s *next;
}wan_if_linklist_node_t;

/*car white link list */
typedef struct car_white_list_node_s
{
	unsigned int usr_ip;
	unsigned short  linkup;// 1 up;0 down
    struct car_white_list_node_s *next;
}car_white_list_node_t;

typedef struct read_car_white_node_param_s
{
    unsigned int usr_ip;
	unsigned short  linkup;
}read_car_white_node_param_t;

/*car table link list*/
typedef struct car_linklist_node_s
{
	unsigned int usr_ip;
	unsigned int reload;
	unsigned short  linkup;// 1 up;0 down
	unsigned short	 configuration_attribute;// 0 static;1 dynamic
    struct car_linklist_node_s *next;
}car_linklist_node_t;

typedef struct read_car_node_param_s
{
    unsigned int usr_ip;
	unsigned int reload;
	unsigned short  linkup;
	unsigned short	 configuration_attribute;// 0 static;1 dynamic
}read_car_node_param_t;

#endif

