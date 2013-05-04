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
* npd_board_ax81_ac4x.c
*
*
* CREATOR:
*		lich@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for special board routine.
*
* DATE:
*		24/05/2012	
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
#include "npd_board_ax81_ac4x.h"
#include "npd_trunk.h"

extern char g_dis_dev[MAX_ASIC_NUM];

ASIC_PORT_ATTRIBUTE_S ax81_ac4x_attr_xg =
{
	ETH_ATTR_ENABLE,1536,PORT_FULL_DUPLEX_E,PORT_SPEED_10000_E,	\
	ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_OFF,ETH_ATTR_OFF,	\
	ETH_ATTR_OFF,ETH_ATTR_OFF,COMBO_PHY_MEDIA_PREFER_NONE
};

struct eth_port_func_data_s ax81_ac4x_funcs = 
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

struct eth_port_counter_s ax81_ac4x_counters=
{
};

asic_cscd_port_t ax81_ac4x_cscd_ports[10]={		
    {0,32},		
    {0,34},		
    {0,36},		
    {0,38},		
    {0,40},		
    {0,42},	
    {0,58},		
    {0,56},		
    {0,54},		
    {0,52}		
};
asic_cpu_port_t ax81_ac4x_to_cpu_ports[MAX_CPU_PORT_NUM] = {
    {0,48},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},    /* connect to master CPU */
	{0,4},{0,6},{0,10},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}     /* connect to slave CPU */
};
asic_eth_port_t ax81_ac4x_eth_ports[4]={   \
	{
		ETH_XGE_FIBER,0xFFFFFFFF,ETH_PORT_MRU_DEFAULT,0x0,0x0,0x0,0x0, \
		&ax81_ac4x_attr_xg,
		&ax81_ac4x_funcs,
		&ax81_ac4x_counters
	},
	{
		ETH_XGE_FIBER,0xFFFFFFFF,ETH_PORT_MRU_DEFAULT,0x0,0x0,0x1,0x0, \
		&ax81_ac4x_attr_xg,
		&ax81_ac4x_funcs,
		&ax81_ac4x_counters
	},
	{
		ETH_XGE_FIBER,0xFFFFFFFF,ETH_PORT_MRU_DEFAULT,0x0,0x0,0x2,0x0, \
		&ax81_ac4x_attr_xg,
		&ax81_ac4x_funcs,
		&ax81_ac4x_counters
	},
	{
		ETH_XGE_FIBER,0xFFFFFFFF,ETH_PORT_MRU_DEFAULT,0x0,0x0,0x3,0x0, \
		&ax81_ac4x_attr_xg,
		&ax81_ac4x_funcs,
		&ax81_ac4x_counters
	}
};


unsigned int ax81_ac4x_port_mapArr[4] = {
    /* x-1 x-2 x-3 x-4*/
	0x0, 0x1, 0x2, 0x3  
};

unsigned int ax81_ac4x_xg_port_to_phyid[4] = {
    3,2,1,0
};
int ax81_4x_trunk_config();

unsigned int ax81_ac4x_test(void) 
{
	int ret = 0;
	unsigned int vid = 4000;
	unsigned int devnum = 0;   /* no use,just for printf */
#if 1
	syslog_ax_board_dbg("slot %d!\n", asic_board->slot_id);
    vid = 1000;
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
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 48, 1);  
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
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 48,1);  
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 4, 1);  
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
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 4, 1);  
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 6, 1);  
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
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 3, 0);  
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 10, 1);  
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}
	vid = 100;
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
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 1, 0);  
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}	
#else
	vid = 1000;
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

    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 6, 1);  
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
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 1, 0);  
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 6, 1);  
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
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 2, 0);  
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 10, 1);  
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
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 3, 0);  
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 10, 1);  
	if(0 != ret){
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
	}
#endif

    syslog_ax_board_dbg("AX81-12X ge_xg_test ready!\n");
	
	return ret;
}



