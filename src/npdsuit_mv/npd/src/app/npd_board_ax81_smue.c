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
* npd_board_ax81_smue.c
*
*
* CREATOR:
*		houxx@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for special board routine.
*
* DATE:
*		06/26/2013	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.1 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
    
#include <stdio.h>
#include "npd_log.h"
#include "npd_board.h"
#include "npd_vlan.h"
#include "npd_board_ax81_smue.h"
#include "npd_trunk.h"

extern char g_dis_dev[MAX_ASIC_NUM];

extern asic_board_t* asic_board;

ASIC_PORT_ATTRIBUTE_S ax81_smue_attr_xg =
{
	ETH_ATTR_ENABLE,1522,PORT_FULL_DUPLEX_E,PORT_SPEED_10000_E,	\
	ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
	ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_NONE
};

struct eth_port_func_data_s ax81_smue_funcs = 
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

struct eth_port_counter_s ax81_smue_counters=
{
};

asic_cscd_port_t ax81_smue_cscd_ports[48]={
	/* lion-0 */
    {0,0},
    {0,2},		
    {0,4},
    {0,6},		
    {0,8},		
    {0,10},
		
    {0,16},		
    {0,18},		
    {0,20},		
    {0,22},		
    {0,24},	
    {0,26},
		
    {0,32},		
    {0,34},		
    {0,36},		
    {0,38},		
    {0,40},		
    {0,42},
		
    {0,48},		
    {0,50},		
    {0,52},		
    {0,54},		
    {0,56},		
    {0,58},
		
    /* lion-1 */
    {1,0},
    {1,2},		
    {1,4},
    {1,6},		
    {1,8},		
    {1,10},
		
    {1,16},		
    {1,18},		
    {1,20},		
    {1,22},		
    {1,24},	
    {1,26},
		
    {1,32},		
    {1,34},		
    {1,36},		
    {1,38},		
    {1,40},		
    {1,42},
		
    {1,48},		
    {1,50},		
    {1,52},		
    {1,54},		
    {1,56},		
    {1,58}
};

/* I am switch board, just set to -1, to avoid Segmentation fault of npd_vlan_to_cpu_port_add */
asic_cpu_port_t ax81_smue_to_cpu_ports[MAX_CPU_PORT_NUM] = {
    {-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},    /* connect to master CPU */
	{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}     /* connect to slave CPU */
};

	
asic_eth_port_t ax81_smue_eth_ports[48]={   \
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,0,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,2,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,4,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,6,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,8,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,10,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,16,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,18,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,20,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,22,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,24,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,26,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,32,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,34,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,36,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,38,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,40,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,42,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,48,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,50,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,52,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,54,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,56,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x0,58,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,0,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,2,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,4,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,6,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,8,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,10,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,16,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,18,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,20,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,22,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,24,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,26,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,32,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,34,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,36,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,38,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,40,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,42,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,48,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,50,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,52,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,54,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,56,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	},
	{
		ETH_XGTX,0xFFFFFFFF,LION_DEFAULT_MTU,0x0,0x1,58,0x0, \
		&ax81_smue_attr_xg,
		&ax81_smue_funcs,
		&ax81_smue_counters
	}
};


unsigned int ax81_smue_port_mapArr[24] = {
    0, 2, 4, 6, 8, 10,
    16,18,20,22,24,26,
    32,34,36,38,40,42,
    48,50,52,54,56,58
};

int npd_ax81_smue_dev_map_table_set();
int ax81_smue_trunk_config();
unsigned int ax81_smue_test(void) 
{
	int ret = 0;
	unsigned int vid = 4000;
	unsigned int devnum = 0;   /* no use,just for printf */
	
#if 1	/* for AX81_AC12C test */
    syslog_ax_board_dbg("slot %d!\n", asic_board->slot_id);
    if(asic_board->slot_id == 7)
    {
    	vid = 1000;
    }
	else if(asic_board->slot_id == 8)
	{
        vid = 4000;
	}
	
    ret = create_vlan(devnum,vid);
	if(0 != ret){
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;		
	}	
	
	/* add for distributed OS, add cscd port to all vlan */
	ret = npd_vlan_cscd_add(vid);
	if (0 != ret) {
		syslog_ax_board_err("npd_vlan_add_cscd to vlan %d error !\n",vid);
		return ret;
	}

    ret = npd_enable_cscd_port();
    if (0 != ret)
    {
        syslog_ax_board_err("enable cscd port on master board error !\n");
    }
    else
    {
        syslog_ax_board_dbg("enable cscd port on master board OK !\n");
    }	
#endif

    syslog_ax_board_dbg("AX81-SMU ge_xg_test ready!\n");
	
	return ret;
}

unsigned int ax81_smue_test_for_12x(void) 
{
	int ret = 0;
	unsigned int vid = 0;
	unsigned int devnum = 0;   /* no use,just for printf */

	syslog_ax_board_dbg("slot %d!\n", asic_board->slot_id);

    /*if smue lion1 working and lion0 doesn't work*/
	if(asic_board->slot_id == 7)
    {
		vid = 2000;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 52, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 54, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		vid = 2010;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 56, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 36, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		vid = 2020;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 32, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 34, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }

		vid = 2030;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 22, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 24, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		vid = 2040;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 26, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 4, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		vid = 2050;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 2, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }

		vid = 2060;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 10, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 8, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		vid = 2070;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 6, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 16, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		vid = 2080;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 20, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 18, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }

		vid = 2090;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 42, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 40, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }

		vid = 2110;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 38, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 50, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }

	}
	else if(asic_board->slot_id == 8)
	{
		vid = 2500;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 56, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 54, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		vid = 2510;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 52, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 32, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		vid = 2520;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 34, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 36, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }

		vid = 2530;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 24, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 26, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		vid = 2540;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 22, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		vid = 2550;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 2, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 4, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }

		vid = 2560;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 6, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 8, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		vid = 2570;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 10, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 20, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		vid = 2580;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 16, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 18, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }

		vid = 2590;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 38, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 40, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }

		vid = 2610;
		ret = create_vlan(devnum,vid);
	    if(0 != ret){
		    syslog_ax_board_err("create new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 50, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }
		ret = nam_asic_vlan_entry_ports_add(devnum, vid, 42, 0);  
	    if(0 != ret){
		    syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		    return ret;		
	    }

	}

    ret = npd_enable_cscd_port();
    if (0 != ret)
    {
        syslog_ax_board_err("enable cscd port on master board error !\n");
    }
    else
    {
        syslog_ax_board_dbg("enable cscd port on master board OK !\n");
    }
    syslog_ax_board_dbg("AX81-SMU ge_xg_test ready!\n");
	
	return ret;
}

