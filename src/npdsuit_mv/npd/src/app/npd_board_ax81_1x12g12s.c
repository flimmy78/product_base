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
* npd_init_ax81_1x12g12s.c
*
*
* CREATOR:
*		zhangdi@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for special board routine.
*
* DATE:
*		11/03/2011	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.1 $	
*******************************************************************************/
#ifdef __cplusplus
    extern "C"
    {
#endif
    
#include <stdio.h>
#include "npd_board.h"
#include "npd_log.h"
#include "npd_board_ax81_1x12g12s.h"
#include "npd_trunk.h"

#define  SIZE_16K  0x4000
#define  SIZE_8K   0x2000
extern char g_dis_dev[MAX_ASIC_NUM];

/* Add fro this board */

ASIC_PORT_ATTRIBUTE_S ax81_1x12g12s_attr_ge =
{
	ETH_ATTR_ENABLE,1536,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
	ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
	ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_NONE
};
ASIC_PORT_ATTRIBUTE_S ax81_1x12g12s_attr_xg =
{
	ETH_ATTR_ENABLE,1536,PORT_FULL_DUPLEX_E,PORT_SPEED_10000_E,	\
	ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_OFF,ETH_ATTR_OFF,	\
	ETH_ATTR_OFF,ETH_ATTR_OFF,COMBO_PHY_MEDIA_PREFER_NONE
};

struct eth_port_func_data_s ax81_1x12g12s_funcs = 
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

struct eth_port_counter_s ax81_1x12g12s_counters=
{
};

asic_eth_port_t ax81_1x12g12s_eth_ports[28]={   \
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x1,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x0,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x3,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x2,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x5,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x4,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x7,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x6,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x9,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0x8,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0xb,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0xa,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0xe,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0xf,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0xc,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0xd,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x12,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x13,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x10,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x11,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x16,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x17,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},	
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x14,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},		
	{
		ETH_GE_FIBER,0xFFFFFFFF,1536,0x0,0x0,0x15,0x0, \
		&ax81_1x12g12s_attr_ge,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,1536,0x0,0x0,0x18,0x0, \
		&ax81_1x12g12s_attr_xg,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,1536,0x0,0x0,0x19,0x0, \
		&ax81_1x12g12s_attr_xg,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,1536,0x0,0x0,0x1a,0x0, \
		&ax81_1x12g12s_attr_xg,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,1536,0x0,0x0,0x1b,0x0, \
		&ax81_1x12g12s_attr_xg,
		&ax81_1x12g12s_funcs,
		&ax81_1x12g12s_counters
	}
};

/* panle port to virtual port map array */
/* 2-1 is dev_port(0,1) */
unsigned int ax81_1x12g12s_port_mapArr[28] = {
            0x1,0x0,0x3,0x2,0x5,0x4,0x7,0x6,
    0x9,0x8,0xb,0xa,0xe,0xf,0xc,0xd,0x12,0x13,
    0x10,0x11,0x16,0x17,0x14,0x15,0x18,0x19,0x1A,0x1B
};

asic_cscd_port_t ax81_1x12g12s_cscd_ports[2]={
    {0,24},
    {0,25}
};

/* for AX81-1X12G12S have two cpu port 26 & 27, here set 27 */
asic_cpu_port_t ax81_1x12g12s_to_cpu_ports[MAX_CPU_PORT_NUM] = {
    {0,27},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},    /* connect to master CPU */
	{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}     /* connect to slave CPU */
};
int npd_ax81_1x12g12s_dev_map_table_set();
int ax81_1x12g12s_trunk_config();
unsigned int ax81_1x12g12s_test(void) 
{
	int ret = 0;
	unsigned int vid = 4000;
	unsigned int devnum = 0;   /* no use,just for printf */

	/* xcat */
	devnum = 0;
	
	vid = 1000;
    ret = create_vlan(devnum,vid);
	if(0 != ret){
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;
		}
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0x1, 0);  /* 3-1 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0x0, 0);  /* 3-2 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}		
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0x3, 0);  /* 3-3 */
    if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
#if 0 	/*0 for test 1-24 and 1 for test 1-4 */
	vid = 2000;
	ret = create_vlan(devnum,vid);
	if(0 != ret)
		{
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;
		}	
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0x2, 0);  /* 3-4 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret; 	
}	
#else   /* for test port 1-24 */
	vid = 100;
    ret = create_vlan(devnum,vid);
	if(0 != ret)
		{
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;
		}	
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0x2, 0);  /* 3-4 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
	

    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0x5, 0);  /* 3-5 */
    if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
	
	vid += 20;
    ret = create_vlan(devnum,vid);
	if(0 != ret)
		{
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;
		}	
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0x4, 0);  /* 3-6 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0x7, 0);   /* 3-7 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	

	vid += 20;
    ret = create_vlan(devnum,vid);
	if(0 != ret)
		{
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;
		}	
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0x6, 0);   /* 3-8 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0x9, 0);   /* 3-9 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	

	vid += 20;
    ret = create_vlan(devnum,vid);
	if(0 != ret)
		{
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;
		}	
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0x8, 0);   /* 3-10 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0xb, 0);   /* 3-11 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	

	vid += 20;
    ret = create_vlan(devnum,vid);
	if(0 != ret)
		{
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;
		}	
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0xa, 0);   /* 3-12 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0xe, 0);   /* 3-13 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
	vid += 20;
							     
    ret = create_vlan(devnum,vid);
	if(0 != ret)
		{
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;
		}	
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0xf, 0);   /* 3-14 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0xc, 0);   /* 3-15 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	

	vid += 20;
    ret = create_vlan(devnum,vid);
	if(0 != ret)
		{
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;
		}	
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0xd, 0);   /* 3-16 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0x12, 0);  /* 3-17 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}

	vid += 20;
    ret = create_vlan(devnum,vid);
	if(0 != ret)
		{
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;
		}	
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0x13, 0);  /* 3-18 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0x10, 0);  /* 3-19 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	

	vid += 20;
    ret = create_vlan(devnum,vid);
	if(0 != ret)
		{
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;
		}
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0x11, 0);  /* 3-20 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0x16, 0); /* 3-21 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	

	vid += 20;
    ret = create_vlan(devnum,vid);
	if(0 != ret)
		{
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;
		}
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0x17, 0);  /* 3-22 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0x14, 0);  /* 3-23 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
	
	vid = 2000;
	ret = create_vlan(devnum,vid);
	if(0 != ret){
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;
	}
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0x15, 0);  /* 3-24 */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
#endif
    syslog_ax_board_dbg("XCAT ge_xg_test ready!\n");
	return 0;
}