int npd_ax81_4x_dev_map_table_set(void)
{
	int ret = RET_FAIL;
	int i = 0;
	cscd_trunk_link_type lion0_linktype,lion1_linktype;
	cscd_trunk_link_hash_mode trunk_hash_mode = CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E;
	cscd_trunk_link_type_mode linktype_mode_trunk = CSCD_TRUNK_LINK_TYPE_TRUNK_E;
	
	lion0_linktype.linkType = linktype_mode_trunk;
	lion0_linktype.linkNum = 118;
	lion1_linktype.linkType = linktype_mode_trunk;
	lion1_linktype.linkNum = 116;
	
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
			syslog_ax_board_dbg("dev Map table is ########################\n");
			syslog_ax_board_dbg("dev  trunk \n");
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
				if((i == 12) || (i == 14))/*just for slot 5*/
					continue;
				if(i == 10)/*for SMU lion1*/
				{
					ret = nam_cscd_dev_map_set(0,i,0,&lion1_linktype,trunk_hash_mode);/*for dev1*/
					if(ret != RET_SUCCESS)
					{
						syslog_ax_board_err("set dev map table FAIL !\n");
						return ret;
					}
					syslog_ax_board_dbg("%d,%d\n",i,lion1_linktype.linkNum);
					continue;
				}
				/*for SMU lion0*/
				ret = nam_cscd_dev_map_set(0,i,0,&lion0_linktype,trunk_hash_mode);/*for MASTER slot*/
				if(ret != RET_SUCCESS)
				{
					syslog_ax_board_err("set dev map table FAIL !\n");
					return ret;
				}
				syslog_ax_board_dbg("%d,%d\n",i,lion0_linktype.linkNum);
			}
			syslog_ax_board_dbg("end of dev Map ########################\n");
			break;
		default:
			syslog_ax_board_warning("unsupport PRODUCT TYPE !!!\n");
			break;
	}
	npd_syslog_info("npd ax81_12x dev map table set successfully !\n");
	return RET_SUCCESS;
}

asic_board_t ax81_ac4x_board_t =
{
    .board_type = 0,
    .board_name = "ax81_ac4x",
    .slot_id = -1,   /* dynamic init in npd */
    .asic_type = ASIC_TYPE_CPSS,
    .capbmp = ASIC_CAPBMP,
	.runbmp = ASIC_RUNBMP,
	
    .asic_port_cnt = 4,  /* asic port on chip  */
    .asic_cscd_port_cnt = 10,
    .asic_port_cnt_visable = 4,  /* asic port on panel */
    .asic_port_start_no = 1,
    
    .asic_port_mapArr = NULL,
	.asic_eth_ports = ax81_ac4x_eth_ports,
    .asic_cscd_ports = ax81_ac4x_cscd_ports,
    .asic_to_cpu_ports = ax81_ac4x_to_cpu_ports,

	.id = 0xF0F0F0F0,
	.state = MODULE_STAT_RUNNING,
	.hw_version = 0x0,
	.sn = "not_init",
	.ext_slot_data = "not_init",   
	
	.func_test=NULL,
    .asic_init=NULL,
    .my_equipment_test = ax81_ac4x_test,
    .distributed_system_dev_map_table_set = npd_ax81_4x_dev_map_table_set,
    .board_cscd_port_trunk_config = ax81_4x_trunk_config
};

/**********************************************************************
 *  phy_88X2140_config
 *
 *  DESCRIPTION:
 *          Config for AX81_AC4X.
 *
 *  INPUT:
 *          devNum,PortNum
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          RET_SUCCESS
 *          RET_FAIL
 *
 *  COMMENTS:
 *          Called only by npd_init_ax81_ac4x()
 *
 **********************************************************************/
