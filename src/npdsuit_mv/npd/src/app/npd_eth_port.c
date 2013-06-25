
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
* npd_eth_port.c
*
*
* CREATOR:
*		chenb@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD by ETHERNET PORT module.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.324 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
/*
  Network Platform Daemon Ethernet Port Management
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <linux/if_vlan.h>
#include <linux/sockios.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <errno.h>
#include <sys/mman.h> 
#include <sys/sysinfo.h>    /* for sysinfo() */

#include "sysdef/npd_sysdef.h"
#include "sysdef/returncode.h"
#include "util/npd_list.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd/nam/npd_amapi.h"
#include "nam/nam_eth_api.h"
#include "dbus/npd/npd_dbus_def.h"
#include "cvm/ethernet-ioctl.h"
#include "npd_product.h"
#include "npd_log.h"
#include "npd_c_slot.h"
#include "npd_eth_port.h"
#include "npd_vlan.h"
#include "npd_trunk.h"
#include "npd_rstp_common.h"
#include "npd_intf.h"
#include "npd_pvlan.h"
#if 1
/*for tipc*/
#include "npd_communication.h"
#endif
#include "sysdef/npd_sysdef.h"
extern asic_board_t* asic_board;

unsigned int setNum1 = 0;/*set for cscd0*/
unsigned int setNum2 = 0;/*set for cscd1*/
char port_ve_mode[2][26]={0};

extern unsigned short advanced_routing_default_vid;
extern unsigned char * prot_vlan_prot_name[PROTOCOL_VLAN_MAP_MAX];
struct global_ethport_s **global_ethport = NULL;
struct global_ethport_s *start_fp[MAX_SLOT_COUNT];

#define ETH_PORT_ADMIN_STATUS_DEFAULT  ETH_ATTR_ENABLE
#define CRSMU_RGMII_PORT_MRU_DEFAULT   1518
#define ETH_PORT_MRU_DEFAULT		   1536
#define ETH_PORT_DUPLEX_DEFAULT 	   PORT_FULL_DUPLEX_E
#define ETH_PORT_SPEED_DEFAULT		   PORT_SPEED_1000_E
#define ETH_PORT_AN_DEFAULT 		   ETH_ATTR_ON	   /* all Auto-Nego options  */
#define ETH_PORT_AN_DUPLEX_DEFAULT	   ETH_ATTR_ON	   /* duplex mode Auto-Nego   */ 
#define ETH_PORT_AN_FC_DEFAULT		   ETH_ATTR_ON	   /* flow control Auto-Nego*/
#define ETH_PORT_AN_SPEED_DEFAULT	   ETH_ATTR_ON	   /* port speed Auto-Nego*/
#define ETH_PORT_FC_STATE_DEFAULT	   ETH_ATTR_ENABLE /* flow control state*/
#define ETH_PORT_BP_STATE_DEFAULT	   ETH_ATTR_DISABLE /* back-pressure state*/
#define ETH_PORT_MEDIA_DEFAULT		   ETH_ATTR_MEDIA_NOT_EXIST_PRIO /* media status*/
#define ETH_PORT_LED_INTF_DEFAULT	   ETH_ATTR_ENABLE


#define AX71_2X12G12S_MAX_PORTNO       26
#define AX81_AC12C_MAX_PORTNO          28
#define AX71_CRSMU_MAX_PORTNO          4

typedef struct board_info_ethport_maparr_s
{
    int port_type;
	int octeon_or_asic;	
}board_info_ethport_maparr_t;
int ax71_crsmu_octeon_or_asic_mapArr[AX71_CRSMU_MAX_PORTNO] = {
	0, 0, 0, 0
};
int ax71_2x12g12s_octeon_or_asic_mapArr[AX71_2X12G12S_MAX_PORTNO] = {
	1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1,  
	1, 1, 1, 1, 1, 1, 
	1, 1
};
int ax81_ac12c_octeon_or_asic_mapArr[AX81_AC12C_MAX_PORTNO] = {
	0, 0, 0, 0, 0, 0, 
	0, 0, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 
	0, 0, 1, 1, 1, 1, 
};
/*
 * by qinhs@autelan.com
 *
 *	eth-port attribute default value for all different modules
 */
port_default_attribute_t ethport_default_attr_mib[] = {	\
	/* MODULE_ID_NONE */
	{0,0,0,0,0,0,0,0,0,0,0},
	/* MODULE_ID_AX7K_CRSMU */
	{
		ETH_ATTR_ENABLE,1518,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
		ETH_ATTR_DISABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
		ETH_ATTR_ON,ETH_ATTR_OFF,COMBO_PHY_MEDIA_PREFER_COPPER
	},

	/* MODULE_ID_AX7_6GTX */
	{
		ETH_ATTR_ENABLE,1522,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
		ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,   \
		ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_NONE
	},
	/* MODULE_ID_AX7_6GE_SFP */
	{
		ETH_ATTR_ENABLE,1522,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,  \
		ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_DISABLE,ETH_ATTR_DISABLE,  \
		ETH_ATTR_DISABLE,ETH_ATTR_ENABLE,COMBO_PHY_MEDIA_PREFER_NONE
	},
	/* MODULE_ID_AX7_XFP */
	{
		ETH_ATTR_ENABLE,1522,PORT_FULL_DUPLEX_E,PORT_SPEED_10000_E,  \
		ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_DISABLE,ETH_ATTR_DISABLE,  \
		ETH_ATTR_DISABLE,ETH_ATTR_DISABLE,COMBO_PHY_MEDIA_PREFER_NONE	
	},
	/* MODULE_ID_AX7_6GTX_POE */
	{
		ETH_ATTR_ENABLE,1522,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
		ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,   \
		ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_NONE
	},
	/* MODULE_ID_AX5_5612 */
	{
		ETH_ATTR_ENABLE,1522,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
		ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
		ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_NONE
	},
	/* MODULE_ID_AU4_4626 */
	{
		ETH_ATTR_ENABLE,1522,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
		ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,   \
		ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_NONE
	},
	/* MODULE_ID_AU4_4524 */
	{
		ETH_ATTR_ENABLE,1522,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
		ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
		ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_NONE
	},	
	/* MODULE_ID_AU4_4524_POE */
	{
		ETH_ATTR_ENABLE,1522,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
		ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
		ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_NONE
	},
	/* MODULE_ID_AU3_3524 */
	{
		ETH_ATTR_ENABLE,1522,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
		ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
		ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_NONE
	},
	/* MODULE_ID_AU3_3028 */
	{
		ETH_ATTR_ENABLE,1522,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
		ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
		ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_NONE
	},
	/* MODULE_ID_AU3_3052 */
	{
		ETH_ATTR_ENABLE,1522,PORT_FULL_DUPLEX_E,PORT_SPEED_100_E,	\
		ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
		ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_FIBER
	},
	/* MODULE_ID_AU3_3528 */
	{
		ETH_ATTR_ENABLE,1522,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
		ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
		ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_NONE
	},
	/* MODULE_ID_AU2_2528 */
	{
		ETH_ATTR_ENABLE,1522,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
		ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
		ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_NONE
	},	
	/* MODULE_ID_AX5_5612I */
	{
		ETH_ATTR_ENABLE,1522,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
		ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
		ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_NONE
	},	
	/* MODULE_ID_SUBCARD_TYPE_A0 */
	{
		ETH_ATTR_ENABLE,1518,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
		ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
		ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_NONE
	},
	/* MODULE_ID_SUBCARD_TYPE_A1 */
	{
		ETH_ATTR_ENABLE,1522,PORT_FULL_DUPLEX_E,PORT_SPEED_10000_E,  \
		ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_DISABLE,ETH_ATTR_DISABLE,  \
		ETH_ATTR_DISABLE,ETH_ATTR_DISABLE,COMBO_PHY_MEDIA_PREFER_NONE
	},	
	/* MODULE_ID_SUBCARD_TYPE_B */
	{
		ETH_ATTR_ENABLE,1518,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
		ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
		ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_NONE
	},
	/* MODULE_ID_AX5_5612E */
	{
		ETH_ATTR_ENABLE,1518,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
		ETH_ATTR_DISABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
		ETH_ATTR_ON,ETH_ATTR_OFF,COMBO_PHY_MEDIA_PREFER_COPPER
	},
	/* MODULE_ID_AX5_5608 */
	{
		ETH_ATTR_ENABLE,1518,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
		ETH_ATTR_DISABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
		ETH_ATTR_ON,ETH_ATTR_OFF,COMBO_PHY_MEDIA_PREFER_COPPER
	},
	/* MODULE_ID_AX71_2X12G12S */
	{
		ETH_ATTR_ENABLE,1522,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
		ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
		ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_NONE
	},
	/* MODULE_ID_DISTRIBUTED */
	{
		ETH_ATTR_ENABLE,1536,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
		ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
		ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_NONE
	},
	/* MODULE_ID_MAX */
	{0,0,0,0,0,0,0,0,0,0,0}
};

port_default_attribute_t *ethport_attr_default
(
	unsigned int module_id	
)
{
	switch(module_id) {
		case MODULE_ID_NONE:
		default:
			return &ethport_default_attr_mib[0];
		case MODULE_ID_AX7_CRSMU:
		case MODULE_ID_AX7I_CRSMU:
			return &ethport_default_attr_mib[1];
		case MODULE_ID_AX7_6GTX:
		case MODULE_ID_AX7I_GTX:
			return &ethport_default_attr_mib[2];
		case MODULE_ID_AX7_6GE_SFP:
			return &ethport_default_attr_mib[3];
		case MODULE_ID_AX7_XFP:
		case MODULE_ID_AX7I_XG_CONNECT:
			return &ethport_default_attr_mib[4];
		case MODULE_ID_AX7_6GTX_POE:
			return &ethport_default_attr_mib[5];
		case MODULE_ID_AX5_5612:
			return &ethport_default_attr_mib[6];
		case MODULE_ID_AU4_4626:
			return &ethport_default_attr_mib[7];
		case MODULE_ID_AU4_4524:
			return &ethport_default_attr_mib[8];
		case MODULE_ID_AU4_4524_POE:
			return &ethport_default_attr_mib[9];
		case MODULE_ID_AU3_3524:
			return &ethport_default_attr_mib[10];
		case MODULE_ID_AU3_3028:
			return &ethport_default_attr_mib[11];
		case MODULE_ID_AU3_3052:
			return &ethport_default_attr_mib[12];
		case MODULE_ID_AU3_3528:
			return &ethport_default_attr_mib[13];
		case MODULE_ID_AU2_2528:
			return &ethport_default_attr_mib[14];
		case MODULE_ID_AX5_5612I:
			return &ethport_default_attr_mib[15];
		case MODULE_ID_SUBCARD_TYPE_A0:
			return &ethport_default_attr_mib[16];
		case MODULE_ID_SUBCARD_TYPE_A1:
			return &ethport_default_attr_mib[17];
		case MODULE_ID_SUBCARD_TYPE_B:
			return &ethport_default_attr_mib[18];			
		case MODULE_ID_AX5_5612E:
			return &ethport_default_attr_mib[19];
		case MODULE_ID_AX5_5608:
			return &ethport_default_attr_mib[20];
		case MODULE_ID_AX71_2X12G12S:
			return &ethport_default_attr_mib[21];
		case MODULE_ID_DISTRIBUTED:
			return &ethport_default_attr_mib[22];

	}
}

static char *ax51_card_desc[] = {
		"AX51-XFP",
		"AX51-GTX",
		"AX51-SFP",
};

struct eth_port_s **g_eth_ports = NULL;
unsigned int g_cpu_port_index = 0, g_spi_port_index = 0;

NPD_ETH_PORT_NOTIFIER_FUNC	portNotifier = NULL;

/* port array need to poll fiber state*/
unsigned int *g_fiber_port_poll = NULL;

NPD_ETHPORT_BUFFER_MODE_E  g_buffer_mode = CPSS_DXCH_PORT_BUFFERS_MODE_SHARED_E;
static unsigned int scType = ETH_PORT_STREAM_BPS_E;


/**********************************************************************************
 *  npd_get_port_type
 *
 *	DESCRIPTION:
 * 		this routine get port type from module type and port number
 *
 *	INPUT:
 *		module_id - module type,such as CRSMU,6GTX,6GE SFP etc.
 *		eth_port_no - ethernet port number 
 *		
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
enum eth_port_type_e npd_get_port_type
(
	enum module_id_e module_id, 
	int eth_port_no
) 
{
	unsigned char ret = 0, subcard_type = 0;
	
	switch (module_id) {
		case MODULE_ID_AX7_CRSMU:
		case MODULE_ID_AX7I_CRSMU:
			if VALUE_IN_RANGE(eth_port_no,1,4) {
				return ETH_GTX;
			}
			break;		
		case MODULE_ID_AX7I_XG_CONNECT:
			if VALUE_IN_RANGE(eth_port_no,1,2) {
				return ETH_XGE_XFP;
			}
			break;
		case MODULE_ID_AX7_6GTX:
		case MODULE_ID_AX7_6GTX_POE:
		case MODULE_ID_AX7I_GTX:
			if VALUE_IN_RANGE(eth_port_no,1,6) {
				return ETH_GTX;
			}
			break;
		case MODULE_ID_AX7_6GE_SFP:
			if VALUE_IN_RANGE(eth_port_no,1,6) {
				return ETH_GE_SFP;
			}
			break;
		case MODULE_ID_AX5_5612:
			if VALUE_IN_RANGE(eth_port_no,1,12) {
				return ETH_GTX;
			}
			break;
		case MODULE_ID_AX5_5612I:
			if VALUE_IN_RANGE(eth_port_no,1,12) {
				return ETH_GTX;
			}
			break;
		case MODULE_ID_AX5_5612E:
			if VALUE_IN_RANGE(eth_port_no,1,4) {
				return ETH_GTX;
			}
			else if VALUE_IN_RANGE(eth_port_no,5,8) {
				return ETH_FE_TX;
			}
			break;
		case MODULE_ID_AX5_5608:
			if VALUE_IN_RANGE(eth_port_no,1,4) {
				return ETH_FE_TX;
			}
			break;
		case MODULE_ID_AU4_4626:
		case MODULE_ID_AU4_4524:
		case MODULE_ID_AU4_4524_POE:
		case MODULE_ID_AU3_3524:
			if VALUE_IN_RANGE(eth_port_no,1,24) {
				return ETH_GTX;
			}
			break;
		case MODULE_ID_AX7_XFP:
			if VALUE_IN_RANGE(eth_port_no, 1,1) {
				return ETH_XGE_XFP;
			}
			break;
		case MODULE_ID_AU3_3052:
			if VALUE_IN_RANGE(eth_port_no, 1, 48) {
				return ETH_FE_TX;
			}
			else if VALUE_IN_RANGE(eth_port_no, 49,50) {
				return ETH_GE_SFP;
			}
			else if VALUE_IN_RANGE(eth_port_no, 51, 52) {
				return ETH_GE_FIBER;
			}
			break;
		case MODULE_ID_AU3_3528:
			if VALUE_IN_RANGE(eth_port_no, 3, 14) {
				return ETH_GTX;
			}
			else if VALUE_IN_RANGE(eth_port_no, 15, 28) {
				return ETH_GE_FIBER;
			}
			else if VALUE_IN_RANGE(eth_port_no,1,2) {
				return ETH_GE_FIBER;
			}
			break;
		case MODULE_ID_AU2_2528:
			if VALUE_IN_RANGE(eth_port_no, 1, 24) {
				return ETH_GTX;
			}
			else if VALUE_IN_RANGE(eth_port_no, 25, 28) {
				return ETH_GE_FIBER;
			}
			break;
		case MODULE_ID_SUBCARD_TYPE_A0:
		case MODULE_ID_SUBCARD_TYPE_A1:
			if VALUE_IN_RANGE(eth_port_no,1,1) {
				ret = npd_get_subcard_port_type(eth_port_no, &subcard_type);
				if(0 == ret) {
					if(AX51_XFP == subcard_type) {
						return ETH_XGE_XFP;
					}
					else if(AX51_GTX == subcard_type) {
						return ETH_GTX;
					}
					else if(AX51_SFP == subcard_type) {
						return ETH_GE_SFP;
					}
				}
			}
			break;	
		case MODULE_ID_SUBCARD_TYPE_B:
			if VALUE_IN_RANGE(eth_port_no,1,2) {
				ret = npd_get_subcard_port_type(eth_port_no, &subcard_type);
				if(0 == ret) {
					if(AX51_XFP == subcard_type) {
						syslog_ax_eth_port_dbg("get subcard_type %d \n",subcard_type);
						return ETH_XGE_XFP;
					}
					else if(AX51_GTX == subcard_type) {
						syslog_ax_eth_port_dbg("get subcard_type %d \n",subcard_type);
						return ETH_GTX;
					}
					else if(AX51_SFP == subcard_type) {
						syslog_ax_eth_port_dbg("get subcard_type %d \n",subcard_type);
						return ETH_GE_SFP;
					}
				}
				else if(1 == ret) {
					syslog_ax_eth_port_dbg("port %d is not at the slot\n",eth_port_no);
					return ETH_MAX;
				}
			}
			break;
		case MODULE_ID_AX71_2X12G12S:   /*zhangdi add 2011-05-05*/
			
            if VALUE_IN_RANGE(eth_port_no,1,2) {
				return ETH_GE_FIBER;
			}
            else if VALUE_IN_RANGE(eth_port_no, 3, 14) {
				return ETH_GTX;
			}			
			else if VALUE_IN_RANGE(eth_port_no, 15, 28) {
				return ETH_GE_FIBER;
			}
			break;			
		case MODULE_ID_NONE:
		case MODULE_ID_MAX:
			break;
		default:
			break; 
	}

	return ETH_INVALID;
}
#if 1

int parse_mac_addr(char* input,FC_ETHERADDR *  macAddr)
 {
 	
	int i = 0;
	char cur = 0,value = 0;
	
	if((NULL == input)||(NULL == macAddr)) {
		return NPD_FAIL;
	}
	/*if(NPD_FAIL == mac_format_check(input,strlen(input))) {
	//	return NPD_FAIL;
//}*/
	
	for(i = 0; i <6;i++) {
		cur = *(input++);
		if(cur == ':') {
			i--;
			continue;
		}
		if((cur >= '0') &&(cur <='9')) {
			value = cur - '0';
		}
		else if((cur >= 'A') &&(cur <='F')) {
			value = cur - 'A';
			value += 0xa;
		}
		else if((cur >= 'a') &&(cur <='f')) {
			value = cur - 'a';
			value += 0xa;
		}
		macAddr->arEther[i] = value;
		cur = *(input++);	
		if((cur >= '0') &&(cur <='9')) {
			value = cur - '0';
		}
		else if((cur >= 'A') &&(cur <='F')) {
			value = cur - 'A';
			value += 0xa;
		}
		else if((cur >= 'a') &&(cur <='f')) {
			value = cur - 'a';
			value += 0xa;
		}
		macAddr->arEther[i] = (macAddr->arEther[i]<< 4)|value;
	}
	
	return NPD_SUCCESS;
} 

/**********************************************************************************
 *  npd_get_port_type
 *
 *	DESCRIPTION:
 * 		this function check a port whether defaule media type or not
 *
 *	INPUT:
 *		module_id - module type,such as CRSMU,6GTX,6GE SFP etc.
 *		eth_port_no - ethernet port number 
 *		
 *	
 *	OUTPUT:
 *		type - whether defaule media type or not
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occurred
 *		NPD_FAIL	 - if error occurred	
 *
 **********************************************************************************/
int npd_eth_port_combo_media_check
(
	unsigned int slot_no,
	unsigned int eth_port_no,
	enum module_id_e module_id
)
{
	unsigned int ret = 0;
	unsigned int slot_index = 0, local_port_index = 0, eth_g_index = 0;
	unsigned char devNum = 0, virtPortNum = 0;
	unsigned int media = 0;
	
	/* change 5612i main control port module_id from MODULE_ID_AX7_CRSMU to MODULE_ID_AX5_5612I */
	if(MODULE_ID_AX7_CRSMU == module_id && VALUE_IN_RANGE(eth_port_no,9,12))
	{
		module_id = MODULE_ID_AX5_5612I;
	}
	switch (module_id) 
	{
		case MODULE_ID_AX7_CRSMU:
		case MODULE_ID_AX7I_CRSMU:
			if VALUE_IN_RANGE(eth_port_no,1,4)
			{
				ret = 1;
			}
			else
			{
				ret = 0;			
			}
			break;
		case MODULE_ID_AX5_5612:
			if VALUE_IN_RANGE(eth_port_no,9,12) 
			{
				ret = 1;
			}
			else
			{
				ret = 0;
			}
			break;
		case MODULE_ID_AX5_5612I:
			if VALUE_IN_RANGE(eth_port_no,9,12) 
			{
				ret = 1;
			}
			else
			{
				ret = 0;
			}
			break;
		case MODULE_ID_AX5_5612E:
			if VALUE_IN_RANGE(eth_port_no,5,8) 
			{
				ret = 1;
			}
			else
			{
				ret = 0;
			}
			break;
		case MODULE_ID_AX5_5608:
			ret = 1;
			break;
		case MODULE_ID_AU4_4626:
		case MODULE_ID_AU4_4524:
		case MODULE_ID_AU4_4524_POE:
		case MODULE_ID_AU3_3524:
			if VALUE_IN_RANGE(eth_port_no,21,24) 
			{
				ret = 1;
			}
			else
			{
				ret = 0;
			}
			break;
		case MODULE_ID_AU3_3052:
			if VALUE_IN_RANGE(eth_port_no,49,50) 
			{
				ret = 1;
			}
			else
			{
				ret = 0;			
			}
			break;
		default:
			ret = 0;
			break; 
	}	
	if(1 == ret)
	{
		ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) 
		{
			if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,eth_port_no))
			{
				slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
				local_port_index = ETH_LOCAL_NO2INDEX(slot_index, eth_port_no);
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,local_port_index);

				ret = ETHPORT_RETURN_CODE_ERR_NONE;
			}
		}

		if(!PRODUCT_IS_BOX && (AX7_CRSMU_SLOT_NUM == slot_index)) 
		{		
			/*preferred media*/
			ret = nbm_get_eth_port_trans_media(eth_g_index,&media);
			if(ret != 0) 
			{
				syslog_ax_eth_port_dbg("get port media error! port(%d %d),ret %d\n",slot_no, eth_port_no,ret);
			}
		}	
		else if((MODULE_ID_AX5_5612I == module_id) && \
			(eth_g_index >= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9)) &&  \
			(eth_g_index <= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,12))) {
			/*preferred media*/
			eth_g_index = eth_g_index - ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9);/*eth_g_index should be 0-3*/
			ret = nbm_get_eth_port_trans_media(eth_g_index,&media);
			if(ret != 0)
			{
				syslog_ax_eth_port_dbg("get port media error! port(%d %d),ret %d\n",slot_no,eth_port_no,ret);
			}			
		}
		else if(MODULE_ID_AX5_5612E == module_id || MODULE_ID_AX5_5608 == module_id) 
		{		
			/*preferred media*/
			ret = nbm_get_eth_port_trans_media(eth_g_index,&media);
			if(ret != 0) 
			{
				syslog_ax_eth_port_dbg("get port media error! port(%d %d),ret %d\n",slot_no, eth_port_no,ret);
			}
		}
		else 
		{			
			ret |= npd_get_devport_by_global_index(eth_g_index,&devNum,&virtPortNum);
			ret |= nam_get_eth_port_trans_media(devNum,virtPortNum,&media);
			if(ret != 0) 
			{
				syslog_ax_eth_port_dbg("get port(%d,%d) combo media fail ret %#0x.\n",devNum,virtPortNum,ret);
			}
		}
		if(MODULE_ID_AU3_3052 == module_id) 
		{
			if(COMBO_PHY_MEDIA_PREFER_COPPER == media) 
			{
				return ETH_PREFERRED_COPPER_MEDIA_BIT;
			}
		}
		else if((MODULE_ID_AX7_CRSMU == module_id) || (MODULE_ID_AX5_5612I == module_id)) 
		{
			if(COMBO_PHY_MEDIA_PREFER_FIBER == media) 
			{
				return ETH_PREFERRED_FIBER_MEDIA_BIT;
			}
		}
		else if((MODULE_ID_AX5_5612E == module_id) || (MODULE_ID_AX5_5608 == module_id))
		{
			if(COMBO_PHY_MEDIA_PREFER_FIBER == media) 
			{
				return ETH_PREFERRED_FIBER_MEDIA_BIT;
			}
			else if(COMBO_PHY_MEDIA_PREFER_COPPER == media) 
			{
				return ETH_PREFERRED_COPPER_MEDIA_BIT;
			}
		}
		syslog_ax_eth_port_dbg("get media value is %d\n",media);
	}
	return ret;	/* code optimize: Missing return statement  0 houxx@autelan.com  2013-1-17 */
}
int npd_slot_to_dev_map(unsigned char *cpu_slot,unsigned char *slot_no,unsigned char *port,unsigned char *devno,unsigned char *vir_port_no)
{
	
	unsigned char Ax71_2X12G12S[26]={24,25,1,0,3,2,5,4,7,6,9,8,11,10,\
									 14,15,12,13,18,19,16,17,22,23,20,21};


	if(((*cpu_slot<1) || (*cpu_slot>2)) || (*slot_no != 3) || (*port<1) || (*port>26))
	{
	    return INTERFACE_RETURN_CODE_NO_SUCH_PORT;
	}
	
	if(*slot_no == 3)
	{
	    *devno = 4;
		*vir_port_no = Ax71_2X12G12S[*port-1];
		*cpu_slot = *cpu_slot-1;
		/* *slot_no = *slot_no; */  /* code optimize: Self assignment  houxx@autelan.com  2013-1-17 */

		return INTERFACE_RETURN_CODE_SUCCESS;
	}
	else
	{
	    *devno = 0;
		*vir_port_no = 0;
	}

	return INTERFACE_RETURN_CODE_NO_SUCH_PORT;
	
}

int npd_port_media_get
(
	unsigned int eth_g_index,
	enum eth_port_type_e *portMedia
)
{
	unsigned int slot_index = 0, port_index = 0, slotno = 0, portno = 0;
	enum module_id_e moduleId = 0;
	unsigned int ret = 0;
	enum eth_port_type_e tmpPortMedia = ETH_INVALID;
	slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	slotno = CHASSIS_SLOT_INDEX2NO(slot_index);
	port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	portno = ETH_LOCAL_INDEX2NO(slot_index, port_index);
	moduleId = MODULE_TYPE_ON_SLOT_INDEX(slot_index);

	if(	MODULE_ID_AU4_4626 == moduleId ||
		MODULE_ID_AU4_4524 == moduleId ||
		MODULE_ID_AU4_4524_POE == moduleId ||
		MODULE_ID_AU3_3524 == moduleId){

		if(VALUE_IN_RANGE(portno, 21, 24)){
			ret = nam_port_phy_port_media_type(moduleId,slotno,portno,&tmpPortMedia);
			if( 0 == ret ){ /*NAM_ERRO_NONE*/
				*portMedia =  tmpPortMedia;
				return NPD_TRUE;
			}
			else
				return NPD_FALSE;
		}
	}
	if(MODULE_ID_AX5_5612 == moduleId) {
		if(VALUE_IN_RANGE(portno, 9, 12)){
			ret = nam_port_phy_port_media_type(moduleId,slotno,portno,&tmpPortMedia);
			if( 0 == ret ){ /*NAM_ERRO_NONE*/
				*portMedia =  tmpPortMedia;
				return NPD_TRUE;
			}
			else
				return NPD_FALSE;
		}
	}	
	if(MODULE_ID_AU3_3052 == moduleId) {
		if(VALUE_IN_RANGE(portno, 49, 50)){
			ret = nam_port_phy_port_media_type(moduleId,slotno,portno,&tmpPortMedia);
			if( 0 == ret ){ /*NAM_ERRO_NONE*/
				syslog_ax_eth_port_dbg("get port(%d,%d) mediaType :: %d\n",slotno,portno,tmpPortMedia);
				*portMedia = tmpPortMedia;
				return NPD_TRUE;
			}
			else
				return NPD_FALSE;
		}		
	}
	return NPD_PORT_NOT_SUPPORT_MEDIA_SEL;
}
#endif

/**********************************************************************************
 *  npd_create_eth_port
 *
 *	DESCRIPTION:
 * 		this routine create ethernet port with <slot,port> and 
 *		attach port info to global data structure.
 *
 *	INPUT:
 *		slot_index - slot number(index is the same as panel slot number)
 *		eth_local_index - port index(index may different from panel number)
 *		
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_create_eth_port
(
	unsigned int slot_index,
	unsigned int eth_local_index
) 
{
    	unsigned int eth_global_index = 0;
	struct eth_port_s *eth_ports = NULL;
	struct eth_port_switch_s* switchNode = NULL;
	struct eth_port_promi_s* promiNode = NULL;
	unsigned char subcard_type = 0, ret = 0;
	unsigned char local_port_no = 0;
	enum product_id_e productId = PRODUCT_ID; 
	int isBoard = NPD_FALSE;/* 1 -if main board port, 0 - slave board port*/ 
	enum module_id_e  moduleId = MODULE_ID_NONE;
	struct timeval tnow;
	struct tm tzone;
   	struct prot_vlan_ctrl_t *ethportVlanCtrl = NULL;
	memset(&tnow, 0, sizeof(struct timeval));
	memset(&tzone, 0, sizeof(struct tm));
	
	moduleId = MODULE_TYPE_ON_SLOT_INDEX(slot_index);
	eth_global_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,eth_local_index);
	local_port_no = ETH_LOCAL_INDEX2NO(slot_index,eth_local_index);
	
	syslog_ax_eth_port_dbg("\tlocal port %d global index %#x\n",eth_local_index,eth_global_index);
	
	if(g_eth_ports[eth_global_index]) {
		syslog_ax_eth_port_dbg("port %d/%d hot plugin, skip reconstuct\n",slot_index, eth_local_index + 1);
		npd_set_port_attr_default(eth_global_index,NPD_FALSE,moduleId); 
		return;
	}
	
	eth_ports = malloc(sizeof(struct eth_port_s));
	if(NULL == eth_ports)
	{
		syslog_ax_eth_port_dbg("memory alloc error when create ether port %d/%d!!!\n",slot_index,eth_local_index);
		return;
	}
	memset((void *)eth_ports,0,sizeof(struct eth_port_s));
	
	gettimeofday (&tnow, &tzone);
	eth_ports->lastLinkChange = tnow.tv_sec;
	
	eth_ports->port_type = npd_get_port_type(MODULE_TYPE_ON_SLOT_INDEX(slot_index),ETH_LOCAL_INDEX2NO(slot_index,eth_local_index));

	if(PRODUCT_ID_AX5K_E != PRODUCT_ID && PRODUCT_ID_AX5608!= PRODUCT_ID){
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_PORT_BASED_VLAN);
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_DOT1Q_BASED_VLAN);
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_DOT1W);
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_LINK_AGGREGATION);
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_IPV4);
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_ACL_RULE);
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_QoS_PROFILE);
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_FDB);
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_STORM_CONTROL);	
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_PROTOCOL_BASED_VLAN);
		
		eth_ports->funcs.funcs_run_bitmap |= (1 << ETH_PORT_FUNC_LINK_AGGREGATION);
		eth_ports->funcs.funcs_run_bitmap |= (1 << ETH_PORT_FUNC_PORT_BASED_VLAN);
		eth_ports->funcs.funcs_run_bitmap |= (1 << ETH_PORT_FUNC_DOT1Q_BASED_VLAN);
	}

    ethportVlanCtrl = (struct prot_vlan_ctrl_t *)malloc(sizeof(struct prot_vlan_ctrl_t));
    memset(ethportVlanCtrl,0,sizeof(struct prot_vlan_ctrl_t));
    eth_ports->funcs.func_data[ETH_PORT_FUNC_PROTOCOL_BASED_VLAN] = ethportVlanCtrl;

	if((PRODUCT_ID_AX7K == productId)&&(eth_global_index < 4)) 
	{
		promiNode = (struct eth_port_promi_s *)malloc(sizeof(struct eth_port_promi_s));
		if(NULL == promiNode)
		{
			syslog_ax_eth_port_dbg("init eth-port %#x memory malloc error!\n",eth_global_index);
			free(eth_ports);/* code optimize: recourse leak zhangcl@autelan.com  2013-1-22 */
			return ;
		}

		memset(promiNode,0,sizeof(struct eth_port_promi_s));
		promiNode->npd_promi_port_notifier_func = (NPD_ETH_PORT_LINK_CHANGE_NOTIFIER_FUNC)npd_promi_port_link_change;
		INIT_LIST_HEAD(&(promiNode->subifList.list));
		
		eth_ports->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS] = promiNode;
		eth_ports->funcs.funcs_run_bitmap |= (1 << ETH_PORT_FUNC_MODE_PROMISCUOUS);
		isBoard = NPD_TRUE;
	}
	else if((PRODUCT_ID_AX7K_I == productId) && ((eth_global_index <= 3) ||((eth_global_index <= 67)&&(eth_global_index >= 64)))) /* code optimize:Unsigned compared against 0 houxx@autelan.com  2013-1-17 */
	{
		promiNode = (struct eth_port_promi_s *)malloc(sizeof(struct eth_port_promi_s));
		if(NULL == promiNode)
		{
			syslog_ax_eth_port_dbg("init eth-port %#x memory malloc error!\n",eth_global_index);
			free(eth_ports);/* code optimize: recourse leak zhangcl@autelan.com  2013-1-22 */
			return ;
		}

		memset(promiNode,0,sizeof(struct eth_port_promi_s));
		promiNode->npd_promi_port_notifier_func = (NPD_ETH_PORT_LINK_CHANGE_NOTIFIER_FUNC)npd_promi_port_link_change;
		INIT_LIST_HEAD(&(promiNode->subifList.list));
		
		eth_ports->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS] = promiNode;
		eth_ports->funcs.funcs_run_bitmap |= (1 << ETH_PORT_FUNC_MODE_PROMISCUOUS);
		isBoard = NPD_TRUE;
	}
	
	else if((PRODUCT_ID_AX5K_I == productId) && (eth_global_index >= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9)) && (eth_global_index <= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,12))) 
	{
		promiNode = (struct eth_port_promi_s *)malloc(sizeof(struct eth_port_promi_s));
		if(NULL == promiNode)
		{
			syslog_ax_eth_port_dbg("init eth-port %#x memory malloc error!\n",eth_global_index);
			free(eth_ports);/* code optimize: recourse leak zhangcl@autelan.com  2013-1-22 */
			return ;
		}

		memset(promiNode,0,sizeof(struct eth_port_promi_s));
		promiNode->npd_promi_port_notifier_func = (NPD_ETH_PORT_LINK_CHANGE_NOTIFIER_FUNC)npd_promi_port_link_change;
		INIT_LIST_HEAD(&(promiNode->subifList.list));
		
		eth_ports->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS] = promiNode;
		eth_ports->funcs.funcs_run_bitmap |= (1 << ETH_PORT_FUNC_MODE_PROMISCUOUS);
		syslog_ax_eth_port_dbg("init 5612i port %d\n",eth_global_index);
		isBoard = NPD_TRUE;
		/*eth_global_index = eth_global_index - ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9);
		eth_global_index = eth_global_index - 8;index should be 0-3 for nbm*/
	}
	else if((PRODUCT_ID_AX5K_E == productId) 	\
		&& (eth_global_index >= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,1)) \
		&& (eth_global_index <= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,8))) {
		syslog_ax_eth_port_dbg("npd create eth port in the PRODUCT_ID_AX5E!\n");
		promiNode = (struct eth_port_promi_s *)malloc(sizeof(struct eth_port_promi_s));
		if(NULL == promiNode)
		{
			syslog_ax_eth_port_dbg("init eth-port %#x memory malloc error!\n",eth_global_index);
			free(eth_ports);/* code optimize: recourse leak zhangcl@autelan.com  2013-1-22 */
			return ;
		}

		memset(promiNode,0,sizeof(struct eth_port_promi_s));
		promiNode->npd_promi_port_notifier_func = (NPD_ETH_PORT_LINK_CHANGE_NOTIFIER_FUNC)npd_promi_port_link_change;
		INIT_LIST_HEAD(&(promiNode->subifList.list));
		
		eth_ports->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS] = promiNode;
		eth_ports->funcs.funcs_run_bitmap |= (1 << ETH_PORT_FUNC_MODE_PROMISCUOUS);
		syslog_ax_eth_port_dbg("init 5612e port %d\n",eth_global_index);
		isBoard = NPD_TRUE;
	}
	else if((PRODUCT_ID_AX5608 == productId) 	\
		&& (eth_global_index >= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,1)) \
		&& (eth_global_index <= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,4))) {
		syslog_ax_eth_port_dbg("npd create eth port in the PRODUCT_ID_AX5608!\n");
		promiNode = (struct eth_port_promi_s *)malloc(sizeof(struct eth_port_promi_s));
		if(NULL == promiNode)
		{
			syslog_ax_eth_port_dbg("init eth-port %#x memory malloc error!\n",eth_global_index);
			free(eth_ports);/* code optimize: recourse leak zhangcl@autelan.com  2013-1-22 */
			return ;
		}

		memset(promiNode,0,sizeof(struct eth_port_promi_s));
		promiNode->npd_promi_port_notifier_func = (NPD_ETH_PORT_LINK_CHANGE_NOTIFIER_FUNC)npd_promi_port_link_change;
		INIT_LIST_HEAD(&(promiNode->subifList.list));
		
		eth_ports->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS] = promiNode;
		eth_ports->funcs.funcs_run_bitmap |= (1 << ETH_PORT_FUNC_MODE_PROMISCUOUS);
		syslog_ax_eth_port_dbg("init 5612e port %d\n",eth_global_index);
		isBoard = NPD_TRUE;
	}
	else if(npd_eslot_check_subcard_module(moduleId) && \
			((AX51_GTX == npd_check_subcard_type(PRODUCT_ID, local_port_no)) || \
			(AX51_SFP == npd_check_subcard_type(PRODUCT_ID, local_port_no)))) {
		promiNode = (struct eth_port_promi_s *)malloc(sizeof(struct eth_port_promi_s));
		if(NULL == promiNode)
		{
			syslog_ax_eth_port_dbg("init eth-port %d memory malloc error!\n",eth_global_index);
			free(eth_ports);/* code optimize: recourse leak zhangcl@autelan.com  2013-1-22 */
			return ;
		}
		
		memset(promiNode,0,sizeof(struct eth_port_promi_s));
		promiNode->npd_promi_port_notifier_func = (NPD_ETH_PORT_LINK_CHANGE_NOTIFIER_FUNC)npd_promi_port_link_change;
		INIT_LIST_HEAD(&(promiNode->subifList.list));
		
		eth_ports->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS] = promiNode;
		eth_ports->funcs.funcs_run_bitmap |= (1 << ETH_PORT_FUNC_MODE_PROMISCUOUS);
		isBoard = NPD_TRUE;
	}
	else 
	{
		/*init port running switch mode*/
		switchNode = (struct eth_port_switch_s *)malloc(sizeof(struct eth_port_switch_s));
		if(NULL == switchNode)
		{
			syslog_ax_eth_port_dbg("init eth-port %d memory malloc error!\n",eth_global_index);
			free(eth_ports);/* code optimize: recourse leak zhangcl@autelan.com  2013-1-22 */
			return ;
		}
			
		switchNode->isSwitch = NPD_TRUE;
		
		eth_ports->funcs.func_data[ETH_PORT_FUNC_BRIDGE] = switchNode;
		eth_ports->funcs.funcs_run_bitmap |= (1 << ETH_PORT_FUNC_BRIDGE);
	}

	g_eth_ports[eth_global_index] = eth_ports;
	
	#if 0	
	ret = npd_get_devport_by_global_index(eth_global_index,&devNum,&portNum);
	if(0 != ret) {
		return NPD_DBUS_ERROR;
	}	
	ret = nam_set_bcm_ethport_phy(devNum,portNum);
	if(0 != ret) {
		return NPD_DBUS_ERROR;
	}		
	#endif
	if((PRODUCT_ID_AX7K_I == productId) && (MODULE_ID_AX7I_XG_CONNECT == moduleId))
	{
		moduleId = MODULE_ID_AX7_XFP;
	}

	/* add for ax71_SMU on slot 1,zhangdi */
    if((PRODUCT_ID_AX7K_I == productId) && (slot_index == 1))  
	{   
	    npd_set_port_attr_default(eth_local_index,isBoard,moduleId); /* use eth_local_index:[0-3],not[64-67] */
	}/* zhangdi add new branch */
	else
	{
	    npd_set_port_attr_default(eth_global_index,isBoard,moduleId); /* port info struct must attached to global array*/
	}

	return;
}
/**********************************************************************************
 *  npd_create_eth_port_distributed
 *
 *	DESCRIPTION:
 * 		this routine create ethernet port with <slot,port> and 
 *		attach port info to global data structure.
 *
 *	INPUT:
 *		slot_index - slot number(index is the same as panel slot number)
 *		eth_local_index - port index(index may different from panel number)
 *		
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_create_eth_port_distributed
(
	unsigned int slot_index,
	unsigned int eth_local_index
) 
{
    unsigned int eth_global_index = 0;
    enum module_id_e module_type;
	/* default attr */
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	unsigned int  attr_bitmap = 0;	
	struct eth_port_s *portInfo = NULL;
	struct eth_port_s tmp_portInfo;
	struct eth_port_s *eth_ports = NULL;
	asic_eth_port_t* asic_eth_port = NULL;   /* asic_board */

	struct eth_port_switch_s* switchNode = NULL;
	struct eth_port_promi_s* promiNode = NULL;
	unsigned char subcard_type = 0, ret = 0;
	unsigned char local_port_no = 0, slot_no = 0;
	enum product_id_e productId = PRODUCT_ID; 
	int isBoard = NPD_FALSE;/* 1 -if main board port, 0 - slave board port*/ 
	/*enum module_id_e  moduleId = MODULE_ID_NONE;*/
	struct timeval tnow;
	struct tm tzone;
   	struct prot_vlan_ctrl_t *ethportVlanCtrl = NULL;
	memset(&tnow, 0, sizeof(struct timeval));
	memset(&tzone, 0, sizeof(struct tm));
    struct sysinfo info = {0};      /* for mobile test */


	asic_eth_port = &(asic_board->asic_eth_ports[eth_local_index]);
	memset(&tmp_portInfo,0,sizeof(struct eth_port_s));	/* code optimize: Uninitialized scalar variable houxx@autelan.com  2013-1-17 */

	/*moduleId = MODULE_TYPE_ON_SLOT_INDEX(slot_index);*/
	eth_global_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,eth_local_index);
	local_port_no = ETH_LOCAL_INDEX2NO(slot_index,eth_local_index);
	
	syslog_ax_eth_port_dbg("\teth_local_index %d global index %#x\n",eth_local_index,eth_global_index);
    syslog_ax_eth_port_dbg("\tlocal_port_no: %d\n",local_port_no);
	
	if(g_eth_ports[eth_global_index])
	{
		syslog_ax_eth_port_dbg("port %d/%d hot plugin, skip reconstuct\n",slot_index, eth_local_index + 1);
		/*npd_set_port_attr_default(eth_global_index,NPD_FALSE,moduleId); */
		goto set_port_attr_default;
	}
	
	eth_ports = malloc(sizeof(struct eth_port_s));
	if(NULL == eth_ports)
	{
		syslog_ax_eth_port_dbg("memory alloc error when create ether port %d/%d!!!\n",slot_index,eth_local_index);
		return;
	}
	memset((void *)eth_ports,0,sizeof(struct eth_port_s));

	sysinfo(&info);
	gettimeofday (&tnow, &tzone);
	eth_ports->lastLinkChange = tnow.tv_sec;
	
	eth_ports->port_type = asic_eth_port->port_type;
    #if 0
	{
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_PORT_BASED_VLAN);
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_DOT1Q_BASED_VLAN);
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_DOT1W);
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_LINK_AGGREGATION);
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_IPV4);
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_ACL_RULE);
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_QoS_PROFILE);
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_FDB);
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_STORM_CONTROL);	
		eth_ports->funcs.funcs_cap_bitmap |= (1 << ETH_PORT_FUNC_PROTOCOL_BASED_VLAN);
		
		eth_ports->funcs.funcs_run_bitmap |= (1 << ETH_PORT_FUNC_LINK_AGGREGATION);
		eth_ports->funcs.funcs_run_bitmap |= (1 << ETH_PORT_FUNC_PORT_BASED_VLAN);
		eth_ports->funcs.funcs_run_bitmap |= (1 << ETH_PORT_FUNC_DOT1Q_BASED_VLAN);
	}
	#endif
	eth_ports->funcs.funcs_run_bitmap = asic_eth_port->funcs->funcs_run_bitmap;
    syslog_ax_eth_port_dbg("funcs_run_bitmap:0x%x\n",eth_ports->funcs.funcs_run_bitmap);
		
    ethportVlanCtrl = (struct prot_vlan_ctrl_t *)malloc(sizeof(struct prot_vlan_ctrl_t));	
    memset(ethportVlanCtrl,0,sizeof(struct prot_vlan_ctrl_t));
    eth_ports->funcs.func_data[ETH_PORT_FUNC_PROTOCOL_BASED_VLAN] = ethportVlanCtrl;


    {
		/*init port running switch mode*/
		switchNode = (struct eth_port_switch_s *)malloc(sizeof(struct eth_port_switch_s));
		if(NULL == switchNode)
		{
			syslog_ax_eth_port_dbg("init eth-port %d memory malloc error!\n",eth_global_index);
			return ;
		}
			
		switchNode->isSwitch = NPD_TRUE;
		
		eth_ports->funcs.func_data[ETH_PORT_FUNC_BRIDGE] = switchNode;
		eth_ports->funcs.funcs_run_bitmap |= (1 << ETH_PORT_FUNC_BRIDGE);
	}

	g_eth_ports[eth_global_index] = eth_ports;

	/* here is the dafault attr */
    set_port_attr_default:
    {
    	portInfo = npd_get_port_by_index(eth_global_index);
    	if(NULL == portInfo) 
		{		
			return ETHPORT_RETURN_CODE_ERR_GENERAL;
    	}
	
		ret = npd_get_devport_by_global_index(eth_global_index,&devNum,&portNum);
		
		if(0 != ret)
		{
			return ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
	    attr_bitmap = asic_eth_port->attr_bitmap;

		/* admin status*/
		ret = nam_set_ethport_enable(devNum,portNum,asic_eth_port->asic_default_attr->admin_state);
		if(NPD_SUCCESS != ret) 
		{
			syslog_ax_eth_port_dbg("set port(%d,%d) admin status error %d\n ",	\
					devNum,portNum,ret);
		}	
		/* flow-control */
		ret = nam_set_ethport_flowCtrl(devNum,portNum,asic_eth_port->asic_default_attr->fc);
		if(NPD_SUCCESS != ret) 
		{
			syslog_ax_eth_port_dbg("set port(%d,%d) flow control error %d\n ",devNum,portNum,ret);
		}
		/* duplex mode */
		ret = nam_set_ethport_duplex_mode(devNum,portNum,asic_eth_port->asic_default_attr->duplex);
		if(NPD_SUCCESS != ret)
		{
			syslog_ax_eth_port_dbg("set port(%d,%d) duplex error %d\n ",devNum,portNum,ret);
		}		
		/* Auto-Nego (AN-FC,AN-speed,AN-duplex)*/
		ret = nam_set_ethport_force_linkdown( devNum, portNum, 1);
		if(NPD_SUCCESS!= ret)
		{
          syslog_ax_eth_port_dbg("set port(%d,%d)force linkdown error\n" ,devNum,portNum);   
		}	
		ret = nam_set_ethport_duplex_autoneg(devNum,portNum,asic_eth_port->asic_default_attr->duplex_an);
		if(NPD_SUCCESS != ret && ETHPORT_RETURN_CODE_NOT_SUPPORT != ret) 
		{
			syslog_ax_eth_port_dbg("set port(%d,%d) duplex_an error %d\n ",devNum,portNum,ret);
		}		
		ret = nam_set_ethport_speed_autoneg(devNum,portNum,asic_eth_port->asic_default_attr->speed_an);
		if(NPD_SUCCESS != ret && ETHPORT_RETURN_CODE_NOT_SUPPORT != ret)
		{
			syslog_ax_eth_port_dbg("set port(%d,%d) speed_an error %d\n ",devNum,portNum,ret);
		}		
		ret = nam_set_ethport_fc_autoneg(devNum,portNum,asic_eth_port->asic_default_attr->fc_an,asic_eth_port->asic_default_attr->fc_an);
		if(NPD_SUCCESS != ret && ETHPORT_RETURN_CODE_NOT_SUPPORT != ret)
		{
			syslog_ax_eth_port_dbg("set port(%d,%d) flow control auto-nego error %d\n ",devNum,portNum,ret);
		}			
		ret = nam_set_phy_ethport_autoneg(devNum,portNum,1);
		if(NPD_SUCCESS != ret)
		{
          syslog_ax_eth_port_dbg("set phy(%d,%d)ethport autoneg error \n" ,devNum,portNum);   
		}
        ret = nam_set_ethport_force_auto( devNum, portNum,0);
		if(NPD_SUCCESS != ret)
		{
          syslog_ax_eth_port_dbg("set port(%d,%d)force auto error\n" ,devNum,portNum);  
		}

		if(1)
		{	
			/* back-pressure disable*/
			ret = nam_set_ethport_backPres(devNum,portNum,asic_eth_port->asic_default_attr->bp);
			if(NPD_SUCCESS != ret) 
			{
				syslog_ax_eth_port_dbg("set port(%d,%d) back pressure error %d\n ",	\
						devNum,portNum,ret);
			}
			/* port speed*/
			ret = nam_set_ethport_speed(devNum,portNum,asic_eth_port->asic_default_attr->speed);
			if(NPD_SUCCESS != ret) 
			{
				syslog_ax_eth_port_dbg("set port(%d,%d) speed error %d\n ",devNum,portNum,ret);
			}	
			/* fiber port should not config auto negotiation because these ports haven't phy */
			if(24 <= portNum && 27 >= portNum) 
			{
				/* Auto-Nego (AN-FC,AN-speed,AN-duplex)*/
				ret = nam_set_ethport_force_linkdown(devNum, portNum, 1);
				if(NPD_SUCCESS!= ret)
				{
		          syslog_ax_eth_port_dbg("set port(%d,%d)force linkdown error\n" ,devNum,portNum);   
				}	
				ret = nam_set_ethport_duplex_autoneg(devNum,portNum,ETH_ATTR_DISABLE);
				if(NPD_SUCCESS != ret) 
				{
					syslog_ax_eth_port_dbg("set port(%d,%d) duplex_an error %d\n ",devNum,portNum,ret);
				}		
				ret = nam_set_ethport_speed_autoneg(devNum,portNum,ETH_ATTR_DISABLE);
				if(NPD_SUCCESS != ret) 
				{
					syslog_ax_eth_port_dbg("set port(%d,%d) speed_an error %d\n ",devNum,portNum,ret);
				}		
				ret = nam_set_ethport_fc_autoneg(devNum,portNum,ETH_ATTR_DISABLE,ETH_ATTR_DISABLE);
				if(NPD_SUCCESS != ret) 
				{
					syslog_ax_eth_port_dbg("set port(%d,%d) flow control auto-nego error %d\n ",devNum,portNum,ret);
				}			
		        ret = nam_set_ethport_force_auto(devNum, portNum,0);
				if(NPD_SUCCESS != ret)
				{
		          syslog_ax_eth_port_dbg("set port(%d,%d)force auto error\n" ,devNum,portNum);  
				}					
			}				
		}		
		portInfo->attr_bitmap = attr_bitmap;
		
		/* MRU 1522*/
		portInfo->mtu = ETH_PORT_MRU_DEFAULT;
		ret = nam_set_ethport_mru(devNum,portNum,ETH_PORT_MRU_DEFAULT);
		if(NPD_SUCCESS != ret) 
		{
			syslog_ax_eth_port_dbg("set port(%d,%d) mru error %d\n",devNum,portNum,ret);
		}
    }

	/* Added by Jia Lihui 2011-6-20 */
	module_type	= MODULE_TYPE_ON_SLOT_INDEX(slot_index);
	slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
	local_port_no	= ETH_LOCAL_INDEX2NO(slot_index, eth_local_index);
	ret = nam_read_eth_port_info(module_type,slot_no,local_port_no,&tmp_portInfo);
	if(NPD_SUCCESS != ret) 
	{
		syslog_ax_eth_port_err("read ASIC port(%d %d) infomation error.\n",slot_no, local_port_no);
	}	

    start_fp[slot_index][local_port_no-1].isValid = VALID_ETHPORT;
	start_fp[slot_index][local_port_no-1].cpu_or_asic = ASIC;
	start_fp[slot_index][local_port_no-1].eth_g_index = eth_global_index + asic_board->asic_port_start_no-1;
	start_fp[slot_index][local_port_no-1].slot_no = SLOT_ID;
	start_fp[slot_index][local_port_no-1].local_port_no = local_port_no;
	start_fp[slot_index][local_port_no-1].local_port_index = eth_local_index + asic_board->asic_port_start_no-1;
	start_fp[slot_index][local_port_no-1].port_type = eth_ports->port_type;
	start_fp[slot_index][local_port_no-1].mtu = tmp_portInfo.mtu;
	start_fp[slot_index][local_port_no-1].attr_bitmap = tmp_portInfo.attr_bitmap;		
	
	start_fp[slot_index][local_port_no-1].linkchanged= 0;
	start_fp[slot_index][local_port_no-1].lastLinkChange = info.uptime;
	start_fp[slot_index][local_port_no-1].port_default_attr.admin_state = asic_eth_port->asic_default_attr->admin_state;
	start_fp[slot_index][local_port_no-1].port_default_attr.autoNego = asic_eth_port->asic_default_attr->autoNego;
	start_fp[slot_index][local_port_no-1].port_default_attr.bp = asic_eth_port->asic_default_attr->bp;
	start_fp[slot_index][local_port_no-1].port_default_attr.duplex = asic_eth_port->asic_default_attr->duplex;
	start_fp[slot_index][local_port_no-1].port_default_attr.duplex_an = asic_eth_port->asic_default_attr->duplex_an;
	start_fp[slot_index][local_port_no-1].port_default_attr.fc = asic_eth_port->asic_default_attr->fc;
	start_fp[slot_index][local_port_no-1].port_default_attr.fc_an = asic_eth_port->asic_default_attr->fc_an;
	start_fp[slot_index][local_port_no-1].port_default_attr.mediaPrefer = asic_eth_port->asic_default_attr->mediaPrefer;
	start_fp[slot_index][local_port_no-1].port_default_attr.mtu = asic_eth_port->asic_default_attr->mtu;
	start_fp[slot_index][local_port_no-1].port_default_attr.speed = asic_eth_port->asic_default_attr->speed;
	start_fp[slot_index][local_port_no-1].port_default_attr.speed_an = asic_eth_port->asic_default_attr->speed_an;

    syslog_ax_eth_port_dbg("port_type %d\n",start_fp[slot_index][local_port_no-1].port_type);

	return ret;
}



/**********************************************************************************
 *  npd_init_eth_ports
 *
 *	DESCRIPTION:
 * 		this routine initialize all ethernet ports
 *
 *	INPUT:
 *		NULL
 *		
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_init_eth_ports(void) {
	int i = 0,j = 0, length = 0;
	int fd;
	char file_path[64];
	
    global_ethport = (struct global_ethport_s **)malloc(sizeof(struct global_ethport_s*)*GLOBAL_ETHPORTS_MAXNUM);
	if(NULL == global_ethport)
	{
		syslog_ax_eth_port_dbg("memory alloc error for eth port init!!!\n");
		return;
	}
	syslog_ax_eth_port_dbg("memory alloc sizeof(struct global_ethport_s)  = %d, sizeof(struct global_ethport_s*) = %d\n", 
		sizeof(struct global_ethport_s),
		sizeof(struct global_ethport_s*));
	memset(global_ethport, 0, sizeof(struct global_ethport_s*)*GLOBAL_ETHPORTS_MAXNUM);
	
	g_eth_ports = malloc(sizeof(struct eth_port_s *)*(MAX_ETH_GLOBAL_INDEX));
	if(NULL == g_eth_ports)
	{
		syslog_ax_eth_port_dbg("memory alloc error for eth port init!!!\n");
		return;
	}
	
	memset(g_eth_ports,0,sizeof(struct eth_port_s *)*(MAX_ETH_GLOBAL_INDEX));

	g_fiber_port_poll = malloc(sizeof(unsigned int) * (MAX_ETH_GLOBAL_INDEX));
	if(NULL == g_fiber_port_poll) {
		syslog_ax_eth_port_dbg("memory alloc error for port state polling array!!!\n");
		return;
	}
	memset(g_fiber_port_poll, 0,sizeof(unsigned int) * (MAX_ETH_GLOBAL_INDEX));
	
	/* Added by Jia Lihui 2011-6-20 */
	for(i = 0; i < CHASSIS_SLOT_COUNT; i++)
    {
		system("mkdir -p /dbm/shm/ethport/");
    	sprintf(file_path, "/dbm/shm/ethport/shm%d", i+1);
		
        syslog_ax_eth_port_dbg("file_path = %s\n", file_path);
		fd = open(file_path, O_CREAT | O_RDWR, 00755);
		
    	if(fd < 0)
        {
            syslog_ax_eth_port_dbg("Failed to open! %s\n", strerror(errno));
            return -1;
        }

		length = lseek(fd, sizeof(struct global_ethport_s)*BOARD_GLOBAL_ETHPORTS_MAXNUM-1, SEEK_SET);
        write(fd,"",1);

        start_fp[i] = (struct global_ethport_s *)mmap(NULL, sizeof(struct global_ethport_s)*BOARD_GLOBAL_ETHPORTS_MAXNUM,
    		PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );
		if(start_fp[i] == MAP_FAILED)
		{
            syslog_ax_eth_port_dbg("Failed to mmap for slot%d! %s\n", i+1, strerror(errno));
			close(fd); 			/* code optimize: recourse leak houxx@autelan.com  2013-1-17 */
			return -1;
		}
		memset(start_fp[i], 0, sizeof(struct global_ethport_s)*BOARD_GLOBAL_ETHPORTS_MAXNUM);

		for(j = 0; j < BOARD_GLOBAL_ETHPORTS_MAXNUM; j++)
		{
			start_fp[i][j].isValid = INVALID_ETHPORT;
			
	        global_ethport[i*BOARD_GLOBAL_ETHPORTS_MAXNUM + j] = &start_fp[i][j];
		}

		close(fd);
    }
	
	for (i = 0; i < CHASSIS_SLOT_COUNT; i++ ) {
		syslog_ax_eth_port_dbg("creating eth-port on slot %d module %s\r\n",	\
				i,module_id_str(MODULE_TYPE_ON_SLOT_INDEX(i)));
		for (j = 0; j < ETH_LOCAL_PORT_COUNT(i); j++) 
		{
			if(SYSTEM_TYPE == IS_DISTRIBUTED)
			{
			    npd_create_eth_port_distributed(i,j);
			}
			else
			{
			    npd_create_eth_port(i,j);
			}
		}
	}

    for(i = 0; i < CHASSIS_SLOT_COUNT; i++)
    {
        msync(start_fp[i], sizeof(struct global_ethport_s)*BOARD_GLOBAL_ETHPORTS_MAXNUM, MS_SYNC);
    }
	/*
	 * by qinhs@autelan.com on 10/23/2008 
	 *
	 *	calculate virtual eth-port index for CPU and SPI port.
	 */
	g_cpu_port_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(	\
							CPU_PORT_VIRTUAL_SLOT,CPU_PORT_VIRTUAL_PORT);
	g_spi_port_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(	\
							SPI_PORT_VIRTUAL_SLOT,SPI_PORT_VIRTUAL_PORT);
	
	return;
}

/**********************************************************************************
 *  npd_get_port_by_index
 *
 *	DESCRIPTION:
 * 		get ethernet port info by global eth index
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL  - if parameters given are wrong
 *		portInfo	- if port found
 *		
 **********************************************************************************/
struct eth_port_s *npd_get_port_by_index
(
	unsigned int eth_g_index
)
{
	struct eth_port_s *portInfo = NULL;
	/*eth port index should be in [0,MAX_ETH_GLOBAL_INDEX)*/
	if(eth_g_index > MAX_ETH_GLOBAL_INDEX) {
		return NULL;
	}
	
	portInfo = g_eth_ports[eth_g_index];

	return portInfo;
}

/**********************************************************************************
 *  npd_eth_port_counter_statistics
 *
 *	DESCRIPTION:
 * 		statistics packets info
 *
 *	INPUT:
 *		eth_g_index - port_index
 *		portPtr - pakets info
 *	
 *
 * RETURN:
 *		NPD_FAIL  
 *		NPD_SUCESS 
 *		
 *
 **********************************************************************************/
int npd_eth_port_counter_statistics
(
	unsigned int eth_g_index,
	struct npd_port_counter *portPtr,
	struct port_oct_s		*portOctPtr
)
{
	struct eth_port_s * g_ptr = NULL;

	g_ptr = npd_get_port_by_index(eth_g_index);
	if(g_ptr){
		g_ptr ->counters.rx.goodbytesl += portPtr->rx.goodbytesl;
		g_ptr ->counters.rx.goodbytesh += portPtr->rx.goodbytesh;		
		g_ptr ->counters.rx.badbytes += portPtr->rx.badbytes;	
		g_ptr ->counters.rx.uncastframes += portPtr->rx.uncastframes;
		g_ptr ->counters.rx.bcastframes += portPtr->rx.bcastframes;
		g_ptr ->counters.rx.mcastframes += portPtr->rx.mcastframes;
		g_ptr ->counters.rx.fcframe += portPtr->rx.fcframe;
		g_ptr ->counters.rx.fifooverruns += portPtr->rx.fifooverruns;
		g_ptr ->counters.rx.underSizepkt += portPtr->rx.underSizepkt;
		g_ptr ->counters.rx.fragments += portPtr->rx.fragments;		
		g_ptr ->counters.rx.overSizepkt += portPtr->rx.overSizepkt;
		g_ptr ->counters.rx.jabber += portPtr->rx.jabber;
		g_ptr ->counters.rx.errorframe += portPtr->rx.errorframe;

		g_ptr ->counters.otc.badCRC += portOctPtr->badCRC;
		g_ptr ->counters.otc.collision += portOctPtr->collision;
		g_ptr ->counters.otc.late_collision += portOctPtr->late_collision;
		
		g_ptr ->counters.tx.sent_deferred += portPtr->tx.sent_deferred;
		g_ptr ->counters.tx.goodbytesl += portPtr->tx.goodbytesl;
		g_ptr ->counters.tx.goodbytesh += portPtr->tx.goodbytesh;
		g_ptr ->counters.tx.uncastframe += portPtr->tx.uncastframe;
		g_ptr ->counters.tx.excessiveCollision += portPtr->tx.excessiveCollision;
		g_ptr ->counters.tx.mcastframe += portPtr->tx.mcastframe;	
		g_ptr ->counters.tx.bcastframe += portPtr->tx.bcastframe;
		g_ptr ->counters.tx.sentMutiple += portPtr->tx.sentMutiple;
		g_ptr ->counters.tx.fcframe += portPtr->tx.fcframe;
		g_ptr ->counters.tx.crcerror_fifooverrun += portPtr->tx.crcerror_fifooverrun;

		g_ptr ->counters.otc.b1024oct2max += portOctPtr->b1024oct2max;
		g_ptr ->counters.otc.b512oct21023 += portOctPtr->b512oct21023;
		g_ptr ->counters.otc.b256oct511 += portOctPtr->b256oct511;
		g_ptr ->counters.otc.b128oct255 += portOctPtr->b128oct255;
		g_ptr ->counters.otc.b64oct127 += portOctPtr->b64oct127;
		g_ptr ->counters.otc.b64oct += portOctPtr->b64oct;
		
		return NPD_SUCCESS;
	}
	else{
		return NPD_FAIL;
	}
}

/**********************************************************************************
 *  npd_get_subcard_port_type
 *
 *	DESCRIPTION:
 * 		get 5612, 5612i, 4626 subcard type port index
 *
 *	INPUT:
 *		unsigned int - panel port number (currently start from 1);
 *	
 *	OUTPUT:
 *		unsigned char - subcard port type
 *						01 ----- AX51-XFP
 *						10 ----- AX51-GTX
 *						11 ----- AX51-SFP
 *
 * 	RETURN:
 * 		unsigned char - subcard whether on or not
 *											
 *
 **********************************************************************************/
unsigned char npd_get_subcard_port_type
(
	unsigned int panel_port,
	unsigned char* subcard_type
)
{
	unsigned char ret = 0, port_type = 0;
	int card_num = 0;

	/* map panel port to driver logic port */
	card_num = panel_port - 1;

	syslog_ax_eth_port_dbg("get whether subcard port %d on or not\n",card_num);
	ret = nbm_cvm_query_card_on(card_num);
	/*********************
	 *0 ----- card is on
	 *1 ----- card is off
	 *********************/ 
	if(0 == ret) {
		syslog_ax_eth_port_dbg("subcard %d is on\n",card_num);
		port_type = cvm_query_card_type(card_num);
		syslog_ax_eth_port_dbg("subcard %d on get port type %s\n", \
					card_num,(!port_type || port_type > 3) ? "ERR": ax51_card_desc[port_type-1]);
		if(1 == port_type) {
			*subcard_type = AX51_XFP;
		}
		else if(2 == port_type) {
			*subcard_type = AX51_GTX;
		}
		else if(3 == port_type) {
			*subcard_type = AX51_SFP;
		}

		/* 
		  *  	product AX5612i has only one type of sub module: AX51-XFP
		  *	if CPLD isnot lastest version, which means port type we read 
		  * 	from CPLD register may be wrong, we must recognize it manually
		  *
		  *	by qinhs@autelan.com 2010-9-8
		  */
		if((AX51_XFP != *subcard_type) && (PRODUCT_ID_AX5K_I == PRODUCT_ID)) {
			syslog_ax_eth_port_dbg("we recognized AX51-XFP, but cpld may need upgrade!\n");
			*subcard_type = AX51_XFP;
		}
	}
	else if(1 == ret) {
		syslog_ax_eth_port_dbg("subcard %d is off\n",card_num);
	}
	else {
		syslog_ax_eth_port_err("Get card %d state error ret %d\n",card_num,ret);
	}
	return ret;
}

/**********************************************************************************
 *  npd_check_subcard_type
 *
 *	DESCRIPTION:
 * 		get 5612, 5612i, 4626 subcard type
 *
 *	INPUT:
 * 		unsigned int - panel port number (currently start from 1);
 *	
 *	OUTPUT:
 *  	NULL
 *
 * 	RETURN:
 * 		unsigned char - subcard port type
 *						01 ----- AX51-XFP
 *						10 ----- AX51-GTX
 *						11 ----- AX51-SFP					
 *		NPD_FALSE  -  subcard is not on its slot
 **********************************************************************************/
unsigned char npd_check_subcard_type
(
	unsigned int product_Id,
	unsigned int panel_port
)
{
	unsigned char ret = 0, port_type = 0;
	int card_num = 0;

	/* map panel port to driver logic port */
	card_num = panel_port - 1;

	if(PRODUCT_ID_AX5K == product_Id || PRODUCT_ID_AX5K_I == product_Id || PRODUCT_ID_AU4K == product_Id) {
		ret = nbm_cvm_query_card_on(card_num);
		/*********************
		 *0 ----- card is on
		 *1 ----- card is off
		 *********************/ 
		if(0 == ret) {
			syslog_ax_eth_port_dbg("subcard %d is on\n",card_num);
			port_type = cvm_query_card_type(card_num);
			syslog_ax_eth_port_dbg("subcard %d on get port type %s\n", \
						card_num,(!port_type || port_type > 3) ? "ERR": ax51_card_desc[port_type-1]);
			
			/* 
			  * 	product AX5612i has only one type of sub module: AX51-XFP
			  * 	if CPLD isnot lastest version, which means port type we read 
			  * 	from CPLD register may be wrong, we must recognize it manually
			  *
			  * by qinhs@autelan.com 2010-9-8
			  */
			if((AX51_XFP != port_type) && (PRODUCT_ID_AX5K_I == PRODUCT_ID)) {
				syslog_ax_eth_port_dbg("we recognized AX51-XFP, but cpld may need upgrade!\n");
				return AX51_XFP;
			}
			if(1 == port_type) {
				return AX51_XFP;
			}
			else if(2 == port_type) {
				return AX51_GTX;
			}
			else if(3 == port_type) {
				return AX51_SFP;
			}
			else {
				syslog_ax_eth_port_err("Get subcard %d type error port_type %d\n",card_num,port_type);
				return NPD_FALSE;			
			}
		}
		else if(1 == ret) {
			syslog_ax_eth_port_dbg("subcard %d is off\n",card_num);
			return NPD_FALSE;
		}
		else {
			syslog_ax_eth_port_err("Get card %d state error ret %d\n",card_num,ret);
			return NPD_FALSE;		
		}
	}
	else {
		syslog_ax_eth_port_dbg("only 4626, 5612, 5612i have subcard\n");
		return NPD_FALSE;
	}
}

/**********************************************************************************
 *  npd_eth_port_set_pvid
 *
 *	DESCRIPTION:
 * 		set ethernet port vid(port-based vlan id)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occurred
 *		NPD_FAIL	 - if error occurred
 *		
 **********************************************************************************/
unsigned int npd_eth_port_set_pvid
(
	unsigned int   eth_g_index,
	unsigned short vlanId
)
{
	unsigned int ret = NPD_SUCCESS;
	
	struct eth_port_s *portInfo = NULL;
	struct port_based_vlan_s  *pvid = NULL;
	/*eth port index should be in [0,MAX_ETH_GLOBAL_INDEX)*/
	if(eth_g_index > MAX_ETH_GLOBAL_INDEX) {
		syslog_ax_eth_port_err("npd eth port set pvid index %#0x error",eth_g_index);	
		return NPD_FAIL;
	}

	portInfo = g_eth_ports[eth_g_index];
	if(NULL == portInfo) {
		syslog_ax_eth_port_err("npd eth port set pvid port %#0x null",eth_g_index);
		return NPD_FAIL;
	}

	pvid = portInfo->funcs.func_data[ETH_PORT_FUNC_PORT_BASED_VLAN];
	if(NULL != pvid) {
		syslog_ax_eth_port_err("npd eth port pvid change %d <-> %d",pvid->vid,vlanId);
		pvid->vid = vlanId;
		pvid->npd_vlan_untagports_notifier_func = (NPD_ETH_PORT_LINK_CHANGE_NOTIFIER_FUNC)npd_vlan_untag_port_link_change;
		ret = NPD_SUCCESS;
	}
	else {
		pvid = (struct port_based_vlan_s *)malloc(sizeof(struct port_based_vlan_s));
		if(NULL == pvid) {
			syslog_ax_eth_port_err("npd eth port set pvid memory alloc error");
			return NPD_FAIL;
		}
		memset(pvid,0,sizeof(struct port_based_vlan_s));

		pvid->vid = vlanId;
		pvid->npd_vlan_untagports_notifier_func = (NPD_ETH_PORT_LINK_CHANGE_NOTIFIER_FUNC)npd_vlan_untag_port_link_change;
		portInfo->funcs.func_data[ETH_PORT_FUNC_PORT_BASED_VLAN] = pvid;
		ret = NPD_SUCCESS;
	}

	return ret;
}

/**********************************************************************************
 *  npd_eth_port_get_pvid
 *
 *	DESCRIPTION:
 * 		get ethernet port vid(port-based vlan id)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		vlanId - vlan id
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occurred
 *		NPD_FAIL	 - if error occurred
 *		
 **********************************************************************************/
unsigned int npd_eth_port_get_pvid
(
	unsigned int   eth_g_index,
	unsigned short *vlanId
)
{
	unsigned int ret = NPD_SUCCESS;
	
	struct eth_port_s *portInfo = NULL;
	struct port_based_vlan_s  *pvid = NULL;
	/*eth port index should be in [0,MAX_ETH_GLOBAL_INDEX)*/
	if(eth_g_index > MAX_ETH_GLOBAL_INDEX) {
		syslog_ax_eth_port_err("npd eth port get pvid index %#0x error",eth_g_index);	
		ret = NPD_FAIL;
		return ret;
	}

	portInfo = g_eth_ports[eth_g_index];
	if(NULL == portInfo) {
		syslog_ax_eth_port_err("npd eth port get pvid port %#0x null",eth_g_index);
		ret = NPD_FAIL;
		return ret;
	}
	
	pvid = portInfo->funcs.func_data[ETH_PORT_FUNC_PORT_BASED_VLAN];
	if(NULL == pvid) {
		syslog_ax_eth_port_err("npd eth port pvid set error");
		ret = NPD_FAIL;
	}
	else {
		*vlanId = pvid->vid;
		ret = NPD_SUCCESS;
	}


	return ret;
}

/**********************************************************************************
 *  npd_eth_port_clear_pvid
 *
 *	DESCRIPTION:
 * 		clear or delete ethernet port vid(port-based vlan id)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occurred
 *		NPD_FAIL	 - if error occurred
 *		
 **********************************************************************************/
unsigned int npd_eth_port_clear_pvid
(
	unsigned int   eth_g_index
)
{
	unsigned int ret = NPD_SUCCESS;
	
	struct eth_port_s *portInfo = NULL;
	struct port_based_vlan_s  *pvid = NULL;
	/*eth port index should be in [0,MAX_ETH_GLOBAL_INDEX)*/
	if(eth_g_index > MAX_ETH_GLOBAL_INDEX) 
	{
		syslog_ax_eth_port_err("npd eth port clear pvid index %#0x error",eth_g_index);	
		return NPD_FAIL;		/* code optimize: Dereference after null check houxx@autelan.com  2013-1-17 */
	}

	portInfo = g_eth_ports[eth_g_index];
	if(NULL == portInfo) 
	{
		syslog_ax_eth_port_err("npd eth port %#0x clear pvid port null",eth_g_index);
		return NPD_FAIL;		/* code optimize: Dereference after null check houxx@autelan.com  2013-1-17 */
	}

	pvid = portInfo->funcs.func_data[ETH_PORT_FUNC_PORT_BASED_VLAN];
	if(NULL == pvid) 
	{
		syslog_ax_eth_port_err("npd eth port %#0x pvid does not exists",eth_g_index);
		ret = NPD_FAIL;
	}
	else
	{
		portInfo->funcs.func_data[ETH_PORT_FUNC_PORT_BASED_VLAN] = NULL;
		free(pvid);
		pvid = NULL;
		ret = NPD_SUCCESS;
	}

	return ret;
}

/**********************************************************************************
 *  npd_eth_port_dot1q_add
 *
 *	DESCRIPTION:
 * 		set ethernet port vid(802.1q-based vlan id)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occurred
 *		NPD_FAIL	 - if error occurred
 *		
 **********************************************************************************/
unsigned int npd_eth_port_dot1q_add
(
	unsigned int   eth_g_index,
	unsigned short vlanId
)
{
	unsigned int ret = NPD_SUCCESS;
	
	struct eth_port_s *portInfo = NULL;
	dot1q_vlan_list_s  *dot1qList = NULL;
	struct eth_port_dot1q_list_s *dot1qNode = NULL;
	
	/*eth port index should be in [0,MAX_ETH_GLOBAL_INDEX)*/
	if(eth_g_index > MAX_ETH_GLOBAL_INDEX) {
		syslog_ax_eth_port_err("eth port %#x set dot1q vlan %d port out of range\n",eth_g_index,vlanId);	
		return NPD_FAIL;
	}

	portInfo = g_eth_ports[eth_g_index];
	if(NULL == portInfo) {
		syslog_ax_eth_port_err("eth port %#x set dot1q vlan %d port null\n",eth_g_index,vlanId);
		return NPD_FAIL;
	}

	dot1qList = portInfo->funcs.func_data[ETH_PORT_FUNC_DOT1Q_BASED_VLAN];
	if(NULL == dot1qList) { /* create dot1q vlan list*/
		syslog_ax_eth_port_dbg("eth port %#0x add first dot1q vlan %d\n",eth_g_index,vlanId);
		dot1qList = (dot1q_vlan_list_s *)malloc(sizeof(dot1q_vlan_list_s));
		if(NULL == dot1qList) {
			syslog_ax_eth_port_err("eth port %#x add first dot1q vlan %d memory err\n",eth_g_index,vlanId);
			return NPD_FAIL;
		}
		else {
			memset(dot1qList, 0, sizeof(dot1q_vlan_list_s));
			INIT_LIST_HEAD(&(dot1qList->list));
			portInfo->funcs.func_data[ETH_PORT_FUNC_DOT1Q_BASED_VLAN] = dot1qList;
		}
	}

	dot1qList->npd_link_change_notifier_func = (NPD_ETH_PORT_LINK_CHANGE_NOTIFIER_FUNC)npd_vlan_tag_port_link_change;
	dot1qNode = (struct eth_port_dot1q_list_s*)malloc(sizeof(struct eth_port_dot1q_list_s));
	if(NULL == dot1qNode) {
		syslog_ax_eth_port_err("eth port %#x add dot1q vlan %d memory err\n",eth_g_index,vlanId);
		return NPD_FAIL;
	}
	else {
		memset(dot1qNode,0,sizeof(struct eth_port_dot1q_list_s));
		dot1qNode->vid = vlanId;
		list_add(&(dot1qNode->list),&(dot1qList->list));
		dot1qList->count++;
		syslog_ax_eth_port_dbg("eth port %#x current dot1q vlan node count %d after add\n",eth_g_index,dot1qList->count);
		ret = NPD_SUCCESS;
	}

	return ret;
}

/**********************************************************************************
 *  npd_eth_port_dot1q_remove
 *
 *	DESCRIPTION:
 * 		remove ethernet port vid(802.1q-based vlan id)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occurred
 *		NPD_FAIL	 - if error occurred
 *		
 **********************************************************************************/
unsigned int npd_eth_port_dot1q_remove
(
	unsigned int   eth_g_index,
	unsigned short vlanId
)
{
	unsigned int ret = NPD_SUCCESS;
	
	struct eth_port_s *portInfo = NULL;
	dot1q_vlan_list_s  *dot1qList = NULL;
	struct eth_port_dot1q_list_s *dot1qNode = NULL;
	struct list_head *list=NULL,*pos=NULL;
	
	/*eth port index should be in [0,MAX_ETH_GLOBAL_INDEX)*/
	if(eth_g_index > MAX_ETH_GLOBAL_INDEX) {
		syslog_ax_eth_port_err("eth port %#0x remove dot1q vlan %d port out of range\n",eth_g_index,vlanId);	
		return NPD_FAIL;
	}

	portInfo = g_eth_ports[eth_g_index];
	if(NULL == portInfo) {
		syslog_ax_eth_port_err("eth port %#0x remove dot1q vlan %d port null\n",eth_g_index,vlanId);
		return NPD_FAIL;
	}

	dot1qList = portInfo->funcs.func_data[ETH_PORT_FUNC_DOT1Q_BASED_VLAN];
	if(NULL == dot1qList) { /*dot1q vlan list doesnot exists*/
		syslog_ax_eth_port_err("eth port %#0x remove dot1q vlan %d but list null\n",eth_g_index,vlanId);
		return NPD_FAIL;
	}
	else {	
		list = &(dot1qList->list);
		__list_for_each(pos,list) {
			dot1qNode = list_entry(pos,struct eth_port_dot1q_list_s,list);
			if(vlanId == dot1qNode->vid) {
				list_del(&(dot1qNode->list));
				break;
			}
		}
		free(dot1qNode);
		if(dot1qList->count > 0) {
			dot1qList->count--;
		}
		syslog_ax_eth_port_dbg("eth port %#0x current dot1q vlan node count %d after remove\n",	\
							eth_g_index ,dot1qList->count);
		if(0 == dot1qList->count) {
			free(dot1qList);
			portInfo->funcs.func_data[ETH_PORT_FUNC_DOT1Q_BASED_VLAN] = NULL;
		}
		
		ret = NPD_SUCCESS;
	}

	return ret;
}

unsigned int npd_eth_port_based_vlan_add
(
	unsigned int eth_g_index,
	unsigned short vlanId
)
{
	struct eth_port_s	*ethPort = NULL;
	struct port_based_vlan_s *untagVlan= NULL;
	
	untagVlan= (struct port_based_vlan_s *)malloc(sizeof(struct port_based_vlan_s));
	if(NULL == untagVlan)
	{
		syslog_ax_eth_port_dbg("memory allocate Error!\r\n");
		return NPD_FAIL;
	}
	untagVlan->vid = vlanId;

	ethPort = npd_get_port_by_index(eth_g_index);
	if(NULL == ethPort)
	{
		syslog_ax_eth_port_dbg("NOT find any eth port Struct!\r\n");
		free(untagVlan);		/* code optimize: recourse leak houxx@autelan.com  2013-1-17 */
		untagVlan = NULL;
		return NPD_FAIL;
	}
	/*ethPort->funcs.funcs_run_bitmap |=1 << ETH_PORT_FUNC_PORT_BASED_VLAN;*/
	ethPort->funcs.func_data[ETH_PORT_FUNC_PORT_BASED_VLAN] = untagVlan;

	return NPD_SUCCESS;
}


unsigned int npd_eth_port_based_vlan_del
(
	unsigned int eth_g_index,
	unsigned short vlanId
)
{
	struct eth_port_s	*ethPort = NULL;
	struct port_based_vlan_s *untagVlan= NULL;
	ethPort = npd_get_port_by_index(eth_g_index);
	if(NULL == ethPort)
	{
		syslog_ax_eth_port_dbg("NOT find any eth port Struct!\r\n");
		return NPD_FAIL;
	}
	/*ethPort->funcs.funcs_run_bitmap &=~(1 << ETH_PORT_FUNC_PORT_BASED_VLAN);*/

	untagVlan = ethPort->funcs.func_data[ETH_PORT_FUNC_PORT_BASED_VLAN];
	if(NULL != untagVlan){
		untagVlan->vid = vlanId;
	}

	return NPD_SUCCESS;
	
}

/**********************************************************************************
 *  npd_eth_port_set_ptrunkid
 *
 *	DESCRIPTION:
 * 		set ethernet port trunkid(port-based trunk id)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		trunkId - trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occurred
 *		NPD_FAIL	 - if error occurred
 *		
 **********************************************************************************/
unsigned int npd_eth_port_set_ptrunkid
(
	unsigned int   eth_g_index,
	unsigned short trunkId
)
{
	unsigned int ret = NPD_SUCCESS;
	
	struct eth_port_s *portInfo = NULL;
	struct port_based_trunk_s  *ptrunkId = NULL;
	/*eth port index should be in [0,MAX_ETH_GLOBAL_INDEX]*/
	if(eth_g_index > MAX_ETH_GLOBAL_INDEX) {
		syslog_ax_eth_port_err("eth port set ptrunkId index %#0x error\n",eth_g_index);	
		return NPD_FAIL;
	}

	portInfo = g_eth_ports[eth_g_index];
	if(NULL == portInfo) {
		syslog_ax_eth_port_err("eth port set ptrunkId port %#0x null\n",eth_g_index);
		return NPD_FAIL;
	}
	ptrunkId = portInfo->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION];
	if(NULL != ptrunkId) {
		ptrunkId->trunkId = trunkId;
		ptrunkId->npd_trunk_ports_notifier_func = (NPD_ETH_PORT_LINK_CHANGE_NOTIFIER_FUNC)npd_trunk_port_link_change;
		syslog_ax_eth_port_dbg("eth port ptrunkid set %d\n",ptrunkId->trunkId);		
		ret = NPD_SUCCESS;
	}
	else {
		syslog_ax_eth_port_dbg("set port %d ptrunkId first time\n",eth_g_index);
		ptrunkId = (struct port_based_trunk_s *)malloc(sizeof(struct port_based_trunk_s));
		if(NULL == ptrunkId) {
			syslog_ax_eth_port_err("eth port set ptrunkId memory alloc error!\n");
			return NPD_FAIL;
		}
		memset(ptrunkId,0,sizeof(struct port_based_trunk_s));

		ptrunkId->trunkId= trunkId;
		ptrunkId->npd_trunk_ports_notifier_func = (NPD_ETH_PORT_LINK_CHANGE_NOTIFIER_FUNC)npd_trunk_port_link_change;
		portInfo->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION] = ptrunkId;
		ret = NPD_SUCCESS;
	}
	return ret;
}

/**********************************************************************************
 *  npd_eth_port_get_ptrunkid
 *
 *	DESCRIPTION:
 * 		get ethernet port trunkid(port-based trunk id)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		trunkId - trunk id
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occurred
 *		NPD_FAIL	 - if error occurred
 *		
 **********************************************************************************/
unsigned int npd_eth_port_get_ptrunkid
(
	unsigned int   eth_g_index,
	unsigned short *trunkId
)
{
	unsigned int ret = NPD_SUCCESS;
	
	struct eth_port_s *portInfo = NULL;
	struct port_based_trunk_s  *ptrunkId = NULL;
	/*eth port index should be in [0,MAX_ETH_GLOBAL_INDEX)*/
	if(eth_g_index > MAX_ETH_GLOBAL_INDEX) {
		syslog_ax_eth_port_err("npd eth port get pvid index %#0x error",eth_g_index);	
		ret = NPD_FAIL;
		return ret;
	}

	portInfo = g_eth_ports[eth_g_index];
	if(NULL == portInfo) {
	    syslog_ax_eth_port_err("npd eth port get ptrunkId port %#0x null",eth_g_index);
		ret = NPD_FAIL;
		return ret;
	}
	
	ptrunkId = portInfo->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION];
	if(NULL == ptrunkId) {
		syslog_ax_eth_port_dbg("check found eth-port %#x not in any LAG\n", eth_g_index);
		ret = NPD_FAIL;
	}
	else {
		*trunkId = ptrunkId->trunkId;
		ret = NPD_SUCCESS;
	}
	
	return ret;
}


/**********************************************************************************
 *  npd_eth_port_clear_ptrunkid
 *
 *	DESCRIPTION:
 * 		clear or delete ethernet port vid(port-based vlan id)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occurred
 *		NPD_FAIL	 - if error occurred
 *		
 **********************************************************************************/
unsigned int npd_eth_port_clear_ptrunkid
(
	unsigned int   eth_g_index
)
{
	unsigned int ret = NPD_SUCCESS;
	
	struct eth_port_s *portInfo = NULL;
	struct port_based_trunk_s  *ptrunkId = NULL;
	/*eth port index should be in [0,MAX_ETH_GLOBAL_INDEX)*/
	if(eth_g_index > MAX_ETH_GLOBAL_INDEX) {
		syslog_ax_eth_port_err("npd eth port clear ptrunkid index %#0x error",eth_g_index);	
		return NPD_FAIL;	/* code optimize: Dereference after null check houxx@autelan.com  2013-1-17 */
	}

	portInfo = g_eth_ports[eth_g_index];
	if(NULL == portInfo) {
		syslog_ax_eth_port_err("npd eth port %#0x clear ptrunkid port null",eth_g_index);
		return NPD_FAIL;	/* code optimize: Dereference after null check houxx@autelan.com  2013-1-17 */
	}

	ptrunkId = portInfo->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION];
	if(NULL == ptrunkId) {
		syslog_ax_eth_port_err("npd eth port %#0x ptrunkid does not exists",eth_g_index);
		ret = NPD_FAIL;
	}
	else {
		portInfo->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION] = NULL;
		free(ptrunkId);
		ret = NPD_SUCCESS;
	}

	return ret;
}

/**********************************************************************************
 *  npd_get_eth_ports_status
 *
 *	DESCRIPTION:
 * 		get ethernet port status
 *
 *	INPUT:
 *		NULL
 *		
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_get_eth_ports_status() {
	int i,j;
	unsigned int eth_global_index, port_attr = 0;
	struct eth_port_s *eth_ports = NULL;

	for(i = 0; i < CHASSIS_SLOT_COUNT; i++) {
		for(j = 0; j < ETH_LOCAL_PORT_COUNT(i); j++) {
			eth_global_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(i,j);
			
			eth_ports = g_eth_ports[eth_global_index];
			
			if(NULL == eth_ports) {
				syslog_ax_eth_port_err("eth-port %d not created!\n",eth_global_index);
				continue;
			}

			/* save previous port attribute map*/
			port_attr = eth_ports->attr_bitmap;

            if((PRODUCT_ID_AX7K == PRODUCT_ID)&& (AX7_CRSMU_SLOT_NUM == i))
            {
				/*slot 0 is CRSMU, read ethernet port from cavium-ethernet driver for RGMII port info*/
				/* TODO: read RGMII ethernet port info from cavium-ethernet driver*/
				nbm_read_eth_port_info(j,eth_ports);					
            }
            else if((PRODUCT_ID_AX7K_I == PRODUCT_ID)&& ((0==i)||(1==i)))
            {
				/*slot 0 & 1 is CRSMU, read ethernet port from cavium-ethernet driver for RGMII port info*/
				/* TODO: read RGMII ethernet port info from cavium-ethernet driver*/
				nbm_read_eth_port_info(j,eth_ports);					
            }   /* zhangdi add the new branch for 7605i, 2011-04-29*/
			else if((PRODUCT_ID_AX5K_I == PRODUCT_ID) \
					&& (eth_global_index >= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9)) \
					&& (eth_global_index <= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,12))) {
				syslog_ax_eth_port_dbg("get port(%d %d) information from nbm\n",i,j);
				eth_global_index = eth_global_index - ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9);
				nbm_read_eth_port_info(eth_global_index,eth_ports);
			}			
			else if((PRODUCT_ID_AX5K_E == PRODUCT_ID) \
					&& (eth_global_index >= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,1)) \
					&& (eth_global_index <= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,8))) {
				syslog_ax_eth_port_dbg("get port(%d %d) information from nbm\n",i,j);
				eth_global_index = eth_global_index - ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,1);
				nbm_read_eth_port_info(eth_global_index,eth_ports);
			}			
			else if((PRODUCT_ID_AX5608== PRODUCT_ID) \
					&& (eth_global_index >= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,1)) \
					&& (eth_global_index <= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,4))) {
				syslog_ax_eth_port_dbg("get port(%d %d) information from nbm\n",i,j);
				eth_global_index = eth_global_index - ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,1);
				nbm_read_eth_port_info(eth_global_index,eth_ports);
			}
			else {	/*sub card ethernet port, read from ASIC driver for GE and XG ports*/
				nam_read_eth_port_info(MODULE_TYPE_ON_SLOT_INDEX(i),CHASSIS_SLOT_INDEX2NO(i),	\
								ETH_LOCAL_INDEX2NO(i,j),eth_ports);
			}		
			syslog_ax_eth_port_dbg("\tread port %#x mtu %d attribute %#x\n",	\
						eth_global_index,eth_ports->mtu,eth_ports->attr_bitmap);
			if(port_attr && port_attr != eth_ports->attr_bitmap) {
				syslog_ax_eth_port_evt("\tport %#x state change %#x -> %#x\n",	\
						eth_global_index,port_attr,eth_ports->attr_bitmap);
			}
		}
	}
}

/**********************************************************************************
 *  npd_scan_eth_ports_link_status
 *
 *	DESCRIPTION:
 * 		link scan ethernet port status
 *
 *	INPUT:
 *		NULL
 *		
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_scan_eth_ports_link_status() 
{
	int i,j;
	unsigned int eth_g_index, port_attr = 0, linkStatus = 0, ret = NPD_SUCCESS;
	struct eth_port_s *eth_ports = NULL;

	for(i = 0; i < CHASSIS_SLOT_COUNT; i++) {
		for(j = 0; j < ETH_LOCAL_PORT_COUNT(i); j++) {
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(i,j);
			
			eth_ports = g_eth_ports[eth_g_index];
			
			if(NULL == eth_ports) {
				syslog_ax_eth_port_err("eth-port %d not created!\n",eth_g_index);
				continue;
			}

			/* save previous port attribute map*/
			port_attr = eth_ports->attr_bitmap;

			if(PRODUCT_IS_BOX ||(AX7_CRSMU_SLOT_NUM != i)) {	/*slot 0 is CRSMU, read ethernet port from cavium-ethernet driver for RGMII port info*/
                if((PRODUCT_ID ==PRODUCT_ID_AX7K_I)&&((0==i)||(1==i))) 
                {
				    continue;    /* zhangdi add for 7605i, when CRSMU on slot 1 */	
                }/* add end */
				ret = npd_get_port_link_status(eth_g_index, &linkStatus);
				if(NPD_SUCCESS == ret) {
					port_attr &= ~(1 << ETH_LINK_STATUS_BIT);
					port_attr |= ((linkStatus << ETH_LINK_STATUS_BIT) & ETH_ATTR_LINK_STATUS);
					/* update port attribute map*/
					if(port_attr && port_attr != eth_ports->attr_bitmap) {
						syslog_ax_eth_port_evt("\tport %#x state change %#x -> %#x\n",	\
								eth_g_index, port_attr, eth_ports->attr_bitmap);
						if(portNotifier) {
							portNotifier(eth_g_index,( linkStatus ? ETH_PORT_NOTIFIER_LINKUP_E : ETH_PORT_NOTIFIER_LINKDOWN_E));
						}
					}  
					eth_ports->attr_bitmap = port_attr;
				}
			}
		}
	}
	return;
}

/**********************************************************************************
 *  npd_get_port_admin_status
 *
 *	DESCRIPTION:
 * 		get ethernet port admin status:enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *	
 *	OUTPUT:
 *		status - admin status
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_get_port_admin_status
(
	IN unsigned int eth_g_index,
	OUT unsigned int *status
)
{
	unsigned char devNum;
	unsigned char portNum;
	unsigned long attr = 0;
	
	npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
	nam_get_port_en_dis(devNum,portNum,&attr);
	if(ETH_ATTR_ENABLE == attr) {
		*status = ETH_ATTR_ENABLE;
	}
	else if(ETH_ATTR_DISABLE == attr) {
		*status = ETH_ATTR_DISABLE;
	}

	return;
}

/**********************************************************************************
 *  npd_set_port_admin_status
 *
 *	DESCRIPTION:
 * 		set ethernet port admin status:enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		status -admin status enable or disable
 *		isBoard - port is on main board or not( 1 - main board port, 0 - slave board port)
 *	
 *	RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *		
 *
 **********************************************************************************/
int npd_set_port_admin_status
(
	IN unsigned int eth_g_index,
	IN unsigned int status,
	IN unsigned int isBoard
)
{
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	unsigned long attr = status;
	int ret = ETHPORT_RETURN_CODE_ERR_NONE;

	if(NPD_FALSE == isBoard) {
		ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
		if(0 != ret)
			return ETHPORT_RETURN_CODE_ERR_GENERAL;
		npd_syslog_dbg("Set gindex %d virport%d states %d\n",eth_g_index,portNum,status);
		ret = nam_set_ethport_enable(devNum,portNum,attr);
		if(0 != ret) {
			ret = ETHPORT_RETURN_CODE_ERR_HW;
		}
		else {
			ret = ETHPORT_RETURN_CODE_ERR_NONE;
		}		
		#if 0
		if((NPD_SUCCESS == ret)&&(NPD_OK == status)) {
			nam_fdb_table_delete_entry_with_port(eth_g_index);
		}
		#endif
	}
	else if(NPD_TRUE == isBoard) {
		syslog_ax_eth_port_dbg("config board %d admin status %d\n ",eth_g_index,status);
		ret = nbm_set_ethport_enable(eth_g_index,status);
	}
	
	return ret;
}

/**********************************************************************************
 *  npd_get_port_link_status
 *
 *	DESCRIPTION:
 * 		get ethernet port link status:link up or link down
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *	
 *	OUTPUT:
 *		status - admin status
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
unsigned int npd_get_port_link_status
(
	IN unsigned int eth_g_index,
	OUT unsigned int *status
)
{
	unsigned char devNum;
	unsigned char portNum;
	unsigned long attr = 0;
	unsigned int ret = NPD_SUCCESS;
	
	ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
	if(NPD_SUCCESS != ret) {
		return ret;
	}
	/*syslog_ax_eth_port_dbg("get port (%d %d) link status\n",devNum,portNum);*/
	ret = nam_get_port_link_state(devNum,portNum,&attr);
	if(NPD_SUCCESS != ret) {
		return ret;
	}
	
	if(ETH_ATTR_LINKUP == attr) {
		*status = ETH_ATTR_LINKUP;
	}
	else if(ETH_ATTR_LINKDOWN == attr) {
		*status = ETH_ATTR_LINKDOWN;
	}

	return ret;
}
/**********************************************************************************
 *  npd_set_port_link_status
 *
 *	DESCRIPTION:
 * 		set ethernet port link status:link up or link down
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		status -linkstatus enable or disable
 *		isBoard - port is on main board or not( 1 - main board port, 0 - slave board port)
 *
 * RETURN:
 *		ETHPORT_RETURN_CODE_ERR_GENERAL
 *		ETHPORT_RETURN_CODE_ERR_NONE
 *      ETHPORT_RETURN_CODE_ERR_HW
 *		ETHPORT_RETURN_CODE_UNSUPPORT
 *
 **********************************************************************************/
int npd_set_port_link_status
(
	IN unsigned int eth_g_index,
	IN unsigned int status,
	IN unsigned int isBoard
)
{
	unsigned char devNum;
	unsigned char portNum;
	unsigned long attr = status;
	int ret = ETHPORT_RETURN_CODE_ERR_NONE;
	if(NPD_FALSE == isBoard) {
		ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
		if(0 != ret)
			return ETHPORT_RETURN_CODE_ERR_GENERAL;
		
		if(1 == attr) {
			ret = nam_set_ethport_force_linkup(devNum,portNum,1);	
			if(0 != ret) {
				ret = ETHPORT_RETURN_CODE_ERR_HW;
			}
			else {
				ret = ETHPORT_RETURN_CODE_ERR_NONE;
			}				
		}
		else if(0 == attr) {
			ret = nam_set_ethport_force_linkdown(devNum, portNum,1);
			if(0 != ret) {
				ret = ETHPORT_RETURN_CODE_ERR_HW;
			}
			else {
				ret = ETHPORT_RETURN_CODE_ERR_NONE;
			}	
		}
		else if(2 == attr) {
			ret = nam_set_ethport_force_auto(devNum,portNum,0);
			if(0 != ret) {
				ret = ETHPORT_RETURN_CODE_ERR_HW;
			}
			else {
				ret = ETHPORT_RETURN_CODE_ERR_NONE;
			}			
		}
	}
	else if(NPD_TRUE == isBoard) {
		syslog_ax_eth_port_dbg("config board %d link %d\n ",eth_g_index,status);
		ret = nbm_set_ethport_force_linkup(eth_g_index,status);
	}

	return ret;
}
/**********************************************************************************
 *  npd_get_port_autoNego_status
 *
 *	DESCRIPTION:
 * 		get ethernet port auto-negotiation status:enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *	
 *	OUTPUT:
 *		status - auto-negotiation status
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_get_port_autoNego_status
(
	unsigned int eth_g_index,
	unsigned int *status
)
{
	unsigned char devNum;
	unsigned char portNum;
	unsigned long attr = 0;
	int ret = 0;
	
	ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
	if(NPD_SUCCESS != ret)/* code optimize: Unchecked return value  houxx@autelan.com  2013-1-17 */
	{
		return;
	}
	
	ret = nam_get_port_autoneg_state(devNum,portNum,&attr);
	if(0 != ret)
	{
		return;
	}
	if(ETH_ATTR_AUTONEG_DONE == attr) {
		*status = ETH_ATTR_AUTONEG_DONE;
	}
	else if(ETH_ATTR_AUTONEG_NOT_DONE== attr) {
		*status = ETH_ATTR_AUTONEG_NOT_DONE;
	}

	return;
}

/**********************************************************************************
 *  npd_set_port_autoNego_status
 *
 *	DESCRIPTION:
 * 		set ethernet port auto-negotiation status:enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		status -autoNego status enable or disable
 *		isBoard - port is on main board or not( 1 - main board port, 0 - slave board port)
 *	
 * RETURN:
 *		NPD_FALI
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_autoNego_status
(
	unsigned int eth_g_index,
	unsigned int status,
	unsigned int isBoard
)
{
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	unsigned long attr = status;
	unsigned long portState = 0;
	unsigned int pauseAdvertise = 1;
	unsigned int slot_index = 0;
	int ret = ETHPORT_RETURN_CODE_ERR_NONE;
	struct eth_port_s *ethPortInfo = NULL;
	unsigned int attr_bitmap = 0;
	enum module_id_e module_type;
 
	if(NPD_FALSE == isBoard) 
	{		
		slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
		module_type = MODULE_TYPE_ON_SLOT_INDEX(slot_index);
		
		ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
		if(0 != ret)
		{
			return ETHPORT_RETURN_CODE_ERR_GENERAL;
		}

		nam_get_port_en_dis(devNum,portNum,&portState);
		if(ETH_ATTR_ENABLE == portState) 
		{
			if(MODULE_ID_AU3_3052 == module_type) 
			{	
				ret |= nam_set_ethport_enable(devNum,portNum,0);
				ret |= nam_set_bcm_ethport_autoneg(devNum,portNum,attr);
				ret |= nam_set_ethport_enable(devNum,portNum,1);
				if(0 != ret) 
				{
					syslog_ax_eth_port_err("set port autoNego status failed in 3052\n");
					return ETHPORT_RETURN_CODE_ERR_HW;
				}
				else 
				{
					ret = ETHPORT_RETURN_CODE_ERR_NONE;
				}
			}
			else
			{
				ret |= nam_set_ethport_force_linkdown( devNum, portNum, 1 );
				ret |= nam_set_ethport_duplex_autoneg(devNum,portNum,attr);
				pauseAdvertise = attr ? 1 : 0;
				ret |= nam_set_ethport_fc_autoneg(devNum,portNum,attr,pauseAdvertise);	
				ret |= nam_set_ethport_speed_autoneg(devNum,portNum,attr);
				ret |= nam_set_phy_ethport_autoneg(devNum,portNum,attr);
				ret |= nam_set_ethport_force_auto( devNum, portNum, 0 );	
				if(NPD_SUCCESS != ret) 
				{
					syslog_ax_eth_port_err("set port autoNego status failed\n");
					return ETHPORT_RETURN_CODE_ERR_HW;
				}
				else
				{
					ret = ETHPORT_RETURN_CODE_ERR_NONE;
				}
			}
		}
		
		else if (ETH_ATTR_DISABLE == portState) 
		{
			return ETHPORT_RETURN_CODE_ERR_OPERATE;
		}
	}	
	else if(NPD_TRUE == isBoard) 
	{
		syslog_ax_eth_port_dbg("config board %d autoNego %d\n ",eth_g_index,status);

		/*ret |= nbm_set_ethport_duplex_autoneg(eth_g_index,status);*/
		/*ret |= nbm_set_ethport_fc_autoneg(eth_g_index,status,0);*/
		/*ret |= nbm_set_ethport_speed_autoneg(eth_g_index,status);*/
		ret = nbm_set_autoneg(eth_g_index, status);
	}

	/* get main control port globe_index */
	if((PRODUCT_ID_AX5K_I == PRODUCT_ID) && (eth_g_index <= 3)) /* code optimize:Unsigned compared against 0 houxx@autelan.com  2013-1-17 */
	{
		eth_g_index = eth_g_index + ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9);
	}

	/* save auto-negotiation enable or disable flag in SW*/
	ethPortInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == ethPortInfo) 
	{
		syslog_ax_eth_port_err("port %d find null info when set auto-negotiation %s\n",	eth_g_index, status ? "enable" :"disable");
		return ETHPORT_RETURN_CODE_ERR_GENERAL;
	}
	attr_bitmap = ethPortInfo->attr_bitmap;
	if(status) 
	{ /* enable*/
		attr_bitmap |= (ETH_ATTR_ON << ETH_AUTONEG_CTRL_BIT);
	}
	else 
	{ /*disable*/
		attr_bitmap &= ~(ETH_ATTR_ON << ETH_AUTONEG_CTRL_BIT);
	}

	ethPortInfo->attr_bitmap = attr_bitmap;
	
	return ret;
}

/**********************************************************************************
 *  npd_set_port_autoNego_duplex
 *
 *	DESCRIPTION:
 * 		set ethernet port auto-negotiation duplex:enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		status -autoNego duplex enable or disable
 *		isBoard - port is on main board or not( 1 - main board port, 0 - slave board port)
 *	
 * RETURN:
 *		NPD_FALI
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_autoNego_duplex
(
	unsigned int eth_g_index,
	unsigned int status,
	unsigned int isBoard
)
{
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	unsigned long attr = status;
	unsigned long portState = 0;	
	int ret = ETHPORT_RETURN_CODE_ERR_NONE;
	unsigned int speed = 0;
	int linkState = 0;

	if(NPD_FALSE == isBoard) {
		ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
		if(0 != ret)
			return ETHPORT_RETURN_CODE_ERR_GENERAL;

		if((ret = nam_get_port_speed(devNum,portNum,&speed)) != 0) {
			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
		#if 0
		else if(0 != nam_get_port_link_state(devNum,portNum,&linkState)) {
			ret = NPD_DBUS_ERROR;
		}
		else if(((PORT_SPEED_1000_E != speed) && (PORT_SPEED_1000_E > speed)) || 	\
			(0 == linkState))
		#endif
		nam_get_port_en_dis(devNum,portNum,&portState);
		if(ETH_ATTR_ENABLE == portState) {		
			ret = nam_set_ethport_force_linkdown(devNum, portNum, 1);
			ret = nam_set_ethport_duplex_autoneg(devNum,portNum,attr);
			if(ETHPORT_RETURN_CODE_NOT_SUPPORT == ret) {
				return ret;
			}
			ret = nam_set_ethport_force_auto(devNum, portNum, 0);
			if(NPD_FAIL == ret) {
				syslog_ax_eth_port_err("set port duplex autoNego status failed\n");
				ret = ETHPORT_RETURN_CODE_ERR_HW;
			}
			else {
				ret = ETHPORT_RETURN_CODE_ERR_NONE;
			}
		}	
		else if (ETH_ATTR_DISABLE== portState) {
              return ETHPORT_RETURN_CODE_ERR_OPERATE;
		}

		return ret;
	
	}
	else if(NPD_TRUE == isBoard) {
		syslog_ax_eth_port_dbg("config board %d autoNego duplex %d\n ",eth_g_index,status);
		ret = nbm_set_ethport_duplex_autoneg(eth_g_index,status);
		if(6 == ret) {
			ret = ETHPORT_RETURN_CODE_UNSUPPORT;
		}
		else {
			ret = ETHPORT_RETURN_CODE_ERR_NONE;
		}
	}

	return ret;
}

/**********************************************************************************
 *  npd_set_port_autoNego_speed
 *
 *	DESCRIPTION:
 * 		set ethernet port auto-negotiation speed:enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		status -autoNego speed enable or disable
 *		isBoard - port is on main board or not( 1 - main board port, 0 - slave board port)
 *	
 * RETURN:
 *		NPD_FALI
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_autoNego_speed
(
	unsigned int eth_g_index,
	unsigned int status,
	unsigned int isBoard
)
{
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	unsigned long attr = status;
	unsigned long portState = 0;	
	int ret = ETHPORT_RETURN_CODE_ERR_NONE;

	if(NPD_FALSE == isBoard) {
		ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
		if(0 != ret)
			return ETHPORT_RETURN_CODE_ERR_GENERAL;

		#if 0
		if((ret = nam_get_port_speed(devNum,portNum,&speed)) != 0) {
			ret  =NPD_DBUS_ERROR;
		}
		else if(0 != nam_get_port_link_state(devNum,portNum,&linkState)) {
			ret = NPD_DBUS_ERROR;
		}
		else if(((PORT_SPEED_1000_E != speed) && (PORT_SPEED_1000_E > speed)) || 	\
			(0 == linkState))
		#endif
		nam_get_port_en_dis(devNum,portNum,&portState);
		if(ETH_ATTR_ENABLE == portState) {
			ret = nam_set_ethport_force_linkdown(devNum, portNum,1);
			ret = nam_set_ethport_speed_autoneg(devNum,portNum,attr);
			if(ETHPORT_RETURN_CODE_NOT_SUPPORT == ret) {
				return ret;
			}
			ret = nam_set_phy_ethport_autoneg(devNum,portNum,attr);
			ret = nam_set_ethport_force_auto(devNum, portNum,0);
			if(NPD_FAIL == ret) {
				syslog_ax_eth_port_err("set port speed autoNego status failed\n");
				ret = ETHPORT_RETURN_CODE_ERR_HW;
			}
			else {
				ret = ETHPORT_RETURN_CODE_ERR_NONE;
			}
		}
		else if (ETH_ATTR_DISABLE== portState) {
             return ETHPORT_RETURN_CODE_ERR_OPERATE;
		}
		return ret;
	}
	else if(NPD_TRUE == isBoard) {
		syslog_ax_eth_port_dbg("config board %d autoNego speed %d\n ",eth_g_index,status);
		ret = nbm_set_ethport_speed_autoneg(eth_g_index,status);
		if(6 == ret) {
			ret = ETHPORT_RETURN_CODE_UNSUPPORT;
		}		
		else {
			ret = ETHPORT_RETURN_CODE_ERR_NONE;
		}	
	}
		
	return ret;
}

/**********************************************************************************
 *  npd_set_port_autoNego_flowctrl
 *
 *	DESCRIPTION:
 * 		set ethernet port auto-negotiation flowcontrol:enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		status -autoNego speed enable or disable
 *		isBoard - port is on main board or not( 1 - main board port, 0 - slave board port)
 *	
 * RETURN:
 *		NPD_FALI
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_autoNego_flowctrl
(
	unsigned int eth_g_index,
	unsigned int status,
	unsigned int isBoard
)
{
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	unsigned long attr = status;
	unsigned long portState = 0;	
	unsigned int pauseAdvertise = 1;
	int ret = 0;
	
	struct product_sysinfo_s *p_sysinfo;
	FC_ETHERADDR  base_mac;

	if(NPD_FALSE == isBoard) 
	{
		ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
		if(0 != ret)
			return ETHPORT_RETURN_CODE_ERR_GENERAL;
		#if 0
		if((ret = nam_get_port_speed(devNum,portNum,&speed)) != 0) {
			ret  =NPD_DBUS_ERROR;
		}
		else if(0 != nam_get_port_link_state(devNum,portNum,&linkState)) {
			ret = NPD_DBUS_ERROR;
		}
		else if(((PORT_SPEED_1000_E != speed) && (PORT_SPEED_1000_E > speed)) || 	\
			(0 == linkState)) {
		#endif

		p_sysinfo = npd_query_sysinfo();
		memset(&base_mac, 0, sizeof(FC_ETHERADDR));
		ret = parse_mac_addr(p_sysinfo->basemac, &base_mac);
		if(NPD_SUCCESS != ret)	/* code optimize: Unchecked return value   houxx@autelan.com  2013-1-17 */
		{
			return ret;
		}
		nam_set_fc_autoneg_sa(devNum,portNum,&base_mac);
		nam_get_port_en_dis(devNum,portNum,&portState);
		if(ETH_ATTR_ENABLE == portState) 
		{		
			ret = nam_set_ethport_force_linkdown(devNum, portNum,1);
			pauseAdvertise = attr ? 1 : 0;
			ret = nam_set_ethport_fc_autoneg(devNum,portNum,attr,pauseAdvertise);
			if(ETHPORT_RETURN_CODE_NOT_SUPPORT == ret) 
			{
				return ret;
			}
			ret = nam_set_ethport_force_auto(devNum, portNum,0);
			if(NPD_FAIL == ret) 
			{
				syslog_ax_eth_port_err("set port fc autoNego status failed\n");
				ret = ETHPORT_RETURN_CODE_ERR_HW;
			}
			else 
			{
				ret = ETHPORT_RETURN_CODE_ERR_NONE;
			}
			return ret;
		}
		else if (ETH_ATTR_DISABLE == portState) 
		{
            return ETHPORT_RETURN_CODE_ERR_OPERATE;
		}
	}
	else if(NPD_TRUE == isBoard) 
	{
		syslog_ax_eth_port_dbg("config board %d autoNego flow control %d\n ",eth_g_index,status);
		ret = nbm_set_ethport_fc_autoneg(eth_g_index,status,0);
		if(6 == ret) 
		{
			ret = ETHPORT_RETURN_CODE_UNSUPPORT;
		}
		else 
		{
			ret = ETHPORT_RETURN_CODE_ERR_NONE;
		}
	}
	return ret;
}

	
/**********************************************************************************
 *	npd_get_port_attrmap_by_gindex
 *
 *	DESCRIPTION:
 *		get ethernet port attribute bit map
 *
 *	INPUT:
 *		eth_g_index - ethernet global index 	
 *	
 *	OUTPUT:
 *		local_port_attrmap - port attribute bit map
 *
 *	RETURN:
 *		ret
 *		
 *
 **********************************************************************************/
unsigned int 
npd_get_port_attrmap_by_gindex
(
	unsigned int eth_g_index,
	unsigned int *isWAN,
	unsigned int *local_port_attrmap
)
{
	unsigned long last_link_change = 0;	
	unsigned char local_port_type = 0;
	unsigned char slotno = 0, local_port_no = 0;
	unsigned int  slotIndex = 0,portIndex = 0;
	unsigned int ret = 0;
	struct eth_port_s portInfo ;
	enum module_id_e module_type = MODULE_ID_NONE;
	
#define SYSFS_PATH_MAX 256
    FILE *f = NULL;
    char path[SYSFS_PATH_MAX] = {0};

	
	slotIndex = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	slotno = CHASSIS_SLOT_INDEX2NO(slotIndex);
	portIndex = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	local_port_no = ETH_LOCAL_INDEX2NO(slotIndex, portIndex);
	last_link_change  = ETH_PORT_LINK_TIME_CHANGE_FROM_LOCAL_INDEX(slotIndex,slotIndex);
	memset(&portInfo, 0, sizeof(struct eth_port_s));
	module_type = MODULE_TYPE_ON_SLOT_INDEX(slotIndex);
	
	syslog_ax_eth_port_dbg("npd_get_port_attrmap_by_gindex  eth_g_index %d, slotIndex %d, slotno %d, portIndex %d, local_port_no %d\n", 
								eth_g_index, slotIndex, slotno, portIndex, local_port_no);
	if(SYSTEM_TYPE == IS_DISTRIBUTED){
		ret = nam_read_eth_port_info(module_type,slotno,local_port_no,&portInfo);
	}
    else{
	if(npd_eslot_check_subcard_module(module_type) && \
		((AX51_GTX == npd_check_subcard_type(PRODUCT_ID, local_port_no)) || \
		 (AX51_SFP == npd_check_subcard_type(PRODUCT_ID, local_port_no)))) {
		ret = nbm_read_eth_port_info(portIndex,&portInfo);
		last_link_change = nbm_get_link_timestamp(portIndex);
		if(NPD_SUCCESS != ret) {
			syslog_ax_eth_port_err("read subcard port(%d %d) infomation error.\n",slotIndex,portIndex);
		}
		local_port_type = portInfo.port_type;
		*isWAN = 1;
	}
	else if(!PRODUCT_IS_BOX && (AX7_CRSMU_SLOT_NUM == slotIndex)) { /* read CRSMU RGMII ethernet port info*/
		/* TODO: read RGMII ethernet port info from cavium-ethernet driver*/
		ret = nbm_read_eth_port_info(portIndex,&portInfo);
		last_link_change = nbm_get_link_timestamp(portIndex);
		
		if(NPD_SUCCESS != ret) {
			syslog_ax_eth_port_err("read AX7 port(%d %d) infomation error.\n",slotIndex,portIndex);
		}		
		local_port_type = portInfo.port_type;
		*isWAN = 1;
	}
	else if((MODULE_ID_AX5_5612I == module_type) && 
		(local_port_no >= 9) && (local_port_no <= 12)) {				
		syslog_ax_eth_port_dbg("get port(%d %d) information from nbm\n",slotno,local_port_no);
		local_port_no = local_port_no - 9;/*portNum should be 0-3 for nbm*/
		ret = nbm_read_eth_port_info(local_port_no,&portInfo);
		last_link_change = nbm_get_link_timestamp(local_port_no);
	
		if(NPD_SUCCESS != ret) {
			syslog_ax_eth_port_err("read AX5 port(%d %d) infomation error.\n",slotIndex,local_port_no + 9);
		}	
		local_port_type = portInfo.port_type;	
		local_port_no = local_port_no + 9;/*portNum turn back to dcli*/
		*isWAN = 1;
	}			
	else if(MODULE_ID_AX5_5612E== module_type  || MODULE_ID_AX5_5608 == module_type){
syslog_ax_eth_port_err("read 5612E port(%d %d) infomation dbg.\n",slotIndex,portIndex);
		ret = nbm_read_eth_port_info(portIndex,&portInfo);
		last_link_change = nbm_get_link_timestamp(portIndex);
		
		if(NPD_SUCCESS != ret) {
			syslog_ax_eth_port_err("read 5612E port(%d %d) infomation error.\n",slotIndex,portIndex);
		}		
		local_port_type = portInfo.port_type;
		*isWAN = 1;
	}
	else { /* read GE or XG ports info from asic*/
		syslog_ax_eth_port_dbg("get eth port %d/%d info\n",slotIndex,portIndex);
	
		if(MODULE_ID_SUBCARD_TYPE_B == module_type) {
			ret = npd_check_subcard_type(PRODUCT_ID_AU4K, local_port_no);
			if(NPD_FALSE == ret) {
				syslog_ax_eth_port_dbg("4626 port(%d,%d) is not at its slot\n",slotIndex,portIndex);
				local_port_type = ETH_MAX;
			}
		}
		else {
			ret = nam_read_eth_port_info(module_type,slotno,local_port_no,&portInfo);
    		}
		}
	}
	if (ret != 0) {
		syslog_ax_eth_port_dbg("port %d/%d attr get FAIL. \n",slotIndex,local_port_no);
	}
	else {
		/*add for check interface is in br*/
		if (1 == *isWAN) {
			snprintf(path, SYSFS_PATH_MAX, "/sys/class/net/eth%d-%d/brport/bridge/ifindex", slotno, local_port_no);
			f = fopen(path, "r");
			if (f) {
				fclose(f);
			}
			else{
				syslog_ax_eth_port_dbg("/sys/class/net/eth%d-%d not in br \n",slotno, local_port_no);
				return NPD_FAIL;
			}
		}
		/*local_port_type		= portInfo.port_type;*/
		*local_port_attrmap	= portInfo.attr_bitmap;
		syslog_ax_eth_port_dbg("port(%d %d) bitmap is %x \n",slotno, local_port_no, *local_port_attrmap);
	}

	return ret;
}

/**********************************************************************************
 *  npd_get_port_duplex_mode
 *
 *	DESCRIPTION:
 * 		get ethernet port duplex status:half or full
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *	
 *	OUTPUT:
 *		mode - duplex half or duplex full
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_get_port_duplex_mode
(
	unsigned int eth_g_index,
	unsigned int *mode
)
{
	unsigned char devNum;
	unsigned char portNum;
	unsigned int attr = 0;
	int ret = 0;
	
	ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
	if(NPD_SUCCESS != ret)	/* code optimize: Unchecked return value   houxx@autelan.com  2013-1-17 */
	{
		return;
	}
	nam_get_port_duplex_mode(devNum,portNum,&attr);
	if(ETH_ATTR_DUPLEX_FULL == attr) {
		*mode = ETH_ATTR_DUPLEX_FULL;
	}
	else if(ETH_ATTR_DUPLEX_HALF == attr) {
		*mode = ETH_ATTR_DUPLEX_HALF;
	}

	return;
}

/**********************************************************************************
 *  npd_set_port_duplex_mode
 *
 *	DESCRIPTION:
 * 		set ethernet port duplex status:half or full
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		mode - full or half
 *		isBoard - port is on main board or not( 1 - main board port, 0 - slave board port)
 *
 * RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_duplex_mode
(
	unsigned int eth_g_index,
	unsigned int mode,
	unsigned int isBoard
)
{
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	unsigned int attr = mode;
	unsigned int slot_index = 0;
	int ret = 0;
	unsigned long anduplex = 0;
	unsigned int speed = 0;
	unsigned long flowControl = 0, anDone = 0, backpressure = 0;
	enum module_id_e module_type;

	slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	module_type = MODULE_TYPE_ON_SLOT_INDEX(slot_index);

	if(NPD_FALSE == isBoard) {
		ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
		if(0 != ret)
			return ETHPORT_RETURN_CODE_ERR_GENERAL;
		
		ret = nam_get_ethport_duplex_autoneg(devNum,portNum,&anduplex);
		if(0 != ret) {
			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
		ret = nam_get_port_autoneg_state(devNum,portNum,&anDone);
		if(0 != ret) {
			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
		ret = nam_get_port_speed(devNum,portNum,&speed);
		if(0 != ret) {
			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}		
		ret = nam_get_port_flowCtrl_state(devNum,portNum,&flowControl);
		if(0 != ret) {
			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
		ret = nam_get_port_backPres_state(devNum,portNum,&backpressure);
		if(0 != ret) {
			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
		if(MODULE_ID_AU3_3052 == module_type) {
			syslog_ax_eth_port_dbg("set 3052 duplex mode\n");
			if((0 == anDone) && (PORT_SPEED_1000_E != speed) && (PORT_SPEED_10000_E > speed)) {
				attr = mode ? PORT_FULL_DUPLEX_E : PORT_HALF_DUPLEX_E;
				if((PORT_HALF_DUPLEX_E == attr) && (ETH_ATTR_FC_ENABLE == flowControl)) {
					ret = ETHPORT_RETURN_CODE_ERROR_DUPLEX_HALF;
					return ret;
				}
				else if((PORT_FULL_DUPLEX_E == attr) && (ETH_ATTR_BP_ENABLE == backpressure)) {
					ret = ETHPORT_RETURN_CODE_ERROR_DUPLEX_FULL;
					return ret;
				}
				ret = nam_set_ethport_duplex_mode(devNum,portNum,attr);
				if(NPD_FAIL == ret) {
					ret = ETHPORT_RETURN_CODE_ERR_HW;
					syslog_ax_eth_port_err("set port duplex mode failed \n");
				}
				else {
					ret = ETHPORT_RETURN_CODE_ERR_NONE;
				}
			}
			else {
				syslog_ax_eth_port_dbg("set port duplex mode %s,anduplex %d ,speed %d\n",attr? "Full":"Half",anduplex,speed);
				ret = ETHPORT_RETURN_CODE_DUPLEX_NODE;
			}			
		}
		else {
			if((0 == anduplex) && (PORT_SPEED_1000_E != speed) && (PORT_SPEED_10000_E > speed)) {
				attr = mode ? PORT_FULL_DUPLEX_E : PORT_HALF_DUPLEX_E;
				syslog_ax_eth_port_dbg("set port duplex mode %s,anduplex %d ,speed %d",attr? "Full":"Half",anduplex,speed);
				if((PORT_HALF_DUPLEX_E == attr) && (ETH_ATTR_FC_ENABLE == flowControl)) {
					ret = ETHPORT_RETURN_CODE_ERROR_DUPLEX_HALF;
					return ret;
				}
				else if((PORT_FULL_DUPLEX_E == attr) && (ETH_ATTR_BP_ENABLE == backpressure)) {
					ret = ETHPORT_RETURN_CODE_ERROR_DUPLEX_FULL;
					return ret;
				}
				ret = nam_set_ethport_duplex_mode(devNum,portNum,attr);
				if(NPD_FAIL == ret) {
					ret = ETHPORT_RETURN_CODE_ERR_HW;
					syslog_ax_eth_port_err("set port duplex mode failed \n");
				}
				else {
					ret = ETHPORT_RETURN_CODE_ERR_NONE;
				}
			}
			else {
				syslog_ax_eth_port_dbg("set port duplex mode %s,anduplex %d ,speed %d\n",attr? "Full":"Half",anduplex,speed);
				ret = ETHPORT_RETURN_CODE_DUPLEX_NODE;
			}
		}
	}
	else if(NPD_TRUE == isBoard) {
		syslog_ax_eth_port_err("config board %d duplex %d\n ",eth_g_index,attr);
		attr = mode ? PORT_FULL_DUPLEX_E : PORT_HALF_DUPLEX_E;
		ret = nbm_set_ethport_duplex_mode(eth_g_index, attr);
	}

	return ret;
}

/**********************************************************************************
 *  npd_get_port_speed
 *
 *	DESCRIPTION:
 * 		get ethernet port speed :10M ,100M or 1000M etc.
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *	
 *	OUTPUT:
 *		speed - port speed 10M or 100M or 1000M
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_get_port_speed
(
	unsigned int eth_g_index,
	unsigned int *speed
)
{
	unsigned char devNum;
	unsigned char portNum;
	unsigned int ret = 0;
	unsigned int attr = 0;
	
	npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
	ret = nam_get_port_speed(devNum,portNum,&attr);
	if(ret != 0) {
		*speed = 0;
	}
	else {
		*speed = attr;
	}

	return;
}

/**********************************************************************************
 *  npd_set_port_speed
 *
 *	DESCRIPTION:
 * 		set ethernet port speed :10M ,100M or 1000M etc.
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		speed 10M,100M,1000M
 *		isBoard - port is on main board or not( 1 - main board port, 0 - slave board port)
 *
 * RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_speed
(
	unsigned int eth_g_index,
	unsigned int speed,
	unsigned int isBoard
)
{
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	int ret = ETHPORT_RETURN_CODE_ERR_NONE;
	unsigned int mode = 0;
	unsigned int slot_index = 0;
	struct eth_port_s *ethPort = NULL;

	unsigned long anspeed = 0, anDone = 0;
	unsigned int attr = PORT_SPEED_1000_E;

	enum module_id_e module_type;

	slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	module_type = MODULE_TYPE_ON_SLOT_INDEX(slot_index);	

	if( speed == 1000)
		attr = PORT_SPEED_1000_E;
	else if(speed == 100)
		attr = PORT_SPEED_100_E;
	else if(speed ==10)
		attr = PORT_SPEED_10_E;

	if(NPD_FALSE == isBoard) {

		/*check the port is trunk member or NOT*/
		ethPort = npd_get_port_by_index(eth_g_index);
		if(NULL == ethPort){
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else if(NULL != ethPort->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION]){
			/*port is member of trunk*/
			return VLAN_RETURN_CODE_PORT_TRUNK_MBR;
		}	
			
		ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
		if(0 != ret)
			return ETHPORT_RETURN_CODE_ERR_GENERAL;
		ret = nam_get_port_autoneg_state(devNum,portNum,&anDone);
		if(0 != ret) {
			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}		
		
		ret = nam_get_ethport_speed_autoneg(devNum,portNum,&anspeed);
		if(0 != ret) {
			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
		ret = nam_get_port_duplex_mode(devNum,portNum,&mode);
        if(ret != 0) {
			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
		if(MODULE_ID_AU3_3052 == module_type) {
			syslog_ax_eth_port_dbg("set 3052 speed mode\n");
	        if ((0 == anDone) && (PORT_HALF_DUPLEX_E == mode) && (PORT_SPEED_1000_E == attr)) {
				syslog_ax_eth_port_err("set port speed failed \n");
				ret = ETHPORT_RETURN_CODE_DUPLEX_MODE;  
	        }
			if((0 == anDone)) {
				ret = nam_set_ethport_speed(devNum,portNum,attr);
				if(NPD_FAIL == ret) {
					syslog_ax_eth_port_err("set port speed failed \n");
					ret = ETHPORT_RETURN_CODE_ERR_HW;
				}
				else {
					ret = ETHPORT_RETURN_CODE_ERR_NONE;
				}
			}
			else {
				syslog_ax_eth_port_dbg("%s ::auto neg speed %d\n",__func__,anspeed);
				ret = ETHPORT_RETURN_CODE_SPEED_NODE;
			}			
		}
		else {
	        if ((0 == anspeed) && (PORT_HALF_DUPLEX_E == mode) && (PORT_SPEED_1000_E == attr)) {
				syslog_ax_eth_port_err("set port speed failed \n");
				ret = ETHPORT_RETURN_CODE_DUPLEX_MODE;  
	        }
			if((0 == anspeed)) {
				ret = nam_set_ethport_speed(devNum,portNum,attr);
				if(NPD_FAIL == ret) {
					syslog_ax_eth_port_err("set port speed failed \n");
					ret = ETHPORT_RETURN_CODE_ERR_HW;
				}
				else {
					ret = ETHPORT_RETURN_CODE_ERR_NONE;
				}
			}
			else {
				syslog_ax_eth_port_dbg("%s ::auto neg speed %d\n",__func__,anspeed);
				ret = ETHPORT_RETURN_CODE_SPEED_NODE;
			}
		}
	}
	else if(NPD_TRUE == isBoard) {
		syslog_ax_eth_port_dbg("config board %d speed %d\n ",eth_g_index,attr);
		ret = nbm_set_ethport_speed(eth_g_index,attr);
	}

	return ret;
}

/**********************************************************************************
 *  npd_get_port_flowCtrl_state
 *
 *	DESCRIPTION:
 * 		get ethernet port flow control status: enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *	
 *	OUTPUT:
 *		status  - flow control is enable or disable
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_get_port_flowCtrl_state
(
	unsigned int eth_g_index,
	unsigned int *status
)
{
	unsigned char devNum;
	unsigned char portNum;
	unsigned long attr = 0;
	
	npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
	nam_get_port_flowCtrl_state(devNum,portNum,&attr);
	if(ETH_ATTR_FC_ENABLE == attr) {
		*status = ETH_ATTR_FC_ENABLE;
	}
	else if(ETH_ATTR_FC_DISABLE == attr) {
		*status = ETH_ATTR_FC_DISABLE;
	}

	return;
}

/**********************************************************************************
 *  npd_set_port_flowCtrl_state
 *
 *	DESCRIPTION:
 * 		set ethernet port flow control status: enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		status - enable or disable
 *		isBoard - port is on main board or not( 1 - main board port, 0 - slave board port)
 *
 * RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_flowCtrl_state
(
	unsigned int eth_g_index,
	unsigned int status,
	unsigned int isBoard
)
{
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	unsigned long attr = status;
	int ret = ETHPORT_RETURN_CODE_ERR_NONE;
	unsigned int curduplex = 0;
	unsigned int portLinkState = 0;
	unsigned long anfc = 0, anDone = 0;
	unsigned int slot_index = 0;
	struct eth_port_s *ethPort = NULL;

	enum module_id_e module_type;

	slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	module_type = MODULE_TYPE_ON_SLOT_INDEX(slot_index);

	if(NPD_FALSE == isBoard) {
		/*check the port is trunk member or NOT*/
		ethPort = npd_get_port_by_index(eth_g_index);
		if(NULL == ethPort){
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else if(NULL != ethPort->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION]){
			/*port is member of trunk*/
			return VLAN_RETURN_CODE_PORT_TRUNK_MBR;
		}
		
		ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
		if(0 != ret)
			return ETHPORT_RETURN_CODE_ERR_GENERAL;
		
		ret = npd_get_port_link_status(eth_g_index,&portLinkState);
		if(0 != ret)
			return ETHPORT_RETURN_CODE_ERR_GENERAL;	
	
		ret = nam_get_port_duplex_mode(devNum,portNum,&curduplex);
		if(0 != ret) {
			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
		ret = nam_get_port_autoneg_state(devNum,portNum,&anDone);
		if(0 != ret) {
			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}		
		ret = nam_get_ethport_fc_autoneg(devNum,portNum,&anfc);
		if(0 != ret) {	
			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
		if(MODULE_ID_AU3_3052 == module_type) {
			syslog_ax_eth_port_dbg("set 3052 flow control mode\n");
			if(((ETH_ATTR_DUPLEX_FULL == curduplex) && (0 == anDone)) || (ETH_ATTR_LINKDOWN == portLinkState)){
				ret = nam_set_ethport_flowCtrl(devNum,portNum,attr);
			}
			else if((ETH_ATTR_DUPLEX_HALF == curduplex) || (1 == anDone)) {
				syslog_ax_eth_port_dbg("%s ::curduplex %d, auto neg %d\n",__func__,curduplex,anDone);
				ret = ETHPORT_RETURN_CODE_FLOWCTL_NODE;
			}
			else 
				ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
		else {
			if(((ETH_ATTR_DUPLEX_FULL == curduplex) && (0 == anfc)) || (ETH_ATTR_LINKDOWN == portLinkState)){
				ret = nam_set_ethport_flowCtrl(devNum,portNum,attr);
			}
			else if((ETH_ATTR_DUPLEX_HALF == curduplex) || (1 == anfc)) {
				syslog_ax_eth_port_dbg("%s ::curduplex %d, auto neg %d\n",__func__,curduplex,anfc);
				ret = ETHPORT_RETURN_CODE_FLOWCTL_NODE;
			}
			else 
				ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
	}
	else if(NPD_TRUE == isBoard) {
		syslog_ax_eth_port_dbg("config board %d flow control %d\n ",eth_g_index,attr);
		ret = nbm_set_ethport_flowCtrl(eth_g_index, attr);
	}

	return ret;
}
 
/**********************************************************************************
 *  npd_get_port_backPressure_state
 *
 *	DESCRIPTION:
 * 		get ethernet port back pressure status: enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *	
 *	OUTPUT:
 *		status  - back pressure is enable or disable
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_get_port_backPressure_state
(
	unsigned int eth_g_index,
	unsigned int *status
)
{
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	unsigned long attr = 0;
	
	npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
	nam_get_port_backPres_state(devNum,portNum,&attr);
	if(ETH_ATTR_BP_ENABLE == attr) {
		*status = ETH_ATTR_BP_ENABLE;
	}
	else if(ETH_ATTR_BP_DISABLE == attr) {
		*status = ETH_ATTR_BP_DISABLE;
	}

	return;
}

/**********************************************************************************
 *  npd_set_port_backPressure_state
 *
 *	DESCRIPTION:
 * 		set ethernet port back pressure status: enable or disable
 *			
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		status - enable or disable
 *		isBoard - port is on main board or not( 1 - main board port, 0 - slave board port)
 *
 * RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_backPressure_state
(
	unsigned int eth_g_index,
	unsigned int status,
	unsigned int isBoard
)
{
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	unsigned long attr = status;
	int ret = ETHPORT_RETURN_CODE_ERR_NONE;
	unsigned int duplex = 0;

	if(NPD_FALSE == isBoard) {
		ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
		if(0 != ret)
			return ETHPORT_RETURN_CODE_ERR_GENERAL;

		ret = nam_get_port_duplex_mode(devNum,portNum,&duplex);
		if(0 != ret) {
			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
		else if(ETH_ATTR_DUPLEX_HALF == duplex) {
			ret = nam_set_ethport_backPres(devNum,portNum,attr);
			if(0 != ret) {
				ret = ETHPORT_RETURN_CODE_ERR_HW;
			}
			else {
				ret = ETHPORT_RETURN_CODE_ERR_NONE;
			}
		}
		else {
			ret = ETHPORT_RETURN_CODE_BACKPRE_NODE;
			syslog_ax_eth_port_dbg("%s ::duplex  %d\n",__func__,duplex);
		}
	}
	else if(NPD_TRUE == isBoard) {
		syslog_ax_eth_port_dbg("config board %d back pressure %d\n ",eth_g_index,attr);
		ret = nbm_set_ethport_backPres(eth_g_index,attr);
	}

	return ret;

}

/**********************************************************************************
 *  npd_get_port_mru
 *
 *	DESCRIPTION:
 * 		get ethernet port MRU
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *	
 *	OUTPUT:
 *		mru  - Maximum Receive Unit value
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_get_port_mru
(
	unsigned int eth_g_index,
	unsigned int *mru
)
{
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	unsigned int ret = 0;
	unsigned int attr = 0;
	
	npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
	ret = nam_get_port_mru(devNum,portNum,&attr);
	if(ret != 0) {
		*mru = 0;
	}
	else {
		*mru = attr;
	}

	return;
}

/**********************************************************************************
 *	npd_eth_port_config_stp
 *		set ethernet port Spanning-Tree state
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		state - enable or disable Spanning-Tree protocol on port
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_FAIL - if ethernet port is not exists or
 *				   memory allocation failed
 *		NPD_SUCCESS - if no error occurred
 *		
 *
 **********************************************************************************/
int npd_eth_port_config_stp
(
	unsigned short	eth_g_index,
	enum stp_running_mode mode,
	unsigned int	state
)
{
	struct eth_port_s *portInfo = NULL;
	struct stp_info_s *stpInfo = NULL;
	int i = 0;

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		return NPD_FAIL;
	}

	stpInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_DOT1W];
	if(NULL == stpInfo) { /* STP state not configured before*/
		stpInfo = (struct stp_info_s *)malloc(sizeof(struct stp_info_s));
		if(NULL == stpInfo) {
			syslog_ax_eth_port_err("npd eth port %#x config stp error memory alloc",eth_g_index);
			return NPD_FAIL;
		}
		memset(stpInfo ,0 ,sizeof(struct stp_info_s));
	}

	stpInfo->stpEnable = (state ? 1:0);
	for(i = 0; i < MAX_MST_ID; i++) {
		stpInfo->prio[i] = DEF_PORT_PRIO;
		stpInfo->pathcost[i] = ADMIN_PORT_PATH_COST_AUTO;
	}
	/*memset(stpInfo->prio, DEF_PORT_PRIO ,MAX_MST_ID);*/
	/*memset(stpInfo->pathcost, ADMIN_PORT_PATH_COST_AUTO ,MAX_MST_ID);*/
	stpInfo->edge = DEF_ADMIN_EDGE;
	stpInfo->p2p = DEF_P2P;
	stpInfo->nonstp = DEF_ADMIN_NON_STP;
	
	stpInfo->npd_stp_port_notifier_func = (NPD_ETH_PORT_LINK_CHANGE_NOTIFIER_FUNC)npd_rstp_link_change;
	if(state){
		portInfo->funcs.funcs_run_bitmap |= (1<<ETH_PORT_FUNC_DOT1W);
	}
	else{
		portInfo->funcs.funcs_run_bitmap &= ~(1<<ETH_PORT_FUNC_DOT1W);
	}
	portInfo->funcs.func_data[ETH_PORT_FUNC_DOT1W] = stpInfo;
	
	return NPD_SUCCESS;
}

/**********************************************************************************
 *  npd_set_port_mru
 *
 *	DESCRIPTION:
 * 		set ethernet port MRU
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		mru <64-8912>
 *		isBoard - port is on main board or not( 1 - main board port, 0 - slave board port)
 *
 * RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_mru
(
	unsigned int eth_g_index,
	unsigned int mru,
	unsigned int isBoard
)
{
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	unsigned int attr = mru;
	int ret = ETHPORT_RETURN_CODE_ERR_NONE;

	if(NPD_FALSE == isBoard) {
		ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
		if(0 == ret) {
			ret = nam_set_ethport_mru(devNum,portNum,attr);
			if(0 != ret) {
				ret = ETHPORT_RETURN_CODE_ERR_HW;
			}
			else {
				ret = ETHPORT_RETURN_CODE_ERR_NONE;
			}			
		}
		else
			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
	}
	else if(NPD_TRUE == isBoard) {
		syslog_ax_eth_port_dbg("config board %d duplex %d\n ",eth_g_index,attr);
		ret = nbm_set_ethport_mru(eth_g_index, attr);
	}
	return ret;
}

/**********************************************************************************
 *  npd_set_port_attr_default
 *
 *	DESCRIPTION:
 * 		set ethernet port default value
 *
 *	INPUT:
 *		eth_g_index - ethernet global index
 *		isBoard - port is on main board or not( 1 - main board port, 0 - slave board port)
 *		moduleId - local module id
 *
 * RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_attr_default
(
	unsigned int eth_g_index,
	unsigned int isBoard,
	enum module_id_e moduleId
)
{
	struct eth_port_s *portInfo = NULL;
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	unsigned int  attr_bitmap = 0;
	int ret = ETHPORT_RETURN_CODE_ERR_NONE;
	unsigned int slot_index = 0, port_index = 0, portno = 0;
	
	slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	portno = ETH_LOCAL_INDEX2NO(slot_index, port_index);

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		return ETHPORT_RETURN_CODE_ERR_GENERAL;
	}
	
	if(MODULE_ID_AX5_5612I == moduleId &&  \
		eth_g_index >= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9) &&  \
		eth_g_index <= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,12)) {
		eth_g_index = eth_g_index - ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9);
		syslog_ax_eth_port_dbg("5612i port globe index %d is not at its slot\n",eth_g_index);
		/* change 5612i main control port module_id MODULE_ID_AX5_5612I to MODULE_ID_AX7_CRSMU */
		moduleId = MODULE_ID_AX7_CRSMU;
	}
	
	if(MODULE_ID_SUBCARD_TYPE_B == moduleId) {
		ret = npd_check_subcard_type(PRODUCT_ID_AU4K, portno);
		if(NPD_FALSE == ret) {
			syslog_ax_eth_port_dbg("4626 port globe index %d is not at its slot\n",eth_g_index);
			return ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
	}

	if(NPD_FALSE == isBoard) {
		ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
		if(0 != ret) 
			return ETHPORT_RETURN_CODE_ERR_GENERAL;
	
		/* admin status bitmap*/
		attr_bitmap |= (ethport_attr_default(moduleId)->admin_state << ETH_ADMIN_STATUS_BIT);
		/* port speed bitmap*/
		attr_bitmap |= (ethport_attr_default(moduleId)->speed<< ETH_SPEED_BIT);
		/* Auto-Nego bitmap*/
		attr_bitmap |= (ethport_attr_default(moduleId)->autoNego << ETH_AUTONEG_CTRL_BIT);
		/* duplex mode bitmap*/
		attr_bitmap |= (ethport_attr_default(moduleId)->duplex<< ETH_DUPLEX_BIT);
		/* flow-control bitmap*/
		attr_bitmap |= (ethport_attr_default(moduleId)->fc<< ETH_FLOWCTRL_BIT);
		/* back-pressure bitmap*/
		attr_bitmap |= (ethport_attr_default(moduleId)->bp<< ETH_BACKPRESSURE_BIT);	

		/* admin status*/
		ret = nam_set_ethport_enable(devNum,portNum,ethport_attr_default(moduleId)->admin_state);
		if(NPD_SUCCESS != ret) {
			syslog_ax_eth_port_dbg("set port(%d,%d) admin status error %d\n ",	\
					devNum,portNum,ret);
		}	
		/* flow-control */
		ret = nam_set_ethport_flowCtrl(devNum,portNum,ethport_attr_default(moduleId)->fc);
		if(NPD_SUCCESS != ret) {
			syslog_ax_eth_port_dbg("set port(%d,%d) flow control error %d\n ",devNum,portNum,ret);
		}
		/* duplex mode */
		ret = nam_set_ethport_duplex_mode(devNum,portNum,ethport_attr_default(moduleId)->duplex);
		if(NPD_SUCCESS != ret) {
			syslog_ax_eth_port_dbg("set port(%d,%d) duplex error %d\n ",devNum,portNum,ret);
		}		
		/* Auto-Nego (AN-FC,AN-speed,AN-duplex)*/
		ret = nam_set_ethport_force_linkdown( devNum, portNum, 1);
		if(NPD_SUCCESS!= ret){
          syslog_ax_eth_port_dbg("set port(%d,%d)force linkdown error\n" ,devNum,portNum);   
		}	
		ret = nam_set_ethport_duplex_autoneg(devNum,portNum,ethport_attr_default(moduleId)->duplex_an);
		if(NPD_SUCCESS != ret && ETHPORT_RETURN_CODE_NOT_SUPPORT != ret) {
			syslog_ax_eth_port_dbg("set port(%d,%d) duplex_an error %d\n ",devNum,portNum,ret);
		}		
		ret = nam_set_ethport_speed_autoneg(devNum,portNum,ethport_attr_default(moduleId)->speed_an);
		if(NPD_SUCCESS != ret && ETHPORT_RETURN_CODE_NOT_SUPPORT != ret) {
			syslog_ax_eth_port_dbg("set port(%d,%d) speed_an error %d\n ",devNum,portNum,ret);
		}		
		ret = nam_set_ethport_fc_autoneg(devNum,portNum,ethport_attr_default(moduleId)->fc_an,ethport_attr_default(moduleId)->fc_an);
		if(NPD_SUCCESS != ret && ETHPORT_RETURN_CODE_NOT_SUPPORT != ret) {
			syslog_ax_eth_port_dbg("set port(%d,%d) flow control auto-nego error %d\n ",devNum,portNum,ret);
		}			
		ret = nam_set_phy_ethport_autoneg(devNum,portNum,1);
		if(NPD_SUCCESS != ret){
          syslog_ax_eth_port_dbg("set phy(%d,%d)ethport autoneg error \n" ,devNum,portNum);   
		}
        ret = nam_set_ethport_force_auto( devNum, portNum,0);
		if(NPD_SUCCESS != ret){
          syslog_ax_eth_port_dbg("set port(%d,%d)force auto error\n" ,devNum,portNum);  
		}
		switch(moduleId){
			case MODULE_ID_AX7_6GTX :
			case MODULE_ID_AX7_6GTX_POE :
				/* back-pressure disable*/
				ret = nam_set_ethport_backPres(devNum,portNum,ethport_attr_default(moduleId)->bp);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set port(%d,%d) back pressure error %d\n ",	\
							devNum,portNum,ret);
				}
				/* port speed*/
				ret = nam_set_ethport_speed(devNum,portNum,ethport_attr_default(moduleId)->speed);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set port(%d,%d) speed error %d\n ",devNum,portNum,ret);
				}		
				break;
			case MODULE_ID_AX5_5612I :
				/* back-pressure disable*/
				ret = nam_set_ethport_backPres(devNum,portNum,ethport_attr_default(moduleId)->bp);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set port(%d,%d) back pressure error %d\n ",	\
							devNum,portNum,ret);
				}
				
				/* port speed*/
				ret = nam_set_ethport_speed(devNum,portNum,ethport_attr_default(moduleId)->speed);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set port(%d,%d) speed error %d\n ",devNum,portNum,ret);
				}	
				
				/* set Master XSMI interface AC TIMING for Access External Phy*/
				ret = nam_set_xsmi_ac_timing(devNum);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set XG port(%d,%d) AC timing error %d\n ",devNum,portNum,ret);
				}					
				break;
			case MODULE_ID_AX5_5612 :
			case MODULE_ID_AU4_4626 :
			case MODULE_ID_AU4_4524 :
			case MODULE_ID_AU4_4524_POE :
			case MODULE_ID_AU3_3524:				
				/* back-pressure disable*/
				ret = nam_set_ethport_backPres(devNum,portNum,ethport_attr_default(moduleId)->bp);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set port(%d,%d) back pressure error %d\n ",	\
							devNum,portNum,ret);
				}
				/* port speed*/
				ret = nam_set_ethport_speed(devNum,portNum,ethport_attr_default(moduleId)->speed);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set port(%d,%d) speed error %d\n ",devNum,portNum,ret);
				}	
				/* add 4 combo ports to poll array*/
				if(VALUE_IN_RANGE(portNum, 20, 23)) {
					npd_eth_port_poll_add_member(eth_g_index); 
				}
				#if 0
				//preferred media
				if(VALUE_IN_RANGE(portNum, 20, 23)) {
					attr_bitmap |= 1 << ETH_PREFERRED_FIBER_MEDIA_BIT;
					nam_set_eth_port_trans_media(devNum,portNum,COMBO_PHY_MEDIA_PREFER_FIBER);
				}
				#endif
				break;
			case MODULE_ID_AU3_3528:
			case MODULE_ID_AU2_2528:
			case MODULE_ID_AX71_2X12G12S:
				/* back-pressure disable*/
				ret = nam_set_ethport_backPres(devNum,portNum,ethport_attr_default(moduleId)->bp);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set port(%d,%d) back pressure error %d\n ",	\
							devNum,portNum,ret);
				}
				/* port speed*/
				ret = nam_set_ethport_speed(devNum,portNum,ethport_attr_default(moduleId)->speed);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set port(%d,%d) speed error %d\n ",devNum,portNum,ret);
				}	
				/* fiber port should not config auto negotiation because these ports haven't phy */
				if(24 <= portNum && 27 >= portNum) {
					/* Auto-Nego (AN-FC,AN-speed,AN-duplex)*/
					ret = nam_set_ethport_force_linkdown(devNum, portNum, 1);
					if(NPD_SUCCESS!= ret){
			          syslog_ax_eth_port_dbg("set port(%d,%d)force linkdown error\n" ,devNum,portNum);   
					}	
					ret = nam_set_ethport_duplex_autoneg(devNum,portNum,ETH_ATTR_DISABLE);
					if(NPD_SUCCESS != ret) {
						syslog_ax_eth_port_dbg("set port(%d,%d) duplex_an error %d\n ",devNum,portNum,ret);
					}		
					ret = nam_set_ethport_speed_autoneg(devNum,portNum,ETH_ATTR_DISABLE);
					if(NPD_SUCCESS != ret) {
						syslog_ax_eth_port_dbg("set port(%d,%d) speed_an error %d\n ",devNum,portNum,ret);
					}		
					ret = nam_set_ethport_fc_autoneg(devNum,portNum,ETH_ATTR_DISABLE,ETH_ATTR_DISABLE);
					if(NPD_SUCCESS != ret) {
						syslog_ax_eth_port_dbg("set port(%d,%d) flow control auto-nego error %d\n ",devNum,portNum,ret);
					}			
			        ret = nam_set_ethport_force_auto(devNum, portNum,0);
					if(NPD_SUCCESS != ret){
			          syslog_ax_eth_port_dbg("set port(%d,%d)force auto error\n" ,devNum,portNum);  
					}					
				}				
				break;	
			case MODULE_ID_AX7_6GE_SFP :
				/* port speed*/
				ret = nam_set_ethport_speed(devNum,portNum,ethport_attr_default(moduleId)->speed);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set port(%d,%d) speed error %d\n ",devNum,portNum,ret);
				}
				/* set port type*/
				ret = nam_set_ethport_type(devNum,portNum,PORT_TYPE_1000BASEX);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set SFP port(%d,%d) port type error %d\n ",devNum,portNum,ret);
				}	 				
				/* port InBand An enable*/
				ret = nam_set_ethport_inband_autoneg(devNum,portNum,ETH_ATTR_OFF);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set SFP port(%d,%d) inband auto-nego error %d\n ",	\
							devNum,portNum,ret);
				}		
				#if 0
				// port Inband An Mode
				ret |= nam_set_ethport_inband_an_mode(devNum,portNum,PORT_INBAND_AN_1000BASEX_MODE);
				#endif
				/* set external-PHY LED interface */
				ret = nam_set_ethport_led_intf(devNum,portNum,ETH_PORT_LED_INTF_DEFAULT);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set SFP port(%d,%d) LED interface error %d\n ",	\
							devNum,portNum,ret);
				}
				
				/* add port to poll array*/
				npd_eth_port_poll_add_member(eth_g_index); 
				break;
			case MODULE_ID_AX7_XFP :
			case MODULE_ID_SUBCARD_TYPE_A1 :
				#if 0
				/*GE port IPG already configured  in marvell Drv*/
				// set IPG for all GE_Port o special dev0:275
				ret |= nam_set_ethport_ipg(devNum);
				#endif 
				/* port speed*/
				ret = nam_set_ethport_speed(devNum,portNum,ethport_attr_default(moduleId)->speed);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set port(%d,%d) speed error %d\n ",devNum,portNum,ret);
				}	
				/* set Master XSMI interface AC TIMING for Access External Phy*/
				ret = nam_set_xsmi_ac_timing(devNum);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set XG port(%d,%d) AC timing error %d\n ",devNum,portNum,ret);
				}				
				/* set external Phy LED interface*/
				ret = nam_set_xgport_led_on_external_phy(devNum,portNum);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set XG port(%d,%d) LED on external phy error %d\n ",	\
							devNum,portNum, ret);
				}				
				/* set ports Buff Shared Mode,Profiles changed in CPSS*/
 				ret = nam_set_port_buff_mode(devNum,CPSS_DXCH_PORT_BUFFERS_MODE_SHARED_E);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set XG port(%d,%d) buffer mode error %d\n ",devNum,portNum,ret);
				}				
				/* set XG port IPG*/
				ret = nam_set_xg_port_ipg(devNum,portNum,\
										CPSS_DXCH_XG_PORT_IPG_FIXED_IPG_LAN_MODE_E,\
										CPSS_DXCH_XG_PORT_IPG_BASE_BYTE_MIN_E);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set XG port(%d,%d) ipg error %d\n ",devNum,portNum,ret);
				}				
				break;
			case MODULE_ID_AU3_3052 :
				/* back-pressure disable*/
				ret = nam_set_ethport_backPres(devNum,portNum,ethport_attr_default(moduleId)->bp);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set port(%d,%d) back pressure error %d\n ",	\
							devNum,portNum,ret);
				}
				/* port speed*/
				if (6 <= portNum) {
					ret = nam_set_ethport_speed(devNum,portNum,ethport_attr_default(moduleId)->speed);
					if(NPD_SUCCESS != ret) {
						syslog_ax_eth_port_dbg("set port(%d,%d) speed error %d\n ",devNum,portNum,ret);
					}
				}
				else if(1 == portNum){
					ret = nam_set_ethport_speed(devNum,portNum,PORT_SPEED_1000_E);
					if(NPD_SUCCESS != ret) {
						syslog_ax_eth_port_dbg("set port(%d,%d) speed error %d\n ",devNum,portNum,ret);
					}
					/*preferred media*/
					attr_bitmap |= 1 << ETH_PREFERRED_FIBER_MEDIA_BIT;
					ret = nam_set_eth_port_trans_media(devNum,portNum,COMBO_PHY_MEDIA_PREFER_FIBER);
					if(NPD_SUCCESS != ret) {
						syslog_ax_eth_port_dbg("set port(%d,%d) preferred media error %d\n ",devNum,portNum,ret);
					}	
				}
				/* set auto negotiation*/
				ret = nam_set_bcm_ethport_autoneg(devNum,portNum,ethport_attr_default(moduleId)->autoNego);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("set port(%d,%d) auto-nego error %d\n ",devNum,portNum,ret);
				}
				break;
			default :
				break;
		}		
		portInfo->attr_bitmap = attr_bitmap;
		
		/* MRU 1522*/
		portInfo->mtu = ETH_PORT_MRU_DEFAULT;
		ret = nam_set_ethport_mru(devNum,portNum,ETH_PORT_MRU_DEFAULT);
		if(NPD_SUCCESS != ret) {
			syslog_ax_eth_port_dbg("set port(%d,%d) mru error %d\n",devNum,portNum,ret);
		}
	}
	else if(NPD_TRUE == isBoard) {
		/* admin status enable*/
		attr_bitmap |= (ETH_PORT_ADMIN_STATUS_DEFAULT << ETH_ADMIN_STATUS_BIT);
		
		syslog_ax_eth_port_dbg("npd set port attr default: eth_g_index %d,admin_state %d.\n",eth_g_index,ethport_attr_default(moduleId)->admin_state);

		ret = nbm_set_ethport_enable(eth_g_index, ethport_attr_default(moduleId)->admin_state);
		/* port speed 1000M unsupport*/

		/* Auto-Nego on (AN-FC,AN-speed,AN-duplex) unsupport */
		attr_bitmap |= (ETH_ATTR_ON << ETH_AUTONEG_CTRL_BIT);
		/* ret = nbm_set_autoneg(eth_g_index, ETH_ATTR_DISABLE); */
		ret = nbm_set_autoneg(eth_g_index, ethport_attr_default(moduleId)->autoNego);
		
		/* ret = nbm_set_ethport_duplex_autoneg(eth_g_index,ethport_attr_default(moduleId)->duplex_an); */
		
		/*preferred media*/
		attr_bitmap |= 1 << ETH_PREFERRED_COPPER_MEDIA_BIT;
		ret = nbm_set_eth_port_trans_media(eth_g_index, ethport_attr_default(moduleId)->mediaPrefer);
		if(NPD_SUCCESS != ret) {
			syslog_ax_eth_port_dbg("set port(%d,%d) preferred media error %d\n ",devNum,portNum,ret);
		}	

#if 0		
		/* duplex mode full unsupport*/
		ret = nbm_set_ethport_duplex_mode(eth_g_index,ethport_attr_default(moduleId)->duplex);
		/* flow-control enable*/
		attr_bitmap |= (ETH_PORT_FC_STATE_DEFAULT << ETH_FLOWCTRL_BIT);
		ret |= nbm_set_ethport_flowCtrl(eth_g_index,ETH_ATTR_DISABLE);
		/* back-pressure disable*/
		attr_bitmap |= (ETH_PORT_BP_STATE_DEFAULT << ETH_BACKPRESSURE_BIT);	
		ret |= nbm_set_ethport_backPres(eth_g_index,ethport_attr_default(moduleId)->bp);
#endif

		/*preferred media none*/
		portInfo->attr_bitmap = attr_bitmap;
		/* MRU 1518 modify liuxy:*/
		ret = nbm_set_ethport_mru(eth_g_index,ethport_attr_default(moduleId)->mtu);
	}
	if(0 != ret) {
		ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
	}
	else {
		ret = ETHPORT_RETURN_CODE_ERR_NONE;
	}
	syslog_ax_eth_port_dbg("\tset port %d default mtu %d attribute %#x\n",	\
		eth_g_index,portInfo->mtu,portInfo->attr_bitmap);
	return ret;

}

/**********************************************************************************
 *	npd_eth_port_l3intf_create
 *
 *    create Layer 3 ethernet port interface(actually add L3 interface index to eth port structure)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		ifIndex - L3 interface index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		INTERFACE_RETURN_CODE_ERROR - if ethernet port is not exists or
 *				   memory allocation failed
 *		INTERFACE_RETURN_CODE_SUCCESS - if no error occurred
 *		
 *
 **********************************************************************************/
unsigned int npd_eth_port_l3intf_create
(
	unsigned short	eth_g_index,
	unsigned int	ifIndex
)
{
	struct eth_port_s *portInfo = NULL;
	struct eth_port_intf_s *node = NULL;

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		return INTERFACE_RETURN_CODE_ERROR;
	}

	node = portInfo->funcs.func_data[ETH_PORT_FUNC_IPV4];
	if(NULL != node) { /* vlan interface created before*/
		syslog_ax_eth_port_err("npd eth port l3 intf create index change %#0x <--> %#0x\n",node->ifIndex,ifIndex);
	}
	else {
		node = (struct eth_port_intf_s *)malloc(sizeof(struct eth_port_intf_s));
		if(NULL == node) {
			syslog_ax_eth_port_err("npd eth port %#0x l3 intf create error memory alloc\n",eth_g_index);
			return INTERFACE_RETURN_CODE_ERROR;
		}
		memset(node ,0 ,sizeof(struct eth_port_intf_s));	
	}

	node->ifIndex = ifIndex;
	memset((char*)node->uniMac, 0xFF, 6);
	node->npd_route_port_notifier_func = (NPD_ETH_PORT_LINK_CHANGE_NOTIFIER_FUNC)npd_route_port_link_change;
	portInfo->funcs.func_data[ETH_PORT_FUNC_IPV4] = node;
	portInfo->funcs.funcs_run_bitmap |= (1 << ETH_PORT_FUNC_IPV4);
	return INTERFACE_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 *	npd_eth_port_l3intf_destroy
 *
 *    destroy Layer 3 ethernet port interface(actually delete L3 interface index from eth port structure)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		ifIndex - L3 interface index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_FAIL - if ethernet port is not exists or
 *				   
 *		NPD_SUCCESS - if no error occurred
 *		
 *
 **********************************************************************************/
unsigned int npd_eth_port_l3intf_destroy
(
	unsigned short	eth_g_index,
	unsigned int	ifIndex
)
{
	struct eth_port_s *portInfo = NULL;
	struct eth_port_intf_s *node = NULL;

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		return NPD_FAIL;
	}

	node = portInfo->funcs.func_data[ETH_PORT_FUNC_IPV4];

	if(NULL == node) {
		syslog_ax_eth_port_err("npd eth port l3 intf destroy index %#0x not created!",ifIndex);
		return NPD_FAIL;
	}
	else {
		if(node->ifIndex != ifIndex) {
			syslog_ax_eth_port_err("npd eth port destroy l3 intf %#0x but has %#0x",ifIndex,node->ifIndex);
		}
		free(node);
		portInfo->funcs.func_data[ETH_PORT_FUNC_IPV4] = NULL;
		portInfo->funcs.funcs_run_bitmap &= ~(1 << ETH_PORT_FUNC_IPV4);
	}
	
	return NPD_SUCCESS;
}

/**********************************************************************************
 *  npd_eth_port_interface_check
 *
 *	DESCRIPTION:
 * 		check out if specified ethernet port is a L3 interface
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		ifIndex - L3 interface index
 *
 * 	RETURN:
 *		NPD_TRUE  - if eth port is a L3 interface
 *		NPD_FALSE - if eth port is not a L3 interface
 *		
 *
 **********************************************************************************/
unsigned int npd_eth_port_interface_check
(
	unsigned int 	eth_g_index,
	unsigned int   *ifIndex
)
{
	struct eth_port_s  *portInfo = NULL;
	struct eth_port_intf_s *l3Intf = NULL;

	portInfo = npd_get_port_by_index(eth_g_index);

	if(NULL == portInfo) {
		syslog_ax_eth_port_err("npd eth port %#x interface check fail for node null",eth_g_index);
		return NPD_FALSE;
	}
	
	if(NULL == portInfo->funcs.func_data[ETH_PORT_FUNC_IPV4]) {
		*ifIndex = ~0UI;
		return NPD_TRUE;
	}

	l3Intf = (struct eth_port_intf_s*)(portInfo->funcs.func_data[ETH_PORT_FUNC_IPV4]);
	*ifIndex = l3Intf->ifIndex;
	
	return NPD_TRUE;
}

/**********************************************************************************
 *  npd_eth_port_interface_l3_flag_set
 *
 *	DESCRIPTION:
 * 		check out if specified ethernet port is a L3 interface
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		enable  - set l3_disable to (!enable)
 *	
 *	OUTPUT:
 *		NONE
 *
 * 	RETURN:
 *		NPD_FAIL  - set failed
 *		NPD_SUCCESS - set success
 *		
 *
 **********************************************************************************/
int npd_eth_port_interface_l3_flag_set
(
	unsigned int 	eth_g_index,
	unsigned int   	enable
)
{
	struct eth_port_s  *portInfo = NULL;
	struct eth_port_intf_s *l3Intf = NULL;

	portInfo = npd_get_port_by_index(eth_g_index);

	if(NULL == portInfo) {
		syslog_ax_eth_port_err("npd eth port %#x interface check fail for node null",eth_g_index);
		return NPD_FAIL;
	}
	
	if(NULL == portInfo->funcs.func_data[ETH_PORT_FUNC_IPV4]) {
		return NPD_FAIL;
	}

	l3Intf = (struct eth_port_intf_s*)(portInfo->funcs.func_data[ETH_PORT_FUNC_IPV4]);
	l3Intf->l3_disable = (!enable);
	
	return NPD_SUCCESS;
}


/**********************************************************************************
 *  npd_eth_port_interface_l3_flag_get
 *
 *	DESCRIPTION:
 * 		check out if specified ethernet port is a L3 interface
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		enable  - get l3 enable or not
 *
 * 	RETURN:
 *		NPD_FAIL  - set failed
 *		NPD_SUCCESS - set success
 *		
 *
 **********************************************************************************/
int npd_eth_port_interface_l3_flag_get
(
	unsigned int 	eth_g_index,
	unsigned int   	*enable
)
{
	struct eth_port_s  *portInfo = NULL;
	struct eth_port_intf_s *l3Intf = NULL;

	portInfo = npd_get_port_by_index(eth_g_index);

	if(NULL == portInfo) {
		syslog_ax_eth_port_err("npd eth port %#x interface check fail for node null",eth_g_index);
		return NPD_FAIL;
	}
	
	if(NULL == portInfo->funcs.func_data[ETH_PORT_FUNC_IPV4]) {
		return NPD_FAIL;
	}

	l3Intf = (struct eth_port_intf_s*)(portInfo->funcs.func_data[ETH_PORT_FUNC_IPV4]);
	if(l3Intf){
		*enable = !(l3Intf->l3_disable);
	}
	
	return NPD_SUCCESS;
}

/**********************************************************************************
 *  npd_eth_port_promis_interface_check
 *
 *	DESCRIPTION:
 * 		check out if specified ethernet port is a L3 interface
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		ifIndex - L3 interface index
 *
 * 	RETURN:
 *		NPD_TRUE  - if eth port is a L3 interface
 *		NPD_FALSE - if eth port is not a L3 interface
 *		
 *
 **********************************************************************************/
unsigned int npd_eth_port_promis_interface_check
(
	unsigned int 	eth_g_index,
	unsigned int   *ifIndex
)
{
	struct eth_port_s  *portInfo = NULL;
	struct eth_port_intf_s *l3Intf = NULL;

	portInfo = npd_get_port_by_index(eth_g_index);

	if(NULL == portInfo) {
		syslog_ax_eth_port_err("npd eth port %#x interface check fail for node null",eth_g_index);
		return NPD_FALSE;
	}
	
	if(NULL == portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS]) {
		*ifIndex = ~0UI;
		return NPD_FALSE;
	}

	l3Intf = (struct eth_port_intf_s*)(portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS]);
	*ifIndex = l3Intf->ifIndex;
	
	return NPD_TRUE;
}


/**********************************************************************************
 *	npd_eth_port_l3intf_mac_get
 *
 *    Get Layer 3 ethernet port interface mac address
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		macAddr - mac address
 *
 * 	RETURN:
 *		NPD_FAIL - if ethernet port is not exists or
 *				   input parameter illegal or
 *				   port is not route mode interface
 *		NPD_SUCCESS - if no error occurred
 **********************************************************************************/
unsigned int npd_eth_port_l3intf_mac_get
(
	unsigned short	eth_g_index,
	unsigned char *macAddr
)
{
	struct eth_port_s *portInfo = NULL;
	struct eth_port_intf_s *node = NULL;

	if(!macAddr) {		
		syslog_ax_eth_port_err("get port %#x route mode intf mac null\n",  \
							eth_g_index);
		return NPD_FAIL;
	}
	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		syslog_ax_eth_port_err("get port %#x route mode intf mac find port null\n",  \
							eth_g_index);
		return NPD_FAIL;
	}

	node = portInfo->funcs.func_data[ETH_PORT_FUNC_IPV4];
	if(NULL == node) { /* vlan interface created before*/
		syslog_ax_eth_port_err("port %#x not route mode l3 intf when get mac\n",   \
							eth_g_index);
		return NPD_FAIL;
	}
	else {
		memcpy(macAddr, node->uniMac, 6);
	}

	syslog_ax_eth_port_dbg("get port %#x route mode intf mac %02x:%02x:%02x:%02x:%02x:%02x ok\n", \
							eth_g_index, macAddr[0], macAddr[1], macAddr[2],  \
							macAddr[3], macAddr[4], macAddr[5]);
	return NPD_SUCCESS;
}

/**********************************************************************************
 *	npd_eth_port_l3intf_mac_set
 *
 *    Set Layer 3 ethernet port interface mac address
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		macAddr - mac address
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_FAIL - if ethernet port is not exists or
 *				   input parameter illegal or
 *				   port is not route mode interface
 *		NPD_SUCCESS - if no error occurred
 *		
 *
 **********************************************************************************/
unsigned int npd_eth_port_l3intf_mac_set
(
	unsigned short	eth_g_index,
	unsigned char *macAddr
)
{
	struct eth_port_s *portInfo = NULL;
	struct eth_port_intf_s *node = NULL;

	if(!macAddr) {		
		syslog_ax_eth_port_err("set port %#x route mode intf mac null\n",  \
							eth_g_index);
		return NPD_FAIL;
	}
	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		syslog_ax_eth_port_err("set port %#x route mode intf mac find port null\n",  \
							eth_g_index);
		return NPD_FAIL;
	}

	node = portInfo->funcs.func_data[ETH_PORT_FUNC_IPV4];
	if(NULL == node) { /* vlan interface created before*/
		syslog_ax_eth_port_err("port %#x not route mode l3 intf when set mac\n",   \
							eth_g_index);
		return NPD_FAIL;
	}
	else {
		memcpy(node->uniMac, macAddr,6);
	}
	syslog_ax_eth_port_dbg("set port %#x route mode intf mac %02x:%02x:%02x:%02x:%02x:%02x ok\n", \
							eth_g_index, macAddr[0], macAddr[1], macAddr[2],  \
							macAddr[3], macAddr[4], macAddr[5]);
	return NPD_SUCCESS;
}

/**********************************************************************************
 *	npd_eth_port_l3intf_ustatus_get
 *
 *    Get Layer 3 ethernet port interface status in user space(NPD scope)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		state - l3 interface status
 *
 * 	RETURN:
 *		NPD_FAIL - if ethernet port is not exists or
 *				   input parameter illegal or
 *				   port is not route mode interface
 *		NPD_SUCCESS - if no error occurred
 **********************************************************************************/
unsigned int npd_eth_port_l3intf_ustatus_get
(
	unsigned short	eth_g_index,
	unsigned char *state
)
{
	struct eth_port_s *portInfo = NULL;
	struct eth_port_intf_s *node = NULL;

	if(!state) {		
		syslog_ax_eth_port_err("get port %#x route mode intf ustatus null\n",  \
							eth_g_index);
		return NPD_FAIL;
	}
	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		syslog_ax_eth_port_err("get port %#x route mode intf status find port null\n",  \
							eth_g_index);
		return NPD_FAIL;
	}

	node = portInfo->funcs.func_data[ETH_PORT_FUNC_IPV4];
	if(NULL == node) { /* vlan interface created before*/
		syslog_ax_eth_port_err("port %#x not route mode l3 intf when get ustatus\n",   \
							eth_g_index);
		return NPD_FAIL;
	}
	else {
		*state = node->state;
	}

	syslog_ax_eth_port_dbg("get port %#x route mode intf ustatus %s(%d) ok\n", \
							eth_g_index, *state ? "UP":"DOWN", *state);
	return NPD_SUCCESS;
}

/**********************************************************************************
 *	npd_eth_port_l3intf_ustatus_set
 *
 *    Set Layer 3 ethernet port interface status in user space(NPD scope)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		state - l3 interface status
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_FAIL - if ethernet port is not exists or
 *				   input parameter illegal or
 *				   port is not route mode interface
 *		NPD_SUCCESS - if no error occurred
 *		
 *
 **********************************************************************************/
unsigned int npd_eth_port_l3intf_ustatus_set
(
	unsigned short	eth_g_index,
	unsigned char state
)
{
	struct eth_port_s *portInfo = NULL;
	struct eth_port_intf_s *node = NULL;

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		syslog_ax_eth_port_err("set port %#x route mode intf ustatus find port null\n",  \
							eth_g_index);
		return NPD_FAIL;
	}

	node = portInfo->funcs.func_data[ETH_PORT_FUNC_IPV4];
	if(NULL == node) { /* vlan interface created before*/
		syslog_ax_eth_port_err("port %#x not route mode l3 intf when set ustatus\n",   \
							eth_g_index);
		return NPD_FAIL;
	}
	else {
		node->state = state;
	}
	syslog_ax_eth_port_dbg("set port %#x route mode intf ustatus %s(%d) ok\n", \
							eth_g_index, state ? "UP":"DOWN", state);
	return NPD_SUCCESS;
}

/**********************************************************************************
 *  npd_eth_port_check_tag_for_other_subif
 *
 *	DESCRIPTION:
 * 		check promi subif vlan id
 *
 *	INPUT:
 *		eth_g_index - port index
 *		vid 				- vlan id for subif
 *		vid2				-vlan id for qinq subif
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		TRUE
 *		FALSE
 *		
 *
 **********************************************************************************/
int npd_eth_port_check_tag_for_other_subif
(
	unsigned int eth_g_index,
	unsigned short  vid,
	unsigned short vid2
)
{
	struct eth_port_s *portInfo = NULL;
	struct eth_port_promi_s* promiInfo = NULL;
	struct eth_port_dot1q_list_s *subifNode = NULL;
	struct eth_port_qinq_list_s *qinqIfNode = NULL;
	struct list_head *list=NULL,*pos=NULL;
	struct list_head *list2=NULL,*pos2=NULL;

	if((vid > (NPD_PORT_L3INTF_VLAN_ID-1))||(vid2 > (NPD_PORT_L3INTF_VLAN_ID-1)))	/* code optimize:Unsigned compared against 0 houxx@autelan.com  2013-1-17 */
	{
		return FALSE;
	}

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) 
	{
		syslog_ax_eth_port_err("check promiscuous port %#x subif on vlan %d %d found port null\n",eth_g_index,vid,vid2);
		return FALSE;
	}

	if(NULL == (promiInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS])) 
	{
		return FALSE;
	}
	if(0 != vid2)
	{
		if(promiInfo->subifList.count > 0)
		{
			list = &(promiInfo->subifList.list);
			__list_for_each(pos,list) 
			{
				subifNode = list_entry(pos,struct eth_port_dot1q_list_s,list);
				if(vid2 == subifNode->vid) 
				{
					return TRUE;
				}
				else if((vid != subifNode->vid)&&(subifNode->subifList.count)) 
				{
					list2 = &(subifNode->subifList.list);		
					__list_for_each(pos2,list2) 
					{
						qinqIfNode = list_entry(pos2,struct eth_port_qinq_list_s,list);
						if((NULL != qinqIfNode)&&(vid2 == qinqIfNode->vid)) 
						{
							return TRUE;
						}
					}
				}
			}
		}
	}
	else
	{
		if(promiInfo->subifList.count > 0)
		{
			list = &(promiInfo->subifList.list);
			__list_for_each(pos,list) 
			{
				subifNode = list_entry(pos,struct eth_port_dot1q_list_s,list);
				if(vid != subifNode->vid) 
				{
					if(subifNode->subifList.count) 
					{
						list2 = &(subifNode->subifList.list);		
						__list_for_each(pos2,list2) 
						{
							qinqIfNode = list_entry(pos2,struct eth_port_qinq_list_s,list);
							if((NULL != qinqIfNode)&&(vid == qinqIfNode->vid)) 
							{
								syslog_ax_eth_port_dbg("check promiscuous port %#x subif on vlan %d %d found duplicate,count %d",	\
										eth_g_index,vid,vid2,subifNode->subifList.count);
								return TRUE;
							}
						}
					}
				}
			}
		}
	}
	
	return FALSE;
}


/**********************************************************************************
 *  npd_eth_port_check_promi_subif
 *
 *	DESCRIPTION:
 * 		check promi subif vlan id
 *
 *	INPUT:
 *		eth_g_index - port index
 *		vid 				- vlan id
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		
 *		TRUE
 *		FALSE
 *		
 *
 **********************************************************************************/
int npd_eth_port_check_promi_subif
(
	unsigned int eth_g_index,
	unsigned short  vid
)
{
	struct eth_port_s *portInfo = NULL;
	struct eth_port_promi_s* promiInfo = NULL;
	struct eth_port_dot1q_list_s *subifNode = NULL;
	struct list_head *list=NULL,*pos=NULL;

	/* code optimize: Logically dead code  houxx@autelan.com  2013-1-17 */
	if((vid < DEFAULT_VLAN_ID) || (vid > (NPD_PORT_L3INTF_VLAN_ID))) /* code optimize: Logically dead code  houxx@autelan.com  2013-1-17 */
	{
		return FALSE;
	}

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) 
	{
		syslog_ax_eth_port_err("check promiscuous port %#x subif on vlan %d found port null\n",	\
					eth_g_index,vid);
		return FALSE;
	}

	if(NULL == (promiInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS])) 
	{
		return FALSE;
	}

	list = &(promiInfo->subifList.list);
	__list_for_each(pos,list) 
	{
		subifNode = list_entry(pos,struct eth_port_dot1q_list_s,list);
		if(vid == subifNode->vid) 
		{
			syslog_ax_eth_port_dbg("check promiscuous port %#x subif on vlan %d found duplicate,count %d",	\
							eth_g_index,vid,promiInfo->subifList.count);
			return TRUE;
		}
	}
	
	return FALSE;
}

/**********************************************************************************
 *  npd_eth_port_check_qinq_subif
 *
 *	DESCRIPTION:
 * 		check promi subif vlan id
 *
 *	INPUT:
 *		eth_g_index - port index
 *		vid 				- vlan id
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		TRUE
 *		FALSE
 *		
 *
 **********************************************************************************/
int npd_eth_port_check_qinq_subif
(
	unsigned int eth_g_index,
	unsigned short  vid,
	unsigned short vid2
)
{
	struct eth_port_s *portInfo = NULL;
	struct eth_port_promi_s* promiInfo = NULL;
	struct eth_port_dot1q_list_s *subifNode = NULL;
	struct eth_port_qinq_list_s *qinqIfNode = NULL;
	struct list_head *list=NULL,*pos=NULL;
	struct list_head *list2=NULL,*pos2=NULL;
	
	/* code optimize: Logically dead code  houxx@autelan.com  2013-1-17 */
	if(((vid < DEFAULT_VLAN_ID) || (vid > (NPD_PORT_L3INTF_VLAN_ID-1)))||((vid2 < DEFAULT_VLAN_ID) || (vid2 > (NPD_PORT_L3INTF_VLAN_ID-1))))
	{
		return FALSE;
	}

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) 
	{
		syslog_ax_eth_port_err("check promiscuous port %#x subif on vlan %d %d found port null\n",	\
					eth_g_index,vid,vid2);
		return FALSE;
	}

	if(NULL == (promiInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS])) 
	{
		return FALSE;
	}
	if(promiInfo->subifList.count > 0)
	{
		list = &(promiInfo->subifList.list);
		__list_for_each(pos,list) 
		{
			subifNode = list_entry(pos,struct eth_port_dot1q_list_s,list);
			if(vid == subifNode->vid) 
			{
				if(subifNode->subifList.count) 
				{
					list2 = &(subifNode->subifList.list);		
					__list_for_each(pos2,list2) 
					{
						qinqIfNode = list_entry(pos2,struct eth_port_qinq_list_s,list);
						if((NULL != qinqIfNode)&&(vid2 == qinqIfNode->vid)) 
						{
							syslog_ax_eth_port_dbg("check promiscuous port %#x subif on vlan %d %d found duplicate,count %d",	\
									eth_g_index,vid,vid2,subifNode->subifList.count);
							return TRUE;
						}
					}
				}
			}
		}
	}
	
	return FALSE;
}


/**********************************************************************************
 *  npd_eth_port_get_subif_count_for_subif
 *
 *	DESCRIPTION:
 * 		check promi subif vlan id
 *
 *	INPUT:
 *		eth_g_index - port index
 *		vid 				- vlan id
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		the count of subif of this subif
 *		
 *
 **********************************************************************************/
int npd_eth_port_get_subif_count_for_subif
(
	unsigned int eth_g_index,
	unsigned short  vid
)
{
	struct eth_port_s *portInfo = NULL;
	struct eth_port_promi_s* promiInfo = NULL;
	struct eth_port_dot1q_list_s *subifNode = NULL;
	struct eth_port_qinq_list_s *qinqIfNode = NULL;
	struct list_head *list=NULL,*pos=NULL;

	if((vid < DEFAULT_VLAN_ID) || (vid > (NPD_PORT_L3INTF_VLAN_ID-1)))	/* code optimize: Logically dead code  houxx@autelan.com  2013-1-17 */
	{
		return FALSE;
	}

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) 
	{
		syslog_ax_eth_port_err("check promiscuous port %#x subif on vlan %d found port null\n",	\
					eth_g_index,vid);
		return 0;
	}

	if(NULL == (promiInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS])) 
	{
		return 0;
	}
	if(promiInfo->subifList.count > 0)
	{
		list = &(promiInfo->subifList.list);
		__list_for_each(pos,list) 
		{
			subifNode = list_entry(pos,struct eth_port_dot1q_list_s,list);
			if(vid == subifNode->vid) 
			{
				return subifNode->subifList.count;
			}
		}
	}
	
	return 0;
}


/**********************************************************************************
 *  npd_eth_port_create_promi_subif
 *
 *	DESCRIPTION:
 * 		create promi subif
 *
 *	INPUT:
 *		eth_g_index - port index
 *		vid - vlan id
 *	
 *	OUTPUT:
 *		
 * 	RETURN:
 *		INTERFACE_RETURN_CODE_ERROR  
 *		INTERFACE_RETURN_CODE_SUCCESS 
 *		
 *
 **********************************************************************************/
int npd_eth_port_create_promi_subif
(
	unsigned int eth_g_index,
	unsigned short vid
)
{
	struct eth_port_s *portInfo = NULL;
	struct eth_port_promi_s* promiInfo = NULL;
	struct eth_port_dot1q_list_s *subifNode = NULL;

	if((vid < DEFAULT_VLAN_ID) && (vid > (NPD_PORT_L3INTF_VLAN_ID-1)))
		return INTERFACE_RETURN_CODE_ERROR;

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		syslog_ax_eth_port_err("npd eth port promi check no port index %d ",eth_g_index);
		return INTERFACE_RETURN_CODE_ERROR;
	}

	if(NULL == (promiInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS])) {
		return INTERFACE_RETURN_CODE_ERROR;
	}

	if(NPD_TRUE == npd_eth_port_check_promi_subif(eth_g_index,vid)) {
		return INTERFACE_RETURN_CODE_ERROR;
	}
	
	subifNode = (struct eth_port_dot1q_list_s*)malloc(sizeof(struct eth_port_dot1q_list_s));
	if(NULL == subifNode) {
		syslog_ax_eth_port_err("npd eth port promi create subif %d on vlan %d malloc mem err",eth_g_index,vid);
		return INTERFACE_RETURN_CODE_ERROR;
	}
	else {
		memset(subifNode,0,sizeof(struct eth_port_dot1q_list_s));
		INIT_LIST_HEAD(&(subifNode->subifList.list));
		subifNode->vid = vid;
		list_add(&(subifNode->list),&(promiInfo->subifList.list));
		promiInfo->subifList.count ++;
		syslog_ax_eth_port_dbg("npd eth port promi port index %d on vlan %d node count %d",eth_g_index,vid,promiInfo->subifList.count);
	}
	
	return INTERFACE_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 *  npd_eth_port_create_qinq_subif
 *
 *	DESCRIPTION:
 * 		create promi subif
 *
 *	INPUT:
 *		eth_g_index - port index
 *		vid - vlan id for promis subif
 *		vid2 - vid for qinq interface
 *	
 *	OUTPUT:
 *		
 * 	RETURN:
 *		NPD_DBUS_ERROR  
 *		NPD_DBUS_SUCCESS 
 *		
 *
 **********************************************************************************/
int npd_eth_port_create_qinq_subif
(
	unsigned int eth_g_index,
	unsigned short vid,
	unsigned short vid2
)
{
	struct eth_port_s *portInfo = NULL;
	struct eth_port_promi_s* promiInfo = NULL;
	struct eth_port_dot1q_list_s *subifNode = NULL;
	struct eth_port_qinq_list_s *qinqIfNode = NULL;
	struct list_head * list = NULL,*pos=NULL;
	/* code optimize: Logically dead code  houxx@autelan.com  2013-1-17 */
	if(((vid < DEFAULT_VLAN_ID) || (vid > (NPD_PORT_L3INTF_VLAN_ID-1)))||((vid2 < DEFAULT_VLAN_ID) || (vid2 > (NPD_PORT_L3INTF_VLAN_ID-1))))
	{
		return NPD_DBUS_ERROR;
	}

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) 
	{
		syslog_ax_eth_port_err("npd eth port promi check no port index %d ",eth_g_index);
		return NPD_DBUS_ERROR;
	}

	if(NULL == (promiInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS])) 
	{
		return NPD_DBUS_ERROR;
	}
	
	qinqIfNode = (struct eth_port_qinq_list_s*)malloc(sizeof(struct eth_port_qinq_list_s));
	if(NULL == qinqIfNode) 
	{
		syslog_ax_eth_port_err("npd eth port promi create qinq subif %d %d on vlan %d malloc mem err",eth_g_index,vid,vid2);
		return NPD_DBUS_ERROR;
	}
	else 
	{
		memset(qinqIfNode,0,sizeof(struct eth_port_qinq_list_s));
		qinqIfNode->vid = vid2;
		list = &(promiInfo->subifList.list);
		__list_for_each(pos,list) 
		{
			subifNode = list_entry(pos,struct eth_port_dot1q_list_s,list);
			if((NULL != subifNode)&&(vid == subifNode->vid)) 
			{
				list_add(&(qinqIfNode->list),&(subifNode->subifList.list));
				subifNode->subifList.count ++;
				syslog_ax_eth_port_dbg("npd eth port promi port index %d on vlan %d %d node count %d",eth_g_index,vid,vid2,subifNode->subifList.count);
				return NPD_DBUS_SUCCESS;
			}
		}
		
	}
	free(qinqIfNode);	/* code optimize: recourse leak houxx@autelan.com  2013-1-17 */
	qinqIfNode = NULL;
	return NPD_DBUS_ERROR;
}


/**********************************************************************************
 *  npd_eth_port_del_promi_subif
 *
 *	DESCRIPTION:
 * 		del promi subif
 *
 *	INPUT:
 *		eth_g_index - port index
 *		vid				- vlan id
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		INTERFACE_RETURN_CODE_ERROR  
 *		INTERFACE_RETURN_CODE_SUCCESS 
 *		INTERFACE_RETURN_CODE_SUBIF_EXISTS
 *		
 *
 **********************************************************************************/
unsigned int npd_eth_port_del_promi_subif
(
	unsigned int eth_g_index,
	unsigned short vid
)
{
	struct eth_port_s *portInfo = NULL;
	struct eth_port_promi_s* promiInfo = NULL;
	struct eth_port_dot1q_list_s *subifNode = NULL;
	struct list_head *list=NULL,*pos=NULL;

	if((vid < DEFAULT_VLAN_ID) && (vid > (NPD_PORT_L3INTF_VLAN_ID-1)))
		return INTERFACE_RETURN_CODE_ERROR;

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		syslog_ax_eth_port_err("npd eth port promi check no port index %d \n",eth_g_index);
		return INTERFACE_RETURN_CODE_ERROR;
	}

	if(NULL == (promiInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS])) {
		return INTERFACE_RETURN_CODE_ERROR;
	}

	if(promiInfo->subifList.count > 0) {
	list = &(promiInfo->subifList.list);
	__list_for_each(pos,list) {
			subifNode = list_entry(pos,struct eth_port_dot1q_list_s,list);
			if((NULL != subifNode) && (vid == subifNode->vid)) {
				if(subifNode->subifList.count > 0){
					return INTERFACE_RETURN_CODE_SUBIF_EXISTS;
				}
				__list_del(subifNode->list.prev,subifNode->list.next);				
				free(subifNode);
				if(promiInfo->subifList.count > 0) {
					promiInfo->subifList.count--;
				}
				break;
			}
		}
		syslog_ax_eth_port_dbg("npd eth port promi del sunif %d on vlan %d node count %d",	\
							eth_g_index ,vid,promiInfo->subifList.count);
	}	
	return INTERFACE_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 *  npd_eth_port_del_qinq_subif
 *
 *	DESCRIPTION:
 * 		del promi subif
 *
 *	INPUT:
 *		eth_g_index - port index
 *		vid				- vlan id for promis subif
 *		vid2 			- vlan id for qinq subif
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		INTERFACE_RETURN_CODE_ERROR  
 *		INTERFACE_RETURN_CODE_SUCCESS 
 *		
 *
 **********************************************************************************/
unsigned int npd_eth_port_del_qinq_subif
(
	unsigned int eth_g_index,
	unsigned short vid,
	unsigned short vid2
)
{
	struct eth_port_s *portInfo = NULL;
	struct eth_port_promi_s* promiInfo = NULL;
	struct eth_port_dot1q_list_s *subifNode = NULL;
	struct eth_port_qinq_list_s *qinqifNode = NULL;
	struct list_head *list=NULL,*pos=NULL;
	struct list_head *list2=NULL,*pos2=NULL;

	if(((vid < DEFAULT_VLAN_ID) && (vid > (NPD_PORT_L3INTF_VLAN_ID-1)))||\
		(vid2 < DEFAULT_VLAN_ID) && (vid2 > (NPD_PORT_L3INTF_VLAN_ID-1)))
		return INTERFACE_RETURN_CODE_ERROR;

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		syslog_ax_eth_port_err("npd eth port promi check no port index %d \n",eth_g_index);
		return INTERFACE_RETURN_CODE_ERROR;
	}

	if(NULL == (promiInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS])) {
		return INTERFACE_RETURN_CODE_ERROR;
	}

	if(promiInfo->subifList.count > 0) {
	list = &(promiInfo->subifList.list);
	__list_for_each(pos,list) {
			subifNode = list_entry(pos,struct eth_port_dot1q_list_s,list);
			if((NULL != subifNode) && (vid == subifNode->vid)) {
				if(subifNode->subifList.count > 0) {
					list2 = &(subifNode->subifList.list);
					__list_for_each(pos2,list2) {
					qinqifNode = list_entry(pos2,struct eth_port_qinq_list_s,list);
					if((NULL != qinqifNode)&&(vid2 == qinqifNode->vid))
						__list_del(qinqifNode->list.prev,qinqifNode->list.next);
						free(qinqifNode);										
						if(subifNode->subifList.count > 0) {
							subifNode->subifList.count--;
						}
						break;
					}
				}
			}
		}
		syslog_ax_eth_port_dbg("npd eth port promi del qinq sunif %d on vlan %d %d node count %d",	\
							eth_g_index ,vid,vid2,subifNode->subifList.count);
	}
	
	return INTERFACE_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 *  npd_eth_port_destroy_all_promi_subif
 *
 *	DESCRIPTION:
 * 		del promi all subif
 *
 *	INPUT:
 *		eth_g_index - port index
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR  
 *		NPD_SUCCESS 
 *		
 *
 **********************************************************************************/
unsigned int npd_eth_port_destroy_all_promi_subif
(
	unsigned int eth_g_index
)
{
	struct eth_port_s *portInfo = NULL;
	struct eth_port_promi_s* promiInfo = NULL;
	struct eth_port_dot1q_list_s *subifNode = NULL;
	struct list_head *list=NULL,*pos=NULL;

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		syslog_ax_eth_port_err("npd eth port promi check no port index %d ",eth_g_index);
		return NPD_DBUS_ERROR;
	}

	if(NULL == (promiInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS])) {
		return NPD_DBUS_ERROR;
	}

	if(promiInfo->subifList.count > 0) {
		list = &(promiInfo->subifList.list);
		__list_for_each(pos,list) {
			subifNode = list_entry(pos,struct eth_port_dot1q_list_s,list);
			if(NULL != subifNode) {
				npd_eth_port_destroy_all_qinq_subif(eth_g_index,subifNode->vid);
				__list_del(subifNode->list.prev,subifNode->list.next);
				syslog_ax_eth_port_dbg("npd eth port promi del sunif %d on vlan %d node count %d",	\
								eth_g_index ,subifNode->vid,promiInfo->subifList.count);
				free(subifNode);
				if(promiInfo->subifList.count > 0) {
					promiInfo->subifList.count--;
				}
			}
		}
	}
	return NPD_DBUS_SUCCESS;
}

/**********************************************************************************
 *  npd_eth_port_destroy_all_qinq_subif
 *
 *	DESCRIPTION:
 * 		del promi all subif
 *
 *	INPUT:
 *		eth_g_index - port index
 *		vid  - subif first tag
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR  
 *		NPD_SUCCESS 
 *		
 *
 **********************************************************************************/
unsigned int npd_eth_port_destroy_all_qinq_subif
(
	unsigned int eth_g_index,
	unsigned short vid
)
{
	struct eth_port_s *portInfo = NULL;
	struct eth_port_promi_s* promiInfo = NULL;
	struct eth_port_dot1q_list_s *subifNode = NULL;
	struct eth_port_qinq_list_s *qinqifNode = NULL;
	struct list_head *list=NULL,*pos=NULL;
	struct list_head *list2=NULL,*pos2=NULL;

	/* code optimize: Logically dead code  houxx@autelan.com  2013-1-17 */
    if((vid < DEFAULT_VLAN_ID) || (vid > (NPD_PORT_L3INTF_VLAN_ID-1)))
	{
		return NPD_DBUS_ERROR;
    }
	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) 
	{
		syslog_ax_eth_port_err("npd eth port promi check no port index %d ",eth_g_index);	
		return NPD_DBUS_ERROR;
	}

	if(NULL == (promiInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS])) 
	{
		return NPD_DBUS_ERROR;
	}

	if(promiInfo->subifList.count > 0) 
	{
		list = &(promiInfo->subifList.list);
		__list_for_each(pos,list) 
		{
			subifNode = list_entry(pos,struct eth_port_dot1q_list_s,list);
			if((NULL != subifNode)&&(vid == subifNode->vid)) 
			{
				if(subifNode->subifList.count > 0) 
				{
					list2 = &(subifNode->subifList.list);
					__list_for_each(pos2,list2) 
					{
						qinqifNode = list_entry(pos2,struct eth_port_qinq_list_s,list);
						if(NULL != qinqifNode)
						{
							__list_del(qinqifNode->list.prev,qinqifNode->list.next);
							syslog_ax_eth_port_dbg("npd eth port promi del sunif %d on vlan %d %d node count %d",	\
											eth_g_index ,vid,qinqifNode->vid,subifNode->subifList.count);
							free(qinqifNode);
						}						
						if(subifNode->subifList.count > 0) 
						{
							subifNode->subifList.count--;
						}
					}
				}
			}
		}
	}
	return NPD_DBUS_SUCCESS;
}


/**********************************************************************************
 *  npd_eth_port_save_intf_qinq_type_cfg
 *
 *	DESCRIPTION:
 * 		set port ve flag
 *
 *	INPUT:
 *		strshow - string for show running ...
 *		avalidLen - current avalid length of show string
 *	
 *	OUTPUT:
 *		
 * 	RETURN:
 *		INTERFACE_RETURN_CODE_ERROR  
 *		INTERFACE_RETURN_CODE_SUCCESS 
 *		
 *
 **********************************************************************************/
unsigned int npd_eth_port_save_intf_qinq_type_cfg
(
	char** strShow,
	unsigned int* avalidLen
)
{
	struct eth_port_s *portInfo = NULL;
	struct eth_port_promi_s* promiInfo = NULL;
	struct eth_port_dot1q_list_s *subifNode = NULL;
	enum product_id_e productId = PRODUCT_ID; 
	struct list_head *list=NULL,*pos=NULL;
	unsigned int length = 0;
	int i =0 ,j = 0;
    int fd = -1;
    struct vlan_ioctl_args if_request;
    unsigned short qinqType = 0;
	int ifnameType = 0;
	unsigned int ifIndex = ~0UI;

	if((NULL == strShow)||(NULL == *strShow)||(NULL == avalidLen)) {
		return INTERFACE_RETURN_CODE_ERROR;
	}
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        syslog_ax_eth_port_err("Can't open socket to get qinq-type!\n");
        return INTERFACE_RETURN_CODE_ERROR;
    }
    if_request.cmd = GET_VLAN_QINQ_TYPE_CMD;
	/*printf("into ..... product %d\n",eth_g_index,productId);*/
	for (i = 0 ; i < CHASSIS_SLOT_COUNT; i++ ) {
		unsigned char local_port_count = ETH_LOCAL_PORT_COUNT(i);
		for (j = 0; j < local_port_count; j++ ) {
			unsigned char local_port_no = ETH_LOCAL_INDEX2NO(i,j);
			unsigned int eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(i,j);
			unsigned int slot_no = CHASSIS_SLOT_INDEX2NO(i);


			portInfo = npd_get_port_by_index(eth_g_index);
			if(NULL == portInfo) {
				continue;
			}

			if(NULL == (promiInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS])) {
				continue;
			}
			 
			if(TRUE == npd_eth_port_rgmii_type_check(slot_no,local_port_no)){

				if(promiInfo->subifList.count > 0) {
					list = &(promiInfo->subifList.list);
					__list_for_each(pos,list) {
						subifNode = list_entry(pos,struct eth_port_dot1q_list_s,list);
                        if(!subifNode){
                            continue;
                        }
						if( 60 > *avalidLen) {
							syslog_ax_eth_port_err("mainboard port subif show running oversized,avalid length %d\n", *avalidLen);
							return INTERFACE_RETURN_CODE_ERROR;
						}
                        memset(if_request.device1,0,24);
                        sprintf(if_request.device1,"eth%d-%d.%d",slot_no,local_port_no,subifNode->vid);
						ifnameType = 0;
						if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(if_request.device1,&ifIndex)){
                        	memset(if_request.device1,0,24);
							sprintf(if_request.device1,"e%d-%d.%d",slot_no,local_port_no,subifNode->vid);
							if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(if_request.device1,&ifIndex)){
								syslog_ax_eth_port_err(" not eth regular or shorten ifname interface\n");
								continue;
							}
							ifnameType = 1;
						}
						if (ioctl(fd, SIOCGIFVLAN, &if_request) < 0){/*get the qinq type from kernel*/
                            syslog_ax_eth_port_err("get %s%d-%d.%d qinq-type failed!\n",ifnameType?"e":"eth",slot_no,local_port_no,subifNode->vid);
                            continue;
						}
                        qinqType = (unsigned short)(if_request.u.flag);
                        if(qinqType){
    						length = sprintf((*strShow),"interface %s%d-%d.%d\n config qinq-type %#x\n exit\n",ifnameType?"e":"eth",slot_no,local_port_no,subifNode->vid,qinqType);
    						*strShow = *strShow + length;
    						*avalidLen -= length;
                        }
					}
				}
			}
			else {
				if(promiInfo->subifList.count > 0) {
					list = &(promiInfo->subifList.list);
					__list_for_each(pos,list) {
						subifNode = list_entry(pos,struct eth_port_dot1q_list_s,list);
                        if(!subifNode){
                            continue;
                        }
						if(60 > *avalidLen) {
							syslog_ax_eth_port_err("promiscuous subif show running oversized,avalid length %d\n", *avalidLen);
							return INTERFACE_RETURN_CODE_ERROR;
						}
                        memset(if_request.device1,0,24);
						sprintf(if_request.device1,"eth%d-%d.%d",slot_no,local_port_no,subifNode->vid);
						ifnameType = 0;
						if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(if_request.device1,&ifIndex)){
                        	memset(if_request.device1,0,24);
							sprintf(if_request.device1,"e%d-%d.%d",slot_no,local_port_no,subifNode->vid);
							if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(if_request.device1,&ifIndex)){
								syslog_ax_eth_port_err(" not eth regular or shorten ifname interface\n");
								continue;
							}
							ifnameType = 1;
						}
						if (ioctl(fd, SIOCGIFVLAN, &if_request) < 0){/*get the qinq type from kernel*/
                            syslog_ax_eth_port_err("get %s%d-%d.%d qinq-type failed!\n",ifnameType?"e":"eth",slot_no,local_port_no,subifNode->vid);
                            continue;
						}
                        qinqType = (unsigned short)(if_request.u.flag);
                        if(qinqType){
    						length = sprintf((*strShow),"interface %s%d-%d.%d\n config qinq-type %#x\n exit\n",ifnameType?"e":"eth",slot_no,local_port_no,subifNode->vid,qinqType);
    						*strShow = *strShow + length;
    						*avalidLen -= length;
                        }
					}
				}
			}
		}
	}
	close(fd);
	return INTERFACE_RETURN_CODE_SUCCESS;
}


/**********************************************************************************
 *  npd_get_promis_subif_count
 *
 *	DESCRIPTION:
 * 		set port ve flag
 *
 *	INPUT:
 *            NONE
 *	OUTPUT:
 *		
 * 	RETURN:
 *		Sub if count
 *		
 *
 **********************************************************************************/
unsigned int npd_get_promis_subif_count
(
    void 
)
{
	struct eth_port_s *portInfo = NULL;
	struct eth_port_promi_s* promiInfo = NULL;
	enum product_id_e productId = PRODUCT_ID; 
	int i =0 ,j = 0;
	int count = 0;

	/*printf("into ..... product %d\n",eth_g_index,productId);*/
	for (i = 0 ; i < CHASSIS_SLOT_COUNT; i++ ) {
		unsigned char local_port_count = ETH_LOCAL_PORT_COUNT(i);
		for (j = 0; j < local_port_count; j++ ) {
			unsigned char local_port_no = ETH_LOCAL_INDEX2NO(i,j);
			unsigned int eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(i,j);
			unsigned int slot_no = CHASSIS_SLOT_INDEX2NO(i);


			portInfo = npd_get_port_by_index(eth_g_index);
			if(NULL == portInfo) {
				continue;
			}

			if(NULL == (promiInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS])) {
				continue;
			}

			if(TRUE == npd_eth_port_rgmii_type_check(slot_no,local_port_no)) {
				if(promiInfo->subifList.count > 0) {
					count += promiInfo->subifList.count;
				}
			}
			else {
				if(promiInfo->subifList.count > 0) {
					count += promiInfo->subifList.count;
				}
			}
		}
	}

	/*printf("buf %s\n",buf);*/
	return count;


}


/**********************************************************************************
 *  npd_eth_port_acl_bind_check
 *
 *	DESCRIPTION:
 * 		check if ethernet port has acl service enabled
 *
 *	INPUT:
 *		eth_g_index - ethernet port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE - acl service enabled
 *		NPD_FALSE - acl service disabled
 *		0xFFFFFFFF - if error occurred
 *
 **********************************************************************************/
 unsigned int npd_eth_port_acl_bind_check
(
	unsigned int eth_g_index,
	unsigned int dir_info
) 
{
	struct eth_port_s	*portInfo = NULL;
	struct eth_group_info * groupNode = NULL;
	enum product_id_e productId = PRODUCT_ID; 
	int isBoard = NPD_FALSE;/* 1 -if main board port, 0 - slave board port*/ 
	
	if(((PRODUCT_ID_AX7K_I == productId)||(PRODUCT_ID_AX7K == productId))
		&& (eth_g_index < 4)) {
		isBoard = NPD_TRUE;
		return NPD_FALSE;
	}
	else if((PRODUCT_ID_AX5K_I == productId) && (eth_g_index >= 8) && (eth_g_index <= 11)) {
		isBoard = NPD_TRUE;
		return NPD_FALSE;
	}
	else if((PRODUCT_ID_AX5K_E == productId) || (PRODUCT_ID_AX5608 == productId)) {
		isBoard = NPD_TRUE;
		return NPD_FALSE;
	}

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		syslog_ax_eth_port_err("no port found when check acl group bind by index %#x\n",eth_g_index);
		return ~0UL;
	}
	if(!(portInfo->funcs.funcs_cap_bitmap & (1<< ETH_PORT_FUNC_ACL_RULE))) {
		syslog_ax_eth_port_err("port %#x capability without ACL functionality\n",eth_g_index);
		return NPD_FALSE;
	}
	else if(!(portInfo->funcs.funcs_run_bitmap & (1<< ETH_PORT_FUNC_ACL_RULE))) {
		syslog_ax_eth_port_err("port %#x ACL functionality not eanbled currently\n",eth_g_index);
		return NPD_FALSE;
	}
	else {
		groupNode = portInfo->funcs.func_data[ETH_PORT_FUNC_ACL_RULE];
		if(groupNode!=NULL){
			if(groupNode->direction==ACL_DIRECTION_TWOWAY_E)
				return NPD_TRUE;
			else if((groupNode->direction==ACL_DIRECTION_INGRESS_E)&&(dir_info==ACL_DIRECTION_INGRESS))
				return NPD_TRUE;
			else if((groupNode->direction==ACL_DIRECTION_EGRESS_E)&&(dir_info==ACL_DIRECTION_EGRESS))
				return NPD_TRUE;
			else
				return NPD_FALSE;
		}
		else{
			return NPD_FALSE;
		}
	}	
}
unsigned int npd_eth_port_pcl_bind_check(unsigned int eth_g_index)
{
	unsigned int ret,rs;
	ret = npd_eth_port_acl_bind_check(eth_g_index,ACL_DIRECTION_INGRESS);
	if(ret != NPD_TRUE){
		rs= npd_eth_port_acl_bind_check(eth_g_index,ACL_DIRECTION_EGRESS);
		   if(rs==NPD_TRUE)
		   	  return NPD_TRUE;
		   else
		   	  return NPD_FALSE;		   	  
	}
	else
		return NPD_FALSE;
}

/**********************************************************************************
 *  eth_port_acl_enable
 *
 *	DESCRIPTION:
 * 		acl enable with port bitmap
 *
 *	INPUT:
 *		eth_g_index - ethernet port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR  - port info error
 *		NPD_DBUS_SUCCESS -success set port bitmap
 **********************************************************************************/

unsigned int eth_port_acl_enable
(
	unsigned int eth_g_index,
	unsigned int acl_enable,
	unsigned int dir_info
)
{
	struct   eth_port_s     *portPtr = NULL;
	struct   eth_group_info * groupNode = NULL;
	unsigned int 			ret=1;

	portPtr = npd_get_port_by_index(eth_g_index);
	if(NULL==portPtr){
		syslog_ax_eth_port_dbg("wrong port infomation when search by eth_g_index %d!\n",eth_g_index);
        ret = 1;
	}		
	else{
		if(acl_enable==NPD_TRUE){
			portPtr->funcs.funcs_cap_bitmap |= (1<< ETH_PORT_FUNC_ACL_RULE);
			portPtr->funcs.funcs_run_bitmap |= (1<< ETH_PORT_FUNC_ACL_RULE);
			
			groupNode = portPtr->funcs.func_data[ETH_PORT_FUNC_ACL_RULE];
			if(NULL == groupNode) {
				groupNode = (struct eth_group_info*)malloc(sizeof(struct eth_group_info));
				if(NULL!=groupNode) {
					memset(groupNode,0,sizeof(struct eth_group_info));
					groupNode->direction=ACL_DIRECTION_NONE_DISABLE_E;/*init*/
					if(dir_info==ACL_DIRECTION_INGRESS)
						groupNode->direction= ACL_DIRECTION_INGRESS_E;
					else if(dir_info==ACL_DIRECTION_EGRESS)
						groupNode->direction= ACL_DIRECTION_EGRESS_E;
					syslog_ax_eth_port_dbg("groupNode->direction %d\n",groupNode->direction);
					portPtr->funcs.func_data[ETH_PORT_FUNC_ACL_RULE] = groupNode;
					
				}
			}
			else {
				if(ACL_DIRECTION_NONE_DISABLE_E==(groupNode->direction)){
					if(dir_info==ACL_DIRECTION_INGRESS)
						groupNode->direction= ACL_DIRECTION_INGRESS_E;
					else if(dir_info==ACL_DIRECTION_EGRESS)
						groupNode->direction= ACL_DIRECTION_EGRESS_E;					
				}
				else if(ACL_DIRECTION_NONE_DISABLE_E!=(groupNode->direction)){
				   if(dir_info!=(groupNode->direction)){
					 /*if not eque before state ,do twoway direction*/
					  groupNode->direction = ACL_DIRECTION_TWOWAY_E;
					 printf("groupNode->direction %d\n",groupNode->direction);
				   }
				   else
				   		groupNode->direction =(ACL_DIRECTION_E)dir_info;
				 }
			}
			ret = 0;
		}
		else if(acl_enable==NPD_FALSE){
			
			groupNode = portPtr->funcs.func_data[ETH_PORT_FUNC_ACL_RULE];
			if(NULL == groupNode){
				ret =0;
			}
			else {
				if(dir_info==ACL_DIRECTION_INGRESS){
					if((ACL_DIRECTION_NONE_DISABLE_E==(groupNode->direction))\
						||(ACL_DIRECTION_INGRESS_E==(groupNode->direction)))
					{
						groupNode->direction= ACL_DIRECTION_NONE_DISABLE_E;
					}
					else if((ACL_DIRECTION_EGRESS_E==(groupNode->direction))\
						||(ACL_DIRECTION_TWOWAY_E==(groupNode->direction)))
					{
					     groupNode->direction= ACL_DIRECTION_EGRESS_E;
					}
				}
				else if(dir_info==ACL_DIRECTION_EGRESS){
					if((ACL_DIRECTION_NONE_DISABLE_E==(groupNode->direction))\
						||(ACL_DIRECTION_EGRESS_E==(groupNode->direction)))
					{
						groupNode->direction= ACL_DIRECTION_NONE_DISABLE_E;
					}
					else if((ACL_DIRECTION_INGRESS_E==(groupNode->direction))\
						||(ACL_DIRECTION_TWOWAY_E==(groupNode->direction)))
					{
					     groupNode->direction= ACL_DIRECTION_INGRESS_E;
					}				
			}
			if(ACL_DIRECTION_NONE_DISABLE_E==(groupNode->direction)){
		    	portPtr->funcs.funcs_cap_bitmap |= (0<< ETH_PORT_FUNC_ACL_RULE);
				portPtr->funcs.funcs_run_bitmap |= (0<< ETH_PORT_FUNC_ACL_RULE);
			}
			 
			ret = 0;

		}
	  }
	}
	return  ret;

}

/********************************************************************************
 *  npd_check_eth_port_status
 *
 *	DESCRIPTION:
 * 		check ethernet port link stauts
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FAIL - if some errors occur
 *		ETH_ATTR_LINKUP
 *		ETH_ATTR_LINKDOWN
 *
 *
 **********************************************************************************/
int npd_check_eth_port_status
(
	unsigned int eth_g_index
) 
{
	struct eth_port_s* portInfo = NULL;
	/* for interface vlan on AX81-SMU, zhangdi@autelan.com 2011-07-21 */
	int ret = -1;
	unsigned long linkup = 0;
	unsigned char devNum = 0, portNum = 0;	
    if(BOARD_TYPE == BOARD_TYPE_AX81_SMU)
    {
		/* jump read of portInfo */
    	npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum);
    	ret = nam_get_port_link_state(devNum,portNum,&linkup);
    	if(NPD_SUCCESS != ret) {
    		return NPD_FAIL;
    	}
	  	syslog_ax_eth_port_dbg("get port (%d %d) link status %d\n",devNum,portNum,linkup);	
	    return linkup;	
    }

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL != portInfo){
		if(portInfo->attr_bitmap & ((ETH_ATTR_LINKUP << ETH_LINK_STATUS_BIT) & ETH_ATTR_LINK_STATUS)) {
			return ETH_ATTR_LINKUP;
		}
		else {
			return ETH_ATTR_LINKDOWN;
		}
	}

	return 	NPD_FAIL;
}

/**********************************************************************************
 *  npd_route_port_link_change
 *
 *	DESCRIPTION:
 * 		ethernet port link event process
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		event - port link up/down event
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occur
 *		NPD_FAIL - if some errors occur
 *		
 *
 **********************************************************************************/
 int npd_route_port_link_change
(
	unsigned int	eth_g_index,
	struct eth_port_intf_s* intfPtr,
	enum ETH_PORT_NOTIFIER_ENT	event
)
{

	unsigned int ifindex = ~0UI;
	int ret = 0;
	
	/*syslog_ax_eth_port_dbg(("npd_eth-port1828>> status change event:port %#x link %s\n",eth_g_index,event ? "down":"up"));*/
	npd_intf_set_port_l3intf_status(eth_g_index,event);
	
	if((event == ETH_PORT_NOTIFIER_LINKUP_E)&&\
		(TRUE == npd_eth_port_interface_check(eth_g_index,&ifindex))&&\
		(~0UI != ifindex)){
		ret = npd_arp_snooping_gratuitous_send(ifindex,0,eth_g_index,KAP_INTERFACE_PORT_E);
		npd_syslog_dbg("port interface gratuitous arp send %s,ret %d \n",(0==ret)?"SUCCESS":"FAILED",ret);
		
	}
	return 0;
}

/**********************************************************************************
 *  npd_promi_port_link_change
 *
 *	DESCRIPTION:
 * 		ethernet port link event process
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		event - port link up/down event
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occur
 *		NPD_FAIL - if some errors occur
 *		
 *
 **********************************************************************************/
 int npd_promi_port_link_change
(
	unsigned int	eth_g_index,
	struct eth_port_promi_s* promiPtr,
	enum ETH_PORT_NOTIFIER_ENT	event
)
{
	unsigned int result = NPD_SUCCESS;
	unsigned char devNum = 0, portNum = 0;
	unsigned int slot_no = 0, slot_index = 0;
	unsigned int local_port_no = 0,port_index = 0;
	ve_netdev_priv ve_promis_param;
	unsigned int cmd = 0;
	unsigned int ifIndex = ~0UI;

	if(NULL == promiPtr) return NPD_FAIL;
		
	if(ETH_PORT_NOTIFIER_LINKUP_E == event) {
		cmd = CVM_IOC_DEV_UP;
	}
	else if(ETH_PORT_NOTIFIER_LINKDOWN_E == event) {
		cmd = CVM_IOC_DEV_DOWN;
	}
	else {
		syslog_ax_eth_port_err("promiscous port %#x ignore link event %d\n",  \
						eth_g_index, event);
		return NPD_FAIL;
	}
	result = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
	if(NPD_SUCCESS != result) {
		syslog_ax_eth_port_err("promiscous port %#x link %s convert to asic port error\n",  \
						eth_g_index, (ETH_PORT_NOTIFIER_LINKUP_E == event)? "UP":"DOWN");
		return NPD_FAIL;
	}
	slot_index 		= SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	slot_no			= CHASSIS_SLOT_INDEX2NO(slot_index);
	port_index 		= ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	local_port_no 	= ETH_LOCAL_INDEX2NO(slot_index,port_index);

	memset(&ve_promis_param,0,sizeof(ve_netdev_priv));
	ve_promis_param.devNum = devNum;
	ve_promis_param.portNum = portNum;
	sprintf(ve_promis_param.name,"eth%d-%d",slot_no,local_port_no);
	if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ve_promis_param.name,&ifIndex)){
		memset(ve_promis_param.name,0,15);
		sprintf(ve_promis_param.name,"e%d-%d.%d",slot_no,local_port_no);
		if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ve_promis_param.name,&ifIndex)){
			syslog_ax_eth_port_err(" not eth regular or shorten ifname interface\n");
			return NPD_FAIL;;
		}
	}

	result = cavim_do_intf(cmd, &ve_promis_param);
	if (0 != result) {
		syslog_ax_eth_port_err("promiscuous port %#x link %s tell kmod error\n",  \
					eth_g_index, (ETH_PORT_NOTIFIER_LINKUP_E == event)? "UP":"DOWN");
		return NPD_FAIL;
	} 	
	
	return 0;
}

/**********************************************************************************
 *  npd_eth_port_get_vlan_all
 *
 *	DESCRIPTION:
 * 		This method gets all bridge vlans and dot1q vlans this port belongs to.
 *	bridge vlans stands for vlan the eth-port belongs to untagged, and 
 *  dot1q vlans for vlan the eth-port belongs to tagged.
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *		brgVid - bridge vlans' vid list
 *		dot1qVid - dot1q vlans' vid list
 *		brgVlanCnt - maximum supported # of bridge vlans
 *		dot1qVlanCnt - maximum supported # of dot1q vlans
 *	
 *	OUTPUT:
 *		brgVlanCnt - # of bridge vlans
 *		dot1qVlanCnt - # of dot1q vlans
 *
 * 	RETURN:
 *			NPD_OK - if no failure occurs
 *			NPD_FAIL - if get vlan list failed
 *
 **********************************************************************************/
int npd_eth_port_get_vlan_all
(
	unsigned int eth_g_index,
	unsigned short brgVid[],
	unsigned short dot1qVid[],
	unsigned int *brgVlanCnt,
	unsigned int *dot1qVlanCnt
)
{
	struct eth_port_s* portInfo = NULL;
	struct port_based_vlan_s *untagVlan= NULL;
	dot1q_vlan_list_s *dot1Vlanlst = NULL;
	struct eth_port_dot1q_list_s* node = NULL;
	struct list_head * list = NULL,*pos = NULL;
	unsigned int totalBrgVCnt = 0, totalDotVCnt = 0;
	unsigned int actualBrgVCnt = 0, actualDotVCnt = 0;
	unsigned short *vidPtr = NULL;

	totalBrgVCnt = *brgVlanCnt;
	totalDotVCnt = *dot1qVlanCnt;

	syslog_ax_eth_port_dbg("totalBrgVCnt = %d,totalDotVCnt = %d\n",totalBrgVCnt,totalDotVCnt);
	portInfo = npd_get_port_by_index(eth_g_index);
	if(!portInfo) {
		syslog_ax_eth_port_err("get port %#x all vlan list found port null\n",eth_g_index);
		return NPD_FAIL;
	}

	/* check if vid array have enough space*/
	if(totalBrgVCnt <= 0 || totalDotVCnt < NPD_VLAN_NUMBER_MAX) {
		syslog_ax_eth_port_err("get port %#x all vlan list with not enough vid array space %d(u) %d(t)\n",	\
				eth_g_index, totalBrgVCnt, totalDotVCnt);
		return NPD_FAIL;
	}
	
	/* get all bridge vlans*/
	vidPtr = brgVid;
	untagVlan = (struct port_based_vlan_s *)(portInfo->funcs.func_data[ETH_PORT_FUNC_PORT_BASED_VLAN]); 
	if(untagVlan){
		vidPtr[0] = untagVlan->vid;
		actualBrgVCnt = 1;
	}
	
	/* get all dot1q vlans*/
	vidPtr = dot1qVid;
	dot1Vlanlst = (dot1q_vlan_list_s *)(portInfo->funcs.func_data[ETH_PORT_FUNC_DOT1Q_BASED_VLAN]);
	if(dot1Vlanlst){
		list = &(dot1Vlanlst->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct eth_port_dot1q_list_s,list);
			if(node){
				vidPtr[actualDotVCnt] = node->vid;
				actualDotVCnt++;
			}
		}
	}
	syslog_ax_eth_port_dbg("actualBrgVCnt = %d,actualDotVCnt = %d\n",actualBrgVCnt,actualDotVCnt);

	*brgVlanCnt = actualBrgVCnt;
	*dot1qVlanCnt = actualDotVCnt;

	return NPD_OK;
}
/**********************************************************************************
 *  npd_eth_port_notifier_for_ax81_smu
 *
 *	DESCRIPTION:
 * 		ethernet port link event process  just for  master board
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		event - port link up/down event
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occur
 *		NPD_FAIL - if some errors occur
 *		
 *
 **********************************************************************************/

int npd_eth_port_notifier_for_ax81_smu(unsigned int	eth_g_index,enum ETH_PORT_NOTIFIER_ENT	event)
{
	int ret = NPD_FAIL;
	unsigned char devNum=0;
	unsigned char portNum = 0;
	unsigned short trunkId = 0;
	unsigned long member = 0;
	unsigned int port_state = 0;
	unsigned long attr;

	ret = npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum);
	if(ret != NPD_OK)
	{
		syslog_ax_eth_port_err("get devport by global index err ! \n");
		return NPD_FAIL;
	}
	

	ret = nam_trunk_port_trunkId_get(devNum,portNum,&member,&trunkId);
	if(ret != NPD_OK)
	{
		syslog_ax_eth_port_err("get trunk ID err !\n");
		return ret;
	}
	syslog_ax_eth_port_dbg("member = %d,trunkId = %d\n",member,trunkId);

	ret = nam_get_port_link_state(devNum,portNum,&attr);
	if(ret != NPD_OK)
	{
		syslog_ax_eth_port_err("get devNum %d,portNum %d linkstatus err !\n",devNum,portNum);
	}

	if(attr == event)
	{
		syslog_ax_eth_port_dbg("devNum %d,portNum %d linkstatus didn't change !\n");
		return NPD_OK;
	}
	else
	{	
		if((member != 0) && ((trunkId >=111) || (trunkId <=118)))
		{
			if(event == ETH_PORT_NOTIFIER_LINKUP_E)
			{
				ret = nam_asic_trunk_port_endis(devNum,portNum,trunkId,1);
				if(ret != NPD_OK)
				{
					syslog_ax_eth_port_err("enable devNum %d,port %d fail !\n",devNum,portNum);
				}
				else
				{
					syslog_ax_eth_port_err("enable devNum %d,port %d success !\n",devNum,portNum);
				}
			}
			else if(event == ETH_PORT_NOTIFIER_LINKDOWN_E)
			{
				ret = nam_asic_trunk_port_endis(devNum,portNum,trunkId,0);
				if(ret != NPD_OK)
				{
					syslog_ax_eth_port_err("disable devNum %d,port %d fail !\n",devNum,portNum);
				}
				else
				{
					syslog_ax_eth_port_err("disable devNum %d,port %d success !\n",devNum,portNum);
				}
			}
		}
	}

	return ret;
}

/**********************************************************************************
 *  npd_eth_port_notifier
 *
 *	DESCRIPTION:
 * 		ethernet port link event process
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		event - port link up/down event
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occur
 *		NPD_FAIL - if some errors occur
 *		
 *
 **********************************************************************************/
int npd_eth_port_notifier
(
	unsigned int	eth_g_index,
	enum ETH_PORT_NOTIFIER_ENT	event
)
{
	struct eth_port_s 	*ethPort = NULL;
	struct eth_port_s    portInfo;
	struct stp_info_s	*stpInfo = NULL;
	struct port_based_vlan_s *portVlan = NULL;
	dot1q_vlan_list_s *dot1qVlan = NULL;
	struct eth_port_intf_s* intfPtr = NULL;
	struct eth_port_promi_s* promiPtr = NULL;
	struct port_based_trunk_s* portTrunk = NULL;
	struct trunk_s *trunkNode = NULL;
	enum module_id_e module_type;
	int i = 0,ret = 0, old_state = 0;
	enum eth_port_type_e portMedia = ETH_INVALID;
	unsigned int arpCount = 0, new_state = 0;
	unsigned int trunkid = 0,trunk_flag = 0;/*identifier that if a trunk member	*/
	unsigned int LinkUpTimes = 0,LinkDownTimes = 0;
	struct timeval tnow;
	struct tm tzone;
	unsigned int slot_no = 0, slot_index = 0;
	unsigned int port_no = 0, eth_l_index = 0;
	unsigned long last_link_change, link_keep_time;
    struct sysinfo info = {0};      /* for mobile test */
	memset(&portInfo,0,sizeof(struct eth_port_s));	/* code optimize: Uninitialized scalar variable houxx@autelan.com  2013-1-17 */
	
	/*by zhangcl@autelan.com on 01/31/2012 - to  force trunk port event report*/
	if(BOARD_TYPE == BOARD_TYPE_AX81_SMU)
	{
		ret = npd_eth_port_notifier_for_ax81_smu(eth_g_index,event);
		if(ret != NPD_SUCCESS)
		{
			syslog_ax_eth_port_err("eth port nogifier for ax81 smu err !\n");
		}
	}
	ethPort = npd_get_port_by_index(eth_g_index);
	if(NULL == ethPort) {
		syslog_ax_eth_port_err("link status change:index %#x node null\n",eth_g_index);
		return NPD_FAIL;
	}

	/*by qinhs@autelan.com on 10/24/2008 - to avoid duplicated link event report*/
	old_state = npd_check_eth_port_status(eth_g_index);
	ret = npd_get_port_link_status(eth_g_index,&new_state);
	if(NPD_SUCCESS != ret) {
		syslog_ax_eth_port_err("link status change get port %#x state error\n",eth_g_index);
		return NPD_FAIL;
	}
	if((old_state == new_state) &&(ETH_PORT_NOTIFIER_LINKPOLL_E == event)) {
		/* fix up COMBO port LED interface*/
		if(ETH_ATTR_LINKUP == new_state) {
			ret = npd_port_media_get(eth_g_index,&portMedia);
			if(NPD_TRUE == ret){
				if(portMedia != ethPort->port_type) {
					syslog_ax_eth_port_evt("poll found port %#x stable link up port type change %d -> %d\n", \
						eth_g_index,ethPort->port_type,portMedia);
				}
				ethPort->port_type = portMedia;
			}
		}
		return NPD_SUCCESS;
	}
	else if(old_state == new_state) { /* event LINKDOWN or LINKUP*/
		syslog_ax_eth_port_err("link %s event but port %#x state %s with no change\n",	\
			(ETH_PORT_NOTIFIER_LINKUP_E == event) ? "UP":"DOWN",eth_g_index,	\
			(ETH_ATTR_LINKUP == old_state) ? "UP":"DOWN");
	}
	else if(ETH_PORT_NOTIFIER_LINKPOLL_E == event) { /*find state change when polling*/
		event = (ETH_ATTR_LINKUP == new_state) ? ETH_PORT_NOTIFIER_LINKUP_E :
				 (ETH_ATTR_LINKDOWN == new_state) ? ETH_PORT_NOTIFIER_LINKDOWN_E : event;
		syslog_ax_eth_port_dbg("poll found port %#x link change %s\n",	\
				eth_g_index, (ETH_ATTR_LINKUP == new_state) ? "UP":"DOWN");
	}

	/* event not match with actual port status*/
	if(((ETH_ATTR_LINKUP == new_state) && (ETH_PORT_NOTIFIER_LINKDOWN_E == event))||
			 ((ETH_ATTR_LINKDOWN == new_state) && (ETH_PORT_NOTIFIER_LINKUP_E == event))){
		syslog_ax_eth_port_err("link %s event but port %#x current state %s error\n",		\
			(ETH_PORT_NOTIFIER_LINKUP_E == event) ? "UP":"DOWN",eth_g_index,	\
			(ETH_ATTR_LINKUP == new_state) ? "UP":"DOWN");
	}

    sysinfo(&info);
	gettimeofday (&tnow, &tzone);
	if(tnow.tv_sec - ethPort->lastLinkChange < 2) {
		syslog_ax_eth_port_evt("port %#x link change so fast (%s)\n", 	\
					eth_g_index, (ETH_PORT_NOTIFIER_LINKUP_E == event) ? "up":"down");
		/* wait for awhile to avoid link down and then up so fast*/
		if(ETH_PORT_NOTIFIER_LINKUP_E == event) {
			sleep(4);
		}
	}
	ethPort->lastLinkChange = tnow.tv_sec;

	slot_no = CHASSIS_SLOT_INDEX2NO(SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index));
	port_no = ETH_LOCAL_INDEX2NO(SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index),ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index));

	/* Added by Jia Lihui 2011-6-20 */
	if(PRODUCT_ID == PRODUCT_ID_AX7K_I || SYSTEM_TYPE == IS_DISTRIBUTED)
	{
    	last_link_change  = ETH_PORT_LINK_TIME_CHANGE_FROM_GLOBAL_INDEX(eth_g_index);
    	module_type	= MODULE_TYPE_ON_SLOT_INDEX(slot_index);	 
    	slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
    	eth_l_index	= ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
    		 
    	ret = nam_read_eth_port_info(module_type,slot_no,port_no,&portInfo);
    	if(NPD_SUCCESS != ret) {
    		syslog_ax_eth_port_err("read ASIC port(%d %d) infomation error.\n",slot_no, port_no);
    	}	
    	
    	link_keep_time = tnow.tv_sec;		
    	syslog_ax_eth_port_dbg("port(%d %d) port_type %d\n",slot_no,port_no, portInfo.port_type);
    	syslog_ax_eth_port_dbg("port %d/%d attr get time %u. \n",slot_no,port_no,link_keep_time);

    	start_fp[slot_index][port_no-1].attr_bitmap = portInfo.attr_bitmap;

		/* The media type doesn't change while the link event is occurring at present. 2011-9-22 Jia Lihui */
		/* start_fp[slot_index][port_no-1].port_type = ethPort->port_type; */
		
    	start_fp[slot_index][port_no-1].linkchanged += 1;
    	start_fp[slot_index][port_no-1].lastLinkChange = info.uptime;

        /* sync the change to ram */
        msync(start_fp[slot_index], sizeof(struct global_ethport_s)*BOARD_GLOBAL_ETHPORTS_MAXNUM, MS_SYNC);		
    	npd_asic_ehtport_update_notifier(eth_g_index);
	}
	syslog7_ax_eth_port_notice("ethernet port %d/%d link %s\n",
						slot_no, port_no, (ETH_PORT_NOTIFIER_LINKUP_E == event) ? "up" : "down");
	
	/* all link conditions have been checked, start treat link event */
	if(ETH_PORT_NOTIFIER_LINKUP_E == event) {
		ret = npd_port_media_get(eth_g_index,&portMedia);
		if(NPD_TRUE == ret){
			syslog_ax_eth_port_dbg("eth_g_index::%d,port_type %d.\n",eth_g_index,portMedia);
			ethPort->port_type = portMedia;
		}
		ethPort->counters.linkupcount = ethPort->counters.linkupcount + 1;
		/*syslog_ax_eth_port_dbg("************ethPort->counters.linkupcount %d\n",ethPort->counters.linkupcount);*/
		ethPort->attr_bitmap |= ((ETH_ATTR_LINKUP << ETH_LINK_STATUS_BIT) & ETH_ATTR_LINK_STATUS);
	}
	else if(ETH_PORT_NOTIFIER_LINKDOWN_E == event) {
		ethPort->counters.linkdowncount = ethPort->counters.linkdowncount + 1;
		/*syslog_ax_eth_port_dbg("**************ethPort->counters.linkdowncount %d\n",ethPort->counters.linkdowncount);	*/	
		ethPort->attr_bitmap &= ~((ETH_ATTR_LINKUP << ETH_LINK_STATUS_BIT) & ETH_ATTR_LINK_STATUS);
	}
	else {
		syslog_ax_eth_port_err("port %#0x link event %d error\n",eth_g_index,event);
		return NPD_SUCCESS;
	}

	/*
	* clear arp info on this port
	* when clear arp,interface have down.
	* so del arp error.
	* now first del arp , second interface down.
	*/

	if(ETH_PORT_NOTIFIER_LINKDOWN_E == event) {
		arpCount = npd_arp_clear_by_port(eth_g_index);
		syslog_ax_eth_port_dbg("npd clear arp on port %#x total %d item cleared\n",eth_g_index,arpCount);

		/*check the port if a trunk member*/
		for(trunkid = 1;trunkid <= CHASSIS_TRUNK_RANGE_MAX;trunkid++){
           ret = npd_trunk_check_port_membership(trunkid,eth_g_index);
		   if(NPD_TRUE == ret){
		   	  trunk_flag = 1;
			  break;
		   }
		}
		/*if not member of one trunk,clear fdb items*/
		if(!trunk_flag){
			ret =nam_fdb_table_delete_entry_with_port(eth_g_index);
			syslog_ax_eth_port_err("link down-fdb delete with port %d::return value %d\n",eth_g_index,ret);
		}
	}
	

	for(i = 0; i < ETH_PORT_FUNC_MAX; i++) {
		if(ethPort->funcs.funcs_run_bitmap & (1 << i)) {
			if(NULL != ethPort->funcs.func_data[i]) {
				switch (i) {
					case ETH_PORT_FUNC_PORT_BASED_VLAN:	/*untagged vlan*/
					    syslog_ax_eth_port_dbg("switch Func %d\n",i);
						portVlan = (struct port_based_vlan_s *)(ethPort->funcs.func_data[ETH_PORT_FUNC_PORT_BASED_VLAN]);
						if(NULL != portVlan) {
							if(NULL != portVlan->npd_vlan_untagports_notifier_func) {
								syslog_ax_eth_port_dbg("vlan untag port status change event:port %#x link %s\n",eth_g_index,event ? "down":"up");
								portVlan->npd_vlan_untagports_notifier_func(eth_g_index,portVlan,event);
							}
						}
						syslog_ax_eth_port_dbg("ETH_PORT_FUNC_PORT_BASED_VLAN dealt over!\n");
						break;
					case ETH_PORT_FUNC_DOT1Q_BASED_VLAN: /*tagged vlan*/
					    syslog_ax_eth_port_dbg("switch Func %d\n",i);
						dot1qVlan = (dot1q_vlan_list_s *)(ethPort->funcs.func_data[ETH_PORT_FUNC_DOT1Q_BASED_VLAN]);
						if(NULL != dot1qVlan) {
							if(NULL != dot1qVlan->npd_link_change_notifier_func) {
								syslog_ax_eth_port_dbg("vlan tag ports status change event:port %#x link %s\n",eth_g_index,event ? "down":"up");
								dot1qVlan->npd_link_change_notifier_func(eth_g_index,dot1qVlan,event);
							}
						}
						break;
					case ETH_PORT_FUNC_PROTOCOL_BASED_VLAN:	/*protocol-based vlan*/
						break;
					case ETH_PORT_FUNC_SUBNET_BASED_VLAN:			/*subnet-based vlan*/
						break;
					case ETH_PORT_FUNC_DOT1AD_EDGE: /* to be studied later on provider bridge functions.*/
						break;
					case ETH_PORT_FUNC_BRIDGE:		/*switch mode*/
						break;
					case ETH_PORT_FUNC_DOT1W:	/*spanning-tree protocol*/ 
					    syslog_ax_eth_port_dbg("switch Func %d\n",i);
					    if(old_state == new_state){
						  syslog_ax_eth_port_dbg("spanning-tree protocol status change event::eth port %#x link %s,the same as old state no change\n",eth_g_index,event ? "down":"up");
                           break;
						}
						stpInfo = (struct stp_info_s *)(ethPort->funcs.func_data[ETH_PORT_FUNC_DOT1W]);
						if(NULL != stpInfo) {
							if(NULL != stpInfo->npd_stp_port_notifier_func) {
								syslog_ax_eth_port_dbg("spanning-tree protocol status change event:port %#x link %s\n",eth_g_index,event ? "down":"up");
								stpInfo->npd_stp_port_notifier_func(eth_g_index,stpInfo,event);
							}

						}

						break;
					case ETH_PORT_FUNC_VLAN_TRANSLATION:
						break;
					case ETH_PORT_FUNC_LINK_AGGREGATION:			/*trunk*/
					    syslog_ax_eth_port_dbg("switch Func %d\n",i);
						portTrunk = (struct port_based_trunk_s*)(ethPort->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION]);
						if(NULL != portTrunk){
							if(NULL != portTrunk->npd_trunk_ports_notifier_func){
								syslog_ax_eth_port_dbg("trunk port member status change event:port %#x link %s\n",eth_g_index,event ? "down":"up");
								portTrunk->npd_trunk_ports_notifier_func(eth_g_index,portTrunk,event);
							}
						}
						break;
					case ETH_PORT_FUNC_IPV4:						/* L3 interface route mode*/
					    syslog_ax_eth_port_dbg("switch Func %d\n",i);
						intfPtr = (struct eth_port_intf_s *)(ethPort->funcs.func_data[ETH_PORT_FUNC_IPV4]);
						if(NULL != intfPtr) {
							if(NULL != intfPtr->npd_route_port_notifier_func) {
								syslog_ax_eth_port_dbg("L3 interface route mode status change event:port %#x link %s\n",eth_g_index,event ? "down":"up");
								intfPtr->npd_route_port_notifier_func(eth_g_index,intfPtr,event);
							}

						}
						break;
					case ETH_PORT_FUNC_ACL_RULE:					/*ACL*/
						break;
					case ETH_PORT_FUNC_QoS_PROFILE:				/*QoS profile*/
						break;
					case ETH_PORT_FUNC_MODE_PROMISCUOUS:          /*pve profile*/ 
						promiPtr = (struct eth_port_promi_s *)(ethPort->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS]);
						if(NULL != promiPtr) {
							if(NULL != promiPtr->npd_promi_port_notifier_func) {
								syslog_ax_eth_port_dbg("promiscuous port %#x link %s event\n",eth_g_index,event ? "down":"up");
								promiPtr->npd_promi_port_notifier_func(eth_g_index,promiPtr,event);
							}
						}
						break;
					default:
						syslog_ax_eth_port_dbg("npd link change error\n");
						break;
				}
				
			}
			else {
				continue;
			}
		}
		else {
			continue;
		}
	}
	/* add by hanhui    2008/11/25  for static arp valid set when port up/down*/
	if(ETH_PORT_NOTIFIER_LINKUP_E == event){
	    npd_static_arp_validate_by_port(eth_g_index);
	}
	else if(ETH_PORT_NOTIFIER_LINKDOWN_E == event){
		npd_static_arp_invalidate_by_port(eth_g_index);
	}
	return NPD_SUCCESS;
}

/**********************************************************************************
 *  npd_eth_ports_on_all_vlan
 *
 *	DESCRIPTION:
 * 		set vlans ,consist of the port ,attribute.		
 *
 *	INPUT:
 *		eth_g_index - index of port's mode 
 *		dev   - device NO.
 *		port  - port NO.
 *		enable - en/disable device some attribute
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *			NPD_DBUS_SUCCESS
 *			NPD_DBUS_ERROR
 *
 **********************************************************************************/
int npd_eth_ports_on_all_vlan
(
	unsigned int eth_g_index,
	unsigned char dev,
	unsigned int flag
)
{
	struct eth_port_s* portInfo = NULL;
	struct eth_port_dot1q_list_s* node = NULL;
	dot1q_vlan_list_s *dot1Vlanlst = NULL;
	struct list_head * list = NULL,*pos = NULL;
	unsigned int ret = 0;

	portInfo = npd_get_port_by_index(eth_g_index);
	if(!portInfo)
		return 0;
	
	dot1Vlanlst = (dot1q_vlan_list_s *)(portInfo->funcs.func_data[ETH_PORT_FUNC_DOT1Q_BASED_VLAN]);
	if(dot1Vlanlst){
		list = &(dot1Vlanlst->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct eth_port_dot1q_list_s,list);
			if(node){
				ret = nam_learning_state_set_na_secur_enable(dev,node->vid,flag);
				if(0 != ret ){
					return NPD_DBUS_ERROR;
				}
			}
		}
	}

	syslog_ax_eth_port_dbg("call nam_learning_state_set_na_secur_enable return\n");
	return 0;
}

unsigned int npd_eth_port_count_of_bmp(unsigned int * portBmp){
	int i = 0;
	int count = 0;
    for(i = 0;i<64;i++){
		if(portBmp[i/32]&(1<<(i%32))){
		    count++;
		}
    }
	return count;
}

/**********************************************************************************
 *  npd_set_cscd_promi_attr
 *
 *	DESCRIPTION:
 * 		set promiscuous port attribute.
 *
 *	INPUT:
 *		eth_g_index - index of port's mode 
 *		dev   - device NO.
 *		port  - port NO.
 *		enable - en/disable device some attribute
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *			NPD_DBUS_SUCCESS
 *			NPD_DBUS_ERROR
 *
 **********************************************************************************/
int npd_set_cscd_promi_attr
(
	unsigned short vid,
	unsigned char dev,
	unsigned char port,
	unsigned int enable
)
{
	unsigned int flag = NPD_FALSE;
	int ret = NPD_DBUS_ERROR;
	flag = (enable) ? NPD_FALSE : NPD_TRUE;
	unsigned int promisPortBmp[2] = {0};
	
	ret = nam_set_promi_attr_on_pvid(dev,port,enable,vid,flag);
	if(NPD_SUCCESS != ret){
		return NPD_DBUS_ERROR;
	}
#if 0
	/*all tag port in vlan*/
	if(0 != npd_eth_ports_on_all_vlan(eth_g_index,dev,flag))
		return NPD_DBUS_ERROR;
#endif
	/*
	*		Sets state of port  learning  on specified device.
	*/
	if(0 != nam_fdb_enable_cscd_port_auto_learn_set(dev,port,enable)){
		return NPD_DBUS_ERROR;
	}

	ret = nam_set_promi_attr_on_all_vlan(PRODUCT_ID, 0,dev,flag);
	if(NPD_SUCCESS != ret){
		return NPD_DBUS_ERROR;
	}

	return NPD_SUCCESS;
}


/**********************************************************************************
 *  npd_set_promi_attr
 *
 *	DESCRIPTION:
 * 		set promiscuous port attribute.
 *
 *	INPUT:
 *		eth_g_index - index of port's mode 
 *		dev   - device NO.
 *		port  - port NO.
 *		enable - en/disable device some attribute
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *			NPD_DBUS_SUCCESS
 *			NPD_DBUS_ERROR
 *
 **********************************************************************************/
int npd_set_promi_attr
(
	unsigned int eth_g_index,
	unsigned int enable
)
{
	unsigned short vid = 0;
	unsigned int flag = NPD_FALSE;
	int ret = NPD_DBUS_ERROR;
	flag = (enable) ? NPD_FALSE : NPD_TRUE;
	unsigned int promisPortBmp[2] = {0};
	unsigned char dev = 0;
	unsigned char port = 0;
	
	if(0 !=  npd_eth_port_get_pvid(eth_g_index,&vid))
		return NPD_DBUS_ERROR;
	
	npd_get_devport_by_global_index(eth_g_index,&dev,&port);
	ret = nam_set_promi_attr_on_pvid(dev,port,enable,vid,flag);
	if(NPD_SUCCESS != ret){
		return NPD_DBUS_ERROR;
	}
	/*all tag port in vlan*/
	if(0 != npd_eth_ports_on_all_vlan(eth_g_index,dev,flag))
		return NPD_DBUS_ERROR;
		/*
	*		Sets state of port  learning  on specified device.
	*/
	if(0 != nam_fdb_enable_port_auto_learn_set(eth_g_index,enable)){
		return NPD_DBUS_ERROR;
	}

	npd_vlan_get_promis_port_bmp(promisPortBmp);
	if( npd_eth_port_count_of_bmp(promisPortBmp) <= 1){
		syslog_ax_eth_port_dbg("get port count %d promisPortBmp %#x\n",npd_eth_port_count_of_bmp(promisPortBmp),promisPortBmp);
		ret = nam_set_promi_attr_on_all_vlan(PRODUCT_ID, eth_g_index,dev,flag);
		if(NPD_SUCCESS != ret){
			return NPD_DBUS_ERROR;
		}
	}
	else {
		syslog_ax_eth_port_dbg("get port count %d\n",npd_eth_port_count_of_bmp(promisPortBmp));
	}

	return NPD_SUCCESS;
}

/**********************************************************************************
 *  npd_set_cscd_port_ipaddr
 *
 *	DESCRIPTION:
 * 		set cscd port ip address.
 *
 *	INPUT:
 *	ipaddr - ip address want to add	
 *	port - witch port want to add
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *			NPD_DBUS_SUCCESS
 *			NPD_DBUS_ERROR
 *
 **********************************************************************************/
int npd_set_cscd_port_ipaddr
(
	unsigned int ipaddr,
	unsigned char port
)
{
	int fd = 0;
	struct sockaddr_nl	local;	
	struct {
		struct nlmsghdr 	n;
		struct ifaddrmsg 	ifa;
		char   			buf[256];
	} req;
	unsigned char *ifname = NULL;
	unsigned int seq = 0,maxlen =0,len = 0;
	int status = 0;				/* code optimize:Unsigned compared against 0 houxx@autelan.com  2013-1-17 */
	struct rtattr *rta = NULL;
	struct nlmsghdr *ns = NULL;
	unsigned char   buf[8192];	
	struct nlmsghdr *h = NULL;
	extern int errno;
    if((PRODUCT_ID_AX7K_I == PRODUCT_ID)&&((0 == LOCAL_CHASSIS_SLOT_NO)||(1 == LOCAL_CHASSIS_SLOT_NO)))
	{
		if (0 == port)
		{
			ifname = "obc0";
		}
		else
		{
			ifname = "obc1";
		}
    }
	else
	{
		if(0 == port)
		{
			ifname = "cscd0";
		}
		else if(1 == port)
		{
			ifname = "cscd1";
		}
		else
		{				/* code optimize: Explicit null dereferenced  0 houxx@autelan.com  2013-1-17 */
			syslog_ax_eth_port_dbg("Cannot open netlink socket!");
			return NPD_FAIL;			
		}
	}
	/*get the fd add bind to local*/
	fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if (fd < 0)
	{
		syslog_ax_eth_port_dbg("Cannot open netlink socket!");
		return NPD_FAIL;
	}

	memset(&local, 0, sizeof(local));
	local.nl_family = AF_NETLINK;
	local.nl_groups = 0;

	if (bind(fd, (struct sockaddr*)&local, sizeof(local)) < 0)
	{
		syslog_ax_eth_port_dbg("Cannot bind netlink socket");
		close(fd);
		return NPD_FAIL;
	}
	seq = time(NULL);

	/*set the reg struct,add ipaddr etc.*/
	req.n.nlmsg_len		= NLMSG_LENGTH(sizeof(struct ifaddrmsg));
	req.n.nlmsg_flags	= NLM_F_REQUEST;
	req.n.nlmsg_type	= RTM_NEWADDR;
	req.ifa.ifa_family	= AF_INET;
	req.ifa.ifa_index	= if_nametoindex(ifname);
	req.ifa.ifa_prefixlen	= CSCD_IP_MASK;

	ipaddr = htonl( ipaddr );

	maxlen = sizeof(req);
	len = RTA_LENGTH(sizeof(ipaddr));

	if (NLMSG_ALIGN(req.n.nlmsg_len) + len > maxlen)
	{
		syslog_ax_eth_port_dbg("Something wrong with the len!");
		close(fd);
		return NPD_FAIL;
	}
	ns = &req.n;
	rta = (struct rtattr*)(((char*)ns) + NLMSG_ALIGN(req.n.nlmsg_len));
	rta->rta_type = IFA_LOCAL;
	rta->rta_len = len;
	memcpy(RTA_DATA(rta), &ipaddr, sizeof(ipaddr));
	req.n.nlmsg_len = NLMSG_ALIGN(req.n.nlmsg_len) + len;

	/*send msg to kernel*/
	struct sockaddr_nl nladdr;
	struct iovec iov = { (void*)ns, req.n.nlmsg_len };
	struct msghdr msg = {
		(void*)&nladdr, sizeof(nladdr),
		&iov,	1,
		NULL,	0,
		0
	};

	memset(&nladdr, 0, sizeof(nladdr));
	nladdr.nl_family = AF_NETLINK;
	nladdr.nl_pid = 0;
	nladdr.nl_groups = 0;

	req.n.nlmsg_seq = ++seq;
	req.n.nlmsg_flags |= NLM_F_ACK;

	status = sendmsg(fd, &msg, 0);
	if (status < 0) 
	{
		syslog_ax_eth_port_dbg("npd set cscd port ipaddr Cannot talk to rtnetlink, status %d.",status);
		close(fd);
		return NPD_FAIL;
	}
	/*receive meg from kernel*/
	iov.iov_base = buf;
	iov.iov_len = sizeof(buf);

	while (1)
	{
		status = recvmsg(fd, &msg, 0);

		if (status < 0) 
		{
			if (errno == EINTR)
			{
				continue;
			}
			syslog_ax_eth_port_dbg("npd set cscd port ipaddr OVERRUN!");
			continue;
		}
		if (status == 0) 
		{
			syslog_ax_eth_port_dbg("npd set cscd port ipaddr EOF on netlink.\n");
			close(fd);/* code optimize: recourse leak houxx@autelan.com  2013-1-17 */
			return NPD_FAIL;
		}
		if (msg.msg_namelen != sizeof(nladdr)) {
			syslog_ax_eth_port_dbg("npd set cscd port ipaddr sender address length == %d\n", msg.msg_namelen);
			close(fd);/* code optimize: recourse leak houxx@autelan.com  2013-1-17 */
			exit(1);
		}
		for (h = (struct nlmsghdr*)buf; status >= sizeof(*h); )
		{
			int err;
			int len = h->nlmsg_len;
			pid_t pid=h->nlmsg_pid;
			int l = len - sizeof(*h);
			unsigned seq=h->nlmsg_seq;

			if (l<0 || len>status)
			{
				if (msg.msg_flags & MSG_TRUNC) 
				{
					syslog_ax_eth_port_dbg("npd set cscd port ipaddr Truncated message\n");
					close(fd);/* code optimize: recourse leak houxx@autelan.com  2013-1-17 */
					return NPD_FAIL;
				}
				syslog_ax_eth_port_dbg("npd set cscd port ipaddr malformed message: len=%d\n", len);
				close(fd);/* code optimize: recourse leak houxx@autelan.com  2013-1-17 */
				exit(1);
			}

			if (h->nlmsg_pid != pid || h->nlmsg_seq != seq)
			{
				continue;
			}

			if (h->nlmsg_type == NLMSG_ERROR) 
			{
				struct nlmsgerr *err = (struct nlmsgerr*)NLMSG_DATA(h);
				if (l < sizeof(struct nlmsgerr)) 
				{
					syslog_ax_eth_port_dbg("npd set cscd port ipaddr ERROR truncated\n");
					close(fd);			/* code optimize: recourse leak houxx@autelan.com  2013-1-17 */
					return NPD_FAIL;
				} 
				else 
				{
					errno = -err->error;
					if (errno == 0) 
					{
						close(fd);		/* code optimize: recourse leak houxx@autelan.com  2013-1-17 */
						return NPD_SUCCESS;
					}
					/* Scott checked for EEXIST and return 0! 9-4-02*/
					if (errno != EEXIST)
					{
						syslog_ax_eth_port_dbg("npd set cscd port ipaddr RTNETLINK answers");
					}
					else
					{
						close(fd);		/* code optimize: recourse leak houxx@autelan.com  2013-1-17 */
						return NPD_SUCCESS;
					}
				}
				close(fd);
				return NPD_FAIL;
			}
			syslog_ax_eth_port_dbg("npd set cscd port ipaddr Unexpected reply!!!\n");

			status -= NLMSG_ALIGN(len);
			h = (struct nlmsghdr*)((char*)h + NLMSG_ALIGN(len));
		}
		if (msg.msg_flags & MSG_TRUNC) 
		{
			syslog_ax_eth_port_dbg("npd set cscd port ipaddr Message truncated\n");
			continue;
		}
		if (status) 
		{
			syslog_ax_eth_port_dbg("npd set cscd port ipaddr Remnant of size %d\n", status);
			close(fd);				/* code optimize: recourse leak houxx@autelan.com  2013-1-17 */
			exit(1);
		}
	}

	close( fd );
	return NPD_SUCCESS;

}


/**********************************************************************************
 *  npd_del_port_switch_mode
 *
 *	DESCRIPTION:
 * 		del switch port running mode
 *
 *	INPUT:
 *		eth_g_index - index of port's mode 
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *			INTERFACE_RETURN_CODE_SUCCESS
 *			INTERFACE_RETURN_CODE_ERROR
 *
 **********************************************************************************/
int npd_del_port_switch_mode(unsigned int eth_g_index,unsigned int mode)
{
	int ret = INTERFACE_RETURN_CODE_SUCCESS;
	struct eth_port_s* portInfo = NULL;
	struct eth_port_switch_s* switchNode = NULL;

	npd_port_vlan_free(eth_g_index);
	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL != portInfo) {
		switchNode = portInfo->funcs.func_data[ETH_PORT_FUNC_BRIDGE];
		free(switchNode);
		portInfo->funcs.func_data[ETH_PORT_FUNC_BRIDGE] = NULL;
		portInfo->funcs.funcs_run_bitmap &= ~(1 << ETH_PORT_FUNC_BRIDGE);
	}
	else 
		ret = INTERFACE_RETURN_CODE_ERROR;

	return ret;
	
}

/**********************************************************************************
 *  npd_del_port_route_mode
 *
 *	DESCRIPTION:
 * 		del route port running mode
 *
 *	INPUT:
 *		eth_g_index - index of port's mode 
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		INTERFACE_RETURN_CODE_SUCCESS 
 *		COMMON_RETURN_CODE_NULL_PTR   -  NULL POINT  error
 *		INTERFACE_RETURN_CODE_FD_ERROR  - fd is invalidate
 *		INTERFACE_RETURN_CODE_IOCTL_ERROR   -  ioctl FAILED
 *		INTERFACE_RETURN_CODE_MAC_GET_ERROR
 *		INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST
 *		INTERFACE_RETURN_CODE_FDB_SET_ERROR
 *		INTERFACE_RETURN_CODE_ERROR
 *
 **********************************************************************************/
int npd_del_port_route_mode(unsigned int eth_g_index,unsigned int opCode)
{
	int ret=INTERFACE_RETURN_CODE_SUCCESS;

	/*ret = npd_route_mode_del_intf_by_port_index(eth_g_index);*/
	ret = npd_intf_del_route_mode(eth_g_index, opCode);
	if(INTERFACE_RETURN_CODE_SUCCESS == ret) {
	    ret = npd_vlan_interface_port_del(NPD_PORT_L3INTF_VLAN_ID,eth_g_index,NPD_FALSE);
		if(NPD_SUCCESS != ret){
			ret = INTERFACE_RETURN_CODE_ERROR;
		}
		else{
			ret = INTERFACE_RETURN_CODE_SUCCESS;
		}
	}
	
	return ret;
}

/**********************************************************************************
 *  npd_del_port_promi_mode
 *
 *	DESCRIPTION:
 * 		del promi port running mode
 *
 *	INPUT:
 *		eth_g_index - index of port's mode 
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *			INTERFACE_RETURN_CODE_SUCCESS
 *			INTERFACE_RETURN_CODE_ERROR
 *
 **********************************************************************************/
int npd_del_port_promi_mode(unsigned int eth_g_index,unsigned int mode,int ifnameType,unsigned int opCode)
{
	int ret = INTERFACE_RETURN_CODE_SUCCESS;
	struct eth_port_s* portInfo = NULL;
	struct eth_port_promi_s* promiNode = NULL;
	ve_netdev_priv ops = {0};
	unsigned char devNum=0,portNum = 0;
	unsigned int slot_no = 0, slot_index = 0;
	unsigned int local_port_no = 0,port_index = 0;
    unsigned char isTagged = FALSE;
	enum product_id_e productId = PRODUCT_ID; 
	enum module_id_e moduleId = MODULE_ID_NONE;
	unsigned char * ifnamePrefix = NULL;
	if(ifnameType){
		ifnamePrefix = "e";
	}
	else{
		ifnamePrefix = "eth";
	}

	if(((PRODUCT_ID_AX7K_I == productId)||(PRODUCT_ID_AX7K == productId))
		&& (eth_g_index < 4)) {
		return INTERFACE_RETURN_CODE_ERROR;
	}

	npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
	slot_index 		= SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	slot_no			= CHASSIS_SLOT_INDEX2NO(slot_index);
	port_index 		= ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	local_port_no 	= ETH_LOCAL_INDEX2NO(slot_index,port_index);
	moduleId		= MODULE_TYPE_ON_SLOT_INDEX(slot_index);

#if 0
    ret = nam_egress_filter_enable(devNum,portNum,1);
	if (0 != ret){
        return ret;
	}
#endif
	if(ETH_PORT_FUNC_IPV4 == mode) {
		/*npd_vlan_interface_port_del(1,eth_g_index,NPD_FALSE);*/
		npd_port_vlan_free(eth_g_index);
	}
	
	nam_pvlan_port_delete(eth_g_index);
	npd_set_promi_attr(eth_g_index,FALSE);

	npd_eth_port_destroy_all_promi_subif(eth_g_index);
	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL != portInfo) {
		promiNode = portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS];
		free(promiNode);
		portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS] = NULL;
		portInfo->funcs.funcs_run_bitmap &= ~(1<<ETH_PORT_FUNC_MODE_PROMISCUOUS);
		ops.devNum = devNum;
		ops.portNum = portNum;
		if(MODULE_ID_AX7I_XG_CONNECT == moduleId) {
			sprintf(ops.name,"cscd%d",local_port_no - 1);
		}
		else {
			sprintf(ops.name,"%s%d-%d",ifnamePrefix,slot_no,local_port_no);
		}
		/*ret = ioctl (promi_fd,BM_IOC_UNREG_,&ops);*/
		if(NPD_INTF_ADV_DIS == opCode){
			ret = cavim_do_intf(CVM_IOC_ADV_DIS_, &ops);
		}
		else{
			ret = cavim_do_intf(CVM_IOC_UNREG_, &ops);
		}
		if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
			syslog_ax_eth_port_err("delete promiscuous port %#x tell kmod error\n", eth_g_index);
			ret = INTERFACE_RETURN_CODE_ERROR;
		} 
	}
	else 
		ret = INTERFACE_RETURN_CODE_ERROR;
	if(INTERFACE_RETURN_CODE_SUCCESS == ret) {
        if((DEFAULT_VLAN_ID != advanced_routing_default_vid)&&\
            (npd_vlan_check_contain_port(advanced_routing_default_vid,eth_g_index,&isTagged))&&\
            (FALSE == isTagged)){
    	    ret = npd_vlan_interface_port_del(advanced_routing_default_vid,eth_g_index,FALSE);
    		if(INTERFACE_RETURN_CODE_SUCCESS != ret){
                syslog_ax_eth_port_err("npd valn interface port del error when del promis mode,ret %#x\n",ret);
    			ret = INTERFACE_RETURN_CODE_DEL_PORT_FAILED;
    		}
        }
	}
	

	return ret;

}

/**********************************************************************************
 *  npd_set_port_switch_mode
 *
 *	DESCRIPTION:
 * 		set switch port running mode
 *
 *	INPUT:
 *		eth_g_index - index of port's mode 
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *			INTERFACE_RETURN_CODE_SUCCESS
 *			INTERFACE_RETURN_CODE_ERROR
 *
 **********************************************************************************/
int npd_set_port_switch_mode(unsigned int eth_g_index,unsigned int oldMode)
{
	int ret=INTERFACE_RETURN_CODE_SUCCESS;
	struct eth_port_s* portInfo = NULL;
	struct eth_port_switch_s* switchNode = NULL;
	#if 0
	if(!((ETH_PORT_FUNC_MODE_PROMISCUOUS == oldMode)&&\
		(advanced_routing_default_vid == DEFAULT_VLAN_ID))){
		ret = npd_vlan_interface_port_add(DEFAULT_VLAN_ID,eth_g_index,NPD_FALSE);
 		if(VLAN_RETURN_CODE_ERR_NONE != ret){
			syslog_ax_eth_port_err("vlan interface port add failed,ret %d\n",ret);
			ret = INTERFACE_RETURN_CODE_ERROR;
 		}
		else {
			ret = INTERFACE_RETURN_CODE_SUCCESS;
		}
	}
	#endif

		switchNode = (struct eth_port_switch_s *)malloc(sizeof(struct eth_port_switch_s));
		if(NULL == switchNode){
			syslog_ax_eth_port_dbg("malloc address for node erro!\n");
			ret=INTERFACE_RETURN_CODE_ERROR;
			return ret;
		}
		
		switchNode->isSwitch = TRUE;
		switchNode->npd_switch_port_notifier_func = NULL;
		portInfo = npd_get_port_by_index(eth_g_index);
		if(NULL != portInfo) {
			portInfo->funcs.func_data[ETH_PORT_FUNC_BRIDGE] = switchNode;
			portInfo->funcs.funcs_run_bitmap |= (1 << ETH_PORT_FUNC_BRIDGE);
		}
		else 
		{
			ret = INTERFACE_RETURN_CODE_ERROR;
			/* code optimize: recourse leak zhangcl@autelan.com  2013-1-22 */
			free(switchNode);
			return ret;
		}
		if((ETH_PORT_FUNC_MODE_PROMISCUOUS == oldMode)&&
			(0 != nam_fdb_enable_port_auto_learn_set(eth_g_index,TRUE))){
			ret = INTERFACE_RETURN_CODE_ERROR;
		}
 
	syslog_ax_eth_port_dbg("npd_eth_port2012 >>set switch mode ret[%d]\n",ret);
	return ret;
	
}

/**********************************************************************************
 *  npd_set_port_route_mode
 *
 *	DESCRIPTION:
 * 		set route port running mode
 *
 *	INPUT:
 *		index - index of port's mode 
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		INTERFACE_RETURN_CODE_SUCCESS 
 *		COMMON_RETURN_CODE_NULL_PTR
 *		INTERFACE_RETURN_CODE_ERROR
 *		INTERFACE_RETURN_CODE_NAM_ERROR 
 *		INTERFACE_RETURN_CODE_CHECK_MAC_ERROR
 *		INTERFACE_RETURN_CODE_FDB_SET_ERROR
 *		INTERFACE_RETURN_CODE_FD_ERROR
 *		INTERFACE_RETURN_CODE_IOCTL_ERROR
 *		INTERFACE_RETURN_CODE_MAC_GET_ERROR
 *
 **********************************************************************************/
int npd_set_port_route_mode(unsigned int eth_g_index,int ifnameType,unsigned int opCode)
{
	int ret=INTERFACE_RETURN_CODE_SUCCESS;
	unsigned char name[16] = {'\0'};
	unsigned int slot_no = 0, slot_index = 0;
	unsigned int local_port_no = 0,port_index = 0;
	enum module_id_e moduleId = MODULE_ID_NONE;
	unsigned char * ifnamePrefix = NULL;
	if(ifnameType){
		ifnamePrefix = "e";
	}
	else{
		ifnamePrefix = "eth";
	}

	/*add port to vlan 4095*/
	/*ret = npd_vlan_interface_port_add(NPD_PORT_L3INTF_VLAN_ID,eth_g_index,NPD_FALSE);*/
	 npd_vlan_interface_port_add(NPD_PORT_L3INTF_VLAN_ID,eth_g_index,NPD_FALSE);
	/*if(NPD_DBUS_SUCCESS == ret) {*/
	slot_index 		= SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	slot_no			= CHASSIS_SLOT_INDEX2NO(slot_index);
	port_index 		= ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	local_port_no 	= ETH_LOCAL_INDEX2NO(slot_index,port_index);
	moduleId		= MODULE_TYPE_ON_SLOT_INDEX(slot_index);
		
	if(MODULE_ID_AX7I_XG_CONNECT == moduleId) {
		sprintf(name,"cscd%d",local_port_no - 1);
	}
	else {
		sprintf((char*)name,"%s%d-%d",ifnamePrefix,slot_no,local_port_no);
	}
	ret = npd_intf_create_router_mode(eth_g_index, name, opCode);
 
	if(INTERFACE_RETURN_CODE_SUCCESS != ret)
		npd_vlan_interface_port_del(NPD_PORT_L3INTF_VLAN_ID,eth_g_index,NPD_FALSE);

	syslog_ax_eth_port_dbg("npd_eth_port2051 >>set route mode ret[%d]\n",ret);
	
	return ret;
}

/**********************************************************************************
 *  npd_set_cscd_port_promi_mode
 *
 *	DESCRIPTION:
 * 		set promiscuous port running mode
 *
 *	INPUT:
 *		port - witch port want to set 
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *			INTERFACE_RETURN_CODE_SUCCESS
 *                INTERFCE_RETURN_CODE_ADVANCED_VLAN_NOT_EXISTS
 *			INTERFACE_RETURN_CODE_ERROR
 *
 **********************************************************************************/
int npd_set_cscd_port_promi_mode(unsigned char port)
{
	int ret=INTERFACE_RETURN_CODE_SUCCESS;
	struct eth_port_s* portInfo = NULL;
	struct eth_port_promi_s* promiNode = NULL;
	ve_netdev_priv ops = {0};
	unsigned char devNum=0,portNum = 0;
	struct asic_port_info_s xgport ;

	memset(&xgport, 0, sizeof(struct asic_port_info_s));
	xgport = ax7i_xg_conn_port_mapArr[0][port];
	if(0 == port){
		devNum = xgport.devNum;
		portNum = xgport.portNum;
	}
	else if (1== port){
		devNum = 0;
		portNum = 12;
	}

	syslog_ax_eth_port_dbg("npd set cscd port promi mode,port %d.\n",port);
	if(((0 == port)&&(0 != setNum1))||((1 == port)&&(0 != setNum2))){
        syslog_ax_eth_port_dbg("npd_set_cscd_port_promi_mode:cscd port %d already creat promisous!\n",port);
		return INTERFACE_RETURN_CODE_INTERFACE_EXIST;
	}
    /* add to default vlan*/
#if 0
	if(TRUE != npd_check_vlan_real_exist(advanced_routing_default_vid)){
		ret = INTERFCE_RETURN_CODE_ADVANCED_VLAN_NOT_EXISTS;
        return ret;
	}
#endif
	ret = npd_vlan_interface_cscd_add(advanced_routing_default_vid,port,NPD_FALSE);
	if(NPD_DBUS_SUCCESS != ret) {
        syslog_ax_eth_port_err("npd_set_cscd_port_promi_mode:add port to %d vlan error!\n",advanced_routing_default_vid);
        return ret;
	}
#if 0
    /* keep the promis port in vlan 4095 also*/
	/*npd_vlan_interface_port_add(NPD_PORT_L3INTF_VLAN_ID,eth_g_index,NPD_FALSE);*/
#endif
    /*    if (prvCpssDrvHwPpSetRegField(PROMISCUOUS_TARGET_DEV_NUM, 0x07800004, PROMISCUOUS_TARGET_PORT_NUM, 1, 1) != 0)
            return NPD_DBUS_ERROR;
        if (prvCpssDrvHwPpSetRegField(PROMISCUOUS_TARGET_DEV_NUM, 0x07800020, PROMISCUOUS_TARGET_PORT_NUM, 1, 1) != 0)
            return NPD_DBUS_ERROR;
	*/
#if 0
		promiNode = (struct eth_port_promi_s *)malloc(sizeof(struct eth_port_promi_s));
		if(NULL == promiNode){
			syslog_ax_eth_port_dbg("malloc address for node erro!\n");
			ret=INTERFACE_RETURN_CODE_ERROR;
			return ret;
		}

		promiNode->type = ETH_PORT_CSCD_FOR_PROMISCUOUS;
		promiNode->up.cscd_port.targetDev = PROMISCUOUS_TARGET_DEV_NUM;
		promiNode->up.cscd_port.targetPort = PROMISCUOUS_TARGET_PORT_NUM;
		promiNode->npd_promi_port_notifier_func = (NPD_ETH_PORT_LINK_CHANGE_NOTIFIER_FUNC)npd_promi_port_link_change;
		memset((char*)promiNode->uniMac, 0xFF, 6);
		memset(&promiNode->subifList,0,sizeof(dot1q_vlan_list_s));
		INIT_LIST_HEAD(&(promiNode->subifList.list));
#endif		
#if 0
		ret = nam_egress_filter_enable(devNum,portNum,0);
		if(0 != ret){
			return INTERFACE_RETURN_CODE_ERROR;
		}
#endif
			ops.devNum = devNum;
			ops.portNum = portNum;
			/*sprintf(ops.name,"cscd%d",port);*/
			if((PRODUCT_ID_AX7K_I == PRODUCT_ID)&&\
				((0 == LOCAL_CHASSIS_SLOT_NO)||(1 == LOCAL_CHASSIS_SLOT_NO))){
				sprintf(ops.name,"obc%d",port);
			}
			else{
				sprintf(ops.name,"cscd%d",port);
			}
			/*ret = ioctl(promi_fd,BM_IOC_REG_ ,&ops);*/
            if((SLOT_ID == 2) && (port == 0)){
                ops.devNum = 3;
                ret = cavim_do_intf(CVM_IOC_REG_,&ops);
            }
            else if((SLOT_ID == 2)&&(port == 1)){
                ops.devNum = 2;
                ret = cavim_do_intf(CVM_IOC_REG_,&ops);
            }
            else{
			    ret = cavim_do_intf(CVM_IOC_REG_, &ops);
            }

			if(INTERFACE_RETURN_CODE_SUCCESS != ret) {
				syslog_ax_eth_port_err("create promiscous devNum %d portNum %d tell kmod error\n", \
								devNum,portNum);
#if 0
				free(promiNode);
				promiNode = NULL;
#endif
				ret = INTERFACE_RETURN_CODE_ERROR;
			}
			else {
#if 0
				/* update promiscuous port link status*/
				unsigned long linkState = ETH_ATTR_LINKDOWN;
				
				ret = nam_get_port_link_state(devNum,portNum,&linkState);
				if(NPD_SUCCESS != ret) {					
					syslog_ax_eth_port_err("create promiscuous devNum %d portNum %d get link status error %d\n",  \
								devNum,portNum, ret);
#if 0
					free(promiNode);
					promiNode = NULL;
#endif
					return INTERFACE_RETURN_CODE_ERROR;
				}
				else if(ETH_ATTR_LINKDOWN == linkState) {
					ret = cavim_do_intf(CVM_IOC_DEV_DOWN, &ops);
					if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
						syslog_ax_eth_port_err("promiscuous devNum %d portNum %d link down tell kmod error %d\n",  \
									devNum,portNum, ret);
#if 0
						free(promiNode);
						promiNode = NULL;
#endif
						return INTERFACE_RETURN_CODE_ERROR;
					}
					syslog_ax_eth_port_dbg("update promiscuous devNum %d portNum %d link down\n", devNum,portNum);
				}
#else

                if((SLOT_ID == 2) && (port == 0)){
                    ops.devNum = 3;
                    ret = cavim_do_intf(CVM_IOC_DEV_UP, &ops);
                }
                else if((SLOT_ID == 2)&&(port == 1)){
                    ops.devNum = 2;
                    ret = cavim_do_intf(CVM_IOC_DEV_UP, &ops);
                }
                else{
                    ret = cavim_do_intf(CVM_IOC_DEV_UP, &ops);
                }

				syslog_ax_eth_port_evt("set interface %s up, ret %d\n",ops.name, ret);

#endif

				ret = nam_pvlan_cscd_port_config_spi(devNum,portNum,PROMISCUOUS_TARGET_DEV_NUM,PROMISCUOUS_TARGET_PORT_NUM);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_dbg("cscd port config spi error, devNum %d portNum %d.\n", devNum,portNum);
				}

				ret = npd_set_cscd_promi_attr(advanced_routing_default_vid,devNum,portNum,TRUE);
				if(NPD_SUCCESS != ret){
					syslog_ax_eth_port_dbg("set cscd promi attribute error, devNum %d portNum %d.\n", devNum,portNum);
				}
			}
	if(0 == port) setNum1++;
	else if(1 == port) setNum2++;
	syslog_ax_eth_port_dbg("set cscd port %#x promiscuous mode ret 0x%x,setNum1 %d,setNum2 %d.\n",portNum,ret,setNum1,setNum2);
	return ret;
}

/**********************************************************************************
 *  npd_set_port_promi_mode
 *
 *	DESCRIPTION:
 * 		set promiscuous port running mode
 *
 *	INPUT:
 *		eth_g_index - index of port's mode 
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *			INTERFACE_RETURN_CODE_SUCCESS
 *                INTERFCE_RETURN_CODE_ADVANCED_VLAN_NOT_EXISTS
 *			INTERFACE_RETURN_CODE_ERROR
 *
 **********************************************************************************/

int npd_set_port_promi_mode(unsigned int eth_g_index,int ifnameType)
{
	int ret=INTERFACE_RETURN_CODE_SUCCESS;
	struct eth_port_s* portInfo = NULL;
	struct eth_port_promi_s* promiNode = NULL;
	ve_netdev_priv ops = {0};
	unsigned char devNum=0,portNum = 0;
	unsigned int slot_no = 0, slot_index = 0;
	unsigned int local_port_no = 0,port_index = 0;
	enum module_id_e moduleId = MODULE_ID_NONE;
	unsigned char * ifnamePrefix = NULL;
	if(ifnameType){
		ifnamePrefix = "e";
	}
	else{
		ifnamePrefix = "eth";
	}

    /* add to default vlan*/
	if(TRUE != npd_check_vlan_real_exist(advanced_routing_default_vid)){
		ret = INTERFCE_RETURN_CODE_ADVANCED_VLAN_NOT_EXISTS;
        return ret;
	}
	ret = npd_vlan_interface_port_add(advanced_routing_default_vid,eth_g_index,NPD_FALSE);
	if(NPD_DBUS_SUCCESS != ret) {
        syslog_ax_eth_port_err("add port to %d vlan error!\n",advanced_routing_default_vid);
        return ret;
	}
#if 0
    /* keep the promis port in vlan 4095 also*/
	/*npd_vlan_interface_port_add(NPD_PORT_L3INTF_VLAN_ID,eth_g_index,NPD_FALSE);*/
#endif
    /*    if (prvCpssDrvHwPpSetRegField(PROMISCUOUS_TARGET_DEV_NUM, 0x07800004, PROMISCUOUS_TARGET_PORT_NUM, 1, 1) != 0)
            return NPD_DBUS_ERROR;
        if (prvCpssDrvHwPpSetRegField(PROMISCUOUS_TARGET_DEV_NUM, 0x07800020, PROMISCUOUS_TARGET_PORT_NUM, 1, 1) != 0)
            return NPD_DBUS_ERROR;
	*/
		promiNode = (struct eth_port_promi_s *)malloc(sizeof(struct eth_port_promi_s));
		if(NULL == promiNode){
			syslog_ax_eth_port_dbg("malloc address for node erro!\n");
			ret=INTERFACE_RETURN_CODE_ERROR;
			return ret;
		}

		promiNode->type = ETH_PORT_CSCD_FOR_PROMISCUOUS;
		promiNode->up.cscd_port.targetDev = PROMISCUOUS_TARGET_DEV_NUM;
		promiNode->up.cscd_port.targetPort = PROMISCUOUS_TARGET_PORT_NUM;
		promiNode->npd_promi_port_notifier_func = (NPD_ETH_PORT_LINK_CHANGE_NOTIFIER_FUNC)npd_promi_port_link_change;
		memset((char*)promiNode->uniMac, 0xFF, 6);
		memset(&promiNode->subifList,0,sizeof(dot1q_vlan_list_s));
		INIT_LIST_HEAD(&(promiNode->subifList.list));
		
		slot_index 		= SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
		slot_no			= CHASSIS_SLOT_INDEX2NO(slot_index);
		port_index 		= ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
		local_port_no 	= ETH_LOCAL_INDEX2NO(slot_index,port_index);
		moduleId		= MODULE_TYPE_ON_SLOT_INDEX(slot_index);
		ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
		if (NPD_SUCCESS != ret) {
			syslog_ax_eth_port_err("get devport by global index %#x error!\n", \
									eth_g_index);
			free(promiNode);
			promiNode = NULL;
			ret = INTERFACE_RETURN_CODE_ERROR;
			return ret;
		}

#if 0
		ret = nam_egress_filter_enable(devNum,portNum,0);
		if(0 != ret){
			return INTERFACE_RETURN_CODE_ERROR;
		}
#endif
		portInfo = npd_get_port_by_index(eth_g_index);
		if(NULL != portInfo) {
			ops.devNum = devNum;
			ops.portNum = portNum;
			if(MODULE_ID_AX7I_XG_CONNECT == moduleId) {
				sprintf(ops.name,"cscd%d",local_port_no - 1);
			}
			else {
				sprintf(ops.name,"%s%d-%d",ifnamePrefix,slot_no,local_port_no);
			}
			/*syslog_ax_eth_port_dbg(("promi fd %d \n",promi_fd);*/
			/*ret = ioctl(promi_fd,BM_IOC_REG_ ,&ops);*/
			ret = cavim_do_intf(CVM_IOC_ADV_EN_, &ops);
			if(INTERFACE_RETURN_CODE_SUCCESS != ret) {
				syslog_ax_eth_port_err("create promiscous port %#x tell kmod error\n", \
								eth_g_index);
				free(promiNode);
				promiNode = NULL;
				ret = INTERFACE_RETURN_CODE_ERROR;
			}
			else {
				
				/* update promiscuous port link status*/
				unsigned int linkState = ETH_ATTR_LINKDOWN;
				ret = npd_get_port_link_status(eth_g_index, &linkState);
				if(NPD_SUCCESS != ret) {
					syslog_ax_eth_port_err("create promiscuous port %#x get link status error %d\n",  \
								eth_g_index, ret);
					free(promiNode);
					promiNode = NULL;
					return INTERFACE_RETURN_CODE_ERROR;					
				}
				else if(ETH_ATTR_LINKDOWN == linkState) {
					ret = cavim_do_intf(CVM_IOC_DEV_DOWN, &ops);
					if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
						syslog_ax_eth_port_err("promiscuous port %#x link down tell kmod error %d\n",  \
									eth_g_index, ret);
						free(promiNode);
						promiNode = NULL;
						return INTERFACE_RETURN_CODE_ERROR;
					}
					syslog_ax_eth_port_dbg("update promiscuous port %#x link down\n", eth_g_index);
				}

				/* product AU5612 has asic port(0,0) as promiscuous uplink port*/
				if(PRODUCT_ID_AX5K == PRODUCT_ID) {
					ret = nam_pvlan_port_config_spi(eth_g_index,PROMISCUOUS_AU5612_TARGET_DEV,PROMISCUOUS_AU5612_TARGET_PORT);
				}
				/* product AU5612I has asic port(0,24) as promiscuous uplink port*/
				else if(PRODUCT_ID_AX5K_I == PRODUCT_ID) {
					ret = nam_pvlan_port_config_spi(eth_g_index,PROMISCUOUS_AU5612I_TARGET_DEV,PROMISCUOUS_AU5612I_TARGET_PORT);
				}
				else if((PRODUCT_ID_AX7K_I == PRODUCT_ID) || (PRODUCT_ID_AX7K == PRODUCT_ID)){
					ret = nam_pvlan_port_config_spi(eth_g_index,PROMISCUOUS_TARGET_DEV_NUM,PROMISCUOUS_TARGET_PORT_NUM);
				}
				npd_set_promi_attr(eth_g_index,TRUE);
				portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS] = promiNode;
				portInfo->funcs.funcs_run_bitmap |= (1<<ETH_PORT_FUNC_MODE_PROMISCUOUS);
				if(NPD_SUCCESS == ret){
					ret = INTERFACE_RETURN_CODE_SUCCESS;
				}
				else{
					ret = INTERFACE_RETURN_CODE_ERROR;
				}

			}
		}
		else {
			/* code optimize: recourse leak zhangcl@autelan.com  2013-1-22 */
			free(promiNode);
			promiNode = NULL;
			ret = INTERFACE_RETURN_CODE_ERROR;
		}
 
	syslog_ax_eth_port_dbg("set port %#x promiscuous mode ret %d\n",portNum,ret);
	return ret;
}

int npd_intf_eth_port_advanced_routing_hw_enable_set
(
	unsigned int eth_g_index,
	unsigned int enable
)
{
		unsigned char devNum = 0,portNum = 0;
		unsigned int ret = 0;
		
		if(!enable)
		{
			nam_pvlan_port_delete(eth_g_index);
			npd_set_promi_attr(eth_g_index,FALSE);
		}
		else
		{
			/* product AU5612 has asic port(0,0) as promiscuous uplink port*/
			if(PRODUCT_ID_AX5K == PRODUCT_ID)
			{
				ret = nam_pvlan_port_config_spi(eth_g_index,PROMISCUOUS_AU5612_TARGET_DEV,PROMISCUOUS_AU5612_TARGET_PORT);
			}
			/* product AU5612I has asic port(0,24) as promiscuous uplink port*/
			else if(PRODUCT_ID_AX5K_I == PRODUCT_ID)
			{
				ret = nam_pvlan_port_config_spi(eth_g_index,PROMISCUOUS_AU5612I_TARGET_DEV,PROMISCUOUS_AU5612I_TARGET_PORT);
			}
			else if(PRODUCT_ID_AX7K == PRODUCT_ID)
			{
				ret = nam_pvlan_port_config_spi(eth_g_index,PROMISCUOUS_TARGET_DEV_NUM,PROMISCUOUS_TARGET_PORT_NUM);
			}
			npd_set_promi_attr(eth_g_index,TRUE);
		}
	return 0; /* code optimize: Missing return statement  0 houxx@autelan.com  2013-1-17 */
}
/**********************************************************************************
 *  npd_check_port_is_mode_type
 *
 *	DESCRIPTION:
 * 		check port swicth mode
 *
 *	INPUT:
 *		eth_g_index - port index 
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *			TRUE
 *			FALSE
 *
 **********************************************************************************/
int npd_check_port_switch_mode(unsigned int eth_g_index)
{
	struct eth_port_s* portInfo = NULL;

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL != portInfo){
		if(NULL == (portInfo->funcs.func_data[ETH_PORT_FUNC_BRIDGE]))
			return FALSE;
		else
			return TRUE;
	}
	else
		return FALSE;
}

/**********************************************************************************
 *  npd_check_port_promi_mode
 *
 *	DESCRIPTION:
 * 		check port promiscuous mode
 *
 *	INPUT:
 *		eth_g_index - port index 
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *			TRUE
 *			FALSE
 *
 **********************************************************************************/
int npd_check_port_promi_mode(unsigned int eth_g_index)
{
	struct eth_port_s* portInfo = NULL;

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL != portInfo){
		if(NULL == (portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS])) {
			return FALSE;
		}
		else {
			return TRUE;
		}
	}
	else
		return FALSE;
}


/**********************************************************************************
 *  npd_port_type_deal
 *
 *	DESCRIPTION:
 * 		deal port mode
 *
 *	INPUT:
 *		eth-g_index - port index
 *		mode  		    - port  mode
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		COMMON_RETURN_CODE_NULL_PTR
 *		INTERFACE_RETURN_CODE_SUCCESS	
 *		INTERFACE_RETURN_CODE_ERROR
 *		INTERFACE_RETURN_CODE_ALREADY_THIS_MODE
 *		INTERFACE_RETURN_CODE_PORT_HAS_SUB_IF
 *		INTERFACE_RETURN_CODE_NAM_ERROR 
 *		INTERFACE_RETURN_CODE_CHECK_MAC_ERROR
 *		INTERFACE_RETURN_CODE_FDB_SET_ERROR
 *		INTERFACE_RETURN_CODE_FD_ERROR
 *		INTERFACE_RETURN_CODE_IOCTL_ERROR
 *		INTERFACE_RETURN_CODE_MAC_GET_ERROR
 *		INTERFACE_RETURN_CODE_DEFAULT_VLAN_IS_L3_VLAN *		
 **********************************************************************************/
int npd_port_type_deal_for_lacp( unsigned int port_index,unsigned char* pname,unsigned char endis)
{
	int ret = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned short  pvid = 1;
    unsigned char   addr[6] = {0};
    unsigned int     ifindex = ~0UI;
    unsigned char   dev = 0, port = 0;
	unsigned int opCode = 0;
    npd_get_devport_by_global_index(port_index, &dev, &port);
    syslog_ax_intf_dbg("set port %#x route mode\n", port_index);
	if(endis)
	{
		opCode = NPD_INTF_CREATE_INTF;
		ret = npd_eth_port_interface_check(port_index,&ifindex);
		if(ifindex == ~0UI)
		{
		    ret = npd_eth_port_l3intf_create(port_index, ifindex);
		    if (INTERFACE_RETURN_CODE_SUCCESS != ret) 
			{
		        syslog_ax_intf_dbg("npd create intf is error\n");
		        ret = INTERFACE_RETURN_CODE_ERROR;
		    }
		    else 
			{
		        ret = npd_intf_create_by_port_index(port_index, pvid, &ifindex, pname,opCode);
		        if (INTERFACE_RETURN_CODE_SUCCESS == ret) 
				{
		            ret = npd_eth_port_l3intf_create(port_index, ifindex);
		            npd_all_arp_clear_by_port(port_index);
		            syslog_ax_intf_dbg("npd create intf by port_index %d correct ,ifindex = %d\n", port_index, ifindex);
		            if (INTERFACE_RETURN_CODE_SUCCESS != ret) 
					{
		                syslog_ax_intf_dbg("npd create intf is error\n");
		                ret = INTERFACE_RETURN_CODE_ERROR;
		            }
		            else 
					{
		                ret = INTERFACE_RETURN_CODE_SUCCESS;
		            }
		        }
		        else 
				{
		            syslog_ax_intf_dbg("npd create intf is error\n");
		            npd_eth_port_l3intf_destroy(port_index, ifindex);
		        }
		    }
			/*disable port L3 function*/
			ret = nam_l3_intf_on_port_enable(port_index,pvid,0);
			if(ret != INTERFACE_RETURN_CODE_SUCCESS)
			{
				syslog_ax_intf_err("disable l3 intf on eth_g_index %d fail \n",port_index);
			}
			ret = npd_eth_port_interface_l3_flag_set(port_index,0);
			if(ret != INTERFACE_RETURN_CODE_SUCCESS)
			{
				syslog_ax_intf_err("disable l3 intf on software fail !\n");
			}
		}
		else
		{
			syslog_ax_intf_dbg("port_index %d is already a L3 interface !\n",port_index);
			ret = INTERFACE_RETURN_CODE_INTERFACE_EXIST;
		}
	}
	else
	{
		opCode = NPD_INTF_DEL_INTF;
		ret = npd_eth_port_interface_check(port_index, &ifindex);
		if((TRUE == ret) && (~0UI != ifindex)) 
		{
			ret = npd_intf_set_port_l3intf_status(port_index,ETH_PORT_NOTIFIER_LINKDOWN_E);
			if(ret != INTERFACE_RETURN_CODE_SUCCESS)
			{
				syslog_ax_intf_err("set L3 port_index %d down fail !!!\n",port_index);
			}
			pthread_mutex_lock(&semPktRxMutex);
            pthread_mutex_lock(&semKapIoMutex);
			ret = npd_intf_del_by_ifindex(ifindex,opCode);
			if(ret != INTERFACE_RETURN_CODE_SUCCESS)
			{
				syslog_ax_intf_err("del port_index %d on hw failed !!!\n",port_index);
			}
			else
			{
				ret = npd_eth_port_l3intf_destroy(port_index,ifindex);
				if(ret != INTERFACE_RETURN_CODE_SUCCESS)
				{
					syslog_ax_intf_err("del port_index %d on sw failed \n",port_index);
				}
			}
			pthread_mutex_unlock(&semKapIoMutex);
            pthread_mutex_unlock(&semPktRxMutex);

			/*set port switch mode*/
			ret = npd_set_port_switch_mode(port_index,ETH_PORT_FUNC_IPV4);
			if(ret != INTERFACE_RETURN_CODE_SUCCESS)
			{
				syslog_ax_intf_err("set port_index %d switch mode failed \n",port_index);
			}
		}
		else
		{
			syslog_ax_intf_err("port_index %d is not a L3 port !!!\n",port_index);
			ret = INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST;
		}
	}
    return ret;
}


/**********************************************************************************
 *  npd_port_type_deal
 *
 *	DESCRIPTION:
 * 		deal port mode
 *
 *	INPUT:
 *		eth-g_index - port index
 *		mode  		    - port  mode
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		COMMON_RETURN_CODE_NULL_PTR
 *		INTERFACE_RETURN_CODE_SUCCESS	
 *		INTERFACE_RETURN_CODE_ERROR
 *		INTERFACE_RETURN_CODE_ALREADY_THIS_MODE
 *		INTERFACE_RETURN_CODE_PORT_HAS_SUB_IF
 *		INTERFACE_RETURN_CODE_NAM_ERROR 
 *		INTERFACE_RETURN_CODE_CHECK_MAC_ERROR
 *		INTERFACE_RETURN_CODE_FDB_SET_ERROR
 *		INTERFACE_RETURN_CODE_FD_ERROR
 *		INTERFACE_RETURN_CODE_IOCTL_ERROR
 *		INTERFACE_RETURN_CODE_MAC_GET_ERROR
 *		INTERFACE_RETURN_CODE_DEFAULT_VLAN_IS_L3_VLAN *		
 **********************************************************************************/
int npd_port_type_deal(unsigned int eth_g_index,unsigned int mode,int ifnameType)
{
	int ret = INTERFACE_RETURN_CODE_SUCCESS;
	unsigned int ifIndex = ~0UI;
	unsigned int oldMode = ETH_PORT_FUNC_BRIDGE;
	unsigned short vlanId = 0;
	struct eth_port_s *portInfo = NULL;
	struct eth_port_promi_s* promiInfo = NULL;
	unsigned int opCode = 0;
	
	switch (mode) {
		case ETH_PORT_FUNC_BRIDGE:
			opCode = NPD_INTF_DEL_INTF;
			ret = npd_check_port_switch_mode(eth_g_index);
			if(TRUE == ret) {
				ret = INTERFACE_RETURN_CODE_ALREADY_THIS_MODE;
				break;
			}
			else {
				ret = npd_check_port_promi_mode( eth_g_index);
				if(TRUE == ret) {
					ret = INTERFACE_RETURN_CODE_SUCCESS;
					portInfo = npd_get_port_by_index(eth_g_index);
					if(NULL != portInfo){
                        promiInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS];
						if((NULL != promiInfo)&&(promiInfo->subifList.count > 0)){
                            ret = INTERFACE_RETURN_CODE_PORT_HAS_SUB_IF;
						}
					}
					else{
						ret = INTERFACE_RETURN_CODE_ERROR;
					}
					if(INTERFACE_RETURN_CODE_SUCCESS == ret){
					    ret = npd_del_port_promi_mode(eth_g_index,mode,ifnameType, opCode);
						oldMode = ETH_PORT_FUNC_MODE_PROMISCUOUS;
						syslog_ax_eth_port_dbg(" del promi mode ret[%#x]\n",ret);
					}
				}
				else{
					ret = npd_eth_port_interface_check(eth_g_index, &ifIndex);
					if((TRUE == ret) && (~0UI != ifIndex)) {
						 ret = INTERFACE_RETURN_CODE_SUCCESS;
						 ret = npd_del_port_route_mode(eth_g_index, opCode);
	                     oldMode = ETH_PORT_FUNC_IPV4;
						 syslog_ax_eth_port_dbg("npd_eth_port2197:: del route mode ret[%#x]\n",ret);
					}
					else{
						ret = INTERFACE_RETURN_CODE_SUCCESS;
					}
				}				
				if(INTERFACE_RETURN_CODE_SUCCESS == ret){
					ret = npd_set_port_switch_mode(eth_g_index,oldMode);
				}
				if(INTERFACE_RETURN_CODE_SUCCESS != ret){
					if(ETH_PORT_FUNC_IPV4 == oldMode){
						opCode = NPD_INTF_CREATE_INTF;
                        npd_set_port_route_mode(eth_g_index,ifnameType, opCode);
					}
					else if(ETH_PORT_FUNC_MODE_PROMISCUOUS == oldMode){
						npd_set_port_promi_mode(eth_g_index,ifnameType);
					}
				}
			}
			break;
			
		case ETH_PORT_FUNC_IPV4:
            oldMode = ETH_PORT_FUNC_IPV4;
			ret = npd_eth_port_interface_check(eth_g_index,&ifIndex);
			if(TRUE == ret && ifIndex != ~0UI) {
				ret = INTERFACE_RETURN_CODE_ALREADY_THIS_MODE;
				break;
			}
			else {
				ret = npd_check_port_promi_mode( eth_g_index);
				if(TRUE == ret) { /*advanced-routing disable */
					ret = INTERFACE_RETURN_CODE_SUCCESS;					
					opCode = NPD_INTF_ADV_DIS;
					portInfo = npd_get_port_by_index(eth_g_index);
					if(NULL != portInfo){
                        promiInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS];
						if((NULL != promiInfo)&&(promiInfo->subifList.count > 0)){
                            ret = INTERFACE_RETURN_CODE_PORT_HAS_SUB_IF;
						}
					}
					else{
						ret = INTERFACE_RETURN_CODE_ERROR;
					}
					if(INTERFACE_RETURN_CODE_SUCCESS == ret){
						ret= npd_del_port_promi_mode(eth_g_index,mode,ifnameType, opCode);
						oldMode = ETH_PORT_FUNC_MODE_PROMISCUOUS;
						syslog_ax_eth_port_dbg("del promi mode ret[%#x]\n",ret);
					}
				}
				else{
					opCode = NPD_INTF_CREATE_INTF;
					ret = npd_check_port_switch_mode(eth_g_index);
					if(TRUE == ret) {
						ret = INTERFACE_RETURN_CODE_SUCCESS;
						ret = npd_del_port_switch_mode(eth_g_index,mode);
						oldMode = ETH_PORT_FUNC_BRIDGE;
						syslog_ax_eth_port_dbg("del switch mode rc[%#x]\n",ret);
					}
					else{
						ret = INTERFACE_RETURN_CODE_SUCCESS;
					}
				}
				if(INTERFACE_RETURN_CODE_SUCCESS == ret){
					ret = npd_set_port_route_mode(eth_g_index,ifnameType, opCode);
				}
				if(INTERFACE_RETURN_CODE_SUCCESS != ret){
					if(ETH_PORT_FUNC_BRIDGE == oldMode){
                        npd_set_port_switch_mode(eth_g_index,oldMode);
					}
					else if(ETH_PORT_FUNC_MODE_PROMISCUOUS == oldMode){
						npd_set_port_promi_mode(eth_g_index,ifnameType);
					}
				}
			}
			break;
			
		case ETH_PORT_FUNC_MODE_PROMISCUOUS:
            oldMode = ETH_PORT_FUNC_MODE_PROMISCUOUS;
			opCode = NPD_INTF_ADV_EN;
			ret = npd_check_port_promi_mode(eth_g_index);
			if(TRUE == ret) {
				ret = INTERFACE_RETURN_CODE_ALREADY_THIS_MODE;
				break;
			}
			else {				
				ret = INTERFACE_RETURN_CODE_SUCCESS;
				if ((!promis_port_add2_intf)){
				    ret = npd_vlan_interface_check(advanced_routing_default_vid,&ifIndex);
					if((TRUE == ret) && (~0UI != ifIndex)){
                        ret = INTERFACE_RETURN_CODE_DEFAULT_VLAN_IS_L3_VLAN;
						break;
					}
					else {
						ret = INTERFACE_RETURN_CODE_SUCCESS;
					}
					/* not check for vlans which this port tag mode added*/
				}
				if(TRUE != npd_check_vlan_real_exist(advanced_routing_default_vid)){
					ret = INTERFCE_RETURN_CODE_ADVANCED_VLAN_NOT_EXISTS;
				}
				else{
					ret = npd_eth_port_interface_check( eth_g_index,&ifIndex);
					if((TRUE == ret) && (~0UI != ifIndex)) {/* advanced-routing enable */
						ret = npd_del_port_route_mode(eth_g_index, opCode);
						oldMode = ETH_PORT_FUNC_IPV4;
						syslog_ax_eth_port_dbg("del route mode ret[%#x]\n",ret);
						if(INTERFACE_RETURN_CODE_SUCCESS == ret){
						    ret = npd_set_port_promi_mode(eth_g_index,ifnameType);
						}
					}
					else{
						ret = npd_check_port_switch_mode(eth_g_index);
						if(TRUE == ret) {
							ret = npd_del_port_switch_mode(eth_g_index,mode);
							oldMode = ETH_PORT_FUNC_BRIDGE;
							syslog_ax_eth_port_dbg("del switch mode ret[%#x]\n",ret);
						}
						else {
							ret = INTERFACE_RETURN_CODE_SUCCESS;
						}
						
						if(INTERFACE_RETURN_CODE_SUCCESS == ret){
						    ret = npd_set_port_promi_mode(eth_g_index,ifnameType);
						}
					}
				}
				
				if(INTERFACE_RETURN_CODE_SUCCESS != ret){
					if(ETH_PORT_FUNC_IPV4 == oldMode){
						opCode = NPD_INTF_ADV_DIS;
                        npd_set_port_route_mode(eth_g_index,ifnameType, opCode);
					}
					else if(ETH_PORT_FUNC_BRIDGE == oldMode){
					    npd_set_port_switch_mode(eth_g_index,oldMode);
					}
				}
			}
			break;
			
		default:
			syslog_ax_eth_port_dbg("mode type error\n");
			ret = INTERFACE_RETURN_CODE_ERROR;
			break;
	}

	return ret;
}

/**********************************************************************************
 *  npd_eth_port_register_notifier_hook
 *
 *	DESCRIPTION:
 * 		Register ethernet port event notifier callback function
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *
 **********************************************************************************/
int npd_eth_port_register_notifier_hook
(
	void
) 
{
	portNotifier = npd_eth_port_notifier;

	return 0;
}

/**********************************************************************************
 *  npd_eth_port_poll_add_member
 *
 *	DESCRIPTION:
 * 		Add port to participate link status polling.
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - find position to add port
 *		NPD_FAIL - no proper position found for the port
 *
 **********************************************************************************/
int npd_eth_port_poll_add_member
(
	unsigned int eth_g_index
) 
{
	int i = 0;
	unsigned int ret = NPD_SUCCESS;
	
	/* disable link status interrupt on the port */
	ret = npd_eth_port_mask_link_interrupt_set(eth_g_index, 0);
	if(ret) {
		syslog_ax_eth_port_err("mask eth-port %#x link status interrupt error %d\n", eth_g_index, ret);
	}
	
	for(; i < MAX_ETH_GLOBAL_INDEX; i++) {
		if(0 == g_fiber_port_poll[i]) {
			g_fiber_port_poll[i] = eth_g_index;
			return NPD_SUCCESS;
		}
	}

	if(MAX_ETH_GLOBAL_INDEX == i) {
		return NPD_FAIL;
	}
	return ret;
}

/**********************************************************************************
 *  npd_eth_port_poll_add_member
 *
 *	DESCRIPTION:
 * 		delete port from poll ports array to stop link status polling.
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - find position to add port.
 *		NPD_FAIL - port not found in the polling array.
 *
 **********************************************************************************/
int npd_eth_port_poll_del_member
(
	unsigned int eth_g_index
) 
{
	int i = 0;

	for(; i < MAX_ETH_GLOBAL_INDEX; i++) {
		if(eth_g_index == g_fiber_port_poll[i]) {
			g_fiber_port_poll[i] = 0;
			return NPD_SUCCESS;
		}
	}

	if(MAX_ETH_GLOBAL_INDEX == i) {
		return NPD_FAIL;
	}
	return NPD_FAIL;
}

/**********************************************************************************
 *  npd_eth_port_status_polling_thread
 *
 *	DESCRIPTION:
 * 		eth-port status polling thread
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - find position to add port.
 *		
 *
 **********************************************************************************/
void npd_eth_port_status_polling_thread
(
	void
) 
{
	int i = 0;
	unsigned int eth_g_index = 0, ret = NPD_SUCCESS;

	/* tell my thread id*/
	npd_init_tell_whoami((unsigned char*)"SfpPoll",0);
	
	while(1) {
		for(i = 0; i < MAX_ETH_GLOBAL_INDEX; i++) {
			eth_g_index = g_fiber_port_poll[i];
			if(0 == eth_g_index) {
				continue;
			}
			portNotifier(eth_g_index,ETH_PORT_NOTIFIER_LINKPOLL_E);
		}
		/* wait for a while	*/	
		sleep(1);
	}
}


/**********************************************************************************
 *  npd_eth_port_link_reset_thread
 *
 *	DESCRIPTION:
 * 		eth-port link reset thread(each time reset eth-port phy device)
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - find position to add port.
 *		
 *
 **********************************************************************************/
void npd_eth_port_link_reset_thread
(
	void
) 
{
#define NPD_ETHPORT_LINK_RESET_ONOFF "/var/run/npd.port.reset"
#define NPD_ETHPORT_LINK_RESET_LIST "/var/run/npd.port.list"
	unsigned int eth_g_index[128] = {0}, ret = NPD_SUCCESS;
	int lr_fd =  -1, lr_list_fd = -1;	
	char line[256] = {0}, buf[256] = {0}, *endPtr = NULL, c = 0;
	int i = 0, j = 0, len = 0, list_cnt = 0;
	unsigned char flush = 0, hasSlot = 0, hasPort = 0, complete = 0, skip = 0;
	unsigned int index = ~0UI, slot = ~0UI, port = ~0UI;
	unsigned int skip_c = 0, reset_c = 0, load_c = 0;

	/* tell my thread id*/
	npd_init_tell_whoami((unsigned char*)"phyReset",0);
	
	while(1) {
		/* wait for a while*/
		sleep(1);

		if(lr_fd < 0){
			lr_fd = open(NPD_ETHPORT_LINK_RESET_ONOFF, O_RDONLY);
			if(lr_fd < 0)	
				continue; 
		}
		/* fd correctly opened*/
		memset(buf, 0, 256);		
		len = read(lr_fd, buf, 256);
		if(len <= 0 || '0' == buf[0]) {/* skip this process for a while*/
			reset_c = load_c = 0;
			syslog_ax_eth_port_dbg("skip %d phy reset for a while(%s %s)\r\n", 
						++skip_c, len<=0 ? "err":"", '0' == buf[0] ? "flag":"");
			close(lr_fd);
			lr_fd = -1;
			sleep(4);
			continue;
		}
		else if('1' == buf[0]) {/* reset phy for the port list*/
			unsigned char unit = 0, port = 0;
			skip_c = load_c = 0;
			syslog_ax_eth_port_dbg("do %d phy reset\r\n", ++reset_c);			
			for(i = 0; i < list_cnt; i++) {
				if(NPD_SUCCESS == npd_get_devport_by_global_index(eth_g_index[i],&unit,&port)) {
					nam_set_phy_reboot(unit, port);
				}
			}
		}
		else {/* load port list */
			reset_c = skip_c = 0;
			syslog_ax_eth_port_dbg("load %d phy reset port list\r\n", ++load_c);
			if(lr_list_fd < 0) {
				lr_list_fd = open(NPD_ETHPORT_LINK_RESET_LIST, O_RDONLY);
				if(lr_list_fd < 0) 
					continue;
			}
			/* fd correctly opened*/
			memset(buf, 0, 256);
			len = read(lr_list_fd, buf, 256);
			if(len <= 0) {
				syslog_ax_eth_port_dbg("read port list err\r\n");
				close(lr_list_fd);
				lr_list_fd = -1;
				continue;
			}
			syslog_ax_eth_port_dbg("port list:%s\n",buf);
			/* parse port list read	*/
			i = j = list_cnt = 0;
			while(i < len && (c=buf[i++]) != '\0') {
				if(' ' == c || '\n' == c|| '\r' == c) {
					if(j)	flush = 1;
					else	continue;
				}
				else if('-' == c || '/' == c){
					if(hasSlot) skip = 1; /* skip illegal format*/
					hasSlot = flush = 1;
				}
				else {
					line[j++] = c;
				}
			
				if(1 == flush) 
				{
					if(!skip) {
						if(1 == hasSlot && ~0UI == slot) {
							slot = strtoul(line,&endPtr,10);
						}
						else if(~0UI != slot){
							port = strtoul(line, &endPtr, 10);
							hasPort = 1;
						}
						else {
							index = strtoul(line, &endPtr, 10);
							complete = 1;
						}
			
						if(hasSlot && hasPort) {
							index  = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot,port);
							hasSlot = hasPort = 0;
							slot = port = ~0UI;
							complete = 1;
						}
			
						if(complete) {
							eth_g_index[list_cnt++] = index;
							syslog_ax_eth_port_dbg("add port %d:%#0x to phy reset list\r\n", list_cnt,index);
						}
					}
					memset(line, 0, 256);
					j = flush = complete = 0;
				}
			}
		}
	}
}

/**********************************************************************************
 *  npd_eth_port_mask_link_interrupt_set
 *
 *	DESCRIPTION:
 * 		This method enable/disable(unmask/mask) eth-port link status change interrupt event.
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - unmask/mask eth-port link status interrupt successfully.
 *		other - set up failed.
 *
 **********************************************************************************/
unsigned int npd_eth_port_mask_link_interrupt_set
(
	unsigned int eth_g_index,
	unsigned int enable
) 
{	
	unsigned char devNum = 0, portNum = 0;
	unsigned int ret = NPD_SUCCESS;

	ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
	if(NPD_SUCCESS != ret) {
		syslog_ax_eth_port_err("get eth-port %#x device port number error %d\n",eth_g_index,ret);
		return ret;
	}

	ret = nam_ethport_mask_link_interrupt_set(devNum,portNum,enable);

	syslog_ax_eth_port_dbg("\t%s port %#x (%d,%d) link status INTR %s\n",	\
				enable ? "enable":"disable",eth_g_index, devNum, portNum, ret ? "FAIL":"OK");
	return ret;
}

unsigned int npd_eth_port_sc_global_cfg
(
   unsigned int modeType   
)
{
   if((ETH_PORT_STREAM_PPS_E != modeType)&& (ETH_PORT_STREAM_BPS_E != modeType)){
	  return NPD_ERR;
   }
   else{
      scType = modeType;
	  return NPD_OK;
   }
}
/**********************************************************************************
 *  npd_eth_port_get_sc_cfg
 *
 *	DESCRIPTION:
 * 		This method get dlf/mcast/bcast stream type (pps/bps) and values.
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *           strmType -     dlf/mcast/bcast
 *	OUTPUT:
 *		rateTYpe - pps/bps
 *          value      -  pps value or bps value
 * 	RETURN:
 *		NPD_OK - get success
 *          -NPD_FAIL - get failed
 **********************************************************************************/

unsigned int npd_eth_port_get_sc_cfg
(
     unsigned int eth_g_index,
     unsigned int strmType,
     unsigned int* rateType,
     unsigned int* value
)
{
    unsigned int ret = 0;
    struct eth_port_s* eth_port = NULL;
	eth_port_sc_ctrl_t* port_sc = NULL;

    if(!rateType || !value){
      return -NPD_FAIL;
	}
	eth_port = npd_get_port_by_index(eth_g_index);
	if(NULL == eth_port){
       *value = 0;
	   *rateType = ETH_PORT_STREAM_INVALID_E;
	   return -NPD_FAIL;
	}
	else if(NULL ==(port_sc = eth_port->funcs.func_data[ETH_PORT_FUNC_STORM_CONTROL])){
       *value = 0;
	   *rateType = ETH_PORT_STREAM_INVALID_E;
	   return -NPD_FAIL;
	}
	else if ((PORT_STORM_CONTROL_STREAM_DLF == strmType)&&(port_sc->dlf.ppsValid)){
       *value = port_sc->dlf.value.pps;
	   *rateType = ETH_PORT_STREAM_PPS_E;
	   return NPD_OK;
	}
	else if ((PORT_STORM_CONTROL_STREAM_DLF == strmType)&&(port_sc->dlf.bpsValid)){
       *value = port_sc->dlf.value.bps;
	   *rateType = ETH_PORT_STREAM_BPS_E;
	   return NPD_OK;
	}	
	else if ((PORT_STORM_CONTROL_STREAM_BCAST == strmType)&&(port_sc->bcast.ppsValid)){
       *value = port_sc->bcast.value.pps;
	   *rateType = ETH_PORT_STREAM_PPS_E;
	   return NPD_OK;
	}
	else if ((PORT_STORM_CONTROL_STREAM_BCAST == strmType)&&(port_sc->bcast.bpsValid)){
       *value = port_sc->bcast.value.bps;
	   *rateType = ETH_PORT_STREAM_BPS_E;
	   return NPD_OK;
	}
	else if ((PORT_STORM_CONTROL_STREAM_MCAST == strmType)&&(port_sc->mcast.ppsValid)){
       *value = port_sc->mcast.value.pps;
	   *rateType = ETH_PORT_STREAM_PPS_E;
	   return NPD_OK;
	}
	else if ((PORT_STORM_CONTROL_STREAM_MCAST == strmType)&&(port_sc->mcast.bpsValid)){
       *value = port_sc->mcast.value.bps;
	   *rateType = ETH_PORT_STREAM_BPS_E;
	   return NPD_OK;
	}
	*value = 0;
	*rateType = ETH_PORT_STREAM_INVALID_E;
	return -NPD_FAIL;
}

int npd_eth_port_sc_cfg
(
    unsigned int eth_g_index,
    unsigned int scMode,
    unsigned int sctype,
    unsigned int scvalue
)
{
	unsigned int  valid= 0,portSpeed = 0;
	unsigned char dev = 0,port = 0;
	int ret = NPD_SUCCESS;
	enum eth_port_type_e portType = ETH_INVALID;
	struct eth_port_s*  eth_port = NULL;
	eth_port_sc_ctrl_t* port_sc = NULL;

	eth_port = npd_get_port_by_index(eth_g_index);
	if(NULL == eth_port){
         syslog_ax_eth_port_err("g_port(%#x) struct not exist!\n",eth_g_index);
         ret =  NPD_DBUS_ERROR_NO_SUCH_PORT;		 
	}
	else{
		ret = npd_get_devport_by_global_index(eth_g_index,&dev,&port);
		if(0 != ret){
           syslog_ax_eth_port_err("get asic port failed for ret %d\n",ret);
		   return 	NPD_DBUS_ERROR;
		}
		portType = eth_port->port_type;
        if((ETH_FE_TX == portType)||(ETH_FE_FIBER == portType)){
			 portType = ETH_PORT_STREAM_FE_E; 
		}
		else{
             portType = ETH_PORT_STREAM_GE_E;
		}
		ret = nam_get_port_speed(dev,port,&portSpeed);
		if(0 != ret){
           syslog_ax_eth_port_err("get port speed failed for ret %d\n",ret);
		   return 	NPD_DBUS_ERROR;
		}	
	    /*GE port support <0-1488100>,FE port support <0-148810>*/
	    if(((PORT_SPEED_100_E == portSpeed)||((0 == portSpeed)&&(ETH_PORT_STREAM_FE_E == portType)))&&(ETH_PORT_STREAM_PPS_E == scMode)&&(scvalue > 148810)){
		   syslog_ax_eth_port_err("FE port or 100Mbps port does not support value larger than 148810!\n");
	       ret = NPD_DBUS_ERROR_NOT_SUPPORT;
		}
		/*
	    else if((PORT_SPEED_10_E == portSpeed )&&(ETH_PORT_STREAM_PPS_E == scMode)&&(scvalue > 14881)){
		   syslog_ax_eth_port_err("10Mbps port does not support value larger than 14881!\n");
	       ret = NPD_DBUS_ERROR_NOT_SUPPORT;
		}	
		*/
	    else if(((PORT_SPEED_100_E == portSpeed )||((0 == portSpeed)&&(ETH_PORT_STREAM_FE_E == portType)))&&(ETH_PORT_STREAM_BPS_E == scMode)&&(scvalue > 100000000)){
		   syslog_ax_eth_port_err("FE port or 100Mbps portdoes not support value larger than 100000000!\n");
	       ret = NPD_DBUS_ERROR_NOT_SUPPORT;
		}
		/*
	    else if((PORT_SPEED_10_E== portSpeed )&&(ETH_PORT_STREAM_BPS_E == scMode)&&(scvalue > 10000000)){
		   syslog_ax_eth_port_err("10Mbps port does not support value larger than 100000000!\n");
	       ret = NPD_DBUS_ERROR_NOT_SUPPORT;
		}	
		*/
		else {
			if(ETH_PORT_STREAM_PPS_E == scMode){
			   valid = ((((portSpeed == PORT_SPEED_100_E)||((0 == portSpeed)&&(ETH_PORT_STREAM_FE_E == portType)))&&(scvalue == 148810))||(((portSpeed == PORT_SPEED_10000_E)||(portSpeed == PORT_SPEED_1000_E)||((0 == portSpeed)&&(ETH_PORT_STREAM_GE_E == portType)))&&(scvalue == 1488100)))? \
															  0 : 1;
			}

			else if(ETH_PORT_STREAM_BPS_E == scMode){
				valid = ((((portSpeed == PORT_SPEED_100_E)||((0 == portSpeed)&&(ETH_PORT_STREAM_FE_E == portType)))&&(scvalue == 100000000))||(((portSpeed == PORT_SPEED_10000_E)||(portSpeed == PORT_SPEED_1000_E)||((0 == portSpeed)&&(ETH_PORT_STREAM_GE_E == portType)))&&(scvalue == 1000000000)))? \
															   0 : 1;
			   
			}			
			port_sc = eth_port->funcs.func_data[ETH_PORT_FUNC_STORM_CONTROL];
			if(NULL != port_sc){
			   if(PORT_STORM_CONTROL_STREAM_DLF == sctype){
				  if(ETH_PORT_STREAM_PPS_E == scMode){
					 if(port_sc->dlf.bpsValid){
						 ret = NPD_DBUS_ERROR_OPERATE;
					 }
					 else{
						port_sc->dlf.ppsValid = valid;
						port_sc->dlf.value.pps = scvalue;
					 }
				  }
				  else if(ETH_PORT_STREAM_BPS_E == scMode){
					 if(port_sc->dlf.ppsValid){
						 ret = NPD_DBUS_ERROR_OPERATE;
					 }
					 else{
						port_sc->dlf.bpsValid = valid;
						port_sc->dlf.value.bps = scvalue;
					 }
				  }
			   }
			   else if(PORT_STORM_CONTROL_STREAM_MCAST == sctype){
				   if(ETH_PORT_STREAM_PPS_E == scMode){
					  if(port_sc->mcast.bpsValid){
						  ret = NPD_DBUS_ERROR_OPERATE;
					  }
					  else{
						 port_sc->mcast.ppsValid = valid;
						 port_sc->mcast.value.pps = scvalue;
					  }
				   }
				   else if(ETH_PORT_STREAM_BPS_E == scMode){
					  if(port_sc->mcast.ppsValid){
						  ret = NPD_DBUS_ERROR_OPERATE;
					  }
					  else{
						 port_sc->mcast.bpsValid = valid;
						 port_sc->mcast.value.bps = scvalue;
					  }
				   }
			   }
			   else if(PORT_STORM_CONTROL_STREAM_BCAST == sctype){
				   if(ETH_PORT_STREAM_PPS_E == scMode){
					  if(port_sc->bcast.bpsValid){
						  ret = NPD_DBUS_ERROR_OPERATE;
					  }
					  else{
						 port_sc->bcast.ppsValid = valid;
						 port_sc->bcast.value.pps = scvalue;
					  }
				   }
				   else if(ETH_PORT_STREAM_BPS_E == scMode){
					  if(port_sc->bcast.ppsValid){
						  ret = NPD_DBUS_ERROR_OPERATE;
					  }
					  else{
						 port_sc->bcast.bpsValid = valid;
						 port_sc->bcast.value.bps = scvalue;
					  }
				   }
			   }
			   if((!port_sc->dlf.bpsValid)&&(!port_sc->dlf.ppsValid)&&(!port_sc->mcast.bpsValid)&&(!port_sc->mcast.ppsValid)&& \
				  (!port_sc->bcast.bpsValid)&&(!port_sc->bcast.ppsValid)){
					  free(port_sc);
					  eth_port->funcs.func_data[ETH_PORT_FUNC_STORM_CONTROL] = NULL;
					  eth_port->funcs.funcs_run_bitmap &= ~(1<<ETH_PORT_FUNC_STORM_CONTROL);
			   }
			 }
			 else if((NULL == port_sc)&&(valid)){
				 port_sc = (eth_port_sc_ctrl_t *)malloc(sizeof(eth_port_sc_ctrl_t));
				 if(NULL == port_sc ){
					 syslog_ax_eth_port_dbg("init eth-port %d memory malloc error!\n",eth_g_index);
					 ret = NPD_DBUS_ERROR;
				 }
				 else {
					 memset(port_sc,0,sizeof(eth_port_sc_ctrl_t ));
					 
					 if(PORT_STORM_CONTROL_STREAM_DLF == sctype){
						 if(ETH_PORT_STREAM_PPS_E == scMode){
							 port_sc->dlf.bpsValid = 0;
							 port_sc->dlf.ppsValid = 1;
							 port_sc->dlf.value.pps = scvalue;
						 }
						 else if(ETH_PORT_STREAM_BPS_E == scMode){
							 port_sc->dlf.ppsValid = 0;
							 port_sc->dlf.bpsValid = 1;
							 port_sc->dlf.value.bps = scvalue;
						 }
					  }
					  else if(PORT_STORM_CONTROL_STREAM_MCAST == sctype){
						  if(ETH_PORT_STREAM_PPS_E == scMode){
							  port_sc->mcast.bpsValid = 0;
							  port_sc->mcast.ppsValid = 1;
							  port_sc->mcast.value.pps = scvalue;
						  }
						  else if(ETH_PORT_STREAM_BPS_E == scMode){
							  port_sc->mcast.ppsValid = 0;
							  port_sc->mcast.bpsValid = 1;
							  port_sc->mcast.value.bps = scvalue;
						  }
					  }
					 else if(PORT_STORM_CONTROL_STREAM_BCAST == sctype){
						 if(ETH_PORT_STREAM_PPS_E == scMode){
							 port_sc->bcast.bpsValid = 0;
							 port_sc->bcast.ppsValid = 1;
							 port_sc->bcast.value.pps = scvalue;
						 }
						 else if(ETH_PORT_STREAM_BPS_E == scMode){
							 port_sc->bcast.ppsValid = 0;
							 port_sc->bcast.bpsValid = 1;
							 port_sc->bcast.value.bps = scvalue;
						 }
					  }
					 eth_port->funcs.func_data[ETH_PORT_FUNC_STORM_CONTROL]= port_sc;
					 eth_port->funcs.funcs_run_bitmap |= (1 << ETH_PORT_FUNC_STORM_CONTROL);
				 }
			 }
		}
	}
	return ret;
}

unsigned int npd_eth_port_sc_set_check
(
   unsigned int modeType
)
{
    int i = 0,j = 0;
	unsigned int rate_type = 0;
 	for (i = 0 ; i < CHASSIS_SLOT_COUNT; i++ ) {
		unsigned char local_port_count = ETH_LOCAL_PORT_COUNT(i);
		for (j = 0; j < local_port_count; j++ ) {
			unsigned char local_port_no = ETH_LOCAL_INDEX2NO(i,j);
			unsigned int eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(i,j);
			unsigned int local_port_attrmap = 0;
			unsigned int slot_no = CHASSIS_SLOT_INDEX2NO(i);
			unsigned int ret = 0,flag = 0;
			unsigned int rateType = 0,rateValue = 0;
			unsigned char devNum = 0,portNum = 0;
			ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
			if(NPD_SUCCESS != ret) {
				continue;
			}
			/* eth-port storm control*/
			if(NPD_OK == npd_eth_port_get_sc_cfg(eth_g_index,PORT_STORM_CONTROL_STREAM_DLF,&rateType,&rateValue)){
               if(modeType != rateType)
			       return NPD_ERR;
            }
			if(NPD_OK == npd_eth_port_get_sc_cfg(eth_g_index,PORT_STORM_CONTROL_STREAM_MCAST,&rateType,&rateValue)){
				if(modeType != rateType)
					return NPD_ERR;
			}
			if(NPD_OK == npd_eth_port_get_sc_cfg(eth_g_index,PORT_STORM_CONTROL_STREAM_BCAST,&rateType,&rateValue)){
				if(modeType != rateType)
					return NPD_ERR;
			}  
		}
 	}
	return NPD_SUCCESS;
}

/************************************************************************************
 *		NPD dbus operation
 *
 ************************************************************************************/
DBusMessage * npd_dbus_ethports_interface_show_ethport_attr(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned char slot_no = 0, local_port_no = 0, type = 0, slot_index = 0;
	unsigned int port_index = 0;
	int eth_g_index = 0;
	struct eth_port_s portInfo;
	enum module_id_e module_type = MODULE_ID_NONE;

	unsigned int ret = 0;
	unsigned char subcard_type = 0;

	struct timeval tnow;
	struct tm tzone;
	unsigned long last_link_change = 0;
	unsigned int link_keep_time = 0;

	memset(&tnow, 0, sizeof(struct timeval));
	memset(&tzone, 0, sizeof(struct tm));
	gettimeofday (&tnow, &tzone);	
	
	DBusError err;

	memset(&portInfo, 0,sizeof(struct eth_port_s));
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_BYTE,&type,
		DBUS_TYPE_BYTE,&slot_no,
		DBUS_TYPE_BYTE,&local_port_no,
		DBUS_TYPE_UINT32,&port_index,
		DBUS_TYPE_INVALID))) {
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}	
	
	if(0 == type){
		ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		syslog_ax_eth_port_dbg("Show attributes of port %d/%d !\n",slot_no,local_port_no);
		if((ETHPORT_RETURN_CODE_NO_SUCH_PORT == ret) && \
			(PRODUCT_ID != PRODUCT_ID_AX7K) && \
			(PRODUCT_ID != PRODUCT_ID_AX7K_I) && \
			(PRODUCT_ID != PRODUCT_ID_AX5K_E) && \
			(PRODUCT_ID != PRODUCT_ID_AX5608) && \
			(0 == slot_no)) {
			if(0 == (nbm_cvm_query_card_on(local_port_no-1))){
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
	            ret = ETHPORT_RETURN_CODE_ERR_NONE;
				syslog_ax_eth_port_dbg("4626 port(%d,%d) is at its slot ret %d\n",slot_no, local_port_no, ret);				
			}
		}
		else {
			if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
				if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
					slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
					eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
					ret = ETHPORT_RETURN_CODE_ERR_NONE;
				}
			}
		}
	}
	else{
		ret = ETHPORT_RETURN_CODE_ERR_NONE;
		eth_g_index = port_index;
		slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
		slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
		local_port_no	= ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
		local_port_no	= ETH_LOCAL_INDEX2NO(slot_index,local_port_no);
	}

	/*get eth port attrs*/

#if 0
	/* here map slot/port to devNum & virPortNum */
	ret = nam_get_devport_by_slotport(slot_no, local_port_no, &devNum, &virPortNum);
	if(ret != 0){
		syslog_ax_eth_port_dbg(("Npd_dbus::npd_get_devport_by_slotport::get devport Error.\n"));
	}
	syslog_ax_eth_port_dbg(("npd_get_devport_by_slotport::slot_no %d,local_port_no %d,devNum %d,virPortNum %d",\
												slot_no,local_port_no,devNum,virPortNum));
#endif
	/* to do somthing here*/
	if(ETHPORT_RETURN_CODE_ERR_NONE == ret ){
		portInfo.port_type = ETH_PORT_TYPE_FROM_GLOBAL_INDEX(eth_g_index);
		portInfo.attr_bitmap = ETH_PORT_ATTRBITMAP_FROM_GLOBAL_INDEX(eth_g_index);
		portInfo.mtu = ETH_PORT_MTU_FROM_GLOBAL_INDEX(eth_g_index);
		module_type = MODULE_TYPE_ON_SLOT_INDEX(slot_index);

		last_link_change  = ETH_PORT_LINK_TIME_CHANGE_FROM_GLOBAL_INDEX(eth_g_index);
		portInfo.lastLinkChange = tnow.tv_sec - last_link_change;		
		
		if(npd_eslot_check_subcard_module(module_type) && \
			((AX51_GTX == npd_check_subcard_type(PRODUCT_ID, local_port_no)) || \
			 (AX51_SFP == npd_check_subcard_type(PRODUCT_ID, local_port_no)))) {
			syslog_ax_eth_port_dbg("SUBCARD local_port_no %d\n",local_port_no);
			ret = nbm_read_eth_port_info(local_port_no - 1,&portInfo);
		}
        else if((PRODUCT_ID_AX7K == PRODUCT_ID)&& (AX7_CRSMU_SLOT_NUM == slot_no))
        {   /* read CRSMU RGMII ethernet port info*/
			/* TODO: read RGMII ethernet port info from cavium-ethernet driver*/
			local_port_no = ETH_LOCAL_NO2INDEX(slot_index,local_port_no);
			ret = nbm_read_eth_port_info(local_port_no,&portInfo);			
        }
		else if((PRODUCT_ID == PRODUCT_ID_AX7K_I) && ((0==slot_no)||(1 == slot_no)))
		{   /* read CRSMU RGMII ethernet port info*/
			/* TODO: read RGMII ethernet port info from cavium-ethernet driver*/
			local_port_no = ETH_LOCAL_NO2INDEX(slot_index,local_port_no);
			ret = nbm_read_eth_port_info(local_port_no,&portInfo);
		}
		else if((MODULE_ID_AX5_5612I == module_type) && 
			(local_port_no >= 9) && (local_port_no <= 12)) {
			syslog_ax_eth_port_dbg("get port(%d %d) information from nbm\n",slot_no,local_port_no);
			local_port_no = local_port_no - 9;
			ret = nbm_read_eth_port_info(local_port_no,&portInfo);
		}		
		else if (MODULE_ID_AX5_5612E == module_type || MODULE_ID_AX5_5608== module_type)
		{
			syslog_ax_eth_port_dbg("Func: %s Line: %d local_port_no == %d\n", __FUNCTION__, __LINE__, local_port_no);
			ret = nbm_read_eth_port_info(local_port_no - 1,&portInfo);
		}
		else { /* read GE or XG ports info from asic*/
			ret = nam_read_eth_port_info(module_type,slot_no,local_port_no,&portInfo);
		}
		if (ret != 0) {
			syslog_ax_eth_port_dbg("port %d/%d attr get FAIL. \n",slot_no,local_port_no);
			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
		else {
			ret = ETHPORT_RETURN_CODE_ERR_NONE;
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &(portInfo.port_type));

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &(portInfo.attr_bitmap));

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &portInfo.mtu);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &portInfo.lastLinkChange);	
	return reply;
}

DBusMessage * npd_dbus_ethports_interface_show_ethport_ipg(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned char slot_no = 0, local_port_no = 0, type = 0, slot_index = 0;
	unsigned int port_index = 0;
	int eth_g_index = 0;
	unsigned int ret = 0;
    unsigned int port_ipg = 0xc;	
	unsigned char devNum= 0, portNum= 0;
	enum product_id_e productId = PRODUCT_ID; 
	int isBoard = NPD_FALSE;/* 1 -if main board port, 0 - slave board port */
	struct eth_port_s* g_ptr = NULL;
	DBusError err;
	
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_BYTE,&type,
		DBUS_TYPE_BYTE,&slot_no,
		DBUS_TYPE_BYTE,&local_port_no,
		DBUS_TYPE_UINT32,&port_index,
		DBUS_TYPE_INVALID))) {
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

    if((SYSTEM_TYPE == IS_DISTRIBUTED) || (PRODUCT_ID == PRODUCT_ID_AX7K_I))
    {
		/* Do not support ipg set of port on AX71-CRSMU ! */
		if(BOARD_TYPE == BOARD_TYPE_AX71_CRSMU)
		{
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
			port_ipg = 0;
		}
		else
		{
    		/* change index to npd use */
            eth_g_index = port_index - (asic_board->asic_port_start_no -1);
    			
    		ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
    		if(0 == ret) 
    		{
    			ret = nam_get_ethport_ipg(devNum,portNum,&port_ipg);
    			if (ret != 0) 
    			{
    				ret = ETHPORT_RETURN_CODE_ERR_HW;
    				syslog_ax_eth_port_dbg("nam_get_ethport_ipg:: dev %d port %d error. \n",devNum,portNum);
    			}
    			else
    			{
        			ret = ETHPORT_RETURN_CODE_ERR_NONE;	   			
            		syslog_ax_eth_port_dbg("nam_get_ethport_ipg:: dev %d port %d ipg %d. \n",devNum,portNum,port_ipg);	
    			}
    		}
    		else
    		{
    			syslog_ax_eth_port_dbg("npd_get_devport_by_global_index  FAIL. \n");					
    			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
    		}		
		}
    }
	else
	{
    	if(0 == type) {
    		ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
    		if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
    			if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
    				slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
    				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
    				ret = ETHPORT_RETURN_CODE_ERR_NONE;
    			}
    		}
    		if((ETHPORT_RETURN_CODE_NO_SUCH_PORT == ret)&&\
    			(PRODUCT_ID != PRODUCT_ID_AX7K)&&\
    			(PRODUCT_ID != PRODUCT_ID_AX7K_I) && \
    			(PRODUCT_ID != PRODUCT_ID_AX5K_E) && \
    			(PRODUCT_ID != PRODUCT_ID_AX5608) && \
    			0 == slot_no){		 
    			if(0 == (nbm_cvm_query_card_on(local_port_no-1))){
    				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
    	            ret = ETHPORT_RETURN_CODE_ERR_NONE;
    			}		
    		}
    	}
    	else{
    		ret = ETHPORT_RETURN_CODE_ERR_NONE;
    		eth_g_index = port_index;
    	}

    	if(ETHPORT_RETURN_CODE_ERR_NONE == ret) {
    		g_ptr = npd_get_port_by_index(eth_g_index);
    		if(NULL == g_ptr){
    			syslog_ax_eth_port_dbg("get port info errors\n");
    			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
    		}  
    		else {
    			if(((PRODUCT_ID_AX7K_I == productId) || (PRODUCT_ID_AX7K == productId))
    				&& (eth_g_index < 4)) {
    				isBoard = NPD_TRUE;
    			}

    			if(NPD_FALSE == isBoard) {
    				ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
    				if(0 == ret) {
    					ret = nam_get_ethport_ipg(devNum,portNum,&port_ipg);
        				if (ret != 0) {
        					syslog_ax_eth_port_dbg("nam_get_ethport_ipg:: dev %d port %d FAIL. \n",devNum,portNum);
        				}					
    				}
    				else {
    					syslog_ax_eth_port_dbg("npd_get_devport_by_global_index  FAIL. \n");					
    					ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
    				}
    			}
    			else if(NPD_TRUE == isBoard) {
    				ret = ETHPORT_RETURN_CODE_BOARD_IPG;
    			}
    		}
    	}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &port_ipg);

	
	return reply;
}
DBusMessage * npd_dbus_ethports_interface_show_ethport_stat
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	
	DBusMessage* reply;
	DBusMessageIter  iter, iter_array, iter_struct;
	unsigned char slot_no = 0, local_port_no = 0;
	unsigned char portNum = 0, devNum = 0, type = 0;
	unsigned int port_index = 0;
	int eth_g_index = 0, i = 0, g_index = 0 ;
	unsigned int linkuptimes = 0, linkdowntimes = 0;

	struct eth_port_s* g_ptr = NULL;

	struct npd_port_counter *ptr, portPktCount;
	ptr = &portPktCount;
	memset(ptr,0,sizeof(struct npd_port_counter));
	struct port_oct_s *ptrOct, portOctCount;
	ptrOct = &portOctCount;
	memset(ptrOct,0,sizeof(struct port_oct_s));

	unsigned long long tmp = 0;
	enum product_id_e productId = PRODUCT_ID;

	unsigned int ret = 0;
	DBusError err;
	
	syslog_ax_eth_port_dbg("Entering show ethport stat!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_BYTE,&type,
		DBUS_TYPE_BYTE,&slot_no,
		DBUS_TYPE_BYTE,&local_port_no,
		DBUS_TYPE_UINT32,&port_index,
		DBUS_TYPE_INVALID))) {
		syslog_ax_eth_port_err("Unable to get input args\n");
		if (dbus_error_is_set(&err)) {
			syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	syslog_ax_eth_port_dbg("Show ether port detail of port %d/%d !\n",slot_no,local_port_no);

	/* temp for AX7605i */
	if (PRODUCT_ID_AX7K_I == productId)
	{
		#if 1
		eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX((slot_no-1),(local_port_no-1));			
   		syslog_ax_eth_port_dbg("temp for AX71-CRSMU\n");		
		int port_num = local_port_no -1;		
   		syslog_ax_eth_port_dbg("cpu port_num:%d\n",port_num);
		g_ptr = NULL;
		
		#else
		g_ptr = npd_get_port_by_index(eth_g_index);
		if(NULL == g_ptr){
			syslog_ax_eth_port_dbg("get port info errors\n");
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		
		memset(&(g_ptr->counters),0,sizeof(struct eth_port_counter_s));			
		ret = nbm_board_port_pkt_statistic(port_num,ptr,ptrOct);

		if(0 != ret){
			syslog_ax_eth_port_dbg("nbm_board_port_pkt_statistic ERROR\n");
			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
		syslog_ax_eth_port_dbg("save counter info\n");

		ret = npd_eth_port_counter_statistics(eth_g_index,ptr,ptrOct);
		if(0 != ret){
			syslog_ax_eth_port_dbg("save counter info error");
			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
		else {
			ret = ETHPORT_RETURN_CODE_ERR_NONE;
		}

		/*get port link up&down times*/
		linkuptimes = g_ptr->counters.linkupcount;
		linkdowntimes = g_ptr->counters.linkdowncount;
		#endif
	}
	else
	{
          if(0 == type){
    		ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
    		if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
    			if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
    				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
    				ret = ETHPORT_RETURN_CODE_ERR_NONE;
    			}
    		}	
    		if((ETHPORT_RETURN_CODE_NO_SUCH_PORT == ret)&&\
    			(PRODUCT_ID != PRODUCT_ID_AX7K)&&\
    			(PRODUCT_ID != PRODUCT_ID_AX7K_I) && \
    			(PRODUCT_ID != PRODUCT_ID_AX5K_E) && \
    			(PRODUCT_ID != PRODUCT_ID_AX5608) && \
    			0 == slot_no){		 
    			if(0 == (nbm_cvm_query_card_on(local_port_no-1))){
    				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
    	            ret = ETHPORT_RETURN_CODE_ERR_NONE;
    			}		
    		}
    	}
    	else{
    		ret = ETHPORT_RETURN_CODE_ERR_NONE;
    		eth_g_index = port_index;
    	}

    	if(ETHPORT_RETURN_CODE_ERR_NONE == ret){
    		g_ptr = npd_get_port_by_index(eth_g_index);
    		if(NULL == g_ptr){
    			syslog_ax_eth_port_dbg("get port info errors\n");
    			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
    		}
    		else {
    			/*get eth port stat*/
                #if 0
    			for(i =0; i<31; i++) {
    				tmp = *((unsigned long long*)(&(g_ptr->counters)) + i);
    				/*syslog_ax_eth_port_dbg("<<tmp %d value %lld>>\n ",i,tmp);*/
    			}
    			#endif
    			if ((eth_g_index <= 3)&& (PRODUCT_ID_AX7K == productId))
    			{
    				memset(&(g_ptr->counters),0,sizeof(struct eth_port_counter_s));			
    				ret = nbm_board_port_pkt_statistic(eth_g_index,ptr,ptrOct); /* add by baolc */
    			}
    			else if(PRODUCT_ID_AX5K_I == productId) {
    				g_index = eth_g_index - ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9);
    				if(g_index >= 0 && g_index <= 3) {
    					memset(&(g_ptr->counters),0,sizeof(struct eth_port_counter_s));			
    					ret = nbm_board_port_pkt_statistic(g_index,ptr,ptrOct);	
    					syslog_ax_eth_port_dbg("ret %d g_index %d\n",ret, g_index);
    				}
    				else {
    					npd_get_devport_by_global_index(eth_g_index, &devNum,&portNum);
    					ret = nam_asic_port_pkt_statistic(devNum,portNum,ptr,ptrOct);
    				}
    			}
    			else if(PRODUCT_ID_AX5K_E == productId || PRODUCT_ID_AX5608 == productId) {
    				memset(&(g_ptr->counters),0,sizeof(struct eth_port_counter_s));
    				ret = nbm_board_port_pkt_statistic(eth_g_index,ptr,ptrOct);
    				syslog_ax_eth_port_dbg("TX uncastframe : %d mcastframe : %d  bcastframe : %d \n",\
    										ptr->tx.uncastframe,ptr->tx.mcastframe,ptr->tx.bcastframe);
    				syslog_ax_eth_port_dbg("RX uncastframe : %d mcastframe : %d  bcastframe : %d \n",\
    										ptr->rx.uncastframes,ptr->rx.mcastframes,ptr->rx.bcastframes);
    				syslog_ax_eth_port_dbg("ret %d eth_g_index %d\n",ret, eth_g_index);
    			}
    			else
    			{
    				/* add for distributed system,zhangdi 2011-07-14 */
    				if(SYSTEM_TYPE == IS_DISTRIBUTED)
    				{
        				npd_get_devport_by_global_index(eth_g_index, &devNum,&portNum);
    					int port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
    					
    					if(asic_board->asic_eth_ports[port_index].port_type == ETH_XGE_FIBER)
    					{
        				    ret = nam_asic_xg_port_pkt_statistic(devNum,portNum,ptr,ptrOct);
    						syslog_ax_eth_port_dbg("get XG port statistic\n");
    					}
    					else
    					{
        				    ret = nam_asic_port_pkt_statistic(devNum,portNum,ptr,ptrOct);
    						syslog_ax_eth_port_dbg("get GE port statistic\n");						
    					}
    				}
    				else
    				{
    					/* not distributed system and AX71-CRSMU */
        				npd_get_devport_by_global_index(eth_g_index, &devNum,&portNum);
        				ret = nam_asic_port_pkt_statistic(devNum,portNum,ptr,ptrOct);					
    				}
    			}
    			if(0 != ret){
    				syslog_ax_eth_port_dbg("port_index %d,devNum %d,portNum %d,ret %d\n",eth_g_index,devNum,portNum,ret);
    				syslog_ax_eth_port_dbg("nam get port pkt statistic ERROR\n");
    				ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
    			}
    			syslog_ax_eth_port_dbg("save counter info\n");

    			ret = npd_eth_port_counter_statistics(eth_g_index,ptr,ptrOct);
    			if(0 != ret){
    				syslog_ax_eth_port_dbg("save counter info error");
    				ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
    			}
    			else {
    				ret = ETHPORT_RETURN_CODE_ERR_NONE;
    			}

    			/*get port link up&down times*/
    			linkuptimes = g_ptr->counters.linkupcount;
    			linkdowntimes = g_ptr->counters.linkdowncount;
    		}
    	}
	}


	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	if(ETHPORT_RETURN_CODE_ERR_NONE == ret ) {

		dbus_message_iter_open_container (&iter,
										   DBUS_TYPE_ARRAY,
										   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
										   DBUS_TYPE_UINT64_AS_STRING
										   DBUS_TYPE_UINT32_AS_STRING
										   DBUS_TYPE_UINT32_AS_STRING
										   DBUS_STRUCT_END_CHAR_AS_STRING,
										   &iter_array);

		for(i = 0; i<32; i++)
		{
			
			dbus_message_iter_open_container (&iter_array,
											   DBUS_TYPE_STRUCT,
											   NULL,
											   &iter_struct);
			if(NULL == g_ptr){
				dbus_message_iter_append_basic 
											(&iter_struct,
											 DBUS_TYPE_UINT64,
											 &(tmp));

			}
			else {
				tmp = *((unsigned long long*)(&(g_ptr->counters)) + i);
				/*syslog_ax_eth_port_dbg(("<<tmp %d value %lld>>\n ",i,tmp));*/
				dbus_message_iter_append_basic 
											(&iter_struct,
											 DBUS_TYPE_UINT64,
											 &(tmp));

			}
			dbus_message_iter_append_basic (&iter_struct,
									 DBUS_TYPE_UINT32,
									 &linkuptimes);
	
			dbus_message_iter_append_basic (&iter_struct,
									 DBUS_TYPE_UINT32,
									 &linkdowntimes);			
			dbus_message_iter_close_container (&iter_array, &iter_struct);	
		}

		dbus_message_iter_close_container (&iter, &iter_array);

	}

	return reply;
}
DBusMessage * npd_dbus_ethports_interface_show_xg_ethport_stat
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	
	DBusMessage* reply;
	DBusMessageIter  iter,iter_array,iter_struct;

    asic_port_t xgPortInfo[MAX_HSP_COUNT];
	asic_port_t *portPtr = NULL;
	unsigned int stat[MAX_HSP_COUNT][HSP_MIB_ITEM_COUNT_EACH] = {0};	
	enum product_id_e productId = PRODUCT_ID;
	unsigned int hsp_count = 0, i = 0, ret = ETHPORT_RETURN_CODE_ERR_NONE, j = 0;
	struct npd_port_counter statInfo;
	memset(&statInfo,0,sizeof(struct npd_port_counter));
	struct port_oct_s  octDetail;
	memset(&octDetail,0,sizeof(struct port_oct_s));

	memset(xgPortInfo, 0, sizeof(asic_port_t)*MAX_HSP_COUNT);
	
	switch(productId) {
		default:
			hsp_count = 0;
			break;
		case PRODUCT_ID_AX7K:
			hsp_count = 3;
			xgPortInfo[0].devNum = 0;
			xgPortInfo[0].portNum = 26;
			xgPortInfo[1].devNum = 1;
			xgPortInfo[1].portNum = 27;
			xgPortInfo[2].devNum = 1;
			xgPortInfo[2].portNum = 26;
			break;
		case PRODUCT_ID_AX5K:
			hsp_count = 1;
			xgPortInfo[0].devNum = 0;
			xgPortInfo[0].portNum = 0;
			break;
		case PRODUCT_ID_AX5K_I:
			hsp_count = 1;
			xgPortInfo[0].devNum = 0;
			xgPortInfo[0].portNum = 24;			
			break;	
	}

	if(hsp_count) {
		for(i = 0; i < hsp_count; i++) {
			memset(&statInfo, 0, sizeof(struct npd_port_counter));
			memset(&octDetail, 0, sizeof(struct port_oct_s));
			portPtr = &xgPortInfo[i];
			
			/* check if asic port XAUI or Giga */
			if((portPtr->portNum) > 23) { /* XAUI port */
				ret = nam_asic_xg_port_pkt_statistic(portPtr->devNum, \
						portPtr->portNum, &statInfo, &octDetail);
				if(0 != ret){
					syslog_ax_eth_port_dbg("save counter info error");
					ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
				}
				else {
					ret = ETHPORT_RETURN_CODE_ERR_NONE;
				}
				
			}	
			else { /* Giga port */
				ret = nam_asic_port_pkt_statistic(portPtr->devNum, \
						portPtr->portNum, &statInfo, &octDetail);
				if(0 != ret){
					syslog_ax_eth_port_dbg("save counter info error");
					ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
				}
				else {
					ret = ETHPORT_RETURN_CODE_ERR_NONE;
				}
			}

			/* save port mib */
			stat[i][0] = statInfo.rx.uncastframes;
			stat[i][1] = statInfo.rx.bcastframes;
			stat[i][2] = statInfo.rx.mcastframes;
			stat[i][3] = statInfo.rx.fcframe;
			stat[i][4] = statInfo.rx.goodbytesl;
			stat[i][5] = statInfo.rx.goodbytesh;
			stat[i][6] = statInfo.rx.badbytes;
			stat[i][7] = octDetail.badCRC;
			stat[i][8] = statInfo.tx.uncastframe;
			stat[i][9] = statInfo.tx.bcastframe;
			stat[i][10] = statInfo.tx.mcastframe;
			stat[i][11] = statInfo.tx.fcframe;
			stat[i][12] = statInfo.tx.goodbytesl;
			stat[i][13] = statInfo.tx.goodbytesh;
			stat[i][14] = statInfo.rx.badbytes;/**/
			stat[i][15] = statInfo.tx.crcerror_fifooverrun;
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &productId);	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &hsp_count);

	if(ETHPORT_RETURN_CODE_ERR_NONE == ret ) {
		dbus_message_iter_open_container (&iter,
									   DBUS_TYPE_ARRAY,
									   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
									   DBUS_TYPE_UINT32_AS_STRING
									   DBUS_STRUCT_END_CHAR_AS_STRING,
									   &iter_array);
			for(i = 0;i < hsp_count;i++) {
				for(j = 0; j < HSP_MIB_ITEM_COUNT_EACH; j++){
					dbus_message_iter_open_container (&iter_array,
												   DBUS_TYPE_STRUCT,
												   NULL,
												   &iter_struct);
					dbus_message_iter_append_basic 
												(&iter_struct,
												 DBUS_TYPE_UINT32,
												 &(stat[i][j]));
				
					dbus_message_iter_close_container (&iter_array, &iter_struct);	
				}
			}	
	 dbus_message_iter_close_container (&iter, &iter_array);
	}
	
   #if 0
	if(NPD_DBUS_SUCCESS == ret ){
		for(i = 0;i < hsp_count;i++) {
			for(j = 0; j < HSP_MIB_ITEM_COUNT_EACH; j++){
				dbus_message_iter_append_basic(&iter,
											 DBUS_TYPE_UINT32,
									    	 &(stat[i][j]));
			}	
		}
	}
	#endif
	return reply;
}
	
DBusMessage * npd_dbus_ethports_interface_clear_ethport_stat(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned char slot_no = 0, local_port_no = 0;
	unsigned char portNum = 0, devNum = 0, type = 0;
	unsigned port_index = 0;
	int eth_g_index = 0;
	/*struct npd_port_counter *ptr,portPktCount ={{0}};*/
	struct eth_port_s* g_ptr = NULL;
	/*ptr = &portPktCount;*/
	enum product_id_e productId = PRODUCT_ID;

	unsigned int ret = 0;
	DBusError err;
	
	syslog_ax_eth_port_dbg("Entering show ethport stat!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_BYTE,&type,
		DBUS_TYPE_BYTE,&slot_no,
		DBUS_TYPE_BYTE,&local_port_no,
		DBUS_TYPE_UINT32,&port_index,
		DBUS_TYPE_INVALID))) {
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	syslog_ax_eth_port_dbg("Show ether port detail of port %d/%d !\n",slot_no,local_port_no);
	if(PRODUCT_ID_AX7K_I == productId)
	{
		#if 1
		
   		syslog_ax_eth_port_dbg("temp for AX71-CRSMU\n");		
		int port_num = local_port_no -1;	
   		syslog_ax_eth_port_dbg("cpu port_num:%d\n",port_num);
		#else

		eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX((slot_no-1),(local_port_no-1));			
		/*get eth port stat*/
		g_ptr = npd_get_port_by_index(eth_g_index);
		if(NULL == g_ptr){
			syslog_ax_eth_port_dbg("get port info errors\n");
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		
		ret = nbm_board_clear_port_pkt_stat(port_num); 
        if(0 != ret)
		{
			syslog_ax_eth_port_dbg("bm_board_clear_port_pkt_stat ERROR\n");
			ret = ETHPORT_RETURN_CODE_ERR_HW;
		}
		else 
		{
			syslog_ax_eth_port_dbg("clear counter info\n");
			memset(&(g_ptr->counters),0,sizeof(struct eth_port_counter_s));
		}
		#endif
	}
	else
	{
		
    	if(0 == type){
    		ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
    		if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
    			if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
    				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
    				ret = ETHPORT_RETURN_CODE_ERR_NONE;
    			}
    		}
    	}
    	else{
    		ret = ETHPORT_RETURN_CODE_ERR_NONE;
    		eth_g_index = port_index;
    	}

    	if(ETHPORT_RETURN_CODE_ERR_NONE == ret){
    		/*get eth port stat*/
    		g_ptr = npd_get_port_by_index(eth_g_index);
    		if(NULL == g_ptr){
    			syslog_ax_eth_port_dbg("get port info errors\n");
    			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
    		}
    		else {
                if ((eth_g_index <= 3)&& ((PRODUCT_ID_AX7K == productId)||(PRODUCT_ID_AX7K_I == productId)))
                {
    				ret = nbm_board_clear_port_pkt_stat(eth_g_index); /* add by baolc */
                }
    			else
    			{
    				/* add for distributed system,zhangdi 2011-07-14 */
    				if(SYSTEM_TYPE == IS_DISTRIBUTED)
    				{
        				npd_get_devport_by_global_index(eth_g_index, &devNum,&portNum);
    					int port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
    					
    					if(asic_board->asic_eth_ports[port_index].port_type == ETH_XGE_FIBER)
    					{
        				    ret = nam_asic_clear_xg_port_pkt_stat(devNum,portNum);
    						syslog_ax_eth_port_dbg("clear XG port statistic\n");
    					}
    					else
    					{
        				    ret = nam_asic_clear_port_pkt_stat(devNum,portNum);
    						syslog_ax_eth_port_dbg("clear GE port statistic\n");						
    					}
    				}
    				else
    				{			
        				npd_get_devport_by_global_index(eth_g_index, &devNum,&portNum);
        				ret = nam_asic_clear_port_pkt_stat(devNum,portNum);
    				}
    			}
    			if(0 != ret) {
    				syslog_ax_eth_port_dbg("port_index %d,devNum %d,portNum %d,ret %d\n",eth_g_index,devNum,portNum,ret);
    				syslog_ax_eth_port_dbg("nam_asic_port_pkt_statistic ERROR\n");
    				ret = ETHPORT_RETURN_CODE_ERR_HW;
    			}
    			else 
    			{
    				syslog_ax_eth_port_dbg("clear counter info\n");
    				memset(&(g_ptr->counters),0,sizeof(struct eth_port_counter_s));
    			}
    		}
    	}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, 
									 &ret);
	return reply;

}

DBusMessage * npd_dbus_ethports_interface_config_ethport_one(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned char slot_no = 0, local_port_no = 0;
	unsigned int eth_g_index = 0, ret = 0;
	DBusError err;
	
	syslog_ax_eth_port_dbg("Entering config ethport one!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_BYTE,&slot_no,
		DBUS_TYPE_BYTE,&local_port_no,
		DBUS_TYPE_INVALID))) {
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	syslog_ax_eth_port_dbg("configure ether port %d/%d!\n",slot_no,local_port_no);
	
	ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
	
	if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
		if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
			if(SYSTEM_TYPE == IS_DISTRIBUTED)
			{
                eth_g_index = eth_g_index + asic_board->asic_port_start_no -1;
			}
			syslog_ax_eth_port_dbg("index for %d/%d is %#0x\n",slot_no,local_port_no,eth_g_index);
			ret = ETHPORT_RETURN_CODE_ERR_NONE;
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &eth_g_index);

	return reply;
}


DBusMessage* npd_dbus_ethports_ipg(DBusConnection *conn, DBusMessage *msg, void *user_data)
{

	DBusMessage* reply;
	DBusError err;
	DBusMessageIter  iter;

	unsigned char port_ipg = 0xf, devNum= 0, portNum= 0;
	unsigned int eth_g_index = 0, slot_index=0, port_index=0;
	unsigned int ret = 0;
	enum product_id_e productId = PRODUCT_ID; 
	int isBoard = NPD_FALSE;/* 1 -if main board port, 0 - slave board port 	*/
	dbus_error_init(&err);

	if (!(dbus_message_get_args( msg, &err,
					DBUS_TYPE_UINT32,&eth_g_index,
					DBUS_TYPE_BYTE,&port_ipg,
					DBUS_TYPE_INVALID))) {
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
    /* convert the index for configuring eth-port on ASIC zhangdi add for set IPG 2011-10-15 */
    if((SYSTEM_TYPE == IS_DISTRIBUTED) || (PRODUCT_ID == PRODUCT_ID_AX7K_I))
	{
		/* Do not support ipg set of port on AX71-CRSMU ! */
		if(BOARD_TYPE == BOARD_TYPE_AX71_CRSMU)
		{
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else
		{
    		slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
    		port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);

            syslog_ax_eth_port_dbg("slot_index %d ,port_index %d ipg value %d.\n",slot_index,port_index,port_ipg);

    		/* for npd convert the dev port */
            eth_g_index = eth_g_index - (asic_board->asic_port_start_no -1);

    		ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
    		if(NPD_SUCCESS == ret) 
    		{
    			syslog_ax_eth_port_dbg("npd_dbus_ethports_ipg :: set dev %d ,port %d ipg value %d.\n",devNum,portNum,port_ipg);
    			ret = nam_set_ethport_ipg(devNum,portNum,port_ipg);
    			if(0 == ret)
    			{
    				ret = ETHPORT_RETURN_CODE_ERR_NONE;					
    			}				
    			else
    		    {
    				ret = ETHPORT_RETURN_CODE_ERR_HW;
    			}
    		}
    		else
    		{
    			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
    		}

    		/* update the global info of eth-port */
            if(ret == ETHPORT_RETURN_CODE_ERR_NONE)
            {
            	start_fp[slot_index][port_index].ipg = port_ipg; 
                /* sync the change to ram */
                msync(start_fp[slot_index], sizeof(struct global_ethport_s)*BOARD_GLOBAL_ETHPORTS_MAXNUM, MS_SYNC);			
        		npd_asic_ehtport_update_notifier(port_index);
            }
		}
	}
	else
	{
	
    	if(((PRODUCT_ID_AX7K_I == productId) || (PRODUCT_ID_AX7K == productId))
    		&& (eth_g_index < 4)) {
    		isBoard = NPD_TRUE;
    	}
    	else if((PRODUCT_ID_AX5K_I == productId) &&  \
    		(eth_g_index >= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9)) &&  \
    		(eth_g_index <= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,12))) {
    		isBoard = NPD_TRUE;
    		eth_g_index = eth_g_index - ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9);
    	}
    	else if(PRODUCT_ID_AX5K_E == productId || PRODUCT_ID_AX5608== productId) {
    		isBoard = NPD_TRUE;
    	}
    	if(NPD_FALSE == isBoard) {
    		ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
    		if(NPD_SUCCESS == ret) {
    			syslog_ax_eth_port_dbg("npd_dbus_ethports_ipg :: set dev %d ,port %d ipg value %d.\n",devNum,portNum,port_ipg);
    			ret = nam_set_ethport_ipg(devNum,portNum,port_ipg);
    			if((0 != ret) && (0x10 != ret)) {
    				ret = ETHPORT_RETURN_CODE_ERR_HW;
    			}
    			else if(0x10 == ret)
    			{
    				ret = ETHPORT_RETURN_CODE_UNSUPPORT;
    			}
    			else {
    				ret = ETHPORT_RETURN_CODE_ERR_NONE;
    			}
    		}
    		else {
    			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
    		}
    	}
    	else if(NPD_TRUE == isBoard) {
    		syslog_ax_eth_port_dbg("config board %d ipg %d\n ",eth_g_index,port_ipg);
    		ret = nbm_set_ethport_ipg(eth_g_index, port_ipg);
    		if(NPD_SUCCESS != ret) {
    			syslog_ax_eth_port_err("main board not hold out config ipg ");
    		}
    		  ret = ETHPORT_RETURN_CODE_BAD_IPG;
    	}
	}
	
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&ret);

	return reply;


}
DBusMessage* npd_dbus_eth_link_state_config(DBusConnection *conn, DBusMessage *msg, void *user_data)
 {
   	DBusMessage* reply;                             
	DBusError err;
	DBusMessageIter  iter;
	unsigned char	slot,port;
	unsigned int  eth_g_index = 0,slot_index;
	unsigned int value = 0,ret = 0;
	unsigned int port_index = 0;
    int isBoard = NPD_FALSE;
	
    syslog_ax_eth_port_dbg("Entering config port mode!\n");
    dbus_error_init(&err);
	
   if (!(dbus_message_get_args ( msg, &err,
								 DBUS_TYPE_BYTE,&slot,
								 DBUS_TYPE_BYTE,&port,
								 DBUS_TYPE_UINT32,&value,
								 DBUS_TYPE_INVALID))) {
	 syslog_ax_eth_port_err("Unable to get input args ");
	 if (dbus_error_is_set(&err)) {
			 syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			 dbus_error_free(&err);
		 }
	     return NULL;
     }
   	ret = NPD_DBUS_ERROR_NO_SUCH_PORT;	
	if (CHASSIS_SLOTNO_ISLEGAL(slot)) {
		if (ETH_LOCAL_PORTNO_ISLEGAL(slot,port)) {
			slot_index = CHASSIS_SLOT_NO2INDEX(slot);
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot,port);
			syslog_ax_eth_port_dbg("index for %d/%d is %#0x\n",slot,port,eth_g_index);
			ret = NPD_DBUS_SUCCESS;
		}
	}
	if(NPD_DBUS_SUCCESS == ret){
	  syslog_ax_eth_port_dbg("npd_set_port_link_status value %d \n",value);
	  ret = npd_set_port_link_status(eth_g_index,value,isBoard);
	  if(ret != NPD_SUCCESS)
	       syslog_ax_eth_port_dbg("set link status failed\n");
   	}

	 reply = dbus_message_new_method_return(msg);
	 
	 dbus_message_iter_init_append (reply, &iter);
	 
	 dbus_message_iter_append_basic (&iter,
									  DBUS_TYPE_UINT32,&ret);
	 dbus_message_iter_append_basic (&iter,
									  DBUS_TYPE_UINT32,&port_index);

	 
	 return reply;

	 }

DBusMessage* npd_dbus_ethports_interface_config_ports_attr(DBusConnection *conn, DBusMessage *msg, void *user_data){

	DBusMessage* reply;
	DBusError err;
	DBusMessageIter  iter;

	unsigned int type = 0;
	unsigned int port_index = 0, eth_g_index = 0, slot_index = 0, eth_l_index = 0;
	unsigned int slot_no = 0, port_no = 0;
	unsigned int value = 0;
	int ret = 0;
	enum product_id_e productId = PRODUCT_ID; 
	int isBoard = NPD_FALSE;/* 1 -if main board port, 0 - slave board port */
	enum module_id_e  modlue_id = MODULE_ID_NONE;
	struct eth_port_s portInfo;

	syslog_ax_eth_port_dbg("Entering config ports attr!\n");
	memset(&portInfo,0,sizeof(struct eth_port_s));	/* code optimize: Uninitialized scalar variable houxx@autelan.com  2013-1-17 */

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
	                              DBUS_TYPE_UINT32,&type,
								  DBUS_TYPE_UINT32,&port_index,
								  DBUS_TYPE_UINT32,&value,
								  DBUS_TYPE_INVALID)))
	{
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) 
		{
			syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	
    /* convert the index for configuring eth-port on ASIC Jia Lihui 2011.7.19 */
	if(SYSTEM_TYPE == IS_DISTRIBUTED)
	{
        port_index = port_index - (asic_board->asic_port_start_no -1);
	}
	
	syslog_ax_eth_port_dbg("configure ether port %d,value %d\n",port_index,value);
	portInfo.port_type = ETH_PORT_TYPE_FROM_GLOBAL_INDEX(port_index);

	if((PRODUCT_ID_AX7K == productId) && (port_index < 4)) 
	{
		isBoard = NPD_TRUE;
	}
	else if((PRODUCT_ID_AX7K_I == productId) && ((port_index <= 3) || ((port_index >= 64)&&(port_index <= 67))))/* code optimize:Unsigned compared against 0 houxx@autelan.com  2013-1-17 */ 
	{
		isBoard = NPD_TRUE;
	    /* Add for 7605i-SMU on slot 1,zhangdi 2011-05-08  */
		if((port_index >= 64)&&(port_index <= 67))
        {
    	    port_index = port_index	-64;
			/*make port_index[64-67] to [0-3]*/
        }	    
	}
	else if((PRODUCT_ID_AX5K_I == productId) && \
		(port_index >= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9)) &&  \
		(port_index <= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,12))) {
		isBoard = NPD_TRUE;
		port_index = port_index - ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9);
	}
	else if(PRODUCT_ID_AX5K_E == productId || PRODUCT_ID_AX5608== productId) 
	{
		isBoard = NPD_TRUE;
	}
	if(portInfo.port_type == ETH_XGE_FIBER)
	{
		switch(type){
			case DUMOD:
		    case BACKPRE:
			case SPEED:
			case AUTONEGT:
			case AUTONEGTS:
			case AUTONEGTD:
			case AUTONEGTF:
				syslog_ax_eth_port_dbg("xg fiber port can not support these config operation.\n");
				ret = ETHPORT_RETURN_CODE_ETH_GE_SFP;
				goto DOEND;
			case DEFAULT:
    			break;	
			default:		
				break;
		}
	}
	switch(type){ /*GTX or SFP both can operate  */
	    case ADMIN:
			syslog_ax_eth_port_dbg("ADMIN %s\n",value?"enable":"disable");
			ret = npd_set_port_admin_status(port_index,value,isBoard);
			if(ret != ETHPORT_RETURN_CODE_ERR_NONE)
				syslog_ax_eth_port_dbg("set admin status failed\n");
			break;	
			
		case DUMOD:
			syslog_ax_eth_port_dbg("DUMOD %s\n",value?"full":"half");
			ret = npd_set_port_duplex_mode(port_index,value,isBoard);
			if(ret != ETHPORT_RETURN_CODE_ERR_NONE)
				syslog_ax_eth_port_dbg("set port duplex mode failed\n");
			break;
			
		case FLOWCTRL:
			syslog_ax_eth_port_dbg("FLOWCTRL %s\n",value?"enable":"disable");
			ret = npd_set_port_flowCtrl_state(port_index,value,isBoard);
			if(ret != ETHPORT_RETURN_CODE_ERR_NONE)
				syslog_ax_eth_port_dbg("set port flow control failed\n");
			break;
			
		case BACKPRE:
			syslog_ax_eth_port_dbg("BRACKUP %s\n",value?"enable":"disable");
			ret = npd_set_port_backPressure_state(port_index,value,isBoard);
			if(ret != ETHPORT_RETURN_CODE_ERR_NONE)
				syslog_ax_eth_port_dbg("set backPressure failed\n");
			break;
			
		case LINKS:
			syslog_ax_eth_port_dbg("LINKS %s\n",(0x1 == value) ? "UP" : ((0x0 == value) ?"DOWN":"AUTO"));
			ret = npd_set_port_link_status(port_index,value,isBoard);
			if(ret != ETHPORT_RETURN_CODE_ERR_NONE)
				syslog_ax_eth_port_dbg("set link status failed\n");
			break;
			
		case CFGMTU:
			syslog_ax_eth_port_dbg("CFGMTU %d\n",value);
			if(value & 1){
				ret = ETHPORT_RETURN_CODE_BAD_VALUE;
			}
			else {
				ret = npd_set_port_mru(port_index,value,isBoard);
			}
				
			if(ret != ETHPORT_RETURN_CODE_ERR_NONE)
				syslog_ax_eth_port_dbg("set mtu failed\n");
			break;	
			
		case DEFAULT:
			syslog_ax_eth_port_dbg("set default value\n");
			eth_g_index = port_index;
			slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
			modlue_id = MODULE_TYPE_ON_SLOT_INDEX(slot_index);

			ret = npd_set_port_attr_default(port_index,isBoard,modlue_id);
			if(ret != ETHPORT_RETURN_CODE_ERR_NONE){
				syslog_ax_eth_port_dbg("set default value failed\n");
			}
			break;		
		default:
			 syslog_ax_eth_port_dbg("trans  type %d\n",type);
	}
	if((portInfo.port_type == ETH_GE_SFP) || (portInfo.port_type == ETH_GE_FIBER)) 
		{ /*only SFP can operate*/
			if(NPD_TRUE == isBoard) {
				/*auto-negotiation duplex can config when PRODUCT_ID is AX7K*/
				syslog_ax_eth_port_dbg("AUTONEGTD %s\n",value?"enable":"disable");
				ret = npd_set_port_autoNego_duplex(port_index,value,isBoard);
				if(ret != ETHPORT_RETURN_CODE_ERR_NONE)
					syslog_ax_eth_port_dbg("set autoNego duplex failed\n");
		}
		else 
		{
			switch(type) 
			{
				case SPEED:
				case AUTONEGT:
				case AUTONEGTS:
				case AUTONEGTD:
				case AUTONEGTF:
					syslog_ax_eth_port_dbg("fiber port can not support these config operation.\n");
					ret = ETHPORT_RETURN_CODE_ETH_GE_SFP;
					break;
				default:					
					syslog_ax_eth_port_dbg("type %d\n",type);
					break;
			}		
		}
	}
	else 
	{
		switch(type)
		{	/*other port type can operate*/
			case SPEED:
				syslog_ax_eth_port_dbg("SPEED %dM\n",value);
				ret = npd_set_port_speed(port_index,value,isBoard);
				if(ret != ETHPORT_RETURN_CODE_ERR_NONE)
				{
					syslog_ax_eth_port_dbg("set port speed failed\n ");
				}
				break;	
				
			case AUTONEGT:
				syslog_ax_eth_port_dbg("AUTONEGT %s\n",value?"enable":"disable");
				/*status Reg can NOT be written.*/
				/*ret = npd_set_port_autoNego_status(port_index,value);*/ 
				ret = npd_set_port_autoNego_status(port_index,value,isBoard);
				if(ret != ETHPORT_RETURN_CODE_ERR_NONE)
				{
					syslog_ax_eth_port_dbg("set autoNego failed\n");
				}
				break;	
				
			case AUTONEGTS:
				syslog_ax_eth_port_dbg("AUTONEGTS %s\n",value?"enable":"disable");
				ret = npd_set_port_autoNego_speed(port_index,value,isBoard);
				if(ret != ETHPORT_RETURN_CODE_ERR_NONE)
				{
					syslog_ax_eth_port_dbg("set autoNego speed failed\n");
				}
				break;	
				
			case AUTONEGTD:
				syslog_ax_eth_port_dbg("AUTONEGTD %s\n",value?"enable":"disable");
				ret = npd_set_port_autoNego_duplex(port_index,value,isBoard);
				if(ret != ETHPORT_RETURN_CODE_ERR_NONE)
				{
					syslog_ax_eth_port_dbg("set autoNego duplex failed\n");
				}
				break;
				
			case AUTONEGTF:
				syslog_ax_eth_port_dbg("AUTONEGTF %s\n",value?"enable":"disable");
				ret = npd_set_port_autoNego_flowctrl(port_index,value,isBoard);
				if(ret != ETHPORT_RETURN_CODE_ERR_NONE)
				{
					syslog_ax_eth_port_dbg("set autoNego flowctrl failed\n");
				}
				break;	
				
			default:
				 syslog_ax_eth_port_dbg("trans type %d\n",type);
				break;
			}
    }
DOEND:
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,								 
        							 &port_index);
	/* Added by Jia Lihui 2011-6-20 */			
    if(ret == ETHPORT_RETURN_CODE_ERR_NONE)
    {
		slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(port_index);	 
		modlue_id	= MODULE_TYPE_ON_SLOT_INDEX(slot_index);
    	slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
    	eth_l_index	= ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(port_index);
    	port_no	= ETH_LOCAL_INDEX2NO(slot_index, eth_l_index);	 
    	ret = nam_read_eth_port_info(modlue_id,slot_no,port_no,&portInfo);
    	if(NPD_SUCCESS != ret) 
		{
    		syslog_ax_eth_port_err("read ASIC port(%d %d) infomation error.\n",slot_no, port_no);
    	}	
		syslog_ax_eth_port_dbg("read tmp_portInfo.attr_bitmap = %#x\n",portInfo.attr_bitmap);
    	syslog_ax_eth_port_dbg("config port(%d %d) and notifier sem to udpate port info\n",slot_no,port_no);

    	start_fp[slot_index][port_no-1].attr_bitmap = portInfo.attr_bitmap;
    	start_fp[slot_index][port_no-1].mtu = portInfo.mtu;
		
        /* sync the change to ram */
        msync(start_fp[slot_index], sizeof(struct global_ethport_s)*BOARD_GLOBAL_ETHPORTS_MAXNUM, MS_SYNC);			
		npd_asic_ehtport_update_notifier(port_index);
    }

	return reply;
}

DBusMessage* npd_dbus_ethports_interface_config_ports_vct(DBusConnection *conn, DBusMessage *msg, void *user_data){

	DBusMessage* reply;
	DBusError err;
	DBusMessageIter  iter;

	unsigned char slot_no = 0, port_no = 0;
	unsigned int g_index= ~0, port_index = 0, mode = 0;
	int ret = 0,isBoard = NPD_FALSE;/* 1 -if main board port, 0 - slave board port */
	enum product_id_e productId = PRODUCT_ID; 
	enum module_id_e  modlue_id = MODULE_ID_NONE;
	struct eth_port_s *portPtr = NULL;
	struct eth_vct_info *vct_info = NULL;
	unsigned char devNum = 0, portNum = 0;

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_BYTE,&slot_no,
								DBUS_TYPE_BYTE,&port_no,
								DBUS_TYPE_UINT32,&port_index,
								DBUS_TYPE_UINT32,&mode,
								DBUS_TYPE_INVALID))) 
	{
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) 
		{
			syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	
	if (mode)
	{
       g_index = port_index;   
	}
	else 
	{
		if (CHASSIS_SLOTNO_ISLEGAL(slot_no))
		{
			if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no, port_no)) 
			{
		    	g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
			}
		}
	}
	
	/* when productid is PRODUCT_ID_AU3K_BCM, port 49 50 51 52 
	  is on phy 5482, this phy do not surport vct, so get these ports out*/
	if (PRODUCT_ID_AU3K_BCM == productId) 
	{
		if (g_index > 0x2f) 
		{
			g_index = ~0;
			ret = NPD_DBUS_ERROR;
		}
	}
	
	if (~g_index) 
	{
		if((PRODUCT_ID_AX7K == productId) && (g_index < 4)) 
		{
			isBoard = NPD_TRUE;
		}
		else if((PRODUCT_ID_AX5K_I == productId) &&	(g_index >= 8) && (g_index <= 11)) 
		{
			isBoard = NPD_TRUE;
			g_index = g_index - 8;
		}
	
		portPtr = npd_get_port_by_index(g_index);

		if ((NULL == portPtr) || (portPtr->port_type == ETH_GE_SFP)|| (portPtr->port_type == ETH_GE_FIBER)) 
		{ /*only ETH_GTX can operate*/
			/*can not suport fiber */
			ret = NPD_DBUS_ERROR;
			syslog_ax_eth_port_err("Unable to get port by index ");	/* code optimize: Dereference after null check houxx@autelan.com  2013-1-17 */
		}
		else 
		{
			ret = npd_get_devport_by_global_index(g_index,&devNum,&portNum);
			if(0 != ret)
			{
				ret = NPD_DBUS_ERROR;
				syslog_ax_eth_port_err("Unable to get devport by global index ");	/* code optimize: Dereference after null check houxx@autelan.com  2013-1-17 */
			}
			else
			{
				portPtr->funcs.funcs_cap_bitmap |= (1<< ETH_PORT_FUNC_OCT_INFO);
				portPtr->funcs.funcs_run_bitmap |= (1<< ETH_PORT_FUNC_OCT_INFO);
				
				vct_info = portPtr->funcs.func_data[ETH_PORT_FUNC_OCT_INFO];
				if(NULL == vct_info) 
				{
					vct_info = (struct eth_group_info*)malloc(sizeof(struct eth_group_info));
					if(NULL != vct_info) 
					{
						memset(vct_info,0,sizeof(struct eth_group_info));
					}
					else
					{
						ret = NPD_DBUS_ERROR;
						syslog_ax_eth_port_err("memmory malloc error ");	/* code optimize: Dereference after null check houxx@autelan.com  2013-1-17 */
					}
				}
				if(ret == NPD_DBUS_SUCCESS)
				{
					ret = nam_vct_phy_enable(devNum, portNum);
					if (!ret) 
					{				
						vct_info->vct_isable = 1;
						portPtr->funcs.func_data[ETH_PORT_FUNC_OCT_INFO] = vct_info;		
					}
					else
					{
						free(vct_info);
					}
				}
			}
		}
	}
	else 
	{
		ret = NPD_DBUS_ERROR;
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &g_index);

	return reply;
}

DBusMessage* npd_dbus_ethports_interface_read_ports_vct(DBusConnection *conn, DBusMessage *msg, void *user_data){

	DBusMessage* reply;
	DBusError err;
	DBusMessageIter  iter;

	unsigned char slot_no = 0, port_no = 0;
	unsigned int g_index= ~0, port_index = 0, mode = 0, len = 0;
	int ret = 0,isBoard = NPD_FALSE;/* 1 -if main board port, 0 - slave board port */
	enum product_id_e productId = PRODUCT_ID; 
	enum module_id_e  modlue_id = MODULE_ID_NONE;
	struct eth_port_s *portPtr = NULL;
	struct eth_vct_info *vct_info = NULL;
	unsigned char devNum = 0, portNum = 0;
	unsigned short state = 0;

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_BYTE,&slot_no,
								DBUS_TYPE_BYTE,&port_no,
								DBUS_TYPE_UINT32,&port_index,
								DBUS_TYPE_UINT32,&mode,
								DBUS_TYPE_INVALID))) {
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	
	if (mode){
       g_index = port_index;   
	}
	else {
		if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
			if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no, port_no)) {
		    	g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
			}
		}
	}
	
	if (~g_index) {
		if((PRODUCT_ID_AX7K == productId) && (g_index < 4)) {
			isBoard = NPD_TRUE;
		}
		else if((PRODUCT_ID_AX5K_I == productId) && 
			(g_index >= 8) && (g_index <= 11)) {
			isBoard = NPD_TRUE;
			g_index = g_index - 8;
		}
		
		portPtr = npd_get_port_by_index(g_index);
		
		if ((NULL == portPtr) || (portPtr->port_type == ETH_GE_SFP) 
			|| (portPtr->port_type == ETH_GE_FIBER)) { /*only ETH_GTX can operate*/
			/*can not suport fiber */
			ret = NPD_DBUS_ERROR;
		}
		else {
			ret = npd_get_devport_by_global_index(g_index,&devNum,&portNum);
			if(0 != ret) {
				ret = NPD_DBUS_ERROR;
			}
			
			vct_info = portPtr->funcs.func_data[ETH_PORT_FUNC_OCT_INFO];
			if ((NULL == vct_info) || !(vct_info->vct_isable)) {
				ret = NPD_DBUS_ERROR;
			}
			else {
				ret = nam_vct_phy_read(devNum, portNum, &state, &len);
				if (!ret) {
					vct_info->vct_isable = 0;
					portPtr->funcs.func_data[ETH_PORT_FUNC_OCT_INFO] = vct_info;
				}
			}	
		}	
	}
	else {
		ret = NPD_DBUS_ERROR;
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &g_index);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT16,
									 &state);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &len);

	return reply;
}
DBusMessage * npd_dbus_ethports_interface_show_ethport_list
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
    if((SYSTEM_TYPE == IS_DISTRIBUTED) || (PRODUCT_ID == PRODUCT_ID_AX7K_I))
    {
    	DBusMessage* reply;
    	DBusMessageIter  iter;
    	DBusMessageIter  iter_array;
    	DBusError err;
    	int j = 0;
    	unsigned int ret = 0;
    	unsigned char slot_count = CHASSIS_SLOT_COUNT;  /* force this to 3,for 12G12S is 1 */

    	struct timeval tnow;
    	struct tm tzone;
    	unsigned long last_link_change = 0;
    	unsigned int link_keep_time = 0;
        unsigned int slotno =	0;
        int octeon_or_asic;
    	slotno = SLOT_ID;
		unsigned int slotindex =LOCAL_CHASSIS_SLOT_INDEX;

        unsigned int local_port_count = ETH_LOCAL_PORT_COUNT(slotindex); 
    	
    	memset(&tnow, 0, sizeof(struct timeval));
    	memset(&tzone, 0, sizeof(struct tm));
    	gettimeofday (&tnow, &tzone);
    	
    	syslog_ax_eth_port_dbg("Entering show ethport list!\n");
    	syslog_ax_eth_port_dbg("local board slotno = %d\n", slotno);	
    	syslog_ax_eth_port_dbg("local board local_port_count = %d\n", local_port_count);	
		
    	reply = dbus_message_new_method_return(msg);
    	
    	syslog_ax_eth_port_dbg("dbus_message_new_method_return!\n");
    	dbus_message_iter_init_append (reply, &iter);
    	/* Total slot count*/
    	syslog_ax_eth_port_dbg("dbus_message_iter_init_append!\n");
		
		dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &slotno);
		dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &local_port_count);

		dbus_message_iter_open_container (&iter,
										DBUS_TYPE_ARRAY,
										DBUS_STRUCT_BEGIN_CHAR_AS_STRING
										DBUS_TYPE_INT32_AS_STRING
										DBUS_TYPE_BYTE_AS_STRING
										DBUS_TYPE_BYTE_AS_STRING
										DBUS_TYPE_UINT32_AS_STRING
										DBUS_TYPE_UINT32_AS_STRING
										DBUS_TYPE_UINT32_AS_STRING
										DBUS_STRUCT_END_CHAR_AS_STRING,
										&iter_array);

	    syslog_ax_eth_port_dbg("dbus_message_iter_open_container!\n");	
		for (j = 0; j < local_port_count; j++) 
		{

			DBusMessageIter iter_struct;
			enum module_id_e module_type;

		    module_type	= MODULE_TYPE_ON_SLOT_INDEX(slotindex);
			switch(module_type)
			{
			case MODULE_ID_AX7I_CRSMU:
				octeon_or_asic = ax71_crsmu_octeon_or_asic_mapArr[j];
				break;
			case MODULE_ID_AX71_2X12G12S:
				octeon_or_asic = ax71_2x12g12s_octeon_or_asic_mapArr[j];
				break;
			case MODULE_ID_AX81_AC12C:
				octeon_or_asic = ax81_ac12c_octeon_or_asic_mapArr[j];	
				break;	
			case MODULE_ID_DISTRIBUTED:
				octeon_or_asic = 1;	
				break;		
			default:
                syslog_ax_eth_port_dbg("We are not interested in module_type %d\n", module_type);
                break;
			}
			
            dbus_message_iter_open_container (&iter_array, DBUS_TYPE_STRUCT, NULL, &iter_struct);

            dbus_message_iter_append_basic(&iter_struct, DBUS_TYPE_INT32, &(octeon_or_asic));
			
			if(octeon_or_asic != ETH_ASIC_PORT)
			{
				syslog_ax_eth_port_err("octeon_or_asic != ETH_ASIC_PORT\n");				
			}
		
    		unsigned char  local_port_no = ETH_LOCAL_INDEX2NO(slotindex, j);
    		unsigned char local_port_type = ETH_PORT_TYPE_FROM_LOCAL_INDEX(slotindex, j);
    		unsigned int local_port_attrmap = ETH_PORT_ATTRBITMAP_FROM_LOCAL_INDEX(slotindex, j);
    		unsigned int local_port_mtu = ETH_PORT_MTU_FROM_LOCAL_INDEX(slotindex, j);
    		last_link_change  = ETH_PORT_LINK_TIME_CHANGE_FROM_LOCAL_INDEX(slotindex, j);
    	
			unsigned int ret = 0;
			struct eth_port_s portInfo;
			memset(&portInfo, 0, sizeof(struct eth_port_s));
			
			ret = nam_read_eth_port_info(module_type,slotno,local_port_no,&portInfo);
    		if(NPD_SUCCESS != ret) {
    			syslog_ax_eth_port_err("read ASIC port(%d %d) infomation error.\n",slotno, j);
    		}	
					
    		local_port_attrmap = portInfo.attr_bitmap;
    		local_port_mtu = portInfo.mtu;

			link_keep_time = tnow.tv_sec - last_link_change;
			syslog_ax_eth_port_dbg("port(%d %d) local_port_no %d port_type %d\n",slotno,j,local_port_no,local_port_type);
			syslog_ax_eth_port_dbg("port %d/%d attr get time %u. \n",slotno,local_port_no,link_keep_time);

			dbus_message_iter_append_basic(&iter_struct, DBUS_TYPE_BYTE, &(local_port_no));
			dbus_message_iter_append_basic(&iter_struct, DBUS_TYPE_BYTE, &(local_port_type));
			dbus_message_iter_append_basic(&iter_struct, DBUS_TYPE_UINT32, &(local_port_attrmap));
			dbus_message_iter_append_basic(&iter_struct, DBUS_TYPE_UINT32, &(local_port_mtu));
			dbus_message_iter_append_basic(&iter_struct, DBUS_TYPE_UINT32, &(link_keep_time));			

			dbus_message_iter_close_container (&iter_array, &iter_struct);
		}
		/* If there is ethernet port in extend slot , add them here */

    	syslog_ax_eth_port_dbg("get eth port end of for\n");
    	dbus_message_iter_close_container (&iter, &iter_array);
    	syslog_ax_eth_port_dbg("dbus_message_iter_close_container!\n");	
    	return reply;
    }
    else
    {
    	DBusMessage* reply;
    	DBusMessageIter  iter;
    	DBusMessageIter  iter_array;
    	int i = 0;
    	unsigned char ret = 0;
    	unsigned char slot_count = CHASSIS_SLOT_COUNT;
    	struct timeval tnow;
    	struct tm tzone;
    	unsigned long last_link_change = 0;
    	unsigned int link_keep_time = 0;

    	memset(&tnow, 0, sizeof(struct timeval));
    	memset(&tzone, 0, sizeof(struct tm));
    	gettimeofday (&tnow, &tzone);
    	
    	syslog_ax_eth_port_dbg("Entering show ethport list!\n");
    	
    	reply = dbus_message_new_method_return(msg);
    	
    	syslog_ax_eth_port_dbg("dbus_message_new_method_return!\n");
    	dbus_message_iter_init_append (reply, &iter);
    	/* Total slot count*/
    	syslog_ax_eth_port_dbg("dbus_message_iter_init_append!\n");
    	dbus_message_iter_append_basic (&iter,
    									 DBUS_TYPE_BYTE,
    									 &slot_count);

    	syslog_ax_eth_port_dbg("dbus_message_iter_append_basic!slot_count::%d\n",slot_count);
    	dbus_message_iter_open_container (&iter,
    									DBUS_TYPE_ARRAY,
    									DBUS_STRUCT_BEGIN_CHAR_AS_STRING
    									DBUS_TYPE_BYTE_AS_STRING
    									DBUS_TYPE_BYTE_AS_STRING
    									DBUS_TYPE_ARRAY_AS_STRING
    									DBUS_STRUCT_BEGIN_CHAR_AS_STRING
    									DBUS_TYPE_BYTE_AS_STRING
    									DBUS_TYPE_BYTE_AS_STRING
    									DBUS_TYPE_UINT32_AS_STRING
    									DBUS_TYPE_UINT32_AS_STRING
    									DBUS_TYPE_UINT32_AS_STRING
    									DBUS_STRUCT_END_CHAR_AS_STRING
    									DBUS_STRUCT_END_CHAR_AS_STRING,
    									&iter_array);

    	syslog_ax_eth_port_dbg("dbus_message_iter_open_container!\n");	
    	for (i = 0; i < slot_count; i++ ) {
    	/* Array of slot port information
    		slot no
    		total port count
    		Array of Port Infos.
    			port no
    			port type
    			port attriute_bitmap
    			port MTU
    	*/
    		DBusMessageIter iter_struct;
    		DBusMessageIter iter_sub_array;
    		int j = 0;
    		unsigned char slotno =	CHASSIS_SLOT_INDEX2NO(i);
    		unsigned char local_port_count = ETH_LOCAL_PORT_COUNT(i);
    		syslog_ax_eth_port_dbg("port slotnoL%d,local_port_count:%d\n",slotno,local_port_count);

    		dbus_message_iter_open_container (&iter_array,
    										   DBUS_TYPE_STRUCT,
    										   NULL,
    										   &iter_struct);
    		dbus_message_iter_append_basic
    				(&iter_struct,
    				  DBUS_TYPE_BYTE,
    				  &(slotno));
    		
    		dbus_message_iter_append_basic
    				(&iter_struct,
    				  DBUS_TYPE_BYTE,
    				  &(local_port_count));
    		
    		dbus_message_iter_open_container (&iter_struct,
    										DBUS_TYPE_ARRAY,
    										DBUS_STRUCT_BEGIN_CHAR_AS_STRING
    										DBUS_TYPE_BYTE_AS_STRING
    										DBUS_TYPE_BYTE_AS_STRING
    										DBUS_TYPE_UINT32_AS_STRING
    										DBUS_TYPE_UINT32_AS_STRING
    										DBUS_TYPE_UINT32_AS_STRING
    										DBUS_STRUCT_END_CHAR_AS_STRING,
    										&iter_sub_array);
    		
    		for (j = 0; j < local_port_count; j++ ) {
    	/*		
    			Array of Port Infos.
    					port no
    					port type
    					port attriute_bitmap
    					port MTU
    	*/
    			DBusMessageIter iter_sub_struct;

    			unsigned char local_port_no = ETH_LOCAL_INDEX2NO(i,j);
    			unsigned char local_port_type = ETH_PORT_TYPE_FROM_LOCAL_INDEX(i,j);
    			unsigned int local_port_attrmap = ETH_PORT_ATTRBITMAP_FROM_LOCAL_INDEX(i,j);
    			unsigned int local_port_mtu = ETH_PORT_MTU_FROM_LOCAL_INDEX(i,j);
    			last_link_change  = ETH_PORT_LINK_TIME_CHANGE_FROM_LOCAL_INDEX(i,j);

    	
    			unsigned int ret = 0;
    			unsigned char subcard_type = 0;
    			struct eth_port_s portInfo ;
    			enum module_id_e module_type;
    			memset(&portInfo, 0, sizeof(struct eth_port_s));
    			syslog_ax_eth_port_dbg("port(%d %d) local_port_no %d port_type %d\n",i,j,local_port_no,local_port_type);
    			module_type = MODULE_TYPE_ON_SLOT_INDEX(i);
    			if(npd_eslot_check_subcard_module(module_type) && \
    				((AX51_GTX == npd_check_subcard_type(PRODUCT_ID, local_port_no)) || \
    				 (AX51_SFP == npd_check_subcard_type(PRODUCT_ID, local_port_no)))) {
    				ret = nbm_read_eth_port_info(j,&portInfo);
    				last_link_change = nbm_get_link_timestamp(j);
    				if(NPD_SUCCESS != ret) {
    					syslog_ax_eth_port_err("read subcard port(%d %d) infomation error.\n",i,j);
    				}
    				local_port_type = portInfo.port_type;	
    			}
    			else if((PRODUCT_ID == PRODUCT_ID_AX7K) && (AX7_CRSMU_SLOT_NUM == i)) { /* read CRSMU RGMII ethernet port info*/
    				/* TODO: read RGMII ethernet port info from cavium-ethernet driver*/
    				ret = nbm_read_eth_port_info(j,&portInfo);
    				last_link_change = nbm_get_link_timestamp(j);
    				
    				if(NPD_SUCCESS != ret) {
    					syslog_ax_eth_port_err("read AX7 port(%d %d) infomation error.\n",i,j);
    				}		
    			    local_port_type = portInfo.port_type;		
    			}
    			else if((PRODUCT_ID == PRODUCT_ID_AX7K_I) && ((0 == i)||(1 == i))) { /* read CRSMU RGMII ethernet port info*/
    				/* TODO: read RGMII ethernet port info from cavium-ethernet driver*/
    				ret = nbm_read_eth_port_info(j,&portInfo);
    				last_link_change = nbm_get_link_timestamp(j);
    				
    				if(NPD_SUCCESS != ret) {
    					syslog_ax_eth_port_err("read AX7 port(%d %d) infomation error.\n",i,j);
    				}		
    			    local_port_type = portInfo.port_type;		
    			}			
    			else if((MODULE_ID_AX5_5612I == module_type) && 
    				(local_port_no >= 9) && (local_port_no <= 12)) {				
    				syslog_ax_eth_port_dbg("get port(%d %d) information from nbm\n",slotno,local_port_no);
    				local_port_no = local_port_no - 9;/*portNum should be 0-3 for nbm*/
    				ret = nbm_read_eth_port_info(local_port_no,&portInfo);
    				last_link_change = nbm_get_link_timestamp(local_port_no);

    				if(NPD_SUCCESS != ret) {
    					syslog_ax_eth_port_err("read AX5 port(%d %d) infomation error.\n",i,local_port_no + 9);
    				}	
    				local_port_type = portInfo.port_type;	
    				local_port_no = local_port_no + 9;/*portNum turn back to dcli*/
    			}			
    			else if(MODULE_ID_AX5_5612E== module_type || MODULE_ID_AX5_5608== module_type){
    				ret = nbm_read_eth_port_info(j,&portInfo);
    				last_link_change = nbm_get_link_timestamp(j);
    				
    				if(NPD_SUCCESS != ret) {
    					syslog_ax_eth_port_err("read 5612E port(%d %d) infomation error.\n",i,j);
    				}		
    			    local_port_type = portInfo.port_type;		
    			}
    			else { /* read GE or XG ports info from asic*/
    				syslog_ax_eth_port_dbg("get eth port %d/%d info\n",i,j);

    				if(MODULE_ID_SUBCARD_TYPE_B == module_type) {
    					ret = npd_check_subcard_type(PRODUCT_ID_AU4K, local_port_no);
    					if(NPD_FALSE == ret) {
    						syslog_ax_eth_port_dbg("4626 port(%d,%d) is not at its slot\n",i,j);
    						local_port_type = ETH_MAX;
    					}
    				}
    				else {
    					ret = nam_read_eth_port_info(module_type,slotno,local_port_no,&portInfo);
    				}
    			}
    			if (ret != 0) {
    				syslog_ax_eth_port_dbg("port %d/%d attr get FAIL. \n",i,local_port_no);
    			}
    			else {
    				/*local_port_type		= portInfo.port_type;*/
    				local_port_attrmap	= portInfo.attr_bitmap;
    				local_port_mtu		= portInfo.mtu;
    			}

    			link_keep_time = tnow.tv_sec - last_link_change;
    			syslog_ax_eth_port_dbg("port %d/%d attr get time %u. \n",i,local_port_no,link_keep_time);
    			dbus_message_iter_open_container (&iter_sub_array,
    											   DBUS_TYPE_STRUCT,
    											   NULL,
    											   &iter_sub_struct);
    			dbus_message_iter_append_basic
    					(&iter_sub_struct,
    					  DBUS_TYPE_BYTE,
    					  &(local_port_no));
    			dbus_message_iter_append_basic
    					(&iter_sub_struct,
    					  DBUS_TYPE_BYTE,
    					  &(local_port_type));
    			dbus_message_iter_append_basic
    					(&iter_sub_struct,
    					  DBUS_TYPE_UINT32,
    					  &(local_port_attrmap));
    			dbus_message_iter_append_basic
    					(&iter_sub_struct,
    					  DBUS_TYPE_UINT32,
    					  &(local_port_mtu));
    			dbus_message_iter_append_basic
    					(&iter_sub_struct,
    					  DBUS_TYPE_UINT32,
    					  &(link_keep_time));			
    			dbus_message_iter_close_container (&iter_sub_array, &iter_sub_struct);

    		}
    		/* If there is ethernet port in extend slot , add them here */
    		dbus_message_iter_close_container (&iter_struct, &iter_sub_array);
    		
    		
    		dbus_message_iter_close_container (&iter_array, &iter_struct);
    		
    	}
    	syslog_ax_eth_port_dbg("get eth port end of for\n");
    	dbus_message_iter_close_container (&iter, &iter_array);
    	syslog_ax_eth_port_dbg("dbus_message_iter_close_container!\n");	
    	return reply;
    }   		
    
}


/****************************************************
 *
 * RETURN:
 *		COMMON_RETURN_CODE_NULL_PTR
 *		INTERFACE_RETURN_CODE_SUCCESS	
 *		INTERFACE_RETURN_CODE_ERROR
 *		INTERFACE_RETURN_CODE_NO_SUCH_PORT
 *		INTERFACE_RETURN_CODE_PORT_HAS_SUB_IF 
 *		INTERFACE_RETURN_CODE_NAM_ERROR 
 *		INTERFACE_RETURN_CODE_CHECK_MAC_ERROR
 *		INTERFACE_RETURN_CODE_FDB_SET_ERROR
 *		INTERFACE_RETURN_CODE_FD_ERROR
 *		INTERFACE_RETURN_CODE_IOCTL_ERROR
 *		INTERFACE_RETURN_CODE_MAC_GET_ERROR
 *
 *******************************************************/

DBusMessage * npd_dbus_port_lacp_function_enable(DBusConnection *conn, DBusMessage *msg, void *user_data){
	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned int	mode;
	int ret = INTERFACE_RETURN_CODE_SUCCESS;
	unsigned char	slot,port;
	DBusError err;
	unsigned int  eth_g_index = 0;
	unsigned int ifIndex = ~0UI;
	int ifnameType = 0;
	unsigned char endis = 0;
	unsigned char ifname[MAX_IFNAME_LEN + 1] = {0};
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32,&ifnameType,
									DBUS_TYPE_BYTE,&slot,
									DBUS_TYPE_BYTE,&port,
									DBUS_TYPE_BYTE,&endis,
									DBUS_TYPE_INVALID))) 
	{
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) 
		{
			syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	syslog_ax_eth_port_dbg("Entering config port %d/%d type %d interface mode!\n", \
							slot, port, ifnameType);	
	if (SLOT_ID == slot) 
	{
		eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot,port);
		syslog_ax_eth_port_dbg("index for %d/%d is %#0x\n",slot,port,eth_g_index);
		ret = INTERFACE_RETURN_CODE_SUCCESS;
	}
	else
	{
		ret = INTERFACE_RETURN_CODE_NO_SUCH_PORT;
	}
	eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot,port);
	if(INTERFACE_RETURN_CODE_SUCCESS == ret) 
	{
		sprintf(ifname,"eth%d-%d",slot,port);
		ret = npd_port_type_deal_for_lacp(eth_g_index,ifname,endis);
		if(ret != INTERFACE_RETURN_CODE_SUCCESS)
		{
			syslog_ax_eth_port_err("npd_port_type_deal err re_value is %#x \n",ret);
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,	&ret);
	dbus_message_iter_init_append (reply, &iter);
	
	return reply;

}


/****************************************************
 *
 * RETURN:
 *		INTERFACE_RETURN_CODE_SUCCESS	
 *		INTERFACE_RETURN_CODE_ERROR
 *		INTERFACE_RETURN_CODE_NO_SUCH_PORT
 *		INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND
 *		INTERFACE_RETURN_CODE_ALREADY_THIS_MODE
 *   no interface 
 *		INTERFACE_RETURN_CODE_PORT_HAS_SUB_IF
 *	advanced-routing disable
 *		COMMON_RETURN_CODE_NULL_PTR
 *		INTERFACE_RETURN_CODE_PORT_HAS_SUB_IF 
 *		INTERFACE_RETURN_CODE_NAM_ERROR 
 *		INTERFACE_RETURN_CODE_CHECK_MAC_ERROR
 *		INTERFACE_RETURN_CODE_FDB_SET_ERROR
 *		INTERFACE_RETURN_CODE_FD_ERROR
 *		INTERFACE_RETURN_CODE_IOCTL_ERROR
 *		INTERFACE_RETURN_CODE_MAC_GET_ERROR
 *	advanced-routing enable
 *		INTERFACE_RETURN_CODE_DEFAULT_VLAN_IS_L3_VLAN
 *******************************************************/
DBusMessage * npd_dbus_config_port_mode(DBusConnection *conn, DBusMessage *msg, void *user_data){
	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned int	mode;
	int ret = INTERFACE_RETURN_CODE_SUCCESS;
	unsigned char	slot,port;
	DBusError err;
	unsigned int  eth_g_index = 0;
	enum product_id_e productId = PRODUCT_ID; 
	enum module_id_e moduleId = MODULE_ID_NONE;
	int ifnameType = 0;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32,&ifnameType,
									DBUS_TYPE_BYTE,&slot,
									DBUS_TYPE_BYTE,&port,
									DBUS_TYPE_UINT32,&mode,
									DBUS_TYPE_INVALID))) {
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
		return NULL;
	}
	syslog_ax_eth_port_dbg("Entering config port %d/%d type %d mode %d!\n", \
							slot, port, ifnameType, mode);
	/*todo:check port type***************;*/
	/*type=npd_check_port_mode_type( slot,port);*/
	ret = INTERFACE_RETURN_CODE_NO_SUCH_PORT;	
	if (CHASSIS_SLOTNO_ISLEGAL(slot)) {
		if (ETH_LOCAL_PORTNO_ISLEGAL(slot,port)) {
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot,port);
			moduleId = MODULE_TYPE_ON_SLOT_INDEX(SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index));
			syslog_ax_eth_port_dbg("index for %d/%d is %#0x\n",slot,port,eth_g_index);
			ret = INTERFACE_RETURN_CODE_SUCCESS;
		}
	}

	if((INTERFACE_RETURN_CODE_NO_SUCH_PORT == ret)&&\
		(PRODUCT_ID != PRODUCT_ID_AX7K)&&\
		(PRODUCT_ID != PRODUCT_ID_AX7K_I) && \
		(PRODUCT_ID != PRODUCT_ID_AX5K_E) && \
		(PRODUCT_ID != PRODUCT_ID_AX5608) && \
		0 == slot){		 
		if(0 == (nbm_cvm_query_card_on(port-1))){
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot,port);
            ret = INTERFACE_RETURN_CODE_SUCCESS;
		}		
	}

	if(INTERFACE_RETURN_CODE_SUCCESS == ret) {
		if((ETH_PORT_FUNC_MODE_PROMISCUOUS == mode) && 
			!((PRODUCT_ID_AX7K == productId)|| (PRODUCT_ID_AX7K_I == productId) ||(MODULE_ID_AX5_5612 == moduleId)||(MODULE_ID_AX5_5612I == moduleId) \
				||(MODULE_ID_AX5_5612E == moduleId) ||(MODULE_ID_SUBCARD_TYPE_A1 == moduleId) ||(MODULE_ID_AX5_5608 == moduleId))) {
			ret = INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND;
		}
		else if(TRUE == npd_eth_port_rgmii_type_check(slot,port)) {
			ret = INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND;
		}
		else { 
			ret = npd_port_type_deal(eth_g_index,mode,ifnameType);
			if(INTERFACE_RETURN_CODE_SUCCESS != ret){
				syslog_ax_eth_port_err("npd_port_type_deal err re_value is %#x \n",ret);
			}
		}
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,	&ret);
	dbus_message_iter_init_append (reply, &iter);
	
	return reply;

}

/****************************************************
 *
 * RETURN:
 *		COMMON_RETURN_CODE_NULL_PTR
 *		INTERFACE_RETURN_CODE_SUCCESS	
 *		INTERFACE_RETURN_CODE_ERROR
 *		INTERFACE_RETURN_CODE_NO_SUCH_PORT
 *		INTERFACE_RETURN_CODE_PORT_HAS_SUB_IF 
 *		INTERFACE_RETURN_CODE_NAM_ERROR 
 *		INTERFACE_RETURN_CODE_CHECK_MAC_ERROR
 *		INTERFACE_RETURN_CODE_FDB_SET_ERROR
 *		INTERFACE_RETURN_CODE_FD_ERROR
 *		INTERFACE_RETURN_CODE_IOCTL_ERROR
 *		INTERFACE_RETURN_CODE_MAC_GET_ERROR
 *
 *******************************************************/

DBusMessage * npd_dbus_config_port_interface_mode(DBusConnection *conn, DBusMessage *msg, void *user_data){
	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned int	mode;
	int ret = INTERFACE_RETURN_CODE_SUCCESS;
	unsigned char	slot,port;
	DBusError err;
	unsigned int  eth_g_index = 0,ipadd =0,i=0;
	enum product_id_e productId = PRODUCT_ID; 
	enum module_id_e moduleId = MODULE_ID_NONE;
	unsigned int ifIndex = ~0UI;
    unsigned short pvid = 0;
	int ifnameType = 0;
	unsigned char ifname[MAX_IFNAME_LEN + 1] = {0};
	unsigned char name[16] = {'\0'};
	unsigned char ipaddr[4] = {0};
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32,&ifnameType,
									DBUS_TYPE_BYTE,&slot,
									DBUS_TYPE_BYTE,&port,
									DBUS_TYPE_INVALID))) {
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
		return NULL;
	}
	syslog_ax_eth_port_dbg("Entering config port %d/%d type %d interface mode!\n", \
							slot, port, ifnameType);
	if((CONFIG_INTF_CSCD == ifnameType)&&(PRODUCT_ID_AX7K_I == PRODUCT_ID)){
		if((0 == LOCAL_CHASSIS_SLOT_NO)||(1 == LOCAL_CHASSIS_SLOT_NO)){
				ret = npd_set_cscd_port_promi_mode(port);
				if(INTERFACE_RETURN_CODE_INTERFACE_EXIST == ret) {
					ret = INTERFACE_RETURN_CODE_SUCCESS;
				}
				#if 0
				else if(INTERFACE_RETURN_CODE_SUCCESS == ret){
					slot = LOCAL_CHASSIS_SLOT_NO;
					if(0 == slot){
						ipaddr[0] = 169;
						ipaddr[1] = 254;
						ipaddr[2] = 0;
						ipaddr[3] = 1;
					}else if(1 == slot){
						ipaddr[0] = 169;
						ipaddr[1] = 254;
						ipaddr[2] = 0;
						ipaddr[3] = 2;
					}
						ipadd |= ipaddr[0];
						ipadd = (ipadd << 8) | ipaddr[1];
						ipadd = (ipadd << 8) | ipaddr[2];
						ipadd = (ipadd << 8) | ipaddr[3];
					
					syslog_ax_eth_port_dbg("set cscd port ipaddr %d:%d:%d:%d! ipadd %x.\n", \
											ipaddr[0], ipaddr[1], ipaddr[2],ipaddr[3],ipadd);
					ret = npd_set_cscd_port_ipaddr(ipadd,port);
					if(NPD_SUCCESS != ret){
						syslog_ax_eth_port_dbg("set cscd port ipaddr error!\n");
					}
				}		
				#endif
		}
	}
	else{
		/*todo:check port type***************;*/
		/*type=npd_check_port_mode_type( slot,port);*/
		ret = INTERFACE_RETURN_CODE_NO_SUCH_PORT;	
		if (CHASSIS_SLOTNO_ISLEGAL(slot)) {
			if (ETH_LOCAL_PORTNO_ISLEGAL(slot,port)) {
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot,port);
				moduleId = MODULE_TYPE_ON_SLOT_INDEX(SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index));
				syslog_ax_eth_port_dbg("index for %d/%d is %#0x\n",slot,port,eth_g_index);
				ret = INTERFACE_RETURN_CODE_SUCCESS;
			}
		}
		if((INTERFACE_RETURN_CODE_NO_SUCH_PORT == ret)&&\
			(PRODUCT_ID != PRODUCT_ID_AX7K)&&\
			(PRODUCT_ID != PRODUCT_ID_AX5K_E) && \
			(PRODUCT_ID != PRODUCT_ID_AX5608) && \
			0 == slot){		 
			if(0 == (nbm_cvm_query_card_on(port-1))){
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot,port);
	            ret = INTERFACE_RETURN_CODE_SUCCESS;
			}		
		}
		if(INTERFACE_RETURN_CODE_SUCCESS == ret) {
			if(ifnameType){
				sprintf(ifname,"e%d-%d",slot,port);
				ret = INTERFACE_RETURN_CODE_SHORTEN_IFNAME_NOT_SUPPORT;
			}
			else{
				sprintf(ifname,"eth%d-%d",slot,port);
				if(TRUE == npd_eth_port_rgmii_type_check(slot,port)){
					/* TODO: set the interface up*/			
					ret = INTERFACE_RETURN_CODE_SUCCESS;
					if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifIndex)){
						ret = INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST;
					}
					if(INTERFACE_RETURN_CODE_SUCCESS == ret){
						npd_intf_set_intf_state(ifname,ETH_ATTR_LINKUP);
					}
				}
				else { 
					ret = npd_eth_port_promis_interface_check( eth_g_index,&ifIndex);
					if(TRUE == ret){
						ret = INTERFACE_RETURN_CODE_SUCCESS;
						if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifIndex)){
							ret = INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST;
						}
					}
					else {
					    ret = npd_eth_port_interface_check(eth_g_index, &ifIndex);
						if((TRUE == ret)&&(ifIndex != ~0UI)){
							ret = INTERFACE_RETURN_CODE_SUCCESS;
							if(ifnameType){
								ret = INTERFACE_RETURN_CODE_SHORTEN_IFNAME_NOT_SUPPORT;
							}
						}
		                else if((NPD_SUCCESS == npd_eth_port_get_pvid(eth_g_index,&pvid))&&\
		                    (DEFAULT_VLAN_ID != pvid)){
		                    syslog_ax_eth_port_err(" port conflict,pvid %d \n",pvid);
		                    ret = INTERFACE_RETURN_CODE_PORT_CONFLICT;
		                }
						else{
							ret = npd_port_type_deal(eth_g_index,ETH_PORT_FUNC_IPV4,ifnameType);
						}
					}

					if(ret != INTERFACE_RETURN_CODE_SUCCESS){
						syslog_ax_eth_port_err("npd_port_type_deal err re_value is %#x \n",ret);
					}
				}
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,	&ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,	&ifIndex);

	dbus_message_iter_init_append (reply, &iter);
	
	return reply;

}


/* add by yinlm@autelan.com for queue wrr and sp */
DBusMessage * npd_dbus_config_buffer_mode(DBusConnection *conn, DBusMessage *msg, void *user_data){
	DBusMessage* 	reply = NULL;
	DBusMessageIter	iter = {0};
	unsigned int	ret = NPD_DBUS_ERROR;;
	unsigned char 	buffer_mode;
	DBusError 		err;
	
	syslog_ax_acl_dbg("Entering config acl!\n");

	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_BYTE,&buffer_mode,
								DBUS_TYPE_INVALID))) {
		syslog_ax_acl_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_acl_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	if(g_buffer_mode != buffer_mode) { 
	
		ret = nam_set_port_global_buffer_mode(buffer_mode);
		if(ret != 0)
		{
			syslog_ax_acl_err("npd_eth_port>>npd_dbus_config_eth_port::cpssDxChPortBuffersModeSet %d\n",ret);
			ret=NPD_DBUS_ERROR;
		}
		else
			{
			g_buffer_mode = buffer_mode;
			ret = NPD_DBUS_SUCCESS;
			}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	return reply;
}
DBusMessage * npd_dbus_show_buffer_mode(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage* 	reply = NULL;
	DBusMessageIter	iter = {0};
	DBusError 		err;
	unsigned int	Isable;
	syslog_ax_eth_port_dbg("show buffer mode\n");

	dbus_error_init(&err);
	if(g_buffer_mode == CPSS_DXCH_PORT_BUFFERS_MODE_SHARED_E) 
	{
	   	syslog_ax_eth_port_dbg("buffer mode is shared\n");		   
	}
	else
	{
		syslog_ax_eth_port_dbg("buffer mode is divided\n");		
	}
	Isable = g_buffer_mode;
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&Isable);
	return reply;	

}
DBusMessage * npd_dbus_create_ve_intf(DBusConnection *conn, DBusMessage *msg, void *user_data)
{

	DBusMessage* reply;
	DBusMessageIter  iter;
	DBusError err;
	ve_netdev_priv ops = {0};
	unsigned char devNum=0,portNum = 0;        
	unsigned int ret = INTERFACE_RETURN_CODE_NO_SUCH_PORT;
	unsigned int eth_g_index = 0, ifindex = ~0UI;
	unsigned char slot = 0, local_port_no = 0, cpu_slot_no = 0;
	unsigned int  rc = FALSE;
	unsigned int  linkState = ETH_ATTR_LINKDOWN;
	struct eth_port_s portInfo;
	int ifnameType = 0;
	unsigned char * ifnamePrefix = NULL;
	unsigned int ifIndex = ~0UI;
	unsigned char ifname[MAX_IFNAME_LEN + 1] = {0};
	int result;


	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32,&ifnameType,
										DBUS_TYPE_BYTE,&slot,
										DBUS_TYPE_BYTE,&local_port_no,
										DBUS_TYPE_BYTE,&cpu_slot_no,
										DBUS_TYPE_INVALID)))
		{
		    syslog_ax_intf_err("Unable to get input args ");
		    if (dbus_error_is_set(&err)) {
			    syslog_ax_intf_err("%s raised: %s", err.name, err.message);
			    dbus_error_free(&err);
		}
		return NULL;
	}
	syslog_ax_intf_dbg("ifnameType = %d,slot=%d,local_port_no=%d,cpu_slot_no=%d\n",ifnameType,slot,local_port_no,cpu_slot_no);



    ifnamePrefix = "ve";


	syslog_ax_intf_dbg("create ve port : %s%d-%d-%d\n", ifnamePrefix,cpu_slot_no, slot, local_port_no);   



	memset(&ops, 0, sizeof(ve_netdev_priv));
	sprintf(ops.name, "%s%d-%d-%d", ifnamePrefix, cpu_slot_no, slot, local_port_no);
	ret = npd_slot_to_dev_map(&cpu_slot_no,&slot,&local_port_no,&devNum,&portNum);

	syslog_ax_intf_dbg("%d,%d,%d\n",cpu_slot_no,slot,local_port_no);
	syslog_ax_intf_dbg("%d,%d,%d\n",cpu_slot_no,devNum,portNum);
	ops.devNum = devNum;
	ops.portNum = portNum;


	
	if((ret == INTERFACE_RETURN_CODE_SUCCESS) && (port_ve_mode[cpu_slot_no][local_port_no-1] != 1))
    {
    	port_ve_mode[cpu_slot_no][local_port_no-1]=1;
        if(BOARD_TYPE == BOARD_TYPE_AX71_2X12G12S)
        {
            syslog_ax_intf_dbg("BOARD_TYPE == 0x4\n");
            result=npd_create_promiscuous_port(cpu_slot_no,slot,portNum);
        }
        else
        {
            syslog_ax_intf_dbg("create\n");
            result = cavim_do_intf(CVM_IOC_REG_, &ops);
        }
    }


	reply = dbus_message_new_method_return(msg);
		
	dbus_message_iter_init_append (reply, &iter);
		
	dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_UINT32,	&ret);

	dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_UINT32,	&ifIndex);
	dbus_message_iter_init_append (reply, &iter);

	return reply;
}

DBusMessage * npd_dbus_del_ve_intf(DBusConnection *conn, DBusMessage *msg, void *user_data)
{

	DBusMessage* reply;
	DBusMessageIter  iter;
	DBusError err;
	ve_netdev_priv ops = {0};
	unsigned char devNum=0,portNum = 0;        
	unsigned int ret = INTERFACE_RETURN_CODE_SUCCESS;
	unsigned int eth_g_index = 0, ifindex = ~0UI;
	unsigned char slot = 0, local_port_no = 0, cpu_slot_no = 0;
	unsigned int  rc = FALSE;
	unsigned int  linkState = ETH_ATTR_LINKDOWN;
	struct eth_port_s portInfo;
	int ifnameType = 0;
	unsigned char * ifnamePrefix = NULL;
	unsigned int ifIndex = ~0UI;
	unsigned char ifname[MAX_IFNAME_LEN + 1] = {0};
	int result;


	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32,&ifnameType,
										DBUS_TYPE_BYTE,&slot,
										DBUS_TYPE_BYTE,&local_port_no,
										DBUS_TYPE_BYTE,&cpu_slot_no,
										DBUS_TYPE_INVALID)))
		{
		    syslog_ax_intf_err("Unable to get input args ");
		    if (dbus_error_is_set(&err)) {
			    syslog_ax_intf_err("%s raised: %s", err.name, err.message);
			    dbus_error_free(&err);
		}
		return NULL;
	}
	syslog_ax_intf_dbg("ifnameType = %d,slot=%d,local_port_no=%d,cpu_slot_no=%d\n",ifnameType,slot,local_port_no,cpu_slot_no);


    ifnamePrefix = "ve";


	syslog_ax_intf_dbg("del ve port : %s%d-%d-%d\n", ifnamePrefix,cpu_slot_no, slot, local_port_no);   


	memset(&ops, 0, sizeof(ve_netdev_priv));

	sprintf(ops.name, "%s%d-%d-%d", ifnamePrefix, cpu_slot_no,  slot, local_port_no);
	ret=npd_slot_to_dev_map(&cpu_slot_no,&slot,&local_port_no,&devNum,&portNum);

	syslog_ax_intf_dbg("%d,%d,%d\n",cpu_slot_no,slot,local_port_no);
	syslog_ax_intf_dbg("%d,%d,%d\n",cpu_slot_no,devNum,portNum);
	ops.devNum = devNum;
	ops.portNum = portNum;
	if((ret == INTERFACE_RETURN_CODE_SUCCESS) && (port_ve_mode[cpu_slot_no][local_port_no-1] == 1))
	{
	    port_ve_mode[cpu_slot_no][local_port_no-1]=0;
	    if(BOARD_TYPE == BOARD_TYPE_AX71_2X12G12S)
	    {
		    syslog_ax_intf_dbg("BOARD_TYPE == 0x4\n");
		    /*
		     result=npd_create_promiscuous_port(cpu_slot_no,cpu_no,slot,portNum);*/
		    result=npd_disable_promiscuous_port(cpu_slot_no-1,slot,portNum);
	    }
	    else
	    {
		    syslog_ax_intf_dbg("%s,%s,%d\n",__FILE__,__FUNCTION__,__LINE__);
		    syslog_ax_intf_dbg("ops.devNum = %d,ops,portNum = %d,ops.name = %s\n",ops.devNum,ops.portNum,ops.name);
		    result = cavim_do_intf(CVM_IOC_UNREG_, &ops);
	    }
	}


	reply = dbus_message_new_method_return(msg);
		
	dbus_message_iter_init_append (reply, &iter);
		
	dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_UINT32,	&ret);
	dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_UINT32,	&ifIndex);
	
	dbus_message_iter_init_append (reply, &iter);

	return reply;
}



DBusMessage *npd_dbus_ethports_show_running_config(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply;
	DBusMessageIter	 iter;

	char *showStr = NULL,*cursor = NULL;
	int totalLen = 0;
	int i = 0, j = 0;
    struct eth_port_s * eth_port = NULL;
    struct prot_vlan_ctrl_t * protVlanCtrl = NULL;
    unsigned int tempVlanId = 0;
    syslog_ax_eth_port_err("npd_dbus_ethports_show_running_config\n");
	showStr = (char*)malloc(NPD_ETHPORT_SHOWRUN_CFG_SIZE);
	if(NULL == showStr) {
		syslog_ax_eth_port_err("memory malloc error\n");
		return showStr;
	}
	memset(showStr,0,NPD_ETHPORT_SHOWRUN_CFG_SIZE);
	cursor = showStr;

    if(IS_DISTRIBUTED == SYSTEM_TYPE)
    {		
        /*sc global control*/
    	if(ETH_PORT_STREAM_PPS_E == scType){
    		totalLen += sprintf(cursor,"config storm-control %s\n",(ETH_PORT_STREAM_PPS_E == scType) ? "pps":"bps");
    		cursor = showStr + totalLen;
    	}	
    	/*i loop around as slot index*/
    	for (i = 0  ; i < CHASSIS_SLOT_COUNT; i++ ) {
    		unsigned char local_port_count = ETH_LOCAL_PORT_COUNT(i);
    		for (j = 0; j < local_port_count; j++ ) {
    			unsigned char local_port_no = ETH_LOCAL_INDEX2NO(i,j);
    			unsigned int eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(i,j);
    			unsigned int local_port_attrmap = 0, local_port_mtu = 0,local_port_ipg = 0;
    			unsigned int slot_no = CHASSIS_SLOT_INDEX2NO(i);
    			unsigned int ret = 0;
    			struct eth_port_s portInfo, *portNode = NULL;
    			enum module_id_e module_type = MODULE_ID_NONE;
    			unsigned char enter_node = 0; /* need to enter eth-port node first*/
    			char tmpBuf[2048] = {0},*tmpPtr = NULL;
    			int length = 0;
    			unsigned char an = 0,an_state = 0,an_fc = 0,fc = 0,an_duplex = 0,duplex = 0,an_speed = 0;
    			unsigned char admin_status = 0,bp = 0,copper = 0,fiber = 0;
    			PORT_SPEED_ENT speed = PORT_SPEED_10_E;
    			unsigned int aclGroupNo = 0;
    			unsigned int ifIndex = ~0UI,rc = NPD_FALSE;
    			unsigned int policymapId=0;
    			unsigned int rateType = 0,rateValue = 0;
    			unsigned char ipg = 0;
    			unsigned char devNum = 0,portNum = 0;
    			QOS_TRAFFIC_SHAPE_PORT_STC  shapeDate;
                #if 0    			
    			if(!npd_eth_port_rgmii_type_check(slot_no,local_port_no)){
        			ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
                    if(NPD_SUCCESS != ret) {
        				syslog_ax_eth_port_dbg("get port %d dev by global error ret %d\n",eth_g_index,ret);
        				continue;
        			}
    			}
				#endif
    			portNode = npd_get_port_by_index(eth_g_index);
    			if(NULL == portNode) {
    				syslog_ax_eth_port_dbg("get port %d info NULL\n",eth_g_index);
    				continue;
    			}
    			
    			memset(&portInfo,0,sizeof(struct eth_port_s));
    			portInfo.attr_bitmap = ETH_PORT_ATTRBITMAP_FROM_LOCAL_INDEX(i,j);
    			portInfo.mtu = ETH_PORT_MTU_FROM_LOCAL_INDEX(i,j);
    			portInfo.ipg = ETH_PORT_IPG_FROM_LOCAL_INDEX(i,j);

    			memset(&shapeDate, 0,sizeof(QOS_TRAFFIC_SHAPE_PORT_STC));
    			
    			module_type = MODULE_TYPE_ON_SLOT_INDEX(i);
				#if 0
    			if(!PRODUCT_IS_BOX && (AX7_CRSMU_SLOT_NUM == i)) { /* read CRSMU RGMII ethernet port info*/
    				ret = nbm_read_eth_port_info(j,&portInfo);
    			}
    			else if((PRODUCT_ID_AX5K_I == PRODUCT_ID) \
    					&& (eth_g_index >= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9)) \
    					&& (eth_g_index <= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,12))) {
    				syslog_ax_eth_port_dbg("get port(%d %d) information from nbm\n",i,j);
    				eth_g_index = eth_g_index - ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9);
    				nbm_read_eth_port_info(eth_g_index,&portInfo);
    				/* change 5612i main control port module_id MODULE_ID_AX5_5612I to MODULE_ID_AX7_CRSMU */
    				module_type = MODULE_ID_AX7_CRSMU;				
    			}			
    			else if(PRODUCT_ID_AX5K_E== PRODUCT_ID || PRODUCT_ID_AX5608== PRODUCT_ID) { /* read PRODUCT_ID_AX5K_E port info*/
    				ret = nbm_read_eth_port_info(j,&portInfo);
    			}
    			else { /* read GE or XG ports info from asic*/
    				ret = nam_read_eth_port_info(module_type,slot_no,local_port_no,&portInfo);
    			}
				#endif
				ret = nam_read_eth_port_info(module_type,slot_no,local_port_no,&portInfo);
    			if (ret != 0) {
    				syslog_ax_eth_port_dbg("get port %d/%d attribute fail %d\n",slot_no,local_port_no,ret);
    			}
    			else {
    				local_port_attrmap	= portInfo.attr_bitmap;
    				local_port_mtu		= portInfo.mtu;
    				local_port_ipg      = portInfo.ipg;
    			}
    			syslog_ax_eth_port_dbg("local_port_attrmap 0x%x\n",local_port_attrmap);
    			tmpPtr = tmpBuf;
    			/* build up show running config string*/
				#if 0
    			if((PRODUCT_ID_AX7K_I == PRODUCT_ID) && (MODULE_ID_AX7I_XG_CONNECT == module_type)) {
    				module_type = MODULE_ID_AX7_XFP;
    			}
				#endif
    			/* MRU*/
    			
    			if(local_port_mtu != ethport_attr_default(module_type)->mtu) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," config mtu %d\n",local_port_mtu);
    				tmpPtr = tmpBuf+length;
    			}
    			if(local_port_ipg != 12 && 
    				(MODULE_ID_AX7_CRSMU != module_type) &&
    				 (MODULE_ID_AX5_5612E != module_type) &&
    				 (MODULE_ID_AX5_5608 != module_type)){
    				 
    				enter_node = 1;
    				length += sprintf(tmpPtr," config minimum-ipg %d\n",local_port_ipg);
    				tmpPtr = tmpBuf+length;
    			}
    			admin_status = (local_port_attrmap & ETH_ATTR_ADMIN_STATUS) >> ETH_ADMIN_STATUS_BIT;
    			bp = (local_port_attrmap & ETH_ATTR_BACKPRESSURE) >> ETH_BACKPRESSURE_BIT;
    			an_state = (local_port_attrmap & ETH_ATTR_AUTONEG) >> ETH_AUTONEG_BIT;
    			an = (local_port_attrmap & ETH_ATTR_AUTONEG_CTRL) >> ETH_AUTONEG_CTRL_BIT;
    			an_speed = (local_port_attrmap & ETH_ATTR_AUTONEG_SPEED) >> ETH_AUTONEG_SPEED_BIT;
    			speed = (local_port_attrmap & ETH_ATTR_SPEED_MASK) >> ETH_SPEED_BIT;
    			an_duplex = (local_port_attrmap & ETH_ATTR_AUTONEG_DUPLEX) >> ETH_AUTONEG_DUPLEX_BIT;
    			duplex = (local_port_attrmap & ETH_ATTR_DUPLEX) >> ETH_DUPLEX_BIT;
    			an_fc = (local_port_attrmap & ETH_ATTR_AUTONEG_FLOWCTRL) >> ETH_AUTONEG_FLOWCTRL_BIT;
    			fc = (local_port_attrmap & ETH_ATTR_FLOWCTRL) >> ETH_FLOWCTRL_BIT;
    			copper = (local_port_attrmap & ETH_ATTR_PREFERRED_COPPER_MEDIA) >> ETH_PREFERRED_COPPER_MEDIA_BIT; 
    			fiber = (local_port_attrmap & ETH_ATTR_PREFERRED_FIBER_MEDIA) >> ETH_PREFERRED_FIBER_MEDIA_BIT;
    			
    			/*media priority*/
    			ret = npd_eth_port_combo_media_check(slot_no,local_port_no,module_type);
    			if(ETH_PREFERRED_COPPER_MEDIA_BIT == ret) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," config media mode copper\n");
    				tmpPtr = tmpBuf + length;				
    			}
    			else if(ETH_PREFERRED_FIBER_MEDIA_BIT == ret) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," config media mode fiber\n");
    				tmpPtr = tmpBuf + length;	
    			}	

    			/*  Auto-Nego - all AN options disable we need one command control*/
    			if((an != ethport_attr_default(module_type)->autoNego) &&
    				!(an_speed != an && an_duplex != an && an_fc != an) /* &&
    				(an == an_speed) && (an == an_duplex) && (an == an_fc) */){
    				enter_node = 1;
    				length += sprintf(tmpPtr," config auto-negotiation %s\n", an ? "enable":"disable");
    				tmpPtr = tmpBuf+length;
    			}
    			
    			/* Auto-Nego speed*/
    			if(((an_speed != ethport_attr_default(module_type)->speed_an && 
    				an == ethport_attr_default(module_type)->autoNego) ||
    				(an_speed != an && an != ethport_attr_default(module_type)->autoNego)) &&
    				!(an_speed != an && an_duplex != an && an_fc != an)) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," config auto-negotiation speed %s\n", an_speed ? "enable":"disable");
    				tmpPtr = tmpBuf+length;
    			}
    				
    			/* Auto-Nego duplex*/
    			if(((an_duplex != ethport_attr_default(module_type)->duplex_an && 
    				an == ethport_attr_default(module_type)->autoNego)||
    				(an_duplex != an  && an != ethport_attr_default(module_type)->autoNego)) &&
    				!(an_speed != an && an_duplex != an && an_fc != an)) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," config auto-negotiation duplex %s\n", an_duplex ? "enable":"disable");
    				tmpPtr = tmpBuf+length;
    			}
    			
    			/* Auto-Nego flow-control*/
    			if(((an_fc != ethport_attr_default(module_type)->fc_an && 
    				an == ethport_attr_default(module_type)->autoNego)||
    				(an_fc != an && an != ethport_attr_default(module_type)->autoNego)) &&
    				!(an_speed != an && an_duplex != an && an_fc != an)) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," config auto-negotiation flow-control %s\n",an_fc ? "enable":"disable");
    				tmpPtr = tmpBuf+length;
    			}

    			/* speed - if not auto-nego speed and speed not default*/
    			if(MODULE_ID_AU3_3052 == module_type) {
    				if((an_speed != ETH_ATTR_ON) && (speed != PORT_SPEED_1000_E) && (1 == portNum)) {
    					enter_node = 1;
    					length += sprintf(tmpPtr," config speed %d\n",(speed == PORT_SPEED_100_E) ? 100:10);
    					tmpPtr = tmpBuf+length;
    				}
    				else if((an_speed != ETH_ATTR_ON) && (speed != ethport_attr_default(module_type)->speed) && (6 <= portNum)) {
    					enter_node = 1;
    					length += sprintf(tmpPtr," config speed %d\n",(speed == PORT_SPEED_100_E) ? 100:10);
    					tmpPtr = tmpBuf+length;
    				}
    			}
    			else {
    				if((an_speed != ETH_ATTR_ON) && ((speed != ethport_attr_default(module_type)->speed)&&(speed != PORT_SPEED_10000_E))) {
    					enter_node = 1;
    					length += sprintf(tmpPtr," config speed %d\n",(speed == PORT_SPEED_100_E) ? 100:10);
    					tmpPtr = tmpBuf+length;
    				}	
    			}
    			
    			/* flow-control - if not auto-nego fc and fc mode not default*/
    			if((an_fc != ETH_ATTR_ON) && (fc != ethport_attr_default(module_type)->fc)){
    				enter_node = 1;
    				length += sprintf(tmpPtr," config flow-control %s\n", fc ? "enable":"disable");
    				tmpPtr = tmpBuf+length;
    			}
    			/* duplex - if not auto-nego duplex and duplex mode not default*/
    			if((an_duplex != ETH_ATTR_ON) && (duplex != ethport_attr_default(module_type)->duplex)){
    				enter_node = 1;
    				length += sprintf(tmpPtr," config duplex mode %s\n", duplex ? "half":"full");
    				tmpPtr = tmpBuf+length;
    			}	
    			/* back-pressure  - back pressure only relevant when port in half-duplex mode*/
    			if((bp != ethport_attr_default(module_type)->bp) && (PORT_HALF_DUPLEX_E == duplex)) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," config back-pressure %s\n", bp ? "enable":"disable");
    				tmpPtr = tmpBuf + length;
    			}

    			/* admin status*/
    			if(admin_status != ethport_attr_default(module_type)->admin_state) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," config admin state %s\n",admin_status ? "enable":"disable");
    				tmpPtr = tmpBuf+length;
    			}
#if 0
    			/* ingress port ACL config*/
    			if(NPD_TRUE == npd_eth_port_acl_bind_check(eth_g_index,ACL_DIRECTION_INGRESS)) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," ingress acl enable\n");
    				tmpPtr = tmpBuf + length;

    				/* NPD_DBUS_SUCCESS + 7 means ACL_GROUP_PORT_BINDED*/
    				if((NPD_DBUS_SUCCESS + 7) == npd_port_bind_group_check(eth_g_index,&aclGroupNo,ACL_DIRECTION_INGRESS)) {
    					length += sprintf(tmpPtr," bind ingress acl-group %d\n",aclGroupNo);
    					tmpPtr = tmpBuf + length;
    				}
    			}
    			
    			if((NPD_DBUS_SUCCESS + 7) == npd_port_bind_group_check(eth_g_index,&aclGroupNo,ACL_DIRECTION_INGRESS)) {
    			
    			  if(NPD_FALSE == npd_eth_port_acl_bind_check(eth_g_index,ACL_DIRECTION_INGRESS)) {
    				 	enter_node = 1;
    				 	length += sprintf(tmpPtr," ingress acl enable\n");
    					tmpPtr = tmpBuf + length;
    				    length += sprintf(tmpPtr," bind ingress acl-group %d \n",aclGroupNo);
    				    tmpPtr = tmpBuf + length;								
    					length += sprintf(tmpPtr," ingress acl disable\n");
    				    tmpPtr = tmpBuf + length;
    			   }
    			}
    					
    			/*egress port ACL config*/
    			if(NPD_TRUE == npd_eth_port_acl_bind_check(eth_g_index,ACL_DIRECTION_EGRESS)) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," egress acl enable\n");
    				tmpPtr = tmpBuf + length;
    	
    				/* NPD_DBUS_SUCCESS + 7 means ACL_GROUP_PORT_BINDED*/
    				if((NPD_DBUS_SUCCESS + 7) == npd_port_bind_group_check(eth_g_index,&aclGroupNo,ACL_DIRECTION_EGRESS)) {
    					length += sprintf(tmpPtr," bind egress acl-group %d\n",aclGroupNo);
    					tmpPtr = tmpBuf + length;
    				}
    			}
    			
    			if((NPD_DBUS_SUCCESS + 7) == npd_port_bind_group_check(eth_g_index,&aclGroupNo,ACL_DIRECTION_EGRESS)) {
    	
    				  if(NPD_FALSE == npd_eth_port_acl_bind_check(eth_g_index,ACL_DIRECTION_EGRESS)) {
    						enter_node = 1;
    						length += sprintf(tmpPtr," egress acl enable\n");
    						tmpPtr = tmpBuf + length;
    					    length += sprintf(tmpPtr," bind egress acl-group %d \n",aclGroupNo);
    					    tmpPtr = tmpBuf + length;								
    						length += sprintf(tmpPtr," egress acl disable\n");
    					    tmpPtr = tmpBuf + length;
    				}
    			}
    			
    			/*QOS port policy map bind*/
    			if(7==npd_qos_port_bind_check(eth_g_index,&policymapId)){
    				if(policymapId!=0){
    	                enter_node = 1;	                
    	                length += sprintf(tmpPtr," bind policy-map %d\n",policymapId);
    					tmpPtr = tmpBuf + length;     
                   }
    			}
    			/*qos traffic shape*/
    			if(0==npd_qos_traffic_shape_check(eth_g_index,&shapeDate)){
    				enter_node = 1;
    				int j=0;				
    				if(NPD_TRUE==shapeDate.portEnable){
    					if (shapeDate.kmstate == 0) {
    						length += sprintf(tmpPtr," traffic-shape %ld k %ld\n",(shapeDate.Maxrate)/64,(shapeDate.burstSize));
    						tmpPtr = tmpBuf + length;
    					}
    					else if (shapeDate.kmstate == 1) {
    						length += sprintf(tmpPtr," traffic-shape %ld m %ld\n",(shapeDate.Maxrate)/1024,(shapeDate.burstSize));
    						tmpPtr = tmpBuf + length;
    					}
    				}
    				for(j=0;j<8;j++){
    					if(NPD_TRUE==(shapeDate.queue[j].queueEnable))
    					{
    						if (shapeDate.queue[j].kmstate == 0) {
    							length += sprintf(tmpPtr," traffic-shape queue %d %ld k %ld\n",j,(shapeDate.queue[j].Maxrate)/64,(shapeDate.queue[j].burstSize));
    							tmpPtr = tmpBuf + length; 
    						}
    						else if (shapeDate.queue[j].kmstate == 1) {
    							length += sprintf(tmpPtr," traffic-shape queue %d %ld m %ld\n",j,(shapeDate.queue[j].Maxrate)/1024,(shapeDate.queue[j].burstSize));
    							tmpPtr = tmpBuf + length; 
    						}
    					}
    				}
    			}
#endif
    			/* eth-port storm control*/
    			if(NPD_OK == npd_eth_port_get_sc_cfg(eth_g_index,PORT_STORM_CONTROL_STREAM_DLF,&rateType,&rateValue)){
                    enter_node = 1;
    				length += sprintf(tmpPtr," config storm-control dlf %s %d\n",(ETH_PORT_STREAM_PPS_E == rateType)? "pps" : "bps",rateValue);
    				tmpPtr = tmpBuf + length; 
    			}
    			if(NPD_OK == npd_eth_port_get_sc_cfg(eth_g_index,PORT_STORM_CONTROL_STREAM_MCAST,&rateType,&rateValue)){
                    enter_node = 1;
    				length += sprintf(tmpPtr," config storm-control multicast %s %d\n",(ETH_PORT_STREAM_PPS_E == rateType)? "pps" : "bps",rateValue);
    				tmpPtr = tmpBuf + length; 
    			}
    			if(NPD_OK == npd_eth_port_get_sc_cfg(eth_g_index,PORT_STORM_CONTROL_STREAM_BCAST,&rateType,&rateValue)){
                    enter_node = 1;
    				length += sprintf(tmpPtr," config storm-control broadcast %s %d\n",(ETH_PORT_STREAM_PPS_E == rateType)? "pps" : "bps",rateValue);
    				tmpPtr = tmpBuf + length; 
    			}	
                eth_port = npd_get_port_by_index(eth_g_index);
                if(eth_port){
                    protVlanCtrl = (struct prot_vlan_ctrl_t *) eth_port->funcs.func_data[ETH_PORT_FUNC_PROTOCOL_BASED_VLAN];
                    if(protVlanCtrl){
                        unsigned int isEnable = 0;
                        int entryNum = 0;
                        isEnable = protVlanCtrl->state;
                        if(isEnable){            
                                enter_node = 1;
                                length += sprintf(tmpPtr," config prot-vlan enable\n");
                                tmpPtr = tmpBuf + length;
                        }
                        for(entryNum = 0;entryNum <= PROTOCOL_VLAN_MAP_MAX;entryNum++){                     
                            tempVlanId = protVlanCtrl->vinfo[entryNum].vid;
                            if(tempVlanId){
                                enter_node = 1;
                                length += sprintf(tmpPtr," config prot-vlan %s %d\n",prot_vlan_prot_name[entryNum],tempVlanId);
                                tmpPtr = tmpBuf + length;
                            }
                        }
                    }
                }
    			
    			if(enter_node) { /* port configured*/
    				if((totalLen + length + 20 + 5) > NPD_ETHPORT_SHOWRUN_CFG_SIZE) { /* 20 for enter node; 5 for exit node*/
    					syslog_ax_eth_port_err("show ethport buffer full");
    					break;
    				}				
    				if(PRODUCT_ID_AX7K_I == PRODUCT_ID) {
    					totalLen += sprintf(cursor,"config eth-port cscd\n");
    				}
    				totalLen += sprintf(cursor,"config eth-port %d/%d\n",slot_no,local_port_no);
    				cursor = showStr + totalLen;
    				totalLen += sprintf(cursor,"%s",tmpBuf);
    				cursor = showStr + totalLen;
    				totalLen += sprintf(cursor," exit\n");
    				cursor = showStr + totalLen;
    				enter_node = 0;
    			}    			
    		}		
    	}

    	reply = dbus_message_new_method_return(msg);
    	
    	dbus_message_iter_init_append (reply, &iter);
    	
    	dbus_message_iter_append_basic (&iter,
    									 DBUS_TYPE_STRING,
    									 &showStr);	
    	free(showStr);
    	showStr = NULL;
    	return reply;		
    }
    else
    {    		
        /*sc global control*/
    	if(ETH_PORT_STREAM_PPS_E == scType){
    		totalLen += sprintf(cursor,"config storm-control %s\n",(ETH_PORT_STREAM_PPS_E == scType) ? "pps":"bps");
    		cursor = showStr + totalLen;
    	}	
    	/*i loop around as slot index*/
    	for (i = 0 ; i < CHASSIS_SLOT_COUNT; i++ ) {
			if(PRODUCT_ID_AX7K_I==PRODUCT_ID)				
			{
				continue;
			}
    		unsigned char local_port_count = ETH_LOCAL_PORT_COUNT(i);
    		for (j = 0; j < local_port_count; j++ ) {
    			unsigned char local_port_no = ETH_LOCAL_INDEX2NO(i,j);
    			unsigned int eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(i,j);
    			unsigned int local_port_attrmap = 0, local_port_mtu = 0,local_port_ipg = 0;
    			unsigned int slot_no = CHASSIS_SLOT_INDEX2NO(i);
    			unsigned int ret = 0;
    			struct eth_port_s portInfo, *portNode = NULL;
    			enum module_id_e module_type = MODULE_ID_NONE;
    			unsigned char enter_node = 0; /* need to enter eth-port node first*/
    			char tmpBuf[2048] = {0},*tmpPtr = NULL;
    			int length = 0;
    			unsigned char an = 0,an_state = 0,an_fc = 0,fc = 0,an_duplex = 0,duplex = 0,an_speed = 0;
    			unsigned char admin_status = 0,bp = 0,copper = 0,fiber = 0;
    			PORT_SPEED_ENT speed = PORT_SPEED_10_E;
    			unsigned int aclGroupNo = 0;
    			unsigned int ifIndex = ~0UI,rc = NPD_FALSE;
    			unsigned int policymapId=0;
    			unsigned int rateType = 0,rateValue = 0;
    			unsigned char ipg = 0;
    			unsigned char devNum = 0,portNum = 0;
    			QOS_TRAFFIC_SHAPE_PORT_STC  shapeDate;
    			
    			if(!npd_eth_port_rgmii_type_check(slot_no,local_port_no)){
        			ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
                    if(NPD_SUCCESS != ret) {
        				syslog_ax_eth_port_dbg("get port %d dev by global error ret %d\n",eth_g_index,ret);
        				continue;
        			}
    			}
    			portNode = npd_get_port_by_index(eth_g_index);
    			if(NULL == portNode) {
    				syslog_ax_eth_port_dbg("get port %d info NULL\n",eth_g_index);
    				continue;
    			}
    			
    			memset(&portInfo,0,sizeof(struct eth_port_s));
    			portInfo.attr_bitmap = ETH_PORT_ATTRBITMAP_FROM_LOCAL_INDEX(i,j);
    			portInfo.mtu = ETH_PORT_MTU_FROM_LOCAL_INDEX(i,j);
    			portInfo.ipg = ETH_PORT_IPG_FROM_LOCAL_INDEX(i,j);

    			memset(&shapeDate, 0,sizeof(QOS_TRAFFIC_SHAPE_PORT_STC));
    			
    			module_type = MODULE_TYPE_ON_SLOT_INDEX(i);
    			if(!PRODUCT_IS_BOX && (AX7_CRSMU_SLOT_NUM == i)) { /* read CRSMU RGMII ethernet port info*/
    				ret = nbm_read_eth_port_info(j,&portInfo);
    			}
    			else if((PRODUCT_ID_AX5K_I == PRODUCT_ID) \
    					&& (eth_g_index >= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9)) \
    					&& (eth_g_index <= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,12))) {
    				syslog_ax_eth_port_dbg("get port(%d %d) information from nbm\n",i,j);
    				eth_g_index = eth_g_index - ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9);
    				nbm_read_eth_port_info(eth_g_index,&portInfo);
    				/* change 5612i main control port module_id MODULE_ID_AX5_5612I to MODULE_ID_AX7_CRSMU */
    				module_type = MODULE_ID_AX7_CRSMU;				
    			}			
    			else if(PRODUCT_ID_AX5K_E== PRODUCT_ID || PRODUCT_ID_AX5608== PRODUCT_ID) { /* read PRODUCT_ID_AX5K_E port info*/
    				ret = nbm_read_eth_port_info(j,&portInfo);
    			}
    			else { /* read GE or XG ports info from asic*/
    				ret = nam_read_eth_port_info(module_type,slot_no,local_port_no,&portInfo);
    			}
    			if (ret != 0) {
    				syslog_ax_eth_port_dbg("get port %d/%d attribute fail %d\n",slot_no,local_port_no,ret);
    			}
    			else {
    				local_port_attrmap	= portInfo.attr_bitmap;
    				local_port_mtu		= portInfo.mtu;
    				local_port_ipg      = portInfo.ipg;
    			}
    			syslog_ax_eth_port_dbg("local_port_attrmap 0x%x\n",local_port_attrmap);
    			tmpPtr = tmpBuf;
    			/* build up show running config string*/
    			if((PRODUCT_ID_AX7K_I == PRODUCT_ID) && (MODULE_ID_AX7I_XG_CONNECT == module_type)) {
    				module_type = MODULE_ID_AX7_XFP;
    			}
    			/* MRU*/
    			
    			if(local_port_mtu != ethport_attr_default(module_type)->mtu) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," config mtu %d\n",local_port_mtu);
    				tmpPtr = tmpBuf+length;
    			}
    			if(local_port_ipg != 12 && 
    				(MODULE_ID_AX7_CRSMU != module_type) &&
    				 (MODULE_ID_AX5_5612E != module_type) &&
    				 (MODULE_ID_AX5_5608 != module_type)){
    				 
    				enter_node = 1;
    				length += sprintf(tmpPtr," config minimum-ipg %d\n",local_port_ipg);
    				tmpPtr = tmpBuf+length;
    			}
    			admin_status = (local_port_attrmap & ETH_ATTR_ADMIN_STATUS) >> ETH_ADMIN_STATUS_BIT;
    			bp = (local_port_attrmap & ETH_ATTR_BACKPRESSURE) >> ETH_BACKPRESSURE_BIT;
    			an_state = (local_port_attrmap & ETH_ATTR_AUTONEG) >> ETH_AUTONEG_BIT;
    			an = (local_port_attrmap & ETH_ATTR_AUTONEG_CTRL) >> ETH_AUTONEG_CTRL_BIT;
    			an_speed = (local_port_attrmap & ETH_ATTR_AUTONEG_SPEED) >> ETH_AUTONEG_SPEED_BIT;
    			speed = (local_port_attrmap & ETH_ATTR_SPEED_MASK) >> ETH_SPEED_BIT;
    			an_duplex = (local_port_attrmap & ETH_ATTR_AUTONEG_DUPLEX) >> ETH_AUTONEG_DUPLEX_BIT;
    			duplex = (local_port_attrmap & ETH_ATTR_DUPLEX) >> ETH_DUPLEX_BIT;
    			an_fc = (local_port_attrmap & ETH_ATTR_AUTONEG_FLOWCTRL) >> ETH_AUTONEG_FLOWCTRL_BIT;
    			fc = (local_port_attrmap & ETH_ATTR_FLOWCTRL) >> ETH_FLOWCTRL_BIT;
    			copper = (local_port_attrmap & ETH_ATTR_PREFERRED_COPPER_MEDIA) >> ETH_PREFERRED_COPPER_MEDIA_BIT; 
    			fiber = (local_port_attrmap & ETH_ATTR_PREFERRED_FIBER_MEDIA) >> ETH_PREFERRED_FIBER_MEDIA_BIT;
    			
    			/*media priority*/
    			ret = npd_eth_port_combo_media_check(slot_no,local_port_no,module_type);
    			if(ETH_PREFERRED_COPPER_MEDIA_BIT == ret) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," config media preferred copper\n");
    				tmpPtr = tmpBuf + length;				
    			}
    			else if(ETH_PREFERRED_FIBER_MEDIA_BIT == ret) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," config media preferred fiber\n");
    				tmpPtr = tmpBuf + length;	
    			}	

    			/*  Auto-Nego - all AN options disable we need one command control*/
    			if((an != ethport_attr_default(module_type)->autoNego) &&
    				!(an_speed != an && an_duplex != an && an_fc != an) /* &&
    				(an == an_speed) && (an == an_duplex) && (an == an_fc) */){
    				enter_node = 1;
    				length += sprintf(tmpPtr," config auto-negotiation %s\n", an ? "enable":"disable");
    				tmpPtr = tmpBuf+length;
    			}
    			
    			/* Auto-Nego speed*/
    			if(((an_speed != ethport_attr_default(module_type)->speed_an && 
    				an == ethport_attr_default(module_type)->autoNego) ||
    				(an_speed != an && an != ethport_attr_default(module_type)->autoNego)) &&
    				!(an_speed != an && an_duplex != an && an_fc != an)) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," config auto-negotiation speed %s\n", an_speed ? "enable":"disable");
    				tmpPtr = tmpBuf+length;
    			}
    				
    			/* Auto-Nego duplex*/
    			if(((an_duplex != ethport_attr_default(module_type)->duplex_an && 
    				an == ethport_attr_default(module_type)->autoNego)||
    				(an_duplex != an  && an != ethport_attr_default(module_type)->autoNego)) &&
    				!(an_speed != an && an_duplex != an && an_fc != an)) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," config auto-negotiation duplex %s\n", an_duplex ? "enable":"disable");
    				tmpPtr = tmpBuf+length;
    			}
    			
    			/* Auto-Nego flow-control*/
    			if(((an_fc != ethport_attr_default(module_type)->fc_an && 
    				an == ethport_attr_default(module_type)->autoNego)||
    				(an_fc != an && an != ethport_attr_default(module_type)->autoNego)) &&
    				!(an_speed != an && an_duplex != an && an_fc != an)) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," config auto-negotiation flow-control %s\n",an_fc ? "enable":"disable");
    				tmpPtr = tmpBuf+length;
    			}

    			/* speed - if not auto-nego speed and speed not default*/
    			if(MODULE_ID_AU3_3052 == module_type) {
    				if((an_speed != ETH_ATTR_ON) && (speed != PORT_SPEED_1000_E) && (1 == portNum)) {
    					enter_node = 1;
    					length += sprintf(tmpPtr," config speed %d\n",(speed == PORT_SPEED_100_E) ? 100:10);
    					tmpPtr = tmpBuf+length;
    				}
    				else if((an_speed != ETH_ATTR_ON) && (speed != ethport_attr_default(module_type)->speed) && (6 <= portNum)) {
    					enter_node = 1;
    					length += sprintf(tmpPtr," config speed %d\n",(speed == PORT_SPEED_100_E) ? 100:10);
    					tmpPtr = tmpBuf+length;
    				}
    			}
    			else {
    				if((an_speed != ETH_ATTR_ON) && (speed != ethport_attr_default(module_type)->speed)) {
    					enter_node = 1;
    					length += sprintf(tmpPtr," config speed %d\n",(speed == PORT_SPEED_100_E) ? 100:10);
    					tmpPtr = tmpBuf+length;
    				}	
    			}
    			
    			/* flow-control - if not auto-nego fc and fc mode not default*/
    			if((an_fc != ETH_ATTR_ON) && (fc != ethport_attr_default(module_type)->fc)){
    				enter_node = 1;
    				length += sprintf(tmpPtr," config flow-control %s\n", fc ? "enable":"disable");
    				tmpPtr = tmpBuf+length;
    			}
    			/* duplex - if not auto-nego duplex and duplex mode not default*/
    			if((an_duplex != ETH_ATTR_ON) && (duplex != ethport_attr_default(module_type)->duplex)){
    				enter_node = 1;
    				length += sprintf(tmpPtr," config duplex mode %s\n", duplex ? "half":"full");
    				tmpPtr = tmpBuf+length;
    			}	
    			/* back-pressure  - back pressure only relevant when port in half-duplex mode*/
    			if((bp != ethport_attr_default(module_type)->bp) && (PORT_HALF_DUPLEX_E == duplex)) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," config back-pressure %s\n", bp ? "enable":"disable");
    				tmpPtr = tmpBuf + length;
    			}

    			#if 0
    			if(MODULE_ID_AU3_3052 == module_type && 1 == portNum) {
    				if(copper != ethport_attr_default(module_type)->copper) {
    					enter_node = 1;
    					length += sprintf(tmpPtr,"  config media preferred copper\n");
                        tmpPtr = tmpBuf + length;
    				}
    			}
    			else {
    				if(copper != ethport_attr_default(module_type)->copper) {
    					enter_node = 1;
    					length += sprintf(tmpPtr,"  config media preferred copper\n");
    					tmpPtr = tmpBuf + length;				
    				}
    				if(fiber != ethport_attr_default(module_type)->fiber) {
    					enter_node = 1;
    					length += sprintf(tmpPtr,"  config media preferred fiber\n");
    					tmpPtr = tmpBuf + length;	
    					npd_syslog_dbg("tmpPtr fiber is %#x\n",tmpPtr);
    				}
    			}
    			if(!PRODUCT_IS_BOX && AX7_CRSMU_SLOT_NUM == i) {
    				// CRSMU eth-port has different preferred media config 
    				if(copper != ETH_PORT_MEDIA_DEFAULT) {
    					enter_node = 1;
    					length += sprintf(tmpPtr,"	config media preferred copper\n");
                        tmpPtr = tmpBuf + length;
    				} 
    				else if(fiber != ETH_PORT_MEDIA_DEFAULT){
    					enter_node = 1;
    					length += sprintf(tmpPtr,"  config media preferred fiber\n");
    					tmpPtr = tmpBuf + length;
    					npd_syslog_dbg("tmpPtr fiber is %#x\n",tmpPtr);
    			    }
    			  
    			}

    			else if(MODULE_ID_AU3_3052 == module_type && 1 == portNum) {
    				if(copper != ETH_PORT_MEDIA_DEFAULT) {
    					enter_node = 1;
    					length += sprintf(tmpPtr,"  config media preferred copper\n");
                        tmpPtr = tmpBuf + length;
    				}
    			}

    			else if(copper != ETH_PORT_MEDIA_DEFAULT) {
    				enter_node = 1;
    				length += sprintf(tmpPtr,"  config media preferred copper\n");
    				tmpPtr = tmpBuf + length;
    			}
    			else if(fiber != ETH_PORT_MEDIA_DEFAULT){ 
    				enter_node = 1;
    				length += sprintf(tmpPtr,"  config media preferred fiber\n");
    				tmpPtr = tmpBuf + length;
    				npd_syslog_dbg("tmpPtr fiber is %#x\n",tmpPtr);
    			}

    			if(MODULE_ID_AU3_3052 == module_type && 1 == portNum) {
    				if(copper != ETH_PORT_MEDIA_DEFAULT) {
    					enter_node = 1;
    					length += sprintf(tmpPtr,"  config media preferred copper\n");
                        tmpPtr = tmpBuf + length;
    				}
    			}	
    			#endif

    			/* admin status*/
    			if(admin_status != ethport_attr_default(module_type)->admin_state) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," config admin state %s\n",admin_status ? "enable":"disable");
    				tmpPtr = tmpBuf+length;
    			}

    			/* ingress port ACL config*/
    			if(NPD_TRUE == npd_eth_port_acl_bind_check(eth_g_index,ACL_DIRECTION_INGRESS)) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," ingress acl enable\n");
    				tmpPtr = tmpBuf + length;

    				/* NPD_DBUS_SUCCESS + 7 means ACL_GROUP_PORT_BINDED*/
    				if((NPD_DBUS_SUCCESS + 7) == npd_port_bind_group_check(eth_g_index,&aclGroupNo,ACL_DIRECTION_INGRESS)) {
    					length += sprintf(tmpPtr," bind ingress acl-group %d\n",aclGroupNo);
    					tmpPtr = tmpBuf + length;
    				}
    			}
    			
    			if((NPD_DBUS_SUCCESS + 7) == npd_port_bind_group_check(eth_g_index,&aclGroupNo,ACL_DIRECTION_INGRESS)) {
    			
    			  if(NPD_FALSE == npd_eth_port_acl_bind_check(eth_g_index,ACL_DIRECTION_INGRESS)) {
    				 	enter_node = 1;
    				 	length += sprintf(tmpPtr," ingress acl enable\n");
    					tmpPtr = tmpBuf + length;
    				    length += sprintf(tmpPtr," bind ingress acl-group %d \n",aclGroupNo);
    				    tmpPtr = tmpBuf + length;								
    					length += sprintf(tmpPtr," ingress acl disable\n");
    				    tmpPtr = tmpBuf + length;
    			   }
    			}
    					
    			/*egress port ACL config*/
    			if(NPD_TRUE == npd_eth_port_acl_bind_check(eth_g_index,ACL_DIRECTION_EGRESS)) {
    				enter_node = 1;
    				length += sprintf(tmpPtr," egress acl enable\n");
    				tmpPtr = tmpBuf + length;
    	
    				/* NPD_DBUS_SUCCESS + 7 means ACL_GROUP_PORT_BINDED*/
    				if((NPD_DBUS_SUCCESS + 7) == npd_port_bind_group_check(eth_g_index,&aclGroupNo,ACL_DIRECTION_EGRESS)) {
    					length += sprintf(tmpPtr," bind egress acl-group %d\n",aclGroupNo);
    					tmpPtr = tmpBuf + length;
    				}
    			}
    			
    			if((NPD_DBUS_SUCCESS + 7) == npd_port_bind_group_check(eth_g_index,&aclGroupNo,ACL_DIRECTION_EGRESS)) {
    	
    				  if(NPD_FALSE == npd_eth_port_acl_bind_check(eth_g_index,ACL_DIRECTION_EGRESS)) {
    						enter_node = 1;
    						length += sprintf(tmpPtr," egress acl enable\n");
    						tmpPtr = tmpBuf + length;
    					    length += sprintf(tmpPtr," bind egress acl-group %d \n",aclGroupNo);
    					    tmpPtr = tmpBuf + length;								
    						length += sprintf(tmpPtr," egress acl disable\n");
    					    tmpPtr = tmpBuf + length;
    				}
    			}
    			
    			/*QOS port policy map bind*/
    			if(7==npd_qos_port_bind_check(eth_g_index,&policymapId)){
    				if(policymapId!=0){
    	                enter_node = 1;	                
    	                length += sprintf(tmpPtr," bind policy-map %d\n",policymapId);
    					tmpPtr = tmpBuf + length;     
                   }
    			}
    			/*qos traffic shape*/
    			if(0==npd_qos_traffic_shape_check(eth_g_index,&shapeDate)){
    				enter_node = 1;
    				int j=0;				
    				if(NPD_TRUE==shapeDate.portEnable){
    					if (shapeDate.kmstate == 0) {
    						length += sprintf(tmpPtr," traffic-shape %ld k %ld\n",(shapeDate.Maxrate)/64,(shapeDate.burstSize));
    						tmpPtr = tmpBuf + length;
    					}
    					else if (shapeDate.kmstate == 1) {
    						length += sprintf(tmpPtr," traffic-shape %ld m %ld\n",(shapeDate.Maxrate)/1024,(shapeDate.burstSize));
    						tmpPtr = tmpBuf + length;
    					}
    				}
    				for(j=0;j<8;j++){
    					if(NPD_TRUE==(shapeDate.queue[j].queueEnable))
    					{
    						if (shapeDate.queue[j].kmstate == 0) {
    							length += sprintf(tmpPtr," traffic-shape queue %d %ld k %ld\n",j,(shapeDate.queue[j].Maxrate)/64,(shapeDate.queue[j].burstSize));
    							tmpPtr = tmpBuf + length; 
    						}
    						else if (shapeDate.queue[j].kmstate == 1) {
    							length += sprintf(tmpPtr," traffic-shape queue %d %ld m %ld\n",j,(shapeDate.queue[j].Maxrate)/1024,(shapeDate.queue[j].burstSize));
    							tmpPtr = tmpBuf + length; 
    						}
    					}
    				}
    			}
    			/* eth-port storm control*/
    			if(NPD_OK == npd_eth_port_get_sc_cfg(eth_g_index,PORT_STORM_CONTROL_STREAM_DLF,&rateType,&rateValue)){
                    enter_node = 1;
    				length += sprintf(tmpPtr," config storm-control dlf %s %d\n",(ETH_PORT_STREAM_PPS_E == rateType)? "pps" : "bps",rateValue);
    				tmpPtr = tmpBuf + length; 
    			}
    			if(NPD_OK == npd_eth_port_get_sc_cfg(eth_g_index,PORT_STORM_CONTROL_STREAM_MCAST,&rateType,&rateValue)){
                    enter_node = 1;
    				length += sprintf(tmpPtr," config storm-control multicast %s %d\n",(ETH_PORT_STREAM_PPS_E == rateType)? "pps" : "bps",rateValue);
    				tmpPtr = tmpBuf + length; 
    			}
    			if(NPD_OK == npd_eth_port_get_sc_cfg(eth_g_index,PORT_STORM_CONTROL_STREAM_BCAST,&rateType,&rateValue)){
                    enter_node = 1;
    				length += sprintf(tmpPtr," config storm-control broadcast %s %d\n",(ETH_PORT_STREAM_PPS_E == rateType)? "pps" : "bps",rateValue);
    				tmpPtr = tmpBuf + length; 
    			}	
                eth_port = npd_get_port_by_index(eth_g_index);
                if(eth_port){
                    protVlanCtrl = (struct prot_vlan_ctrl_t *) eth_port->funcs.func_data[ETH_PORT_FUNC_PROTOCOL_BASED_VLAN];
                    if(protVlanCtrl){
                        unsigned int isEnable = 0;
                        int entryNum = 0;
                        isEnable = protVlanCtrl->state;
                        if(isEnable){            
                                enter_node = 1;
                                length += sprintf(tmpPtr," config prot-vlan enable\n");
                                tmpPtr = tmpBuf + length;
                        }
                        for(entryNum = 0;entryNum <= PROTOCOL_VLAN_MAP_MAX;entryNum++){                     
                            tempVlanId = protVlanCtrl->vinfo[entryNum].vid;
                            if(tempVlanId){
                                enter_node = 1;
                                length += sprintf(tmpPtr," config prot-vlan %s %d\n",prot_vlan_prot_name[entryNum],tempVlanId);
                                tmpPtr = tmpBuf + length;
                            }
                        }
                    }
                }
    			
    			if(enter_node) { /* port configured*/
    				if((totalLen + length + 20 + 5) > NPD_ETHPORT_SHOWRUN_CFG_SIZE) { /* 20 for enter node; 5 for exit node*/
    					syslog_ax_eth_port_err("show ethport buffer full");
    					break;
    				}				
    				if(PRODUCT_ID_AX7K_I == PRODUCT_ID) {
    					totalLen += sprintf(cursor,"config eth-port cscd\n");
    				}
    				totalLen += sprintf(cursor,"config eth-port %d/%d\n",slot_no,local_port_no);
    				cursor = showStr + totalLen;
    				totalLen += sprintf(cursor,"%s",tmpBuf);
    				cursor = showStr + totalLen;
    				totalLen += sprintf(cursor," exit\n");
    				cursor = showStr + totalLen;
    				enter_node = 0;
    			}
    			
#if 0

    			rc = npd_check_port_promi_mode(eth_g_index);
    			if((TRUE == rc)&&(TRUE != npd_eth_port_rgmii_type_check(slot_no,local_port_no))) {
    					totalLen += sprintf(cursor,"interface eth%d-%d\n advanced-routing enable\n exit\n",slot_no,local_port_no);
    					cursor = showStr + totalLen;
    			}

    		  /*port mode*/
    			if((totalLen + 80) < NPD_ETHPORT_SHOWRUN_CFG_SIZE) {
    				rc = npd_eth_port_interface_check(eth_g_index,&ifIndex);
    				if(NPD_TRUE == rc && ifIndex != ~0UI) {
    					if(PRODUCT_ID_AX7K_I == PRODUCT_ID) {
    						totalLen += sprintf(cursor,"interface cscd\n exit\n");
    					}
    					else {
    						totalLen += sprintf(cursor,"interface eth%d-%d\n exit\n",slot_no,local_port_no);
    					}
    					cursor = showStr + totalLen;
    				}
    				
    				/*rc = npd_check_port_promi_mode(eth_g_index);
    				if(NPD_TRUE == rc) {
    					totalLen += sprintf(cursor,"config eth-port %d/%d mode promiscuous\n",slot_no,local_port_no);
    					cursor = showStr + totalLen;
    				}*/
    			}
#endif
    		}		
    	}

    	reply = dbus_message_new_method_return(msg);
    	
    	dbus_message_iter_init_append (reply, &iter);
    	
    	dbus_message_iter_append_basic (&iter,
    									 DBUS_TYPE_STRING,
    									 &showStr);	
    	free(showStr);
    	showStr = NULL;
    	return reply;
    }
}

DBusMessage *npd_dbus_ethports_interface_config_ports_media(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;

	unsigned int ret = ETHPORT_RETURN_CODE_ERR_NONE;
	unsigned int media = 0, eth_g_index = 0;
	unsigned char dev = 0, port = 0;
	enum product_id_e productId = PRODUCT_ID; 
	unsigned int slot_no = 0, slot_index = 0;
	unsigned int local_port_no = 0, port_index = 0, eth_l_index;
	enum module_id_e module_type;
    struct eth_port_s portInfo;

	syslog_ax_eth_port_dbg("npd_dbus_ethports_interface_config_ports_media!\n");
	memset(&portInfo,0,sizeof(struct eth_port_s));	/* code optimize: Uninitialized scalar variable houxx@autelan.com  2013-1-17 */
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_UINT32,&eth_g_index,
								DBUS_TYPE_UINT32,&media,
								DBUS_TYPE_INVALID))) {
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	/* convert the index for configuring eth-port on ASIC Jia Lihui 2011.7.19 */
	if(SYSTEM_TYPE == IS_DISTRIBUTED)
	{
        eth_g_index = eth_g_index - (asic_board->asic_port_start_no -1);
	}
	
	slot_index 		= SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	slot_no			= CHASSIS_SLOT_INDEX2NO(slot_index);
	port_index 		= ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	local_port_no 	= ETH_LOCAL_INDEX2NO(slot_index,port_index);

    module_type	= MODULE_TYPE_ON_SLOT_INDEX(slot_index);

/*	if((0 == slot_no) && ((PRODUCT_ID_AX7K_I == productId) || (PRODUCT_ID_AX7K == productId))) { */
    if(MODULE_ID_AX7_CRSMU == module_type)
    {
		ret = nbm_set_eth_port_trans_media(eth_g_index, media);
		if(NPD_DBUS_ERROR_UNSUPPORT == ret) {
			ret = ETHPORT_RETURN_CODE_UNSUPPORT;
		}
		else if(NPD_DBUS_ERROR == ret) {
			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
		else {
			ret = ETHPORT_RETURN_CODE_ERR_NONE;	
		}
	}
	else if((MODULE_ID_AX5_5612I == module_type) 	\
		&& (eth_g_index >= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9)) \
		&& (eth_g_index <= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,12))) {
		eth_g_index = eth_g_index - ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9);
		ret = nbm_set_eth_port_trans_media(eth_g_index, media);
		if(NPD_DBUS_ERROR_UNSUPPORT == ret) {
			ret = ETHPORT_RETURN_CODE_UNSUPPORT;
		}
		else if(NPD_DBUS_ERROR == ret) {
			ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
		else {
			ret = ETHPORT_RETURN_CODE_ERR_NONE;	
		}
	}
	else if(MODULE_ID_AX5_5612E == module_type || MODULE_ID_AX5_5608 == module_type)
	{
		ret = nbm_set_eth_port_trans_media(eth_g_index, media);
	}
	else if(MODULE_ID_AX71_2X12G12S == module_type)
	{
        ret = ETHPORT_RETURN_CODE_UNSUPPORT;
	}
	else {
		ret = npd_get_devport_by_global_index(eth_g_index, &dev, &port);
		if(NPD_SUCCESS == ret) {
			ret = nam_set_eth_port_trans_media(dev,port,media);
			if(NPD_DBUS_ERROR_UNSUPPORT == ret) {
				ret = ETHPORT_RETURN_CODE_UNSUPPORT;
			}
			else if(NPD_DBUS_ERROR == ret) {
				ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
			}
			else if(ETHPORT_RETURN_CODE_NOT_SUPPORT == ret) {
				ret = ETHPORT_RETURN_CODE_NOT_SUPPORT;
			}
			else {
				ret = ETHPORT_RETURN_CODE_ERR_NONE;	
			}		
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	

    if(ret == ETHPORT_RETURN_CODE_ERR_NONE)
    {
    	module_type	= MODULE_TYPE_ON_SLOT_INDEX(slot_index);	 
    	slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
    	slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
    	eth_l_index	= ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
    	local_port_no	= ETH_LOCAL_INDEX2NO(slot_index, eth_l_index);
    		 
    	ret = nam_read_eth_port_info(module_type,slot_no,local_port_no,&portInfo);
    	if(NPD_SUCCESS != ret) {
    		syslog_ax_eth_port_err("read ASIC port(%d %d) infomation error.\n",slot_no, local_port_no);
    	}	
			
    	syslog_ax_eth_port_dbg("config media port(%d %d) and notifier sem to udpate port info\n",slot_no,local_port_no);

    	start_fp[slot_index][local_port_no-1].attr_bitmap = portInfo.attr_bitmap;
        if(media == PHY_MEDIA_MODE_COPPER)
        {
		    start_fp[slot_index][local_port_no-1].port_type = ETH_GTX;
        }
		else
        {
            start_fp[slot_index][local_port_no-1].port_type = ETH_GE_SFP;
		}
	
		npd_asic_ehtport_update_notifier(eth_g_index);
    }	
	return reply;
}

DBusMessage *npd_dbus_get_slot_port_by_portindex(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned char	slot_no = 0, port_no = 0, slot_index = 0;
	unsigned int 	eth_g_index = 0;
	int ret = 0;
	DBusError err;
	
	 syslog_ax_rstp_dbg("Get slot/port!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&eth_g_index,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_rstp_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_rstp_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	

	slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
	port_no	= ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	syslog_ax_eth_port_dbg(" 1 port_no = %d, eth_g_index = %d\n",port_no, eth_g_index);
	port_no	= ETH_LOCAL_INDEX2NO(slot_index,port_no);
    syslog_ax_eth_port_dbg(" 2 port_no = %d\n",port_no);
		
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_BYTE,
									 &slot_no);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_BYTE,
									 &port_no);
	
	return reply;

}


DBusMessage *npd_dbus_config_sc_global_model(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned int	modeType = ETH_PORT_STREAM_INVALID_E;
	unsigned int ret = NPD_SUCCESS;
	int productid = PRODUCT_ID;
	DBusError err;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&modeType,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			 dbus_error_free(&err);
		}
		return NULL;
	}
	if(productid == PRODUCT_ID_AU3K_BCM){
		npd_syslog_dbg("AU 3k does not support global config!\n");
        ret = NPD_DBUS_ERROR_NOT_SUPPORT;
	}
    else if(NPD_SUCCESS!= (ret = npd_eth_port_sc_set_check(modeType))){
		npd_syslog_err("check ports modetype set failed before global configure!\n");
        ret = NPD_DBUS_ERROR_OPERATE;
	}
	else if(NPD_OK != (ret = npd_eth_port_sc_global_cfg(modeType))){
		npd_syslog_err("config global modetype failed!\n");
		ret = -NPD_FAIL;
	}
	else if(NPD_OK !=(ret = nam_eth_port_sc_global_set(modeType))){
		npd_syslog_dbg("hardware cnfig error!\n");
		ret = -NPD_FAIL;
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);	
	return reply;

}


DBusMessage *npd_dbus_config_storm_control(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned char	slotno = 0,portno = 0,modeType = 0;
	unsigned int  scMode= 0,sctype = 0,scvalue = 0,portflg = 0,valid= 0,g_index = 0;
	unsigned int	eth_g_index = 0;
	int ret = NPD_SUCCESS;
	unsigned int productId = PRODUCT_ID;
	enum eth_port_type_e portType = ETH_INVALID;
	struct eth_port_s*  eth_port = NULL;
	eth_port_sc_ctrl_t* port_sc = NULL;
	DBusError err;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_BYTE,&modeType,
		DBUS_TYPE_BYTE,&slotno,
		DBUS_TYPE_BYTE,&portno,
		DBUS_TYPE_UINT32,&g_index,
		DBUS_TYPE_UINT32,&scMode,
		DBUS_TYPE_UINT32,&sctype,
		DBUS_TYPE_UINT32,&scvalue,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			 dbus_error_free(&err);
		}
		return NULL;
	}
	if(1 == modeType){
       eth_g_index = g_index;   
	}
	else if(0 == modeType){
	   eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotno,portno);
	}

	if((productId != PRODUCT_ID_AU3K_BCM)&&(scType != scMode)) {
		ret = NPD_DBUS_ERROR_OPERATE;
	}
    else{
		ret = npd_eth_port_sc_cfg(eth_g_index,scMode,sctype,scvalue);
		if(NPD_SUCCESS != ret){
		   syslog_ax_eth_port_err("config g_index(%#x) strom control failed,sc_mode %s,sc_type %s,sc_value %d,ret %d\n",
					eth_g_index,(ETH_PORT_STREAM_PPS_E == scMode) ? "pps" : "bps",(PORT_STORM_CONTROL_STREAM_DLF == sctype) ?"dlf" : \
					((PORT_STORM_CONTROL_STREAM_MCAST == sctype)? "mcast" : "bcast"),scvalue,ret);
		}
		
		else{
			eth_port = npd_get_port_by_index(eth_g_index);
			if(!eth_port){
                ret = -NPD_FAIL;
			}
			else{
				portType = eth_port->port_type;
				portflg = ((ETH_FE_TX == portType)||(ETH_FE_FIBER == portType))? 0:1;
				if(ETH_PORT_STREAM_PPS_E == scMode){			   
				   ret = nam_eth_port_sc_pps_set(eth_g_index,sctype,scvalue,portflg);
				}
				else if(ETH_PORT_STREAM_BPS_E == scMode){
				   ret = nam_eth_port_sc_bps_set(eth_g_index,sctype,scvalue,portflg);
				}
				if(NPD_OK != ret){
					syslog_ax_eth_port_dbg("set storm control failed,ret %d\n",ret);
					ret = NPD_DBUS_ERROR_OPERATE;
				}
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);	
	return reply;

}
#if 0
DBusMessage *npd_dbus_ethport_clear_arp(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned char slot_no = 0,local_port_no = 0,slot_index = 0;
	unsigned int port_index = 0,type = 0;
	unsigned int eth_g_index = 0;
	unsigned int arpCount = 0;
	unsigned int isStatic = 0;
    struct eth_port_s*g_ptr = NULL;
	unsigned int ret = 0;
	DBusError err;
	
	syslog_ax_eth_port_dbg("Entering clear ethport arp!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&type,
		DBUS_TYPE_BYTE,&slot_no,
		DBUS_TYPE_BYTE,&local_port_no,
		DBUS_TYPE_UINT32,&port_index,
		DBUS_TYPE_UINT32,&isStatic,
		DBUS_TYPE_INVALID))) {
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	syslog_ax_eth_port_dbg("type %d port_index %d slot %d/%d !\n",type,port_index,slot_no,local_port_no);
	
	
	if(0 == type){
		ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
		if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
			if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
				slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
				ret = NPD_DBUS_SUCCESS;
			}
		}
	}
	else{
		ret = NPD_DBUS_SUCCESS;
		eth_g_index = port_index;
	}
	if(NPD_DBUS_SUCCESS == ret) {
		g_ptr = npd_get_port_by_index(eth_g_index);
		if(NULL == g_ptr){
			syslog_ax_eth_port_dbg("get port info errors\n");
			ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
		} 
	}
	syslog_ax_eth_port_dbg("clear ether port detail of port %d/%d ,eth_g_index %d!\n",slot_no,local_port_no,eth_g_index);
	if(NPD_DBUS_SUCCESS == ret) {
		if(isStatic){
			arpCount = npd_static_arp_clear_by_port(eth_g_index);
		}
		else {
			arpCount = npd_arp_clear_by_port(eth_g_index);
		}
		syslog_ax_eth_port_dbg("npd clear %s arp on port %#x total %d item cleared",isStatic?"static":"",eth_g_index,arpCount);
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	
	return reply;


}
#endif
#if 1
int is_master;
int slot_num;
DBusMessage *npd_dbus_ethport_clear_arp(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned char slot_no = 0,local_port_no = 0,slot_index = 0;
	unsigned int port_index = 0,type = 0;
	unsigned int eth_g_index = 0;
	unsigned int arpCount = 0;
	unsigned int isStatic = 0;
    struct eth_port_s*g_ptr = NULL;
	unsigned int ret = NPD_DBUS_SUCCESS;
	char cmd_string[512];
	DBusError err;
	
	syslog_ax_eth_port_dbg("Entering clear ethport arp!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&type,
		DBUS_TYPE_BYTE,&slot_no,
		DBUS_TYPE_BYTE,&local_port_no,
		DBUS_TYPE_UINT32,&port_index,
		DBUS_TYPE_UINT32,&isStatic,
		DBUS_TYPE_INVALID))) {
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	
	syslog_ax_eth_port_dbg("type %d port_index %d slot %d/%d !\n",type,port_index,slot_no,local_port_no);
	if (0 != host_board_info_get()) {
		syslog_ax_eth_port_err("npd_clear_eth_port_arp get host board info err");
	}
	if((PRODUCT_ID_AX7K_I == PRODUCT_ID) && (is_master || (slot_no != slot_num))) {
		
		sprintf(cmd_string,"arp -i eth%d-%d|awk '/^[1-9]/ {print \"arp -d \"$1}'|sh",slot_no, local_port_no);
		if (-1 != system(cmd_string)) {
		   ret = NPD_DBUS_SUCCESS; 
		}
	}
	else if((NULL != asic_board)&&((local_port_no < asic_board->asic_port_start_no) || (slot_no != slot_num)))
	{
   
		sprintf(cmd_string,"arp -i eth%d-%d|awk '/^[1-9]/ {print \"arp -d \"$1}'|sh",slot_no, local_port_no);
		if (-1 != system(cmd_string)) {
			ret = NPD_DBUS_SUCCESS; 
		}

     }
    else {
    	
    	if(0 == type){
    		ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
    		if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
    			if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
    				slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
    				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
    				ret = NPD_DBUS_SUCCESS;
    			}
    		}
    	}
    	else{
    		ret = NPD_DBUS_SUCCESS;
    		eth_g_index = port_index;
    	}
    	if(NPD_DBUS_SUCCESS == ret) {
    		g_ptr = npd_get_port_by_index(eth_g_index);
    		if(NULL == g_ptr){
    			syslog_ax_eth_port_dbg("get port info errors\n");
    			ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
    		} 
    	}
    	syslog_ax_eth_port_dbg("clear ether port detail of port %d/%d ,eth_g_index %d!\n",slot_no,local_port_no,eth_g_index);
    	if(NPD_DBUS_SUCCESS == ret) {
    		if(isStatic){
    			arpCount = npd_static_arp_clear_by_port(eth_g_index);
    		}
    		else {
    			arpCount = npd_arp_clear_by_port(eth_g_index);
    		}
    		syslog_ax_eth_port_dbg("npd clear %s arp on port %#x total %d item cleared",isStatic?"static":"",eth_g_index,arpCount);
    	}	
    }

	reply = dbus_message_new_method_return(msg);
    	
    dbus_message_iter_init_append (reply, &iter);
    	
    dbus_message_iter_append_basic (&iter,
    							    DBUS_TYPE_UINT32,
    								&ret);
    	
    return reply;

}
#endif
DBusMessage* npd_dbus_ethport_show_arp(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter  iter,iter_array;
	unsigned char slot_no = 0,local_port_no = 0,slot_index = 0;
	unsigned int port_index = 0,i = 0,j = 0,k = 0;
	unsigned int eth_g_index = 0;
	unsigned int arpCount = 0;
	unsigned int ipaddr= 0,type = 0;
	unsigned char mac[6] = {0};
	unsigned char isTrunk = 0,trunkId = 0,isTagged = 0,isStatic = 0;
    unsigned char isValid = 0;
	unsigned short vid =0 ,vidx = 0;
	unsigned int ret = 0;
	struct arp_snooping_item_s ** arpInfo = NULL;
	DBusError err;
	struct arp_snooping_item_s * tmpArp = NULL;
	
	syslog_ax_eth_port_dbg("Entering show ethport arp!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&type,
		DBUS_TYPE_BYTE,&slot_no,
		DBUS_TYPE_BYTE,&local_port_no,
		DBUS_TYPE_UINT32,&port_index,
		DBUS_TYPE_INVALID))) {
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	
	
	if(0 == type){
		syslog_ax_eth_port_dbg("show arp info slot %d/%d !\n",slot_no,local_port_no);
		ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
		if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
			if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
				slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
				ret = NPD_DBUS_SUCCESS;
			}
		}
		if((NPD_DBUS_ERROR_NO_SUCH_PORT == ret)&&\
			(PRODUCT_ID != PRODUCT_ID_AX7K)&&\
			(PRODUCT_ID != PRODUCT_ID_AX7K_I) && \
			(PRODUCT_ID != PRODUCT_ID_AX5K_E) && \
			(PRODUCT_ID != PRODUCT_ID_AX5608) && \
			0 == slot_no){		 
			if(0 == (nbm_cvm_query_card_on(local_port_no-1))){
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
	            ret = NPD_DBUS_SUCCESS;
			}		
		}
	}
	else{
		syslog_ax_eth_port_dbg("show arp info port_index %#x !\n",port_index);
		ret = NPD_DBUS_SUCCESS;
		eth_g_index = port_index;
	}


	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	/* Total slot count*/
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32,
									&ret);
	
	pthread_mutex_lock(&arpHashMutex);
	arpCount = npd_arp_snooping_get_num_byport(eth_g_index); 
	if(0 != arpCount) {
		arpInfo = (struct arp_snooping_item_s **)malloc(arpCount * (sizeof(struct arp_snooping_item_s *)));
		if(NULL == arpInfo) {
			ret = NPD_DBUS_ERROR;
		}
		else {
			memset(arpInfo,0,arpCount * (sizeof(struct arp_snooping_item_s *)));
			npd_arp_snooping_find_item_byport(eth_g_index,arpInfo,arpCount);  
			for(k = 0;k<arpCount;k++){
				tmpArp = (struct arp_snooping_item_s *)malloc(sizeof(struct arp_snooping_item_s ));
				memset(tmpArp,0,sizeof(struct arp_snooping_item_s));
				memcpy(tmpArp,arpInfo[k],sizeof(struct arp_snooping_item_s));
				arpInfo[k] = tmpArp;
				tmpArp = NULL;
			}
		}
	}
	else
		ret = NPD_DBUS_ERROR;
	pthread_mutex_unlock(&arpHashMutex);
	/*printf("learned arp NUM %d on port %d\n",arpCount,eth_g_index);*/

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &arpCount);
	if(NPD_DBUS_SUCCESS == ret) {
		/*get arp count*/

		if(arpCount > 0) {
			dbus_message_iter_open_container (&iter,
											   DBUS_TYPE_ARRAY,
											   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
											            DBUS_TYPE_BYTE_AS_STRING
														DBUS_TYPE_UINT32_AS_STRING   /* ip addr*/
														
														DBUS_TYPE_BYTE_AS_STRING       /* mac*/
														DBUS_TYPE_BYTE_AS_STRING
														DBUS_TYPE_BYTE_AS_STRING
														DBUS_TYPE_BYTE_AS_STRING
														DBUS_TYPE_BYTE_AS_STRING
														DBUS_TYPE_BYTE_AS_STRING
														
														DBUS_TYPE_BYTE_AS_STRING		/*isTrunk*/
														DBUS_TYPE_BYTE_AS_STRING		/*trunk id*/
														DBUS_TYPE_UINT16_AS_STRING	/*vid*/
														DBUS_TYPE_UINT16_AS_STRING	/*vidx*/
														DBUS_TYPE_BYTE_AS_STRING		/*isTagged*/
														DBUS_TYPE_BYTE_AS_STRING		/*isStatic*/
											   DBUS_STRUCT_END_CHAR_AS_STRING,
											   &iter_array);
			
			
			for (i = 0; i < arpCount; i++ ) {
				
				DBusMessageIter iter_struct;
				dbus_message_iter_open_container (&iter_array,
												   DBUS_TYPE_STRUCT,
												   NULL,
												   &iter_struct);
				
				if(NULL != arpInfo[i]) {
					/*printf("arpInfo %p\n",arpInfo);*/
					ipaddr = arpInfo[i]->ipAddr;
					memcpy(mac,arpInfo[i]->mac,6);
					isTrunk = arpInfo[i]->dstIntf.isTrunk;
					trunkId = arpInfo[i]->dstIntf.intf.trunk.trunkId;
					vid = arpInfo[i]->vid;
					vidx = arpInfo[i]->vidx;
					isTagged = arpInfo[i]->isTagged;
					isStatic = arpInfo[i]->isStatic;
                    isValid = arpInfo[i]->isValid;


#if 0
					printf("%-8s %-15s %-17s %-4s %-6s %-6s %-4s\n",	\
						"IFINDEX","IP ADDRESS","MAC ADDRESS","VID","TAG","DEVICE","PORT");
					printf("-------- --------------- ----------------- ---- ------ ------ ----\n");

					printf("%-8x %-3d.%-3d.%-3d.%-3d %02x:%02x:%02x:%02x:%02x:%02x %-4d %-6s %-6d %-4d\n",	\
					arpInfo[i]->ifIndex,(ipaddr>>24)&0xFF,(ipaddr>>16)&0xFF,	\
					(ipaddr>>8)&0xFF,ipaddr&0xFF,mac[0],mac[1],	\
					mac[2],mac[3],mac[4],mac[5],vid,	\
					isTagged ? "TRUE":"FALSE",arpInfo[i]->dstIntf.intf.port.devNum,	\
					arpInfo[i]->dstIntf.intf.port.portNum);
#endif

                   free(arpInfo[i]);
                   arpInfo[i]=NULL;
				}
				else {
					ipaddr = 0;
					memset(mac,0,6);
					isTrunk = 0;
					trunkId = 0;
					vid = 0;
					vidx = 0;
					isTagged = 3;
					isStatic = 0;
                    isValid = 0;
				}
				
                dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_BYTE,
						  &(isValid)); /*isValid*/
                
				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_UINT32,
						  &(ipaddr));  /* ip addr*/
				
				for (j = 0; j < 6; j++ ) {
					
					dbus_message_iter_append_basic
							(&iter_struct,
							  DBUS_TYPE_BYTE,
							  &(mac[j]));  /*mac*/
				}

				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_BYTE,
						  &(isTrunk)); /*is trunkId*/
						  
				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_BYTE,
						  &(trunkId)); /*trunkId*/
						  
				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_UINT16,
						  &(vid)); /*id*/

				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_UINT16,
						  &(vidx)); /*vidx*/

				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_BYTE,
						  &(isTagged)); /*isTagged*/

				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_BYTE,
						  &(isStatic)); /*isStatic*/
                
					
				dbus_message_iter_close_container (&iter_array, &iter_struct);
			}
				
			dbus_message_iter_close_container (&iter, &iter_array);
		}
	
	}
	free(arpInfo);		/* code optimize: recourse leak houxx@autelan.com  2013-1-17 */
	arpInfo = NULL;	
	return reply;
}

DBusMessage* npd_dbus_ethport_show_nexthop(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter  iter,iter_array;
	unsigned char slot_no = 0,local_port_no = 0,slot_index = 0;
	unsigned int port_index = 0,i = 0,j = 0;
	unsigned int eth_g_index = 0;
	unsigned int arpCount = 0;
	unsigned int ipaddr= 0,type = 0;
	unsigned char mac[6] = {0};
	unsigned char isTrunk = 0,trunkId = 0,isTagged = 0,isStatic = 0;
	unsigned short vid =0 ,vidx = 0;
	unsigned int ret = 0;
	struct arp_snooping_item_s *arpInfo = NULL;
	unsigned int *refCntArray = NULL, refCnt = 0;
	DBusError err;
	
	syslog_ax_eth_port_dbg("Entering show ethport arp!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&type,
		DBUS_TYPE_BYTE,&slot_no,
		DBUS_TYPE_BYTE,&local_port_no,
		DBUS_TYPE_UINT32,&port_index,
		DBUS_TYPE_INVALID))) {
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	
	
	if(0 == type){
		syslog_ax_eth_port_dbg("show arp info slot %d/%d !\n",slot_no,local_port_no);
		ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
		if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
			if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
				slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
				ret = NPD_DBUS_SUCCESS;
			}
		}
		if((NPD_DBUS_ERROR_NO_SUCH_PORT == ret)&&\
			(PRODUCT_ID != PRODUCT_ID_AX7K)&&\
			(PRODUCT_ID != PRODUCT_ID_AX7K_I) && \
			(PRODUCT_ID != PRODUCT_ID_AX5K_E) && \
			(PRODUCT_ID != PRODUCT_ID_AX5608) && \
			0 == slot_no){		 
			if(0 == (nbm_cvm_query_card_on(local_port_no-1))){
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
	            ret = NPD_DBUS_SUCCESS;
			}		
		}
	}
	else{
		syslog_ax_eth_port_dbg("show arp info port_index %#x !\n",port_index);
		ret = NPD_DBUS_SUCCESS;
		eth_g_index = port_index;
	}


	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	pthread_mutex_lock(&arpHashMutex);
	arpCount = npd_arp_snooping_get_valid_num_byport_for_nexthop(eth_g_index);  /*add by hanhui for show valid arp*/
	if(0 != arpCount) {
		arpInfo = (struct arp_snooping_item_s *)malloc(arpCount * (sizeof(struct arp_snooping_item_s )));
		if(NULL == arpInfo) 
			ret = NPD_DBUS_ERROR;
		else {
			/*printf("get nexthop info arpInfo %p\n",arpInfo);*/
			memset(arpInfo,0,arpCount * (sizeof(struct arp_snooping_item_s )));
			refCntArray = (unsigned int *)malloc(sizeof(int) * arpCount);
			if(NULL == refCntArray) {
				ret = NPD_DBUS_ERROR;
				free(arpInfo);
				arpInfo = NULL;
			}
			else {
				memset(refCntArray, 0, sizeof(int)*arpCount);
		 		npd_arp_snooping_get_valid_nexthop_item(eth_g_index,arpInfo,arpCount,refCntArray);   /*add by hanhui for show valid arp*/
			}
		}
	}
	pthread_mutex_unlock(&arpHashMutex);
	/*printf("learned arp NUM %d on port %d, %d \n",arpCount,eth_g_index,ret);*/

		/* Total slot count*/
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32,
									&ret);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &arpCount);
	if(NPD_DBUS_SUCCESS == ret) {
		/*get arp count*/

		if(arpCount > 0) {
			dbus_message_iter_open_container (&iter,
											   DBUS_TYPE_ARRAY,
											   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
														DBUS_TYPE_UINT32_AS_STRING   /* ip addr*/
														
														DBUS_TYPE_BYTE_AS_STRING       /* mac*/
														DBUS_TYPE_BYTE_AS_STRING
														DBUS_TYPE_BYTE_AS_STRING
														DBUS_TYPE_BYTE_AS_STRING
														DBUS_TYPE_BYTE_AS_STRING
														DBUS_TYPE_BYTE_AS_STRING
														
														DBUS_TYPE_BYTE_AS_STRING		/*isTrunk*/
														DBUS_TYPE_BYTE_AS_STRING		/*trunk id*/
														DBUS_TYPE_UINT16_AS_STRING	/*vid*/
														DBUS_TYPE_UINT16_AS_STRING	/*vidx*/
														DBUS_TYPE_BYTE_AS_STRING		/*isTagged*/
														DBUS_TYPE_BYTE_AS_STRING		/*isStatic*/
														DBUS_TYPE_UINT32_AS_STRING	/*reference count*/
											   DBUS_STRUCT_END_CHAR_AS_STRING,
											   &iter_array);
			
			
			for (i = 0; i < arpCount; i++ ) {
				
				DBusMessageIter iter_struct;
				dbus_message_iter_open_container (&iter_array,
												   DBUS_TYPE_STRUCT,
												   NULL,
												   &iter_struct);
				
				if(0 != arpInfo[i].ifIndex) {
					/*printf("arpInfo %p\n",arpInfo[i]);*/
					ipaddr = arpInfo[i].ipAddr;
					memcpy(mac,arpInfo[i].mac,6);
					isTrunk = arpInfo[i].dstIntf.isTrunk;
					trunkId = arpInfo[i].dstIntf.intf.trunk.trunkId;
					vid = arpInfo[i].vid;
					vidx = arpInfo[i].vidx;
					isTagged = arpInfo[i].isTagged;
					isStatic = arpInfo[i].isStatic;
					refCnt = refCntArray[i];

#if 0
					printf("%-8s %-15s %-17s %-4s %-6s %-6s %-4s\n",	\
						"IFINDEX","IP ADDRESS","MAC ADDRESS","VID","TAG","DEVICE","PORT");
					printf("-------- --------------- ----------------- ---- ------ ------ ----\n");

					printf("%-8x %-3d.%-3d.%-3d.%-3d %02x:%02x:%02x:%02x:%02x:%02x %-4d %-6s %-6d %-4d\n",	\
					arpInfo[i].ifIndex,(ipaddr>>24)&0xFF,(ipaddr>>16)&0xFF,	\
					(ipaddr>>8)&0xFF,ipaddr&0xFF,mac[0],mac[1],	\
					mac[2],mac[3],mac[4],mac[5],vid,	\
					isTagged ? "TRUE":"FALSE",arpInfo[i].dstIntf.intf.port.devNum,	\
					arpInfo[i].dstIntf.intf.port.portNum);
#endif

				}
				else {
					ipaddr = 0;
					memset(mac,0,6);
					isTrunk = 0;
					trunkId = 0;
					vid = 0;
					vidx = 0;
					isTagged = 3;
					isStatic = 0;
					refCnt = 0;
				}
				
				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_UINT32,
						  &(ipaddr));  /* ip addr*/
				
				for (j = 0; j < 6; j++ ) {
					
					dbus_message_iter_append_basic
							(&iter_struct,
							  DBUS_TYPE_BYTE,
							  &(mac[j]));  /* mac*/
				}

				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_BYTE,
						  &(isTrunk)); /*is trunkId*/
						  
				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_BYTE,
						  &(trunkId)); /*trunkId*/
						  
				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_UINT16,
						  &(vid)); /*vid*/

				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_UINT16,
						  &(vidx)); /*vidx*/

				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_BYTE,
						  &(isTagged)); /*isTagged*/

				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_BYTE,
						  &(isStatic)); /*isStatic*/
						  
				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_UINT32,
						  &(refCnt)); /*reference count*/
					
				dbus_message_iter_close_container (&iter_array, &iter_struct);
			}
				
			dbus_message_iter_close_container (&iter, &iter_array);
		}
	
	}
	free(arpInfo);	/* code optimize: recourse leak houxx@autelan.com  2013-1-17 */
	free(refCntArray);
	arpInfo = NULL;
	refCntArray = NULL;
	return reply;
}
/**********************************************************************************
 * npd_dbus_save_buffer_mode_cfg
 *	This method is used to save buffer mode config entry.
 *
 *	INPUT:
 *		char* showStr
 *		totalLength
 *		currentLen
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *	NOTE:
 *
 **********************************************************************************/
void npd_dbus_save_buffer_mode_cfg
(
	char ** showStr,
	int* avalidLen
)
{
	int len = 0;
	
	if((NULL == showStr)||(NULL == *showStr )||(NULL == avalidLen)) {
		return ;
	}
	else if( 30 > *avalidLen)
		return ;
	else {
		if(CPSS_DXCH_PORT_BUFFERS_MODE_DIVIDED_E == g_buffer_mode) {
			len = sprintf((*showStr),"config buffer mode divided\n");
			*showStr += len;
			*avalidLen -= len;
		}
	}
	return ;
}

/**********************************************************************************
 *  npd_dbus_ethport_vct__cfg
 *	This method is used to save port vct config entry.
 *
 *	INPUT:
 *		char* showStr
 *		totalLength
 *		currentLen
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *	NOTE:
 *
 **********************************************************************************/
void npd_dbus_ethport_vct_cfg
(
	char ** showStr,
	int* avalidLen
)
{
	int len = 0;
	unsigned int	i = 0, j = 0, g_index = 0;
	struct eth_port_s *portPtr = NULL;	
	struct eth_vct_info *vct_info = NULL;
	
	if((NULL == showStr)||(NULL == *showStr )||(NULL == avalidLen)) {
		return ;
	}
	else if( 30 > *avalidLen) {
		return ;
	}
	else {
		for (i = 0; i < CHASSIS_SLOT_COUNT; i++ ) { 
			for (j = 0; j < ETH_LOCAL_PORT_COUNT(i); j++) {
				g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(i, j);
				
				portPtr = npd_get_port_by_index(g_index);
				
				if ((NULL == portPtr) || (portPtr->port_type == ETH_GE_SFP) 
					|| (portPtr->port_type == ETH_GE_FIBER)) { /*only SFP can operate*/
					/*can not suport fiber */
					continue;
				}
				else {
					if ((portPtr->funcs.funcs_run_bitmap) & (1<< ETH_PORT_FUNC_OCT_INFO)) {
						vct_info = portPtr->funcs.func_data[ETH_PORT_FUNC_OCT_INFO];
						if(NULL != vct_info) {
							if (vct_info->vct_isable) {
								len = sprintf((*showStr), "config eth-port %d/%d vct state enable\n", (i+1), (j+1));
								*showStr += len;
								*avalidLen -= len;
							}
						}
					}
				}
			}
		}
	}
	return ;
}

/***********************************************
 *npd_eth_port_get_slotno_portno_by_eth_g_index
 *		get slot no. and port no. by global index
 * INPUT:
 *		eth_g_index - global index
 * OUTPUT:
 *		slot_no  - the slot no. we got
 *		port_no - the port no. we got
 *
 ***********************************************/
unsigned int npd_eth_port_get_slotno_portno_by_eth_g_index
(
    unsigned int eth_g_index,
    unsigned char *slot_no,
    unsigned char *port_no
)
{
    unsigned int slot_index = 0;
	unsigned int port_index = 0;

	slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	*slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
	*port_no = ETH_LOCAL_INDEX2NO(slot_index,port_index);
	
    return NPD_SUCCESS;
}

/*******************************************************************************
 * npd_eth_port_rgmii_type_check
 *		connect to CPU RGMII interface 
 * INPUT:
 *		slot_no - the slot no. of the port we want to check (eg. 0~4 in PRODUCT_ID_AX7K)
 *		port_no - the port no. of the port we want to check (eg. 1~52 in PRODUCT_ID_AU3K)
 * OUTPUT:
 *		NONE
 * RETURN:
 *		TRUE - the port is connect to CPU RGMII interface
 *		FALSE - the port is NOT connect to CPU RGMII interface
 * NOTE:
 *
 *******************************************************************************/
unsigned int npd_eth_port_rgmii_type_check
(
	unsigned int slot_no,
	unsigned int port_no
)
{
	if(((PRODUCT_ID_AX7K == PRODUCT_ID)||(PRODUCT_ID_AX7K_I == PRODUCT_ID))&&\
		(AX7_CRSMU_SLOT_NUM == slot_no)){
		return TRUE;
	}
	else if(PRODUCT_ID_AX5K_I == PRODUCT_ID){
		/* eth-port 1/9 ~ 1/12 */
		if((1 == slot_no)&&\
		((port_no > 8)&&(port_no <= 12))){
		return TRUE;
		}
	}
	else if(PRODUCT_ID_AX5K_E == PRODUCT_ID || PRODUCT_ID_AX5608== PRODUCT_ID) {
		return TRUE;
	}
	else if((0 == slot_no)&&\
		(port_no > 0)&&(port_no <= 2)&&\
		(0 == nbm_cvm_query_card_on(port_no - 1))&&\ 
		(1 != cvm_query_card_type(port_no - 1))){/* cardNum = port_no -1*/
		return TRUE;
	}
	return FALSE;
}

/**************************************************************
 *npd_eth_port_get_global_indexes_by_mode
 *
 *DESCRIPTION:
 *    get port bitmap by mode
 *INPUT:
 *    mode  -- the mode what by we want to get the bitmap 
 *OUTPUT:
 *    portBmp  --  the bitmap we got 
 *RETURN:
 *	0
 *NOTE:
 *    we assume the port is not more than 64, and the portBmp should be a 
 * integer array which length is 2
 *
 **************************************************************/
unsigned int npd_eth_port_get_global_indexes_by_mode(unsigned int * eth_g_indexs,unsigned int * count,unsigned int mode){
	unsigned int eth_g_index = 0;
	unsigned int slot_index = 0;
	unsigned int port_index = 0;
	unsigned int slotNo = 0;
	unsigned int localPortNo = 0;
	unsigned int i = 0;
	unsigned int ifIndex = ~0UI;
	unsigned tempCount = 0;
    if((NULL == eth_g_indexs)||(NULL == count)){
        return INTERFACE_RETURN_CODE_ERROR;
    }
	if(0 < (*count)){
		memset(eth_g_indexs,0,sizeof(int)*(*count));
	}	
	for(i = 0; i < 64; i++){
		if(PRODUCT_ID_AX7K == PRODUCT_ID) {
			slotNo = i/8 + 1;
			localPortNo = i%8;
		}
		else if((PRODUCT_ID_AX5K == PRODUCT_ID) ||
				(PRODUCT_ID_AU4K == PRODUCT_ID) ||
				(PRODUCT_ID_AU3K == PRODUCT_ID) ||
				(PRODUCT_ID_AU3K_BCM == PRODUCT_ID) ||
				(PRODUCT_ID_AU3K_BCAT == PRODUCT_ID) ||
				(PRODUCT_ID_AU2K_TCAT == PRODUCT_ID)){
			slotNo = 1;
			localPortNo = i;
		}
		else if(PRODUCT_ID_AX5K_I == PRODUCT_ID) {
			/* ports 1/9 ~ 1/12 are out of control which 
 			 * 	 connect to CPU RGMII interface directly
 			 *	by qinhs@autelan.com 2009/7/27
			 */ 
			if( i >= 9 ) {
				continue;
			}
			else {
				slotNo = 1;
				localPortNo = i;
			}
		}
		if (ETH_LOCAL_PORTNO_ISLEGAL(slotNo,localPortNo)) {
			slot_index = CHASSIS_SLOT_NO2INDEX(slotNo);
			port_index = ETH_LOCAL_NO2INDEX(slot_index, localPortNo);
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,port_index);
		
			if(ETH_PORT_FUNC_BRIDGE == mode){
				if(NPD_TRUE == npd_check_port_switch_mode(eth_g_index)){
	                eth_g_indexs[tempCount++] = eth_g_index;
					if(tempCount >= *count){
						break;
					}
				}
			}else if(ETH_PORT_FUNC_IPV4 == mode ){
				if((NPD_TRUE == npd_eth_port_interface_check(eth_g_index,&ifIndex))&&(ifIndex != ~0UI)){
					eth_g_indexs[tempCount++] = eth_g_index;
					if(tempCount >= *count){
						break;
					}
		        }
			}
			else if(ETH_PORT_FUNC_MODE_PROMISCUOUS == mode){
		        if((TRUE != npd_eth_port_rgmii_type_check(slotNo,localPortNo))&&\
					(NPD_TRUE == npd_check_port_promi_mode(eth_g_index))){
					eth_g_indexs[tempCount++] = eth_g_index;
					if(tempCount >= *count){
						break;
					}
		        }
			}
		}
    }
	*count = tempCount;
	return INTERFACE_RETURN_CODE_SUCCESS;
}
npd_eth_port_check_subif_and_qinq_by_vid
(
    unsigned short vid
)
{
    unsigned int eth_g_index[64]={0};
    unsigned int count = 0;
    unsigned int i = 0;
    unsigned int ret = FALSE;
    memset(eth_g_index,0,64);
    npd_eth_port_get_global_indexes_by_mode(eth_g_index,&count,ETH_PORT_FUNC_MODE_PROMISCUOUS);
    for(i = 0;i < count;i++){
        if(npd_eth_port_check_tag_for_other_subif(eth_g_index[i],0,vid)){
            ret = TRUE;
            break;
        }
    }
    return ret;
}

/**********************************************************************************
 *  npd_eth_port_reinit
 *
 *	DESCRIPTION:
 * 		eth-port reinit when hot plugin
 *
 *	INPUT:
 *		chassis_slot_index - eth-port slot index
 *		local_port_index - eth-port port index on slot
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - find position to add port.
 *		
 *
 **********************************************************************************/
int npd_eth_port_reinit
(
	unsigned int chassis_slot_index,
	unsigned int local_port_index
)
{
	unsigned char dev = 0, port = 0;
	unsigned int retval = NPD_SUCCESS;
	struct eth_port_s*  eth_port = NULL;
	unsigned int eth_g_index = 0;

	eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(chassis_slot_index,local_port_index);

	eth_port = npd_get_port_by_index(eth_g_index);
	if(NULL == eth_port){
         syslog_ax_eth_port_err("port %#x not exist when reinit!\n",eth_g_index);
         return ETHPORT_RETURN_CODE_NO_SUCH_PORT;		 
	}

	retval = npd_get_devport_by_global_index(eth_g_index,&dev,&port);
	if(0 != retval){
		syslog_ax_eth_port_err("port %#x reinit get asic port failed %d\n",eth_g_index, retval);
		return ETHPORT_RETURN_CODE_BAD_VALUE;
	}
	
	retval = nam_eth_port_phy_reinit(dev, port);
	if(NPD_SUCCESS != retval) {
		syslog_ax_eth_port_err("port %#x reinit get asic port failed %d\n",eth_g_index, retval);
		retval = ETHPORT_RETURN_CODE_ERR_OPERATE;
	}
	syslog_ax_eth_port_dbg("port %d/%d index %#x reinit get asic port ok\n", \
							chassis_slot_index, local_port_index + 1,eth_g_index);
	
	syslog_ax_eth_port_dbg("reinit create port %d/%d!\n",chassis_slot_index, local_port_index + 1);
	npd_create_eth_port(chassis_slot_index,local_port_index);

	return retval;
}

#ifdef __cplusplus
}
#endif