asic_board_t ax81_smue_board_t =
{
    .board_type = 0,
    .board_name = "ax81_smue",
    .slot_id = -1,   /* dynamic init in npd */
    .asic_type = ASIC_TYPE_CPSS,
    .capbmp = ASIC_CAPBMP,
	.runbmp = ASIC_RUNBMP,
	
    .asic_port_cnt = 48,  /* asic port on chip  */
    .asic_cscd_port_cnt = 48,
    .asic_port_cnt_visable = 0,  /* asic port on panel */
    .asic_port_start_no = 1,
    
    .asic_port_mapArr = NULL,
	.asic_eth_ports = ax81_smue_eth_ports,
    .asic_cscd_ports = ax81_smue_cscd_ports,
    .asic_to_cpu_ports = ax81_smue_to_cpu_ports,

	.id = 0xF0F0F0F0,
	.state = MODULE_STAT_RUNNING,
	.hw_version = 0x0,
	.sn = "not_init",
	.ext_slot_data = "not_init",   
	
	.func_test=NULL,
    .asic_init=NULL,
    .my_equipment_test = ax81_smue_test_for_12x,
    .distributed_system_dev_map_table_set = npd_ax81_smue_dev_map_table_set,
    .board_cscd_port_trunk_config = ax81_smue_trunk_config
    
};


/**********************************************************************
 *  ax81_smue_trunk_port_status_init
 *
 *  DESCRIPTION:
 *         config SMU  trunk groups and init the port which belong to the trunk status
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
 *
 **********************************************************************/

int ax81_smue_trunk_port_status_init_for_lion0(unsigned char devNum)
{
	unsigned int ret = RET_FAIL;

	unsigned long attr = 0;
	syslog_ax_board_dbg(" devNum is %d  \n",devNum);
	sleep(1);  /*add by zhangcl@autelan.com 01/31/2012  --for xcat board  because G-port force to    XG-port need some time when read the port status register*/
	/*add port 0,2,4 to trunk 118 */
	ret = nam_get_port_link_state(devNum,0,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 0 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 0 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,0,118,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 0 to trunk 118 error !\n");
	}
	ret = nam_get_port_link_state(devNum,2,&attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 2 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 2 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,2,118,attr); 
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 2 to trunk 118 error !\n");
	}
	ret = nam_get_port_link_state(devNum,4,&attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 4 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 4 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,4,118,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 4 to trunk 118 error !\n");
	}

	/*add port 22,24,26 to trunk 117 */
	ret = nam_get_port_link_state(devNum,22,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 22 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 22 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,22,117,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 22 to trunk 117 error !\n");
	}
	ret = nam_get_port_link_state(devNum,24,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 24 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 24 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,24,117,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 24 to trunk 117 error !\n");
	}
	ret = nam_get_port_link_state(devNum,26,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 26 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 26 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,26,117,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 26 to trunk 117 error !\n");
	}



	/*add port 32,34,36 to trunk 116 */
	ret = nam_get_port_link_state(devNum,32,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 32 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 32 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,32,116,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 32 to trunk 116 error !\n");
	}
	ret = nam_get_port_link_state(devNum,34,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 34 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 34 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,34,116,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 34 to trunk 116 error !\n");
	}
	ret = nam_get_port_link_state(devNum,36,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 36 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 36 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,36,116,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 36 to trunk 116 error !\n");
	}


	/*add port 54,56,58 to trunk 115 */
	ret = nam_get_port_link_state(devNum,54,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 54 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 54 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,54,115,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 54 to trunk 115 error !\n");
	}
	ret = nam_get_port_link_state(devNum,56,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 56 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 56 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,56,115,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 56 to trunk 115 error !\n");
	}
	ret = nam_get_port_link_state(devNum,58,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 58 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 58 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,58,115,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 58 to trunk 115 error !\n");
	}



	/*add port 48,50,52 to trunk 114 */
	ret = nam_get_port_link_state(devNum,48,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 48 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 48 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,48,114,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 48 to trunk 114 error !\n");
	}
	ret = nam_get_port_link_state(devNum,50,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 50 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 50 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,50,114,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 50 to trunk 114 error !\n");
	}
	ret = nam_get_port_link_state(devNum,52,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 52 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 52 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,52,114,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 52 to trunk 114 error !\n");
	}


	/*add port 38,40,42 to trunk 113 */
	ret = nam_get_port_link_state(devNum,38,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 38 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 38 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,38,113,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 38 to trunk 113 error !\n");
	}
	ret = nam_get_port_link_state(devNum,40,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 40 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 40 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,40,113,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 40 to trunk 113 error !\n");
	}
	ret = nam_get_port_link_state(devNum,42,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 42 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 42 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,42,113,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 42 to trunk 113 error !\n");
	}


	/*add port 16,18,20 to trunk 112 */
	ret = nam_get_port_link_state(devNum,16,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 16 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 16 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,16,112,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 16 to trunk 112 error !\n");
	}
	ret = nam_get_port_link_state(devNum,18,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 18 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 18 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,18,112,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 18 to trunk 112 error !\n");
	}
	ret = nam_get_port_link_state(devNum,20,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 20 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 20 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,20,112,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 20 to trunk 112 error !\n");
	}


	/*add port 6,8,10 to trunk 111 */
	ret = nam_get_port_link_state(devNum,6,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 6 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 6 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,6,111,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 6 to trunk 111 error !\n");
	}
	ret = nam_get_port_link_state(devNum,8,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 8 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 8 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,8,111,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 8 to trunk 111 error !\n");
	}
	ret = nam_get_port_link_state(devNum,10,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 10 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 10 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,10,111,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 10 to trunk 111 error !\n");
	}
	return ret;
}




/**********************************************************************
 *  ax81_smue_trunk_port_status_init
 *
 *  DESCRIPTION:
 *         config SMU  trunk groups and init the port which belong to the trunk status
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
 *
 **********************************************************************/