asic_board_t ax81_1x12g12s_board_t =
{
    .board_type = 0,
    .board_name = "ax81_1x12g12s",
    .slot_id = -1,
    .asic_type = ASIC_TYPE_CPSS,
    .capbmp = ASIC_CAPBMP,
	.runbmp = ASIC_RUNBMP,
	
    .asic_port_cnt = 28,    /* asic port on chip  */
    .asic_cscd_port_cnt = 2,
    .asic_port_cnt_visable = 24, /* asic port on panel */    
    .asic_port_start_no = 1,

    .asic_port_mapArr = NULL,
	.asic_eth_ports = ax81_1x12g12s_eth_ports,
    .asic_cscd_ports = ax81_1x12g12s_cscd_ports,
    .asic_to_cpu_ports = ax81_1x12g12s_to_cpu_ports,
    
	.id = 0xF0F0F0F0,
	.state = MODULE_STAT_RUNNING,
	.hw_version = 0x0,
	.sn = "not_init",
	.ext_slot_data = "not_init",   
	
	.func_test=NULL,
    .asic_init=NULL,
    .my_equipment_test = ax81_1x12g12s_test,
    .distributed_system_dev_map_table_set = npd_ax81_1x12g12s_dev_map_table_set,
    .board_cscd_port_trunk_config = ax81_1x12g12s_trunk_config
};


asic_cscd_port_t ax81_1x12g12s_cscd_port_slot1={0,25};
asic_cscd_port_t ax81_1x12g12s_cscd_port_slot2={0,24};


/**********************************************************************
 * ax81_1x12g12s_port_config
 *
 *  DESCRIPTION:
 *          port config for AX81_1X12G12S.
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
static int ax81_1x12g12s_port_config(void)
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
            syslog_ax_board_err("AX81_1X12G12S port %d inband autonegotiation enable fail.\n", portNum);
            return RET_FAIL;
        }
        syslog_ax_board_dbg("AX81_1X12G12S port %d inband autonegotiation enable.\n", portNum);
    }

    return RET_SUCCESS;
}

int ax81_1x12g12s_trunk_config(void)
{
	int ret = RET_FAIL;
	ret = nam_asic_trunk_ports_add(0,119,26,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_1X12G12S add port 26 to trunk 100 error !\n");
	}
	ret = nam_asic_trunk_ports_add(0,119,27,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_1X12G12S add port 27 to trunk 100 error !\n");
	}

	ret = nam_asic_trunk_load_balanc_set(0,119,4); /*set trunk load balance by mac and ip */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_1X12G12S set trunk 100 load-balance based source ip error ! \n");
	}
	return ret;
}

int npd_ax81_1x12g12s_dev_map_table_set(void)
{
	int ret = RET_FAIL;
	int i = 0;
	cscd_trunk_link_type smu_linktype,standby_linktype;

	smu_linktype.linkNum = 24;
	smu_linktype.linkType = CSCD_TRUNK_LINK_TYPE_PORT_E;
	standby_linktype.linkNum = 25;
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
	syslog_ax_board_dbg("npd ax81_1x12g12s dev map table set successfully !\n");
	return RET_SUCCESS;
}

/**********************************************************************
 * npd_init_ax81_1x12g12s
 *
 *  DESCRIPTION:
 *          Config for AX81_1X12G12S.
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
int npd_init_ax81_1x12g12s(void)
{
    int ret = RET_FAIL;
    int devNum = 0;
	int mtuIndex = 0;

	ret = force_to_xg_mode();
    if (0 != ret)
    {
        syslog_ax_board_err("force port to XG mode on AX81_1X12G12S error !\n");
    }
	
    ret = ax81_1x12g12s_port_config();
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("ax81_1x12g12s_port_config fail, return\n");
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

	if(PRODUCT_TYPE == AUTELAN_PRODUCT_AX8603)
	{
		if(SLOT_ID == 1)
		{
			ax81_1x12g12s_board_t.asic_cscd_port_cnt = 1;
			ax81_1x12g12s_board_t.asic_cscd_ports = &ax81_1x12g12s_cscd_port_slot1;
		}
		else if(SLOT_ID == 2)
		{
			ax81_1x12g12s_board_t.asic_cscd_port_cnt = 1;
			ax81_1x12g12s_board_t.asic_cscd_ports = &ax81_1x12g12s_cscd_port_slot2;
		}
	}
    return RET_SUCCESS;
}



#ifdef __cplusplus
}
#endif 
