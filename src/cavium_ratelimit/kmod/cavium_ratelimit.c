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
*cavium_ratelimit.c
*
*CREATOR:
*   <hanhui@autelan.com>
*
*DESCRIPTION:
*     for cavium rate limit by protocol priority
*
*DATE:
*   05/04/2012	
*
*  FILE REVISION NUMBER:
*       $Revision: 1.1 $
*
*******************************************************************************/
#ifdef __cplusplus
    extern "C"
    {
#endif
#define DRV_NAME    "cavium_ratelimit"
#define DRV_VERSION "1.1"
#define DRV_DESCRIPTION "Cavium packet receive rate limit"
#define DRV_COPYRIGHT   "(C) 2012 by Autelan Technology "
#define DRV_LICENSE "Autelan"
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/major.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/etherdevice.h>
#include <linux/miscdevice.h>
#include <linux/ethtool.h>
#include <linux/rtnetlink.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/crc32.h>
#include <linux/in.h>
#include <linux/ctype.h>

#include <linux/smp_lock.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <asm/system.h>
#include <asm/uaccess.h>
/*
#include <pthread.h>
*/
#include "cvmx-config.h"
#include "cvmx.h"
#include "cvmx-bootmem.h"
#include "cvmx-sysinfo.h"

#include "cavium_ratelimit.h"


int rate_limit_enabled = 0;
int rate_limit_fastfwd_enabled = 0;
//module_param(rate_limit_enabled, int, 0666);

int cr_major_num = CR_IOC_MAGIC;
module_param(cr_major_num, int, 0444);
int cr_minor_num = 0;
module_param(cr_minor_num,int,0444);

static int cvm_rate_limit_debug = 0;
//module_param(cvm_rate_limit_debug, int, 0666);
MODULE_PARM_DESC(cvm_rate_limit_debug,"Enable debug messages.");
static int cr_next_valid_index = 0;
static int cr_last_valid_rule_index = 0;
static STATISTIC_TYPE cvm_rate_limit_drop_counter = 0;
static STATISTIC_TYPE cvm_rate_limit_pass_counter = 0;

#define FAST_FWD_RATE_LIMIT

#ifdef  FAST_FWD_RATE_LIMIT

#define SCALE_FACTOR_BIT_SHIFT  16
#define SCALE_FACTOR            (1 << SCALE_FACTOR_BIT_SHIFT)

typedef struct ratelimit_s
{
	uint32_t   rate;        /* CIR in kbps*/
	uint32_t   depth;       /*CBS in bytes*/

	uint64_t   rate_in_cycles_per_byte;
	uint64_t   depth_in_cycles;
	uint64_t   cycles_prev;
} ratelimit_t; 

typedef struct ratelimit_pps_s
{
	uint32_t   rate_pps;        /* pps */
	uint64_t   rate_in_cycles_per_pkt;
	uint64_t   cycles_prev;

} ratelimit_pps_t; 

/* sigal match rule item struct */
typedef struct protocol_match_item_s{
	uint8_t name[PROTOCOL_NAME_LEN];
	uint64_t protocol_match_rule[PACKET_MATCH_BYTE_NUM];
	uint64_t protocol_match_mask[PACKET_MATCH_BYTE_NUM];
	uint32_t rate_bps;
	uint32_t rate_pps;
	uint32_t rules_length;/* the number of 64bit  */
	uint32_t unused;
	uint64_t drop_counter;/*for statistic */
	uint64_t pass_counter;/*for statistic */
	ratelimit_t ratelimit_bps;/*for calculate*/
	ratelimit_pps_t ratelimit_pps;	/*for calculate*/
} protocol_match_item_fastfwd_t;

typedef struct global_various_s{
	uint64_t car_rate_scaled;
	uint64_t car_rate_pps_scaled;	
	uint64_t cvm_rate_limit_drop_counter;
	uint64_t cvm_rate_limit_pass_counter;
	int 	 cvm_rate_limit_enabled;
	uint32_t last_valid_index;
	uint64_t unused[11];
} global_various_t;

void * protocol_rate_limiter_fastfwd_mem_base = NULL;

global_various_t * global_various_ptr_fastfwd = NULL;

protocol_match_item_fastfwd_t * protocol_rate_limiter_fastfwd = NULL;

//CVMX_SHARED uint32_t rate_limit_fastfwd_tbl_size = 0;

CVMX_SHARED uint64_t car_rate_scaled = 0;
CVMX_SHARED uint64_t car_rate_pps_scaled = 0; 
#if 0
static int cvm_car_init(void)
{
	cvmx_sysinfo_t    *sys_info_ptr  = cvmx_sysinfo_get();
	uint64_t           cpu_clock_hz  = sys_info_ptr->cpu_clock_hz;
	car_rate_scaled = ((cpu_clock_hz/1024) * (SCALE_FACTOR * 8));
	car_rate_pps_scaled = (cpu_clock_hz);
	return 0;
}
#endif

/*******************************************************
 *
 * DESCRIPTION:
 *		get meminfo from fastfwd module
 * RETURN:
 *		0   -  success and ok
 *		-1  -  failed or error
 * 
 *******************************************************/
static int cr_get_fastfwd_ratelimit_info(void)
{
	cvmx_bootmem_named_block_desc_t *block_desc = NULL;
	unsigned int rate_limit_fastfwd_tbl_size = 0;

	block_desc = (cvmx_bootmem_named_block_desc_t *)cvmx_bootmem_find_named_block(RATELIMIT_TBL_NAME);
	if (block_desc)
	{	 
		protocol_rate_limiter_fastfwd_mem_base = (void *)cvmx_phys_to_ptr(block_desc->base_addr);
		rate_limit_fastfwd_tbl_size = (block_desc->size - 128) / sizeof(protocol_match_item_fastfwd_t);
		if(protocol_rate_limiter_fastfwd_mem_base && \
			(rate_limit_fastfwd_tbl_size == MAX_MATCH_RULES_NUM))
		{
			protocol_rate_limiter_fastfwd = (protocol_match_item_fastfwd_t *)((void *)protocol_rate_limiter_fastfwd_mem_base + 128);
			//car_rate_scaled = *((unsigned long long *)protocol_rate_limiter_fastfwd_mem_base);
			//car_rate_pps_scaled = *(((unsigned long long *)protocol_rate_limiter_fastfwd_mem_base) + 1);
			global_various_ptr_fastfwd = (global_various_t * )protocol_rate_limiter_fastfwd_mem_base;
			car_rate_scaled = global_various_ptr_fastfwd->car_rate_scaled;
			car_rate_pps_scaled = global_various_ptr_fastfwd->car_rate_pps_scaled;
			printk(KERN_INFO "traffic-policer: get meminfo %p bps %llu pps %llu!\n",\
				protocol_rate_limiter_fastfwd, car_rate_scaled, car_rate_pps_scaled);
			return 0;
		}
	}
	protocol_rate_limiter_fastfwd = NULL;
	rate_limit_fastfwd_tbl_size = 0;
	return -1;
}

/*************************************************
 * for set ratelimit_pps struct, function copied from fastfwd module
 *************************************************/
static inline int cvm_car_pps_set(ratelimit_pps_t* ratelimit_pps, unsigned int rate_pps)
{
	if((rate_pps <0) || (ratelimit_pps == NULL))
		return -1;

	ratelimit_pps->rate_pps = rate_pps;
    ratelimit_pps->cycles_prev = 0;
	if (ratelimit_pps->rate_pps == 0)
	{
		ratelimit_pps->rate_in_cycles_per_pkt = 0x0FFFFFFFFFFFFFFFUL;
	}
	else if(CVM_RATE_NO_LIMIT == ratelimit_pps->rate_pps)
	{
		/*for nolimit */
		ratelimit_pps->rate_in_cycles_per_pkt = 0;
	}
	else
	{
		ratelimit_pps->rate_in_cycles_per_pkt = (car_rate_pps_scaled/ratelimit_pps->rate_pps);
	}

	return 0;
}

#endif
/* sigal match rule item struct */
typedef struct protocol_match_item{
	unsigned char name[PROTOCOL_NAME_LEN];
	MATCH_TYPE protocol_match_rule[PACKET_MATCH_BYTE_NUM];
	MATCH_TYPE protocol_match_mask [PACKET_MATCH_BYTE_NUM];
	unsigned int rateLimit;
	unsigned int rules_length;
	STATISTIC_TYPE drop_counter;/*for statistic */
	STATISTIC_TYPE pass_counter;/*for statistic */
	unsigned int counter;/*for timer*/
	unsigned int unused;
} protocol_match_item_t;
/*struct for ioctl use*/
struct cr_ioctl_struct{
	union{
		protocol_match_item_t protocol_rule;
		struct names{
			unsigned char new_name[PROTOCOL_NAME_LEN];
			unsigned char old_name[PROTOCOL_NAME_LEN];
		}name;
		struct counter{/*for global statistic , all protocol when rate limit enable*/
			STATISTIC_TYPE drop_counter;
			STATISTIC_TYPE pass_counter;
		}c;	
		struct enable
		{
			unsigned int enable;
			unsigned int flag;
		}e;
		unsigned int index;
		unsigned int enable;
		unsigned int flag;
	}u;
};
/*struct of timer and all rules*/
typedef struct rate_limit
{
	struct timer_list rate_limit_timer;
	protocol_match_item_t *protocol_rate_limiter_ptr;
}rate_limit_t;
/*dev for /dev/cavium_ratelimit*/
struct oct_dev_s {
	long quantum;
	long qset;
	unsigned long size;
	unsigned long access_key;
	struct cdev cdev;
};



/************************************************************************
 {   unsigned char array    
 
     00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F
     10  11  12  13  14  15  16  17  18  19  1A  1B  1C  1D  1E  1F
     20  21  22  23  24  25  26  27  28  29  2A  2B  2C  2D  2E  2F
     30  31  32  33  34  35  36  37  38  39  3A  3B  3C  3D  3E  3F
     40  41  42  43  44  45  46  47  48  49  4A  4B  4C  4D  4E  4F
     50  51  52  53  54  55  56  57  58  59  5A  5B  5C  5D  5E  5F
     60  61  62  63  64  65  66  67  68  69  6A  6B  6C  6D  6E  6F
     70  71  72  73  74  75  76  77  78  79  7A  7B  7C  7D  7E  7F 	
     
 }
 {    unsigned int array (5612i & 5612e test result)
    
     00010203                04050607                08090A0B				 0C0D0E0F
     10111213                14151617                18191A1B				 1C1D1E1F
     20212223                24252627                28292A2B				 2C2D2E2F
     30313233                34353637                38393A3B				 3C3D3E3F
     40414243                44454647                48494A4B				 4C4D4E4F
     50515253                54555657                58595A5B				 5C5D5E5F
     60616263                64656667                68696A6B				 6C6D6E6F
     70717273                74757677                78797A7B				 7C7D7E7F	
     
 }
 ************************************************************************/
#define TP_DFLT_PPS					50000
#define TP_ARP_PPS 					4000
#define TP_VRRP_PPS					800
#define TP_ICMP_PPS					1000
#define TP_CAPWAPC_PPS				50000 /* user control packet */
#define TP_CAPWAPD_D_EAP_PPS		30000 /* user control packet, data frame with eap*/
#define TP_CAPWAPD_D_ARP_PPS		3000 /* user control packet, data frame arp */
#define TP_CAPWAPD_D_ICMP_PPS		1000 /* user control packet, icmp packet */
#define TP_CAPWAPD_D_DHCPS_PPS		500 /* user control packet, data frame dhcp request ,as dhcp server */
#define TP_CAPWAPD_D_TCP_PPS		30000 /* user control packet, tcp */
#define TP_CAPWAPD_D_UDP_PPS		30000 /* user control packet, udp */
#define TP_CAPWAPD_D_PPS			50000 /* user control packet, other packet */
#define TP_CAPWAPD_M_PPS			50000 /* user control packet, manage frame */
#define TP_PORTAL_PPS				25000
#define TP_TELNETS_PPS				15000
#define TP_DHCPS_PPS				500
#define TP_RADIUS_AU_C_PPS			21000 /* RADIUS auth client and acct client */
#define TP_RADIUS_AU_C2_PPS			21000 /* RADIUS auth client 2 */
#define TP_RADIUS_AC_C2_PPS			21000 /* RADIUS acct client 2 */
#define TP_SNMPC_PPS				18000 /* SNMP client */
#define TP_HTTPSS_PPS				30000 /* HTTPS server */
#define TP_TCP_PPS					30000
#define TP_UDP_PPS					30000
#define TP_TIPC_PPS					CVM_RATE_NO_LIMIT

/* default limiter value for rule[0-11] */
unsigned int rate_default_limiter[CVM_RATE_LIMIT_DEFAULT_RULE_NUM] = 
{
	TP_DFLT_PPS,
	TP_ARP_PPS,
	TP_VRRP_PPS,
	TP_ICMP_PPS,
	TP_CAPWAPC_PPS,
	TP_CAPWAPD_D_EAP_PPS,
	TP_CAPWAPD_D_ARP_PPS,
	TP_CAPWAPD_D_ICMP_PPS,
	TP_CAPWAPD_D_DHCPS_PPS,
	TP_CAPWAPD_D_TCP_PPS,
	TP_CAPWAPD_D_UDP_PPS,
	TP_CAPWAPD_D_PPS,
	TP_CAPWAPD_M_PPS,
	TP_PORTAL_PPS,
	TP_TELNETS_PPS,
	TP_DHCPS_PPS,
	TP_RADIUS_AU_C_PPS,
	TP_RADIUS_AU_C2_PPS,
	TP_RADIUS_AC_C2_PPS,
	TP_SNMPC_PPS,
	TP_HTTPSS_PPS,
	TP_TCP_PPS,
	TP_UDP_PPS,
	TP_TIPC_PPS,
};

protocol_match_item_t protocol_rate_limiter[MAX_MATCH_RULES_NUM] = 
{	/*MATCH_TYPE : unsigned long long */
	{	/*DEFAULT work when no item match */ 
		"DEFAULT",
		{0x0}/*rule*/,
		{0x0}/*mask*/,TP_DFLT_PPS/*rateLimit (pps)*/
	},
	{	/*arp request/reply*/
		"ARP",
		{0x0,0x08060000}/*rule*/,
		{0x0,0xffff0000}/*mask*/,TP_ARP_PPS/*rateLimit (pps)*/
	},
	{	/*VRRP*/
		"VRRP",
		{0x01005e0000000000,0x08004000,0x00000070}/*rule*/,
		{0xffffff0000000000,0xfffff000,0x000000ff}/*mask*/,TP_VRRP_PPS/*rateLimit (pps)*/
	},
	{	/*ICMP*/
		"ICMP",
		{0x0,0x08004000,0x00000001}/*rule*/,
		{0x0,0xfffff000,0x000000ff}/*mask*/,TP_ICMP_PPS/*rateLimit (pps)*/
	},
	{	/*CAPWAP control packet (UDP: sport 32768, dport 5246)*/
		"CAPWAP_C",
		{0x0,0x08004500,0x00000011,0x0,0x00008000147e0000}/*rule*/,
		{0x0,0xffffff00,0x000000ff,0x0,0x0000ffffffff0000}/*mask*/,TP_CAPWAPC_PPS/*rateLimit (pps)*/
		
	},
	{	/*CAPWAP data packet (UDP: sport 32769, dport 5247)*/
		"CAPWAP_D_D_EAP",
		{0x0,0x08004500,0x11,0x0,0x8001147f0000,0x0000002000000000/*capwap bit18-22*/,0x0,0x0/*802.11 bit2-4*/,0x0,0x0,0x0,0x888e000000000000/*LLC bit 48-63*/}/*rule*/,
		{0x0,0xffffff00,0xff,0x0,0xffffffff0000,0x0000ffff00000000/*capwap bit18-22*/,0x0,0x0/*802.11 bit2-4*/,0x0,0x0,0x0,0xffff000000000000/*LLC bit 48-63*/}/*mask*/,TP_CAPWAPD_D_EAP_PPS/*rateLimit (pps)*/
	},
	{	/*CAPWAP data packet (UDP: sport 32769, dport 5247)*/
		"CAPWAP_D_D_ARP",
		{0x0,0x08004500,0x11,0x0,0x8001147f0000,0x0000002000000000/*capwap bit18-22*/,0x0,0x0/*802.11 bit2-4*/,0x0,0x0,0x0,0x0000000008060000/*LLC bit 48-63*/}/*rule*/,
		{0x0,0xffffff00,0xff,0x0,0xffffffff0000,0x0000ffff00000000/*capwap bit18-22*/,0x0,0x0/*802.11 bit2-4*/,0x0,0x0,0x0,0x00000000ffff0000/*LLC bit 48-63*/}/*mask*/,TP_CAPWAPD_D_ARP_PPS /*rateLimit (pps)*/
	},
	{	/*CAPWAP data packet (UDP: sport 32769, dport 5247)*/
		"CAPWAP_D_D_ICMP",
		{0x0,0x08004500,0x11,0x0,0x8001147f0000,0x0000002000000000/*capwap bit18-22*/,0x0,0x0/*802.11 bit2-4*/,0x0,0x0,0x0,0x0800450000000000/*LLC bit 48-63*/, 0x0100000000}/*rule*/,
		{0x0,0xffffff00,0xff,0x0,0xffffffff0000,0x0000ffff00000000/*capwap bit18-22*/,0x0,0x0/*802.11 bit2-4*/,0x0,0x0,0x0,0xffffff0000000000/*LLC bit 48-63*/, 0xff00000000}/*mask*/,TP_CAPWAPD_D_ICMP_PPS /*rateLimit (pps)*/
	},
	{	/*CAPWAP data packet (UDP: sport 32769, dport 5247)*/
		"CAPWAP_D_D_DHCP_request",
		{0x0,0x08004500,0x11,0x0,0x8001147f0000,0x0000002000000000/*capwap bit18-22*/,0x0,0x0/*802.11 bit2-4*/,0x0,0x0,0x0,0x0800450000000000/*LLC bit 48-63*/, 0x1100000000, 0x0044, 0x0043000000000000}/*rule*/,
		{0x0,0xffffff00,0xff,0x0,0xffffffff0000,0x0000ffff00000000/*capwap bit18-22*/,0x0,0x0/*802.11 bit2-4*/,0x0,0x0,0x0,0xffffff0000000000/*LLC bit 48-63*/, 0xff00000000, 0xffff, 0xffff000000000000}/*mask*/,TP_CAPWAPD_D_DHCPS_PPS /*rateLimit (pps)*/
	},
	{	/*CAPWAP data packet (UDP: sport 32769, dport 5247)*/
		"CAPWAP_D_D_TCP",
		{0x0,0x08004500,0x11,0x0,0x8001147f0000,0x0000002000000000/*capwap bit18-22*/,0x0,0x0/*802.11 bit2-4*/,0x0,0x0,0x0,0x0800450000000000/*LLC bit 48-63*/, 0x0600000000}/*rule*/,
		{0x0,0xffffff00,0xff,0x0,0xffffffff0000,0x0000ffff00000000/*capwap bit18-22*/,0x0,0x0/*802.11 bit2-4*/,0x0,0x0,0x0,0xffffff0000000000/*LLC bit 48-63*/, 0xff00000000}/*mask*/,TP_CAPWAPD_D_TCP_PPS /*rateLimit (pps)*/
	},
	{	/*CAPWAP data packet (UDP: sport 32769, dport 5247)*/
		"CAPWAP_D_D_UDP",
		{0x0,0x08004500,0x11,0x0,0x8001147f0000,0x0000002000000000/*capwap bit18-22*/,0x0,0x0/*802.11 bit2-4*/,0x0,0x0,0x0,0x0800450000000000/*LLC bit 48-63*/, 0x1100000000}/*rule*/,
		{0x0,0xffffff00,0xff,0x0,0xffffffff0000,0x0000ffff00000000/*capwap bit18-22*/,0x0,0x0/*802.11 bit2-4*/,0x0,0x0,0x0,0xffffff0000000000/*LLC bit 48-63*/, 0xff00000000}/*mask*/,TP_CAPWAPD_D_UDP_PPS /*rateLimit (pps)*/
	},
	{	/*CAPWAP data packet (UDP: sport 32769, dport 5247)*/
		"CAPWAP_D_D",
		{0x0,0x08004500,0x11,0x0,0x8001147f0000,0x0000002000000000/*capwap bit18-22*/,0x0,0x0/*802.11 bit2-4*/}/*rule*/,
		{0x0,0xffffff00,0xff,0x0,0xffffffff0000,0x0000ffff00000000/*capwap bit18-22*/,0x0,0x0/*802.11 bit2-4*/}/*mask*/,TP_CAPWAPD_D_PPS /*rateLimit (pps)*/
	},
	{	/*CAPWAP data packet (UDP: sport 32769, dport 5247)*/
		"CAPWAP_D_M",
		{0x0,0x08004500,0x11,0x0,0x8001147f0000,0x02000000/*capwap bit18-22*/,0x0,0x000000000000/*802.11 bit2-3*/}/*rule*/,
		{0x0,0xffffff00,0xff,0x0,0xffffffff0000,0x3e000000/*capwap bit18-22*/,0x0,0x300000000000/*802.11 bit2-3*/}/*mask*/,TP_CAPWAPD_M_PPS/*rateLimit (pps)*/
	},
	{	/*PORTAL (UDP: dport 2000) */
		"PORTAL",
		{0x0,0x08004500,0x00000011,0x0,0x0000000007d00000}/*rule*/,
		{0x0,0xffffff00,0x000000ff,0x0,0x00000000ffff0000}/*mask*/,TP_PORTAL_PPS/*rateLimit (pps)*/
	},
	{	/*TELNET server and SSH server*/
		"TELNET_SSH_Request",
		{0x0,0x08004500,0x06,0x0,0x00160000}/*rule*/,
		{0x0,0xffffff00,0xff,0x0,0xfffe0000}/*mask*/,TP_TELNETS_PPS/*rateLimit (pps)*/
	},
	{	/*DHCP server*/
		"DHCP_Request",
		{0x0,0x08004500,0x00000011,0x0,0x0000004400430000}/*rule*/,
		{0x0,0xffffff00,0x000000ff,0x0,0x0000ffffffff0000}/*mask*/,TP_DHCPS_PPS/*rateLimit (pps)*/
	},
	{	/*RADIUS auth client and RADIUS acct client */
		"RADIUS_auth_acct_client",
		{0x0,0x08004500,0x11,0x0,0x071400000000}/*rule*/,
		{0x0,0xffffff00,0xff,0x0,0xfffe00000000}/*mask*/,TP_RADIUS_AU_C_PPS/*rateLimit(pps)*/
	},
	{	/*RADIUS auth client and RADIUS acct client*/
		"RADIUS_auth_client2",
		{0x0,0x08004500,0x11,0x0,0x066d00000000}/*rule*/,
		{0x0,0xffffff00,0xff,0x0,0xffff00000000}/*mask*/,TP_RADIUS_AU_C2_PPS/*rateLimit(pps)*/
	},
	{	/*RADIUS auth client and RADIUS acct client*/
		"RADIUS_acct_client2",
		{0x0,0x08004500,0x11,0x0,0x066e00000000}/*rule*/,
		{0x0,0xffffff00,0xff,0x0,0xffff00000000}/*mask*/,TP_RADIUS_AC_C2_PPS/*rateLimit(pps)*/
	},
	{	/*RADIUS auth client and RADIUS acct client*/
		"SNMP_client",
		{0x0,0x08004500,0x11,0x0,0x000000a10000}/*rule*/,
		{0x0,0xffffff00,0xff,0x0,0x0000ffff0000}/*mask*/,TP_SNMPC_PPS/*rateLimit(pps)*/
	},
	{	/*RADIUS auth client and RADIUS acct client*/
		"HTTPS_server",
		{0x0,0x08004500,0x06,0x0,0x000001bb0000}/*rule*/,
		{0x0,0xffffff00,0xff,0x0,0x0000ffff0000}/*mask*/,TP_HTTPSS_PPS/*rateLimit(pps)*/
	},
	{	/*TCP*/
		"TCP",
		{0x0,0x08004500,0x06}/*rule*/,
		{0x0,0xffffff00,0xff}/*mask*/,TP_TCP_PPS/*rateLimit(pps)*/
	},
	{	/*UDP*/
		"UDP",
		{0x0,0x08004500,0x11}/*rule*/,
		{0x0,0xffffff00,0xff}/*mask*/,TP_UDP_PPS/*rateLimit(pps)*/
	},
	{	/*TIPC */
		"TIPC",
		{0x0,0x88ca0000,0x00000000,0x0,0x0000000000000000}/*rule*/,
		{0x0,0xffff0000,0x00000000,0x0,0x0000000000000000}/*mask*/,TP_TIPC_PPS/*rateLimit (pps)*/
	},
};

rate_limit_t rate_limit_s = {{0,0,NULL,&rate_limit_s},&protocol_rate_limiter};

static struct oct_dev_s cr_dev;
/*********************************************************
 * DESCRIPTION:
 *  get the match rule or default rule
 * INPUT:
 *  uint32 * - skbdata -- the pointer of skb->data
 *  uint32   - len  -- skb->len: the length of skb->data
 * OUTPUT:
 *  NONE
 * RETURN:
 *  1~(MAX_MATCH_RULES_NUM-1)  -- the matched rule index 
 *  0   --  the  default rule index (no rule matched )
 * NOTE:
 *  NONE
 *
 *********************************************************/

static int get_protocol_match_rule(MATCH_TYPE * skbdata, int len){
#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif
#define ETH_VLAN_LEN 4
#define BITMASK64_LOW32		0x00000000ffffffff
#define BITMASK64_HIGH32 	0xffffffff00000000
	int i = 0;
	int j = 0;
	int data_len = len;
	int match = 0;
	int match_len = 0;
	unsigned int rules_length = 0;
	static const unsigned int match_type_len = MATCH_TYPE_LEN;
	MATCH_TYPE * rules_ptr = NULL;
	MATCH_TYPE * masks_ptr = NULL;
	unsigned int * tmp_skb_ptr = (unsigned int *)skbdata;
	MATCH_TYPE * new_skb_ptr = skbdata;
	unsigned long long * long_new_skb = NULL;
	unsigned int tag1 = 0;
	unsigned int tag2 = 0;
	
	if(NULL == skbdata)
	{
		return CVM_RCV_PROTOCOL_UNKNOWN;
	}

	if(ETH_TYPE_8021Q == ((tmp_skb_ptr[3]>>16) & 0xffff))
	{/*tag process*/
		if(ETH_TYPE_8021Q == ((tmp_skb_ptr[4]>>16) & 0xffff))
		{/*qinq process*/
			new_skb_ptr = (MATCH_TYPE *)(tmp_skb_ptr + 2);
			data_len -= ETH_VLAN_LEN*2;
			tag2 = tmp_skb_ptr[4] & 0xfff;/* vlan ctag 12 bits */
		}
		else
		{/*sigal tag process*/
			new_skb_ptr = (MATCH_TYPE *)(tmp_skb_ptr + 1);
			data_len -= ETH_VLAN_LEN;
		}	
		tag1 = tmp_skb_ptr[3] & 0xfff;/* vlan stag 12 bits */
	}
	long_new_skb = (unsigned long long *)new_skb_ptr;
	
	for(i = 1;i < MAX_MATCH_RULES_NUM;i++)
	{/*start to match rules from 1, because 0 is reserve for default*/
		if(i > cr_last_valid_rule_index)
		{
			break;
		}
		
		rules_length = protocol_rate_limiter[i].rules_length;
		if(data_len < rules_length)
		{
			continue;
		}
		else
		{
			match_len = (rules_length + match_type_len - 1) / match_type_len;/*align by match type */
		}
		
		if(match_len)
		{
			unsigned long long *long_mask_ptr = NULL;
			unsigned long long *long_rule_ptr = NULL;
			unsigned long long *long_skb_ptr  = NULL;
			match = TRUE;/*set match*/
			rules_ptr = protocol_rate_limiter[i].protocol_match_rule;
			masks_ptr = protocol_rate_limiter[i].protocol_match_mask;
			long_mask_ptr = masks_ptr;
			long_rule_ptr = rules_ptr;
			long_skb_ptr  = skbdata;
			if(long_mask_ptr[0])
			{
				if((long_skb_ptr[0] & long_mask_ptr[0])\
					!= (long_rule_ptr[0] & long_mask_ptr[0]))
				{
					match = FALSE;/*not match*/
					continue;
				}
			}

			if(long_mask_ptr[1] & BITMASK64_HIGH32)
			{
				if(((long_skb_ptr[1] & long_mask_ptr[1]) & BITMASK64_HIGH32)\
					!= ((long_rule_ptr[1] & long_mask_ptr[1]) & BITMASK64_HIGH32))
				{
					match = FALSE;/*not match*/
					continue;
				}
			}			
			
			if(long_mask_ptr[1] & BITMASK64_LOW32)
			{
				if(((long_new_skb[1] & long_mask_ptr[1]) & BITMASK64_LOW32)\
					!= ((long_rule_ptr[1] & long_mask_ptr[1]) & BITMASK64_LOW32))
				{
					match = FALSE;/*not match*/
					continue;
				}
			}
			
			for(j = 2;j < match_len;j++)
			{/* already matched element 0 and 1
			skb has some bytes reserved ,
			so even the last bytes not enough 8 bytes we can use it as 8 bytes*/
				if(masks_ptr[j])
				{
					if((new_skb_ptr[j] & masks_ptr[j])\
						!= (rules_ptr[j] & masks_ptr[j]))
					{
						match = FALSE;/*not match*/
						break;
					}
				}
			}
						
			if(match)
			{
				if(cvm_rate_limit_debug)
				{
					printk(KERN_DEBUG "traffic-policer:  match rules[%d] %s tag1 %d tag2 %d \n",
						i, protocol_rate_limiter[i].name, tag1, tag2);
					printk(KERN_DEBUG "ethertype %04x data len %d counter %d limiter %d\n", \
						tag1 ? (tag2 ? ((tmp_skb_ptr[5]>>16) & 0xffff) : \
						((tmp_skb_ptr[4]>>16) & 0xffff)) : ((tmp_skb_ptr[3]>>16) & 0xffff), \
						data_len, \
						protocol_rate_limiter[i].counter,\
						protocol_rate_limiter[i].rateLimit);
					if(cvm_rate_limit_debug < 0)
					{
						cvm_rate_limit_debug ++;
					}
				}
				return i;
			}
		}
	}
	if(cvm_rate_limit_debug)
	{
		printk(KERN_DEBUG "traffic-policer:  match rules[%d] %s ethertype %04x len %d counter %d limiter %d\n", \
			CVM_RCV_PROTOCOL_UNKNOWN, \
			protocol_rate_limiter[CVM_RCV_PROTOCOL_UNKNOWN].name, \
			(*((unsigned short *)skbdata + 6))&0xffff, \
			len, \
			protocol_rate_limiter[CVM_RCV_PROTOCOL_UNKNOWN].counter,\
			protocol_rate_limiter[CVM_RCV_PROTOCOL_UNKNOWN].rateLimit);
		if(cvm_rate_limit_debug < 0)
		{
			cvm_rate_limit_debug ++;
		}
	}
	return CVM_RCV_PROTOCOL_UNKNOWN;
}
/*static function for log output*/
static int outRules(unsigned char *prefix,unsigned int * rules)
{
#define INT_PER_LINE 8
	int i = 0;
	int len = (sizeof(MATCH_TYPE)/sizeof(unsigned int))*PACKET_MATCH_BYTE_NUM;
	if(!rules)
	{
		return -1;
	}
	printk(KERN_DEBUG "%s\n", prefix ? prefix : "????");
	printk(KERN_DEBUG "\t");
	for(i = 0;i < len;i++)
	{
		printk("%08X%s", rules[i], ((i+1)%INT_PER_LINE) ? " " : "\n");
	}
	printk("\n");
	return 0;
}
/***********************************************************
 * DESCRIPTION:
 *  get a rule by name 
 * INPUT:
 *  string - name -- the name of the rule we want to get
 * OUTPUT:
 *  NONE
 * RETEURN:
 *   -1 -- the rule not exists 
 *  >=0 -- the rule index we got
 *
 *
 ***********************************************************/
static int get_protocol_rule_byname(unsigned char * name){
#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif
    int i = 0;
	
	if(NULL == name)
	{
		return -1;
	}
    
    for(i = 0;i < MAX_MATCH_RULES_NUM;i++)
    {/*start to match rules from 1, because 0 is reserve for default*/
        if(i > cr_last_valid_rule_index)
    	{
            break;
    	}
        if(!strcmp(name,protocol_rate_limiter[i].name))
    	{	
            if(cvm_rate_limit_debug)
    		{
                printk(KERN_DEBUG "traffic-policer:  get rules[%d] byname %s limiter %d\n", \
                    i, protocol_rate_limiter[i].name, protocol_rate_limiter[i].rateLimit);
                outRules("RULE:",protocol_rate_limiter[i].protocol_match_rule);
                outRules("MASK:",protocol_rate_limiter[i].protocol_match_mask);
                if(cvm_rate_limit_debug < 0)
        		{
                    cvm_rate_limit_debug ++;
        		}
    		}
            return i;
    	}
    }
    if(cvm_rate_limit_debug)
    {
        printk(KERN_DEBUG "traffic-policer:  can't get rules  byname %s i %d last_valid_rule_index %d\n", \
            protocol_rate_limiter[i].name, i, cr_last_valid_rule_index);
        outRules("RULE:",protocol_rate_limiter[i].protocol_match_rule);
        outRules("MASK:",protocol_rate_limiter[i].protocol_match_mask);
        if(cvm_rate_limit_debug < 0)
    	{
            cvm_rate_limit_debug ++;
    	}
    }
    return -1;
}

/***********************************************************
 * DESCRIPTION:
 *  get a rule by name 
 * INPUT:
 *  string - name -- the name of the rule we want to get
 * OUTPUT:
 *  NONE
 * RETEURN:
 *   -1 -- the rule not exists 
 *  >=0 -- the rule index we got
 *
 *
 ***********************************************************/
static int get_protocol_rule_byname_fastfwd(unsigned char * name){
#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif
    int i = 0;
    int print_once = 0;
	if(NULL == name)
	{
		return -1;
	}
#ifdef	FAST_FWD_RATE_LIMIT
	if(protocol_rate_limiter_fastfwd)
	{
	    for(i = 0;i < MAX_MATCH_RULES_NUM;i++)
	    {/*start to match rules from 1, because 0 is reserve for default*/
	        if(i > cr_last_valid_rule_index)
	    	{
				if(cvm_rate_limit_debug && (!print_once))
				{
					printk(KERN_INFO "traffic-policer:  get rules byname %s for fast-forward i %d > last valid rule index %d, go on try\n", \
						name, i, cr_last_valid_rule_index);
					print_once = 1;
				}
				//break;
	    	}
	        if(!strcmp(name,protocol_rate_limiter_fastfwd[i].name))
	    	{	
	            if(cvm_rate_limit_debug)
	    		{
	                printk(KERN_DEBUG "traffic-policer:  get rules[%d] byname %s limiter %d for fast-forward\n", \
	                    i, protocol_rate_limiter_fastfwd[i].name, \
	                    protocol_rate_limiter_fastfwd[i].ratelimit_pps.rate_pps);
	                outRules("RULE:",protocol_rate_limiter_fastfwd[i].protocol_match_rule);
	                outRules("MASK:",protocol_rate_limiter_fastfwd[i].protocol_match_mask);
	                if(cvm_rate_limit_debug < 0)
	        		{
	                    cvm_rate_limit_debug ++;
	        		}
	    		}
	            return i;
	    	}
	    }
	    if(cvm_rate_limit_debug)
	    {
	        printk(KERN_DEBUG "traffic-policer: can't get rules  byname %s i %d last_valid_rule_index %d for fast-forward\n", \
	            protocol_rate_limiter_fastfwd[i].name, i, cr_last_valid_rule_index);
	        outRules("RULE:",protocol_rate_limiter_fastfwd[i].protocol_match_rule);
	        outRules("MASK:",protocol_rate_limiter_fastfwd[i].protocol_match_mask);
	        if(cvm_rate_limit_debug < 0)
	    	{
	            cvm_rate_limit_debug ++;
	    	}
	    }
	}
#endif
    return -1;
}

/**************************************************
 * DESCRIPTION:
 *  clear the receive counter of all rules, this function
 *   is used for timer
 * INPUT:
 *  long arg * the address of the global rate_limit_s
 *
 **************************************************/
void clear_receive_counter(unsigned long arg)
{
    int i = 0;
    rate_limit_t * rate_limit_ptr = (rate_limit_t *) arg;
    if(cvm_rate_limit_debug && rate_limit_enabled)
    {
        printk(KERN_DEBUG "traffic-policer: rate limit timer expire, clear receive counter, DEFAULT %d\n",
            rate_limit_ptr->protocol_rate_limiter_ptr[i].counter);
        if(cvm_rate_limit_debug < 0)
    	{
            cvm_rate_limit_debug ++;
    	}
    }
    for(i = 0;i < MAX_MATCH_RULES_NUM;i++)
    {
        rate_limit_ptr->protocol_rate_limiter_ptr[i].counter = 0;
    }	
    mod_timer(&rate_limit_ptr->rate_limit_timer, jiffies+CVM_RATE_LIMIT_SECOND);
}
/*****************************************************
 * DESCRIPTION:
 *      the main function for cavium-ethernet driver hook and call
 * INPUT:
 *  uint32 * - skbdata -- the pointer of skb->data
 *  uint32   - len  -- skb->len: the length of skb->data
 * OUTPUT:
 *  NONE
 * RETURN:
 *  -1 -- means receive counter is more than limiter,
 *          the package thould be drop
 *  0  -- means the package should pass
 *****************************************************/
int rate_limit_main(MATCH_TYPE * skbBuffPtr, int len)
{
	protocol_match_item_t * protocol_match_rule_ptr = NULL;
	int i = 0;
	static int print_once = 0;

	if(NULL == skbBuffPtr)
	{
		return -1;
	}
	if(! rate_limit_enabled)
	{
		/*cvm_rate_limit_pass_counter ++; ignore statistic when rate limit disable*/
		return 0;
	}
	if(cvm_rate_limit_debug < 0 && !print_once)
	{/*temp code */
		printk(KERN_DEBUG "traffic-policer: skb data %p len %d \n", skbBuffPtr, len);
		print_once = 1;
	}
	i = get_protocol_match_rule(skbBuffPtr, len);
	protocol_match_rule_ptr = &protocol_rate_limiter[i];
	if(protocol_match_rule_ptr)
	{
		protocol_match_rule_ptr->counter++;
		if((protocol_match_rule_ptr->counter > protocol_match_rule_ptr->rateLimit) && (protocol_match_rule_ptr->rateLimit < CVM_RATE_NO_LIMIT))
		{
			cvm_rate_limit_drop_counter ++;
			protocol_match_rule_ptr->drop_counter++;
			if(cvm_rate_limit_debug)
			{
				printk(KERN_DEBUG "traffic-policer: rate limit main: rules[%d] %s ethertype %04x len %d counter %d limiter %d drop the packet, drop counter %lld\n", \
					i, protocol_rate_limiter[i].name, (*((unsigned short *)skbBuffPtr + 6))&0xffff, len, protocol_match_rule_ptr->counter, protocol_match_rule_ptr->rateLimit,
					cvm_rate_limit_drop_counter);
				if(cvm_rate_limit_debug < 0)
				{
					cvm_rate_limit_debug ++;
				}
			}
			return -1;
		}
	}
	cvm_rate_limit_pass_counter ++;
	protocol_match_rule_ptr->pass_counter++;
	return 0;
}

static int get_mask_len(MATCH_TYPE mask)
{
	int i = 0;
	unsigned char *ptr = (unsigned char *)&mask;
	for(i = MATCH_TYPE_LEN; i > 0; i--)
	{
		if(ptr[i - 1])
		{
			return i;
		}
	}
	return MATCH_TYPE_LEN;
}
/*********************************************************
 * DESCRIPTION:
 *  ioctl functions called for app processer
 * INPUT:
 *  uint32 - cmd  -- command code
 *  long    - arg   -- the address of the ioctl_struct from user
 * OUTPUT:
 *  *arg  --  the value of user address arg
 * RETURN:
 *  0 -- set or get ok
 *  -errno -- error occured or set or get failed ,not allowed , etc.
 *  
 *
 *********************************************************/
int cr_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    struct cr_ioctl_struct rule_struct ;
    void __user* argp = (void __user*)arg;
	unsigned int flag = 0;
    int i = 0;
    
#ifdef DYNAMIC_MAJOR_NUM
#define CMD _IOC_NR(cmd)
#else
#define CMD cmd
#endif  
    memset(&rule_struct,0,sizeof(protocol_match_item_t));
    if(cvm_rate_limit_debug)
    {
        printk(KERN_DEBUG "traffic-policer: cr_ioctl cmd %#X, type %d nr %d\n", \
                cmd, _IOC_TYPE(cmd), _IOC_NR(cmd));
        if(cvm_rate_limit_debug < 0)
    	{
            cvm_rate_limit_debug ++;
    	}
    }
    if(_IOC_TYPE(cmd) != cr_major_num){return -ENOTTY;}
    if(_IOC_NR(cmd) > CR_MAXNR){return -ENOTTY;}

    if (copy_from_user(&rule_struct, argp, sizeof(rule_struct))){
            return -EFAULT;
    }

    if((CMD != CRMODIFYNAME) && (CMD != CRGETRULEBYINDEX) \
        &&(CMD != CRENABLESET)&&(CMD != CRENABLEGET)\
        &&(CMD != CRSTACOUNTGET)&&(CMD != CRSTACOUNTCLEAR)\
        &&(CMD != CRDMESGENABLESET)&&(CMD != CRCLEARUDFRULES)\
        &&(CMD != CRGETRULE4FFWD))
    {
        if(cvm_rate_limit_debug)
    	{
            printk(KERN_DEBUG "traffic-policer: cr_ioctl cmd %#x, rule name %s rule length %d rate limiter %d\n", \
                cmd, rule_struct.u.protocol_rule.name, rule_struct.u.protocol_rule.rules_length, rule_struct.u.protocol_rule.rateLimit);
            outRules("RULE:", rule_struct.u.protocol_rule.protocol_match_rule);
            outRules("MASK:", rule_struct.u.protocol_rule.protocol_match_mask);
            if(cvm_rate_limit_debug < 0)
    		{
                cvm_rate_limit_debug ++;
    		}
    	}
        i = get_protocol_rule_byname(rule_struct.u.protocol_rule.name);
    }
	if(CRGETRULE4FFWD == CMD)
	{
#ifdef	FAST_FWD_RATE_LIMIT
		if(protocol_rate_limiter_fastfwd)
		{
			i = get_protocol_rule_byname_fastfwd(rule_struct.u.protocol_rule.name);
		}
#endif
	}

    switch (CMD) {		
        case CRENABLESET:
			if(rule_struct.u.e.flag & CR_SET_FLAG_CVM_ETH)
			{
            	rate_limit_enabled = rule_struct.u.e.enable;
			}
#ifdef  FAST_FWD_RATE_LIMIT
			if((rule_struct.u.e.flag & CR_SET_FLAG_FAST_FWD) &&  (global_various_ptr_fastfwd))
			{
				global_various_ptr_fastfwd->cvm_rate_limit_enabled = rule_struct.u.e.enable;
			}
#endif
            if(cvm_rate_limit_debug)
    		{
                printk(KERN_DEBUG "traffic-policer: rate limit is set %s and %s for fast-forward\n", \
					rate_limit_enabled ? "enabled" : "disabled" , rate_limit_fastfwd_enabled ? "enabled" : "disabled" );
                if(cvm_rate_limit_debug < 0)
        		{
                    cvm_rate_limit_debug ++;
        		}
    		}
            break;		
        case CRENABLEGET:
            rule_struct.u.enable = rate_limit_enabled;
#ifdef  FAST_FWD_RATE_LIMIT
			if((global_various_ptr_fastfwd) && (global_various_ptr_fastfwd->cvm_rate_limit_enabled))
			{
				rule_struct.u.enable |= CR_SET_FLAG_FAST_FWD;
			}
#endif
            break;
        case CRADDRULES:
            if((i > 0)||(!strcmp(rule_struct.u.protocol_rule.name, "DEFAULT")))
    		{
                return -EEXIST;
    		}
            if(0 == cr_next_valid_index)
    		{
                return -EFBIG;
    		}
            if(cvm_rate_limit_debug)
    		{
                printk(KERN_DEBUG "traffic-policer: add rule %d name %s\n", cr_next_valid_index, rule_struct.u.protocol_rule.name);
                if(cvm_rate_limit_debug < 0)
        		{
                    cvm_rate_limit_debug ++;
        		}
    		}
            memcpy(&protocol_rate_limiter[cr_next_valid_index], \
                &rule_struct.u.protocol_rule, sizeof(protocol_match_item_t));
            protocol_rate_limiter[cr_next_valid_index].counter = 0;
#ifdef  FAST_FWD_RATE_LIMIT
			if(protocol_rate_limiter_fastfwd)
			{
				memcpy( &protocol_rate_limiter_fastfwd[cr_next_valid_index].name,\
					&protocol_rate_limiter[cr_next_valid_index].name, PROTOCOL_NAME_LEN);
				
				memcpy(&protocol_rate_limiter_fastfwd[cr_next_valid_index].protocol_match_rule, \
	                &protocol_rate_limiter[cr_next_valid_index].protocol_match_rule, \
	                PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
				
				memcpy(&protocol_rate_limiter_fastfwd[cr_next_valid_index].protocol_match_mask, \
	                &protocol_rate_limiter[cr_next_valid_index].protocol_match_mask, \
	                PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
				
				cvm_car_pps_set(&protocol_rate_limiter_fastfwd[cr_next_valid_index].ratelimit_pps,\
					protocol_rate_limiter[cr_next_valid_index].rateLimit);
				
				protocol_rate_limiter_fastfwd[cr_next_valid_index].rules_length = \
					((protocol_rate_limiter[cr_next_valid_index].rules_length + MATCH_TYPE_LEN -1)/MATCH_TYPE_LEN);
				
			}
#endif
            cr_last_valid_rule_index = cr_next_valid_index;
#ifdef  FAST_FWD_RATE_LIMIT
			if(protocol_rate_limiter_fastfwd)
			{
				global_various_ptr_fastfwd->last_valid_index = cr_last_valid_rule_index;
			}
#endif
    		
			if(cr_last_valid_rule_index == MAX_MATCH_RULES_NUM - 1)
    		{
                cr_next_valid_index = 0;
    		}
        	else
    		{
                cr_next_valid_index += 1;
    		}
    		
            if(cvm_rate_limit_debug)
    		{
                printk(KERN_DEBUG "traffic-policer: next valid %d last valid %d\n", cr_next_valid_index, cr_last_valid_rule_index);
        					
                if(cvm_rate_limit_debug < 0)
        		{
                    cvm_rate_limit_debug ++;
        		}
    		}
            break;
        case CRDELRULES:
            if(i >= CVM_RATE_LIMIT_DEFAULT_RULE_NUM)
            {/*check the item is can be deleted*/
                if(cvm_rate_limit_debug)
        		{
                    printk(KERN_DEBUG "traffic-policer: delete rule %d name %s\n", i, protocol_rate_limiter[i].name);
                    if(cvm_rate_limit_debug < 0)
        			{
                        cvm_rate_limit_debug ++;
        			}
        		}

        						
                if(i != cr_last_valid_rule_index)
        		{
        			
                    memcpy(&protocol_rate_limiter[i], &protocol_rate_limiter[i+1], \
                        (cr_last_valid_rule_index - i)*sizeof(protocol_match_item_t));
#ifdef  FAST_FWD_RATE_LIMIT
					if(protocol_rate_limiter_fastfwd)
					{
						memcpy( &protocol_rate_limiter_fastfwd[i], &protocol_rate_limiter[i+1], \
							(cr_last_valid_rule_index - i)*sizeof(protocol_match_item_fastfwd_t));
					}
#endif
            							
        		}

                memset(&protocol_rate_limiter[cr_last_valid_rule_index], 0, \
                        sizeof(protocol_match_item_t));
#ifdef  FAST_FWD_RATE_LIMIT
				if(protocol_rate_limiter_fastfwd)
				{
					memset( &protocol_rate_limiter_fastfwd[cr_last_valid_rule_index], 0, \
						sizeof(protocol_match_item_fastfwd_t));
				}
#endif
    			
                cr_next_valid_index = cr_last_valid_rule_index;
                cr_last_valid_rule_index -= 1;
			
#ifdef  FAST_FWD_RATE_LIMIT
				if(protocol_rate_limiter_fastfwd)
				{
					global_various_ptr_fastfwd->last_valid_index = cr_last_valid_rule_index;
				}
#endif
    			
                if(cvm_rate_limit_debug)
        		{
                    printk(KERN_DEBUG "traffic-policer: next valid %d last valid %d\n", cr_next_valid_index, cr_last_valid_rule_index);
                    if(cvm_rate_limit_debug < 0)
        			{
                        cvm_rate_limit_debug ++;
        			}
        		}
    		}
            else if(i >= 0)
    		{
                return -EPERM;
    		}
        	else
    		{
                return -ENOENT;
    		}
            break;
        case CRMODIFYRULES:
            if(i >= CVM_RATE_LIMIT_DEFAULT_RULE_NUM){
                memcpy(protocol_rate_limiter[i].protocol_match_rule,\
                    rule_struct.u.protocol_rule.protocol_match_rule,\
                    PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
                protocol_rate_limiter[i].counter = 0;
 #ifdef  FAST_FWD_RATE_LIMIT
				if(protocol_rate_limiter_fastfwd)
				{
					memcpy( &protocol_rate_limiter_fastfwd[i].protocol_match_rule, \
						protocol_rate_limiter[i].protocol_match_rule, \
						PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
				}
#endif
    		}
            else if(i >= 0)
    		{
                return -EPERM;
    		}
        	else
    		{
                return -ENOENT;
    		}
            break;
        case CRMODIFYMASKS:
            if(i >= CVM_RATE_LIMIT_DEFAULT_RULE_NUM){
                int j = 0;
                memcpy(protocol_rate_limiter[i].protocol_match_mask, \
                    rule_struct.u.protocol_rule.protocol_match_mask,\
                    PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
                protocol_rate_limiter[i].counter = 0;
				
 #ifdef  FAST_FWD_RATE_LIMIT
				if(protocol_rate_limiter_fastfwd)
				{
					memcpy( &protocol_rate_limiter_fastfwd[i].protocol_match_mask, \
						protocol_rate_limiter[i].protocol_match_mask, \
						PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
				}
#endif
				for(j = PACKET_MATCH_BYTE_NUM; j > 0;j--)
        		{
                    if(protocol_rate_limiter[i].protocol_match_mask[j-1])
        			{
						protocol_rate_limiter[i].rules_length = ((j - 1) * MATCH_TYPE_LEN) \
							+ get_mask_len(protocol_rate_limiter[i].protocol_match_mask[j-1]) ;
						
						rule_struct.u.protocol_rule.rules_length = protocol_rate_limiter[i].rules_length ;
#ifdef	FAST_FWD_RATE_LIMIT
						if(protocol_rate_limiter_fastfwd)
						{
							protocol_rate_limiter_fastfwd[i].rules_length = j;
						}
#endif

						break;
        			}
        		}
    		}
            else if(i >= 0)
    		{
                return -EPERM;
    		}
        	else
    		{
                return -ENOENT;
    		}
            break;
        case CRMODIFYLIMITER:
            if(i >= 0)
    		{
                protocol_rate_limiter[i].rateLimit = rule_struct.u.protocol_rule.rateLimit;
                protocol_rate_limiter[i].counter = 0;
#ifdef	FAST_FWD_RATE_LIMIT
				if(protocol_rate_limiter_fastfwd)
				{
					cvm_car_pps_set(&protocol_rate_limiter_fastfwd[i].ratelimit_pps, protocol_rate_limiter[i].rateLimit);
				}
#endif
        	}			
        	else
    		{
                return -ENOENT;
    		}
            break;
        case CRMODIFYNAME:
            i = get_protocol_rule_byname(rule_struct.u.name.new_name);
            if(i >= 0)
    		{
                return -EEXIST;
    		}
            i = get_protocol_rule_byname(rule_struct.u.name.old_name);
            if(i >= 0)
    		{
                memcpy(protocol_rate_limiter[i].name, rule_struct.u.name.new_name, PROTOCOL_NAME_LEN);
#ifdef	FAST_FWD_RATE_LIMIT
				if(protocol_rate_limiter_fastfwd)
				{
					memcpy(protocol_rate_limiter_fastfwd[i].name, protocol_rate_limiter[i].name, PROTOCOL_NAME_LEN);
				}
#endif
    		}
        	else
    		{
                return -ENOENT;
    		}
            break;
        case CRGETRULE:
            if((i > 0)||(!strcmp(rule_struct.u.protocol_rule.name, "DEFAULT")))
        	{				
                if(cvm_rate_limit_debug)
        		{
                    printk(KERN_DEBUG "traffic-policer: get rule %d name %s\n", i, protocol_rate_limiter[i].name);
                    if(cvm_rate_limit_debug < 0)
        			{
                        cvm_rate_limit_debug ++;
        			}
        		}
                memcpy(&rule_struct.u.protocol_rule, &protocol_rate_limiter[i], sizeof(protocol_match_item_t));
        	}			
        	else
    		{
                return -ENOENT;
    		}
            break;
		case CRGETRULE4FFWD:			
#ifdef	FAST_FWD_RATE_LIMIT
            if((i > 0)||(!strcmp(rule_struct.u.protocol_rule.name, "DEFAULT")))
        	{			
        		if(protocol_rate_limiter_fastfwd && ((!i) || protocol_rate_limiter_fastfwd[i].rules_length))
        		{
	                if(cvm_rate_limit_debug)
	        		{
	                    printk(KERN_DEBUG "traffic-policer: get rule %d name %s for fast-forward\n", i, protocol_rate_limiter_fastfwd[i].name);
	                    if(cvm_rate_limit_debug < 0)
	        			{
	                        cvm_rate_limit_debug ++;
	        			}
	        		}
	                memcpy(&rule_struct.u.protocol_rule.name, &protocol_rate_limiter_fastfwd[i].name, PROTOCOL_NAME_LEN);
					
	                rule_struct.u.protocol_rule.rules_length = protocol_rate_limiter_fastfwd[i].rules_length*8;
					
	                memcpy(&rule_struct.u.protocol_rule.protocol_match_rule, \
						&protocol_rate_limiter_fastfwd[i].protocol_match_rule, \
						PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
					
	                memcpy(&rule_struct.u.protocol_rule.protocol_match_mask, \
						&protocol_rate_limiter_fastfwd[i].protocol_match_mask, \
						PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
					
	                rule_struct.u.protocol_rule.rateLimit = protocol_rate_limiter_fastfwd[i].ratelimit_pps.rate_pps;
					
	                rule_struct.u.protocol_rule.drop_counter = protocol_rate_limiter_fastfwd[i].drop_counter;
					
	                rule_struct.u.protocol_rule.pass_counter = protocol_rate_limiter_fastfwd[i].pass_counter;
        		}
				else
				{
					return -ENOENT;
				}
        	}			
        	else
    		{
                return -ENOENT;
    		}
#endif
            break;
        case CRGETRULEBYINDEX:
            i = rule_struct.u.index;
            if((i >= MAX_MATCH_RULES_NUM)||(i < 0))
    		{
                return -EINVAL;
    		}
            if(!i || protocol_rate_limiter[i].rules_length)
    		{
                if(cvm_rate_limit_debug)
        		{
                    printk(KERN_DEBUG "traffic-policer: get rule %d name %s\n", i, protocol_rate_limiter[i].name);
        			
                    if(cvm_rate_limit_debug < 0)
        			{
                        cvm_rate_limit_debug ++;
        			}
        		}
                memcpy(&rule_struct.u.protocol_rule, &protocol_rate_limiter[i], sizeof(protocol_match_item_t));
    		}
        	else
    		{
                return -ENOENT;
    		}
            break;
        case CRGETRULEBYIDX4FFWD:
#ifdef	FAST_FWD_RATE_LIMIT
            i = rule_struct.u.index;
            if((i >= MAX_MATCH_RULES_NUM)||(i < 0))
    		{
                return -EINVAL;
    		}
            if(/*!i ||*/ (protocol_rate_limiter_fastfwd && ((!i) || protocol_rate_limiter_fastfwd[i].rules_length)))
    		{
                if(cvm_rate_limit_debug)
        		{
                    printk(KERN_DEBUG "traffic-policer: get rule %d name %s from fast-forward\n", i, protocol_rate_limiter[i].name);
        			
                    if(cvm_rate_limit_debug < 0)
        			{
                        cvm_rate_limit_debug ++;
        			}
        		}
                memcpy(&rule_struct.u.protocol_rule.name, &protocol_rate_limiter_fastfwd[i].name, PROTOCOL_NAME_LEN);
				
                rule_struct.u.protocol_rule.rules_length = protocol_rate_limiter_fastfwd[i].rules_length*8;
				
                memcpy(&rule_struct.u.protocol_rule.protocol_match_rule, \
					&protocol_rate_limiter_fastfwd[i].protocol_match_rule, \
					PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
				
                memcpy(&rule_struct.u.protocol_rule.protocol_match_mask, \
					&protocol_rate_limiter_fastfwd[i].protocol_match_mask, \
					PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
				
                rule_struct.u.protocol_rule.rateLimit = protocol_rate_limiter_fastfwd[i].ratelimit_pps.rate_pps;
				
                rule_struct.u.protocol_rule.drop_counter = protocol_rate_limiter_fastfwd[i].drop_counter;
				
                rule_struct.u.protocol_rule.pass_counter = protocol_rate_limiter_fastfwd[i].pass_counter;
				
    		}
        	else
    		{
                return -ENOENT;
    		}
#endif
            break;	
        case CRCHECKDEFAULT:
            i = rule_struct.u.index;
            if((i >= CVM_RATE_LIMIT_DEFAULT_RULE_NUM)||(i < 0))
    		{
                return -EINVAL;
    		}
            if(rate_default_limiter[i] == protocol_rate_limiter[i].rateLimit)
    		{
                return -EEXIST;
    		}
        	else
    		{
                memcpy(rule_struct.u.protocol_rule.name, protocol_rate_limiter[i].name, PROTOCOL_NAME_LEN);
                rule_struct.u.protocol_rule.rateLimit = protocol_rate_limiter[i].rateLimit;
    		}
            break;
        case CRSTACOUNTGET:
			flag = rule_struct.u.flag;
			if(cvm_rate_limit_debug)
    		{
                printk(KERN_DEBUG "traffic-policer: show statistic counter, befor get flag %d\n",
						rule_struct.u.flag);
    			
                if(cvm_rate_limit_debug < 0)
        		{
                    cvm_rate_limit_debug ++;
        		}
    		}
			if(flag & CR_SET_FLAG_CVM_ETH)
			{
	            rule_struct.u.c.drop_counter = cvm_rate_limit_drop_counter;
	            rule_struct.u.c.pass_counter = cvm_rate_limit_pass_counter;
			}
#ifdef FAST_FWD_RATE_LIMIT
			else if(global_various_ptr_fastfwd)
			{
		        rule_struct.u.c.drop_counter = global_various_ptr_fastfwd->cvm_rate_limit_drop_counter;
		        rule_struct.u.c.pass_counter = global_various_ptr_fastfwd->cvm_rate_limit_pass_counter;	
			}
			else
			{
		        rule_struct.u.c.drop_counter = 0;
		        rule_struct.u.c.pass_counter = 0;
			}
#endif
			if(cvm_rate_limit_debug)
    		{
                printk(KERN_DEBUG "traffic-policer: show statistic counter, after get drop counter %lld pass counter %lld flag %d\n",
					rule_struct.u.c.drop_counter,
					rule_struct.u.c.pass_counter,
                    rule_struct.u.flag);
    			
                if(cvm_rate_limit_debug < 0)
        		{
                    cvm_rate_limit_debug ++;
        		}
    		}
            break;
        case CRSTACOUNTCLEAR:
			flag = rule_struct.u.flag;
			if(cvm_rate_limit_debug)
    		{
                printk(KERN_DEBUG "traffic-policer: clear statistic counter, before clear flag %d\n", flag);
    			
                if(cvm_rate_limit_debug < 0)
        		{
                    cvm_rate_limit_debug ++;
        		}
    		}
			
			for(i = 0; i < MAX_MATCH_RULES_NUM; i++)
    		{
				if(flag & CR_SET_FLAG_CVM_ETH)
				{
	                protocol_rate_limiter[i].drop_counter = 0;
	                protocol_rate_limiter[i].pass_counter = 0;
				}
#ifdef	FAST_FWD_RATE_LIMIT
				if(protocol_rate_limiter_fastfwd && (flag & CR_SET_FLAG_FAST_FWD))
				{
					protocol_rate_limiter_fastfwd[i].drop_counter = 0;
					protocol_rate_limiter_fastfwd[i].pass_counter = 0;
				}
#endif
    		}
			
			if(flag & CR_SET_FLAG_CVM_ETH)
			{
		        rule_struct.u.c.drop_counter = cvm_rate_limit_drop_counter;
		        rule_struct.u.c.pass_counter = cvm_rate_limit_pass_counter;	
	            cvm_rate_limit_drop_counter = 0;
	            cvm_rate_limit_pass_counter = 0;
			}
#ifdef FAST_FWD_RATE_LIMIT
			else if(global_various_ptr_fastfwd)
			{
		        rule_struct.u.c.drop_counter = global_various_ptr_fastfwd->cvm_rate_limit_drop_counter;
		        rule_struct.u.c.pass_counter = global_various_ptr_fastfwd->cvm_rate_limit_pass_counter;	
			}
			else
			{
		        rule_struct.u.c.drop_counter = 0;
		        rule_struct.u.c.pass_counter = 0;
			}
			if((flag & CR_SET_FLAG_FAST_FWD) && (global_various_ptr_fastfwd))
			{
				global_various_ptr_fastfwd->cvm_rate_limit_drop_counter = 0;
				global_various_ptr_fastfwd->cvm_rate_limit_pass_counter = 0;
			}
#endif    		
            if(cvm_rate_limit_debug)
    		{
                printk(KERN_DEBUG "traffic-policer: clear statistic counter, drop %lld pass %lld flag %d\n",
                    rule_struct.u.c.drop_counter,
                    rule_struct.u.c.pass_counter,
                    flag);
    			
                if(cvm_rate_limit_debug < 0)
        		{
                    cvm_rate_limit_debug ++;
        		}
    		}
            break;
        case CRDMESGENABLESET:
            cvm_rate_limit_debug = (int)rule_struct.u.enable;
            break;
        case CRCLEARUDFRULES:
            for(i = CVM_RATE_LIMIT_DEFAULT_RULE_NUM; i < MAX_MATCH_RULES_NUM; i++)
    		{
                if(cvm_rate_limit_debug)
        		{
                    printk(KERN_DEBUG "traffic-policer: delete rule %d name %s\n", i, protocol_rate_limiter[i].name);
                    if(cvm_rate_limit_debug < 0)
        			{
                        cvm_rate_limit_debug ++;
					}
				}
				memset(&protocol_rate_limiter[i], 0, sizeof(protocol_match_item_t));	
#ifdef	FAST_FWD_RATE_LIMIT
				if(protocol_rate_limiter_fastfwd)
				{
					memset(&protocol_rate_limiter_fastfwd[i], 0, sizeof(protocol_match_item_fastfwd_t));
				}
#endif

			}
			for(i = 0; i < CVM_RATE_LIMIT_DEFAULT_RULE_NUM; i++)
    		{
        		protocol_rate_limiter[i].rateLimit = rate_default_limiter[i];
#ifdef	FAST_FWD_RATE_LIMIT
				if(protocol_rate_limiter_fastfwd)
				{
					cvm_car_pps_set(&protocol_rate_limiter_fastfwd[i].ratelimit_pps, protocol_rate_limiter[i].rateLimit);
				}
#endif
			}
			
			cr_next_valid_index = CVM_RATE_LIMIT_DEFAULT_RULE_NUM;
			
			cr_last_valid_rule_index = cr_next_valid_index - 1;
			
#ifdef  FAST_FWD_RATE_LIMIT
			if(protocol_rate_limiter_fastfwd)
			{
				global_various_ptr_fastfwd->last_valid_index = cr_last_valid_rule_index;
			}
#endif
			if(cvm_rate_limit_debug)
			{
				printk(KERN_DEBUG "traffic-policer: next valid %d last valid %d after clear udf rules and reset default limiter\n", cr_next_valid_index, cr_last_valid_rule_index);
				if(cvm_rate_limit_debug < 0)
				{
					cvm_rate_limit_debug ++;
				}
			}
			break;
        default:
            rule_struct.u.protocol_rule.rules_length = 0;
            break;
    }
    copy_to_user(argp, &rule_struct, sizeof(rule_struct));
    return 0;
}

long cr_compat_ioctl (struct file *file, unsigned int cmd, unsigned long arg)
{
     int rval;
 
     lock_kernel();
     rval = cr_ioctl(file->f_dentry->d_inode, file, cmd, arg);
     unlock_kernel();
 
     return (rval == -EINVAL) ? -ENOIOCTLCMD : rval;
}


int cr_open (struct inode *inode, struct file *file)
{
    struct oct_dev_s *dev = NULL;

    dev = container_of(inode->i_cdev, struct oct_dev_s, cdev);
    file->private_data = dev;
    dev->size = 0;
    return 0;
}


int cr_release (struct inode *inode, struct file *file)
{
    return 0;
}


static struct file_operations cvm_rate_limit_fops = {
    .owner  = THIS_MODULE,
    .ioctl  = cr_ioctl,
    .compat_ioctl = cr_compat_ioctl,
    .open   = cr_open,
    .release  = cr_release,
    /*.llseek = no_llseek	*/
};
static void cvm_rate_limit_timer_init(void)
{
    init_timer(&rate_limit_s.rate_limit_timer);
    rate_limit_s.rate_limit_timer.function = clear_receive_counter;
    rate_limit_s.rate_limit_timer.data = (unsigned long)&rate_limit_s;
    mod_timer(&rate_limit_s.rate_limit_timer, jiffies+CVM_RATE_LIMIT_SECOND);	
    rcv_rate_limit_hook = rate_limit_main;
    return ;
}

static int __init cvm_rate_limit_init(void)
{
    int result;
    dev_t cr_devt;
    int i = 0;
    int j = 0;
	int old_enable_fastfwd = 0;
    
    cr_devt = MKDEV(cr_major_num,cr_minor_num);
    result = register_chrdev_region(cr_devt,1,DRV_NAME);
    printk(KERN_INFO DRV_NAME "traffic-policer: register major dev [%d]\n",MAJOR(cr_devt));

    if (result < 0) {
        printk(KERN_WARNING DRV_NAME "traffic-policer: register_chrdev_region err[%d]\n",result);
        return 0;
    } 
    cdev_init(&(cr_dev.cdev),&cvm_rate_limit_fops);
    cr_dev.cdev.owner = THIS_MODULE;
    result = cdev_add(&(cr_dev.cdev),cr_devt,1);
    if (result < 0) {
        printk(KERN_WARNING DRV_NAME "traffic-policer: cdev_add err[%d]\n",result);
    } else {
        printk(KERN_INFO DRV_NAME "traffic-policer: loaded successfully.\n");
    }
    cr_next_valid_index = 0;
	
#ifdef  FAST_FWD_RATE_LIMIT


	/* get meminfo from fastfwd */
	result = cr_get_fastfwd_ratelimit_info();
	if(result)
	{
		printk(KERN_WARNING "traffic-policer: get meminfo from fast-forward module failed!\n");
	}
	else
	{
		printk(KERN_INFO "traffic-policer: get meminfo base addr %p from fast-forward module successed!\n", protocol_rate_limiter_fastfwd_mem_base);

		if(protocol_rate_limiter_fastfwd)
		{
			old_enable_fastfwd = global_various_ptr_fastfwd->cvm_rate_limit_enabled;
			global_various_ptr_fastfwd->cvm_rate_limit_enabled = 0; /*temp disable while inital */
		}
	}
		
#endif

    for(i = 0; i < CVM_RATE_LIMIT_DEFAULT_RULE_NUM; i++)
    {
        protocol_rate_limiter[i].rateLimit = rate_default_limiter[i];
#ifdef  FAST_FWD_RATE_LIMIT
		if(protocol_rate_limiter_fastfwd)
		{

			memset(&protocol_rate_limiter_fastfwd[i], 0, sizeof(protocol_match_item_fastfwd_t));
			
			cvm_car_pps_set(&protocol_rate_limiter_fastfwd[i].ratelimit_pps, rate_default_limiter[i]);
			
			memcpy(protocol_rate_limiter_fastfwd[i].name, protocol_rate_limiter[i].name, PROTOCOL_NAME_LEN);
			
			memcpy(protocol_rate_limiter_fastfwd[i].protocol_match_rule, protocol_rate_limiter[i].protocol_match_rule, \
				PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
			
			memcpy(protocol_rate_limiter_fastfwd[i].protocol_match_mask, protocol_rate_limiter[i].protocol_match_mask, \
				PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
		}
#endif
        for(j = PACKET_MATCH_BYTE_NUM; j > 0; j--)
    	{
			if(protocol_rate_limiter[i].protocol_match_mask[j-1])
			{
				protocol_rate_limiter[i].rules_length = ((j - 1) * MATCH_TYPE_LEN) \
							+ get_mask_len(protocol_rate_limiter[i].protocol_match_mask[j-1]);
#ifdef  FAST_FWD_RATE_LIMIT
				if(protocol_rate_limiter_fastfwd)
				{
					protocol_rate_limiter_fastfwd[i].rules_length = j;
				}
#endif
				break;
			}		
		}
	}


	cr_last_valid_rule_index = CVM_RATE_LIMIT_DEFAULT_RULE_NUM - 1;
/* if some udf rules initallized , */
    for(i = CVM_RATE_LIMIT_DEFAULT_RULE_NUM; i < MAX_MATCH_RULES_NUM; i++)
    {
#ifdef  FAST_FWD_RATE_LIMIT
		if(protocol_rate_limiter_fastfwd)
		{
			memset(&protocol_rate_limiter_fastfwd[i], 0, sizeof(protocol_match_item_fastfwd_t));
		}
#endif
        /*protocol_rate_limiter[i].next = i + 1;*/
		if(!cr_next_valid_index)
		{
			
	        if(!(protocol_rate_limiter[i].rules_length))
	    	{
	            cr_next_valid_index = i;
	            //break;
	    	}
	        else if(protocol_rate_limiter[i].rules_length)
	    	{
	    	
#ifdef  FAST_FWD_RATE_LIMIT
				if(protocol_rate_limiter_fastfwd)
				{
					
					cvm_car_pps_set(&protocol_rate_limiter_fastfwd[i].ratelimit_pps, protocol_rate_limiter[i].rateLimit);
					
					memcpy(protocol_rate_limiter_fastfwd[i].name, protocol_rate_limiter[i].name, PROTOCOL_NAME_LEN);
					
					memcpy(protocol_rate_limiter_fastfwd[i].protocol_match_rule, protocol_rate_limiter[i].protocol_match_rule, \
						PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
					
					memcpy(protocol_rate_limiter_fastfwd[i].protocol_match_mask, protocol_rate_limiter[i].protocol_match_mask, \
						PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
				}
#endif
				for(j = PACKET_MATCH_BYTE_NUM; j > 0; j--)
				{
					if(protocol_rate_limiter[i].protocol_match_mask[j-1])
					{
						protocol_rate_limiter[i].rules_length = ((j - 1) * MATCH_TYPE_LEN) \
									+ get_mask_len(protocol_rate_limiter[i].protocol_match_mask[j-1]);
#ifdef  FAST_FWD_RATE_LIMIT
						if(protocol_rate_limiter_fastfwd)
						{
							protocol_rate_limiter_fastfwd[i].rules_length = j;
						}
#endif
						break;
					}		
				}
				
	            cr_last_valid_rule_index = i;
				
	    	}
		}
		else if(protocol_rate_limiter[i].rules_length)
		{	/*move to first blank room */

			memcpy(&protocol_rate_limiter[cr_next_valid_index], &protocol_rate_limiter[i], sizeof(protocol_match_item_t));
			
			memset(&protocol_rate_limiter[i], 0, sizeof(protocol_match_item_t));
			
#ifdef  FAST_FWD_RATE_LIMIT
			if(protocol_rate_limiter_fastfwd)
			{
				
				cvm_car_pps_set(&protocol_rate_limiter_fastfwd[cr_next_valid_index].ratelimit_pps, protocol_rate_limiter[cr_next_valid_index].rateLimit);
				
				memcpy(protocol_rate_limiter_fastfwd[cr_next_valid_index].name, protocol_rate_limiter[cr_next_valid_index].name, PROTOCOL_NAME_LEN);
				
				memcpy(protocol_rate_limiter_fastfwd[cr_next_valid_index].protocol_match_rule, protocol_rate_limiter[cr_next_valid_index].protocol_match_rule, \
					PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
				
				memcpy(protocol_rate_limiter_fastfwd[cr_next_valid_index].protocol_match_mask, protocol_rate_limiter[cr_next_valid_index].protocol_match_mask, \
					PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
			}
#endif
			for(j = PACKET_MATCH_BYTE_NUM; j > 0; j--)
			{
				if(protocol_rate_limiter[cr_next_valid_index].protocol_match_mask[j-1])
				{
					protocol_rate_limiter[cr_next_valid_index].rules_length = ((j - 1) * MATCH_TYPE_LEN) \
								+ get_mask_len(protocol_rate_limiter[cr_next_valid_index].protocol_match_mask[j-1]);
#ifdef  FAST_FWD_RATE_LIMIT
					if(protocol_rate_limiter_fastfwd)
					{
						protocol_rate_limiter_fastfwd[cr_next_valid_index].rules_length = j;
					}
#endif
					break;
				}		
			}
			
	        cr_last_valid_rule_index = cr_next_valid_index;
			
			cr_next_valid_index = i;

    	}
	}

#ifdef  FAST_FWD_RATE_LIMIT
	if(protocol_rate_limiter_fastfwd)
	{
		global_various_ptr_fastfwd->last_valid_index = cr_last_valid_rule_index;
		global_various_ptr_fastfwd->cvm_rate_limit_enabled = old_enable_fastfwd;
	}
#endif	
    

    cvm_rate_limit_timer_init();

    return 0;
}
static void __exit cvm_rate_limit_exit(void)
{   
    rcv_rate_limit_hook = NULL;
    del_timer(&rate_limit_s.rate_limit_timer);
    printk(KERN_INFO DRV_NAME "traffic-policer: Unregister MajorNum[%d]\n",MAJOR(cr_dev.cdev.dev));	
    unregister_chrdev_region(cr_dev.cdev.dev,1);	
    cdev_del(&(cr_dev.cdev));
    printk(KERN_INFO DRV_NAME "traffic-policer: unloaded\n");
    return ;
}

module_init(cvm_rate_limit_init);
module_exit(cvm_rate_limit_exit);
MODULE_DESCRIPTION(DRV_DESCRIPTION);
MODULE_AUTHOR(DRV_COPYRIGHT);
MODULE_LICENSE(DRV_LICENSE);

#ifdef __cplusplus
}
#endif