int ax81_smue_trunk_port_status_init_for_8800_lion0(unsigned char devNum)
{
	unsigned int ret = RET_FAIL;

	unsigned long attr = 0;
	syslog_ax_board_dbg(" devNum is %d  \n",devNum);
	sleep(1);  /*add by zhangcl@autelan.com 01/31/2012  --for xcat board  because G-port force to    XG-port need some time when read the port status register*/
	/*add port 0,2 to trunk 118 for slot1*/
	ret = nam_get_port_link_state(devNum,0,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 0 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 0 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,0,118,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 0 to trunk 118 error !\n");
	}
	ret = nam_get_port_link_state(devNum,2,&attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 2 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 2 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,2,118,attr); 
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 2 to trunk 118 error !\n");
	}

	/*add port 8,10 to trunk 117 for slot2*/
	ret = nam_get_port_link_state(devNum,8,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 8 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 8 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,8,117,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 8 to trunk 117 error !\n");
	}
	ret = nam_get_port_link_state(devNum,10,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 10 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 10 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,10,117,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 10 to trunk 117 error !\n");
	}

	/*add port 20,22 to trunk 116 for slot3*/
	ret = nam_get_port_link_state(devNum,20,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 20 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 20 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,20,116,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 20 to trunk 116 error !\n");
	}
	ret = nam_get_port_link_state(devNum,22,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 22 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 22 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,22,116,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 22 to trunk 116 error !\n");
	}

	/*add port 32,34 to trunk 115 for slot 4*/
	ret = nam_get_port_link_state(devNum,32,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 32 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 32 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,32,115,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 32 to trunk 115 error !\n");
	}
	ret = nam_get_port_link_state(devNum,34,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 34 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 34 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,34,115,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 34 to trunk 115 error !\n");
	}

	/*add port 40,42 to trunk 114 for slot 5*/
	ret = nam_get_port_link_state(devNum,40,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 40 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 40 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,40,114,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 40 to trunk 114 error !\n");
	}
	ret = nam_get_port_link_state(devNum,42,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 42 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 42 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,42,114,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 42 to trunk 114 error !\n");
	}

	/*add port  to trunk 113 for slot 6*/
	ret = nam_get_port_link_state(devNum,52,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 52 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 52 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,52,113,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 52 to trunk 113 error !\n");
	}
	ret = nam_get_port_link_state(devNum,54,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 54 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 54 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,54,113,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 54 to trunk 113 error !\n");
	}

	/*add port 48,50 to trunk 112 for slot 9*/
	ret = nam_get_port_link_state(devNum,48,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 48 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 48 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,48,112,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 48 to trunk 112 error !\n");
	}
	ret = nam_get_port_link_state(devNum,50,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 50 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 50 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,50,112,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 50 to trunk 112 error !\n");
	}

	/*add port 56,58 to trunk 111 for slot 10*/
	ret = nam_get_port_link_state(devNum,56,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 56 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 56 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,56,111,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 56 to trunk 111 error !\n");
	}
	ret = nam_get_port_link_state(devNum,58,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 58 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 58 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,58,111,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 58 to trunk 111 error !\n");
	}

	/*add port 32,38 to trunk 110 for slot 11*/
	ret = nam_get_port_link_state(devNum,36,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 36 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 36 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,36,110,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 36 to trunk 110 error !\n");
	}
	ret = nam_get_port_link_state(devNum,38,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 38 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 38 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,38,110,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 38 to trunk 110 error !\n");
	}

	/*add port 16,18 to trunk 109 for slot 12*/
	ret = nam_get_port_link_state(devNum,16,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 16 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 16 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,16,109,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 16 to trunk 109 error !\n");
	}
	ret = nam_get_port_link_state(devNum,18,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 18 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 18 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,18,109,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 18 to trunk 109 error !\n");
	}

	/*add port 24,26 to trunk 108 for slot 13*/
	ret = nam_get_port_link_state(devNum,24,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 24 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 24 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,24,108,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 24 to trunk 108 error !\n");
	}
	ret = nam_get_port_link_state(devNum,26,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 26 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 26 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,26,108,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 26 to trunk 108 error !\n");
	}

	/*add port 4,6 to trunk 107 for slot 14*/
	ret = nam_get_port_link_state(devNum,4,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 4 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 4 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,4,107,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 4 to trunk 107 error !\n");
	}
	ret = nam_get_port_link_state(devNum,6,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 6 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 6 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,6,107,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 6 to trunk 107 error !\n");
	}
	return ret;
}






/**********************************************************************
 *  ax81_smue_trunk_port_status_init
 *
 *  DESCRIPTION:
 *         config SMU  trunk groups and init the port which belong to the trunk status
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
 *
 **********************************************************************/

int ax81_smue_trunk_port_status_init_for_8800_lion1(unsigned char devNum)
{
	unsigned int ret = RET_FAIL;

	unsigned long attr = 0;
	syslog_ax_board_dbg(" devNum is %d  \n",devNum);
	sleep(1);  /*add by zhangcl@autelan.com 01/31/2012  --for xcat board  because G-port force to    XG-port need some time when read the port status register*/
	/*add port 50,52 to trunk 118 for slot1*/
	ret = nam_get_port_link_state(devNum,50,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 50 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 50 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,50,118,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 50 to trunk 118 error !\n");
	}
	ret = nam_get_port_link_state(devNum,52,&attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 52 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 52 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,52,118,attr); 
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 52 to trunk 118 error !\n");
	}

	/*add port 40,42 to trunk 117 for slot2*/
	ret = nam_get_port_link_state(devNum,40,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 40 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 40 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,40,117,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 40 to trunk 117 error !\n");
	}
	ret = nam_get_port_link_state(devNum,42,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 42 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 42 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,42,117,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 42 to trunk 117 error !\n");
	}

	/*add port 32,34 to trunk 116 for slot  3*/
	ret = nam_get_port_link_state(devNum,32,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 32 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 32 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,32,116,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 32 to trunk 116 error !\n");
	}
	ret = nam_get_port_link_state(devNum,34,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 34 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 34 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,34,116,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 34 to trunk 116 error !\n");
	}

	/*add port 22,20 to trunk 115 for slot 4*/
	ret = nam_get_port_link_state(devNum,20,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 20 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 20 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,20,115,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 20 to trunk 115 error !\n");
	}
	ret = nam_get_port_link_state(devNum,22,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 22 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 22 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,22,115,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 22 to trunk 115 error !\n");
	}

	/*add port 8,10  to trunk 114 for slot 5*/
	ret = nam_get_port_link_state(devNum,8,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 8 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 8 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,8,114,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 8 to trunk 114 error !\n");
	}
	ret = nam_get_port_link_state(devNum,10,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 10 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 10 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,10,114,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 10 to trunk 114 error !\n");
	}

	/*add port  0,2 to trunk 113 for slot 6*/
	ret = nam_get_port_link_state(devNum,0,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 0 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 0 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,0,113,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 0 to trunk 113 error !\n");
	}
	ret = nam_get_port_link_state(devNum,2,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 2 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 2 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,2,113,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 2 to trunk 113 error !\n");
	}

	/*add port 4,6  to trunk 112 for slot 9*/
	ret = nam_get_port_link_state(devNum,4,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 4 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 4 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,4,112,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 4 to trunk 112 error !\n");
	}
	ret = nam_get_port_link_state(devNum,6,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 6 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 6 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,6,112,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 6 to trunk 112 error !\n");
	}

	/*add port  24,26 to trunk 111 for slot 10*/
	ret = nam_get_port_link_state(devNum,24,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 24 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 24 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,24,111,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 24 to trunk 111 error !\n");
	}
	ret = nam_get_port_link_state(devNum,26,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 26 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 26 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,26,111,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 26 to trunk 111 error !\n");
	}

	/*add port  16,18 to trunk 110 for slot 11*/
	ret = nam_get_port_link_state(devNum,16,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 16 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 16 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,16,110,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 16 to trunk 110 error !\n");
	}
	ret = nam_get_port_link_state(devNum,18,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 18 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 18 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,18,110,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 18 to trunk 110 error !\n");
	}

	/*add port 36,38  to trunk 109 for slot 12*/
	ret = nam_get_port_link_state(devNum,36,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 36 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 36 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,36,109,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 36 to trunk 109 error !\n");
	}
	ret = nam_get_port_link_state(devNum,38,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 38 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 38 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,38,109,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 38 to trunk 109 error !\n");
	}

	/*add port 54,56  to trunk 108 for slot 13*/
	ret = nam_get_port_link_state(devNum,54,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 54 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 54 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,54,108,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 54 to trunk 108 error !\n");
	}
	ret = nam_get_port_link_state(devNum,56,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 56 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 56 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,56,108,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 56 to trunk 108 error !\n");
	}

	/*add port  48 to trunk 107 for slot 14*/
	ret = nam_get_port_link_state(devNum,48,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 48 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 48 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,48,107,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 4 to trunk 107 error !\n");
	}
	return ret;
}