/*int phy_88X2140_config(int devNum,int PortNum)*/
static int phy_88X2140_config(int devNum, int portNum)
{
	int ret = 0;
	int phyId = ax81_ac4x_xg_port_to_phyid[portNum];

	ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xF002, 31, 0x8102);
    usleep( 10 );
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD0AA, 30, 0x027D); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD0A0, 30, 0x000B); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD0A1, 30, 0x0204); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD0A6, 30, 0x0D09); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD0A7, 30, 0x0904); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD0A5, 30, 0x0409); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD0F1, 30, 0x7E81); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD0AE, 30, 0x0080); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD0A2, 30, 0x9FFF); 
    usleep( 2 );
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD0A2, 30, 0x1FFF);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xF002, 31, 0x8102);
    usleep( 10 );
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xF06A, 4, 0x2000);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xF06A, 4, 0x3000);
    usleep( 5 );
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xC000, 30, 0x8C00);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xC000, 30, 0x9C00);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xC000, 30, 0xDC00);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xC000, 30, 0xFC00);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE000, 30, 0x7F00);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD070, 30, 0x0010); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD078, 30, 0x0010); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD080, 30, 0x0010); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD088, 30, 0x0010); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD090, 30, 0x0010); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD098, 30, 0x0010); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD073, 30, 0x0D89); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD075, 30, 0xF277); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD07B, 30, 0x203A); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD07D, 30, 0xDFC6); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD040, 30, 0x4020); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD0B5, 30, 0x0224); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD0B0, 30, 0x0000);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD0B1, 30, 0x0204); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD08A, 30, 0x0140); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD08B, 30, 0x0834); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD08C, 30, 0x013F); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD08D, 30, 0xF7CC); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD082, 30, 0x0140); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD083, 30, 0x0834); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD084, 30, 0x013F); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD085, 30, 0xF7CC); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD092, 30, 0x0140); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD093, 30, 0x1388); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD094, 30, 0x013F); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD095, 30, 0xEC78); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD09A, 30, 0x0140); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD09B, 30, 0x1388); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD09C, 30, 0x013F); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD09D, 30, 0xEC78); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD01C, 30, 0x1800); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xC030, 30, 0x0209); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD070, 30, 0x8010);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD078, 30, 0x8010); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD080, 30, 0x8010);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD088, 30, 0x8010); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD090, 30, 0x8010); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD098, 30, 0x8010); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD040, 30, 0x4020); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD042, 30, 0x0A6C); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD04F, 30, 0xC200);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD04F, 30, 0xC20F);
    usleep( 1 );
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD049, 30, 0x0001);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD049, 30, 0x0011);
    usleep( 5 );
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD049, 30, 0x0002);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD049, 30, 0x0012);
    usleep( 5 );
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD049, 30, 0x0004);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD049, 30, 0x0014);
    usleep( 5 );
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD049, 30, 0x0008);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD049, 30, 0x0018);
    usleep( 5 );
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD049, 30, 0x0000);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD04F, 30, 0xC20F);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD012, 30, 0x0058); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD011, 30, 0x12C4); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD0AE, 30, 0x0080); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD0BC, 30, 0x0080); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xC205, 30, 0xF0F4); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xD0E0, 30, 0x0318); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE016, 30, 0x0B05); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE000, 30, 0x7F00);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE005, 30, 0x28A6); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE005, 30, 0x28A7); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE016, 30, 0x0B05); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE016, 30, 0x0A05); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE011, 30, 0x2380); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE02E, 30, 0x00FF); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE01D, 30, 0x0000);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE01C, 30, 0x0000);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE011, 30, 0x2380); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE002, 30, 0x677A); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE02F, 30, 0x2045); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE02F, 30, 0x20A5); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE010, 30, 0x2940); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE040, 30, 0x0003); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE040, 30, 0x000B); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE02C, 30, 0x007F); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE001, 30, 0x380A); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE001, 30, 0x383C); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE03C, 30, 0x003C); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE001, 30, 0x383C); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE002, 30, 0x697A); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE02B, 30, 0x1900); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE01D, 30, 0x0000);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE01C, 30, 0x0000);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE02F, 30, 0x20A5); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE029, 30, 0x1900); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE02A, 30, 0x0000);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE028, 30, 0x0000);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE025, 30, 0x0012); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE042, 30, 0x2000); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE043, 30, 0x0006); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE004, 30, 0x0A0C); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE023, 30, 0x1820); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE00A, 30, 0x0FFF);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE00B, 30, 0x0FFF);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE00C, 30, 0x0FFF);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE008, 30, 0x06DB); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE009, 30, 0x0249); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE006, 30, 0x06DB); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE007, 30, 0x06DB); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE003, 30, 0x0404); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE024, 30, 0x1014); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE005, 30, 0x29F7); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE00F, 30, 0x009C); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE00F, 30, 0x0098); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE00D, 30, 0xBF00); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE00E, 30, 0x1500); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE01E, 30, 0x0002); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE01F, 30, 0x000A); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE01A, 30, 0x0000);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE01B, 30, 0x0000);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE016, 30, 0x0A05); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE016, 30, 0x0205); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE016, 30, 0x0005); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE027, 30, 0x8001); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE00D, 30, 0x9F00); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE042, 30, 0x0400); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE052, 30, 0x0820); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE053, 30, 0x0820); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE048, 30, 0x0300); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE000, 30, 0x7F01);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE000, 30, 0x7F00);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE048, 30, 0x0200); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE000, 30, 0x7F01);
    usleep( 5 );
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE000, 30, 0x7F00);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xE000, 30, 0x7F02);
    usleep( 750 );
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xC005, 30, 0x0018); 
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xC005, 30, 0x0000);
    usleep( 5 );
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xF06A, 4, 0x2000);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xF06A, 4, 0x0000);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xC005, 30, 0x0001);
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xC005, 30, 0x0000);
	
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xF012, 31, 0x0CB0);
    /*LED0 configuration*/
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xF020, 31, 0x0100);
    /*LED1 configuration*/
    ret = nam_asic_auxi_phy_write(devNum,portNum, phyId, 1, 0xF021, 31, 0x0060);

    return RET_SUCCESS;
}

