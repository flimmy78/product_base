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
* npd_board_ax71_2x12g12s.c
*
*
* CREATOR:
*		luoxun@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for special board routine.
*
* DATE:
*		03/24/2011	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.15 $	
*******************************************************************************/
#ifdef __cplusplus
    extern "C"
    {
#endif
    
#include <stdio.h>
#include "npd_board.h"
#include "npd_log.h"
#include "npd_board_ax71_2x12g12s.h"
#include "npd_trunk.h"

#define  SIZE_16K  0x4000
#define  SIZE_8K   0x2000

extern char g_dis_dev[MAX_ASIC_NUM];

/* Add fro this board */

ASIC_PORT_ATTRIBUTE_S ax71_2x12g12s_attr_ge =
{
	ETH_ATTR_ENABLE,1536,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
	ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
	ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_NONE
};
ASIC_PORT_ATTRIBUTE_S ax71_2x12g12s_attr_xg =
{
	ETH_ATTR_ENABLE,1536,PORT_FULL_DUPLEX_E,PORT_SPEED_10000_E,	\
	ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_OFF,ETH_ATTR_OFF,	\
	ETH_ATTR_OFF,ETH_ATTR_OFF,COMBO_PHY_MEDIA_PREFER_NONE
};

struct eth_port_func_data_s ax71_2x12g12s_funcs = 
{
	.funcs_cap_bitmap =  (1 << ETH_PORT_FUNC_PORT_BASED_VLAN)| \
                		 (1 << ETH_PORT_FUNC_DOT1Q_BASED_VLAN)| \
                		 (1 << ETH_PORT_FUNC_DOT1W)| \
                		 (1 << ETH_PORT_FUNC_LINK_AGGREGATION)| \
                		 (1 << ETH_PORT_FUNC_IPV4)| \
                		 (1 << ETH_PORT_FUNC_ACL_RULE)| \
                		 (1 << ETH_PORT_FUNC_QoS_PROFILE)| \
                		 (1 << ETH_PORT_FUNC_FDB)| \
                		 (1 << ETH_PORT_FUNC_STORM_CONTROL)| \	
                		 (1 << ETH_PORT_FUNC_PROTOCOL_BASED_VLAN),
                		
    .funcs_run_bitmap =	 (1 << ETH_PORT_FUNC_LINK_AGGREGATION)| \
                		 (1 << ETH_PORT_FUNC_PORT_BASED_VLAN)| \
                		 (1 << ETH_PORT_FUNC_DOT1Q_BASED_VLAN),
    .func_data=NULL,    /* void *func_data[ETH_PORT_FUNC_MAX]; */	
};

struct eth_port_counter_s ax71_2x12g12s_counters=
{
};

asic_eth_port_t ax71_2x12g12s_eth_ports[28]={   \
	{
		ETH_XGE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x18,0x0, \
		&ax71_2x12g12s_attr_xg,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},
	{
		ETH_XGE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x19,0x0, \
		&ax71_2x12g12s_attr_xg,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x1,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x0,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x3,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x2,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x5,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x4,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x7,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x6,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x9,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x8,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0xb,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0xa,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0xe,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0xf,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0xc,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0xd,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x12,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x13,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x10,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x11,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x16,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x17,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x14,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},		
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x15,0x0, \
		&ax71_2x12g12s_attr_ge,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},
	{
		ETH_XGE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x1a,0x0, \
		&ax71_2x12g12s_attr_xg,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	},
	{
		ETH_XGE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x1b,0x0, \
		&ax71_2x12g12s_attr_xg,
		&ax71_2x12g12s_funcs,
		&ax71_2x12g12s_counters
	}
};

/* panle port to virtual port map array */
/* 2-1 is dev_port(0,0x18) */
unsigned int ax71_2x12g12s_port_mapArr[28] = {
    0x18,0x19,0x1,0x0,0x3,0x2,0x5,0x4,0x7,0x6,
    0x9,0x8,0xb,0xa,0xe,0xf,0xc,0xd,0x12,0x13,
    0x10,0x11,0x16,0x17,0x14,0x15,0x1A,0x1B
};