int ax81_smue_trunk_port_status_init_for_8606(unsigned char devNum)
{
	unsigned int ret = RET_FAIL;

	unsigned long attr = 0;
	syslog_ax_board_dbg(" devNum is %d  \n",devNum);
	sleep(1);  /*add by zhangcl@autelan.com 01/31/2012  --for xcat board  because G-port force to    XG-port need some time when read the port status register*/

    /*4x and 12x only to 8606 SUM lion0*/
	/*add port 0,2,4,22,24,26 to trunk 118 to slot1*/	
	ret = nam_get_port_link_state(devNum,0,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 0 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 0 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,0,118,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 0 to trunk 118 error !\n");
	}
	ret = nam_get_port_link_state(devNum,2,&attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 2 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 2 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,2,118,attr); 
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 2 to trunk 118 error !\n");
	}
	ret = nam_get_port_link_state(devNum,4,&attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 4 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 4 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,4,118,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 4 to trunk 118 error !\n");
	}
	ret = nam_get_port_link_state(devNum,22,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("get dev %d port 22 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 22 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,22,118,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 22 to trunk 118 error !\n");
	}
	ret = nam_get_port_link_state(devNum,24,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 24 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 24 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,24,118,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 24 to trunk 118 error !\n");
	}
	ret = nam_get_port_link_state(devNum,26,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 26 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 26 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,26,118,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 26 to trunk 118 error !\n");
	}



	/*add port 32,34,36,54,56,58 to trunk 117 to slot2*/
	ret = nam_get_port_link_state(devNum,32,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 32 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 32 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,32,117,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 32 to trunk 117 error !\n");
	}
	ret = nam_get_port_link_state(devNum,34,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 34 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 34 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,34,117,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 34 to trunk 117 error !\n");
	}
	ret = nam_get_port_link_state(devNum,36,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 36 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 36 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,36,117,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 36 to trunk 117 error !\n");
	}
	ret = nam_get_port_link_state(devNum,54,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 54 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 54 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,54,117,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 54 to trunk 117 error !\n");
	}
	ret = nam_get_port_link_state(devNum,56,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 56 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 56 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,56,117,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 56 to trunk 117 error !\n");
	}
	ret = nam_get_port_link_state(devNum,58,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 58 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 58 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,58,117,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 58 to trunk 117 error !\n");
	}



	/*add port 38,40,42,48,50,52 to trunk 116 to slot5*/
	ret = nam_get_port_link_state(devNum,48,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 48 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 48 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,48,116,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 48 to trunk 116 error !\n");
	}
	ret = nam_get_port_link_state(devNum,50,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 50 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 50 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,50,116,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 50 to trunk 116 error !\n");
	}
	ret = nam_get_port_link_state(devNum,52,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 52 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 52 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,52,116,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 52 to trunk 116 error !\n");
	}

	ret = nam_get_port_link_state(devNum,38,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 38 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 38 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,38,116,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 38 to trunk 116 error !\n");
	}
	ret = nam_get_port_link_state(devNum,40,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 40 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 40 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,40,116,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 40 to trunk 116 error !\n");
	}
	ret = nam_get_port_link_state(devNum,42,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 42 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 42 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,42,116,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 42 to trunk 116 error !\n");
	}


	/*add port 6,8,10,16,18,20 to trunk 115 to slot6*/
	ret = nam_get_port_link_state(devNum,16,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 16 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 16 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,16,115,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 16 to trunk 115 error !\n");
	}
	ret = nam_get_port_link_state(devNum,18,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 18 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 18 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,18,115,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 18 to trunk 115 error !\n");
	}
	ret = nam_get_port_link_state(devNum,20,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 20 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 20 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,20,115,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 20 to trunk 115 error !\n");
	}


	ret = nam_get_port_link_state(devNum,6,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 6 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 6 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,6,115,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 6 to trunk 115 error !\n");
	}
	ret = nam_get_port_link_state(devNum,8,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 8 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 8 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,8,115,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 8 to trunk 115 error !\n");
	}
	ret = nam_get_port_link_state(devNum,10,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 10 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 10 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,10,115,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 10 to trunk 115 error !\n");
	}
	return ret;
}


