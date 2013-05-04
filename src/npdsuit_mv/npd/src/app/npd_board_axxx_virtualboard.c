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
* npd_board_axxx_virtualboard.c
*
*
* CREATOR:
*		wangchong@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for virtual board routine.
*
* DATE:
*		7/26/2012	
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
#include "npd_board_axxx_virtualboard.h"

ASIC_PORT_ATTRIBUTE_S axxx_virtualboard_attr_xg =
{
	ETH_ATTR_ENABLE,1536,PORT_FULL_DUPLEX_E,PORT_SPEED_10000_E,	\
	ETH_ATTR_ENABLE,ETH_ATTR_DISABLE,ETH_ATTR_OFF,ETH_ATTR_OFF,	\
	ETH_ATTR_OFF,ETH_ATTR_OFF,COMBO_PHY_MEDIA_PREFER_NONE
};

struct eth_port_func_data_s axxx_virtualboard_funcs = 
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

struct eth_port_counter_s axxx_virtualboard_counters=
{
};

asic_cscd_port_t axxx_virtualboard_cscd_ports[0]={				
};
asic_cpu_port_t axxx_virtualboard_to_cpu_ports[MAX_CPU_PORT_NUM] = {
    {-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},    /* connect to master CPU */
	{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}     /* connect to slave CPU */
};
asic_eth_port_t axxx_virtualboard_eth_ports[0]={
};


unsigned int axxx_virtualboard_port_mapArr[0] = {
};

asic_board_t axxx_virtualboard_board_t =
{
    .board_type = 0,
    .board_name = "axxx_virtualboard",
    .slot_id = -1,   /* dynamic init in npd */
    .asic_type = ASIC_TYPE_CPSS,
    .capbmp = ASIC_CAPBMP,
	.runbmp = ASIC_RUNBMP,
	
    .asic_port_cnt = 0,  /* asic port on chip  */
    .asic_cscd_port_cnt = 0,
    .asic_port_cnt_visable = 0,  /* asic port on panel */
    .asic_port_start_no = 1,
    
    .asic_port_mapArr = NULL,
	.asic_eth_ports = axxx_virtualboard_eth_ports,
    .asic_cscd_ports = axxx_virtualboard_cscd_ports,
    .asic_to_cpu_ports =axxx_virtualboard_to_cpu_ports,

	.id = 0xF0F0F0F0,
	.state = MODULE_STAT_RUNNING,
	.hw_version = 0x0,
	.sn = "not_init",
	.ext_slot_data = "not_init",   
	
	.func_test=NULL,
    .asic_init=NULL,
    .my_equipment_test = NULL
};
/**********************************************************************
 *  npd_init_axxx_virtualboard
 *
 *  DESCRIPTION:
 *          Config for axxx_virtualboard
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
int npd_init_axxx_virtualboard(void)
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
    syslog_ax_board_dbg("Init for board ax81-12x\n");
    return ret;
}

#ifdef __cplusplus
}
#endif 