asic_cscd_port_t ax71_2x12g12s_cscd_ports[2]={
    {0,26},
    {0,27}
};
/* I am switch board, just set to -1, to avoid Segmentation fault of npd_vlan_to_cpu_port_add */
asic_cpu_port_t ax71_2x12g12s_to_cpu_ports[MAX_CPU_PORT_NUM] = {
    {-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},    /* connect to master CPU */
	{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}     /* connect to slave CPU */
};
int npd_ax71_2x12g12s_dev_map_table_set(void)
{
	int ret = RET_FAIL;
	int i = 0;
	cscd_trunk_link_type smu_linktype,standby_linktype;

	smu_linktype.linkNum = 26;
	smu_linktype.linkType = CSCD_TRUNK_LINK_TYPE_PORT_E;
	standby_linktype.linkNum = 27;
	standby_linktype.linkType = CSCD_TRUNK_LINK_TYPE_PORT_E;
	
	switch(PRODUCT_TYPE)
	{
		case AUTELAN_PRODUCT_AX8610:
			if(SLOT_ID < 5)
			{
				ret = nam_asic_redistribute_devnum(0,(SLOT_ID*2 - 2));
				if(ret != RET_SUCCESS)
				{
					syslog_ax_board_err("set distributed devnum FAIL !\n");
					return ret;
				}
				g_dis_dev[0] = SLOT_ID*2 - 2;
			}
			else if(SLOT_ID > 5)
			{
				ret = nam_asic_redistribute_devnum(0,(SLOT_ID*2 + 2));
				if(ret != RET_SUCCESS)
				{
					syslog_ax_board_err("set distributed devnum FAIL !\n");
					return ret;
				}
				g_dis_dev[0] = SLOT_ID*2 + 2;
			}
			for(i=0;i<=22;i=i+2)
			{
				if(SLOT_ID < 5)	 
				{
					if((SLOT_ID*2 - 2) == i)
						continue;
				}
				else if(SLOT_ID > 5)
				{
					if((SLOT_ID*2 + 2) == i)
						continue;
				}
				if((i == 10) || (i == 12) || (i == 14))
				{
					continue;
				}
				ret = nam_cscd_dev_map_set(0,i,0,&smu_linktype,0);/*for MASTER slot*/
				if(ret != RET_SUCCESS)
				{
					syslog_ax_board_err("set dev map table FAIL !\n");
					return ret;
				}
				#if 0
				ret = nam_cscd_dev_map_set(0,i,0,&standby_linktype,0);/*for STANDBY slot*/
				if(ret != NPD_SUCCESS)
				{
					syslog_ax_board_err("set dev map table FAIL !\n");
					return ret;
				}
				#endif
			}
			break;

		default:
			syslog_ax_board_warning("unsupport PRODUCT TYPE !!!\n");
			break;
	}
	syslog_ax_board_dbg("npd ax81_2x12g12s dev map table set successfully !\n");
	return RET_SUCCESS;
}

unsigned int ax71_2x12g12s_test(void) 
{
	int ret = 0;
	unsigned int vid = 4000;
	unsigned int devnum = 0;   /* no use,just for printf */

	/* xcat */
	devnum = 0;
#if 0  /* for AX81_AC12C test */
	vid = 1000;
    ret = create_vlan(devnum,vid);
	if(0 != ret){
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;		
	}	
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 26, 1);  /* (0,26) */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0, 0);  /* (0,0) */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}
	
	vid = 4000;
    ret = create_vlan(devnum,vid);
	if(0 != ret){
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;		
	}	
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 27, 1);  /* (0,27) */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 1, 0);  /* (0,1) */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}
#endif

#if 1  /* for AX81_1X12G12S test */
	vid = 300;
    ret = create_vlan(devnum,vid);
	if(0 != ret){
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;		
	}	
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 24, 0);  /* (0,26) */
	if(0 != ret){
		syslog_ax_board_err("add tagged ports to new vlan %d error.\n",vid);
		return ret;		
	}
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 1, 0);  /* (0,1) */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}
	vid = 400;
    ret = create_vlan(devnum,vid);
	if(0 != ret){
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;		
	}	
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 25, 0);  /* (0,26) */
	if(0 != ret){
		syslog_ax_board_err("add tagged ports to new vlan %d error.\n",vid);
		return ret;		
	}
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0, 0);  /* (0,1) */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
#endif