int ax81_smue_trunk_port_status_init_for_lion1(unsigned char devNum)
{
	unsigned int ret = RET_FAIL;
	unsigned long attr = 0;
	
	syslog_ax_board_dbg(" devNum is %d  \n",devNum);

	/*add port 52,54,56 to trunk 110 */
	ret = nam_get_port_link_state(devNum,52,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 52 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 52 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,52,118,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 52 to trunk 110 error !\n");
	}
	ret = nam_get_port_link_state(devNum,54,&attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 54 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 54 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,54,118,attr); 
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 54 to trunk 110 error !\n");
	}
	ret = nam_get_port_link_state(devNum,56,&attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 56 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 56 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,56,118,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 56 to trunk 110 error !\n");
	}





	/*add port 32,34,36 to trunk 109 */
	ret = nam_get_port_link_state(devNum,32,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 32 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 32 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,32,117,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 32 to trunk 109 error !\n");
	}
	ret = nam_get_port_link_state(devNum,34,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 34 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 34 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,34,117,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 34 to trunk 109 error !\n");
	}
	ret = nam_get_port_link_state(devNum,36,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 36 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 36 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,36,117,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 26 to trunk 109 error !\n");
	}





	/*add port 22,24,26 to trunk 108 */
	ret = nam_get_port_link_state(devNum,22,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 22 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 22 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,22,116,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 22 to trunk 108 error !\n");
	}
	ret = nam_get_port_link_state(devNum,24,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 24 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 24 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,24,116,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 24 to trunk 108 error !\n");
	}
	ret = nam_get_port_link_state(devNum,26,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 26 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 26 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,26,116,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 26 to trunk 108 error !\n");
	}


	/*add port 0,2,4 to trunk 107 */
	ret = nam_get_port_link_state(devNum,0,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 0 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 0 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,0,115,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 2 to trunk 107 error !\n");
	}
	ret = nam_get_port_link_state(devNum,2,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 2 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 2 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,2,115,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 2 to trunk 107 error !\n");
	}
	ret = nam_get_port_link_state(devNum,4,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 4 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 4 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,4,115,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 4to trunk 107 error !\n");
	}



	/*add port 6,8,10 to trunk 106 */
	ret = nam_get_port_link_state(devNum,6,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 6 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 6 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,6,114,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 6 to trunk 106 error !\n");
	}
	ret = nam_get_port_link_state(devNum,8,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 8 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 8 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,8,114,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 8 to trunk 106 error !\n");
	}
	ret = nam_get_port_link_state(devNum,10,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 10 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 10 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,10,114,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 10 to trunk 106 error !\n");
	}


	/*add port 16,18,20 to trunk 105*/
	ret = nam_get_port_link_state(devNum,16,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 16 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 16 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,16,113,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 16 to trunk 105 error !\n");
	}
	ret = nam_get_port_link_state(devNum,18,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 18 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 18 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,18,113,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 18 to trunk 105 error !\n");
	}
	ret = nam_get_port_link_state(devNum,20,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 20 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 20 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,20,113,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 20 to trunk 105 error !\n");
	}


	/*add port 38,40,42 to trunk 104 */
	ret = nam_get_port_link_state(devNum,38,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 38 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 38 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,38,112,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 38 to trunk 104 error !\n");
	}
	ret = nam_get_port_link_state(devNum,40,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 40 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 40 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,40,112,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 40 to trunk 104 error !\n");
	}
	ret = nam_get_port_link_state(devNum,42,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 42 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 42 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,42,112,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 42 to trunk 104 error !\n");
	}


	/*add port 48,50 to trunk 103 */
	ret = nam_get_port_link_state(devNum,48,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 48 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 48 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,48,111,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 48 to trunk 103 error !\n");
	}
	ret = nam_get_port_link_state(devNum,50,&attr); /*get port linkstatus*/
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("get dev %d port 50 linkstatus err !\n",devNum);
		return ret;
	}
	syslog_ax_board_dbg(" port 50 linkstatus is %d \n",attr);
	ret = nam_asic_trunk_port_endis(devNum,50,111,attr);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 50 to trunk 103 error !\n");
	}
	return ret;
}


/**********************************************************************
 *  ax81_smue_trunk_config
 *
 *  DESCRIPTION:
 *         config SMU back port to  trunk groups
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
 *
 **********************************************************************/


static int ax81_smue_trunk_config_for_lion0(unsigned char devNum)
{
	unsigned int ret = RET_FAIL;

	unsigned long attr = 0,member = 0;
	int i=0,j=16,m=32,n=48;
	unsigned short trunkId = 0;
	
	/*add port 0,2,4 to trunk 118 */
	
	ret = nam_asic_trunk_ports_add(devNum,118,0,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 0 to trunk 118 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,118,2,1); 
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 2 to trunk 118 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,118,4,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 4 to trunk 118 error !\n");
	}

	ret = nam_asic_trunk_load_balanc_set(devNum,118,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 118 load-balance based dest mac error ! \n");
	}



	/*add port 22,24,26 to trunk 117 */
	ret = nam_asic_trunk_ports_add(devNum,117,22,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 22 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,117,24,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 24 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,117,26,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 26 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,117,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 117 load-balance based dest mac error ! \n");
	}




	/*add port 32,34,36 to trunk 116 */
	ret = nam_asic_trunk_ports_add(devNum,116,32,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 32 to trunk 116 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,116,34,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 34 to trunk 116 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,116,36,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 36 to trunk 116 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,116,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 116 load-balance based dest mac error ! \n");
	}

	/*add port 54,56,58 to trunk 115 */
	ret = nam_asic_trunk_ports_add(devNum,115,54,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 54 to trunk 115 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,115,56,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 56 to trunk 115 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,115,58,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 58 to trunk 115 error !\n");
	}

	ret = nam_asic_trunk_load_balanc_set(devNum,115,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 115 load-balance based dest mac error ! \n");
	}

	/*add port 48,50,52 to trunk 114 */
	ret = nam_asic_trunk_ports_add(devNum,114,48,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 48 to trunk 114 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,114,50,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 50 to trunk 114 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,114,52,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 52 to trunk 114 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,114,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 114 load-balance based dest mac error ! \n");
	}

	/*add port 38,40,42 to trunk 113 */
	ret = nam_asic_trunk_ports_add(devNum,113,38,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 38 to trunk 113 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,113,40,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 40 to trunk 113 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,113,42,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 42 to trunk 113 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,113,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 113 load-balance based dest mac error ! \n");
	}

	/*add port 16,18,20 to trunk 112 */
	ret = nam_asic_trunk_ports_add(devNum,112,16,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 16 to trunk 112 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,112,18,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 18 to trunk 112 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,112,20,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 20 to trunk 112 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,112,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 112 load-balance based dest mac error ! \n");
	}

	/*add port 6,8,10 to trunk 111 */
	ret = nam_asic_trunk_ports_add(devNum,111,6,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 6 to trunk 111 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,111,8,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 8 to trunk 111 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,111,10,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 10 to trunk 111 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,111,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 111 load-balance based dest mac error ! \n");
	}

	return ret;
}