static int ax81_4x_trunk_config_for_8610(unsigned char devNum)
{
	unsigned int ret = RET_FAIL;

	/*add port 32,36,40 to trunk 118 for solt5 lion0*/
	ret = nam_asic_trunk_ports_add(devNum,118,32,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 32 to trunk 118 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,118,36,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 36 to trunk 118 error !\n");
	}

	ret = nam_asic_trunk_ports_add(devNum,118,40,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 36 to trunk 118 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,118,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x set trunk 118 load-balance based dest mac error ! \n");
	}

	/*add port 34,38,42 to trunk 117 for slot6 lion0*/
	ret = nam_asic_trunk_ports_add(devNum,117,34,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 34 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,117,38,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 38 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,117,42,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 38 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,117,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x set trunk 117 load-balance based dest mac error ! \n");
	}

	
	/*add port 54,58 to trunk 116 for slot5 lion1*/
	ret = nam_asic_trunk_ports_add(devNum,116,54,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 54 to trunk 116 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,116,58,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 58 to trunk 116 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,116,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x set trunk 116 load-balance based dest mac error ! \n");
	}

	
	/*add port 52,56 to trunk 115 for slot6 lion1*/
	ret = nam_asic_trunk_ports_add(devNum,115,52,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 52 to trunk 115 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,115,56,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 56 to trunk 115 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,115,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x set trunk 115 load-balance based dest mac error ! \n");
	}
	return ret;
}

static int ax81_4x_trunk_config_for_8800(unsigned char devNum)
{
	unsigned int ret = RET_FAIL;

	/*add port 32,36 to trunk 118 for slot5 lion0*/
	ret = nam_asic_trunk_ports_add(devNum,118,32,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 32 to trunk 118 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,118,36,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 36 to trunk 118 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,118,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x set trunk 118 load-balance based dest mac error ! \n");
	}

	/*add port 34,38 to trunk 117 for slot6 lion0*/
	ret = nam_asic_trunk_ports_add(devNum,117,34,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 34 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,117,38,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 38 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,117,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x set trunk 117 load-balance based dest mac error ! \n");
	}

	
	/*add port 54,58 to trunk 116 for slot6 lion1*/
	ret = nam_asic_trunk_ports_add(devNum,116,54,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 54 to trunk 116 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,116,58,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 58 to trunk 116 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,116,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x set trunk 116 load-balance based dest mac error ! \n");
	}

	
	/*add port 52,56 to trunk 115 for slot5 lion1*/
	ret = nam_asic_trunk_ports_add(devNum,115,52,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 52 to trunk 115 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,115,56,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 56 to trunk 115 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,115,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x set trunk 115 load-balance based dest mac error ! \n");
	}
	return ret;
}