#if 0
	#if 0  /* for AX81-2X12G12S test on AX7605i,set port 26 27 untag to AX71_CRSMU on slot-1 */
    force_to_xg_mode();
	vid = 1000;
    create_vlan(devnum,vid);
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x18, 0);  /* 2-1 */
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x1b, 0);  /* port27: 2-28 */

	vid = 2000;
    create_vlan(devnum,vid);
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x15, 0);  /* 2-26 */
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x1a, 0);  /* port26: 2-27 */	
	#else   /* for AX71-2X12G12S test on AX7605i */
	vid = 1000;
    create_vlan(devnum,vid);
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x18, 0);  /* 2-1 */
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x1b, 1);  /* port27: 2-28 */

	vid = 2000;
    create_vlan(devnum,vid);
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x15, 0);  /* 2-26 */
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x1a, 1);  /* port26: 2-27 */
    #endif
    vid = 3000;
    create_vlan(devnum,vid);
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x19, 0);  /* 2-2 */
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x1, 0);   /* 2-3 */
    vid--;
	
    create_vlan(devnum,vid);
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x0, 0);  
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x3, 0);
    vid--;
	
    create_vlan(devnum,vid);
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x2, 0);  
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x5, 0);
    vid--;

    create_vlan(devnum,vid);
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x4, 0);  
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x7, 0);
    vid--;

    create_vlan(devnum,vid);
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x6, 0);  
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x9, 0);
    vid--;

    create_vlan(devnum,vid);
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x8, 0);  
    nam_asic_vlan_entry_ports_add(devnum, vid, 0xb, 0);
    vid--;
							     
    create_vlan(devnum,vid);
    nam_asic_vlan_entry_ports_add(devnum, vid, 0xa, 0);  
    nam_asic_vlan_entry_ports_add(devnum, vid, 0xe, 0);
    vid--;

    create_vlan(devnum,vid);
    nam_asic_vlan_entry_ports_add(devnum, vid, 0xf, 0);  
    nam_asic_vlan_entry_ports_add(devnum, vid, 0xc, 0);
    vid--;

    create_vlan(devnum,vid);
    nam_asic_vlan_entry_ports_add(devnum, vid, 0xd, 0);  
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x12, 0);
    vid--;

    create_vlan(devnum,vid);
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x13, 0);  
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x10, 0);
    vid--;

    create_vlan(devnum,vid);
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x11, 0);  
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x16, 0);
    vid--;
	
    create_vlan(devnum,vid);
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x17, 0);  
    nam_asic_vlan_entry_ports_add(devnum, vid, 0x14, 0);
#endif	
    syslog_ax_board_dbg("XCAT ge_xg_test ready!\n");
	return 0;
}

asic_board_t ax71_2x12g12s_board_t =
{
    .board_type = 0,
    .board_name = "ax71/81_2x12g12s",
    .slot_id = -1,
    .asic_type = ASIC_TYPE_CPSS,
    .capbmp = ASIC_CAPBMP,
	.runbmp = ASIC_RUNBMP,
	
    .asic_port_cnt = 28,    /* asic port on chip  */
    .asic_cscd_port_cnt = 2,
    .asic_port_cnt_visable = 26, /* asic port on panel */    
    .asic_port_start_no = 1,

    .asic_port_mapArr = NULL,
	.asic_eth_ports = ax71_2x12g12s_eth_ports,
    .asic_cscd_ports = ax71_2x12g12s_cscd_ports,
    .asic_to_cpu_ports = ax71_2x12g12s_to_cpu_ports,
    
	.id = 0xF0F0F0F0,
	.state = MODULE_STAT_RUNNING,
	.hw_version = 0x0,
	.sn = "not_init",
	.ext_slot_data = "not_init",   
	
	.func_test=NULL,
    .asic_init=NULL,
    .my_equipment_test = ax71_2x12g12s_test,
    .distributed_system_dev_map_table_set = npd_ax71_2x12g12s_dev_map_table_set,
    .board_cscd_port_trunk_config = NULL
};