int npd_ax81_smue_dev_map_table_set(void)
{
	int ret = RET_FAIL;
	int i = 0,j=0;
	cscd_trunk_link_type slot_linktype;
	cscd_trunk_link_hash_mode trunk_hash_mode = CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E;
	cscd_trunk_link_type_mode linktype_mode_trunk = CSCD_TRUNK_LINK_TYPE_TRUNK_E;
	slot_linktype.linkType = linktype_mode_trunk;
	int trunkId[]={118,117,116,115,114,113,112,111};

	
	switch(PRODUCT_TYPE)
	{
		case AUTELAN_PRODUCT_AX8610:
			if(SLOT_ID == 5)
			{
				ret = nam_asic_redistribute_devnum(0,(SLOT_ID*2 - 2));
				if(ret != RET_SUCCESS)
				{
					syslog_ax_board_err("set distributed devnum FAIL !\n");
					return ret;
				}

				ret = nam_asic_redistribute_devnum(1,(SLOT_ID*2));
				if(ret != RET_SUCCESS)
				{
					syslog_ax_board_err("set distributed devnum FAIL !\n");
					return ret;
				}
				g_dis_dev[0] = SLOT_ID*2 - 2;
				g_dis_dev[1] = SLOT_ID*2;
			}
			else if(SLOT_ID == 6)
			{
				ret = nam_asic_redistribute_devnum(0,(SLOT_ID*2));
				if(ret != RET_SUCCESS)
				{
					syslog_ax_board_err("set distributed devnum FAIL !\n");
					return ret;
				}
				ret = nam_asic_redistribute_devnum(1,(SLOT_ID*2 + 2));
				if(ret != RET_SUCCESS)
				{
					syslog_ax_board_err("set distributed devnum FAIL !\n");
					return ret;
				}
				g_dis_dev[0] = SLOT_ID*2;
				g_dis_dev[1] = SLOT_ID*2 + 2;
			}
			syslog_ax_board_dbg("dev Map table is ########################\n");
			syslog_ax_board_dbg("dev  trunk \n");
			/*i  is devnum j is trunk ID*/
			for(i=0;i<=22;i=i+2)
			{
				if((i > 7) && (i < 16))
				{
					continue;
				}
				slot_linktype.linkNum = trunkId[j];
				ret = nam_cscd_dev_map_set(0,i,0,&slot_linktype,trunk_hash_mode);/*for lion0*/
				if(ret != RET_SUCCESS)
				{
					syslog_ax_board_err("set dev map table FAIL !\n");
					return ret;
				}
				ret = nam_cscd_dev_map_set(1,i,0,&slot_linktype,trunk_hash_mode);/*for lion1 */
				if(ret != RET_SUCCESS)
				{
					syslog_ax_board_err("set dev map table FAIL !\n");
					return ret;
				}
				syslog_ax_board_dbg("%d, %d\n",i,trunkId[j]);
				j++;
			}
			syslog_ax_board_dbg("end of dev Map ########################\n");
			break;
		default:
			syslog_ax_board_warning("unsupport PRODUCT TYPE !!!\n");
			break;
	}
	npd_syslog_info("npd ax81_smue dev map table set successfully !\n");
	return RET_SUCCESS;
}




static int ax81_smue_trunk_config_for_8606(unsigned char devNum)
{
	unsigned int ret = RET_FAIL;

	unsigned long attr = 0,member = 0;
	unsigned short trunkId = 0;
	
	/*add port 0,2,4,22,24,26 to trunk 118 */
	
	ret = nam_asic_trunk_ports_add(devNum,118,0,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 0 to trunk 118 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,118,2,1); 
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 2 to trunk 118 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,118,4,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 4 to trunk 118 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,118,22,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 22 to trunk 118 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,118,24,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 24 to trunk 118 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,118,26,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 26 to trunk 118 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,118,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 118 load-balance based dest mac error ! \n");
	}




	/*add port 32,34,36,54,56,58 to trunk 117 */
	ret = nam_asic_trunk_ports_add(devNum,117,32,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 32 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,117,34,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 34 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,117,36,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 36 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,117,54,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 54 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,117,56,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 56 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,117,58,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 58 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,117,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 117 load-balance based dest mac error ! \n");
	}

	/*add port 38,40,42,48,50,52 to trunk 114 */
	ret = nam_asic_trunk_ports_add(devNum,116,38,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 38 to trunk 116 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,116,40,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 40 to trunk 116 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,116,42,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 42 to trunk 116 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,116,48,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 48 to trunk 116 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,116,50,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 50 to trunk 116 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,116,52,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 52 to trunk 116 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,116,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 116 load-balance based dest mac error ! \n");
	}

	/*add port 6,8,10,16,18,20 to trunk 115 */
	ret = nam_asic_trunk_ports_add(devNum,115,6,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 6 to trunk 115 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,115,8,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 8 to trunk 115 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,115,10,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 10 to trunk 115 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,115,16,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 16 to trunk 115 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,115,18,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 18 to trunk 115 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,115,20,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 20 to trunk 115 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,115,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 115 load-balance based dest mac error ! \n");
	}


	return ret;
}


/**********************************************************************
 *  ax81_smue_trunk_config
 *
 *  DESCRIPTION:
 *         config SMU back port to  trunk groups
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
 *
 **********************************************************************/


