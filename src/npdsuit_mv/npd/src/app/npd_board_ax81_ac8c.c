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
* npd_board_ax81_ac8c.c
*
*
* CREATOR:
*		zhangdi@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for special board routine.
*
* DATE:
*		02/06/2011	
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
#include "npd_board_ax81_ac8c.h"
#include "npd_trunk.h"

extern char g_dis_dev[MAX_ASIC_NUM];

ASIC_PORT_ATTRIBUTE_S ax81_ac8c_attr_ge =
{
	ETH_ATTR_ENABLE,1536,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
	ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
	ETH_ATTR_ON,ETH_ATTR_ON,COMBO_PHY_MEDIA_PREFER_COPPER
};

struct eth_port_func_data_s ax81_ac8c_funcs = 
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

struct eth_port_counter_s ax81_ac8c_counters=
{
};

asic_eth_port_t ax81_ac8c_eth_ports[4]={   \
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,0,0x0, \
		&ax81_ac8c_attr_ge,
		&ax81_ac8c_funcs,
		&ax81_ac8c_counters
	},
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,1,0x0, \
		&ax81_ac8c_attr_ge,
		&ax81_ac8c_funcs,
		&ax81_ac8c_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,2,0x0, \
		&ax81_ac8c_attr_ge,
		&ax81_ac8c_funcs,
		&ax81_ac8c_counters
	},	
	{
		ETH_GTX,0xFFFFFFFF,1536,0x0,0x0,3,0x0, \
		&ax81_ac8c_attr_ge,
		&ax81_ac8c_funcs,
		&ax81_ac8c_counters
	}
};

/* panle port to virtual port map array */
/* 2-1 is dev_port(0,0x0) */
unsigned int ax81_ac8c_port_mapArr[4] = {
    /* 2-1 2-2 2-3 2-4 */
	0x0, 0x1, 0x2, 0x3  
};

asic_cscd_port_t ax81_ac8c_cscd_ports[2]={
    {0,25},
    {0,26}
};

asic_cpu_port_t ax81_ac8c_to_cpu_ports[MAX_CPU_PORT_NUM] = {
    {0,27},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},    /* connect to master CPU */
	{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}     /* connect to slave CPU */
};

int npd_ax81_ac8c_dev_map_table_set();

unsigned int ax81_ac8c_test(void) 
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
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 2, 0);  /* (0,2) */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
	
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 25, 0);  /* (0,25) */
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
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 3, 0);  /* (0,3) */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 26, 0);  /* (0,26) */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}
	
    vid = 3000;
    ret = create_vlan(devnum,vid);
	if(0 != ret){
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;				
	}	
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 0, 0);  /* (0,0) */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;				
	}	
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 1, 0);   /* (0,1) */
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;				
	}
    syslog_ax_board_dbg("AX81-AC8C ge_xg_test ready!\n");
	return ret;
}

asic_board_t ax81_ac8c_board_t =
{
    .board_type = 0,
    .board_name = "ax81_ac8c",
    .slot_id = -1,   /* dynamic init in npd */
    .asic_type = ASIC_TYPE_CPSS,
    .capbmp = ASIC_CAPBMP,
	.runbmp = ASIC_RUNBMP,
    .asic_port_cnt = 4,
    .asic_cscd_port_cnt = 2,
    .asic_port_cnt_visable = 4, /* asic port on panel */    
    .asic_port_start_no = 5,

    .asic_port_mapArr = NULL,
	.asic_eth_ports = ax81_ac8c_eth_ports,
    .asic_cscd_ports = ax81_ac8c_cscd_ports,
    .asic_to_cpu_ports = ax81_ac8c_to_cpu_ports,

	.id = 0xF0F0F0F0,
	.state = MODULE_STAT_RUNNING,
	.hw_version = 0x0,
	.sn = "not_init",
	.ext_slot_data = "not_init",   
	
	.func_test=NULL,
    .asic_init=NULL,
    .my_equipment_test = ax81_ac8c_test,
    .distributed_system_dev_map_table_set = npd_ax81_ac8c_dev_map_table_set,
    .board_cscd_port_trunk_config = NULL
};

asic_cscd_port_t ax81_ac8c_cscd_port_slot1={0,26};
asic_cscd_port_t ax81_ac8c_cscd_port_slot2={0,25};
int npd_ax81_ac8c_dev_map_table_set(void)
{
	int ret = RET_FAIL;
	int i = 0;
	cscd_trunk_link_type smu_linktype,standby_linktype;

	smu_linktype.linkNum = 25;
	smu_linktype.linkType = CSCD_TRUNK_LINK_TYPE_PORT_E;
	standby_linktype.linkNum = 26;
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
	syslog_ax_board_dbg("npd ax81_ac8c dev map table set successfully !\n");
	return RET_SUCCESS;
}

/**********************************************************************
 *  npd_init_ax81_ac8c
 *
 *  DESCRIPTION:
 *          Config for AX81_AC8C.
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
int npd_init_ax81_ac8c(void)
{
    int ret = RET_FAIL;
    unsigned char devNum = 0;
	unsigned char portNum = 0;
	int mtuIndex = 0;

    /* change the cscd port 26,27 of XCAT to 10G mode on AX8610 */
	ret = force_port_to_xg_mode(0,26);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("force port to 10G mode error !\n");
        return RET_FAIL;
    }
	ret = force_port_to_xg_mode(0,27);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("force port to 10G mode error !\n");
        return RET_FAIL;
    }		
	syslog_ax_vlan_dbg("force AC8C XCAT port 26,27 to xg mode on AX81 OK.\n");

	/* set mtu of port24,25,26,27 on xcat, to 1536 */	
	ret = nam_set_ethport_mru(devNum,24,INTERNAL_MTU);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("nam set mtu fail, return %d\n",ret);
        return RET_FAIL;
    }
	ret = nam_set_ethport_mru(devNum,25,INTERNAL_MTU);
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
			ax81_ac8c_board_t.asic_cscd_port_cnt = 1;
			ax81_ac8c_board_t.asic_cscd_ports = &ax81_ac8c_cscd_port_slot1;
		}
		else if(SLOT_ID == 2)
		{
			ax81_ac8c_board_t.asic_cscd_port_cnt = 1;
			ax81_ac8c_board_t.asic_cscd_ports = &ax81_ac8c_cscd_port_slot2;
		}
	}
    syslog_ax_board_dbg("Init for board ax81-ac8c, add here! now return !\n");

    return RET_SUCCESS;
}

#ifdef __cplusplus
}
#endif 