/**********************************************************************
 * ax71_2x12g12s_port_config
 *
 *  DESCRIPTION:
 *          port config for AX71_2X12G12S.
 *
 *  INPUT:
 *          None
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          RET_SUCCESS
 *          RET_FAIL
 *
 *  COMMENTS:
 *          None
 *
 **********************************************************************/
static int ax71_2x12g12s_port_config(void)
{
    int ret = RET_FAIL;
    unsigned char devNum  = 0;
    unsigned char portNum = 12;
    
    /* Port 12-23 is fiber port, inband autoneg must enable. */
    for (portNum=12; portNum<=23; portNum++)
    {
        ret = nam_set_ethport_inband_autoneg(devNum, portNum, ENABLE);
        if (RET_SUCCESS != ret)
        {
            syslog_ax_board_err("AX71_2X12G12S port %d inband autonegotiation enable fail.\n", portNum);
            return RET_FAIL;
        }
        syslog_ax_board_dbg("AX71_2X12G12S port %d inband autonegotiation enable.\n", portNum);
    }

    return RET_SUCCESS;
}



/**********************************************************************
 * firmware_byte_write
 *
 *  DESCRIPTION:
 *          Write firmware bin file to QT2225 one by one.
 *
 *  INPUT:
 *          portNum
 *          phyDev
 *          phyReg
 *          dataPtr
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          RET_SUCCESS
 *          RET_FAIL
 *
 *  COMMENTS:
 *          None
 *
 **********************************************************************/