static int ax81_smue_trunk_config_for_8800_lion0(unsigned char devNum)
{
	unsigned int ret = RET_FAIL;
	unsigned short trunkId = 0;
	
	/*add port 0,2 to trunk 118 */
	
	ret = nam_asic_trunk_ports_add(devNum,118,0,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 0 to trunk 118 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,118,2,1); 
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 2 to trunk 118 error !\n");
	}

	ret = nam_asic_trunk_load_balanc_set(devNum,118,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 118 load-balance based dest mac error ! \n");
	}



	/*add port 8,10 to trunk 117 */
	ret = nam_asic_trunk_ports_add(devNum,117,8,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 8 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,117,10,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 10 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,117,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 117 load-balance based dest mac error ! \n");
	}




	/*add port 20,22 to trunk 116 */
	ret = nam_asic_trunk_ports_add(devNum,116,20,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 4 to trunk 116 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,116,22,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 6 to trunk 116 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,116,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 116 load-balance based dest mac error ! \n");
	}

	/*add port 32,34 to trunk 115 */
	ret = nam_asic_trunk_ports_add(devNum,115,32,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 32 to trunk 115 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,115,34,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 34 to trunk 115 error !\n");
	}

	ret = nam_asic_trunk_load_balanc_set(devNum,115,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 115 load-balance based dest mac error ! \n");
	}

	/*add port 40,42 to trunk 114 */
	ret = nam_asic_trunk_ports_add(devNum,114,40,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 40 to trunk 114 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,114,42,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 42 to trunk 114 error !\n");
	}

	ret = nam_asic_trunk_load_balanc_set(devNum,114,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 114 load-balance based dest mac error ! \n");
	}

	/*add port 52,54 to trunk 113 */
	ret = nam_asic_trunk_ports_add(devNum,113,52,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 52 to trunk 113 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,113,54,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 54 to trunk 113 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,113,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 113 load-balance based dest mac error ! \n");
	}

	/*add port 48,50 to trunk 112 */
	ret = nam_asic_trunk_ports_add(devNum,112,48,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 48 to trunk 112 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,112,50,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 50 to trunk 112 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,112,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 112 load-balance based dest mac error ! \n");
	}

	/*add port 56,58 to trunk 111 */
	ret = nam_asic_trunk_ports_add(devNum,111,56,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 56 to trunk 111 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,111,58,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 58 to trunk 111 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,111,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 111 load-balance based dest mac error ! \n");
	}
	/*add port 36,38 to trunk 110 */
	ret = nam_asic_trunk_ports_add(devNum,110,36,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 36 to trunk 110 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,110,38,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 38 to trunk 110 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,110,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 110 load-balance based dest mac error ! \n");
	}

	/*add port 16,18 to trunk 109 */
	ret = nam_asic_trunk_ports_add(devNum,109,16,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 16 to trunk 109 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,109,18,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 18 to trunk 109 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,109,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 109 load-balance based dest mac error ! \n");
	}

	/*add port 24,26 to trunk 108 */
	ret = nam_asic_trunk_ports_add(devNum,108,24,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 24 to trunk 108 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,108,26,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 26 to trunk 108 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,108,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 108 load-balance based dest mac error ! \n");
	}
	
	/*add port 4,6 to trunk 107 */
	ret = nam_asic_trunk_ports_add(devNum,107,4,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 4 to trunk 107 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,107,6,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 6 to trunk 107 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,107,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 107 load-balance based dest mac error ! \n");
	}
	return ret;
}

static int ax81_smue_trunk_config_for_8800_lion1(unsigned char devNum)
{
	unsigned int ret = RET_FAIL;
	unsigned short trunkId = 0;
	
	/*add port 50,52 to trunk 118 */
	
	ret = nam_asic_trunk_ports_add(devNum,118,50,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 50 to trunk 118 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,118,52,1); 
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 52 to trunk 118 error !\n");
	}

	ret = nam_asic_trunk_load_balanc_set(devNum,118,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 118 load-balance based dest mac error ! \n");
	}



	/*add port 40,42 to trunk 117 */
	ret = nam_asic_trunk_ports_add(devNum,117,40,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 40 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,117,42,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 42 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,117,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 117 load-balance based dest mac error ! \n");
	}




	/*add port 32,34 to trunk 116 */
	ret = nam_asic_trunk_ports_add(devNum,116,32,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 32 to trunk 116 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,116,34,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 34 to trunk 116 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,116,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 116 load-balance based dest mac error ! \n");
	}

	/*add port 20,22 to trunk 115 */
	ret = nam_asic_trunk_ports_add(devNum,115,20,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 20 to trunk 115 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,115,22,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 22 to trunk 115 error !\n");
	}

	ret = nam_asic_trunk_load_balanc_set(devNum,115,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 115 load-balance based dest mac error ! \n");
	}

	/*add port 8,10 to trunk 114 */
	ret = nam_asic_trunk_ports_add(devNum,114,8,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 8 to trunk 114 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,114,10,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 10 to trunk 114 error !\n");
	}

	ret = nam_asic_trunk_load_balanc_set(devNum,114,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 114 load-balance based dest mac error ! \n");
	}

	/*add port 0,2 to trunk 113 */
	ret = nam_asic_trunk_ports_add(devNum,113,0,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 0 to trunk 113 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,113,2,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 2 to trunk 113 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,113,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 113 load-balance based dest mac error ! \n");
	}

	/*add port 4,6 to trunk 112 */
	ret = nam_asic_trunk_ports_add(devNum,112,4,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 4 to trunk 112 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,112,6,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 6 to trunk 112 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,112,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 112 load-balance based dest mac error ! \n");
	}

	/*add port 24,26 to trunk 111 */
	ret = nam_asic_trunk_ports_add(devNum,111,24,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 24 to trunk 111 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,111,26,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 26 to trunk 111 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,111,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 111 load-balance based dest mac error ! \n");
	}
	/*add port 16,18 to trunk 110 */
	ret = nam_asic_trunk_ports_add(devNum,110,16,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 16 to trunk 110 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,110,18,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 18 to trunk 110 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,110,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 110 load-balance based dest mac error ! \n");
	}

	/*add port 36,38 to trunk 109 */
	ret = nam_asic_trunk_ports_add(devNum,109,36,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 36 to trunk 109 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,109,38,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 38 to trunk 109 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,109,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 109 load-balance based dest mac error ! \n");
	}
	
	/*add port 54,56 to trunk 108 */
	ret = nam_asic_trunk_ports_add(devNum,108,54,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 54 to trunk 108 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,108,56,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 56 to trunk 108 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,108,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 108 load-balance based dest mac error ! \n");
	}
	
	/*add port 48 to trunk 107 */
	ret = nam_asic_trunk_ports_add(devNum,107,48,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue add port 48 to trunk 107 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,107,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_dbg("AX81_smue set trunk 107 load-balance based dest mac error ! \n");
	}
	return ret;
}