static int ax81_4x_trunk_config_for_8606(unsigned char devNum)
{
	unsigned int ret = RET_FAIL;

	/*add port 32,36,40,52,56 to trunk 118 for solt3 lion0*/
	ret = nam_asic_trunk_ports_add(devNum,118,32,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 32 to trunk 118 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,118,36,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 36 to trunk 118 error !\n");
	}

	ret = nam_asic_trunk_ports_add(devNum,118,40,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 36 to trunk 118 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,118,52,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 52 to trunk 118 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,118,56,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 56 to trunk 118 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,118,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x set trunk 118 load-balance based dest mac error ! \n");
	}

	/*add port 34,38,42 to trunk 117 for slot4 lion0*/
	ret = nam_asic_trunk_ports_add(devNum,117,34,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 34 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,117,38,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 38 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,117,42,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 38 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,117,54,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 54 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_ports_add(devNum,117,58,1);
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x add port 58 to trunk 117 error !\n");
	}
	ret = nam_asic_trunk_load_balanc_set(devNum,117,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
	if(ret != RET_SUCCESS)
	{
		syslog_ax_board_err("AX81_4x set trunk 117 load-balance based dest mac error ! \n");
	}

	return ret;
}

int ax81_4x_trunk_config()
{
	int ret = RET_FAIL;
	if(PRODUCT_TYPE == AUTELAN_PRODUCT_AX8610)
	{
		ret = ax81_4x_trunk_config_for_8610(0);
		if(RET_SUCCESS != ret)
		{
			syslog_ax_board_err("AX81_ac4x trunk init for AX8610 fail !\n");
			return RET_FAIL;
		}
	}
	if(PRODUCT_TYPE == AUTELAN_PRODUCT_AX8800)
	{
		ret = ax81_4x_trunk_config_for_8800(0);
		if(RET_SUCCESS != ret)
		{
			syslog_ax_board_err("AX81_ac4x trunk init for AX8700 fail !\n");
			return RET_FAIL;
		}
	}
	if(PRODUCT_TYPE == AUTELAN_PRODUCT_AX8606)
	{
		ret = ax81_4x_trunk_config_for_8606(0);
		if(RET_SUCCESS != ret)
		{
			syslog_ax_board_err("AX81_ac4x trunk init for AX8606 fail !\n");
			return RET_FAIL;
		}
	}
	
	return ret;
}
/**********************************************************************
 *  npd_init_ax81_ac4x
 *
 *  DESCRIPTION:
 *          Config for AX81_ac4x.
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
int npd_init_ax81_ac4x(void)
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

	/* set mtu of GroupId 1,2,3 to 1548 */
	/* default mtu of GroupId 0 to 1536 */
    for(GroupId=0; GroupId<4; GroupId++)    
    {
        PortNum = 16 * GroupId;
        for(i=0; i<12; i++)
        {
			if((GroupId == 0) && (i < 4))
				continue;
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
	
	/*change 0 throught 3 ports mode from xaui to rxaui*/
    for(PortNum=0; PortNum<4; PortNum++)
    {
		ret=nam_change_mode_xaui_to_rxaui(devNum,PortNum);
	    if (RET_SUCCESS != ret)
	    {
		    syslog_ax_board_err("cpss set dev %d port %d rxaui mode fail, return %d\n", devNum, PortNum, ret);
			return RET_FAIL;
	    }
    }
	
    syslog_ax_board_dbg("change 0 throught 3 ports mode from xaui to rxaui finshed!\n");
    PortNum = 6;
	ret=nam_change_mode_xaui_to_rdxaui(devNum, PortNum);
	if (RET_SUCCESS != ret)
	{
		syslog_ax_board_err("cpss set dev %d port %d rxaui mode fail, return %d\n", devNum, PortNum, ret);
        return RET_FAIL;
	}
	syslog_ax_board_dbg("change %d port mode from xaui to rdxaui finshed!\n",PortNum);
	PortNum = 10;
	ret=nam_change_mode_xaui_to_rdxaui(devNum, PortNum);
	if (RET_SUCCESS != ret)
	{
		syslog_ax_board_err("cpss set dev %d port %d rxaui mode fail, return %d\n", devNum, PortNum, ret);
        return RET_FAIL;
	}
	syslog_ax_board_dbg("change %d port mode from xaui to rdxaui finshed!\n",PortNum);
	
    /*config 88X2140 phy*/
	for(PortNum = 3; PortNum >= 0; PortNum--)
	{
        ret = phy_88X2140_config(devNum,PortNum);
        if(RET_SUCCESS != ret)
        {
            syslog_ax_board_err("port %d phy_88x2140 config fail, ret=%d\n",PortNum, ret);
            return RET_FAIL;
        }
	}
	syslog_ax_board_dbg("phy_88x2140 config finshed!\n");	
	
    syslog_ax_board_dbg("Init for board ax81-12x\n");
    return ret;
}

#ifdef __cplusplus
}
#endif 