int firmware_byte_write(unsigned char  portNum,
                             unsigned char  phyDev, 
                             unsigned short phyReg,
                             unsigned short dataPtr)
{
    int ret = RET_FAIL;
    int old_dataPtr = 0;
    unsigned char  devNum  = 0;
    unsigned char  phyId   = portNum;
    unsigned char  useExternalPhy = 1;
	
    
    dataPtr = dataPtr & 0x00ff;
    ret = nam_asic_auxi_phy_write(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
    NAM_ASIC_AUXI_PHY_WRITE_CHECK(old_dataPtr,ret,devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
	
    return RET_SUCCESS;
}


/**********************************************************************
 * firmware_bin_print
 *
 *  DESCRIPTION:
 *          Used for debug.
 *
 *  INPUT:
 *          buf
 *          size
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          None
 *
 *  COMMENTS:
 *          None
 *
 **********************************************************************/
void firmware_bin_print(const char *buf, const unsigned int size)
{
    int i = 0;
    for(i=0; i<size; i++)
    {
        printf(" ");
        if ( i%8 == 0 )
            printf("\n");
        
        if( buf[i] & 0x80 )
            printf("1");
        else
            printf("0");
        if( buf[i] & 0x40 )
            printf("1");
        else
            printf("0");
        if( buf[i] & 0x20 )
            printf("1");
        else
            printf("0");
        if( buf[i] & 0x10 )
            printf("1");
        else
            printf("0");
        if( buf[i] & 0x08 )
            printf("1");
        else
            printf("0");
        if( buf[i] & 0x04 )
            printf("1");
        else
            printf("0");
        if( buf[i] & 0x02 )
            printf("1");
        else
            printf("0");
        if( buf[i] & 0x01 )
            printf("1");
        else
            printf("0");
    }
    printf("\n");

    #if 0
    for(i=0; i<size; i++)
        osPrintf("buf[%d]=%02x\n", i, buf[i]);
    #endif
}

/**********************************************************************
 * phy_QT2225_access_prepare
 *
 *  DESCRIPTION:
 *          Configration of QT2225 before firmware download.
 *
 *  INPUT:
 *          portNum
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          RET_SUCCESS
 *          RET_FAIL
 *
 *  COMMENTS:
 *          Asic register 0x01800180 bit 14 must be set 1, otherwise,
 *          registers of QT2225 cannot be accessed.
 *
 **********************************************************************/
int phy_QT2225_access_prepare(unsigned char  portNum)
{
    int ret = RET_FAIL;
    unsigned char devNum  = 0;
    unsigned int  portGroupId = 0;
    unsigned int  regAddr = 0x01800180;
    unsigned int  regValue = 0;
    unsigned char  phyId   = portNum;
    unsigned char  useExternalPhy = 1;
    unsigned short phyReg  = 0xC300;
    unsigned char  phyDev  = 1;
    unsigned short dataPtr = 0;
    unsigned short old_dataPtr = 0;
	
    ret = nam_read_reg(devNum, portGroupId, regAddr, &regValue);
    if(RET_SUCCESS != ret)
    {
        syslog_ax_board_err("nam_read_reg fail, ret=%d\n", ret);
        return RET_FAIL;
    }

    /* set asic 0x01800180 bit 14 to 1 */
    regValue = regValue | 0x4000;
    ret = nam_write_reg(devNum, portGroupId, regAddr, regValue);
    if(RET_SUCCESS != ret)
    {
        syslog_ax_board_err("nam_write_reg fail, ret=%d\n", ret);
        return RET_FAIL;
    }

    ret = nam_asic_auxi_phy_read(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, &dataPtr);
    if(RET_SUCCESS != ret)
    {
        syslog_ax_board_err("nam_asic_auxi_phy_read fail, ret=%d\n", ret);
        return RET_FAIL;
    }

    /* set QT2225 1.C300 to 0. [Places internal micro into reset state] */
    phyDev  = 1;
    phyReg  = 0xC300;
    dataPtr = 0;
    ret = nam_asic_auxi_phy_write(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
    NAM_ASIC_AUXI_PHY_WRITE_CHECK(old_dataPtr,ret,devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
	

    /* set QT2225 1.C302 */
    phyDev  = 1;
    phyReg  = 0xC302;
    dataPtr = 0x0004;
    ret = nam_asic_auxi_phy_write(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
    NAM_ASIC_AUXI_PHY_WRITE_CHECK(old_dataPtr,ret,devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
	
    /* set QT2225 */
    phyDev  = 1;
    phyReg  = 0xC319;
    dataPtr = 0x0078;
    ret = nam_asic_auxi_phy_write(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
    NAM_ASIC_AUXI_PHY_WRITE_CHECK(old_dataPtr,ret,devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);

    /* set QT2225 */
    phyDev  = 1;
    phyReg  = 0xC31A;
    dataPtr = 0x0098;
    ret = nam_asic_auxi_phy_write(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
    NAM_ASIC_AUXI_PHY_WRITE_CHECK(old_dataPtr,ret,devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);

    /* set QT2225 */
    phyDev  = 3;
    phyReg  = 0x0026;
    dataPtr = 0x0E00;
    ret = nam_asic_auxi_phy_write(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
    NAM_ASIC_AUXI_PHY_WRITE_CHECK(old_dataPtr,ret,devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);

    /* set QT2225 */
    phyDev  = 3;
    phyReg  = 0x0027;
    dataPtr = 0x0093;
    ret = nam_asic_auxi_phy_write(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
    NAM_ASIC_AUXI_PHY_WRITE_CHECK(old_dataPtr,ret,devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);

    /* set QT2225 */
    phyDev  = 3;
    phyReg  = 0x0028;
    dataPtr = 0xA528;
    ret = nam_asic_auxi_phy_write(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
    NAM_ASIC_AUXI_PHY_WRITE_CHECK(old_dataPtr,ret,devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);

    /* set QT2225 */
    phyDev  = 3;
    phyReg  = 0x0029;
    dataPtr = 0x0003;
    ret = nam_asic_auxi_phy_write(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
    NAM_ASIC_AUXI_PHY_WRITE_CHECK(old_dataPtr,ret,devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);

    /* set QT2225 1.C300 bit 1 to 1 */
    phyDev  = 1;
    phyReg  = 0xC300;
    dataPtr = 0x0002;
    ret = nam_asic_auxi_phy_write(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
    NAM_ASIC_AUXI_PHY_WRITE_CHECK(old_dataPtr,ret,devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);

    /* set QT2225 3.E854 bit 7 to 1 */
    phyDev  = 3;
    phyReg  = 0xE854;
    dataPtr = 0x00C0;
    ret = nam_asic_auxi_phy_write(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
    NAM_ASIC_AUXI_PHY_WRITE_CHECK(old_dataPtr,ret,devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);

    return RET_SUCCESS;
}


/**********************************************************************
 * phy_QT2225_config
 *
 *  DESCRIPTION:
 *          Configration of QT2225 after firmware download.
 *
 *  INPUT:
 *          portNum
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          RET_SUCCESS
 *          RET_FAIL
 *
 *  COMMENTS:
 *          None
 *
 **********************************************************************/
int phy_QT2225_config(unsigned char  portNum)
{
    int ret = -1;
    unsigned char  devNum  = 0;
	unsigned char  phyId   = portNum;
	unsigned char  useExternalPhy = 1;
	unsigned short phyReg  = 0;
	unsigned char  phyDev  = 0;
	unsigned short dataPtr = 0;
    unsigned short old_dataPtr = 0;
    
    /* set QT2225 3.E854 bit 7 to 0, bit 6 to 1 */
    phyDev  = 3;
    phyReg  = 0xE854;
    dataPtr = 0x0040;
    ret = nam_asic_auxi_phy_write(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
    NAM_ASIC_AUXI_PHY_WRITE_CHECK(old_dataPtr,ret,devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
	

    /* check for firmware heartbeat */

    /* set LED */
    phyDev  = 1;
    phyReg  = 0xD006;
    dataPtr = 0x9;
    ret = nam_asic_auxi_phy_write(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
#if 0
    NAM_ASIC_AUXI_PHY_WRITE_CHECK(old_dataPtr,ret,devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
#endif
    phyDev  = 1;
    phyReg  = 0xD007;
    dataPtr = 0x2;
    ret = nam_asic_auxi_phy_write(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
#if 0
    NAM_ASIC_AUXI_PHY_WRITE_CHECK(old_dataPtr,ret,devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
#endif
    phyDev  = 1;
    phyReg  = 0xD008;
    dataPtr = 0xA;
    ret = nam_asic_auxi_phy_write(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
#if 0
    NAM_ASIC_AUXI_PHY_WRITE_CHECK(old_dataPtr,ret,devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
#endif

    /* Invert polarity for SFP+/XFP */
    phyDev  = 1;
    phyReg  = 0xC301;
    dataPtr = 0x2084;
    ret = nam_asic_auxi_phy_write(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);	
#if 0
    NAM_ASIC_AUXI_PHY_WRITE_CHECK(old_dataPtr,ret,devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
#endif
	
    /* XAUI Lane Map */
    phyDev  = 4;
    phyReg  = 0xC210;
    dataPtr = 0x1B1B;
    ret = nam_asic_auxi_phy_write(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
#if 0
    NAM_ASIC_AUXI_PHY_WRITE_CHECK(old_dataPtr,ret,devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
#endif
	
    return RET_SUCCESS;
}



/**********************************************************************
 * phy_QT2225_firmware_load
 *
 *  DESCRIPTION:
 *          Firmware load for QT2225.
 *
 *  INPUT:
 *          portNum
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          RET_SUCCESS
 *          RET_FAIL
 *
 *  COMMENTS:
 *          None
 *
 **********************************************************************/
int phy_QT2225_firmware_load(unsigned char  portNum)
{
    int ret = RET_FAIL;
    unsigned char  devNum  = 0;
    unsigned char  phyId   = portNum;
    unsigned char  useExternalPhy = 1;
    unsigned short phyReg  = 0;
    unsigned char  phyDev  = 0;
    unsigned short dataPtr = 0;
    FILE *fp = NULL;
    const char *filename = "/opt/bin/QT2x25D_module_firmware_v2_0_3_0_MDIO.bin";
    unsigned int buf_size = (SIZE_16K + SIZE_8K);
    char buf[buf_size];
    int i = 0;

    ret = phy_QT2225_access_prepare(portNum);
    if(RET_SUCCESS != ret)
    {
        syslog_ax_board_err("phy_QT2225_access_prepare fail, ret=%d\n", ret);
        return RET_FAIL;
    }

    fp = fopen(filename, "rb");
	if(fp == NULL)
	{
        syslog_ax_board_err("fopen firmware failed.\n");
        return RET_FAIL;		
	}
	
    fread(buf, sizeof(char), buf_size, fp);
    firmware_bin_print(buf, 50);

    for (i=0; i<buf_size; i++)
    {
        if (i<SIZE_16K)
        {
            ret=firmware_byte_write(portNum, 3, 0x8000+i, buf[i]); /* 3.8000 - 3.BFFF */
            if(ret != RET_SUCCESS)
            {
                syslog_ax_board_err("firmware_byte_write fail,ret = %d\n",ret);
				fclose(fp);     /* code optimize: Resource leak. zhangdi@autelan.com 2013-01-18 */
                return RET_FAIL;
            }
        }
        else
        {
            ret=firmware_byte_write(portNum, 4, 0x8000+(i-SIZE_16K), buf[i]); /* 4.8000 - 4.9FFF */
            if(ret != RET_SUCCESS)
            {
                syslog_ax_board_err("firmware_byte_write fail,ret = %d\n",ret);
        		fclose(fp);				
                return RET_FAIL;
            }
        }
    }

    ret = phy_QT2225_config(portNum);
    if(RET_SUCCESS != ret)
    {
        syslog_ax_board_err("phy_QT2225_access_prepare fail, ret=%d\n", ret);
		fclose(fp);		
        return RET_FAIL;
    }

    return RET_SUCCESS;
}


/**********************************************************************
 * mac_config_for_phy_QT2225
 *
 *  DESCRIPTION:
 *          MAC configration for QT2225.
 *
 *  INPUT:
 *          None
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          RET_SUCCESS
 *          RET_FAIL
 *
 *  COMMENTS:
 *          None
 *
 **********************************************************************/
void mac_config_for_phy_QT2225(void)
{
    int i = 0;
    int unit = 0;
    unsigned int portGroupId = 0;
    unsigned int data = 0;
    unsigned int regAddr = 0;

	#if 0
    /* Fixed IPG */	
    for (regAddr=0x0A800048; regAddr<=0x0A805C48; regAddr+=0x400)
    {
        nam_write_reg(unit, portGroupId, regAddr, 0x200);
    }
	#endif
    
    nam_write_reg(unit, portGroupId, 0x0A806048, 0x200);
    nam_write_reg(unit, portGroupId, 0x0A806448, 0x200);
    nam_write_reg(unit, portGroupId, 0x0A806848, 0x200);
    nam_write_reg(unit, portGroupId, 0x0A806C48, 0x200);
    
    regAddr = 0x08806000;
    for (i=0; i<4; i++)
    {
        nam_read_reg(unit, portGroupId, regAddr, &data);
        data = data | (1<<7);   /*set bit7 to 1*/
        data = data & (~(1<<6));/*set bit6 to 0*/
        nam_write_reg(unit, portGroupId, regAddr, data);
        regAddr += 0x400;
    }

    regAddr = 0x08806008;
    for (i=0; i<4; i++)
    {
        nam_read_reg(unit, portGroupId, regAddr, &data);
        data = data | (1<<10);   /*set bit10 to 1*/
        nam_write_reg(unit, portGroupId, regAddr, data);
        regAddr += 0x400;
    }
    syslog_ax_board_dbg("Set IPG to 8 bytes of XCAT OK !\n");
    
}

int npd_disable_promiscuous_port(unsigned char dest_slot,
                                         unsigned char src_slot,
                                         unsigned char src_port)
{
    /* (slot, port) need to be transfered to (dev, asic_port) first */

    int ret = RET_SUCCESS;


    /* Set (xcat port) --> (slot0 dx804 port26) */
    ret = nam_pvlan_disable_port_pve(0, src_port, (2*dest_slot + 1), 26);
    syslog_ax_board_dbg("(xcat port%d) --> (slot%d dx804 port26) --> CPU\n", src_port, dest_slot);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("Set (xcat port%d) --> (slot%d dx804 port26) fail, return\n", src_port, dest_slot);
        return RET_FAIL;
    }

    return ret;
}

int npd_create_promiscuous_port(unsigned char dest_slot,
                                         unsigned char src_slot,
                                         unsigned char src_port)
{
    int ret = RET_SUCCESS;

    /* Set Asic PVE enable */
    ret = nam_pvlan_set_pve_enable(0, 1);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("Set Asic PVE enable fail, return\n");
        return RET_FAIL;
    }

    /* Set (xcat port) --> (slot0 dx804 port26) */
    ret = nam_pvlan_set_port_pve(0, src_port, (2*dest_slot + 1), 26);
    syslog_ax_board_dbg("(xcat port%d) --> (slot%d dx804 port26) --> CPU\n", src_port, dest_slot);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("Set (xcat port%d) --> (slot%d dx804 port26) fail, return\n", src_port, dest_slot);
        return RET_FAIL;
    }

    return ret;
}

/**********************************************************************
 * srcid_egress_filter_table_set
 *
 *  DESCRIPTION:
 *          Set Source ID Egress Filter Table on each Device.
 *
 *  INPUT:
 *          None
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          RET_SUCCESS
 *          RET_FAIL
 *
 *  COMMENTS:
 *          None
 *
 **********************************************************************/
int srcid_egress_filter_table_set(void)
{
    return nam_distribute_srcid_egress_filter_table_set(0);
}


/**********************************************************************
 * npd_init_ax71_2x12g12s
 *
 *  DESCRIPTION:
 *          Config for AX71_2X12G12S.
 *
 *  INPUT:
 *          None
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          RET_SUCCESS
 *          RET_FAIL
 *
 *  COMMENTS:
 *          Called only by npd_init_board()
 *
 **********************************************************************/
int npd_init_ax71_2x12g12s(void)
{
    int ret = RET_FAIL;
    int devNum = 0;
	int mtuIndex = 0;
	int product_no = 0;

    /* change the cscd port 26,27 of XCAT to 10G mode on AX8610 */
	if(PRODUCT_TYPE != AUTELAN_PRODUCT_AX7605I)    /* force XCAT port 26 27 to 10G mode on AX81 */
	{
   		ret = force_to_xg_mode();
        if (0 != ret)
        {
            syslog_ax_board_dbg("force port to XG mode on AX71_2X12G12S error !\n");
        }
       	else
		{
	        syslog_ax_board_dbg("force port to XG mode on AX71_2X12G12S OK!\n");
		} 
	}
    ret = ax71_2x12g12s_port_config();
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("npd_init_ax71_2x12g12s fail, return\n");
        return RET_FAIL;
    }

	/* set mtu of port24,25,26,27 on xcat, to 1536 */	
	ret = nam_set_ethport_mru(devNum,24,EXTERNAL_MTU);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("nam set mtu fail, return %d\n",ret);
        return RET_FAIL;
    }
	ret = nam_set_ethport_mru(devNum,25,EXTERNAL_MTU);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("nam set mtu fail, return %d\n",ret);
        return RET_FAIL;
    }	
	ret = nam_set_ethport_mru(devNum,26,INTERNAL_MTU);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("nam set mtu fail, return %d\n",ret);
        return RET_FAIL;
    }
	ret = nam_set_ethport_mru(devNum,27,INTERNAL_MTU);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("nam set mtu fail, return %d\n",ret);
        return RET_FAIL;
    }	

	/* set vlan mtu of XCAT, VLAN MRU Profile Configuration Reg-ister */
    for(mtuIndex = 0; mtuIndex < 8; mtuIndex++)
    {
        ret = nam_config_vlan_mtu(devNum,mtuIndex,INTERNAL_MTU);
        if (RET_SUCCESS != ret)
        {
            syslog_ax_board_err("nam set vlan mtu fail, return %d\n",ret);
            return RET_FAIL;
        }
    }
	
    ret = phy_QT2225_firmware_load(24);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("phy_QT2225_firmware_load(24) fail, return\n");
        return RET_FAIL;
    }
    ret = phy_QT2225_firmware_load(25);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("phy_QT2225_firmware_load(25) fail, return\n");
        return RET_FAIL;
    }

    /* 
        * It makes the port stop sending packets out.
        * Need more comprehension about SrcID_Egress_Filter_Table 
        */
    #if 0 
    ret = srcid_egress_filter_table_set();
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("srcid_egress_filter_table_set fail, return\n");
        return RET_FAIL;
    }
    #endif    
    return RET_SUCCESS;
}



#ifdef __cplusplus
}
#endif 