static int ax81_smue_trunk_config_for_lion1(unsigned char devNum)
{
	unsigned int ret = RET_FAIL;

	unsigned long attr = 0,member = 0;
	unsigned short trunkId = 0;
	int i=0,j=16,m=32,n=48;

	
	/*add port 52,54,56 to trunk 110 */
	
	ret = nam_asic_trunk_ports_add(devNum,118,52,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 52 to trunk 110 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,118,54,1); 
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 54 to trunk 110 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,118,56,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 56 to trunk 110 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,118,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue set trunk 110 load-balance based dest mac error ! \n");
	}

	/*add port 32,34,36 to trunk 109 */
	ret = nam_asic_trunk_ports_add(devNum,117,32,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 32 to trunk 109 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,117,34,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 34 to trunk 109 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,117,36,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 36 to trunk 109 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,117,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue set trunk 109 load-balance based dest mac error ! \n");
	}




	/*add port 22,24,26 to trunk 108 */
	ret = nam_asic_trunk_ports_add(devNum,116,22,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 22 to trunk 108 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,116,24,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 24 to trunk 108 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,116,26,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 26 to trunk 108 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,116,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue set trunk 108 load-balance based dest mac error ! \n");
	}

	/*add port 0,2,4 to trunk 107 */
	ret = nam_asic_trunk_ports_add(devNum,115 ,0,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 0 to trunk 115 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,115 ,2,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 2 to trunk 107  error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,115 ,4,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 4 to trunk 107  error !\n");
	}

	ret = nam_asic_trunk_load_balanc_set(devNum,115,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue set trunk 107  load-balance based dest mac error ! \n");
	}

	/*add port 6,8,10 to trunk 106 */
	ret = nam_asic_trunk_ports_add(devNum,114,6,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 6 to trunk 106 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,114,8,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 8 to trunk 106 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,114,10,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 10 to trunk 106 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,114,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue set trunk 106 load-balance based dest mac error ! \n");
	}

	/*add port 16,18,20 to trunk 105 */
	ret = nam_asic_trunk_ports_add(devNum,113 ,16,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 16 to trunk 105  error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,113 ,18,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 18 to trunk 105  error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,113 ,20,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 20 to trunk 105  error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,113,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue set trunk 105  load-balance based dest mac error ! \n");
	}

	/*add port 38,40,42 to trunk 104 */
	ret = nam_asic_trunk_ports_add(devNum,112,38,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 38 to trunk 104 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,112,40,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 40 to trunk 104 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,112,42,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 42 to trunk 104 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,112,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue set trunk 104 load-balance based dest mac error ! \n");
	}

	/*add port 48,50 to trunk 103 */
	ret = nam_asic_trunk_ports_add(devNum,111,48,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 48 to trunk 103 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,111,50,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue add port 50 to trunk 103 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,111,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_smue set trunk 103 load-balance based dest mac error ! \n");
	}

	
	return ret;
}

/**********************************************************************
 *  npd_ax81_smue_port_interrupt_init
 *
 *  DESCRIPTION:
 *         config SMU interrupt   -- its has for groups 
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
 *
 **********************************************************************/

static int npd_ax81_smue_port_interrupt_init(void)
{
	int ret = RET_FAIL;
	unsigned int data = 0x9ffbffee;
	unsigned int regAddr = 0x00000034;
	int dd = 0,group = 0;

	for(dd;dd<2;dd++)
	{
		for(group;group<4;group++)
		{
			ret = nam_write_reg(dd,group,regAddr,data); 
			if(ret != RET_SUCCESS)
			{
				syslog_ax_board_err("ax81_smue init dev %d group %d interrupt err !\n",dd,group);
				return ret;
			}
		}
	}

	return ret;
}

int ax81_smue_trunk_config(void)
{
	int ret = RET_FAIL;

	if(PRODUCT_TYPE == AUTELAN_PRODUCT_AX8610)
	{
		ret = ax81_smue_trunk_config_for_lion0(0);
		if(ret != RET_SUCCESS)
		{
			syslog_ax_board_err("config lion0 cscd port to trunk err !\n");
			return RET_FAIL;
		}
		ret = ax81_smue_trunk_config_for_lion1(1);
		if(ret != RET_SUCCESS)
		{
			syslog_ax_board_err("config lion1 cscd port to trunk err !\n");
			return RET_FAIL;
		}
	}
	else if(PRODUCT_TYPE == AUTELAN_PRODUCT_AX8606)
	{
		ret = ax81_smue_trunk_config_for_8606(0);
		if(ret != RET_SUCCESS)
		{
			syslog_ax_board_err("config lion0 cscd port to trunk err !\n");
			return RET_FAIL;
		}
	}
	else if(PRODUCT_TYPE == AUTELAN_PRODUCT_AX8800)
	{
		ret = ax81_smue_trunk_config_for_8800_lion0(0);
		if(ret != RET_SUCCESS)
		{
			syslog_ax_board_err("config lion0 cscd port to trunk err !\n");
			return RET_FAIL;
		}
		ret = ax81_smue_trunk_config_for_8800_lion1(1);
		if(ret != RET_SUCCESS)
		{
			syslog_ax_board_err("config lion1 cscd port to trunk err !\n");
			return RET_FAIL;
		}
	}

	return ret;
	
}

/**********************************************************************
 *  npd_init_ax81_smue
 *
 *  DESCRIPTION:
 *          Config for AX81_SMU.
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
int npd_init_ax81_smue(void)
{
    int ret = RET_FAIL;	
    int devNum = 0;
	int mtuIndex = 0;
    int GroupId = 0;
    int PortNum = 0;
    int i = 0;
    unsigned int data = 0;
    unsigned int regAddr_TxIPGMode = 0;
    unsigned int regAddr_FixedIPGBase = 0;
	unsigned char num_of_asic = 0;

	num_of_asic = nam_asic_get_instance_num();
	for(devNum;devNum<num_of_asic;devNum++)
	{
	
		/* set mtu of all ports to 1548 */
	    for(GroupId=0; GroupId<4; GroupId++)
	    {
	        PortNum = 16 * GroupId;
	        for(i=0; i<12; i++)
	        {
	            ret = nam_set_ethport_mru(devNum, (PortNum+i), INTERNAL_MTU);
	            if (RET_SUCCESS != ret)
	            {
	                syslog_ax_board_err("nam set mtu fail, return %d\n",ret);
	                return RET_FAIL;
	            }
	            
	        }
	    }

		/* set vlan mtu of LION, VLAN MRU Profile Configuration Reg-ister */
	    for(mtuIndex = 0; mtuIndex < 8; mtuIndex++)
	    {
	        ret = nam_config_vlan_mtu(devNum,mtuIndex,INTERNAL_MTU);
	        if (RET_SUCCESS != ret)
	        {
	            syslog_ax_board_err("nam set vlan mtu fail, return %d\n",ret);
	            return RET_FAIL;
	        }
	    }
	    
	}

	ret = npd_ax81_smue_port_interrupt_init();
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("ax81_smue port interrupt init err !\n");
		return ret;
	}
    syslog_ax_board_dbg("Init for board ax81-smue OK\n");
    return ret;
}

#ifdef __cplusplus
}
#endif 
