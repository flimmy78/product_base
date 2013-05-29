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
* nam_fdb.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		APIs used in NAM for FDB module.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.116 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef DRV_LIB_BCM
#include <bcm/l2.h>
#include <bcm/mcast.h>
#include <bcm/types.h>
#include <bcm/switch.h>
#endif

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <netinet/in.h>
#include <pthread.h>
#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "nam_fdb.h"
#include "nam_asic.h"
#include "nam_log.h"

#ifdef DRV_LIB_BCM
#include "bcm/l2.h"
#include "bcm/mcast.h"
#endif
extern char g_dis_dev[MAX_ASIC_NUM];

static unsigned char  dut[6]={0x00,0x11,0x22,0x33,0x44,0x66};
static unsigned char  rip2dip[4]={224,0,0,9};
static unsigned char  ospf1dip[4]={224,0,0,5};
static unsigned char  ospf2dip[4]={224,0,0,6};
static unsigned char   anySip[4]={0,0,0,0};
static unsigned char* dutinit[SYSTEM_STATIC_LEN]={dut,rip2dip,ospf1dip,ospf2dip};

pthread_mutex_t FdbActMutex=PTHREAD_MUTEX_INITIALIZER;

#define GT_HW_MAC_LOW32(macAddr)                \
                ((macAddr)->arEther[5] |          \
                ((macAddr)->arEther[4] << 8) |    \
                ((macAddr)->arEther[3] << 16) |   \
                ((macAddr)->arEther[2] << 24))
#define GT_HW_MAC_HIGH16(macAddr)           \
        ((macAddr)->arEther[1] | ((macAddr)->arEther[0] << 8))              
#define getbit(word,bitNum) ((word) >> (bitNum))

#ifdef DRV_LIB_BCM
typedef struct {
     bcm_l2_addr_t  l2addr[L2_ADDR_NUM_MAX];
	 unsigned int size;
	 unsigned int count;
}user_data;
#endif
/***********************************************************
 *  nam_get_distributed_devnum
 * 
 *  get distributed devices's dev num 
 *
 *	input params:
 *		devNum - devnum 
 *	
 *	output: none
 *
 * 	return:
 *		0- get dev num success
 *		other - get dev Num failure
 *		
 ************************************************/
unsigned long nam_get_distributed_devnum(unsigned char asic_num,unsigned int *devNum)
{
	unsigned long ret = NAM_ERR;
	#ifdef DRV_LIB_CPSS
	ret = prvCpssDrvHwPpGetRegField(asic_num,0x58,4,5,devNum);
	if(ret != NAM_OK)
	{
		nam_syslog_err("nam_get_distributed_devnum FAIL !\n");
	}
	else
	{
		nam_syslog_dbg("asic_num is %d distributed devnum is %d \n",asic_num,*devNum);
	}
	#endif
	return ret; 
}

/***********************************************************
 *  nam_enable_device
 * 
 *  set device enable/disable 
 *
 *	input params:
 *		devNum - asic num 
 *      isEnable - 0/1
 *	
 *	output: none
 *
 * 	return:
 *		NAM_OK- set success
 *		NAM_ERR - set failure
 *		
 ************************************************/
unsigned long nam_enable_device(unsigned char devNum,unsigned char isEnable)
{
    unsigned long rc = NAM_ERR;

	/*code optimize: Unsigned compared against 0
	zhangcl@autelan.com 2013-1-21*/
	if(devNum >31)
	{
		return NAM_ERR;
	}
#ifdef DRV_LIB_CPSS	
	#if 0
	/* 1. Mask the FDB interrupt!
	 * disable the interrupt of FDb for xcat & lion,
	 * Or, once the broadcast into port, the npd will grab the CPU.
	 * zhangdi@autelan.com 2012-01-12 bug: AXSSZFI-722
	 */

	/* Table 903: FDB Interrupt Mask Register Offset: 0x0600001C  */
    unsigned int regAddr = 0x0600001C;		
    /* Disable/Enable TA and AA */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 32, 0x0);
    if (rc != 0)
    {
		nam_syslog_err("nam_enable_device:: failed ! mask the interrupt of FDB error!\n");			
        return rc;
    }
    nam_syslog_dbg("nam_enable_device:: mask the interrupt of FDB dev %d. \n", devNum);		
	#endif
    /*
     * Set the i2c of asic to disable, bugfix: AXSSZFI-829, zhangdi@autelan.com 2012-03-29 
	 * Table 1030: TWSI Global Configuration Register. Offset: 0x00000010, bit[TWSIEn] to 0  
     * APPLICABLE DEVICES:
	 * 98DX275 & 804, XCAT, LION
	 */
    unsigned int regAddr = 0x00000010;		
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 22, 1, 0x0);
    if (rc != NAM_OK)
    {
		nam_syslog_err("nam_enable_device:: disable i2c of asci %d error!\n", devNum);			
        return rc;
    }
	nam_syslog_dbg("nam_enable_device:: disable i2c of asci %d OK!\n", devNum);			
	
	/* 2. Enable the device */
	rc = cpssDxChCfgDevEnable(devNum,isEnable);
    if (rc != NAM_OK)
    {
		nam_syslog_err("nam_enable_device:: %s dev %d error!\n", isEnable?"Enable":"Disable",devNum);			
        return rc;
    }
    nam_syslog_dbg("nam_enable_device:: %s dev %d OK!\n", isEnable?"Enable":"Disable",devNum);		
    return NAM_OK;	
#endif
}
/***********************************************************
 *  nam_mac_match
 * 
 * compare mac address whether equal
 *
 *	input params:
 *		str1- mac address pointer 
 *		str2 -mac address pointer
 *		size - bytes count
 *	
 *	output: none
 *
 * 	return:
 *		 1- if equal
 *		 0- if not equal
 *		
 ************************************************/

int nam_mac_match(unsigned char *str1,unsigned char *str2,int size)
{
	unsigned int i=0;
	for (;size>i;i++){
		if (str1[i]==str2[i])
			continue;
		else { return 0;}
		}
	return 1;
}

int nam_mac_is_multicast(unsigned char *str1)
{
    unsigned int ret = NPD_FALSE;
	if((str1[0]&0x1)&&(0x5e == str1[2])){/*is multicast*/
	    ret = NPD_TRUE;
	}
	return ret;
}

#ifdef DRV_LIB_BCM
void
nam_dump_l2_addr(char *pfx, bcm_l2_addr_t *l2addr)
{
   /* int port;*/
   /* int          count;*/

    
    nam_syslog_dbg("%smac=%02x:%02x:%02x:%02x:%02x:%02x vlan=%d port=%d",
           pfx,
           l2addr->mac[0], l2addr->mac[1], l2addr->mac[2],
           l2addr->mac[3], l2addr->mac[4], l2addr->mac[5],
           l2addr->vid,l2addr->port);


    if (l2addr->flags & BCM_L2_TRUNK_MEMBER) {
        nam_syslog_dbg(" Trunk=%d", l2addr->tgid);
    }

    if (l2addr->flags & BCM_L2_STATIC) {
         nam_syslog_dbg(" Static");
    }

    if (l2addr->flags & BCM_L2_HIT) {
         nam_syslog_dbg(" Hit");
    }

    if (l2addr->cos_src != 0 || l2addr->cos_dst != 0) {
         nam_syslog_dbg(" COS(src=%d,dst=%d)", l2addr->cos_src, l2addr->cos_dst);
    }

    if (l2addr->flags & BCM_L2_COS_SRC_PRI) {
         nam_syslog_dbg(" SCP");
    }

    if (l2addr->flags & BCM_L2_COPY_TO_CPU) {
         nam_syslog_dbg(" CPU");
    }

    if (l2addr->flags & BCM_L2_L3LOOKUP) {
         nam_syslog_dbg(" L3");
    }

    if (l2addr->flags & BCM_L2_DISCARD_SRC) {
         nam_syslog_dbg(" DiscardSrc");
    }

    if (l2addr->flags & BCM_L2_DISCARD_DST) {
         nam_syslog_dbg(" DiscardDest");
    }

    if (l2addr->flags & BCM_L2_MCAST) {
         nam_syslog_dbg(" MCast=%d", l2addr->l2mc_index);
    }
    nam_syslog_dbg("\n");
}
#endif
/*
 * Function: codeby zhengcs@autelan.com at 2009/02/05
 *    nam_l2_addr_show
 * Notes:
 *      SHOW the l2 entry
 */
#ifdef DRV_LIB_BCM
int nam_l2_addr_show(int unit,bcm_l2_addr_t *info, user_data* data) {
	if(NULL != info){
	  if((1 == unit)&& (info->flags & BCM_L2_STATIC)){
           return NAM_E_NONE;
	  }
      memcpy(&(data->l2addr[data->count]),info,sizeof(bcm_l2_addr_t));
	  data->l2addr[data->count].vid = info->vid;		 
	  data->l2addr[data->count].port = info->port;
	  data->l2addr[data->count].tgid = info->tgid;
	  data->l2addr[data->count].flags = info->flags;
	  data->l2addr[data->count].modid = info->modid;
	  if (info->flags & BCM_L2_MCAST) {
	  	data->l2addr[data->count].l2mc_index = info->l2mc_index;
	  }
      data->count++;
   }
#if 0
   syslog_ax_nam_fdb_dbg("info: vid %d mac %02x:%02x:%02x:%02x:%02x:%02x,port %d,modid %d,flags %#x \n",
          info->vid,info->mac[0],info->mac[1],info->mac[2],info->mac[3],info->mac[4],info->mac[5],info->port,info->modid,info->flags);
#endif
   return NAM_E_NONE;
}
#endif

/*************************************
*nam_set_trunk_member_endis
*
*enable/disable all port members in trunk trunkid
*
*INPUT:
*  unsigned short  trunkid
*
*OUTPUT:
*NONE
*
*RETURN:
*  FDB_CONFIG_SUCCESS--if aciton is ok
*
*  FDB_CONFIG_FAIL--if action is failed
*
***************************************/

unsigned int nam_set_trunk_member_endis
(
	unsigned short trunk_no,
	unsigned int   isEnable
)
{
#ifdef DRV_LIB_CPSS
    int ret = 0;
	unsigned int  isBoard = 0,eth_g_index = 0;;
	unsigned char devNum = 0,portNum = 0;
	struct trunk_s *trunkNode = NULL;
	trunk_port_list_s *portList = NULL;
	struct trunk_port_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;
	enum product_id_e productId = npd_query_product_id();

	trunkNode = npd_find_trunk(trunk_no);
	if(NULL == trunkNode) {
		nam_syslog_err("fdb nam:The trunk %d not exist\n",trunk_no);
		return FDB_CONFIG_FAIL;
	}	
	portList = trunkNode->portList;
	if(NULL == portList) {
		nam_syslog_err("fdb nam:The trunk %d has no members\n",trunk_no);
		return FDB_CONFIG_FAIL;
	}
	else if(0 != portList->count) {
		list = &(portList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct trunk_port_list_node_s,list);
			if(NULL != node) {
				eth_g_index = node->eth_global_index;
				ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
				if(NAM_OK != ret){
					syslog_ax_nam_fdb_err("eth_index %#x get asic port failed for ret %d\n",eth_g_index,ret);
					return FDB_CONFIG_FAIL;
				}
				/* set port admin disable*/
				if(((PRODUCT_ID_AX7K == productId)|| (PRODUCT_ID_AX7K_I == productId)) 
					&& (eth_g_index < 4)) {
				   isBoard = NPD_TRUE;
				}
				if(NPD_FALSE == isBoard){
					nam_syslog_dbg("port = %d,devNum = %d\n",portNum,devNum);
					cpssDxChPortEnableSet(devNum,portNum,isEnable);
				}
			}
		}
	}
	return FDB_CONFIG_SUCCESS;
#endif
}


unsigned int nam_fdb_enable_cscd_port_auto_learn_set
(
	unsigned char devNum,
	unsigned char portNum,
	NAM_BOOL state
)
{
	unsigned int  result  = 0;
#ifdef DRV_LIB_CPSS	
	result = cpssDxChBrgFdbPortLearnStatusSet(devNum,portNum,state,CPSS_LOCK_DROP_E);
	if(result != 0){
		nam_syslog_err("nam_fdb_enable_port_auto_learn_set: err!\n");
	}
#endif	
    return result;	
}


unsigned int nam_fdb_enable_port_auto_learn_set(unsigned int index,NAM_BOOL state)
{
	unsigned char devNum  = 0;
	unsigned char portNum = 0;
	unsigned int  result  = 0;
#ifdef DRV_LIB_CPSS	
    npd_get_devport_by_global_index(index,&devNum, &portNum);
	result = cpssDxChBrgFdbPortLearnStatusSet(devNum,portNum,state,CPSS_LOCK_DROP_E);
	if(result != 0){
		nam_syslog_err("nam_fdb_enable_port_auto_learn_set: err!\n");
	}
#endif	
    return result;	
}


/***************************************************
 *  nam_fdb_table_agingtime_set
 * 
 * config fdb table agingtime on hw
 *
 *	input params:
 *		timeout-aging time value
 *	
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/

int nam_fdb_table_agingtime_set(unsigned int timeout) {
#ifdef DRV_LIB_BCM
	int ret = 0,num_of_asic = 0,i = 0;
#endif	
#ifdef DRV_LIB_CPSS
	unsigned char devNum;
    unsigned long	ret;
	devNum = FDB_INIT;
	unsigned int  actDev=0;
    unsigned int  actDevMask=0x1f;
	unsigned char asic_num = 0,num_of_asic = 0;
	int asictype = 0;
#endif

#ifdef DRV_LIB_CPSS
	num_of_asic = nam_asic_get_instance_num();
	for(asic_num;asic_num < num_of_asic;asic_num++ )
	{
		devNum = asic_num;
		if(timeout==0)
		{			
			ret= cpssDxChBrgFdbMacTriggerModeSet( devNum,CPSS_ACT_TRIG_E);
			if(FDB_CONFIG_SUCCESS != ret)
			{
				nam_syslog_err("nam_fdb:cpssDxChBrgFdbMacTriggerModeSet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
				return FDB_CONFIG_FAIL;
			}
			 
			ret = cpssDxChBrgFdbActionModeSet(devNum,CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E);
			if(FDB_CONFIG_SUCCESS != ret)
			{
				nam_syslog_err("nam_fdb:cpssDxChBrgFdbActionModeSet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
				return FDB_CONFIG_FAIL;
			}
		}
		else
		{
			 ret=cpssDxChBrgFdbActionsEnableSet( devNum, NAM_TRUE);
			 if(FDB_CONFIG_SUCCESS != ret)
				{
					nam_syslog_err("nam_fdb:cpssDxChBrgFdbActionsEnableSet:FAIL.\n");
					nam_syslog_err("err vlaue is: %ld\n",ret);
					return FDB_CONFIG_FAIL;
				}
			 
			 ret= cpssDxChBrgFdbActionActiveInterfaceSet( devNum, 0, 0,0,0);
			 if(FDB_CONFIG_SUCCESS != ret)
				{
				   nam_syslog_err("nam_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
					nam_syslog_err("err vlaue is: %d\n",ret);
					return FDB_CONFIG_FAIL;
				}	

			if(IS_DISTRIBUTED == SYSTEM_TYPE)
			{
				ret = nam_get_distributed_devnum(asic_num,&actDev);
				if(ret != FDB_CONFIG_SUCCESS)
				{
					nam_syslog_dbg("get distributed dev num failure !!! \n");
				}

				 ret= cpssDxChBrgFdbActionActiveDevSet( devNum,actDev, actDevMask);
			 	if(FDB_CONFIG_SUCCESS != ret)
			   {
				   nam_syslog_err("nam_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
				   nam_syslog_err("err vlaue is: %d\n",ret);
				   return FDB_CONFIG_FAIL;
			   }	
			}
			else
			{
			
			 	ret= cpssDxChBrgFdbActionActiveDevSet( devNum,0, 0);
				if(FDB_CONFIG_SUCCESS != ret)
			 	{
					nam_syslog_err("nam_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
					nam_syslog_err("err vlaue is: %d\n",ret);
					return FDB_CONFIG_FAIL;
				}	
			}

			ret= cpssDxChBrgFdbActionActiveVlanSet( devNum,0,0);
			if(FDB_CONFIG_SUCCESS != ret)
			{
				nam_syslog_err("nam_fdb:cpssDxChBrgFdbActionActiveVlanSet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
				return FDB_CONFIG_FAIL;
			}

			asictype = nam_asic_get_asic_type(devNum);
			if((asictype < ASIC_TYPE_START_EXMX_E) || (asictype > ASIC_TYPE_LAST_E))
			{
				nam_syslog_err("get asic type fail !\n");
			}
			if(asictype == ASIC_TYPE_DXCH_LION_E)
			{
				ret = cpssDxChBrgFdbActionModeSet(devNum,CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E);
			 	if(FDB_CONFIG_SUCCESS != ret)
				{
					nam_syslog_err("nam_fdb:cpssDxChBrgFdbActionModeSet:FAIL.\n");
					nam_syslog_err("err vlaue is: %d\n",ret);
					return FDB_CONFIG_FAIL;
			 	}
			}
			else
			{
				ret=cpssDxChBrgFdbActionModeSet(devNum,CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E);
				if(FDB_CONFIG_SUCCESS != ret)
				{
					nam_syslog_err("nam_fdb:cpssDxChBrgFdbActionModeSet:FAIL.\n");
					nam_syslog_err("err vlaue is: %ld\n",ret);
					return FDB_CONFIG_FAIL;
				}
			}
			nam_syslog_dbg("timeout = %d\n",timeout);
			ret = cpssDxChBrgFdbAgingTimeoutSet(devNum,timeout);
				
			if(FDB_CONFIG_SUCCESS != ret)
			{
				nam_syslog_err("nam_fdb:cpssDxChBrgFdbAgingTimeoutSet:FAIL.\n");
				nam_syslog_err("err vlaue is: %ld\n",ret);
				return FDB_CONFIG_FAIL;
			}
			ret= cpssDxChBrgFdbMacTriggerModeSet( devNum,CPSS_ACT_AUTO_E);
			if(FDB_CONFIG_SUCCESS != ret)
			{
				nam_syslog_err("nam_fdb:cpssDxChBrgFdbMacTriggerModeSet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
				return FDB_CONFIG_FAIL;
			}

		}
	}

#endif
	
#ifdef DRV_LIB_BCM
	num_of_asic = nam_asic_get_instance_num();
	for(i = 0;i<num_of_asic;i++){
		ret = bcm_l2_age_timer_set(i,timeout);
	    if (ret < 0) {
	        syslog_ax_nam_fdb_dbg("Set fdb aging time %d error!ret %d\n",ret);
	        return FDB_CONFIG_FAIL;
	    }
	}
#endif

	return FDB_CONFIG_SUCCESS;
}

/***************************************************
 * nam_fdb_table_delete_entry_with_vlan_for_debug
 * 
 * delete fdb table items which belong to the vlan on hw
 *
 *	input params:
 *		vlanid-valid vlan index
 *	
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/

int nam_fdb_table_delete_entry_with_vlan_for_debug(unsigned short vlanid)
{
#ifdef DRV_LIB_BCM
    int rv = 0,ret = 0,num_of_asic = 0,i = 0;
#endif

#ifdef DRV_LIB_CPSS
  	unsigned char devNum = 0,portNum = 0;

	unsigned int ret=FDB_INIT;
	NAM_BOOL  ok = NAM_FALSE;
	NAM_BOOL  Atenable = NAM_FALSE;
	devNum = 0;
	unsigned int vlanMask=0xfff;

	unsigned int  actIsTrunk=0;
    unsigned int  actIsTrunkMask=1;

    unsigned int  actTrunkPort=0;
    unsigned int  actTrunkPortMask=0x7f;

	unsigned int  actDev=0;
    unsigned int  actDevMask=0x1f;

	unsigned short  actVlanorg = 0;
	unsigned short  actVlanMaskorg = 0xfff;
	unsigned int u_mbr_count = 0,t_mbr_count = 0,i = 0,j = 0;
	unsigned int  re_flag = 0;
	unsigned int isEnable = 0;
	unsigned char asic_num = 0,num_of_asic = 0;
	unsigned int fdb_state_count = 0;

	CPSS_FDB_ACTION_MODE_ENT  actmode = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;
	CPSS_MAC_ACTION_MODE_ENT  macmode = CPSS_ACT_AUTO_E;

#endif

#ifdef DRV_LIB_CPSS
    /*
          save the pre-configurations before deleting
       */
    num_of_asic = nam_asic_get_instance_num();
	for(asic_num;asic_num < num_of_asic;asic_num++ )
	{
		devNum = asic_num;
	    ret = cpssDxChBrgFdbAAandTAToCpuGet(devNum,&Atenable);
		if(0 != ret){
	         nam_syslog_err("nam_fdb:cpssDxChBrgFdbAAandTATOCpuGet error: %d\n",ret);
	         return FDB_CONFIG_FAIL;
		}

		if(IS_DISTRIBUTED == SYSTEM_TYPE)
		{
			ret = nam_get_distributed_devnum(asic_num,&actDev);
			if(ret != FDB_CONFIG_SUCCESS)
			{
				nam_syslog_dbg("get distributed dev num failure !!! \n");
			}
		}
		ret = cpssDxChBrgFdbActionActiveInterfaceGet(devNum,& actIsTrunk,& actIsTrunkMask,&actTrunkPort,& actTrunkPortMask);
		if(0 != ret){
			 nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceGet:FAIL.\n");
			 nam_syslog_err("err vlaue is: %d\n",ret);
			 return FDB_CONFIG_FAIL;
		}
		ret = cpssDxChBrgFdbActionActiveVlanGet(devNum,&actVlanorg,&actVlanMaskorg);
		if(0 != ret){
			 nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveVlanGet:FAIL.\n");
			 nam_syslog_err("err vlaue is: %d\n",ret);
			 return FDB_CONFIG_FAIL;
		}

		ret = cpssDxChBrgFdbActionModeGet(devNum,&actmode);
		if( 0 != ret){
	           nam_syslog_err("nam_fdb:cpssDxChBrgFdbActionModeGet error %d \n",ret);
	           return FDB_CONFIG_FAIL;
		}

		ret = cpssDxChBrgFdbMacTriggerModeGet(devNum,&macmode);
		if( 0 != ret){
	           nam_syslog_err("nam_fdb:  cpssDxChBrgFdbMacTriggerModeGet: error %d\n",ret);
	           return FDB_CONFIG_FAIL;
		}

		ret=cpssDxChBrgFdbActionsEnableSet( devNum, NAM_FALSE);
		if(0 != ret){
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionsEnableSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %ld\n",ret);
			re_flag++;
			goto isflag;
		}


		ret=cpssDxChBrgFdbAAandTAToCpuSet(devNum,NAM_FALSE);
		if(0 != ret){
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbAAandTAToCpuSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			re_flag++;
	       goto isflag;
		}

		ret= cpssDxChBrgFdbMacTriggerModeSet( devNum,CPSS_ACT_TRIG_E);
		if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbMacTriggerModeSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			goto isflag;
		}

		ret= cpssDxChBrgFdbStaticDelEnable( devNum,NAM_FALSE);
		if(0 != ret){
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbStaticDelEnable:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			re_flag++;
			goto isflag;

		}

		ret= cpssDxChBrgFdbActionActiveInterfaceSet( devNum, 0, 0,0,0);
		if(0 != ret){
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			re_flag++;
			goto isflag;

		}	
		
		ret= cpssDxChBrgFdbActionActiveDevSet( devNum,0, 0);
		if(0 != ret){
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			re_flag++;
			goto isflag;

		}	
	  
		ret= cpssDxChBrgFdbActionActiveVlanSet( devNum,vlanid,vlanMask);
		if(0 != ret){
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveVlanSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			re_flag++;
			goto isflag;

	    }
		
		pthread_mutex_lock(&FdbActMutex);		 
		ret=cpssDxChBrgFdbTrigActionStart( devNum, CPSS_FDB_ACTION_DELETING_E);
		pthread_mutex_unlock(&FdbActMutex);	
		if(0 != ret){
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStart:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret); 
			re_flag++;
			goto isflag;
	    }
		while(fdb_state_count<=6)
		{
			ret = cpssDxChBrgFdbTrigActionStatusGet(devNum,&ok);
			if(ret != 0)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStatusGet:FAIL!\n");
				fdb_state_count++;
				continue;
			}
			else if(0 == ok)
			{
				nam_syslog_err("delete fdb action did not finished !\n");
				osTimerWkAfter(100);
				fdb_state_count++;
				continue;
			}
			else if(1 == ok)
			{
				nam_syslog_dbg("delete fdb action finished !\n");
				break;
			}
			fdb_state_count++;
		}
		if(ok == 0)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStatusGet:delete failed!ok value %d\n",ok);
			re_flag++;
			goto isflag;
		}

		/*
		   ACTION RESTORE
		*/
	   isflag:   
		
		ret=cpssDxChBrgFdbActionsEnableSet( devNum, NAM_TRUE);
		if(0 != ret)
				{
					nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionsEnableSet:FAIL.\n");
					nam_syslog_err("err vlaue is: %ld\n",ret);
					re_flag++;
				}

		
		ret=cpssDxChBrgFdbActionModeSet( devNum, actmode);
		if(0 != ret)
				{
					nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionModeSet:FAIL.\n");
					nam_syslog_err("err vlaue is: %d\n",ret); 
					re_flag++;
			    }


		ret= cpssDxChBrgFdbMacTriggerModeSet( devNum,macmode);
		if(0 != ret)
				{
					nam_syslog_err("npd_fdb:cpssDxChBrgFdbMacTriggerModeSet:FAIL.\n");
					nam_syslog_err("err vlaue is: %d\n",ret);
					re_flag++;
				}

		ret = cpssDxChBrgFdbActionActiveVlanSet(devNum,actVlanorg,actVlanMaskorg);
		if(0 != ret)
				{
					nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveVlanSet:FAIL.\n");
					nam_syslog_err("err vlaue is: %d\n",ret);
					re_flag++;
				}

	   
		ret= cpssDxChBrgFdbActionActiveInterfaceSet( devNum, actIsTrunk,  actIsTrunkMask,actTrunkPort,actTrunkPortMask);
		if(0 != ret)
				{
					nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
					nam_syslog_err("err vlaue is: %d\n",ret);
					re_flag++;
				}	
		
		ret= cpssDxChBrgFdbActionActiveDevSet( devNum, actDev, actDevMask);
		if(0 != ret)
				{
					nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
					nam_syslog_err("err vlaue is: %d\n",ret);
					re_flag++;
				}
		
		ret=cpssDxChBrgFdbAAandTAToCpuSet(devNum,Atenable);
		if(0 != ret)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbAAandTAToCpuSet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
				re_flag++;
			}
		if(re_flag)
		{
			nam_syslog_err("delete fdb for dev %d vlan %d err !!!\n",asic_num,vlanid);
			break;
		}
	}
	if(!re_flag)
	   return FDB_CONFIG_SUCCESS;
	else 
	   return FDB_CONFIG_FAIL;
#endif
	
#ifdef DRV_LIB_BCM
	num_of_asic = nam_asic_get_instance_num();
	for(i = 0;i<num_of_asic;i++){
       ret = bcm_l2_addr_delete_by_vlan(i,vlanid,0);
	   if(NAM_OK != ret){
         syslog_ax_nam_fdb_err("unit %d vlan %d delete fdb failed for ret %d\n",i,vlanid,ret);
	   }
	   NAM_RV_REPLACE_OK(ret, rv, NAM_E_UNAVAIL);

	}
	return rv;
#endif

}
/***************************************************
 * nam_fdb_table_delete_entry_with_vlan
 * 
 * delete fdb table items which belong to the vlan on hw
 *
 *	input params:
 *		vlanid-valid vlan index
 *	
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/

int nam_fdb_table_delete_entry_with_vlan(unsigned short vlanid)
{
#ifdef DRV_LIB_BCM
    int rv = 0,ret = 0,num_of_asic = 0,i = 0;
#endif

#ifdef DRV_LIB_CPSS
  	unsigned char devNum = 0,portNum = 0;

	unsigned int ret=FDB_INIT;
	NAM_BOOL  ok = NAM_FALSE;
	NAM_BOOL  Atenable = NAM_FALSE;
	devNum = 0;
	unsigned int vlanMask=0xfff;

	unsigned int  actIsTrunk=0;
    unsigned int  actIsTrunkMask=1;

    unsigned int  actTrunkPort=0;
    unsigned int  actTrunkPortMask=0x7f;

	unsigned int  actDev=0;
    unsigned int  actDevMask=0x1f;

	unsigned short  actVlanorg = 0;
	unsigned short  actVlanMaskorg = 0xfff;
	unsigned int u_mbr_count = 0,t_mbr_count = 0,i = 0,j = 0;
	unsigned int  re_flag = 0;
	unsigned int isEnable = 0;
	unsigned char asic_num = 0,num_of_asic = 0;
	unsigned int fdb_state_count = 0;

	CPSS_FDB_ACTION_MODE_ENT  actmode = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;
	CPSS_MAC_ACTION_MODE_ENT  macmode = CPSS_ACT_AUTO_E;

#endif

#ifdef DRV_LIB_CPSS
    /*
          save the pre-configurations before deleting
       */
    num_of_asic = nam_asic_get_instance_num();
	for(asic_num;asic_num < num_of_asic;asic_num++ )
	{
		devNum = asic_num;
	    ret = cpssDxChBrgFdbAAandTAToCpuGet(devNum,&Atenable);
		if(0 != ret)
		{
	         nam_syslog_err("nam_fdb:cpssDxChBrgFdbAAandTATOCpuGet error: %d\n",ret);
	         return FDB_CONFIG_FAIL;
		}

		if(IS_DISTRIBUTED == SYSTEM_TYPE)
		{
			ret = nam_get_distributed_devnum(asic_num,&actDev);
			if(ret != FDB_CONFIG_SUCCESS)
			{
				nam_syslog_dbg("get distributed dev num failure !!! \n");
			}
		}
		ret = cpssDxChBrgFdbActionActiveInterfaceGet(devNum,& actIsTrunk,& actIsTrunkMask,&actTrunkPort,& actTrunkPortMask);
		if(0 != ret)
		{
			 nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceGet:FAIL.\n");
			 nam_syslog_err("err vlaue is: %d\n",ret);
			 return FDB_CONFIG_FAIL;
		}
		ret = cpssDxChBrgFdbActionActiveVlanGet(devNum,&actVlanorg,&actVlanMaskorg);
		if(0 != ret)
		{
			 nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveVlanGet:FAIL.\n");
			 nam_syslog_err("err vlaue is: %d\n",ret);
			 return FDB_CONFIG_FAIL;
		}

		ret = cpssDxChBrgFdbActionModeGet(devNum,&actmode);
		if( 0 != ret)
		{
	           nam_syslog_err("nam_fdb:cpssDxChBrgFdbActionModeGet error %d \n",ret);
	           return FDB_CONFIG_FAIL;
		}

		ret = cpssDxChBrgFdbMacTriggerModeGet(devNum,&macmode);
		if( 0 != ret)
		{
	           nam_syslog_err("nam_fdb:  cpssDxChBrgFdbMacTriggerModeGet: error %d\n",ret);
	           return FDB_CONFIG_FAIL;
		}
		ret=cpssDxChBrgFdbActionsEnableSet( devNum, NAM_FALSE);
		if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionsEnableSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %ld\n",ret);
			re_flag++;
			goto isflag;
		}


		ret=cpssDxChBrgFdbAAandTAToCpuSet(devNum,NAM_FALSE);
		if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbAAandTAToCpuSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			re_flag++;
	       goto isflag;
		}

		ret= cpssDxChBrgFdbStaticDelEnable( devNum,NAM_FALSE);
		if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbStaticDelEnable:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			re_flag++;
			goto isflag;

		}

		ret= cpssDxChBrgFdbMacTriggerModeSet( devNum,CPSS_ACT_TRIG_E);
		if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbMacTriggerModeSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			goto isflag;
		}

		ret= cpssDxChBrgFdbActionActiveInterfaceSet( devNum, 0, 0,0,0);
		if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			re_flag++;
			goto isflag;

		}	
		
		ret= cpssDxChBrgFdbActionActiveDevSet( devNum,0, 0);
		if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			re_flag++;
			goto isflag;

		}	
	  
		ret= cpssDxChBrgFdbActionActiveVlanSet( devNum,vlanid,vlanMask);
		if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveVlanSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			re_flag++;
			goto isflag;

	    }
		
		pthread_mutex_lock(&FdbActMutex);		 
		ret=cpssDxChBrgFdbTrigActionStart( devNum, CPSS_FDB_ACTION_DELETING_E);
		pthread_mutex_unlock(&FdbActMutex);	
		if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStart:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret); 
			re_flag++;
			goto isflag;
	    }
		while(fdb_state_count<=6)
		{
			ret = cpssDxChBrgFdbTrigActionStatusGet(devNum,&ok);
			if(ret != 0)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStatusGet:FAIL!\n");
				fdb_state_count++;
				continue;
			}
			else if(0 == ok)
			{
				nam_syslog_err("delete fdb action did not finished !\n");
				osTimerWkAfter(100);
				fdb_state_count++;
				continue;
			}
			else if(1 == ok)
			{
				nam_syslog_dbg("delete fdb action finished !\n");
				break;
			}
			fdb_state_count++;
		}
		if(ok == 0)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStatusGet:delete failed!ok value %d\n",ok);
			re_flag++;
			goto isflag;
		}
		isflag:
		ret=cpssDxChBrgFdbActionsEnableSet( devNum, NAM_TRUE);
		if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionsEnableSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %ld\n",ret);
			re_flag++;
		}

		
		ret=cpssDxChBrgFdbActionModeSet( devNum, actmode);
		if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionModeSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret); 
			re_flag++;
	    }


		ret= cpssDxChBrgFdbMacTriggerModeSet( devNum,macmode);
		if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbMacTriggerModeSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			re_flag++;
		}

		ret = cpssDxChBrgFdbActionActiveVlanSet(devNum,actVlanorg,actVlanMaskorg);
		if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveVlanSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			re_flag++;
		}

	   
		ret= cpssDxChBrgFdbActionActiveInterfaceSet( devNum, actIsTrunk,  actIsTrunkMask,actTrunkPort,actTrunkPortMask);
		if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			re_flag++;
		}	
		
		ret= cpssDxChBrgFdbActionActiveDevSet( devNum, actDev, actDevMask);
		if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			re_flag++;
		}
		
		ret=cpssDxChBrgFdbAAandTAToCpuSet(devNum,Atenable);
		if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbAAandTAToCpuSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			re_flag++;
		}
		if(re_flag)
		{
			nam_syslog_err("delete fdb for dev %d vlan %d err !!!\n",asic_num,vlanid);
			break;
		}
	}
	if(!re_flag)
	   return FDB_CONFIG_SUCCESS;
	else 
	   return FDB_CONFIG_FAIL;
#endif
	
#ifdef DRV_LIB_BCM
	num_of_asic = nam_asic_get_instance_num();
	for(i = 0;i<num_of_asic;i++){
       ret = bcm_l2_addr_delete_by_vlan(i,vlanid,0);
	   if(NAM_OK != ret){
         syslog_ax_nam_fdb_err("unit %d vlan %d delete fdb failed for ret %d\n",i,vlanid,ret);
	   }
	   NAM_RV_REPLACE_OK(ret, rv, NAM_E_UNAVAIL);

	}
	return rv;
#endif

}
	
/***************************************************
 * nam_fdb_table_delete_entry_with_port
 * 
 * delete fdb table items which belong to the port on hw
 *
 *	input params:
 *		index-eth_g_ index,which is from slot/port
 *	
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/

int nam_fdb_table_delete_entry_with_port(unsigned int index)
{
#ifdef DRV_LIB_BCM
    int rv = 0,ret = 0,mod = 0,i = 0,num_of_asic = 0;
	unsigned char unit = 0,port = 0;
#endif

#ifdef DRV_LIB_CPSS
  	unsigned char devNum = 0;
	unsigned char portNum = 0;

	unsigned int slot_index = 0,slot_no = 0,port_index = 0,port_no = 0;
	unsigned int isEnable = 0;
	unsigned int ret=FDB_INIT;
	devNum = 0;
	NAM_BOOL  ok = NAM_FALSE;
	NAM_BOOL  Atenable = NAM_FALSE,learn = NAM_FALSE;
	unsigned int  actIsTrunk=0;
    unsigned int  actIsTrunkMask=1;
	unsigned int  actIsTrunkorg=0;
    unsigned int  actIsTrunkMaskorg=1;
    unsigned int  actTrunkPort=0;
    unsigned int  actTrunkPortMask=0x7f;
	unsigned int  actTrunkPortorg=0;
    unsigned int  actTrunkPortMaskorg=0x7f;
	unsigned int  actDev=0;
    unsigned int  actDevMask=0x1f;
	unsigned int  actDevorg=0;
    unsigned int  actDevMaskorg=0x1f;
	unsigned short  actVlan = 0;
	unsigned short  actVlanMask = 0;
	unsigned int  re_flag = 0;
	enum product_id_e productId = npd_query_product_id();
	unsigned int isBoard = 0,linkState = 0;
	unsigned char asic_num = 0,ammount_asic_nums = 0;
	unsigned int fdb_state_count = 0;

	CPSS_MAC_ACTION_MODE_ENT  macmode = CPSS_ACT_AUTO_E;
	CPSS_FDB_ACTION_MODE_ENT  actmode = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;
	CPSS_PORT_LOCK_CMD_ENT    learncmd = CPSS_LOCK_FRWRD_E;
#endif

#ifdef DRV_LIB_CPSS

	ret = npd_get_devport_by_global_index(index,&devNum, &portNum);
    if(NAM_OK != ret){
		syslog_ax_nam_fdb_err("eth_index %#x get asic port failed for ret %d\n",index,ret);
        return FDB_CONFIG_FAIL;
	}	
	actDev = devNum;
	actTrunkPort=portNum;

    slot_index = npd_get_slot_index_from_eth_index(index);
	slot_no = npd_get_slot_no_from_index(slot_index);
	port_index = npd_get_port_index_from_eth_index(index);
	port_no = npd_get_port_no_from_index(slot_index,port_index);

	if(IS_DISTRIBUTED == SYSTEM_TYPE)   /*slove bug  AXSSZFI-170	*/
	{
		ret = nam_get_distributed_devnum(asic_num,&actDev);
		if(ret != FDB_CONFIG_SUCCESS)
		{
			nam_syslog_dbg("get distributed dev num failure !!! \n");
		}
	}
   /*
         save the pre-configuration before action
      */
      
   pthread_mutex_lock(&FdbActMutex);
   
   ret = cpssDxChBrgFdbAAandTAToCpuGet(devNum,&Atenable);
   if( 0 != ret ){
         nam_syslog_err("nam_fdb:cpssDxChBrgFdbAAandTAToCpuGet error: %d\n",ret);
         pthread_mutex_unlock(&FdbActMutex);
		 return FDB_CONFIG_FAIL;
   }
#if 0
   ret = cpssDxChBrgFdbPortLearnStatusGet(devNum,portNum,&learn,&learncmd);
   if( 0 != ret ){
         nam_syslog_err("nam_fdb: cpssDxChBrgFdbPortLearnStatusGet error: %d\n",ret);
		 return FDB_CONFIG_FAIL;
   }
#endif
   ret = cpssDxChBrgFdbActionModeGet(devNum,&actmode);
   if( 0 != ret){
         nam_syslog_err("nam_fdb: cpssDxChBrgFdbActionModeGet error: %d\n",ret);
         nam_syslog_err("Now the CPSS_FDB_ACTION_MODE_ENT value is %d\n",actmode);
         pthread_mutex_unlock(&FdbActMutex);
		 return FDB_CONFIG_FAIL;
   }
   ret = cpssDxChBrgFdbMacTriggerModeGet(devNum,&macmode);
   if(0 != ret){
         nam_syslog_err("nam_fdb: cpssDxChBrgFdbMacTriggerModeGet error :%d\n",ret);
         nam_syslog_err("Now the CPSS_MAC_ACTION_MODE_ENT value is %d\n",macmode);
         pthread_mutex_unlock(&FdbActMutex);
		 return FDB_CONFIG_FAIL;
   }
   ret = cpssDxChBrgFdbActionActiveDevGet( devNum,&actDevorg,&actDevMaskorg);
   	if(0 != ret)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveDevGet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
                pthread_mutex_unlock(&FdbActMutex);
				return FDB_CONFIG_FAIL;
			}
	if(IS_DISTRIBUTED == SYSTEM_TYPE)
	{
		ret = nam_get_distributed_devnum(asic_num,&actDev);
		if(ret != FDB_CONFIG_SUCCESS)
		{
			nam_syslog_dbg("get distributed dev num failure !!! \n");
		}
	}
    ret = cpssDxChBrgFdbActionActiveInterfaceGet(devNum,&actIsTrunkorg,&actIsTrunkMaskorg,&actTrunkPortorg,&actTrunkPortMaskorg);
    if(0 != ret)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceGet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
                pthread_mutex_unlock(&FdbActMutex);
				return FDB_CONFIG_FAIL;
			}

    ret = cpssDxChBrgFdbActionActiveVlanGet(devNum,&actVlan,& actVlanMask);
    if(0 != ret)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveVlanGet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);                
                pthread_mutex_unlock(&FdbActMutex);
				return FDB_CONFIG_FAIL;
			}
	ret = cpssDxChPortEnableGet(devNum,portNum,&linkState);
	if(0 != ret){
          nam_syslog_err("nam_fdb:get link state error!ret %d\n",ret);          
          pthread_mutex_unlock(&FdbActMutex);
		  return FDB_CONFIG_FAIL;
	}


   /*
       ACTION SET
    */
   /*first set port admin disable*/
   if(((PRODUCT_ID_AX7K == productId)||(PRODUCT_ID_AX7K_I == productId))
   		&& (index < 4)) {
	  isBoard = NPD_TRUE;
   }
   if(NPD_FALSE == isBoard){
	   isEnable = 0;
	   ret = cpssDxChPortEnableSet(devNum,portNum,isEnable);
	   osTimerWkAfter(1);/*gave time to stop the packet stream*/
   }

   
    /*
          disable action first to avoid auto aging
       */
     ret=cpssDxChBrgFdbActionsEnableSet( devNum, NAM_FALSE);
	if(0 != ret){
		 nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionsEnableSet:FAIL.\n");
		 nam_syslog_err("err vlaue is: %ld\n",ret);
		 re_flag++;
		 goto isenable;
	}

	
    ret=cpssDxChBrgFdbAAandTAToCpuSet(devNum,NAM_FALSE);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbAAandTAToCpuSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
		 goto isenable;
	}
	ret= cpssDxChBrgFdbMacTriggerModeSet( devNum,CPSS_ACT_TRIG_E);
	if(0 != ret)
	{
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbMacTriggerModeSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		goto isenable;
	}
#if 0
    ret= cpssDxChBrgFdbPortLearnStatusSet(devNum,portNum,0,CPSS_LOCK_DROP_E);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbPortLearnStatusSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
		 goto isenable;
	}
#endif
	ret= cpssDxChBrgFdbActionActiveVlanSet( devNum,0,0);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveVlanSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
		 goto isenable;
	}

	/* disable FDB static del*/
	ret= cpssDxChBrgFdbStaticDelEnable( devNum,NAM_FALSE);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbStaticDelEnable:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;/*set reflag value to identifier that the return value is fail*/
		goto isenable;
	}

	ret= cpssDxChBrgFdbActionActiveInterfaceSet( devNum, actIsTrunk,  actIsTrunkMask,actTrunkPort,actTrunkPortMask);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
		 goto isenable;
	}	
	ret= cpssDxChBrgFdbActionActiveDevSet( devNum, actDev, actDevMask);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
		 goto isenable;
	}	
	
	ret=cpssDxChBrgFdbTrigActionStart( devNum, CPSS_FDB_ACTION_DELETING_E);
	
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStart1:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret); 
		re_flag++;
		 goto isenable;
		/*if call failed ,do not return*/
    }
	while(fdb_state_count <= 6)
	{
		ret = cpssDxChBrgFdbTrigActionStatusGet(devNum,&ok);
		if(ret != 0)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStatusGet:FAIL!\n");
			fdb_state_count++;
			continue;
		}
		else if(0 == ok)
		{
			nam_syslog_err("delete fdb action did not finished !\n");
			osTimerWkAfter(100);
			fdb_state_count++;
			continue;
		}
		else if(1 == ok)
		{
			nam_syslog_dbg("delete fdb action finished !\n");
			break;
		}
		fdb_state_count++;
	}
	if(ok == 0)
	{
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStatusGet:delete failed!ok value %d\n",ok);
		re_flag++;
		goto isenable;
	}
    /*reset the fdb limit if set*/
	npd_fdb_reset_fdbCount_by_port(slot_no,port_no);
    /*
         RESTORE THE CONFIGURATION
      */
         /*enable the port */
  isenable:
  
	if(((PRODUCT_ID_AX7K == productId)||(PRODUCT_ID_AX7K_I == productId))
		&& (index < 4)) {
	   isBoard = NPD_TRUE;
	}
	if(NPD_FALSE == isBoard){
		ret = cpssDxChPortEnableSet(devNum,portNum,linkState);
	}
	
	ret=cpssDxChBrgFdbActionsEnableSet( devNum, NAM_TRUE);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionsEnableSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %ld\n",ret);
		re_flag++;
	}
		
	ret=cpssDxChBrgFdbActionModeSet( devNum, actmode);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionModeSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret); 
		re_flag++;
	}

	ret= cpssDxChBrgFdbMacTriggerModeSet( devNum,macmode);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbMacTriggerModeSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
	}


	ret = cpssDxChBrgFdbActionActiveVlanSet(devNum,actVlan,actVlanMask);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveVlanSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
	}


	ret= cpssDxChBrgFdbActionActiveInterfaceSet( devNum, actIsTrunkorg,  actIsTrunkMaskorg,actTrunkPortorg,actTrunkPortMaskorg);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
	}	

	ret= cpssDxChBrgFdbActionActiveDevSet( devNum, actDevorg, actDevMaskorg);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
	}
	
	ret=cpssDxChBrgFdbAAandTAToCpuSet(devNum,Atenable);/*enable the AA*/
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbAAandTAToCpuSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
	}
#if 0
	ret=cpssDxChBrgFdbPortLearnStatusSet(devNum,portNum,learn,learncmd);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbPortLearnStatusSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
	}
#endif
    pthread_mutex_unlock(&FdbActMutex);

    if(re_flag){
        return FDB_CONFIG_FAIL;
	}
	else{
	    return FDB_CONFIG_SUCCESS;
	}
#endif

#ifdef DRV_LIB_BCM
    ret = npd_get_devport_by_global_index(index,&unit,&port);
    if(NAM_OK != ret){
		syslog_ax_nam_fdb_err("eth_index %#x get asic port failed for ret %d\n",index,ret);
        return FDB_CONFIG_FAIL;
	}
	ret = bcm_stk_modid_get(unit,&mod);	/*the mod get according to the unit*/
	if(NAM_OK != ret){
       syslog_ax_nam_fdb_err("get mod-id %d in device %d error, ret %d\n", mod, unit, ret);
	   return FDB_CONFIG_FAIL;
	}
    num_of_asic = nam_asic_get_instance_num();
	for(i = 0;i < num_of_asic;i++){
		ret = bcm_l2_addr_delete_by_port(i,mod,port,0);
		if(NAM_OK != ret){
	       syslog_ax_nam_fdb_err("delete fdb eth-port %#x asic port(%d,%d) failed in hw for ret %d!\n",index,i,port,ret);
		}
		NAM_RV_REPLACE_OK(ret, rv, NAM_E_UNAVAIL);
	}
	return rv;
#endif
}


int nam_fdb_table_delete_entry_with_dev_port(unsigned char devnum,unsigned char portnum)
{
#ifdef DRV_LIB_BCM
    int rv = 0,ret = 0,mod = 0,i = 0,num_of_asic = 0;
	unsigned char unit = 0,port = 0;
#endif

#ifdef DRV_LIB_CPSS
	unsigned int isEnable = 0;
	unsigned int ret=FDB_INIT;
	NAM_BOOL  ok = NAM_FALSE;
	NAM_BOOL  Atenable = NAM_FALSE,learn = NAM_FALSE;
	unsigned int  actIsTrunk=0;
    unsigned int  actIsTrunkMask=1;
	unsigned int  actIsTrunkorg=0;
    unsigned int  actIsTrunkMaskorg=1;
    unsigned int  actTrunkPort=0;
    unsigned int  actTrunkPortMask=0x7f;
	unsigned int  actTrunkPortorg=0;
    unsigned int  actTrunkPortMaskorg=0x7f;
	unsigned int  actDev=0;
    unsigned int  actDevMask=0x1f;
	unsigned int  actDevorg=0;
    unsigned int  actDevMaskorg=0x1f;
	unsigned short  actVlan = 0;
	unsigned short  actVlanMask = 0;
	unsigned int  re_flag = 0;
	enum product_id_e productId = npd_query_product_id();
	unsigned int isBoard = 0,linkState = 0;
	unsigned char asic_num = 0,ammount_asic_nums = 0;
	unsigned int fdb_state_count = 0;

	CPSS_MAC_ACTION_MODE_ENT  macmode = CPSS_ACT_AUTO_E;
	CPSS_FDB_ACTION_MODE_ENT  actmode = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;
	CPSS_PORT_LOCK_CMD_ENT    learncmd = CPSS_LOCK_FRWRD_E;
#endif

#ifdef DRV_LIB_CPSS

	actDev = devnum;
	actTrunkPort=portnum;
	if(IS_DISTRIBUTED == SYSTEM_TYPE)   /*slove bug  AXSSZFI-170	*/
	{
		ret = nam_get_distributed_devnum(asic_num,&actDev);
		if(ret != FDB_CONFIG_SUCCESS)
		{
			nam_syslog_dbg("get distributed dev num failure !!! \n");
		}
	}
   /*
         save the pre-configuration before action
      */
      
   pthread_mutex_lock(&FdbActMutex);
   
   ret = cpssDxChBrgFdbAAandTAToCpuGet(devnum,&Atenable);
   if( 0 != ret ){
         nam_syslog_err("nam_fdb:cpssDxChBrgFdbAAandTAToCpuGet error: %d\n",ret);
         pthread_mutex_unlock(&FdbActMutex);
		 return FDB_CONFIG_FAIL;
   }
#if 0
   ret = cpssDxChBrgFdbPortLearnStatusGet(devNum,portNum,&learn,&learncmd);
   if( 0 != ret ){
         nam_syslog_err("nam_fdb: cpssDxChBrgFdbPortLearnStatusGet error: %d\n",ret);
		 return FDB_CONFIG_FAIL;
   }
#endif
   ret = cpssDxChBrgFdbActionModeGet(devnum,&actmode);
   if( 0 != ret){
         nam_syslog_err("nam_fdb: cpssDxChBrgFdbActionModeGet error: %d\n",ret);
         nam_syslog_err("Now the CPSS_FDB_ACTION_MODE_ENT value is %d\n",actmode);
         pthread_mutex_unlock(&FdbActMutex);
		 return FDB_CONFIG_FAIL;
   }
   ret = cpssDxChBrgFdbMacTriggerModeGet(devnum,&macmode);
   if(0 != ret){
         nam_syslog_err("nam_fdb: cpssDxChBrgFdbMacTriggerModeGet error :%d\n",ret);
         nam_syslog_err("Now the CPSS_MAC_ACTION_MODE_ENT value is %d\n",macmode);
         pthread_mutex_unlock(&FdbActMutex);
		 return FDB_CONFIG_FAIL;
   }
   ret = cpssDxChBrgFdbActionActiveDevGet( devnum,&actDevorg,&actDevMaskorg);
   	if(0 != ret)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveDevGet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
                pthread_mutex_unlock(&FdbActMutex);
				return FDB_CONFIG_FAIL;
			}
	if(IS_DISTRIBUTED == SYSTEM_TYPE)
	{
		ret = nam_get_distributed_devnum(asic_num,&actDev);
		if(ret != FDB_CONFIG_SUCCESS)
		{
			nam_syslog_dbg("get distributed dev num failure !!! \n");
		}
	}
    ret = cpssDxChBrgFdbActionActiveInterfaceGet(devnum,&actIsTrunkorg,&actIsTrunkMaskorg,&actTrunkPortorg,&actTrunkPortMaskorg);
    if(0 != ret)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceGet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
                pthread_mutex_unlock(&FdbActMutex);
				return FDB_CONFIG_FAIL;
			}

    ret = cpssDxChBrgFdbActionActiveVlanGet(devnum,&actVlan,& actVlanMask);
    if(0 != ret)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveVlanGet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);                
                pthread_mutex_unlock(&FdbActMutex);
				return FDB_CONFIG_FAIL;
			}
	ret = cpssDxChPortEnableGet(devnum,portnum,&linkState);
	if(0 != ret){
          nam_syslog_err("nam_fdb:get link state error!ret %d\n",ret);          
          pthread_mutex_unlock(&FdbActMutex);
		  return FDB_CONFIG_FAIL;
	}


   /*
       ACTION SET
    */
   /*first set port admin disable*/
   if(((PRODUCT_ID_AX7K == productId)||(PRODUCT_ID_AX7K_I == productId))
   		&& (index < 4)) {
	  isBoard = NPD_TRUE;
   }
   if(NPD_FALSE == isBoard){
	   isEnable = 0;
	   ret = cpssDxChPortEnableSet(devnum,portnum,isEnable);
	   osTimerWkAfter(1);/*gave time to stop the packet stream*/
   }

   
    /*
          disable action first to avoid auto aging
       */
     ret=cpssDxChBrgFdbActionsEnableSet( devnum, NAM_FALSE);
	if(0 != ret){
		 nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionsEnableSet:FAIL.\n");
		 nam_syslog_err("err vlaue is: %ld\n",ret);
		 re_flag++;
		 goto isenable;
	}

	
    ret=cpssDxChBrgFdbAAandTAToCpuSet(devnum,NAM_FALSE);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbAAandTAToCpuSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
		 goto isenable;
	}
	ret= cpssDxChBrgFdbMacTriggerModeSet( devnum,CPSS_ACT_TRIG_E);
	if(0 != ret)
	{
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbMacTriggerModeSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		goto isenable;
	}
#if 0
    ret= cpssDxChBrgFdbPortLearnStatusSet(devNum,portNum,0,CPSS_LOCK_DROP_E);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbPortLearnStatusSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
		 goto isenable;
	}
#endif
	ret= cpssDxChBrgFdbActionActiveVlanSet( devnum,0,0);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveVlanSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
		 goto isenable;
	}

	/* disable FDB static del*/
	ret= cpssDxChBrgFdbStaticDelEnable( devnum,NAM_FALSE);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbStaticDelEnable:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;/*set reflag value to identifier that the return value is fail*/
		goto isenable;
	}

	ret= cpssDxChBrgFdbActionActiveInterfaceSet( devnum, actIsTrunk,  actIsTrunkMask,actTrunkPort,actTrunkPortMask);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
		 goto isenable;
	}	
	ret= cpssDxChBrgFdbActionActiveDevSet( devnum, actDev, actDevMask);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
		 goto isenable;
	}	
	
	ret=cpssDxChBrgFdbTrigActionStart( devnum, CPSS_FDB_ACTION_DELETING_E);
	
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStart1:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret); 
		re_flag++;
		 goto isenable;
		/*if call failed ,do not return*/
    }
	while(fdb_state_count <= 6)
	{
		ret = cpssDxChBrgFdbTrigActionStatusGet(devnum,&ok);
		if(ret != 0)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStatusGet:FAIL!\n");
			fdb_state_count++;
			continue;
		}
		else if(0 == ok)
		{
			nam_syslog_err("delete fdb action did not finished !\n");
			osTimerWkAfter(100);
			fdb_state_count++;
			continue;
		}
		else if(1 == ok)
		{
			nam_syslog_dbg("delete fdb action finished !\n");
			break;
		}
		fdb_state_count++;
	}
	if(ok == 0)
	{
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStatusGet:delete failed!ok value %d\n",ok);
		re_flag++;
		goto isenable;
	}
         /*enable the port */
  isenable:
  
	if(((PRODUCT_ID_AX7K == productId)||(PRODUCT_ID_AX7K_I == productId))
		&& (index < 4)) {
	   isBoard = NPD_TRUE;
	}
	if(NPD_FALSE == isBoard){
		ret = cpssDxChPortEnableSet(devnum,portnum,linkState);
	}
	
	ret=cpssDxChBrgFdbActionsEnableSet( devnum, NAM_TRUE);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionsEnableSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %ld\n",ret);
		re_flag++;
	}
		
	ret=cpssDxChBrgFdbActionModeSet( devnum, actmode);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionModeSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret); 
		re_flag++;
	}

	ret= cpssDxChBrgFdbMacTriggerModeSet( devnum,macmode);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbMacTriggerModeSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
	}


	ret = cpssDxChBrgFdbActionActiveVlanSet(devnum,actVlan,actVlanMask);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveVlanSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
	}


	ret= cpssDxChBrgFdbActionActiveInterfaceSet( devnum, actIsTrunkorg,  actIsTrunkMaskorg,actTrunkPortorg,actTrunkPortMaskorg);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
	}	

	ret= cpssDxChBrgFdbActionActiveDevSet( devnum, actDevorg, actDevMaskorg);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
	}
	
	ret=cpssDxChBrgFdbAAandTAToCpuSet(devnum,Atenable);/*enable the AA*/
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbAAandTAToCpuSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
	}
#if 0
	ret=cpssDxChBrgFdbPortLearnStatusSet(devNum,portNum,learn,learncmd);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbPortLearnStatusSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
	}
#endif
    pthread_mutex_unlock(&FdbActMutex);

    if(re_flag){
        return FDB_CONFIG_FAIL;
	}
	else{
	    return FDB_CONFIG_SUCCESS;
	}
#endif

#ifdef DRV_LIB_BCM
    ret = npd_get_devport_by_global_index(index,&unit,&port);
    if(NAM_OK != ret){
		syslog_ax_nam_fdb_err("eth_index %#x get asic port failed for ret %d\n",index,ret);
        return FDB_CONFIG_FAIL;
	}
	ret = bcm_stk_modid_get(unit,&mod);	/*the mod get according to the unit*/
	if(NAM_OK != ret){
       syslog_ax_nam_fdb_err("get mod-id %d in device %d error, ret %d\n", mod, unit, ret);
	   return FDB_CONFIG_FAIL;
	}
    num_of_asic = nam_asic_get_instance_num();
	for(i = 0;i < num_of_asic;i++){
		ret = bcm_l2_addr_delete_by_port(i,mod,port,0);
		if(NAM_OK != ret){
	       syslog_ax_nam_fdb_err("delete fdb eth-port %#x asic port(%d,%d) failed in hw for ret %d!\n",index,i,port,ret);
		}
		NAM_RV_REPLACE_OK(ret, rv, NAM_E_UNAVAIL);
	}
	return rv;
#endif
}


/***************************************************
 * nam_fdb_table_delete_entry_with_trunk
 * 
 * delete fdb table items which belong to the port on hw
 *
 *	input params:
 *		trunk_no 
 *	
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/

int nam_fdb_table_delete_entry_with_trunk(unsigned short trunk_no)
{
#ifdef DRV_LIB_BCM
    int rv = 0,ret = 0,i = 0,num_of_asic = 0;
#endif

#ifdef DRV_LIB_CPSS 
	unsigned char devNum = 0;
	unsigned int ret=FDB_INIT;
	NAM_BOOL	ok = NAM_FALSE,Atenable = NAM_FALSE;
	unsigned int	actIsTrunk=0x1;
	unsigned int	actIsTrunkMask=0x1;
	unsigned int	actIsTrunkorg=0;
	unsigned int	actIsTrunkMaskorg=1;
	unsigned int	actTrunkPort=trunk_no;
	unsigned int	actTrunkPortMask=0x7f;
	unsigned int	actTrunkPortorg=0;
	unsigned int	actTrunkPortMaskorg=0x7f;
	unsigned int	actDev=devNum;
	unsigned int	actDevMask=0x1f;
	unsigned int	actDevorg=0;
	unsigned int	actDevMaskorg=0x1f;
	unsigned short  actVlan = 0;
	unsigned short  actVlanMask = 0;
	unsigned int	enable = 1,disable = 0;/*action to enable/disable trunk members*/
	unsigned int	re_flag = 0;/* value that is any action failed, o means all success*/
	unsigned char 	asic_num = 0,num_of_asic = 0;
	unsigned int fdb_state_count = 0;
	int asictype = 0; 
#endif

#ifdef DRV_LIB_CPSS
	num_of_asic = nam_asic_get_instance_num();
	for(asic_num;asic_num < num_of_asic;asic_num++ )
	{	
		devNum = asic_num;
		if(IS_DISTRIBUTED == SYSTEM_TYPE)
		{
			ret = nam_get_distributed_devnum(asic_num,&actDev);
			if(ret != FDB_CONFIG_SUCCESS)
			{
				nam_syslog_dbg("get distributed dev num failure !!! \n");
			}
		}

		ret = cpssDxChBrgFdbAAandTAToCpuGet(devNum,&Atenable);
		if(0 != ret){
	         nam_syslog_err("nam_fdb:cpssDxChBrgFdbAAandTATOCpuGet error: %d\n",ret);
	         return FDB_CONFIG_FAIL;
		}
		/*Get some value to restore after action*/
		ret = cpssDxChBrgFdbActionActiveDevGet( devNum,&actDevorg,&actDevMaskorg);
		if(0 != ret)
		{
		  nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveDevGet:FAIL.\n");
		  nam_syslog_err("err vlaue is: %d\n",ret);
		  return FDB_CONFIG_FAIL;
		}
		ret = cpssDxChBrgFdbActionActiveInterfaceGet(devNum,&actIsTrunkorg,&actIsTrunkMaskorg,&actTrunkPortorg,&actTrunkPortMaskorg);
		if(0 != ret)
		{
		  nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceGet:FAIL.\n");
		  nam_syslog_err("err vlaue is: %d\n",ret);
		  return FDB_CONFIG_FAIL;
		}

		ret = cpssDxChBrgFdbActionActiveVlanGet(devNum,&actVlan,& actVlanMask);
		if(0 != ret)
		{
		  nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveVlanGet:FAIL.\n");
		  nam_syslog_err("err vlaue is: %d\n",ret);
		  return FDB_CONFIG_FAIL;
		}

		/*Get all port members of this trunk,and set each port admin disable before action*/
		nam_set_trunk_member_endis(trunk_no,disable);

		/*actions should be set before delete */
		ret=cpssDxChBrgFdbAAandTAToCpuSet(devNum,NAM_FALSE);
		if(0 != ret)
		{
		  nam_syslog_err("npd_fdb:cpssDxChBrgFdbAAandTAToCpuSet:FAIL.\n");
		  nam_syslog_err("err vlaue is: %d\n",ret);
		  re_flag++;
		  goto isenable;
		}
		/*disable action before deleting*/
		ret=cpssDxChBrgFdbActionsEnableSet( devNum, NAM_FALSE);
		if(0 != ret)
		{
		  nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionsEnableSet:FAIL.\n");
		  nam_syslog_err("err vlaue is: %ld\n",ret);
		  re_flag++;
		  goto isenable;
		}   

		/* set triggered mode*/
		ret= cpssDxChBrgFdbMacTriggerModeSet( devNum,NAM_TRUE);
		if(0 != ret)
		{
		  nam_syslog_err("npd_fdb:cpssDxChBrgFdbMacTriggerModeSet:FAIL.\n");
		  nam_syslog_err("err vlaue is: %d\n",ret);
		  re_flag++;
		  goto isenable;
		}

		asictype = nam_asic_get_asic_type(devNum);
		if((asictype < ASIC_TYPE_START_EXMX_E) || (asictype > ASIC_TYPE_LAST_E))
		{
			nam_syslog_err("get asic type fail !\n");
		}
		if(asictype == ASIC_TYPE_DXCH_LION_E)
		{
		 	ret = cpssDxChBrgFdbActionModeSet(devNum,CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E);
		 	if(0 != ret)
			{
				nam_syslog_err("nam_fdb:cpssDxChBrgFdbActionModeSet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
				return FDB_CONFIG_FAIL;
		 	}
		 }
		 else
		 {
		 	ret=cpssDxChBrgFdbActionModeSet(devNum,CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E);
			if(0 != ret)
			{
				nam_syslog_err("nam_fdb:cpssDxChBrgFdbActionModeSet:FAIL.\n");
				nam_syslog_err("err vlaue is: %ld\n",ret);
				return FDB_CONFIG_FAIL;
			}
		 }
		/* disable FDB static del*/
		ret= cpssDxChBrgFdbStaticDelEnable( devNum,NAM_FALSE);
		if(0 != ret)
		{
		  nam_syslog_err("npd_fdb:cpssDxChBrgFdbStaticDelEnable:FAIL.\n");
		  nam_syslog_err("err vlaue is: %d\n",ret);
		  re_flag++;
		  goto isenable;
		}
		  
		/* set the vlan filters disable*/
			  
		ret= cpssDxChBrgFdbActionActiveVlanSet( devNum,0,0);
		if(0 != ret)
		{
		  nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveVlanSet:FAIL.\n");
		  nam_syslog_err("err vlaue is: %d\n",ret);
		  re_flag++;
		  goto isenable;
		}
		/*set trunk filters*/
		ret= cpssDxChBrgFdbActionActiveInterfaceSet( devNum, actIsTrunk,	actIsTrunkMask,actTrunkPort,actTrunkPortMask);
		if(0 != ret)
		{
		  nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
		  nam_syslog_err("err vlaue is: %d\n",ret);
		  re_flag++;
		  goto isenable;
		}   

		ret= cpssDxChBrgFdbActionActiveDevSet( devNum, actDev, actDevMask);
		if(0 != ret)
		{
		  nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
		  nam_syslog_err("err vlaue is: %d\n",ret);
		  re_flag++;
		  goto isenable;
		}   
		/*action start*/
		pthread_mutex_lock(&FdbActMutex);
		ret=cpssDxChBrgFdbTrigActionStart( devNum, CPSS_FDB_ACTION_DELETING_E);
		pthread_mutex_unlock(&FdbActMutex);
		if(0 != ret)
		{
		  nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStart1:FAIL.\n");
		  nam_syslog_err("err vlaue is: %d\n",ret);
		  re_flag++;
		  goto isenable;
		}
		while(fdb_state_count <= 6)
		{
			ret = cpssDxChBrgFdbTrigActionStatusGet(devNum,&ok);
			if(ret != 0)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStatusGet:FAIL!\n");
				fdb_state_count++;
				continue;
			}
			else if(0 == ok)
			{
				nam_syslog_err("delete fdb action did not finished !\n");
				osTimerWkAfter(100);
				fdb_state_count++;
				continue;
			}
			else if(1 == ok)
			{
				nam_syslog_dbg("delete fdb action finished !\n");
				break;
			}
			fdb_state_count++;
		}
		if(ok == 0)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStatusGet:delete failed!ok value %d\n",ok);
			re_flag++;
			goto isenable;
		}

		isenable:
		nam_set_trunk_member_endis(trunk_no,enable);

		asictype = nam_asic_get_asic_type(devNum);
		if((asictype < ASIC_TYPE_START_EXMX_E) || (asictype > ASIC_TYPE_LAST_E))
		{
			nam_syslog_err("get asic type fail !\n");
		}
		if(asictype == ASIC_TYPE_DXCH_LION_E)
		{
			ret=cpssDxChBrgFdbActionModeSet( devNum, CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E);
			if(0 != ret)
			{
			  nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStart2:FAIL.\n");
			  nam_syslog_err("err vlaue is: %d\n",ret); 
			  re_flag++;
			}
		}
		else
		{
			ret=cpssDxChBrgFdbActionModeSet( devNum, CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E);
			if(0 != ret)
			{
			  nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStart2:FAIL.\n");
			  nam_syslog_err("err vlaue is: %d\n",ret); 
			  re_flag++;
			}
		}

		/* set to automatic trigger mode*/

		ret= cpssDxChBrgFdbMacTriggerModeSet( devNum,CPSS_ACT_AUTO_E);
		if(0 != ret)
		{
		  nam_syslog_err("npd_fdb:cpssDxChBrgFdbMacTriggerModeSet:FAIL.\n");
		  nam_syslog_err("err vlaue is: %d\n",ret);
		  re_flag++;
		}
		  
		  
		/*restore vlan filter*/

		ret = cpssDxChBrgFdbActionActiveVlanSet(devNum,actVlan,actVlanMask);
		if(0 != ret)
		{
		  nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveVlanSet:FAIL.\n");
		  nam_syslog_err("err vlaue is: %d\n",ret);
		  re_flag++;
		}


		ret= cpssDxChBrgFdbActionActiveInterfaceSet( devNum, actIsTrunkorg,  actIsTrunkMaskorg,actTrunkPortorg,actTrunkPortMaskorg);
		if(0 != ret)
		{
		  nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
		  nam_syslog_err("err vlaue is: %d\n",ret);
		  re_flag++;
		}   

		ret= cpssDxChBrgFdbActionActiveDevSet( devNum, actDevorg, actDevMaskorg);
		if(0 != ret)
		{
		  nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
		  nam_syslog_err("err vlaue is: %d\n",ret);
		  re_flag++;
		}
		if(IS_DISTRIBUTED == SYSTEM_TYPE)
		{
			ret = nam_get_distributed_devnum(asic_num,&actDev);
			if(ret != FDB_CONFIG_SUCCESS)
			{
				nam_syslog_dbg("get distributed dev num failure !!! \n");
			}
		}

		ret=cpssDxChBrgFdbAAandTAToCpuSet(devNum,Atenable);
		if(0 != ret)
		{
		  nam_syslog_err("npd_fdb:cpssDxChBrgFdbAAandTAToCpuSet:FAIL.\n");
		  nam_syslog_err("err vlaue is: %d\n",ret);
		  re_flag++;
		}

		ret=cpssDxChBrgFdbActionsEnableSet( devNum, NAM_TRUE);
		if(0 != ret)
		{
		  nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionsEnableSet:FAIL.\n");
		  nam_syslog_err("err vlaue is: %ld\n",ret);
		  re_flag++;
		}
		if(re_flag)
		{
			nam_syslog_err("Dev %d Some errors in action,err number %d\n",asic_num,re_flag);
		}
	}
	if(re_flag){
	  nam_syslog_err("Some errors in action,err number %d\n",re_flag);
	  return FDB_CONFIG_FAIL;
	}
	else{ 	  
	  return FDB_CONFIG_SUCCESS;
	}
#endif
#ifdef DRV_LIB_BCM
	num_of_asic = nam_asic_get_instance_num();
	for(i = 0;i<num_of_asic;i++){
	    ret = bcm_l2_addr_delete_by_trunk(i,trunk_no,0);/*no delete static entries*/
		if(NAM_OK != ret){
	       syslog_ax_nam_fdb_err("delete fdb by trunk %d failed in hw unit %d for ret %d!\n", trunk_no,i,ret);
		}  
		NAM_RV_REPLACE_OK(ret, rv, NAM_E_UNAVAIL);
	}
	return rv;
#endif

}



int nam_fdb_table_delete_entry_with_vlan_trunk
(
	unsigned int vlanid, 
	unsigned int trunkid
)
{
	/*code optimize: Missing return statement
	zhangcl@autelan.com 2013-1-21*/
	int ret = 0;
#ifdef DRV_LIB_BCM
   int rv = 0,ret = 0,num_of_asic = 0,unit = 0;
#endif

#ifdef DRV_LIB_BCM
   num_of_asic = nam_asic_get_instance_num();
   for(unit = 0;unit < num_of_asic;unit++)
   {
	   ret = bcm_l2_addr_delete_by_vlan_trunk(unit,vlanid,trunkid,0);
	   if(NAM_OK != ret)
	   {
	      syslog_ax_nam_fdb_err("fdb delete by vlan %d trunk %d failed in hw unit %d for ret %d\n",vlanid,trunkid,unit,ret);
	   }
	   NAM_RV_REPLACE_OK(ret, rv, NAM_E_UNAVAIL);
   }
   return rv;
#endif
	return ret;

}


/***************************************************
 * nam_fdb_table_agingtime_get
 * 
 * get the agingtime value on hw
 *
 *	input params:
 *		timeout-which hold the getting value
 *	
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/

int nam_fdb_table_agingtime_get(unsigned int* 	timeout)
{
#ifdef DRV_LIB_BCM
	int ret = FDB_CONFIG_SUCCESS,i = 0,num_of_asic = 0,agingtime = -1;
#endif

#ifdef DRV_LIB_CPSS
	unsigned char devNum;
	unsigned long	ret;
	devNum = FDB_INIT;
#endif

#ifdef DRV_LIB_CPSS
	ret = cpssDxChBrgFdbAgingTimeoutGet(devNum,timeout);
	nam_syslog_dbg("get timeout vlaue is: %d\n",*timeout);
	if(0 != ret)
	{
		nam_syslog_err("npd_fdb::fdb_table_agingtime_get::FAIL.\n");
		nam_syslog_err("err vlaue is: %ld",ret);
		nam_syslog_err("agingtime %d", *timeout);
		return FDB_CONFIG_FAIL;
	}
	return FDB_CONFIG_SUCCESS;
	
#endif

#ifdef DRV_LIB_BCM
	/*num_of_asic = nam_asic_get_instance_num();*/
	/*for(i = 0;i<num_of_asic;i++){*/
       	ret = bcm_l2_age_timer_get(0,&agingtime);/*get dev 0 agingtime*/
        if (ret < 0) {
		   syslog_ax_nam_fdb_err("Failed to get aging time,ret %d\n",ret);
	       return ret;
	    } 
		else if(NAM_OK == ret){
			*timeout = agingtime;
		} 
	return ret;
#endif
}


int nam_fdb_table_delete_entry_with_vlan_port
(
	unsigned int vlanid, 
	unsigned int index
)
{
#ifdef DRV_LIB_BCM
    int rv = 0,ret = 0,mod = 0,i = 0,num_of_asic = 0;
    unsigned char unit = 0,port = 0;
#endif

#ifdef DRV_LIB_CPSS
  	unsigned char devNum;
	unsigned char portNum;
	unsigned int ret=FDB_INIT;
	devNum = 0;
	NAM_BOOL  ok = NAM_FALSE;
	NAM_BOOL  Atenable = NAM_FALSE;
	unsigned int  actIsTrunk=0;
    unsigned int  actIsTrunkMask=1;
	unsigned int  actIsTrunkorg=0;
    unsigned int  actIsTrunkMaskorg=1;
	
    unsigned int  actTrunkPort=0;
    unsigned int  actTrunkPortMask=0x7f;
	unsigned int  actTrunkPortorg=0;
    unsigned int  actTrunkPortMaskorg=0x7f;
	
	unsigned int  actDev=0;
    unsigned int  actDevMask=0x1f;
	unsigned int  actDevorg=0;
    unsigned int  actDevMaskorg=0x1f;
	unsigned int  isEnable = 0;
	
	unsigned int  actVlan = vlanid;
	unsigned int  actVlanMask = 0xfff;
	unsigned short  actVlanorg = 0;
	unsigned short  actVlanMaskorg = 0;
	unsigned int re_flag = 0;
	enum product_id_e productId = npd_query_product_id();
	unsigned int isBoard = 0,linkState = 0,learn = 0;
	unsigned char asic_num = 0,ammount_asic_nums = 0;
	unsigned int fdb_state_count = 0;

	CPSS_MAC_ACTION_MODE_ENT macmode = CPSS_ACT_AUTO_E;
	CPSS_FDB_ACTION_MODE_ENT actmode = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;
	CPSS_PORT_LOCK_CMD_ENT    learncmd = CPSS_LOCK_FRWRD_E;
#endif

#ifdef DRV_LIB_CPSS
	ret = npd_get_devport_by_global_index(index,&devNum, &portNum);
    if(NAM_OK != ret){
		syslog_ax_nam_fdb_err("eth_index %#x get asic port failed for ret %d\n",index,ret);
        return FDB_CONFIG_FAIL;
	}	
	actDev = devNum;
	actTrunkPort=portNum;

	/*
         save the pre-configuration before action
      */
    if(IS_DISTRIBUTED == SYSTEM_TYPE)
	{
		ret = nam_get_distributed_devnum(asic_num,&actDev);
		if(ret != FDB_CONFIG_SUCCESS)
		{
			nam_syslog_dbg("get distributed dev num failure !!! \n");
		}
	}

   ret = cpssDxChBrgFdbAAandTAToCpuGet(devNum,&Atenable);
   if( 0 != ret ){
         nam_syslog_err("nam_fdb:cpssDxChBrgFdbAAandTAToCpuGet error: %d\n",ret);
		 return FDB_CONFIG_FAIL;
   }
   ret = cpssDxChBrgFdbPortLearnStatusGet(devNum,portNum,&learn,&learncmd);
   if( 0 != ret ){
         nam_syslog_err("nam_fdb: cpssDxChBrgFdbPortLearnStatusGet error: %d\n",ret);
		 return FDB_CONFIG_FAIL;
   }
   ret = cpssDxChBrgFdbActionModeGet(devNum,&actmode);
   if( 0 != ret){
         nam_syslog_err("nam_fdb: cpssDxChBrgFdbActionModeGet error: %d\n",ret);
         nam_syslog_err("Now the CPSS_FDB_ACTION_MODE_ENT value is %d\n",actmode);
		 return FDB_CONFIG_FAIL;
   }

   ret = cpssDxChBrgFdbMacTriggerModeGet(devNum,&macmode);
   if(0 != ret){
         nam_syslog_err("nam_fdb: cpssDxChBrgFdbMacTriggerModeGet error :%d\n",ret);
         nam_syslog_err("Now the CPSS_MAC_ACTION_MODE_ENT value is %d\n",macmode);
		 return FDB_CONFIG_FAIL;
   }
   
   ret = cpssDxChBrgFdbActionActiveDevGet( devNum,&actDevorg,&actDevMaskorg);
   if(0 != ret)
		   {
			   nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveDevGet:FAIL.\n");
			   nam_syslog_err("err vlaue is: %d\n",ret);
			   return FDB_CONFIG_FAIL;
		   }
   if(IS_DISTRIBUTED == SYSTEM_TYPE)
	{
		ret = nam_get_distributed_devnum(asic_num,&actDev);
		if(ret != FDB_CONFIG_SUCCESS)
		{
			nam_syslog_dbg("get distributed dev num failure !!! \n");
		}
	}
   ret = cpssDxChBrgFdbActionActiveInterfaceGet(devNum,&actIsTrunkorg,&actIsTrunkMaskorg,&actTrunkPortorg,&actTrunkPortMaskorg);
   if(0 != ret)
		   {
			   nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceGet:FAIL.\n");
			   nam_syslog_err("err vlaue is: %d\n",ret);
			   return FDB_CONFIG_FAIL;
		   }

   ret = cpssDxChBrgFdbActionActiveVlanGet(devNum,&actVlanorg,& actVlanMaskorg);
   if(0 != ret)
		   {
			   nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveVlanGet:FAIL.\n");
			   nam_syslog_err("err vlaue is: %d\n",ret);
			   return FDB_CONFIG_FAIL;
		   }

   ret = cpssDxChPortEnableGet(devNum,portNum,&linkState);
   if(0 != ret){
   	   nam_syslog_err("Get port %d admin state failed!ret %d\n",portNum,ret);
	   return FDB_CONFIG_FAIL;

   }
	/*first set port admin disable before action*/
	if(((PRODUCT_ID_AX7K == productId)||(PRODUCT_ID_AX7K_I == productId))
		&& (index < 4)) {
	   isBoard = NPD_TRUE;
    }
	if(NPD_FALSE == isBoard){
		isEnable = 0;
		if(linkState != isEnable){
			ret = cpssDxChPortEnableSet(devNum,portNum,isEnable);
			osTimerWkAfter(1);/*gave time to stop the packet stream*/
		}
	}


    /*
         set the AA and TA disable
      */
	ret=cpssDxChBrgFdbAAandTAToCpuSet(devNum,NAM_FALSE);
	if(0 != ret)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbAAandTAToCpuSet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
				re_flag++;/*set reflag value to identifier that the return value is fail*/
				goto isenable;
			}
	/*disable action before deleting*/
	ret=cpssDxChBrgFdbActionsEnableSet( devNum, NAM_FALSE);
	if(0 != ret)
		{
			    nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionsEnableSet:FAIL.\n");
				nam_syslog_err("err vlaue is: %ld\n",ret);
				re_flag++;/*set reflag value to identifier that the return value is fail*/
				goto isenable;
		}

    ret= cpssDxChBrgFdbPortLearnStatusSet(devNum,portNum,0,CPSS_LOCK_DROP_E);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbPortLearnStatusSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
		 goto isenable;
	}
	ret= cpssDxChBrgFdbMacTriggerModeSet( devNum,CPSS_ACT_TRIG_E);
	if(0 != ret)
	{
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbMacTriggerModeSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		goto isenable;
	}

	/* disable FDB static del*/
	ret= cpssDxChBrgFdbStaticDelEnable( devNum,NAM_FALSE);
	if(0 != ret)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbStaticDelEnable:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
				re_flag++;/*set reflag value to identifier that the return value is fail*/
				goto isenable;
			}
		
   /* set the vlan filters disable*/
		
	
	ret= cpssDxChBrgFdbActionActiveVlanSet( devNum,actVlan,actVlanMask);
	if(0 != ret)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveVlanSet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
				re_flag++;/*set reflag value to identifier that the return value is fail*/
				goto isenable;
			}

   
	ret= cpssDxChBrgFdbActionActiveInterfaceSet( devNum, actIsTrunk,  actIsTrunkMask,actTrunkPort,actTrunkPortMask);
	if(0 != ret)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
				re_flag++;/*set reflag value to identifier that the return value is fail*/
				goto isenable;
			}	
	
	ret= cpssDxChBrgFdbActionActiveDevSet( devNum, actDev, actDevMask);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;/*set reflag value to identifier that the return value is fail*/
		goto isenable;
	}	
	pthread_mutex_lock(&FdbActMutex);
	ret=cpssDxChBrgFdbTrigActionStart( devNum, CPSS_FDB_ACTION_DELETING_E);
	pthread_mutex_unlock(&FdbActMutex);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStart1:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret); 
		re_flag++;/*set reflag value to identifier that the return value is fail*/
		goto isenable;
	}
	while(fdb_state_count <= 6)
	{
		ret = cpssDxChBrgFdbTrigActionStatusGet(devNum,&ok);
		if(ret != 0)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStatusGet:FAIL!\n");
			fdb_state_count++;
			continue;
		}
		else if(0 == ok)
		{
			nam_syslog_err("delete fdb action did not finished !\n");
			if(fdb_state_count == 0)
			{
				osTimerWkAfter(10);
			}
			else
			{
				osTimerWkAfter(100);
			}
			fdb_state_count++;
			continue;
		}
		else if(1 == ok)
		{
			nam_syslog_dbg("delete fdb action finished !\n");
			break;
		}
		fdb_state_count++;
	}
	if(ok == 0)
	{
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStatusGet:delete failed!ok value %d\n",ok);
		re_flag++;
		goto isenable;
	}
	
	isenable:
	if(((PRODUCT_ID_AX7K == productId)||(PRODUCT_ID_AX7K_I == productId))
		&& (index < 4)) {
	   isBoard = NPD_TRUE;
	}
	if(NPD_FALSE == isBoard){
		cpssDxChPortEnableGet(devNum,portNum,&isEnable);
		if(linkState != isEnable){
			ret = cpssDxChPortEnableSet(devNum,portNum,linkState);
		}
	}
	
    ret=cpssDxChBrgFdbActionsEnableSet( devNum, NAM_TRUE);
    if(0 != ret)
		{
			nam_syslog_err("nam_fdb:cpssDxChBrgFdbActionsEnableSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %ld\n",ret);
			re_flag++;/*set reflag value to identifier that the return value is fail*/
		}
	
    ret= cpssDxChBrgFdbPortLearnStatusSet(devNum,portNum,learn,learncmd);
	if(0 != ret){
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbPortLearnStatusSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		re_flag++;
	}

	ret=cpssDxChBrgFdbActionModeSet( devNum, actmode);
	if(0 != ret)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbTrigActionStart2:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret); 
				re_flag++;/*set reflag value to identifier that the return value is fail*/
		    }

	/* set to automatic trigger mode*/
	
	ret= cpssDxChBrgFdbMacTriggerModeSet( devNum,macmode);
	if(0 != ret)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbMacTriggerModeSet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
				re_flag++;/*set reflag value to identifier that the return value is fail*/
			}
        
        
	/*restore vlan filter*/

	ret = cpssDxChBrgFdbActionActiveVlanSet(devNum,actVlanorg,actVlanMaskorg);
	if(0 != ret)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveVlanSet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
				re_flag++;/*set reflag value to identifier that the return value is fail*/
			}

   
	ret= cpssDxChBrgFdbActionActiveInterfaceSet( devNum, actIsTrunkorg,  actIsTrunkMaskorg,actTrunkPortorg,actTrunkPortMaskorg);
	if(0 != ret)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
				re_flag++;/*set reflag value to identifier that the return value is fail*/
			}	
	
	ret= cpssDxChBrgFdbActionActiveDevSet( devNum, actDevorg, actDevMaskorg);
	if(0 != ret)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbActionActiveInterfaceSet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
				re_flag++;/*set reflag value to identifier that the return value is fail*/
			}
	
	
	ret=cpssDxChBrgFdbAAandTAToCpuSet(devNum,Atenable);
	if(0 != ret)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbAAandTAToCpuSet:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
				re_flag++;/*set reflag value to identifier that the return value is fail*/
			}
	if(re_flag){
       nam_syslog_err("fdb delete-Error occured when configuration!re_flag %d\n",re_flag);
	   return FDB_CONFIG_FAIL;

	}
	return FDB_CONFIG_SUCCESS;
#endif

#ifdef DRV_LIB_BCM
	num_of_asic = nam_asic_get_instance_num();
	ret = npd_get_devport_by_global_index(index,&unit,&port);
	if(0 != ret){
       nam_syslog_dbg("get unit-port failed from gloab index(%d) for ret %d\n",index,ret);
	   return FDB_CONFIG_FAIL;
	}
	ret = bcm_stk_modid_get(unit,&mod);
	if(NAM_OK != ret){
       syslog_ax_nam_fdb_err("unit %d get module id failed for ret %d\n",i,ret);
	   return FDB_CONFIG_FAIL;
	}	
	for(i = 0;i < num_of_asic;i++){
		ret = bcm_l2_addr_delete_by_vlan_port(i,vlanid,mod,port,0);/*no care static entries*/
		if(NAM_OK != ret){
	      syslog_ax_nam_fdb_err("vlan %d eth-port %#x,asic port(%d,%d) delete fdb failed,ret %d!\n",vlanid,index,unit,port,ret);
		}
		NAM_RV_REPLACE_OK(ret, rv, NAM_E_UNAVAIL);
	}
	return rv;
#endif

}


/***************************************************
 * nam_fdb_static_table_delete_entry_with_vlan
 * 
 * delete fdb table items which belong to the vlan on hw
 *
 *	input params:
 *		vlanId--The vlan ID 
 *	
 *	output: none
 *
 * 	return:
 *		 0- if operation ok
 *           other-operation fail
 *		
 *********************************************/


unsigned int nam_fdb_static_table_delete_entry_with_vlan
(
    unsigned short vlanId
)
{
#ifdef DRV_LIB_BCM
    int rv = 0,ret = 0;
    int i = 0,unit = 0,num_of_asic = 0;
	int j = 0;
    char macComp[13]= {0},macAddr[6]= {0};
	user_data usrdata;
	NPD_FDB *fdb;

#endif

#ifdef DRV_LIB_CPSS
    unsigned int            ret = 0;
	unsigned int 			index = 0;
	unsigned int			result=0;
	unsigned int         	i = 0;
	unsigned char			devNum =0;
	unsigned int        	validPtr=0;
    unsigned int       	skipPtr= 0;
    NAM_BOOL                agedPtr;
	unsigned char			associatedDevNumPtr;
	CPSS_MAC_ENTRY_EXT_STC	entryPtr;
	unsigned char macComp[13]= {0},macAddr[6]= {0};
	unsigned int            board_type = 0;
#endif

#ifdef DRV_LIB_CPSS
	
	if(BOARD_TYPE == BOARD_TYPE_AX81_SMU)
	{
		board_type = 1;/*used for lion 32K fdb table*/
	}
	do{
		 /* call cpss api function */
		memset(&entryPtr,0,sizeof(CPSS_MAC_ENTRY_EXT_STC));
		result = cpssDxChBrgFdbMacEntryRead(devNum, index, &validPtr, &skipPtr,
										&agedPtr, &associatedDevNumPtr, &entryPtr);
        /* check vlan ID*/
		if( vlanId != entryPtr.key.key.macVlan.vlanId){
           index++;
		   continue;
		}
		
		if((!validPtr) ||(skipPtr)||(1 != entryPtr.isStatic)) {
				index++;
				continue;
		}


		/*if is one  MULTICAST, do not delete*/
        if((entryPtr.key.entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E)||
			                      (entryPtr.key.entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E) ){
			index++;
			continue;
		}

        /* if is one trap to cpu, do not delete*/
		if(entryPtr.daCommand==CPSS_MAC_TABLE_CNTL_E){
			index++;
			continue;
		}
	    if (result == 0) {
		 	/* if is one blacklist or IGMP, do not delete*/
			if((CPSS_MAC_TABLE_DROP_E==entryPtr.daCommand)||(CPSS_MAC_TABLE_DROP_E == entryPtr.saCommand)){
	   			index++;
	            continue;
			}
			else{
	            /*delete the entry*/
				/*check if the sys mac*/
				memcpy(macAddr,entryPtr.key.key.macVlan.macAddr.arEther,6);
						sprintf(macComp,"%02x%02x%02x%02x%02x%02x", \
						macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
				ret = npd_system_verify_basemac(macComp);
				if(0 == ret){/* not the sys mac */
				    /*check if the SPI entry --dev 1 port 26*/
				    if(!((1 == entryPtr.dstInterface.devPort.devNum)&&(26 == entryPtr.dstInterface.devPort.portNum))){
						entryPtr.isStatic = 0;
						ret = cpssDxChBrgFdbMacEntryWrite(devNum,index,1,&entryPtr);
						if(0 != ret){
							 nam_syslog_err("nam_fdb_static_table_delete_entry_with_vlan: error to delete the static entry , index: %d\n",index);
							 return FDB_CONFIG_FAIL;
						}
						i++;
				    }
				}
			}
			index++;
	   }
		else {
			index++;
			continue;
		}
	}while((!result)&&(((board_type == 1)?(index<32768):(index<16384))));
    nam_syslog_dbg("The number delete fdb static is %d\n",i);
    return FDB_CONFIG_SUCCESS;	
#endif

#ifdef DRV_LIB_BCM

    memset(&usrdata,0,sizeof(usrdata));
	num_of_asic = nam_asic_get_instance_num();
	for(i = 0;i<num_of_asic;i++){
		ret = bcm_l2_traverse(unit,nam_l2_addr_show,&usrdata);
        if(NAM_E_NONE!=ret){
           syslog_ax_nam_fdb_err("Traverse fdb table failed ,ret%d\n",ret);
		}
		if(0 >= usrdata.count){
                continue;
		}
		for(j = 0;j < usrdata.count;j++){
			if(!(usrdata.l2addr[j].flags & BCM_L2_STATIC)){
				continue;
		    }			
			if((usrdata.l2addr[j].port != 0)&&(vlanId == usrdata.l2addr[j].vid)){ 
				 ret = bcm_l2_addr_delete(i,usrdata.l2addr[j].mac,vlanId);
				 if(NAM_OK != ret){
					  syslog_ax_nam_fdb_err("delete fdb by unit %d vlan %d failed in hw for ret %d!\n",i,vlanId,ret);
				 }
		    }	
		}   
   }

	return rv;
#endif

}

/***************************************************
 * nam_fdb_static_table_delete_entry_with_port
 * 
 * delete fdb table items which belong to the port on hw
 *
 *	input params:
 *		eth_g_index--the global index of one port
 *	
 *	output: none
 *
 * 	return:
 *		 0- if operation ok
 *           other-operation fail
 *		
 *********************************************/


unsigned int nam_fdb_static_table_delete_entry_with_port
(
    unsigned int eth_g_index
)
{
#ifdef DRV_LIB_BCM
    int rv = 0,ret = 0,mod = 0,num_of_asic = 0,i = 0;
    unsigned char unit = 0,port = 0;
#endif	

#ifdef DRV_LIB_CPSS
    unsigned int            ret = 0;
	unsigned int 			index = 0;
	unsigned int			result=0;
	unsigned int         	i = 0;
	unsigned char			devNum =0;
	unsigned int            actDev = 0;
	unsigned char            portNum = 0;
	unsigned int        	validPtr=0;
    unsigned int       	skipPtr= 0;
    NAM_BOOL                agedPtr;
	unsigned char			associatedDevNumPtr;
	unsigned char   asic_num = 0,ammount_asic_nums = 0;
	CPSS_MAC_ENTRY_EXT_STC	entryPtr;
	char  macComp[13]={0};
	unsigned char macAddr[6] = {0};
#endif

#ifdef DRV_LIB_CPSS
	/* Get dev,port number */
	ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
    if(NAM_OK != ret){
		syslog_ax_nam_fdb_err("eth_index %#x get asic port failed for ret %d\n",eth_g_index,ret);
        return FDB_CONFIG_FAIL;
	}	

	
	if(IS_DISTRIBUTED == SYSTEM_TYPE)
	{
		ret = nam_get_distributed_devnum(asic_num,&actDev);
		if(ret != FDB_CONFIG_SUCCESS)
		{
			nam_syslog_dbg("get distributed dev num failure !!! \n");
		}
	}
	do{
		 /* call cpss api function */
		memset(&entryPtr,0,sizeof(CPSS_MAC_ENTRY_EXT_STC));
		result = cpssDxChBrgFdbMacEntryRead(devNum, index, &validPtr, &skipPtr,
										&agedPtr, &associatedDevNumPtr, &entryPtr);

		if((!validPtr) ||(skipPtr)||(1 != entryPtr.isStatic)) {
			index++;
			continue;
		}


		/*if is one  MULTICAST, do not delete*/
        if((entryPtr.key.entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E)||
			                      (entryPtr.key.entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E) ){
			index++;
			continue;
		}

        /* if is one trap to cpu, do not delete*/
		if(entryPtr.daCommand==CPSS_MAC_TABLE_CNTL_E){
			index++;
			continue;
		}

		/* check the port*/
		if(IS_DISTRIBUTED == SYSTEM_TYPE)
		{
			if((portNum != entryPtr.dstInterface.devPort.portNum)||(actDev != entryPtr.dstInterface.devPort.devNum)){
            	index++;
				continue;
			}
		}
		else
		{
			if((portNum != entryPtr.dstInterface.devPort.portNum)||(devNum != entryPtr.dstInterface.devPort.devNum)){
            	index++;
				continue;
			}
		}
	    if (result == 0) {
		 	/* if is one blacklist or IGMP, do not delete*/
			if((CPSS_MAC_TABLE_DROP_E==entryPtr.daCommand)||(CPSS_MAC_TABLE_DROP_E == entryPtr.saCommand)){
	   			index++;
	            continue;
			}
			else{
	            /*delete the entry*/
				/*check if the sys mac*/
				memcpy(macAddr,entryPtr.key.key.macVlan.macAddr.arEther,6);
						sprintf(macComp,"%02x%02x%02x%02x%02x%02x", \
						macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
	            ret = npd_system_verify_basemac(macComp);
				if(0 == ret){/* not the sys mac */
					entryPtr.isStatic = 0;
					ret = cpssDxChBrgFdbMacEntryWrite(devNum,index,1,&entryPtr);
					if(0 != ret){
						 nam_syslog_err("nam_fdb_static_table_delete_entry_with_vlan: error to delete the static entry , index: %d\n",index);
						 return FDB_CONFIG_FAIL;
					}
					i++;
				}
				
			}
			index++;
	   }
		else {
			index++;
			continue;
		}
	}while((!result)&&(index<16384));
    nam_syslog_dbg("The number delete fdb static is %d\n",i);
    return FDB_CONFIG_SUCCESS;	
#endif

#ifdef DRV_LIB_BCM
	ret = npd_get_devport_by_global_index(eth_g_index,&unit,&port);
    if(NAM_OK != ret){
		syslog_ax_nam_fdb_err("eth-port %#x get asic port failed,ret %d\n",eth_g_index,ret);
        return FDB_CONFIG_FAIL;
	}
	ret = bcm_stk_modid_get(unit,&mod);
	if(NAM_OK != ret){
		syslog_ax_nam_fdb_err("unit %d get moduld id failed for ret %d\n",unit,ret);
		return FDB_CONFIG_FAIL;
	}
	num_of_asic = nam_asic_get_instance_num();
	for(i = 0;i < num_of_asic;i++){
		ret = bcm_l2_addr_delete_by_port(i,mod,port,BCM_L2_DELETE_STATIC);
		if(NAM_OK != ret){
	       syslog_ax_nam_fdb_err("eth-port %#x unit %d mod %d delete static fdb failed, ret %d!\n",eth_g_index,i,mod,ret);
		}
		NAM_RV_REPLACE_OK(ret, rv, NAM_E_UNAVAIL);		
	}
	return rv;
#endif

}

	
/************************************************************
 *nam_static_fdb_entry_mac_set_for_system
 * 
 * config the system static fdb item which need during inintalization on hw
 *
 *	input params:
 *		vlanid -which vlan index need config
 *		flag - judge the params is whether wrong
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/

int nam_static_fdb_entry_mac_set_for_system
(
	unsigned short vlanId,
	unsigned int flag
) 
{
#ifdef DRV_LIB_BCM
	unsigned long ret;
	unsigned int i=0,j = 0;
	unsigned nam_flag =1;
	unsigned int hashPtr;
	int skip = 0;
	unsigned char num_of_asic = 0;
	unsigned char portNum =0;
	bcm_l2_addr_t l2addr;	
#endif
#ifdef DRV_LIB_CPSS
	unsigned long ret;
	unsigned int i=0;
	unsigned nam_flag =1;
	unsigned int hashPtr;
	NAM_BOOL skip = NAM_FALSE;
	unsigned char devNum = 0;
	unsigned char portNum = 63;
	CPSS_MAC_ENTRY_EXT_STC		 macEntryPtr;

	ETHERADDR *macPtr=NULL ;
#endif
	if(flag!=1){
		return FDB_CONFIG_FAIL;
	}

#ifdef DRV_LIB_CPSS
	memset(&macEntryPtr, 0 ,sizeof(CPSS_MAC_ENTRY_EXT_STC));
	for(;i<SYSTEM_STATIC_LEN;i++){
		if(i==0){
			macEntryPtr.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
			macPtr = &(macEntryPtr.key.key.macVlan.macAddr);
			memcpy(macPtr->arEther,dutinit[i],6);
			macEntryPtr.key.key.macVlan.vlanId = (unsigned short)vlanId;
			macEntryPtr.dstInterface.type = CPSS_INTERFACE_PORT_E;
			#if 0
			macEntryPtr.dstInterface.devPort.devNum = devNum;
			#endif
			macEntryPtr.dstInterface.devPort.devNum = g_dis_dev[0];
			macEntryPtr.dstInterface.devPort.portNum = portNum;
			NAM_CONVERT_DEV_PORT_MAC(macEntryPtr.dstInterface.devPort.devNum,
									 macEntryPtr.dstInterface.devPort.portNum);
		}
		else{
			if(nam_flag==1){
				nam_flag--;
				ret=cpssDxChBrgVlanIpmBridgingEnable(devNum,vlanId,CPSS_IP_PROTOCOL_IPV4_E,NAM_TRUE);
				if( ret!=0){
					nam_syslog_err("cpssDxChBrgFdbHashCalc erro is :%ld\n",ret);
					return FDB_CONFIG_FAIL;
					}
				ret= cpssDxChBrgVlanIpmBridgingModeSet(devNum,vlanId,CPSS_IP_PROTOCOL_IPV4_E,CPSS_BRG_IPM_GV_E);
				if( ret!=0){
					nam_syslog_err("cpssDxChBrgFdbHashCalc erro is :%ld\n",ret);
					return FDB_CONFIG_FAIL;
					}
				}
			macEntryPtr.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
			macEntryPtr.key.key.ipMcast.vlanId=vlanId;
			
			memcpy(macEntryPtr.key.key.ipMcast.sip,anySip,4);
			memcpy(macEntryPtr.key.key.ipMcast.dip,dutinit[i],4);
			
			macEntryPtr.dstInterface.type = CPSS_INTERFACE_VID_E;
			macEntryPtr.dstInterface.vlanId = vlanId;
			macEntryPtr.dstInterface.vidx = 0xFFF;
		}
		/*nam_syslog_err("the value of i= %d\n",i);*/
		ret=cpssDxChBrgFdbHashCalc(  devNum,&macEntryPtr.key, &hashPtr);
		/*nam_syslog_err("the hashPtr vlaue is : %d\n",hashPtr);*/
		if( ret!=0){
			nam_syslog_err("cpssDxChBrgFdbHashCalc erro is :%ld\n",ret);
			return FDB_CONFIG_FAIL;
			}
		
		
		macEntryPtr.isStatic = NAM_TRUE;/* static FDB */
		macEntryPtr.daCommand = CPSS_MAC_TABLE_CNTL_E;
		macEntryPtr.saCommand = CPSS_MAC_TABLE_DROP_E;
		macEntryPtr.daRoute =NAM_TRUE;
		macEntryPtr.mirrorToRxAnalyzerPortEn = NAM_FALSE;
		macEntryPtr.sourceID = 0;
		macEntryPtr.userDefined = 0;
		macEntryPtr.daQosIndex = 0;
		macEntryPtr.saQosIndex = 0;
		macEntryPtr.daSecurityLevel = 0;
		macEntryPtr.saSecurityLevel = 0;
		macEntryPtr.appSpecificCpuCode = NAM_TRUE;

		ret=cpssDxChBrgFdbMacEntryWrite(devNum, hashPtr, skip, &macEntryPtr);

		if(0 != ret) {
			nam_syslog_err("cpssDxChBrgFdbMacEntryWrite! \n");
			nam_syslog_err("err vlaue is: %ld\n",ret);
			return FDB_CONFIG_FAIL;
		}
	}
#endif

#ifdef DRV_LIB_BCM
	memset(&l2addr,0,sizeof(bcm_l2_addr_t));
	num_of_asic = nam_asic_get_instance_num();
	for(;i<SYSTEM_STATIC_LEN;i++){
		if(i==0){
			l2addr.vid = vlanId;
			memcpy(l2addr.mac,dutinit[i],6);
			l2addr.flags |= (BCM_L2_STATIC | BCM_L2_COPY_TO_CPU | BCM_L2_DISCARD_SRC | BCM_L2_L3LOOKUP|BCM_L2_REPLACE_DYNAMIC);
            for(j = 0;j<num_of_asic;j++){
	 			/*l2addr.port = CMIC_PORT(i);*/
	 			l2addr.port = 0; /*port 0 as cpu port*/
				bcm_l2_addr_add(j,&l2addr);                
			}
		}
		else{
			if(nam_flag==1){
				nam_flag--;
				/*=====================================================================
				ret=cpssDxChBrgVlanIpmBridgingEnable(devNum,vlanId,CPSS_IP_PROTOCOL_IPV4_E,NAM_TRUE);
				if( ret!=0){
					nam_syslog_err("cpssDxChBrgFdbHashCalc erro is :%ld\n",ret);
					return FDB_CONFIG_FAIL;
					}
				ret= cpssDxChBrgVlanIpmBridgingModeSet(devNum,vlanId,CPSS_IP_PROTOCOL_IPV4_E,CPSS_BRG_IPM_GV_E);
				if( ret!=0){
					nam_syslog_err("cpssDxChBrgFdbHashCalc erro is :%ld\n",ret);
					return FDB_CONFIG_FAIL;
					}
				====================================================================*/
				}
                /*===========================================================
                             In broadcom ASIC no ip mulcast.
                          =============================================================*/
	   }		
	}
#endif
	return FDB_CONFIG_SUCCESS;
}

/************************************************************
 *nam_static_fdb_entry_indirect_set_for_mld
 * 
 * config the system static fdb item which for mld protocol
 *
 *	input params:
 *		vlanid -which vlan index need config
 *		flag - judge the params is whether wrong
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/
 /*****************************************************************
  *API Below adopt the indirect access method to write FDB entry!!
  *By call Cpss Api :cpssDxChBrgFdbMacEntrySet 
  *	It'll Send Au message from host CPU to PP device.
  *
  ***wujh*********************************************************/
int nam_static_fdb_entry_indirect_set_for_mld
(
	unsigned int dip,
	unsigned short vidx,
	unsigned short vlanId
) 
{

	unsigned long ret;
	unsigned char devNum = 0;
	unsigned char mcMac[6] = {0x33,0x33,0xff,0xff,0xff,0xff};
	unsigned int iphost = 0x0;
	unsigned int i;
	unsigned char dmip[4]={0};

	iphost = htonl(dip);
	nam_syslog_dbg("network order ip :iphost = 0x%8x\r\n",iphost);
	for(i=0;i<4;i++){
		dmip[i] = (iphost>>(i*8))&0xff; /*dip -->iphost*/
		nam_syslog_err("dmip[%d] = 0x%2x!\n",i,dmip[i]);
		}
	
	mcMac[2] =dmip[3];
	mcMac[3] =dmip[2];
	mcMac[4] =dmip[1];
	mcMac[5] =dmip[0];
	
	#ifdef DRV_LIB_CPSS
	CPSS_MAC_ENTRY_EXT_STC	macEntryPtr;
	memset(&macEntryPtr, 0 ,sizeof(CPSS_MAC_ENTRY_EXT_STC));
	#endif

	#ifdef DRV_LIB_BCM
	int unit = 0,num_of_asic = 0,rv = 0;
	bcm_mcast_addr_t mcast;
	memset(&mcast, 0, sizeof(bcm_mcast_addr_t));
	#endif

	#ifdef DRV_LIB_CPSS
	nam_syslog_dbg("nam_fdb for mld Get params: vlanId %d,vidx %d,groupIP 0x%8x.\r\n",vlanId,vidx,dip);
    #if 0
	// <MAC, V>
	macEntryPtr.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
	macEntryPtr.key.key.macVlan.vlanId = vlanId;
	//memcpy(&(macEntryPtr.key.key.macVlan.macAddr),mcMac,6);
	macEntryPtr.key.key.macVlan.macAddr.arEther[0] = mcMac[0];
	macEntryPtr.key.key.macVlan.macAddr.arEther[1] = mcMac[1];
	macEntryPtr.key.key.macVlan.macAddr.arEther[2] = mcMac[2];
	macEntryPtr.key.key.macVlan.macAddr.arEther[3] = mcMac[3];
	macEntryPtr.key.key.macVlan.macAddr.arEther[4] = mcMac[4];
	macEntryPtr.key.key.macVlan.macAddr.arEther[5] = mcMac[5];
	macEntryPtr.key.key.macVlan.vlanId = vlanId;
	nam_syslog_err("#####################################\n");
	nam_syslog_err("Multicast MAC set to HW as follow::\n");
	nam_syslog_err("%02x:%02x:%02x:%02x:%02x:%02x\r\n",\
								macEntryPtr.key.key.macVlan.macAddr.arEther[0],\
								macEntryPtr.key.key.macVlan.macAddr.arEther[1],\
								macEntryPtr.key.key.macVlan.macAddr.arEther[2],\
								macEntryPtr.key.key.macVlan.macAddr.arEther[3],\
								macEntryPtr.key.key.macVlan.macAddr.arEther[4],\
								macEntryPtr.key.key.macVlan.macAddr.arEther[5]);

    #endif
	/* <*, G, V>*/
	macEntryPtr.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E;
	macEntryPtr.key.key.ipMcast.vlanId = vlanId;
	memcpy(macEntryPtr.key.key.ipMcast.sip, anySip, 4);

	#if 0
	#ifdef DRV_LIB_CPSS_3_4
	macEntryPtr.key.key.ipMcast.dip[0] = dmip[0];
	macEntryPtr.key.key.ipMcast.dip[1] = dmip[1];
	macEntryPtr.key.key.ipMcast.dip[2] = dmip[2];
	macEntryPtr.key.key.ipMcast.dip[3] = dmip[3];
	#else
	#endif
	#endif
	
	macEntryPtr.key.key.ipMcast.dip[0] = dmip[3];
	macEntryPtr.key.key.ipMcast.dip[1] = dmip[2];
	macEntryPtr.key.key.ipMcast.dip[2] = dmip[1];
	macEntryPtr.key.key.ipMcast.dip[3] = dmip[0];

	macEntryPtr.dstInterface.type = CPSS_INTERFACE_VIDX_E;
	macEntryPtr.dstInterface.vlanId = vlanId;
	macEntryPtr.dstInterface.vidx = vidx;
	
	macEntryPtr.isStatic = NAM_TRUE;/* static FDB */
	macEntryPtr.daCommand = CPSS_MAC_TABLE_FRWRD_E;
	macEntryPtr.saCommand = CPSS_MAC_TABLE_DROP_E;
	macEntryPtr.daRoute = NAM_FALSE;
	macEntryPtr.mirrorToRxAnalyzerPortEn = NAM_FALSE;
	macEntryPtr.sourceID = 0;
	macEntryPtr.userDefined = 0;
	macEntryPtr.daQosIndex = 0;
	macEntryPtr.saQosIndex = 0;
	macEntryPtr.daSecurityLevel = 0;
	macEntryPtr.saSecurityLevel = 0;
	macEntryPtr.appSpecificCpuCode = NAM_TRUE;

	ret=cpssDxChBrgFdbMacEntrySet(devNum, &macEntryPtr);

	if(0 != ret) {
		nam_syslog_err("cpssDxChBrgFdbMacEntrySet! \n");
		nam_syslog_err("err vlaue is: %ld\n",ret);
		return FDB_CONFIG_FAIL;
	}
	#endif
	#if 0
	#ifdef DRV_LIB_BCM
   /*Attention: HERE USE VIDX AS L2MAST INDEX*/
	num_of_asic = nam_asic_get_instance_num();
	for (unit = 0; unit < num_of_asic; unit++) {
		memcpy(mcast.mac, mcMac, 6);
		mcast.vid = vlanId;
		mcast.l2mc_index = vidx;
		mcast.flags |= (BCM_L2_STATIC | BCM_L2_MCAST );  

		rv = bcm_mcast_addr_add_w_l2mcindex(unit, &mcast);
		if (NAM_E_NONE != rv) {
			syslog_ax_nam_fdb_err("Failed to set mcast to l2 for ret %d on unit %d\n",
									rv, unit);
			return FDB_CONFIG_FAIL;
		}
		memset(&mcast, 0, sizeof(bcm_mcast_addr_t));
	}
	#endif
	#endif

	return FDB_CONFIG_SUCCESS; 
}

int nam_static_fdb_entry_indirect_delete_for_mld
(
	unsigned int dip,
	unsigned short vid
) 
{

	nam_syslog_dbg("Get params: vlanId %d,groupIP 0x%x.\r\n",vid,dip);
	#ifdef DRV_LIB_CPSS
	unsigned long ret = 0;
	unsigned char devNum = 0;
	unsigned int hash = 0;
	unsigned int valid = 0,skip = 0;
	unsigned int aged = 0;
	unsigned char  associatedDevNum = 0;
	#endif
	unsigned char mcMac[6] = {0x33,0x33,0xff,0xff,0xff,0xff};
	
	#ifdef DRV_LIB_CPSS
	CPSS_MAC_ENTRY_EXT_STC	entryPtr;
	CPSS_MAC_ENTRY_EXT_KEY_STC macEntryKeyPtr;
	memset(&entryPtr,0,sizeof(CPSS_MAC_ENTRY_EXT_STC));
	memset(&macEntryKeyPtr,0,sizeof(CPSS_MAC_ENTRY_EXT_KEY_STC));
	#endif

	#ifdef DRV_LIB_BCM
	unsigned int rv = 0;
	unsigned int unit_i = 0;
	unsigned int num_of_asic = 0;
	
	#endif

	unsigned int iphost = 0x0;
	iphost = htonl(dip);
	nam_syslog_dbg("host order ip :iphost = 0x%x\r\n",iphost);

	unsigned int i;
	unsigned char dmip[4]={0};
	unsigned char tmp_dmip[4]={0};

	for(i=0;i<4;i++){
		dmip[i] = (iphost>>(i*8))&0xff; /*dip -->iphost*/
		}
	
	mcMac[2] &=dmip[3];
	mcMac[3] &=dmip[2];
	mcMac[4] &=dmip[1];
	mcMac[5] &=dmip[0];

	#ifdef DRV_LIB_CPSS
	ret= cpssDxChBrgFdbStaticDelEnable( devNum,NAM_TRUE);
	if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbStaticDelEnable:FAIL.\n");
			nam_syslog_err("err vlaue is: %ld\n",ret);
			return FDB_CONFIG_FAIL;
		}

#if 0
	macEntryKeyPtr.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
	macEntryKeyPtr.key.macVlan.vlanId = vid;
	//memcpy(&(macEntryPtr.key.key.macVlan.macAddr),mcMac,6);
	macEntryKeyPtr.key.macVlan.macAddr.arEther[0] = mcMac[0];
	macEntryKeyPtr.key.macVlan.macAddr.arEther[1] = mcMac[1];
	macEntryKeyPtr.key.macVlan.macAddr.arEther[2] = mcMac[2];
	macEntryKeyPtr.key.macVlan.macAddr.arEther[3] = mcMac[3];
	macEntryKeyPtr.key.macVlan.macAddr.arEther[4] = mcMac[4];
	macEntryKeyPtr.key.macVlan.macAddr.arEther[5] = mcMac[5];
	macEntryKeyPtr.key.macVlan.vlanId = vid;
	nam_syslog_dbg("#####################################\n");
	nam_syslog_dbg("Delete Multicast FDB entry MAC to HW as follow::\n");
	nam_syslog_dbg("%02x:%02x:%02x:%02x:%02x:%02x\r\n",\
								macEntryKeyPtr.key.macVlan.macAddr.arEther[0],\
								macEntryKeyPtr.key.macVlan.macAddr.arEther[1],\
								macEntryKeyPtr.key.macVlan.macAddr.arEther[2],\
								macEntryKeyPtr.key.macVlan.macAddr.arEther[3],\
								macEntryKeyPtr.key.macVlan.macAddr.arEther[4],\
								macEntryKeyPtr.key.macVlan.macAddr.arEther[5]);
#endif 

#if 1
	macEntryKeyPtr.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E;
	macEntryKeyPtr.key.ipMcast.vlanId = vid;
	memcpy(macEntryKeyPtr.key.ipMcast.sip,anySip,4);
	/*memcpy(macEntryKeyPtr.key.ipMcast.dip,dmip,4);*/

	#ifdef DRV_LIB_CPSS_3_4
	macEntryKeyPtr.key.ipMcast.dip[0] = dmip[0];
	macEntryKeyPtr.key.ipMcast.dip[1] = dmip[1];
	macEntryKeyPtr.key.ipMcast.dip[2] = dmip[2];
	macEntryKeyPtr.key.ipMcast.dip[3] = dmip[3];
	tmp_dmip[0] = dmip[0];
	tmp_dmip[1] = dmip[1];	
	tmp_dmip[2] = dmip[2];
	tmp_dmip[3] = dmip[3];
	#else
	macEntryKeyPtr.key.ipMcast.dip[0] = dmip[3];
	macEntryKeyPtr.key.ipMcast.dip[1] = dmip[2];
	macEntryKeyPtr.key.ipMcast.dip[2] = dmip[1];
	macEntryKeyPtr.key.ipMcast.dip[3] = dmip[0];
	tmp_dmip[0] = dmip[3];
	tmp_dmip[1] = dmip[2];	
	tmp_dmip[2] = dmip[1];
	tmp_dmip[3] = dmip[0];
	#endif

#endif
	ret = cpssDxChBrgFdbHashCalc(devNum,&macEntryKeyPtr,&hash);
	if(0 != ret){
		 nam_syslog_err("nam_static_fdb_entry_indirect_delete_for_igmp:: error to get the hash index of this entry\n");
		 nam_syslog_err("Error value is %d\n",ret);
		 return FDB_CONFIG_FAIL;
	}
	else{
		ret =  cpssDxChBrgFdbMacEntryRead(devNum,hash,&valid,&skip,&aged,&associatedDevNum,&entryPtr);
		if( 0 != ret){
			nam_syslog_err("nam_static_fdb_entry_indirect_delete_for_igmp:: error to read from the hash index entry\n");
			nam_syslog_err("Error value is %d\n",ret);
			return FDB_CONFIG_FAIL;
		}
		else{
			/*ret =nam_mac_match(entryPtr.key.key.macVlan.macAddr.arEther,mcMac,6);*/
			nam_syslog_dbg("entryPtr.key.key.ipMcast.dip[%d.%d.%d.%d] dip[%d.%d.%d.%d]\n",
							entryPtr.key.key.ipMcast.dip[0],
							entryPtr.key.key.ipMcast.dip[1],
							entryPtr.key.key.ipMcast.dip[2],
							entryPtr.key.key.ipMcast.dip[3],
							tmp_dmip[0],
							tmp_dmip[1],
							tmp_dmip[2],
							tmp_dmip[3]);
			ret =nam_mac_match(entryPtr.key.key.ipMcast.dip, tmp_dmip, 4);
			if((ret==1)&&(entryPtr.key.key.ipMcast.vlanId == vid)){
				 entryPtr.isStatic = 0;
				 ret = cpssDxChBrgFdbMacEntryWrite(devNum,hash,1,&entryPtr);
				 if(0 != ret) {
					nam_syslog_err("cpssDxChBrgFdbMacEntryWrite! \n");
					nam_syslog_err("err vlaue is: %ld\n",ret);
					return FDB_CONFIG_FAIL;
				}
			}
			else{
				nam_syslog_err("nam_static_fdb_entry_indirect_delete_for_igmp::The read value is error!\n");
				return FDB_CONFIG_FAIL;
			}
		}
	}
	#endif

	#ifdef DRV_LIB_BCM
	num_of_asic = nam_asic_get_instance_num();
	bcm_l2_addr_t find_t;
	memset(&find_t, 0, sizeof(bcm_l2_addr_t));

	for (unit_i = 0; unit_i < num_of_asic; unit_i++) {
		rv = bcm_l2_addr_get(unit_i, mcMac, vid, &find_t);
		if (NAM_E_NOT_FOUND == rv) {
			continue;
		}else if (NAM_E_NONE != rv) {
		   	syslog_ax_nam_fdb_err("nam_static_fdb_entry_indirect_delete_for_igmp: " \
								"Failed to find l2 mcast addr %02x:%02x:%02x:%02x:%02x:%02x on device %d ret %d\n",\
								mcMac[0], mcMac[1],	mcMac[2],
								mcMac[3], mcMac[4],	mcMac[5],
								unit_i, rv);
			return FDB_CONFIG_FAIL;
		}

		rv = bcm_mcast_addr_remove(unit_i, mcMac, vid);
		if ((NAM_E_NONE != rv) && (NAM_E_NOT_FOUND != rv)){
		   syslog_ax_nam_fdb_err("nam_static_fdb_entry_indirect_delete_for_igmp: "\
		   						"Failed to del l2 mcast addr on device % ret %d\n",
		   						unit_i, rv);
		   return FDB_CONFIG_FAIL;
		}	
		memset(&find_t, 0, sizeof(bcm_l2_addr_t));
	}
	#endif
	return FDB_CONFIG_SUCCESS;

}

/************************************************************
 *nam_static_fdb_entry_indirect_set_for_igmp
 * 
 * config the system static fdb item which for igmp protocol
 *
 *	input params:
 *		vlanid -which vlan index need config
 *		flag - judge the params is whether wrong
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/
 /*****************************************************************
  *API Below adopt the indirect access method to write FDB entry!!
  *By call Cpss Api :cpssDxChBrgFdbMacEntrySet 
  *	It'll Send Au message from host CPU to PP device.
  *
  ***wujh*********************************************************/
int nam_static_fdb_entry_indirect_set_for_igmp
(
	unsigned int dip,
	unsigned short vidx,
	unsigned short vlanId
) 
{

	unsigned long ret;
	unsigned char devNum = 0;
	unsigned char mcMac[6] = {0x01,0x00,0x5e,0x7f,0xff,0xff};
	unsigned int iphost = 0x0;

	iphost = htonl(dip);
	nam_syslog_dbg("network order ip :iphost = 0x%x\r\n",iphost);
	unsigned int i;
	unsigned char dmip[4]={0};
	for(i=0;i<4;i++){
		dmip[i] = (iphost>>(i*8))&0xff; /*dip -->iphost*/
		nam_syslog_err("dmip[%d] = 0x%x!\n",i,dmip[i]);
		}
	mcMac[3] &=dmip[2];
	mcMac[4] &=dmip[1];
	mcMac[5] &=dmip[0];
	
	#ifdef DRV_LIB_CPSS
	CPSS_MAC_ENTRY_EXT_STC	macEntryPtr;
	memset(&macEntryPtr, 0 ,sizeof(CPSS_MAC_ENTRY_EXT_STC));
	#endif

	#ifdef DRV_LIB_BCM
	int unit = 0,num_of_asic = 0,rv = 0;
	bcm_mcast_addr_t mcast;
	memset(&mcast, 0, sizeof(bcm_mcast_addr_t));
	#endif

	#ifdef DRV_LIB_CPSS
	nam_syslog_dbg("Get params: vlanId %d,vidx %d,groupIP 0x%x.\r\n",vlanId,vidx,dip);
    #if 0
	// <MAC, V>
	macEntryPtr.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
	macEntryPtr.key.key.macVlan.vlanId = vlanId;
	//memcpy(&(macEntryPtr.key.key.macVlan.macAddr),mcMac,6);
	macEntryPtr.key.key.macVlan.macAddr.arEther[0] = mcMac[0];
	macEntryPtr.key.key.macVlan.macAddr.arEther[1] = mcMac[1];
	macEntryPtr.key.key.macVlan.macAddr.arEther[2] = mcMac[2];
	macEntryPtr.key.key.macVlan.macAddr.arEther[3] = mcMac[3];
	macEntryPtr.key.key.macVlan.macAddr.arEther[4] = mcMac[4];
	macEntryPtr.key.key.macVlan.macAddr.arEther[5] = mcMac[5];
	macEntryPtr.key.key.macVlan.vlanId = vlanId;
	nam_syslog_err("#####################################\n");
	nam_syslog_err("Multicast MAC set to HW as follow::\n");
	nam_syslog_err("%02x:%02x:%02x:%02x:%02x:%02x\r\n",\
								macEntryPtr.key.key.macVlan.macAddr.arEther[0],\
								macEntryPtr.key.key.macVlan.macAddr.arEther[1],\
								macEntryPtr.key.key.macVlan.macAddr.arEther[2],\
								macEntryPtr.key.key.macVlan.macAddr.arEther[3],\
								macEntryPtr.key.key.macVlan.macAddr.arEther[4],\
								macEntryPtr.key.key.macVlan.macAddr.arEther[5]);

    #endif
	#if 1
	/* <*, G, V>*/
	macEntryPtr.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
	macEntryPtr.key.key.ipMcast.vlanId = vlanId;
	memcpy(macEntryPtr.key.key.ipMcast.sip, anySip, 4);


	/* use the old sequence for IGMP, houxx@autelan.com 2013-02-22 */
	macEntryPtr.key.key.ipMcast.dip[0] = dmip[3];
	macEntryPtr.key.key.ipMcast.dip[1] = dmip[2];
	macEntryPtr.key.key.ipMcast.dip[2] = dmip[1];
	macEntryPtr.key.key.ipMcast.dip[3] = dmip[0];

	#endif
	macEntryPtr.dstInterface.type = CPSS_INTERFACE_VIDX_E;
	macEntryPtr.dstInterface.vlanId = vlanId;
	macEntryPtr.dstInterface.vidx = vidx;
	
	macEntryPtr.isStatic = NAM_TRUE;/* static FDB */
	macEntryPtr.daCommand = CPSS_MAC_TABLE_FRWRD_E;
	macEntryPtr.saCommand = CPSS_MAC_TABLE_DROP_E;
	macEntryPtr.daRoute = NAM_FALSE;
	macEntryPtr.mirrorToRxAnalyzerPortEn = NAM_FALSE;
	macEntryPtr.sourceID = 0;
	macEntryPtr.userDefined = 0;
	macEntryPtr.daQosIndex = 0;
	macEntryPtr.saQosIndex = 0;
	macEntryPtr.daSecurityLevel = 0;
	macEntryPtr.saSecurityLevel = 0;
	macEntryPtr.appSpecificCpuCode = NAM_TRUE;

	ret=cpssDxChBrgFdbMacEntrySet(devNum, &macEntryPtr);

	if(0 != ret) {
		nam_syslog_err("cpssDxChBrgFdbMacEntrySet! \n");
		nam_syslog_err("err vlaue is: %ld\n",ret);
		return FDB_CONFIG_FAIL;
	}
	#endif
	#ifdef DRV_LIB_BCM
   /*Attention: HERE USE VIDX AS L2MAST INDEX*/
	num_of_asic = nam_asic_get_instance_num();
	for (unit = 0; unit < num_of_asic; unit++) {
		memcpy(mcast.mac, mcMac, 6);
		mcast.vid = vlanId;
		mcast.l2mc_index = vidx;
		mcast.flags |= (BCM_L2_STATIC | BCM_L2_MCAST );  

		rv = bcm_mcast_addr_add_w_l2mcindex(unit, &mcast);
		if (NAM_E_NONE != rv) {
			syslog_ax_nam_fdb_err("Failed to set mcast to l2 for ret %d on unit %d\n",
									rv, unit);
			return FDB_CONFIG_FAIL;
		}
		memset(&mcast, 0, sizeof(bcm_mcast_addr_t));
	}
	#endif

	return FDB_CONFIG_SUCCESS; 
}

int nam_static_fdb_entry_indirect_delete_for_igmp
(
	unsigned int dip,
	unsigned short vid
) 
{

	nam_syslog_dbg("Get params: vlanId %d,groupIP 0x%x.\r\n",vid,dip);
	#ifdef DRV_LIB_CPSS
	unsigned long ret = 0;
	unsigned char devNum = 0;
	unsigned int hash = 0;
	unsigned int valid = 0,skip = 0;
	unsigned int aged = 0;
	unsigned char  associatedDevNum = 0;
	#endif
	unsigned char mcMac[6] = {0x01,0x00,0x5e,0x7f,0xff,0xff};
	
	#ifdef DRV_LIB_CPSS
	CPSS_MAC_ENTRY_EXT_STC	entryPtr;
	CPSS_MAC_ENTRY_EXT_KEY_STC macEntryKeyPtr;
	memset(&entryPtr,0,sizeof(CPSS_MAC_ENTRY_EXT_STC));
	memset(&macEntryKeyPtr,0,sizeof(CPSS_MAC_ENTRY_EXT_KEY_STC));
	#endif

	#ifdef DRV_LIB_BCM
	unsigned int rv = 0;
	unsigned int unit_i = 0;
	unsigned int num_of_asic = 0;
	
	#endif

	unsigned int iphost = 0x0;
	iphost = htonl(dip);
	nam_syslog_dbg("host order ip :iphost = 0x%x\r\n",iphost);

	unsigned int i;
	unsigned char dmip[4]={0};
	unsigned char tmp_dmip[4]={0};

	for(i=0;i<4;i++){
		dmip[i] = (iphost>>(i*8))&0xff; /*dip -->iphost*/
		}
	mcMac[3] &=dmip[2];
	mcMac[4] &=dmip[1];
	mcMac[5] &=dmip[0];

	#ifdef DRV_LIB_CPSS
	ret= cpssDxChBrgFdbStaticDelEnable( devNum,NAM_TRUE);
	if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbStaticDelEnable:FAIL.\n");
			nam_syslog_err("err vlaue is: %ld\n",ret);
			return FDB_CONFIG_FAIL;
		}

#if 0
	macEntryKeyPtr.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
	macEntryKeyPtr.key.macVlan.vlanId = vid;
	//memcpy(&(macEntryPtr.key.key.macVlan.macAddr),mcMac,6);
	macEntryKeyPtr.key.macVlan.macAddr.arEther[0] = mcMac[0];
	macEntryKeyPtr.key.macVlan.macAddr.arEther[1] = mcMac[1];
	macEntryKeyPtr.key.macVlan.macAddr.arEther[2] = mcMac[2];
	macEntryKeyPtr.key.macVlan.macAddr.arEther[3] = mcMac[3];
	macEntryKeyPtr.key.macVlan.macAddr.arEther[4] = mcMac[4];
	macEntryKeyPtr.key.macVlan.macAddr.arEther[5] = mcMac[5];
	macEntryKeyPtr.key.macVlan.vlanId = vid;
	nam_syslog_dbg("#####################################\n");
	nam_syslog_dbg("Delete Multicast FDB entry MAC to HW as follow::\n");
	nam_syslog_dbg("%02x:%02x:%02x:%02x:%02x:%02x\r\n",\
								macEntryKeyPtr.key.macVlan.macAddr.arEther[0],\
								macEntryKeyPtr.key.macVlan.macAddr.arEther[1],\
								macEntryKeyPtr.key.macVlan.macAddr.arEther[2],\
								macEntryKeyPtr.key.macVlan.macAddr.arEther[3],\
								macEntryKeyPtr.key.macVlan.macAddr.arEther[4],\
								macEntryKeyPtr.key.macVlan.macAddr.arEther[5]);
#endif 

#if 1
	macEntryKeyPtr.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
	macEntryKeyPtr.key.ipMcast.vlanId = vid;
	memcpy(macEntryKeyPtr.key.ipMcast.sip,anySip,4);
	/*memcpy(macEntryKeyPtr.key.ipMcast.dip,dmip,4);*/

	/* use the old sequence for IGMP, houxx@autelan.com 2013-02-22 */
	macEntryKeyPtr.key.ipMcast.dip[0] = dmip[3];
	macEntryKeyPtr.key.ipMcast.dip[1] = dmip[2];
	macEntryKeyPtr.key.ipMcast.dip[2] = dmip[1];
	macEntryKeyPtr.key.ipMcast.dip[3] = dmip[0];
	tmp_dmip[0] = dmip[3];
	tmp_dmip[1] = dmip[2];	
	tmp_dmip[2] = dmip[1];
	tmp_dmip[3] = dmip[0];

#endif
	ret = cpssDxChBrgFdbHashCalc(devNum,&macEntryKeyPtr,&hash);
	if(0 != ret){
		 nam_syslog_err("nam_static_fdb_entry_indirect_delete_for_igmp:: error to get the hash index of this entry\n");
		 nam_syslog_err("Error value is %d\n",ret);
		 return FDB_CONFIG_FAIL;
	}
	else{
		ret =  cpssDxChBrgFdbMacEntryRead(devNum,hash,&valid,&skip,&aged,&associatedDevNum,&entryPtr);
		if( 0 != ret){
			nam_syslog_err("nam_static_fdb_entry_indirect_delete_for_igmp:: error to read from the hash index entry\n");
			nam_syslog_err("Error value is %d\n",ret);
			return FDB_CONFIG_FAIL;
		}
		else{
			/*ret =nam_mac_match(entryPtr.key.key.macVlan.macAddr.arEther,mcMac,6);*/
			nam_syslog_dbg("entryPtr.key.key.ipMcast.dip[%d.%d.%d.%d] dip[%d.%d.%d.%d]\n",
							entryPtr.key.key.ipMcast.dip[0],
							entryPtr.key.key.ipMcast.dip[1],
							entryPtr.key.key.ipMcast.dip[2],
							entryPtr.key.key.ipMcast.dip[3],
							tmp_dmip[0],
							tmp_dmip[1],
							tmp_dmip[2],
							tmp_dmip[3]);
			ret =nam_mac_match(entryPtr.key.key.ipMcast.dip, tmp_dmip, 4);
			if((ret==1)&&(entryPtr.key.key.ipMcast.vlanId == vid)){
				 entryPtr.isStatic = 0;
				 ret = cpssDxChBrgFdbMacEntryWrite(devNum,hash,1,&entryPtr);
				 if(0 != ret) {
					nam_syslog_err("cpssDxChBrgFdbMacEntryWrite! \n");
					nam_syslog_err("err vlaue is: %ld\n",ret);
					return FDB_CONFIG_FAIL;
				}
			}
			else{
				nam_syslog_err("nam_static_fdb_entry_indirect_delete_for_igmp::The read value is error!\n");
				return FDB_CONFIG_FAIL;
			}
		}
	}
	#endif

	#ifdef DRV_LIB_BCM
	num_of_asic = nam_asic_get_instance_num();
	bcm_l2_addr_t find_t;
	memset(&find_t, 0, sizeof(bcm_l2_addr_t));

	for (unit_i = 0; unit_i < num_of_asic; unit_i++) {
		rv = bcm_l2_addr_get(unit_i, mcMac, vid, &find_t);
		if (NAM_E_NOT_FOUND == rv) {
			continue;
		}else if (NAM_E_NONE != rv) {
		   	syslog_ax_nam_fdb_err("nam_static_fdb_entry_indirect_delete_for_igmp: " \
								"Failed to find l2 mcast addr %02x:%02x:%02x:%02x:%02x:%02x on device %d ret %d\n",\
								mcMac[0], mcMac[1],	mcMac[2],
								mcMac[3], mcMac[4],	mcMac[5],
								unit_i, rv);
			return FDB_CONFIG_FAIL;
		}

		rv = bcm_mcast_addr_remove(unit_i, mcMac, vid);
		if ((NAM_E_NONE != rv) && (NAM_E_NOT_FOUND != rv)){
		   syslog_ax_nam_fdb_err("nam_static_fdb_entry_indirect_delete_for_igmp: "\
		   						"Failed to del l2 mcast addr on device % ret %d\n",
		   						unit_i, rv);
		   return FDB_CONFIG_FAIL;
		}	
		memset(&find_t, 0, sizeof(bcm_l2_addr_t));
	}
	#endif
	return FDB_CONFIG_SUCCESS;

}


/************************************************************
 *nam_static_fdb_entry_mac_vlan_port_set
 * 
 * config the system static fdb table forwarding item 
 *
 *	input params:
 *		macAddr[] -forwarding destination mac address
 *		vlanId- the mac address belong to the valid vlan index
 * 		globle_index- get form slot/port value
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/
int nam_static_fdb_entry_mac_vlan_port_set
(
	unsigned char macAddr[],
	unsigned short vlanId,
	unsigned int globle_index
) 
{
#ifdef DRV_LIB_BCM
    int rv = 0,ret = 0;
	int num_of_asic = 0,mod = 0;
	unsigned char unit = 0,port = 0;
	bcm_l2_addr_t		l2addr;
    memset(&l2addr,0,sizeof(bcm_l2_addr_t));
#endif

#ifdef DRV_LIB_CPSS
	unsigned long ret = 0;
	unsigned int flag = 0;
	unsigned int hashPtr = 0;
	NAM_BOOL skip = NAM_FALSE;
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	unsigned int actDev = 0;
	unsigned char asic_num = 0,ammount_asic_nums = 0;
	CPSS_MAC_ENTRY_EXT_STC		     macEntryPtr;
	ETHERADDR  *macPtr = NULL;
	unsigned int					 validPtr;
	unsigned int					 skipPtr;
	NAM_BOOL					     agedPtr;
	unsigned char					 associatedDevNumPtr;
	CPSS_MAC_ENTRY_EXT_STC		     readEntryPtr;
	memset(&macEntryPtr, 0 ,sizeof(CPSS_MAC_ENTRY_EXT_STC));
#endif

#ifdef DRV_LIB_CPSS
	ret = npd_get_devport_by_global_index(globle_index,&devNum, &portNum);
    if(NAM_OK != ret){
		syslog_ax_nam_fdb_err("eth_index %#x get asic port failed for ret %d\n",globle_index,ret);
        return FDB_CONFIG_FAIL;
	}

	macEntryPtr.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
	macPtr = &(macEntryPtr.key.key.macVlan.macAddr);
	memcpy(macPtr->arEther,macAddr,6);
	macEntryPtr.key.key.macVlan.vlanId = (unsigned short)vlanId;

	ret=cpssDxChBrgFdbHashCalc( devNum,&macEntryPtr.key, &hashPtr);

	if( ret!=0){
		nam_syslog_err("cpssDxChBrgFdbHashCalc erro is :%ld\n",ret);
		return FDB_CONFIG_FAIL;
		}
	ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
													 &skipPtr,&agedPtr,&associatedDevNumPtr,&readEntryPtr); 

	nam_syslog_dbg("The fdb hash index is %d\n",hashPtr);
	if( ret!=0){
		nam_syslog_err("cpssDxChBrgFdbMacEntryRead erro is :%ld\n",ret);
		return FDB_CONFIG_FAIL;
	}
	/* If find error, repeat the lookup progress max to    4*/
	if(1 == validPtr)
	{
		if(skipPtr)
		{
			;
		}
		else if((readEntryPtr.isStatic == 0)&&(readEntryPtr.key.key.macVlan.vlanId == vlanId)&&
									   ( !memcmp(readEntryPtr.key.key.macVlan.macAddr.arEther,macAddr,6)))
		{
			;
		}
		else 
		{
			while((1 == validPtr)&&(flag < 3))
			{
				if(readEntryPtr.isStatic == 1)
				{
					;
				}
				else 
				{
					break;
				}
				nam_syslog_err("This index %d has been used by another static entry and find another index,validPtr %d,skipPtr %d ,agedPtr %d !\n",hashPtr,validPtr,skipPtr,agedPtr);
				hashPtr++;
				flag++;
				ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
														 &skipPtr,&agedPtr,&associatedDevNumPtr,&readEntryPtr); 
			}
			if( !(flag < 3))
			{
				nam_syslog_err("nam_static_fdb_entry_mac_vlan_port_set find index for the static fdb set error!\n");
				return FDB_CONFIG_FAIL;
			}
	  }
	}
	macEntryPtr.dstInterface.type = CPSS_INTERFACE_PORT_E;
	#if 0
	if(IS_DISTRIBUTED == SYSTEM_TYPE)
	{
		ret = nam_get_distributed_devnum(asic_num,&actDev);
		if(ret != FDB_CONFIG_SUCCESS)
		{
			nam_syslog_dbg("get distributed dev num failure !!! \n");
		}

		macEntryPtr.dstInterface.devPort.devNum = actDev;
	}
	else
	{
		macEntryPtr.dstInterface.devPort.devNum = devNum;
	}
	#endif
	macEntryPtr.dstInterface.devPort.devNum = g_dis_dev[0];
	macEntryPtr.dstInterface.devPort.portNum = portNum;
	NAM_CONVERT_DEV_PORT_MAC(macEntryPtr.dstInterface.devPort.devNum,
							 macEntryPtr.dstInterface.devPort.portNum);
	macEntryPtr.dstInterface.trunkId=0;
	macEntryPtr.dstInterface.vidx=0;
	macEntryPtr.dstInterface.vlanId=0;
	macEntryPtr.isStatic = 0x1; /*NAM_TRUE; static FDB */
	macEntryPtr.daCommand = CPSS_MAC_TABLE_FRWRD_E;
	macEntryPtr.saCommand = CPSS_MAC_TABLE_FRWRD_E;
	macEntryPtr.daRoute =NAM_FALSE;/* no L3 route */
	macEntryPtr.mirrorToRxAnalyzerPortEn = NAM_FALSE;
	macEntryPtr.sourceID = 0;
	macEntryPtr.userDefined = 0;
	macEntryPtr.daQosIndex = 0;
	macEntryPtr.saQosIndex = 0;
	macEntryPtr.daSecurityLevel = 0;
	macEntryPtr.saSecurityLevel = 0;
	macEntryPtr.appSpecificCpuCode = NAM_FALSE;

	nam_syslog_dbg("nam_static_fdb_entry_mac_vlan_port_set: macEntryPtr.isStatic = %x\n",
					macEntryPtr.isStatic);

	ret=cpssDxChBrgFdbMacEntryWrite(devNum, hashPtr, skip, &macEntryPtr);

	if(0 != ret) 
	{
		nam_syslog_err("cpssDxChBrgStaticFdbMacEntrySet erro! \n");
		nam_syslog_err("err vlaue is: %ld\n",ret);
		return FDB_CONFIG_FAIL;
	}
	else
	{
		memset(&readEntryPtr, 0 ,sizeof(CPSS_MAC_ENTRY_EXT_STC));
		ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
															 &skipPtr,&agedPtr,&associatedDevNumPtr,&readEntryPtr); 
		if( ret!=0)
		{
			nam_syslog_err("cpssDxChBrgFdbMacEntryRead after write erro is :%ld\n",ret);
			return FDB_CONFIG_FAIL;
		}
		else
		{
			nam_syslog_dbg("The hashindex is:%d,valid: %d,skip:  %d, static: %d, mac: %02x:%02x:%02x:%02x:%02x:%02x\n",
											   hashPtr,validPtr,skipPtr,readEntryPtr.isStatic,readEntryPtr.key.key.macVlan.macAddr.arEther[0],
											   readEntryPtr.key.key.macVlan.macAddr.arEther[1],readEntryPtr.key.key.macVlan.macAddr.arEther[2],
											   readEntryPtr.key.key.macVlan.macAddr.arEther[3],readEntryPtr.key.key.macVlan.macAddr.arEther[4],
											   readEntryPtr.key.key.macVlan.macAddr.arEther[5]);
		}
	}
	return FDB_CONFIG_SUCCESS;
#endif

#ifdef DRV_LIB_BCM
 
	ret = npd_get_devport_by_global_index(globle_index, &unit,&port);
    if(NAM_OK != ret){
		syslog_ax_nam_fdb_err("eth_index %#x get asic port failed for ret %d\n",globle_index,ret);
        return FDB_CONFIG_FAIL;
	}
	num_of_asic = nam_asic_get_instance_num();
	ret = bcm_stk_modid_get(unit,&mod);
	if(0!=ret){
       syslog_ax_nam_fdb_dbg("Failed to get modid of unit %d,ret %d\n",unit,ret);
       return FDB_CONFIG_FAIL;
	}
	for(unit = 0;unit < num_of_asic;unit++){
	   ret = bcm_l2_addr_get(unit,macAddr,vlanId,&l2addr);
	   if(NAM_OK == ret){/*there is already entry here,update it*/
		   if(l2addr.flags & BCM_L2_STATIC){/*former is static already,do nothing*/
		       return FDB_CONFIG_FAIL;
		   }
		   else {/*update it*/
		     l2addr.port = port;
			 l2addr.modid = mod;
		     l2addr.flags |= BCM_L2_STATIC | BCM_L2_REPLACE_DYNAMIC;
		     ret= bcm_l2_addr_add(unit,&l2addr);
		   }
		}
		else{/*if not find former entry,create one and add it.*/
	        bcm_l2_addr_t_init(&l2addr,macAddr,vlanId);
	        l2addr.port = port;
			l2addr.modid = mod;
			l2addr.flags |= BCM_L2_STATIC;		
			if ( FDB_CONFIG_SUCCESS != (ret = bcm_l2_addr_add(unit,&l2addr))) {
				syslog_ax_nam_fdb_err("set fdb static mac %02x:%02x:%02x:%02x:%02x:%02x: vlan %d port %d failed!\n", \
					       macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5],vlanId,port);
			}
			else{
				syslog_ax_nam_fdb_dbg("set fdb static mac %02x:%02x:%02x:%02x:%02x:%02x: vlan %d port %d success!\n", \
					       macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5],vlanId,port);

			}
	    }
		NAM_RV_REPLACE_OK(ret, rv, NAM_E_UNAVAIL);
		nam_dump_l2_addr("Static:",&l2addr);
	}
	return rv;
#endif

}


int nam_static_fdb_entry_mac_vlan_port_set_for_debug
(
	unsigned char macAddr[],
	unsigned short vlanId,
	unsigned char devNum,
	unsigned char portNum
) 
{

#ifdef DRV_LIB_CPSS
	unsigned long ret = 0;
	unsigned int flag = 0;
	unsigned int hashPtr = 0;
	NAM_BOOL skip = NAM_FALSE;
	unsigned int actDev = 0;
	unsigned char asic_num = 0,ammount_asic_nums = 0;
	CPSS_MAC_ENTRY_EXT_STC		     macEntryPtr;
	ETHERADDR  *macPtr = NULL;
	unsigned int					 validPtr;
	unsigned int					 skipPtr;
	NAM_BOOL					     agedPtr;
	unsigned char					 associatedDevNumPtr;
	CPSS_MAC_ENTRY_EXT_STC		     readEntryPtr;
	memset(&macEntryPtr, 0 ,sizeof(CPSS_MAC_ENTRY_EXT_STC));
#endif

#ifdef DRV_LIB_CPSS

	nam_syslog_dbg("devnum = %d,portnum = %d\n",devNum,portNum);
	macEntryPtr.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
	macPtr = &(macEntryPtr.key.key.macVlan.macAddr);
	memcpy(macPtr->arEther,macAddr,6);
	macEntryPtr.key.key.macVlan.vlanId = (unsigned short)vlanId;

	ret=cpssDxChBrgFdbHashCalc( devNum,&macEntryPtr.key, &hashPtr);

	if( ret!=0){
		nam_syslog_err("cpssDxChBrgFdbHashCalc erro is :%ld\n",ret);
		return FDB_CONFIG_FAIL;
		}
	ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
													 &skipPtr,&agedPtr,&associatedDevNumPtr,&readEntryPtr); 

	nam_syslog_dbg("The fdb hash index is %d\n",hashPtr);
	if( ret!=0){
		nam_syslog_err("cpssDxChBrgFdbMacEntryRead erro is :%ld\n",ret);
		return FDB_CONFIG_FAIL;
	}
	/* If find error, repeat the lookup progress max to    4*/
	if(1 == validPtr)
	{
		if(skipPtr)
		{
			;
		}
		else if((readEntryPtr.isStatic == 0)&&(readEntryPtr.key.key.macVlan.vlanId == vlanId)&&
									   ( !memcmp(readEntryPtr.key.key.macVlan.macAddr.arEther,macAddr,6)))
		{
			;
		}
		else 
		{
			while((1 == validPtr)&&(flag < 3))
			{
				if(readEntryPtr.isStatic == 1)
				{
					;
				}
				else 
				{
					break;
				}
				nam_syslog_err("This index %d has been used by another static entry and find another index,validPtr %d,skipPtr %d ,agedPtr %d !\n",hashPtr,validPtr,skipPtr,agedPtr);
				hashPtr++;
				flag++;
				ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
														 &skipPtr,&agedPtr,&associatedDevNumPtr,&readEntryPtr); 
			}
			if( !(flag < 3))
			{
				nam_syslog_err("nam_static_fdb_entry_mac_vlan_port_set find index for the static fdb set error!\n");
				return FDB_CONFIG_FAIL;
			}
	  }
	}
	macEntryPtr.dstInterface.type = CPSS_INTERFACE_PORT_E;
	macEntryPtr.dstInterface.devPort.devNum = devNum;
	macEntryPtr.dstInterface.devPort.portNum = portNum;
	macEntryPtr.dstInterface.trunkId=0;
	macEntryPtr.dstInterface.vidx=0;
	macEntryPtr.dstInterface.vlanId=0;
	macEntryPtr.isStatic = 0x1; /*NAM_TRUE; static FDB */
	macEntryPtr.daCommand = CPSS_MAC_TABLE_FRWRD_E;
	macEntryPtr.saCommand = CPSS_MAC_TABLE_FRWRD_E;
	macEntryPtr.daRoute =NAM_FALSE;/* no L3 route */
	macEntryPtr.mirrorToRxAnalyzerPortEn = NAM_FALSE;
	macEntryPtr.sourceID = 0;
	macEntryPtr.userDefined = 0;
	macEntryPtr.daQosIndex = 0;
	macEntryPtr.saQosIndex = 0;
	macEntryPtr.daSecurityLevel = 0;
	macEntryPtr.saSecurityLevel = 0;
	macEntryPtr.appSpecificCpuCode = NAM_FALSE;

	nam_syslog_dbg("nam_static_fdb_entry_mac_vlan_port_set: macEntryPtr.isStatic = %x\n",
					macEntryPtr.isStatic);

	ret=cpssDxChBrgFdbMacEntryWrite(devNum, hashPtr, skip, &macEntryPtr);

	if(0 != ret) 
	{
		nam_syslog_err("cpssDxChBrgStaticFdbMacEntrySet erro! \n");
		nam_syslog_err("err vlaue is: %ld\n",ret);
		return FDB_CONFIG_FAIL;
	}
	else
	{
		memset(&readEntryPtr, 0 ,sizeof(CPSS_MAC_ENTRY_EXT_STC));
		ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
															 &skipPtr,&agedPtr,&associatedDevNumPtr,&readEntryPtr); 
		if( ret!=0)
		{
			nam_syslog_err("cpssDxChBrgFdbMacEntryRead after write erro is :%ld\n",ret);
			return FDB_CONFIG_FAIL;
		}
		else
		{
			nam_syslog_dbg("The hashindex is:%d,valid: %d,skip:  %d, static: %d, mac: %02x:%02x:%02x:%02x:%02x:%02x\n",
											   hashPtr,validPtr,skipPtr,readEntryPtr.isStatic,readEntryPtr.key.key.macVlan.macAddr.arEther[0],
											   readEntryPtr.key.key.macVlan.macAddr.arEther[1],readEntryPtr.key.key.macVlan.macAddr.arEther[2],
											   readEntryPtr.key.key.macVlan.macAddr.arEther[3],readEntryPtr.key.key.macVlan.macAddr.arEther[4],
											   readEntryPtr.key.key.macVlan.macAddr.arEther[5]);
		}
	}
	return FDB_CONFIG_SUCCESS;
#endif
}

#ifndef AX_PLATFORM_DISTRIBUTED
int nam_static_fdb_entry_mac_vlan_port_set_distributed
(
	unsigned char macAddr[],
	unsigned short vlanId,
	unsigned int devNum,
	unsigned int portNum
) 
{

#ifdef DRV_LIB_CPSS
  	int	ret;
	ret= cpssDxChBrgStaticFdbMacEntrySet(1, macAddr,vlanId, devNum,portNum,NPD_FALSE);
	if(0 != ret) {
		nam_syslog_err("add static fdb for l3 error %d\n", ret);
		return FDB_CONFIG_FAIL;
	}
	return FDB_CONFIG_SUCCESS;
#endif
}

#endif

int nam_fdb_static_system_source_mac_add
(
	unsigned short vlanId,
	unsigned int   if_flag
)
{
	unsigned char macAddr[6];
	memset(&macAddr, 0 ,sizeof(unsigned char)*6);
    
#ifdef DRV_LIB_BCM
		int rv = 0,ret = 0;
		int num_of_asic = 0,mod = 0;
		unsigned char unit = 0,port = 0;
		bcm_l2_addr_t		l2addr;
		memset(&l2addr,0,sizeof(bcm_l2_addr_t));
#endif
	
#ifdef DRV_LIB_CPSS
		unsigned long ret = 0;
		unsigned int flag = 0;
		unsigned int hashPtr = 0;
		NAM_BOOL skip = NAM_FALSE;
		unsigned char devNum = 0;
		unsigned char portNum = 0;
		CPSS_MAC_ENTRY_EXT_STC		 macEntryPtr;
		ETHERADDR  *macPtr = NULL;
		unsigned int					 validPtr;
		unsigned int					 skipPtr;
		NAM_BOOL					 agedPtr;
		unsigned char					 associatedDevNumPtr;
		CPSS_MAC_ENTRY_EXT_STC		 readEntryPtr;
		memset(&macEntryPtr, 0 ,sizeof(CPSS_MAC_ENTRY_EXT_STC));
#endif
	   ret = npd_system_get_basemac(macAddr,6);
#ifdef DRV_LIB_CPSS
	
		macEntryPtr.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
		macPtr = &(macEntryPtr.key.key.macVlan.macAddr);
		memcpy(macPtr->arEther,macAddr,6);
		macEntryPtr.key.key.macVlan.vlanId = (unsigned short)vlanId;
	
		ret=cpssDxChBrgFdbHashCalc(devNum,&macEntryPtr.key,&hashPtr);
	
		if( ret!=0){
			nam_syslog_err("cpssDxChBrgFdbHashCalc erro is :%ld\n",ret);
			return FDB_CONFIG_FAIL;
		}
		ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
														 &skipPtr,&agedPtr,&associatedDevNumPtr,&readEntryPtr); 
	
		nam_syslog_dbg("The fdb hash index is %d\n",hashPtr);
		if( ret!=0){
			nam_syslog_err("cpssDxChBrgFdbMacEntryRead erro is :%ld\n",ret);
			return FDB_CONFIG_FAIL;
		}
		if(if_flag){
			nam_syslog_dbg("The hashindex is:%d,valid: %d,skip:  %d, static: %d, mac: %02x:%02x:%02x:%02x:%02x:%02x\n",
											   hashPtr,validPtr,skipPtr,readEntryPtr.isStatic,readEntryPtr.key.key.macVlan.macAddr.arEther[0],
											   readEntryPtr.key.key.macVlan.macAddr.arEther[1],readEntryPtr.key.key.macVlan.macAddr.arEther[2],
											   readEntryPtr.key.key.macVlan.macAddr.arEther[3],readEntryPtr.key.key.macVlan.macAddr.arEther[4],
											   readEntryPtr.key.key.macVlan.macAddr.arEther[5]);

		}
		/* If find error, repeat the lookup progress max to    4*/
		else if(1 == validPtr){
			if(skipPtr){
				;
			}
			else if((readEntryPtr.key.key.macVlan.vlanId == vlanId)&&
										   ( !memcmp(readEntryPtr.key.key.macVlan.macAddr.arEther,macAddr,6))){
               ;
			}
			else {
				while((1 == validPtr)&&(flag < 4)){
					if(readEntryPtr.isStatic == 1){
						;
					}
					else {
						break;
					}
					nam_syslog_err("This index %d has been used by another static entry and find another index,validPtr %d,skipPtr %d ,agedPtr %d !\n",hashPtr,validPtr,skipPtr,agedPtr);
					hashPtr++;
					flag++;
					ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
															 &skipPtr,&agedPtr,&associatedDevNumPtr,&readEntryPtr); 
				}
				if( !(flag < 4)){
					nam_syslog_err("nam_fdb_static_system_source_mac_add find index for the static fdb set error!\n");
					return FDB_CONFIG_FAIL;
				}
		  }
		}

		if(if_flag){
			memcpy(&macEntryPtr,&readEntryPtr,sizeof(CPSS_MAC_ENTRY_EXT_STC));
			macEntryPtr.saCommand = CPSS_MAC_TABLE_CNTL_E;
		}
		else {
			macEntryPtr.dstInterface.type = CPSS_INTERFACE_PORT_E;
			macEntryPtr.dstInterface.devPort.devNum = 0;
			macEntryPtr.dstInterface.devPort.portNum = 63;
			NAM_CONVERT_DEV_PORT_MAC(macEntryPtr.dstInterface.devPort.devNum,
								 macEntryPtr.dstInterface.devPort.portNum);
			macEntryPtr.isStatic = NAM_TRUE;/* static FDB */
			macEntryPtr.daCommand = CPSS_MAC_TABLE_FRWRD_E;
			macEntryPtr.saCommand = CPSS_MAC_TABLE_CNTL_E;
			macEntryPtr.daRoute =NAM_FALSE;/* no L3 route */
			macEntryPtr.mirrorToRxAnalyzerPortEn = NAM_FALSE;
			macEntryPtr.sourceID = 0;
			macEntryPtr.userDefined = 0;
			macEntryPtr.daQosIndex = 0;
			macEntryPtr.saQosIndex = 0;
			macEntryPtr.daSecurityLevel = 0;
			macEntryPtr.saSecurityLevel = 0;
			macEntryPtr.appSpecificCpuCode = NAM_FALSE;
		}
		ret=cpssDxChBrgFdbMacEntryWrite(devNum, hashPtr, skip, &macEntryPtr);
	
		if(0 != ret) {
			nam_syslog_err("cpssDxChBrgStaticFdbMacEntrySet erro! \n");
			nam_syslog_err("err vlaue is: %ld\n",ret);
			return FDB_CONFIG_FAIL;
		}
		return FDB_CONFIG_SUCCESS;
#endif
	
#ifdef DRV_LIB_BCM
	 
		num_of_asic = nam_asic_get_instance_num();
		ret = bcm_stk_modid_get(unit,&mod);
		if(0!=ret){
		   syslog_ax_nam_fdb_dbg("Failed to get modid of unit %d,ret %d\n",unit,ret);
		   return FDB_CONFIG_FAIL;
		}
		for(unit = 0;unit < num_of_asic;unit++){
		   ret = bcm_l2_addr_get(unit,macAddr,vlanId,&l2addr);
		   if(NAM_OK == ret){/*there is already entry here,update it*/
		       if(if_flag){
			   	   ;
		       }
			   else {/*update it*/
				 l2addr.flags = BCM_L2_STATIC | BCM_L2_LOCAL_CPU;
				 ret= bcm_l2_addr_add(unit,&l2addr);
			   }
			}
			else{/*if not find former entry,create one and add it.*/
				bcm_l2_addr_t_init(&l2addr,macAddr,vlanId);
				l2addr.port = port;
				l2addr.modid = mod;
				l2addr.flags = BCM_L2_STATIC | BCM_L2_LOCAL_CPU;		
				if ( FDB_CONFIG_SUCCESS != (ret = bcm_l2_addr_add(unit,&l2addr))) {
					syslog_ax_nam_fdb_err("set fdb static mac %02x:%02x:%02x:%02x:%02x:%02x: vlan %d port %d failed!\n", \
							   macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5],vlanId,port);

				}
				else{
					syslog_ax_nam_fdb_dbg("set fdb static mac %02x:%02x:%02x:%02x:%02x:%02x: vlan %d port %d success!\n", \
							   macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5],vlanId,port);					
				}
			}
			/*trap station move packet to cpu*/
			ret = bcm_switch_control_port_set(unit,0,bcmSwitchL2StaticMoveToCpu,1);
			if(0 != ret){
               nam_syslog_dbg("set station move to cpu failed for %d\n",ret);
			   return ret;
			}
			NAM_RV_REPLACE_OK(ret, rv, NAM_E_UNAVAIL);
			nam_dump_l2_addr("Static:",&l2addr);
		}
		return rv;
#endif

}
/************************************************************
 *nam_fdb_static_delete_by_port_vlan
 * 
 * delete the special static fdb from HW
 *
 *	input params:
 * 		vlanId- the valid vlan index
 * 		globle_index- get form slot/port value
 *	output: none
 *
 * 	return:
 *		 FDB_CONFIG_FAIL- if there exist erro
 *		 FDB_CONFIG_SUCCESS- if operation ok
 *		
 *********************************************/


int nam_fdb_static_delete_by_port_vlan(unsigned short vlanId,int eth_g_index)
{
    unsigned int            ret = 0;
	unsigned int 			index = 0;
	unsigned int			result=0;
	unsigned int         	i = 0;
	unsigned char			devNum =0;
	unsigned int            actDev = 0;
	unsigned char            portNum = 0;
	unsigned int        	validPtr=0;
    unsigned int       	skipPtr= 0;
    NAM_BOOL                agedPtr;
	unsigned char			associatedDevNumPtr;
	unsigned char   asic_num = 0,ammount_asic_nums = 0;
	CPSS_MAC_ENTRY_EXT_STC	entryPtr;
	char  macComp[13]={0};
	unsigned char macAddr[6] = {0};


#ifdef DRV_LIB_CPSS
	ret = npd_get_devport_by_global_index(eth_g_index,&devNum, &portNum);
    if(NAM_OK != ret){
		syslog_ax_nam_fdb_err("eth_index %#x get asic port failed for ret %d\n",index,ret);
        return FDB_CONFIG_FAIL;
	}	
	
	if(IS_DISTRIBUTED == SYSTEM_TYPE)
	{
		ret = nam_get_distributed_devnum(asic_num,&actDev);
		if(ret != FDB_CONFIG_SUCCESS)
		{
			nam_syslog_dbg("get distributed dev num failure !!! \n");
		}
	}
	do
	{
		 /* call cpss api function */
		memset(&entryPtr,0,sizeof(CPSS_MAC_ENTRY_EXT_STC));
		result = cpssDxChBrgFdbMacEntryRead(devNum, index, &validPtr, &skipPtr,
										&agedPtr, &associatedDevNumPtr, &entryPtr);

		/*check if the fdb item is valid of useful data or is static*/
		if((!validPtr) ||(skipPtr)||(1 != entryPtr.isStatic)) 
		{
			index++;
			continue;
		}

		

		/*if is one  MULTICAST, do not delete*/
        if((entryPtr.key.entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E)||
			                      (entryPtr.key.entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E) ){
			index++;
			continue;
		}

        /* if is one trap to cpu, do not delete*/
		if(entryPtr.daCommand==CPSS_MAC_TABLE_CNTL_E){
			index++;
			continue;
		}

		/*check the vlan*/
		if( vlanId != entryPtr.key.key.macVlan.vlanId){
           index++;
		   continue;
		}
		/* check the port*/
		if((portNum != entryPtr.dstInterface.devPort.portNum)||(actDev != entryPtr.dstInterface.devPort.devNum)){
        	index++;
			continue;
		}
	    if (result == 0) 
		{
		 	/* if is one blacklist or IGMP, do not delete*/
			if((CPSS_MAC_TABLE_DROP_E==entryPtr.daCommand)||(CPSS_MAC_TABLE_DROP_E == entryPtr.saCommand))
			{
	   			index++;
	            continue;
			}
			else
			{
	            /*delete the entry*/
				/*check if the sys mac*/
				memcpy(macAddr,entryPtr.key.key.macVlan.macAddr.arEther,6);
						sprintf(macComp,"%02x%02x%02x%02x%02x%02x", \
						macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
	            ret = npd_system_verify_basemac(macComp);
				if(0 == ret)
				{
					/* not the sys mac */
					entryPtr.isStatic = 0;
					ret = cpssDxChBrgFdbMacEntryWrite(devNum,index,1,&entryPtr);
					if(0 != ret)
					{
						 nam_syslog_err("nam_fdb_static_table_delete_entry_with_vlan: error to delete the static entry , index: %d\n",index);
						 return FDB_CONFIG_FAIL;
					}
					i++;
				}
				
			}
			index++;
	   }
		else 
		{
			index++;
			continue;
		}
	}while((!result)&&(index<16384));
    nam_syslog_dbg("The number delete fdb static is %d\n",i);
    return FDB_CONFIG_SUCCESS;	
#endif
}

int nam_fdb_static_system_source_mac_del
(
	unsigned short vlanId,
	unsigned int   if_flag
)
{
	unsigned char macAddr[6];
	memset(&macAddr, 0 ,sizeof(unsigned char)*6);
    
#ifdef DRV_LIB_BCM
		int rv = 0,ret = 0;
		int num_of_asic = 0,mod = 0;
		unsigned char unit = 0,port = 0;
		bcm_l2_addr_t		l2addr;
		memset(&l2addr,0,sizeof(bcm_l2_addr_t));
#endif
	
#ifdef DRV_LIB_CPSS
		unsigned long ret = 0;
		unsigned int flag = 0;
		unsigned int hashPtr = 0;
		NAM_BOOL skip = NAM_FALSE;
		unsigned char devNum = 0;
		unsigned char portNum = 0;
		CPSS_MAC_ENTRY_EXT_STC		 macEntryPtr;
		ETHERADDR  *macPtr = NULL;
		unsigned int					 validPtr;
		unsigned int					 skipPtr;
		NAM_BOOL					 agedPtr;
		unsigned char					 associatedDevNumPtr;
		CPSS_MAC_ENTRY_EXT_STC		 readEntryPtr;
		memset(&macEntryPtr, 0 ,sizeof(CPSS_MAC_ENTRY_EXT_STC));
#endif
	   ret = npd_system_get_basemac(macAddr,6);
#ifdef DRV_LIB_CPSS
	
		macEntryPtr.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
		macPtr = &(macEntryPtr.key.key.macVlan.macAddr);
		memcpy(macPtr->arEther,macAddr,6);
		macEntryPtr.key.key.macVlan.vlanId = (unsigned short)vlanId;
	
		ret= cpssDxChBrgFdbStaticDelEnable( devNum,NAM_TRUE);
		if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbStaticDelEnable:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			return FDB_CONFIG_FAIL;
		}
		ret=cpssDxChBrgFdbHashCalc(devNum,&macEntryPtr, &hashPtr);
		if( ret!=0){
			nam_syslog_err("cpssDxChBrgFdbHashCalc erro is :%d",ret);
			return FDB_CONFIG_FAIL;
			}
		ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
	                                         &skipPtr,&agedPtr,&associatedDevNumPtr,&readEntryPtr);
		
		if( ret!=0){
			nam_syslog_err("cpssDxChBrgFdbMacEntryRead erro is :%d",ret);
			return FDB_CONFIG_FAIL;
		}
        if(if_flag){
            readEntryPtr.saCommand = CPSS_MAC_TABLE_FRWRD_E;
			
			ret = cpssDxChBrgFdbMacEntryWrite(devNum,hashPtr,0,&readEntryPtr);
			if( ret!=0){
				nam_syslog_err("cpssDxChBrgFdbMacEntrywrite erro is :%d",ret);
				return FDB_CONFIG_FAIL;
			}
		}
		else{
			/* If find error, repeat the lookup progress max to  4*/
			while(((readEntryPtr.isStatic == 0)||(readEntryPtr.key.key.macVlan.vlanId != vlanId)||
					   (memcmp(readEntryPtr.key.key.macVlan.macAddr.arEther,macAddr,6)))&&(flag < 4)){
				hashPtr++;
				flag++;
				ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
														 &skipPtr,&agedPtr,&associatedDevNumPtr,&readEntryPtr); 
			}
			if( !(flag < 4)){
				nam_syslog_err("nam_fdb_static_system_source_mac_del find index for the static fdb set error!\n");
				return FDB_CONFIG_FAIL;
			}
			readEntryPtr.isStatic = 0;
			ret = cpssDxChBrgFdbMacEntryWrite(devNum,hashPtr,1,&readEntryPtr);
			if( ret!=0){
				nam_syslog_err("cpssDxChBrgFdbMacEntrywrite erro is :%d",ret);
				return FDB_CONFIG_FAIL;
			}
		}		
		return FDB_CONFIG_SUCCESS;

#endif
	
#ifdef DRV_LIB_BCM
	 
		num_of_asic = nam_asic_get_instance_num();
		ret = bcm_stk_modid_get(unit,&mod);
		if(0!=ret){
		   syslog_ax_nam_fdb_dbg("Failed to get modid of unit %d,ret %d\n",unit,ret);
		   return FDB_CONFIG_FAIL;
		}
		for(unit = 0;unit < num_of_asic;unit++){
		   ret = bcm_l2_addr_get(unit,macAddr,vlanId,&l2addr);
		   if(NAM_OK == ret){/*there is already entry here,update it*/
			   if(if_flag){/*former is static already,do nothing*/
                  ;
			   }
			   else {/*update it */
				 bcm_l2_addr_delete(unit,macAddr,vlanId);
			   }
			}
			/*no trap station move packet to cpu*/
			
			/*TODO:: here should check if other vlans lldh enabled,if,not do this aciton!!!*/
			
			ret = bcm_switch_control_port_set(unit,0,bcmSwitchL2StaticMoveToCpu,0);
			if(0 != ret){
               nam_syslog_dbg("set station move to cpu failed for %d\n",ret);
			   return ret;
			}
			/*show*/
			NAM_RV_REPLACE_OK(ret, rv, NAM_E_UNAVAIL);
			nam_dump_l2_addr("Static:",&l2addr);
		}
		return rv;
#endif

}

/************************************************************
 *nam_static_fdb_entry_mac_vlan_trunk_set
 * 
 * config the system static fdb table forwarding item 
 *
 *	input params:
 *		macAddr[] -forwarding destination mac address
 *		vlanId- the mac address belong to the valid vlan index
 * 		trunkid- trunk Id
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/

unsigned int nam_static_fdb_entry_mac_vlan_trunk_set
(
	unsigned char macAddr[],
	unsigned short vlanId,
	unsigned short trunkId
) 
{
#ifdef DRV_LIB_BCM
    int ret = 0,rv = 0;
	int num_of_asic = 0,unit = 0,port = 0;
	bcm_l2_addr_t		l2addr;
    memset(&l2addr,0,sizeof(bcm_l2_addr_t));
#endif

#ifdef DRV_LIB_CPSS
  	unsigned long ret = 0;
	unsigned int hashPtr = 0;
	NAM_BOOL skip = NAM_FALSE;
	unsigned char devNum = 0;
	unsigned char asic_num = 0,ammount_asic_nums = 0;
	CPSS_MAC_ENTRY_EXT_STC       macEntryPtr;
	ETHERADDR  *macPtr = NULL;
	unsigned int					 validPtr;
	unsigned int					 skipPtr;
	NAM_BOOL					 agedPtr;
	unsigned char					 associatedDevNumPtr;
	CPSS_MAC_ENTRY_EXT_STC       readEntryPtr;
	unsigned char          actDev=0;
	memset(&macEntryPtr, 0 ,sizeof(CPSS_MAC_ENTRY_EXT_STC));
#endif

#ifdef DRV_LIB_CPSS
    macEntryPtr.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
	macPtr = &(macEntryPtr.key.key.macVlan.macAddr);
    memcpy(macPtr->arEther,macAddr,6);
    macEntryPtr.key.key.macVlan.vlanId = (unsigned short)vlanId;
	ret=cpssDxChBrgFdbHashCalc(  devNum,&macEntryPtr.key, &hashPtr);
	
	if( ret!=0){
		nam_syslog_err("cpssDxChBrgFdbHashCalc erro is :%ld\n",ret);
		return FDB_CONFIG_FAIL;
		}
	ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
													 &skipPtr,&agedPtr,&associatedDevNumPtr,&readEntryPtr); 
	if( ret!=0){
		nam_syslog_err("cpssDxChBrgFdbMacEntryRead erro is :%ld\n",ret);
		return FDB_CONFIG_FAIL;
	}

    macEntryPtr.dstInterface.type = CPSS_INTERFACE_TRUNK_E;
	macEntryPtr.dstInterface.trunkId = trunkId;
	macEntryPtr.dstInterface.vidx=0;
	macEntryPtr.dstInterface.vlanId=0;
	#if 0
	if(IS_DISTRIBUTED == SYSTEM_TYPE)
	{
		ret = nam_get_distributed_devnum(asic_num,&actDev);
		if(ret != FDB_CONFIG_SUCCESS)
		{
			nam_syslog_dbg("get distributed dev num failure !!! \n");
		}

		macEntryPtr.dstInterface.devPort.devNum = actDev;
	}
	else
	{
		macEntryPtr.dstInterface.devPort.devNum = devNum;
	}
	#endif
	macEntryPtr.dstInterface.devPort.devNum = g_dis_dev[0];
	macEntryPtr.dstInterface.devPort.portNum = 0;
    macEntryPtr.isStatic = NAM_TRUE;/* static FDB */
    macEntryPtr.daCommand = CPSS_MAC_TABLE_FRWRD_E;
    macEntryPtr.saCommand = CPSS_MAC_TABLE_FRWRD_E;
    macEntryPtr.daRoute =NAM_FALSE;/* no L3 route */
    macEntryPtr.mirrorToRxAnalyzerPortEn = NAM_FALSE;
    macEntryPtr.sourceID = 0;
    macEntryPtr.userDefined = 0;
    macEntryPtr.daQosIndex = 0;
    macEntryPtr.saQosIndex = 0;
    macEntryPtr.daSecurityLevel = 0;
    macEntryPtr.saSecurityLevel = 0;
    macEntryPtr.appSpecificCpuCode = NAM_FALSE;

	ret=cpssDxChBrgFdbMacEntryWrite(devNum, hashPtr, skip, &macEntryPtr);

	if(0 != ret) {
		nam_syslog_err("cpssDxChBrgStaticFdbMacEntrySet erro! \n");
		nam_syslog_err("err vlaue is: %ld\n",ret);
		return FDB_CONFIG_FAIL;
	}
	else{
		
		ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
															 &skipPtr,&agedPtr,&associatedDevNumPtr,&readEntryPtr); 
        if( ret!=0){
		    nam_syslog_err("cpssDxChBrgFdbMacEntryRead after write erro is :%ld\n",ret);
		    return FDB_CONFIG_FAIL;
		}
		else{
            nam_syslog_dbg("The hashindex is:%d,valid: %d,skip:  %d, static: %d, mac: %02x:%02x:%02x:%02x:%02x:%02x\n",
				                               hashPtr,validPtr,skipPtr,readEntryPtr.isStatic,readEntryPtr.key.key.macVlan.macAddr.arEther[0],
				                               readEntryPtr.key.key.macVlan.macAddr.arEther[1],readEntryPtr.key.key.macVlan.macAddr.arEther[2],
				                               readEntryPtr.key.key.macVlan.macAddr.arEther[3],readEntryPtr.key.key.macVlan.macAddr.arEther[4],
				                               readEntryPtr.key.key.macVlan.macAddr.arEther[5]);
		}
	}
	return FDB_CONFIG_SUCCESS;
#endif

#ifdef DRV_LIB_BCM
	num_of_asic = nam_asic_get_instance_num();
	for(unit = 0;unit < num_of_asic;unit++){
	   ret = bcm_l2_addr_get(unit,macAddr,vlanId,&l2addr);
		if(NAM_OK == ret){/*there is already entry here,update it*/
		   if(l2addr.flags & BCM_L2_STATIC){/*former is static already,do nothing*/
	         return FDB_CONFIG_FAIL;
		   }
		   else {/*update it*/
		     l2addr.tgid = trunkId;
			 l2addr.flags |= BCM_L2_TRUNK_MEMBER;
		     l2addr.flags |= BCM_L2_STATIC;
		     ret =  bcm_l2_addr_add(unit,&l2addr);
		   }
		}
		else{/*if not find former entry,create one and add it.*/
	        bcm_l2_addr_t_init(&l2addr,macAddr,vlanId);
			l2addr.tgid = trunkId;
			l2addr.flags |= BCM_L2_TRUNK_MEMBER;
			l2addr.flags |= BCM_L2_STATIC;		
	        ret = bcm_l2_addr_add(unit,&l2addr);
			if(FDB_CONFIG_SUCCESS != ret){
				syslog_ax_nam_fdb_err("set fdb static mac %02x:%02x:%02x:%02x:%02x:%02x: vlan %d trunk %d unit %d failed,ret %d\n", \
						   macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5],vlanId,unit, trunkId);
			}
	    }
		NAM_RV_REPLACE_OK(ret, rv, NAM_E_UNAVAIL);
		nam_dump_l2_addr("Static:",&l2addr);
	}
	return rv;
#endif
}


/************************************************************
 *nam_no_static_fdb_entry_mac_vlan_set
 * 
 * delete the system static fdb table forwarding item 
 *
 *	input params:
 *		macAddr[] -forwarding destination mac address
 *		vlanId- the mac address belong to the valid vlan index
 * 
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/

int nam_no_static_fdb_entry_mac_vlan_set(unsigned char macAddr[],unsigned short vlanId) 
{
#ifdef DRV_LIB_BCM
    int rv= 0,ret = 0;
	int num_of_asic = 0,unit = 0,lport = 0;
	bcm_l2_addr_t		l2addr;
    memset(&l2addr,0,sizeof(bcm_l2_addr_t));
#endif

#ifdef DRV_LIB_CPSS
	unsigned char devNum;
	unsigned int hashPtr;
	int ret;
	unsigned int					 validPtr;
    unsigned int					 skipPtr;
    NAM_BOOL					 agedPtr;
    unsigned char					 associatedDevNumPtr;
    CPSS_MAC_ENTRY_EXT_STC       readEntryPtr;
	CPSS_MAC_ENTRY_EXT_KEY_STC		 entryPtr;
	ETHERADDR  *macPtr = NULL;
	devNum = FDB_INIT;
	unsigned int flag = 0;
#endif

#ifdef DRV_LIB_CPSS
	entryPtr.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
	macPtr = &(entryPtr.key.macVlan.macAddr);
    memcpy(macPtr->arEther,macAddr,6);
	
	entryPtr.key.macVlan.vlanId = vlanId;
	
	ret= cpssDxChBrgFdbStaticDelEnable( devNum,NAM_TRUE);
	if(0 != ret)
	{
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbStaticDelEnable:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		return FDB_CONFIG_FAIL;
	}
	ret=cpssDxChBrgFdbHashCalc(devNum,&entryPtr, &hashPtr);
	if( ret!=0)
	{
		nam_syslog_err("cpssDxChBrgFdbHashCalc erro is :%d",ret);
		return FDB_CONFIG_FAIL;
	}
	ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
                                         &skipPtr,&agedPtr,&associatedDevNumPtr,&readEntryPtr);
	if((ret == 0) && (validPtr))
	{
		/* If find error, repeat the lookup progress max to  4*/
		while(((readEntryPtr.isStatic == 0)||(readEntryPtr.key.key.macVlan.vlanId != vlanId)||\
			(memcmp(readEntryPtr.key.key.macVlan.macAddr.arEther,macAddr,6))||(skipPtr))&&(flag < 3))
	    {
			nam_syslog_err("This index %d has been used by another  entry and find another index !\n",hashPtr);
			hashPtr++;
			flag++;
			ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
													 &skipPtr,&agedPtr,&associatedDevNumPtr,&readEntryPtr);
		}
		if( !(flag < 3))
		{
			ret= cpssDxChBrgFdbStaticDelEnable( devNum,NAM_FALSE);
			if(0 != ret)
			{
				nam_syslog_err("npd_fdb:cpssDxChBrgFdbStaticDelEnable:FAIL.\n");
				nam_syslog_err("err vlaue is: %d\n",ret);
				return FDB_CONFIG_FAIL;
			}
	        nam_syslog_err("nam_no_static_fdb_entry_mac_vlan_set find index for the static fdb set error!\n");
			return FDB_CONFIG_FAIL;
		}

		readEntryPtr.isStatic = 0;
		ret = cpssDxChBrgFdbMacEntryWrite(devNum,hashPtr,1,&readEntryPtr);
		if( ret!=0)
		{
			nam_syslog_err("cpssDxChBrgFdbMacEntrywrite erro is :%d",ret);
			return FDB_CONFIG_FAIL;
		}
		ret= cpssDxChBrgFdbStaticDelEnable( devNum,NAM_FALSE);
		if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbStaticDelEnable:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			return FDB_CONFIG_FAIL;
		}
	}
	else
	{
		ret= cpssDxChBrgFdbStaticDelEnable( devNum,NAM_FALSE);
		if(0 != ret)
		{
			nam_syslog_err("npd_fdb:cpssDxChBrgFdbStaticDelEnable:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			return FDB_CONFIG_FAIL;
		}
		if(!validPtr)
		{
			nam_syslog_dbg("mac  %02x:%02x:%02x:%02x:%02x:%02x do not exist in the fdb table !!!,do not need to delete\n",macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5],hashPtr);
			return FDB_CONFIG_SUCCESS;
		}
		nam_syslog_err("cpssDxChBrgFdbMacEntryRead erro is :%d",ret);
		return FDB_CONFIG_FAIL;
	}
	
	return FDB_CONFIG_SUCCESS;
#endif

#ifdef DRV_LIB_BCM
	num_of_asic = nam_asic_get_instance_num();
	/*=====================================
	get vir port by globle index ,modified code here!
	npd_get_devport_by_global_index(globle_index,&unit,&lport);
	======================================*/
	for(unit = 0;unit < num_of_asic;unit++){
		ret = bcm_l2_addr_get(unit,macAddr,vlanId,&l2addr);
		if(NAM_OK != ret){
	       syslog_ax_nam_fdb_err("no static mac %02x:%02x:%02x:%02x:%02x:%02x vlanid %d unit %d not found in hw!\n", \
		   	           macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5], vlanId,unit);
		}
		else{/*there is already entry here,update it*/
		   ret = bcm_l2_addr_delete(unit,macAddr,vlanId);
		   if(NAM_OK != ret){
	        syslog_ax_nam_fdb_err("no static mac %02x:%02x:%02x:%02x:%02x:%02x vlanid %d unit %d failed in hw!\n", \
		   	           macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5], vlanId,unit);  
		   }
		}
	   NAM_RV_REPLACE_OK(ret, rv, NAM_E_UNAVAIL);
	   nam_dump_l2_addr("Static:",&l2addr);
	}
	return rv;
#endif
}


/************************************************************
 *nam_fdb_entry_mac_vlan_drop
 * 
 * config  the system static fdb table dmac or smac macth drop item 
 *
 *	input params:
 *		macAddr[] -forwarding destination mac address
 *		vlanId- the mac address belong to the valid vlan index
 *		flag - judge destination or source mac match drop
 * 
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/

int nam_fdb_entry_mac_vlan_drop
(
	/*unsigned char devNum,*/
	unsigned char macAddr[],
	unsigned short vlanId,
	unsigned int flag,
	unsigned int *hash_index
) 
{
#ifdef DRV_LIB_BCM
    int rv = 0,ret = 0,i = 0;
	int num_of_asic = 0,unit = 0,lport = 0,ports_count = 0;
	bcm_l2_addr_t		l2addr;
	bcm_mcast_addr_t mcaddr;
	bcm_pbmp_t pbmp ,ubmp;
    memset(&l2addr,0,sizeof(bcm_l2_addr_t));
    memset(&mcaddr,0,sizeof(bcm_mcast_addr_t));
	memset(&pbmp,0,sizeof(bcm_pbmp_t));
	memset(&ubmp,0,sizeof(bcm_pbmp_t));
#endif

#ifdef DRV_LIB_CPSS
  	unsigned long ret;
    unsigned int count = 0;
	unsigned int hashPtr;
	NAM_BOOL skip = NAM_FALSE;
	unsigned char devNum = 0;
	CPSS_MAC_ENTRY_EXT_STC       macEntryPtr;
	ETHERADDR  *macPtr = NULL;
    ETHERADDR  *readPtr = NULL;
	unsigned int					 validPtr;
	unsigned int					 skipPtr;
	NAM_BOOL					 agedPtr;
	unsigned char					 associatedDevNumPtr;
	CPSS_MAC_ENTRY_EXT_STC       readEntryPtr;
	memset(&macEntryPtr, 0 ,sizeof(CPSS_MAC_ENTRY_EXT_STC));
#endif

#ifdef DRV_LIB_CPSS
    macEntryPtr.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
	macPtr = &(macEntryPtr.key.key.macVlan.macAddr);
    memcpy(macPtr->arEther,macAddr,6);
    macEntryPtr.key.key.macVlan.vlanId = (unsigned short)vlanId;
	ret=cpssDxChBrgFdbHashCalc(  devNum,&macEntryPtr.key, &hashPtr);
	
	if( ret!=0){
		nam_syslog_err("cpssDxChBrgFdbHashCalc erro is :%ld\n",ret);
		return FDB_CONFIG_FAIL;
		}
	ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
			    &skipPtr,&agedPtr,&associatedDevNumPtr,&readEntryPtr); 
	
	if( ret!=0){
		nam_syslog_err("cpssDxChBrgFdbMacEntryRead erro is :%ld\n",ret);
		return FDB_CONFIG_FAIL;
	}
	while((1 == validPtr)&&(count < 4)){
			if(((readEntryPtr.isStatic ==1)&&((readEntryPtr.key.key.macVlan.vlanId != vlanId)||
		                       ( memcmp(readEntryPtr.key.key.macVlan.macAddr.arEther,macAddr,6))))){
     
			nam_syslog_err("This index %d has been used by another static entry and find another index,validPtr %d,skipPtr %d ,agedPtr %d !\n",hashPtr,validPtr,skipPtr,agedPtr);
			hashPtr++;
			count++;
			ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
													 &skipPtr,&agedPtr,&associatedDevNumPtr,&readEntryPtr); 
		    }
			else{
               break;
			}
	}
	if( !(count < 4)){
        nam_syslog_err("nam_fdb_entry_mac_vlan_drop find index for the static fdb set error!\n");
		return FDB_CONFIG_FAIL;
	}
	readPtr=&(readEntryPtr.key.key.macVlan.macAddr);
   
	ret =nam_mac_match(macAddr,readPtr->arEther,6);
	if((1==ret)&&(vlanId == readEntryPtr.key.key.macVlan.vlanId)&&(!skipPtr)&&(validPtr)){
		printf("The Entry used and chang it to blacklist: readEntryPtr.interface.type is %d  portNum is %d\n",
			     readEntryPtr.dstInterface.type,readEntryPtr.dstInterface.devPort.portNum);
		readEntryPtr.isStatic = NAM_TRUE;/*SET TO STATIC*/
		if(1==flag){
	       readEntryPtr.daCommand = CPSS_MAC_TABLE_DROP_E;
	    }
		else{
          readEntryPtr.saCommand = CPSS_MAC_TABLE_DROP_E;
		}
		printf("The dacommand is %d,the sacommand is %d\n",readEntryPtr.daCommand,readEntryPtr.saCommand);
        ret=cpssDxChBrgFdbMacEntryWrite(devNum, hashPtr, skip, &readEntryPtr);
	    *hash_index = hashPtr;
		if(0 != ret) {
			nam_syslog_err("cpssDxChBrgStaticFdbMacEntrySet erro! \n");
			nam_syslog_err("err vlaue is: %ld\n",ret);
			return FDB_CONFIG_FAIL;
		}
	    printf("The hash index is %d\n",hashPtr);
		return FDB_CONFIG_SUCCESS;
	}
	
    NAM_CONVERT_DEV_PORT_MAC(macEntryPtr.dstInterface.devPort.devNum,
                             macEntryPtr.dstInterface.devPort.portNum);
    macEntryPtr.isStatic = NAM_TRUE;/* static FDB */
    /*macEntryPtr.daRoute =NAM_FALSE;*//* no L3 route */
	macEntryPtr.daCommand = readEntryPtr.daCommand;
	macEntryPtr.saCommand = readEntryPtr.saCommand;
    macEntryPtr.mirrorToRxAnalyzerPortEn = NAM_FALSE;
    macEntryPtr.sourceID = 0;
    macEntryPtr.userDefined = 0;
    macEntryPtr.daQosIndex = 0;
    macEntryPtr.saQosIndex = 0;
    macEntryPtr.daSecurityLevel = 0;
    macEntryPtr.saSecurityLevel = 0;
    macEntryPtr.appSpecificCpuCode = NAM_FALSE;

	if(1==flag){
	   macEntryPtr.dstInterface.type = CPSS_INTERFACE_PORT_E;
	   macEntryPtr.daCommand = CPSS_MAC_TABLE_DROP_E;
       /*forward the da mac in the vid*/
	   macEntryPtr.dstInterface.type = CPSS_INTERFACE_VID_E;
	   macEntryPtr.dstInterface.vlanId=vlanId;
	   macEntryPtr.dstInterface.devPort.devNum = 0;
       macEntryPtr.dstInterface.devPort.portNum = 0;
	}
	else{
       macEntryPtr.saCommand = CPSS_MAC_TABLE_DROP_E;
	   /*forward the da mac in the vid*/
	   macEntryPtr.dstInterface.type = CPSS_INTERFACE_VID_E;
	   macEntryPtr.dstInterface.vlanId=vlanId;
	   macEntryPtr.dstInterface.devPort.devNum = 0;
       macEntryPtr.dstInterface.devPort.portNum = 0;
	}
	nam_syslog_dbg("The mac is %02x:%02x:%02x:%02x:%02x:%02x\n",macEntryPtr.key.key.macVlan.macAddr.arEther[0],
		                         macEntryPtr.key.key.macVlan.macAddr.arEther[1],macEntryPtr.key.key.macVlan.macAddr.arEther[2],
		                         macEntryPtr.key.key.macVlan.macAddr.arEther[3],macEntryPtr.key.key.macVlan.macAddr.arEther[4],
		                         macEntryPtr.key.key.macVlan.macAddr.arEther[5]);
    nam_syslog_dbg("The dacommand is %d,the sacommand is %d\n",macEntryPtr.daCommand,macEntryPtr.saCommand);
	ret=cpssDxChBrgFdbMacEntryWrite(devNum, hashPtr, skip, &macEntryPtr);
    *hash_index = hashPtr;
	if(0 != ret) {
		nam_syslog_err("cpssDxChBrgStaticFdbMacEntrySet erro! \n");
		nam_syslog_err("err vlaue is: %ld\n",ret);
		return FDB_CONFIG_FAIL;
	}
   /*nam_syslog_dbg("The hash index is %d\n",hashPtr);*/
	return FDB_CONFIG_SUCCESS;
#endif

#ifdef DRV_LIB_BCM
	num_of_asic = nam_asic_get_instance_num();
	for(unit = 0;unit < num_of_asic;unit++){
	    ret = bcm_l2_addr_get(unit,macAddr,vlanId,&l2addr);
		if(NAM_OK == ret){/*there is already entry here,update it*/
		    if((l2addr.flags & BCM_L2_STATIC)&&((l2addr.flags & BCM_L2_COPY_TO_CPU)  \
				           ||(l2addr.flags & BCM_L2_MIRROR))){/*if mirror or to cpu, do not access to set blacklist*/
	            syslog_ax_nam_fdb_err("set fdb blacklist mac %02x:%02x:%02x:%02x:%02x:%02x vlanId %d unit %d err(former static is TO CPU or MIRROR)!\n", \
			 	        macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5],vlanId,unit);
			}
		    if(0 == flag){/*SRC MAC */
		        l2addr.flags |= BCM_L2_STATIC | BCM_L2_DISCARD_SRC;
		        ret = bcm_l2_addr_add(unit,&l2addr);
				if(0 != ret){
                    nam_syslog_err("add arl addr failed for ret %d unit %d\n",ret,unit);
					return ret;
				}
			}
			else if(1 == flag){/*DST MAC*/
			    l2addr.flags |= BCM_L2_STATIC | BCM_L2_DISCARD_DST;
		        ret = bcm_l2_addr_add(unit,&l2addr);
				if(0 != ret){
                    nam_syslog_err("add arl addr failed for ret %d unit %d\n",ret,unit);
					return ret;
				}		        
			}
			else {/*err */
			    syslog_ax_nam_fdb_err("set fdb blacklist mac %02x:%02x:%02x:%02x:%02x:%02x vlanId %d unit %d err!\n", \
			 	        macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5],vlanId,unit);
			}	   
		}
		else{/*if not find entry,return err.*/
			if(0 == flag){/*src blacklist,the packet  matches this flood in the vlan*/
			    /*get all the vlan ports*/
		        bcm_vlan_port_get(unit,vlanId,&pbmp,&ubmp);
			    /*new Multicast*/
				bcm_mcast_addr_t_init(&mcaddr,macAddr,vlanId);
				memcpy(&mcaddr.pbmp ,&pbmp,sizeof(pbmp));
				memcpy(&mcaddr.ubmp , &ubmp,sizeof(ubmp));
				ret = bcm_mcast_addr_add(unit,&mcaddr);
				if(0 != ret){
                    nam_syslog_err("add black list failed on unit %d for %d\n",unit,ret);
					return ret;
				}
				ret = bcm_l2_addr_get(unit,macAddr,vlanId,&l2addr);
				if(0 != ret){
                    nam_syslog_err("get arl failed for ret %d,unit %d\n",ret,unit);
					return ret;
				}
				else{
					l2addr.flags |= BCM_L2_STATIC |BCM_L2_DISCARD_SRC;
                    ret = bcm_l2_addr_add(unit,&l2addr);
					if(0 != ret){
	                    nam_syslog_err("add arl failed for ret %d,unit %d\n",ret,unit);
						return ret;
					}					
				}
			}
			else if(1 == flag){/*dst blacklist, the packset mathes this droped or no used*/
				l2addr.flags = BCM_L2_STATIC | BCM_L2_DISCARD_DST;
				memcpy(l2addr.mac,macAddr,6);
				l2addr.vid = vlanId;
				ret = bcm_l2_addr_add(unit,&l2addr);
				if(0 != ret){
                    nam_syslog_err("Add black list failed on unit %d for %d\n",unit,ret);
					return ret;
				}
			}
	    }
	   NAM_RV_REPLACE_OK(ret, rv, NAM_E_UNAVAIL);	
	   nam_dump_l2_addr("Static:",&l2addr);	   
	}
	return rv;
#endif
}

/************************************************************
 *nam_fdb_entry_mac_vlan_no_drop
 * 
 * config  the system static fdb table dmac or smac macth forwarding item 
 *
 *	input params:
 *		macAddr[] -forwarding destination mac address
 *		vlanId- the mac address belong to the valid vlan index
 *		flag - judge destination or source mac match drop
 * 
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/

int nam_fdb_entry_mac_vlan_no_drop
(
	/*unsigned char devNum,*/
	unsigned char macAddr[],
	unsigned short vlanId,
	unsigned int flag
) 
{
#ifdef DRV_LIB_BCM
    int rv = 0, ret = 0;
	int num_of_asic = 0,unit = 0,lport = 0,errno = 0;
	bcm_l2_addr_t		l2addr;
    memset(&l2addr,0,sizeof(bcm_l2_addr_t));
#endif

#ifdef DRV_LIB_CPSS
  	unsigned char devNum;
	unsigned int hashPtr;
	unsigned int count = 0;
	int ret;
	unsigned int                    validPtr;
    unsigned int                    skipPtr;
    NAM_BOOL                        agedPtr;
    unsigned char                   associatedDevNumPtr;
    CPSS_MAC_ENTRY_EXT_STC          entryPtr;
	CPSS_MAC_ENTRY_EXT_STC          fdbMacEntryPtr;
	ETHERADDR  *macPtr = NULL;
    ETHERADDR  *readPtr = NULL;
	devNum = FDB_INIT;
#endif

#ifdef DRV_LIB_CPSS
	fdbMacEntryPtr.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
	macPtr = &(fdbMacEntryPtr.key.key.macVlan.macAddr);
    memcpy(macPtr->arEther,macAddr,6);
    fdbMacEntryPtr.key.key.macVlan.vlanId = vlanId;
	
	ret=cpssDxChBrgFdbHashCalc(  devNum,&fdbMacEntryPtr.key, &hashPtr);
	if( ret!=0){
		nam_syslog_err("cpssDxChBrgFdbHashCalc erro is :%d",ret);
		return FDB_CONFIG_FAIL;
		}
	ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
                                             &skipPtr,&agedPtr,&associatedDevNumPtr,&entryPtr);	

	while((1 == validPtr)&&(count < 4)){
			if(((entryPtr.isStatic ==1)&&((entryPtr.key.key.macVlan.vlanId != vlanId)||
		                       ( memcmp(entryPtr.key.key.macVlan.macAddr.arEther,macAddr,6))))){
     
			nam_syslog_err("This index %d has been used by another static entry and find another index,validPtr %d,skipPtr %d ,agedPtr %d !\n",hashPtr,validPtr,skipPtr,agedPtr);
			hashPtr++;
			count++;
			ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
													 &skipPtr,&agedPtr,&associatedDevNumPtr,&entryPtr); 
		    }
			else{
               break;
			}
	}
	if( !(count < 4)){
        nam_syslog_err("no blacklist find index for the static fdb set error!\n");
		return FDB_CONFIG_FAIL;
	}
	readPtr=&(entryPtr.key.key.macVlan.macAddr);
    ret =nam_mac_match(macAddr,readPtr->arEther,6);
	if(ret==0){
		nam_syslog_err("the mac address is no exist!\n");
		return FDB_CONFIG_NOEXST;
	}
	if(vlanId != entryPtr.key.key.macVlan.vlanId){
		nam_syslog_err("the entry is no exist!\n");
		return FDB_CONFIG_NOEXST;

	}
	
	if (1 == flag){
		    nam_syslog_dbg("The dacommand is %d,the sacommand is %d\n",entryPtr.daCommand,entryPtr.saCommand);
			entryPtr.daCommand = CPSS_MAC_TABLE_FRWRD_E;
			if(CPSS_MAC_TABLE_DROP_E != entryPtr.saCommand){
				nam_syslog_dbg("The dacommand is %d,the sacommand is %d\n",entryPtr.daCommand,entryPtr.saCommand);
				entryPtr.isStatic = NAM_FALSE;/*no static FDB */
				nam_syslog_dbg("delete the entry in the asic\n");
				ret=cpssDxChBrgFdbMacEntryWrite(devNum, hashPtr, 1, &entryPtr);
				if(0 != ret) {
					nam_syslog_err("cpssDxChBrgFdbMacEntryWrite erro! \n");
					nam_syslog_err("err vlaue is: %ld\n",ret);
					return FDB_CONFIG_FAIL;
				}
				return FDB_CONFIG_SUCCESS;
			}
	}
	else if(0 == flag){
		    nam_syslog_dbg("The dacommand is %d,the sacommand is %d\n",entryPtr.daCommand,entryPtr.saCommand);
	    	entryPtr.saCommand = CPSS_MAC_TABLE_FRWRD_E;
			if(CPSS_MAC_TABLE_DROP_E != entryPtr.daCommand){
				nam_syslog_dbg("The dacommand is %d,the sacommand is %d\n",entryPtr.daCommand,entryPtr.saCommand);
				entryPtr.isStatic = NAM_FALSE;/*no static FDB */
				nam_syslog_dbg("delete the entry in the asic\n");
				ret=cpssDxChBrgFdbMacEntryWrite(devNum, hashPtr, 1, &entryPtr);
				if(0 != ret) {
					nam_syslog_err("cpssDxChBrgFdbMacEntryWrite erro! \n");
					nam_syslog_err("err vlaue is: %ld\n",ret);
					return FDB_CONFIG_FAIL;
				}
				return FDB_CONFIG_SUCCESS;
			}
	}
	
	    ret = cpssDxChBrgFdbMacEntrySet(devNum, &entryPtr);
	    if(0 != ret) {
		   nam_syslog_err("npd_fdb::fdb_entry_mac_vlan_port_set::FAIL.\v");
		   nam_syslog_err("err vlaue is: %d\n",ret);
		   return FDB_CONFIG_FAIL;
	}
	return FDB_CONFIG_SUCCESS;
#endif

#ifdef DRV_LIB_BCM
    num_of_asic = nam_asic_get_instance_num();
	for(unit = 0;unit < num_of_asic;unit++){
		ret = bcm_l2_addr_get(unit,macAddr,vlanId,&l2addr);
		if(NAM_OK == ret){/*there is already entry here,update it*/
		   if(l2addr.flags & BCM_L2_STATIC){/*former is static already,do nothing*/
	          if((0 == flag)&&(l2addr.flags & BCM_L2_DISCARD_SRC)){/*SRC MAC */
	              l2addr.flags &= ~BCM_L2_DISCARD_SRC;
				  if(l2addr.flags & BCM_L2_DISCARD_DST){
				     ret = bcm_l2_addr_add(unit,&l2addr);
					 if(0 != ret){
                        nam_syslog_dbg("l2 addr add failed for ret %d,unit %d\n",unit,ret);
						return ret;
					 }
				  }
				  else{
				  	 if(l2addr.flags & BCM_L2_MCAST){
						 ret = bcm_mcast_addr_remove(unit,l2addr.mac,l2addr.vid);
						 if(0 != ret){
						    nam_syslog_dbg("l2 addr delete failed for ret %d,unit %d\n",unit,ret);
							return ret;
						 }
				  	 }
					 else{
                         ret = bcm_l2_addr_delete(unit,l2addr.mac,l2addr.vid);
						 if(0 != ret){
						    nam_syslog_dbg("l2 addr delete failed for ret %d,unit %d\n",unit,ret);
							return ret;
						 }						 
					 }
				  }
			  }
			  else if((1 == flag)&&(l2addr.flags & BCM_L2_DISCARD_DST)){/*DST MAC*/
	              l2addr.flags &= ~BCM_L2_DISCARD_DST;
				  if(l2addr.flags & BCM_L2_DISCARD_SRC){
				     ret = bcm_l2_addr_add(unit,&l2addr);
					 if(0 != ret){
                        nam_syslog_dbg("l2 addr add failed for ret %d,unit %d\n",unit,ret);
						return ret;
					 }
				  }
				  else{
					  ret = bcm_l2_addr_delete(unit,l2addr.mac,l2addr.vid);
					  if(0 != ret){
						 nam_syslog_dbg("l2 addr delete failed for ret %d,unit %d\n",unit,ret);
						 return ret;
					  }
				  }				  
			  }
			  else {/*err */
			     syslog_ax_nam_fdb_err("delete fdb blacklist mac %02x:%02x:%02x:%02x:%02x:%02x vlanId %d ,l2addr.flags(src,dst)%d on unit %d not compared!\n", \
			 	          macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5],vlanId,l2addr.flags,unit);
			  }
		   }
		   else {/*if not static entry,err*/
		     syslog_ax_nam_fdb_dbg("delete fdb blacklist mac %02x:%02x:%02x:%02x:%02x:%02x vlanId %d unit %d is not static entry\n", \
			 	          macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5],vlanId,unit);
		   }
		}
		else{/*if not find entry,return err.*/
		    syslog_ax_nam_fdb_dbg("delete fdb blacklist mac %02x:%02x:%02x:%02x:%02x:%02x vlanId %d unit %d not found\n", \
			 	          macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5],vlanId,unit);
	    }
	   NAM_RV_REPLACE_OK(ret, rv, NAM_E_UNAVAIL);		
	}
	nam_dump_l2_addr("Static:",&l2addr);
	return rv;
#endif
}

/************************************************************
 *nam_show_fdb
 * 
 * dispaly avalid fdb items 
 *
 *	input params:
 *		fdb -hold output fdb items pointer
 *		size- the maxnum dispaly count
 * 
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/
unsigned int nam_show_fdb 
(
	NPD_FDB *fdb,
	unsigned int size
)
{
#ifdef DRV_LIB_BCM
    int i = 0,j = 0,unit = 0,num_of_asic = 0;
	int ret = 0;
	unsigned int eth_g_index = 0,old_value = 0;
    user_data usrdata;
#endif

#ifdef DRV_LIB_CPSS
    unsigned int            ret = 0;
	unsigned int 			index = 0;
	unsigned int 			eth_g_index=0;
	unsigned int			result=0;
	unsigned int         	i = 0,j=0;
	unsigned char			devNum = 0, devCnt = 0;
	unsigned char			trgDev = 0, trgPort = 0;
	unsigned int        	validPtr=0;
    unsigned int       	skipPtr= 0;
    NAM_BOOL                agedPtr;
	unsigned int            board_type = 0;
	unsigned char			associatedDevNumPtr;
	CPSS_MAC_ENTRY_EXT_STC	entryPtr;
	ETHERADDR	*macPtr = NULL;
#endif

#ifdef DRV_LIB_CPSS

	if((BOARD_TYPE == BOARD_TYPE_AX81_SMU) || (BOARD_TYPE == BOARD_TYPE_AX71_CRSMU) )
	{
		return 0;	/*do not show fdb items return zero item*/
	}
	if(BOARD_TYPE == BOARD_TYPE_AX81_12X)
	{
		board_type = 1;/*used for lion 32K fdb table*/
	}
	else
	{
		size = 16384;
	}
	devCnt = nam_asic_get_instance_num();
	for(devNum = 0; devNum < devCnt; devNum++)
	{
		ret = cpssDxChBrgFdbActionsEnableSet(devNum,NAM_FALSE);
		if(0 != ret)
		{
			nam_syslog_err("nam_fdb:cpssDxChBrgFdbActionsEnableSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %d\n",ret);
			return FDB_CONFIG_FAIL;
		}

		do{
			 /* call cpss api function */
			memset(&entryPtr,0,sizeof(CPSS_MAC_ENTRY_EXT_STC));
			result = cpssDxChBrgFdbMacEntryRead(devNum, index, &validPtr, &skipPtr,
											&agedPtr, &associatedDevNumPtr, &entryPtr);
			if((!validPtr) ||(skipPtr)) 
			{
					index++;
					continue;
			}

			if(entryPtr.key.key.macVlan.vlanId >= 4094)
			{
				index++;
				continue;
			}
			ret = nam_mac_is_multicast(entryPtr.key.key.macVlan.macAddr.arEther);
			if(!ret)
			{
				/*if is the multicast,do not check and show directly*/
		        if(entryPtr.key.entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E)
				{
					index++;
					continue;
				}
					
				if(entryPtr.daCommand==CPSS_MAC_TABLE_CNTL_E)
				{
					index++;
					continue;
				}
				/* do not care the blacklist unicast*/

				if((CPSS_MAC_TABLE_DROP_E == entryPtr.daCommand)||(CPSS_MAC_TABLE_DROP_E == entryPtr.saCommand))
				{
		           index++;
				   continue;
				}
			}
		 	if (result == 0) 
			{
				 	
    			macPtr = &(entryPtr.key.key.macVlan.macAddr);
				fdb[i].vlanid = entryPtr.key.key.macVlan.vlanId;
                memcpy(fdb[i].ether_mac,entryPtr.key.key.macVlan.macAddr.arEther,6);

				trgDev = entryPtr.dstInterface.devPort.devNum;
				trgPort = entryPtr.dstInterface.devPort.portNum;

				if( entryPtr.dstInterface.type == CPSS_INTERFACE_PORT_E) 
				{
            		fdb[i].inter_type = CPSS_INTERFACE_PORT_E;
						
					NAM_CONVERT_BACK_DEV_PORT_MAC(entryPtr.dstInterface.devPort.devNum, 
							 				entryPtr.dstInterface.devPort.portNum);

					if(0x3F == entryPtr.dstInterface.devPort.portNum)
					{ /* target port is CPU*/
						eth_g_index = g_cpu_port_index;
					}
					else if (NPD_TRUE == npd_product_check_spi_port(trgDev, trgPort))
					{
						eth_g_index = g_spi_port_index;
					}					
					else
					{
						/*if not local dev,do not show for distributed system*/
						if(trgDev != g_dis_dev[0])
						{
							index++;
							continue;
						}
						switch(BOARD_TYPE)
						{
							case BOARD_TYPE_AX81_AC12C:
								if(entryPtr.dstInterface.devPort.portNum > 5)
								{
									index++;
									continue;
								}
								break;
							case BOARD_TYPE_AX81_AC8C:
								if(entryPtr.dstInterface.devPort.portNum > 3)
								{
									index++;
									continue;
								}
								break;
							case BOARD_TYPE_AX71_2X12G12S:
								if(entryPtr.dstInterface.devPort.portNum > 25)
								{
									index++;
									continue;
								}
								break;
							case BOARD_TYPE_AX81_1X12G12S:
								if(entryPtr.dstInterface.devPort.portNum > 23)
								{
									index++;
									continue;
								}
								break;
							default:
								nam_syslog_err("unknown board type !\n");
								break;
						}
	            		if (npd_get_global_index_by_devport( devNum ,entryPtr.dstInterface.devPort.portNum ,&eth_g_index))
	 					{
                           index++;
						   continue;
						}
					}
				 	fdb[i].value= eth_g_index;
				}
			 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_TRUNK_E)
				{
					if(entryPtr.dstInterface.trunkId > 110)
					{
						index++;
						continue;
					}
			 		fdb[i].inter_type = CPSS_INTERFACE_TRUNK_E;
					fdb[i].value = entryPtr.dstInterface.trunkId;
			 	}
			 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_VID_E)
				{
			 		fdb[i].inter_type = CPSS_INTERFACE_VID_E;
			 		fdb[i].value = entryPtr.dstInterface.vlanId ;
				}
			 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_VIDX_E)
				{
			 		fdb[i].inter_type = CPSS_INTERFACE_VIDX_E;
			 		fdb[i].value = entryPtr.dstInterface.vidx;
				}
				else 
				{
				 	/*nam_syslog_err ("illegal desinterface type!/n");*/
				 	index++;
					continue;
				}
				/*item type*/
				if( 0 == entryPtr.isStatic)
				{
                    fdb[i].type_flag = 0;
				}
				else if( 1 == entryPtr.isStatic)
				{
					if((nam_mac_is_multicast(entryPtr.key.key.macVlan.macAddr.arEther))||((CPSS_MAC_TABLE_DROP_E != entryPtr.daCommand)&&(CPSS_MAC_TABLE_DROP_E != entryPtr.saCommand)))
					{
                       fdb[i].type_flag = 1;
					}
				}
				i++;
				index++;
			}
			else 
			{
				index++;
				continue;
			}
		}while((!result)&&(i < size)&&(((board_type == 1)?(index<32768):(index<16384))));


		ret = cpssDxChBrgFdbActionsEnableSet(devNum,NAM_TRUE);
		if(0 != ret)
		{
			nam_syslog_err("nam_fdb:cpssDxChBrgFdbActionsEnableSet:FAIL.\n");
			nam_syslog_err("err vlaue is: %ld\n",ret);
			return FDB_CONFIG_FAIL;
		}
	}
    return i;	
#endif

#ifdef DRV_LIB_BCM
    memset(&usrdata,0,sizeof(user_data));
	num_of_asic = nam_asic_get_instance_num();
	for(unit = 0;unit < num_of_asic;unit++){
		ret = bcm_l2_traverse(unit,nam_l2_addr_show,&usrdata);
		sleep(1);
		if(NAM_E_NONE != ret){
           syslog_ax_nam_fdb_err("Traverse fdb table failed,ret %d\n",ret);
		}
		if(0 >= usrdata.count)
			 continue;
		for(i = 0;i < usrdata.count;i++){
           /*check read info */
		   #if 0
		   if((usrdata.l2addr[i].vid < 0)||(usrdata.l2addr[i].vid >= 4095)){
              syslog_ax_nam_fdb_err("error vid %d read from bcm!\n",usrdata.l2addr[i].vid);
			  continue;
		   }
		   #endif
		   if((usrdata.l2addr[i].port < 0)||(usrdata.l2addr[i].port > 29)||(( 1!=usrdata.l2addr[i].port)&&(2!=usrdata.l2addr[i].port)&&(0 < usrdata.l2addr[i].port)&&(( usrdata.l2addr[i].port < 6)))){
			  /* syslog_ax_nam_fdb_err("error asic port %d read from bcm!\n",usrdata.l2addr[i].port);*/
			   continue;
		   }
		   else if(unit != usrdata.l2addr[i].modid){/*do not show the remote fdb item*/
               /*syslog_ax_nam_fdb_dbg("The item (modid %d)is on remote asic %d",usrdata.l2addr[i].modid,unit);*/
               continue;
		   }
		   else if ((usrdata.l2addr[i].flags & BCM_L2_DISCARD_DST) ||
		   			(usrdata.l2addr[i].flags & BCM_L2_DISCARD_SRC)){
              /* syslog_ax_nam_fdb_dbg("Blacklist do not show!\n");*/
			   continue;
		   }
	       fdb[j].vlanid =usrdata.l2addr[i].vid;
		   memcpy(fdb[j].ether_mac,usrdata.l2addr[i].mac,6);
		   fdb[j].inter_type = (usrdata.l2addr[i].flags & BCM_L2_TRUNK_MEMBER)? 1: ((usrdata.l2addr[i].flags & BCM_L2_MCAST) ? 2:0);
		   if(!(fdb[j].inter_type)){/*port type*/
		      if(usrdata.l2addr[i].port == 0){/*cpu port*/
		      	if (usrdata.l2addr[i].flags & BCM_L2_MCAST){
					fdb[j].value = usrdata.l2addr[i].l2mc_index;
				}else {
                	fdb[j].value = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(	\
							CPU_PORT_VIRTUAL_SLOT,CPU_PORT_VIRTUAL_PORT);/*tmp value for cpu identify*/
				}
			  }
			  else{
			     /* npd_syslog_dbg("modid %d,port %d\n",usrdata.l2addr[i].modid,usrdata.l2addr[i].port);*/
		          npd_get_global_index_by_devport(usrdata.l2addr[i].modid,usrdata.l2addr[i].port,&eth_g_index);
			      fdb[j].value = eth_g_index;
			  }
		   }
		   else{
	          fdb[j].value = usrdata.l2addr[i].tgid;
		   }

		   if (usrdata.l2addr[i].flags & BCM_L2_MCAST){
				fdb[j].value = usrdata.l2addr[i].l2mc_index;
		   }		   
		   fdb[j].type_flag = (usrdata.l2addr[i].flags & BCM_L2_STATIC)? 1 : 0;
           if(j<size)
		       j++;
		   else
		   	   return j;
		}
		memset(&usrdata,0,sizeof(user_data));
	}
	return j;
#endif
}


/* for show debug fdb */
unsigned int nam_show_fdb_debug 
(
	NPD_FDB_DBG *fdb,
	unsigned int size
)
{
#ifdef DRV_LIB_CPSS
    unsigned int            ret = 0;
	unsigned int 			index = 0;
	unsigned int 			eth_g_index=0;
	unsigned int			result=0;
	unsigned int         	i = 0,j=0;
	unsigned char			devNum = 0, devCnt = 0;
	unsigned char			trgDev = 0, trgPort = 0;
	unsigned int        	validPtr=0;
    unsigned int       	skipPtr= 0;
    unsigned int       	board_type = 0;
    NAM_BOOL                agedPtr;
	unsigned char			associatedDevNumPtr;
	CPSS_MAC_ENTRY_EXT_STC	entryPtr;
	ETHERADDR	*macPtr = NULL;
	int asictype = 0;
#endif

    asictype = nam_asic_get_asic_type(devNum);
	if((asictype < ASIC_TYPE_START_EXMX_E) || (asictype > ASIC_TYPE_LAST_E))
	{
		nam_syslog_err("get asic type fail !\n");
	}
	if(asictype == ASIC_TYPE_DXCH_LION_E)
	{
		board_type = 1;/*used for lion 32K fdb table*/
	}
	else
	{
		size = 16384;
	}

    nam_syslog_dbg("size: %d\n",size);
    nam_syslog_dbg("board_type: %d\n",board_type);


#ifdef DRV_LIB_CPSS
	devCnt = nam_asic_get_instance_num();
	for(devNum = 0; devNum < devCnt; devNum++)
	{
        nam_syslog_dbg("dev: %d\n",devNum);		
    	ret = cpssDxChBrgFdbActionsEnableSet(devNum,NAM_FALSE);
    	if(0 != ret)
    	{
    		nam_syslog_err("nam_fdb:cpssDxChBrgFdbActionsEnableSet:FAIL.\n");
    		nam_syslog_err("err vlaue is: %d\n",ret);
    		return FDB_CONFIG_FAIL;
    	}

    	do{
    		/* call cpss api function */
    		memset(&entryPtr,0,sizeof(CPSS_MAC_ENTRY_EXT_STC));
    		result = cpssDxChBrgFdbMacEntryRead(devNum, index, &validPtr, &skipPtr, &agedPtr, &associatedDevNumPtr, &entryPtr);
    		if((!validPtr) ||(skipPtr))
			{
				index++;
				continue;
    		}

    		ret = nam_mac_is_multicast(entryPtr.key.key.macVlan.macAddr.arEther);
    		if(!ret)
			{/*if is the multicast,do not check and show directly*/
    	        if(entryPtr.key.entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E)
				{					
    				index++;
    				continue;
    			}
    			
    			if(entryPtr.daCommand==CPSS_MAC_TABLE_CNTL_E)
				{
    				index++;
    				continue;
    			}
    			/* do not care the blacklist unicast*/

    			if((CPSS_MAC_TABLE_DROP_E == entryPtr.daCommand)||(CPSS_MAC_TABLE_DROP_E == entryPtr.saCommand))
				{
					
    	            index++;
    			    continue;
    			}
    		}
    	 	if (result == 0)
    		{
    		 	
    			macPtr = &(entryPtr.key.key.macVlan.macAddr);
				fdb[i].vlanid = entryPtr.key.key.macVlan.vlanId;
                memcpy(fdb[i].ether_mac,entryPtr.key.key.macVlan.macAddr.arEther,6);

				trgDev = entryPtr.dstInterface.devPort.devNum;
				trgPort = entryPtr.dstInterface.devPort.portNum;
				
                fdb[i].dev = trgDev;
				fdb[i].asic_num = devNum;
				if( entryPtr.dstInterface.type == CPSS_INTERFACE_PORT_E)
				{
            		fdb[i].inter_type = CPSS_INTERFACE_PORT_E;
					eth_g_index = entryPtr.dstInterface.devPort.portNum;
				 	fdb[i].value= eth_g_index;
				}
			 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_TRUNK_E){
			 		fdb[i].inter_type = CPSS_INTERFACE_TRUNK_E;
					fdb[i].value = entryPtr.dstInterface.trunkId;
			 	}
			 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_VID_E){
			 		fdb[i].inter_type = CPSS_INTERFACE_VID_E;
			 		fdb[i].value = entryPtr.dstInterface.vlanId ;
				}
			 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_VIDX_E){
			 		fdb[i].inter_type = CPSS_INTERFACE_VIDX_E;
			 		fdb[i].value = entryPtr.dstInterface.vidx;
				}
				else {
				 	/*nam_syslog_err ("illegal desinterface type!/n");*/
				 	index++;
					continue;
				}
				
				/*item type*/
				if( 0 == entryPtr.isStatic)
				{
                    fdb[i].type_flag = 0;
				}
				else if( 1 == entryPtr.isStatic)
				{
					if((nam_mac_is_multicast(entryPtr.key.key.macVlan.macAddr.arEther))||((CPSS_MAC_TABLE_DROP_E != entryPtr.daCommand)&&(CPSS_MAC_TABLE_DROP_E != entryPtr.saCommand))){
                       fdb[i].type_flag = 1;
					}
				}
				i++;
				index++;
    		 }
    		 else 
			 {   		 	
    			index++;
    			continue;
    		 }
    	}while((!result)&&(i < size)&&(((board_type == 1)?(index<32768):(index<16384))));

    	ret = cpssDxChBrgFdbActionsEnableSet(devNum,NAM_TRUE);
    	if(0 != ret)
    	{
    		nam_syslog_err("nam_fdb:cpssDxChBrgFdbActionsEnableSet:FAIL.\n");
    		nam_syslog_err("err vlaue is: %ld\n",ret);
    		return FDB_CONFIG_FAIL;
    	}
    	index = 0;
		if(devNum == 0)
			j=i;
		nam_syslog_dbg("dev %d has fdb count is %d\n",devNum,devNum?(i-j):j);
	}
    return i;	
#endif
}




/************************************************************
 *nam_show_dynamic_fdb
 * 
 * dispaly avalid fdb items 
 *
 *	input params:
 *		fdb -hold output fdb items pointer
 *		size- the maxnum dispaly count
 * 
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/

unsigned int nam_show_dynamic_fdb 
(
	NPD_FDB *fdb,
	unsigned int size
)
{
#ifdef DRV_LIB_BCM
	int i = 0,j = 0,unit = 0,num_of_asic = 0;
	int ret = 0;
	unsigned int eth_g_index = 0,old_value = 0;
	user_data usrdata;
#endif

#ifdef DRV_LIB_CPSS
    unsigned int            ret = 0;
	unsigned int 			index = 0;
	unsigned int 			eth_g_index=0;
	unsigned int			result=0;
	unsigned int         	i = 0,j=0;
	unsigned char			devNum =0;
	unsigned char			trgDev = 0, trgPort = 0;
	unsigned int        	validPtr=0;
    unsigned int       	    skipPtr= 0;
    NAM_BOOL                agedPtr;
	unsigned char			associatedDevNumPtr;
	CPSS_MAC_ENTRY_EXT_STC	entryPtr;
	ETHERADDR	*macPtr = NULL;
	unsigned int            board_type = 0;
#endif

#ifdef DRV_LIB_CPSS

	if((BOARD_TYPE == BOARD_TYPE_AX81_SMU) || (BOARD_TYPE == BOARD_TYPE_AX71_CRSMU))
	{
		return 0;
	}

	if(BOARD_TYPE == BOARD_TYPE_AX81_12X)
	{
		board_type = 1;/*used for lion 32K fdb table*/
	}
	else
	{
		size = 16384;
	}
	ret = cpssDxChBrgFdbActionsEnableSet(devNum,NAM_FALSE);
	if(0 != ret)
	{
		nam_syslog_err("nam_fdb:cpssDxChBrgFdbActionsEnableSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		return FDB_CONFIG_FAIL;
	}
	/*nam_syslog_err("----------------------------------------------------------\n");
	nam_syslog_err("%-4s %-17s %-4s %-6s %-4s %-5s\n","ITEM","MAC Address","VLAN","DEVICE","PORT","ROUTE");
	nam_syslog_err("---- ----------------- ---- ------ ---- -----\n");*/

		do{
			 /* call cpss api function */
			memset(&entryPtr,0,sizeof(CPSS_MAC_ENTRY_EXT_STC));
			result = cpssDxChBrgFdbMacEntryRead(devNum, index, &validPtr, &skipPtr,
											&agedPtr, &associatedDevNumPtr, &entryPtr);
			if((!validPtr) ||(skipPtr)||(1 == entryPtr.isStatic)) 
			{
					index++;
					continue;
			}
			/*nam_syslog_err("entryPtr.key.entryType value is %d\n",entryPtr.key.entryType);*/
			/*ipv4 multicust no show###################*/
	        if(entryPtr.key.entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E)
			{
				index++;
				continue;
			}
			if(entryPtr.daCommand==CPSS_MAC_TABLE_CNTL_E)
			{
				index++;
				continue;
			}
			if(entryPtr.key.key.macVlan.vlanId >= 4094)
			{
				index++;
				continue;
			}
		 	if (result == 0) 
			{
		 	 		
		 		macPtr = &(entryPtr.key.key.macVlan.macAddr);
		 		/*nam_syslog_err("%-4d %02X:%02X:%02X:%02X:%02X:%02X %-4d %-6d %-4d %-5s\n",i+1, \
						macPtr->arEther[0],macPtr->arEther[1],macPtr->arEther[2],	\
						macPtr->arEther[3],macPtr->arEther[4],macPtr->arEther[5],	\
						entryPtr.key.key.macVlan.vlanId,		\
						entryPtr.dstInterface.devPort.devNum,	\
						entryPtr.dstInterface.devPort.portNum,	\
						(entryPtr.daRoute =1) ? "en":"dis");*/
				fdb[i].vlanid = entryPtr.key.key.macVlan.vlanId;
	            memcpy(fdb[i].ether_mac,entryPtr.key.key.macVlan.macAddr.arEther,6);
	               /* nam_syslog_err("entryPtr.dstInterface.type is %d\n",entryPtr.dstInterface.type);*/

				trgDev = entryPtr.dstInterface.devPort.devNum;
				trgPort = entryPtr.dstInterface.devPort.portNum;

	            if( entryPtr.dstInterface.type == CPSS_INTERFACE_PORT_E) 
				{
		        	fdb[i].inter_type = CPSS_INTERFACE_PORT_E;
					NAM_CONVERT_BACK_DEV_PORT_MAC(entryPtr.dstInterface.devPort.devNum, 
									 				entryPtr.dstInterface.devPort.portNum);
					if(CPU_PORT_ASIC_PORTNO == entryPtr.dstInterface.devPort.portNum)
					{ /* target is CPU*/
						eth_g_index = g_cpu_port_index;
					}
					else if (NPD_TRUE == npd_product_check_spi_port(trgDev, trgPort)) 
					{
						eth_g_index = g_spi_port_index;
					}
					else
					{
						/*if not local dev,do not show for distributed system*/
						if(trgDev != g_dis_dev[0])
						{
							index++;
							continue;
						}
						switch(BOARD_TYPE)
						{
							case BOARD_TYPE_AX81_AC12C:
								if(entryPtr.dstInterface.devPort.portNum > 5)
								{
									index++;
									continue;
								}
								break;
							case BOARD_TYPE_AX81_AC8C:
								if(entryPtr.dstInterface.devPort.portNum > 3)
								{
									index++;
									continue;
								}
								break;
							case BOARD_TYPE_AX71_2X12G12S:
								if(entryPtr.dstInterface.devPort.portNum > 25)
								{
									index++;
									continue;
								}
								break;
							case BOARD_TYPE_AX81_1X12G12S:
								if(entryPtr.dstInterface.devPort.portNum > 23)
								{
									index++;
									continue;
								}
								break;
							default:
								nam_syslog_err("unknown board type !\n");
								break;
						}
	            		if (npd_get_global_index_by_devport(devNum ,entryPtr.dstInterface.devPort.portNum ,&eth_g_index)){
	                        index++;
							continue;
						}
					}
					fdb[i].value= eth_g_index;
				}
				else if (entryPtr.dstInterface.type == CPSS_INTERFACE_TRUNK_E)
				{
					if(entryPtr.dstInterface.trunkId > 110)
					{
						index++;
						continue;
					}
					fdb[i].inter_type = CPSS_INTERFACE_TRUNK_E;
					fdb[i].value = entryPtr.dstInterface.trunkId;
				}
				i++;
				index++;
			}
			else 
			{
				index++;
				continue;
			}
		}while((!result)&&(i < size)&&(((board_type == 1)?(index<32768):(index<16384))));

	ret = cpssDxChBrgFdbActionsEnableSet(devNum,NAM_TRUE);
	if(0 != ret)
	{
		nam_syslog_err("nam_fdb:cpssDxChBrgFdbActionsEnableSet:FAIL.\n");
		nam_syslog_err("err vlaue is: %ld\n",ret);
		return FDB_CONFIG_FAIL;
	}
    return i;	
#endif

#ifdef DRV_LIB_BCM
	memset(&usrdata,0,sizeof(user_data));
	num_of_asic = nam_asic_get_instance_num();
	for(unit = 0;unit < num_of_asic;unit++){
		ret = bcm_l2_traverse(unit,nam_l2_addr_show,&usrdata);
		sleep(1);
		if(NAM_E_NONE != ret){
		   syslog_ax_nam_fdb_err("Traverse fdb table failed,ret %d\n",ret);
		}
		if(0 >= usrdata.count)
			 continue;
		for(i = 0;i < usrdata.count;i++){
		   /*check read info */
	   #if 0
		   if((usrdata.l2addr[i].vid < 0)||(usrdata.l2addr[i].vid >= 4095)){
			  syslog_ax_nam_fdb_err("error vid %d read from bcm!\n",usrdata.l2addr[i].vid);
			  continue;
		   }
	   #endif
		   if((usrdata.l2addr[i].port < 0)||(usrdata.l2addr[i].port > 29)||((0 < usrdata.l2addr[i].port)&&(( usrdata.l2addr[i].port < 6)))){
			   /*syslog_ax_nam_fdb_err("error port %d read from bcm!\n",usrdata.l2addr[i].port);*/
			   continue;
		   }
		   else if(usrdata.l2addr[i].flags & BCM_L2_STATIC){
               continue;
		   }
		   else if((usrdata.l2addr[i].flags & BCM_L2_DISCARD_DST)||(usrdata.l2addr[i].flags & BCM_L2_DISCARD_SRC)){
              /* syslog_ax_nam_fdb_dbg("Blacklist do not show!\n");*/
			   continue;
	       }
		   else if(unit != usrdata.l2addr[i].modid){/*do not show the remote fdb item*/
			  /* syslog_ax_nam_fdb_dbg("The item (modid %d)is on remote asic %d",usrdata.l2addr[i].modid,unit);*/
			   continue;
		   }
		   fdb[j].vlanid =usrdata.l2addr[i].vid;
		   memcpy(fdb[j].ether_mac,usrdata.l2addr[i].mac,6);
		   fdb[j].inter_type = (usrdata.l2addr[i].flags & BCM_L2_TRUNK_MEMBER)? 1:0;
		   if(!(fdb[j].inter_type)){/*port type*/
			  if(usrdata.l2addr[i].port == 0){/*cpu port*/
				fdb[j].value = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX( \
							CPU_PORT_VIRTUAL_SLOT,CPU_PORT_VIRTUAL_PORT);/*tmp value for cpu identify*/
			  }
			  else{
				 /* npd_syslog_dbg("modid %d,port %d\n",usrdata.l2addr[i].modid,usrdata.l2addr[i].port);*/
				  npd_get_global_index_by_devport(usrdata.l2addr[i].modid,usrdata.l2addr[i].port,&eth_g_index);
				  fdb[j].value = eth_g_index;
			  }
		   }
		   else{
			  fdb[j].value = usrdata.l2addr[i].tgid;
		   }
		   fdb[j].type_flag = (usrdata.l2addr[i].flags & BCM_L2_STATIC)? 1 : 0;
		   if(j<size)
			   j++;
		   else
			   return j;
		}
		memset(&usrdata,0,sizeof(user_data));
	}
	return j;
#endif
}

/************************************************************
 *nam_show_fdb
 * 
 * dispaly avalid fdb items 
 *
 *	input params: none
 *		
 *	output: none
 *
 * 	return:
 *		 valid fdb items count
 *		
 *********************************************/

unsigned int nam_show_fdb_count ( ) 

{
#ifdef DRV_LIB_BCM
	int unit = 0,num_of_asic = 0;
	int ret = 0;
	user_data usrdata;
	unsigned int count[2];
#endif

#ifdef DRV_LIB_CPSS
    unsigned int 			index = 0;
	unsigned int			result=0;
	unsigned int         	i = 0;
	unsigned char			devNum =0;
	unsigned int        	validPtr=0;
    unsigned int       	    skipPtr= 0;
    NAM_BOOL                agedPtr;
	unsigned char			associatedDevNumPtr;
	CPSS_MAC_ENTRY_EXT_STC	entryPtr;
	unsigned int            board_type = 0;
	int asictype = 0;
	
#endif

#ifdef DRV_LIB_CPSS
	asictype = nam_asic_get_asic_type(devNum);
	if((asictype < ASIC_TYPE_START_EXMX_E) || (asictype > ASIC_TYPE_LAST_E))
	{
		nam_syslog_err("get asic type fail !\n");
	}
	if(asictype == ASIC_TYPE_DXCH_LION_E)
	{
		board_type = 1;/*used for lion 32K fdb table*/
	}
	do{
		 /* call cpss api function */
		result = cpssDxChBrgFdbMacEntryRead(devNum, index, &validPtr, &skipPtr,
										&agedPtr, &associatedDevNumPtr, &entryPtr);
		if((!validPtr) ||(skipPtr)) {
				index++;
				continue;
		}
		if((CPSS_MAC_TABLE_DROP_E == entryPtr.daCommand)||(CPSS_MAC_TABLE_DROP_E == entryPtr.saCommand)){
           index++;
		   continue;
		}
        
	 	else if (result == 0) {
		 	i++;
		 	}
		else {
			nam_syslog_err("nam read fdb entry error %d\n",result);
		}
		index++;
	}while((!result)&&(((board_type == 1)?(index<32768):(index<16384))));
	nam_syslog_dbg("index is: %d\n",index);
	nam_syslog_dbg("return i = %d\n ",i);
    return i;	
#endif

#ifdef DRV_LIB_BCM
	memset(&usrdata,0,sizeof(user_data));
	num_of_asic = nam_asic_get_instance_num();
	for(unit = 0;unit < num_of_asic;unit++){
		ret = bcm_l2_traverse(unit,nam_l2_addr_show,&usrdata);
		sleep(1);
		if(NAM_E_NONE != ret){
		   syslog_ax_nam_fdb_err("Traverse fdb table failed,ret %d\n",ret);
		}
		else{
           count[unit] = usrdata.count;
		}
		memset(&usrdata,0,sizeof(user_data));
	}
	if(count[0]!= count[1]){
		syslog_ax_nam_fdb_err("unit 0 count %d not equal unit 1 count %d\n",count[0],count[1]);
		return count[0]>count[1] ? count[0]:count[1];
	}
	return count[0];
#endif
}



/************************************************************
 *nam_show_static_fdb
 * 
 * dispaly avalid static fdb items 
 *
 *	input params:
 *		fdb -hold output fdb items pointer
 *		size- the maxnum dispaly count
 * 
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/

unsigned int nam_show_static_fdb 
(
	NPD_FDB *fdb,
	unsigned int size
)
{
#ifdef DRV_LIB_BCM
	int i = 0,j = 0,unit = 0,num_of_asic = 0;
	int ret = 0;
	unsigned int eth_g_index = 0,old_value = 0;
	user_data usrdata;
#endif

#ifdef DRV_LIB_CPSS
    unsigned int 			index = 0;
	unsigned int           isMulti = 0;
	unsigned int 			eth_g_index=0;
	unsigned int			result=0;
	unsigned int         	i = 0;
	unsigned char			devNum =0;
	unsigned char			trgDev = 0, trgPort = 0;
	unsigned int        	validPtr=0;
    unsigned int      	skipPtr= 0;
    NAM_BOOL      		agedPtr=0;
	unsigned char			associatedDevNumPtr;
	CPSS_MAC_ENTRY_EXT_STC	entryPtr;
	ETHERADDR	*macPtr = NULL;
	unsigned int            board_type = 0;
#endif

#ifdef DRV_LIB_CPSS
	if(BOARD_TYPE == BOARD_TYPE_AX81_SMU)
	{
		board_type = 1;/*used for lion 32K fdb table*/
	}
	else
	{
		size = 16384;
	}
	nam_syslog_dbg("----------------------------------------------------------\n");
	nam_syslog_dbg("%-4s %-17s %-4s %-6s %-4s %-5s\n","ITEM","MAC Address","VLAN","DEVICE","PORT","ROUTE");
	nam_syslog_dbg("---- ----------------- ---- ------ ---- -----\n");
	do{
		 /* call cpss api function */
		result = cpssDxChBrgFdbMacEntryRead(devNum, index, &validPtr, &skipPtr,
										&agedPtr, &associatedDevNumPtr, &entryPtr);
		if((!validPtr) ||(skipPtr)||(entryPtr.isStatic==0)) {
				index++;
				continue;
		}
		if(entryPtr.key.key.macVlan.vlanId >= 4094)
		{
			index++;
			continue;
		}
		isMulti = nam_mac_is_multicast(entryPtr.key.key.macVlan.macAddr.arEther);
		if(!isMulti){
			if(entryPtr.key.entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E){
				index++;
				continue;
				}
			if((CPSS_MAC_TABLE_DROP_E== entryPtr.daCommand)||(CPSS_MAC_TABLE_DROP_E== entryPtr.saCommand)){
	            index++;
				continue;
			}
		}
        /*nam_syslog_err("entryPtr.static is : %d\n",entryPtr.isStatic);*/
	 	 if (result == 0) {/*0 right*/
	 	 		
	 			macPtr = &(entryPtr.key.key.macVlan.macAddr);
	 			nam_syslog_dbg("%-4d %02X:%02X:%02X:%02X:%02X:%02X %-4d %-6d %-4d %-5s\n",i+1, \
						macPtr->arEther[0],macPtr->arEther[1],macPtr->arEther[2],	\
						macPtr->arEther[3],macPtr->arEther[4],macPtr->arEther[5],	\
						entryPtr.key.key.macVlan.vlanId,		\
						entryPtr.dstInterface.devPort.devNum,	\
						entryPtr.dstInterface.devPort.portNum,	\
						(entryPtr.daRoute ==1) ? "en":"dis");
				fdb[i].vlanid = entryPtr.key.key.macVlan.vlanId;
                memcpy(fdb[i].ether_mac,entryPtr.key.key.macVlan.macAddr.arEther,6);

				trgDev = entryPtr.dstInterface.devPort.devNum;
				trgPort = entryPtr.dstInterface.devPort.portNum;
            	if( entryPtr.dstInterface.type == CPSS_INTERFACE_PORT_E) {
	            		fdb[i].inter_type = CPSS_INTERFACE_PORT_E;

				    NAM_CONVERT_BACK_DEV_PORT_MAC(entryPtr.dstInterface.devPort.devNum, 
							 				entryPtr.dstInterface.devPort.portNum);
					if((CPU_PORT_ASIC_PORTNO == entryPtr.dstInterface.devPort.portNum)||
						(CPSS_MAC_TABLE_CNTL_E == entryPtr.daCommand)){ /* target is CPU*/
						eth_g_index = g_cpu_port_index;
					}	
					else if (NPD_TRUE == npd_product_check_spi_port(trgDev, trgPort)) {
						eth_g_index = g_spi_port_index;
					}
					else {
	            		if(npd_get_global_index_by_devport(devNum ,entryPtr.dstInterface.devPort.portNum ,&eth_g_index)){
                            index++;
							continue;
						}
						index++;
						continue;
					}
				 	fdb[i].value= eth_g_index;
			 	}
				
			 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_TRUNK_E){
			 		fdb[i].inter_type = CPSS_INTERFACE_TRUNK_E;
					fdb[i].value = entryPtr.dstInterface.trunkId;
			 	}
			 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_VID_E){
			 		fdb[i].inter_type = CPSS_INTERFACE_VID_E;
			 		fdb[i].value = entryPtr.dstInterface.vlanId ;
				}
			 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_VIDX_E){
			 		fdb[i].inter_type = CPSS_INTERFACE_VIDX_E;
			 		fdb[i].value = entryPtr.dstInterface.vidx;
				}
				else {
				 	nam_syslog_err ("illegal desinterface type!/n");
				 	index++;
					continue;
				}
				i++;
		 }
		else {
			index++;
			continue;
		}
		index++;
	}while((!result)&&(i < size)&&(((board_type == 1)?(index<32768):(index<16384))));
	nam_syslog_dbg("index is: %d\n",index);
	nam_syslog_dbg("return i = %d\n ",i);
    return i;	
#endif

#ifdef DRV_LIB_BCM
	memset(&usrdata,0,sizeof(user_data));
	num_of_asic = nam_asic_get_instance_num();
	for(unit = 0;unit < num_of_asic;unit++){
		ret = bcm_l2_traverse(unit,nam_l2_addr_show,&usrdata);
		sleep(1);
		if(NAM_E_NONE != ret){
		   syslog_ax_nam_fdb_err("Traverse fdb table failed,ret %d\n",ret);
		}
		if(0 >= usrdata.count)
			 continue;
		for(i = 0;i < usrdata.count;i++){
		   /*check read info */
		   if((usrdata.l2addr[i].port < 0)||(usrdata.l2addr[i].port > 29)||((0 < usrdata.l2addr[i].port)&&(( usrdata.l2addr[i].port < 6)))){
			  /* syslog_ax_nam_fdb_err("error port %d read from bcm!\n",usrdata.l2addr[i].port);*/
			   continue;
		   }
		   else if(!(usrdata.l2addr[i].flags & BCM_L2_STATIC)){
			   continue;
		   }
		   else if((usrdata.l2addr[i].flags & BCM_L2_DISCARD_DST)||(usrdata.l2addr[i].flags & BCM_L2_DISCARD_SRC)){
			  /* syslog_ax_nam_fdb_dbg("Blacklist do not show!\n");*/
			   continue;
		   }
		   else if(unit != usrdata.l2addr[i].modid){/*do not show the remote fdb item*/
			  /* syslog_ax_nam_fdb_dbg("The item (modid %d)is on remote asic %d",usrdata.l2addr[i].modid,unit);*/
			   continue;
		   }
		   fdb[j].vlanid =usrdata.l2addr[i].vid;
		   memcpy(fdb[j].ether_mac,usrdata.l2addr[i].mac,6);
		   fdb[j].inter_type = (usrdata.l2addr[i].flags & BCM_L2_TRUNK_MEMBER)? 1:0;
		   if(!(fdb[j].inter_type)){/*port type*/
			  if(usrdata.l2addr[i].port == 0){/*cpu port*/
				fdb[j].value = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX( \
							CPU_PORT_VIRTUAL_SLOT,CPU_PORT_VIRTUAL_PORT);/*tmp value for cpu identify*/
			  }
			  else{

				  continue;
				 /* npd_syslog_dbg("modid %d,port %d\n",usrdata.l2addr[i].modid,usrdata.l2addr[i].port);*/
				/*  npd_get_global_index_by_devport(usrdata.l2addr[i].modid,usrdata.l2addr[i].port,&eth_g_index);
				  fdb[j].value = eth_g_index;*/
			  }
		   }
		   else{
			  fdb[j].value = usrdata.l2addr[i].tgid;
		   }
		   fdb[j].type_flag = (usrdata.l2addr[i].flags & BCM_L2_STATIC)? 1 : 0;
		   if(j<size)
			   j++;
		   else
			   return j;
		}
		memset(&usrdata,0,sizeof(user_data));
	}
	return j;
#endif
}


/************************************************************
 *nam_show_fdb_port
 * 
 * dispaly avalid  fdb items destination interface is the port
 *
 *	input params:
 *		fdb -hold output fdb items pointer
 *		size- the maxnum dispaly count
 *          port-globle index value which is get from slot/port
 * 
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/

unsigned int nam_show_fdb_port
(
	NPD_FDB *fdb,
	unsigned int size,
	unsigned int port
)
{

#ifdef DRV_LIB_BCM
    int ret = 0;
	int i = 0,j = 0,unit = 0,num_of_asic = 0;
	unsigned int eth_g_index = 0,old_value = 0;
	user_data usrdata;
#endif

#ifdef DRV_LIB_CPSS
    unsigned int 			index = 0;
	unsigned int 			eth_g_index;
	unsigned int			result;
	unsigned int         	i = 0,j=0;
	unsigned char           dev=0;
	unsigned int            actDev;
	unsigned char			devNum =0;
	unsigned char           slotNum=0;
	unsigned int        	validPtr=0;
	unsigned char       asic_num= 0,ammount_asic_nums = 0;
    unsigned int       	skipPtr= 0;
    NAM_BOOL      		agedPtr;
	unsigned char			associatedDevNumPtr;
	CPSS_MAC_ENTRY_EXT_STC	entryPtr;
	ETHERADDR	*macPtr = NULL;
	unsigned int            board_type = 0;
	unsigned long ret = 0;
#endif

#ifdef DRV_LIB_CPSS
	if(BOARD_TYPE == BOARD_TYPE_AX81_12X)
	{
		board_type = 1;/*used for lion 32K fdb table*/
	}
	else
	{
		size = 16384;
	}
    result = npd_get_devport_by_global_index(port,&devNum, &slotNum);
    if(NAM_OK != result){
		syslog_ax_nam_fdb_err("eth_index %#x get asic port failed for ret %d\n",port,result);
        return FDB_CONFIG_FAIL;
	}
	nam_syslog_dbg("----------------------------------------------------------\n");
	nam_syslog_dbg("%-4s %-17s %-4s %-6s %-4s %-5s\n","ITEM","MAC Address","VLAN","DEVICE","PORT","ROUTE");
	nam_syslog_dbg("---- ----------------- ---- ------ ---- -----\n");

	if(IS_DISTRIBUTED == SYSTEM_TYPE)
	{
		/*code optimize: Unchecked return value 
		zhangcl@autelan.com 2013-1-21*/
		ret = nam_get_distributed_devnum(asic_num,&actDev);
		if(ret != FDB_CONFIG_SUCCESS)
		{
			syslog_ax_nam_fdb_err("get devnum fail !\n");
			return FDB_CONFIG_FAIL;
		}

		do
		{
			 /* call cpss api function */
			result = cpssDxChBrgFdbMacEntryRead(dev, index, &validPtr, &skipPtr,
											&agedPtr, &associatedDevNumPtr, &entryPtr);
			if((!validPtr) ||(skipPtr)) 
			{
				index++;
				continue;
			}
	        if(entryPtr.key.entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E)
			{
				index++;
				continue;
			}
			if((CPSS_MAC_TABLE_DROP_E == entryPtr.daCommand)||(CPSS_MAC_TABLE_DROP_E == entryPtr.saCommand))
			{
	           index++;
			   continue;
			}
			if(entryPtr.key.key.macVlan.vlanId >= 4094)
			{
				index++;
				continue;
			}
		 	if ((result == 0)&&(slotNum==entryPtr.dstInterface.devPort.portNum) && (actDev == entryPtr.dstInterface.devPort.devNum)) 
			{/*0 right*/
	 			macPtr = &(entryPtr.key.key.macVlan.macAddr);
				fdb[i].vlanid = entryPtr.key.key.macVlan.vlanId;
                memcpy(fdb[i].ether_mac,entryPtr.key.key.macVlan.macAddr.arEther,6);

            	if( entryPtr.dstInterface.type == CPSS_INTERFACE_PORT_E) 
				{
					/*if not local dev,do not show for distributed system*/
					if(entryPtr.dstInterface.devPort.devNum != g_dis_dev[0])
					{
						index++;
						continue;
					}
					switch(BOARD_TYPE)
					{
						case BOARD_TYPE_AX81_AC12C:
							if(entryPtr.dstInterface.devPort.portNum > 5)
							{
								index++;
								continue;
							}
							break;
						case BOARD_TYPE_AX81_AC8C:
							if(entryPtr.dstInterface.devPort.portNum > 3)
							{
								index++;
								continue;
							}
							break;
						case BOARD_TYPE_AX71_2X12G12S:
							if(entryPtr.dstInterface.devPort.portNum > 25)
							{
								index++;
								continue;
							}
							break;
						case BOARD_TYPE_AX81_1X12G12S:
							if(entryPtr.dstInterface.devPort.portNum > 23)
							{
								index++;
								continue;
							}
							break;
						default:
							nam_syslog_err("unknown board type !\n");
							break;
					}
            		fdb[i].inter_type = CPSS_INTERFACE_PORT_E;

					if (npd_get_global_index_by_devport( devNum,entryPtr.dstInterface.devPort.portNum ,&eth_g_index))
 					{
                        index++;
						continue;
					}
				 	fdb[i].value= eth_g_index;
			 	}
			 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_TRUNK_E)
				{
					if(entryPtr.dstInterface.trunkId > 110)
					{
						index++;
						continue;
					}
					fdb[i].inter_type = CPSS_INTERFACE_TRUNK_E;
					fdb[i].value = entryPtr.dstInterface.trunkId;
			 	}
			 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_VID_E)
				{
			 		fdb[i].inter_type = CPSS_INTERFACE_VID_E;
			 		fdb[i].value = entryPtr.dstInterface.vlanId ;
				}
			 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_VIDX_E)
				{
			 		fdb[i].inter_type = CPSS_INTERFACE_VIDX_E;
			 		fdb[i].value = entryPtr.dstInterface.vidx;
				}
				else 
				{
				 	nam_syslog_err ("illegal desinterface type!/n");
				 	return 0;
				}
				i++;
			}
			else 
			{
				index++;
				continue;
			}
			index++;
	    }while((!result)&&(i < size)&&(((board_type == 1)?(index<32768):(index<16384))));
	}
	else
	{

		do{
			 /* call cpss api function */
			result = cpssDxChBrgFdbMacEntryRead(dev, index, &validPtr, &skipPtr,
											&agedPtr, &associatedDevNumPtr, &entryPtr);
			
			if((!validPtr) ||(skipPtr)) {
					index++;
					continue;
			}
	        if(entryPtr.key.entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E){
						index++;
						continue;
			}
			if((CPSS_MAC_TABLE_DROP_E == entryPtr.daCommand)||(CPSS_MAC_TABLE_DROP_E == entryPtr.saCommand)){
	           index++;
			   continue;
			}
		 	if ((result == 0)&&(devNum==entryPtr.dstInterface.devPort.devNum)\
			 					&&(slotNum==entryPtr.dstInterface.devPort.portNum)) {/*0 right*/
		 			macPtr = &(entryPtr.key.key.macVlan.macAddr);
		 			nam_syslog_dbg("%-4d %02X:%02X:%02X:%02X:%02X:%02X %-4d %-6d %-4d %-5s\n",i, \
							macPtr->arEther[0],macPtr->arEther[1],macPtr->arEther[2],	\
							macPtr->arEther[3],macPtr->arEther[4],macPtr->arEther[5],	\
							entryPtr.key.key.macVlan.vlanId,		\
							entryPtr.dstInterface.devPort.devNum,	\
							entryPtr.dstInterface.devPort.portNum,	\
							(entryPtr.daRoute ==1) ? "en":"dis");
					fdb[i].vlanid = entryPtr.key.key.macVlan.vlanId;
	                memcpy(fdb[i].ether_mac,entryPtr.key.key.macVlan.macAddr.arEther,6);

	            	if( entryPtr.dstInterface.type == CPSS_INTERFACE_PORT_E) {
		            		fdb[i].inter_type = CPSS_INTERFACE_PORT_E;
						 NAM_CONVERT_BACK_DEV_PORT_MAC(entryPtr.dstInterface.devPort.devNum, \
								 				entryPtr.dstInterface.devPort.portNum);
	            		if (npd_get_global_index_by_devport( entryPtr.dstInterface.devPort.devNum , \
	 											entryPtr.dstInterface.devPort.portNum ,&eth_g_index)){
	                        index++;
							continue;
						}
					 	fdb[i].value= eth_g_index;
				 	}
				 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_TRUNK_E){
				 		fdb[i].inter_type = CPSS_INTERFACE_TRUNK_E;
						fdb[i].value = entryPtr.dstInterface.trunkId;
				 	}
				 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_VID_E){
				 		fdb[i].inter_type = CPSS_INTERFACE_VID_E;
				 		fdb[i].value = entryPtr.dstInterface.vlanId ;
					}
				 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_VIDX_E){
				 		fdb[i].inter_type = CPSS_INTERFACE_VIDX_E;
				 		fdb[i].value = entryPtr.dstInterface.vidx;
					}
					else {
					 	nam_syslog_err ("illegal desinterface type!/n");
					 	return 0;
					}
					i++;
			 }
			else {
				index++;
				continue;
			}
			index++;
		}while((!result)&&(i < size));
	}
	/*code optimize: Structurally dead code
	zhangcl@autelan.com 2013-1-21*/
	nam_syslog_dbg("i is %d in func: nam_show_fdb_port.\n",i);
    return i;	
#endif

#ifdef DRV_LIB_BCM
	memset(&usrdata,0,sizeof(user_data));
	num_of_asic = nam_asic_get_instance_num();
	for(unit = 0;unit < num_of_asic;unit++){
		ret = bcm_l2_traverse(unit,nam_l2_addr_show,&usrdata);
		sleep(1);
		if(NAM_E_NONE != ret){
		   syslog_ax_nam_fdb_err("Traverse fdb table failed,ret %d\n",ret);
		}
		if(0 >= usrdata.count)
			 continue;
		for(i = 0;i < usrdata.count;i++){
		   if((usrdata.l2addr[i].port < 0)||(usrdata.l2addr[i].port > 29)||((0 < usrdata.l2addr[i].port)&&(( usrdata.l2addr[i].port < 6)))){
			   /*syslog_ax_nam_fdb_err("error port %d read from bcm!\n",usrdata.l2addr[i].port);*/
			   continue;
		   }
		   else if((usrdata.l2addr[i].flags & BCM_L2_DISCARD_DST)||(usrdata.l2addr[i].flags & BCM_L2_DISCARD_SRC)){
			  /* syslog_ax_nam_fdb_dbg("Blacklist do not show!\n");*/
			   continue;
		   }
		   else if(unit != usrdata.l2addr[i].modid){/*do not show the remote fdb item*/
			  /* syslog_ax_nam_fdb_dbg("The item (modid %d)is on remote asic %d",usrdata.l2addr[i].modid,unit);*/
			   continue;
		   }	   
		   fdb[j].inter_type = (usrdata.l2addr[i].flags & BCM_L2_TRUNK_MEMBER)? 1:0;
		   if(!(fdb[j].inter_type)){/*port type*/
			  if(usrdata.l2addr[i].port == 0){/*cpu port*/
				fdb[j].value = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX( \
							CPU_PORT_VIRTUAL_SLOT,CPU_PORT_VIRTUAL_PORT);/*tmp value for cpu identify*/
			  }
			  else{
				 /* npd_syslog_dbg("modid %d,port %d\n",usrdata.l2addr[i].modid,usrdata.l2addr[i].port);*/
				  npd_get_global_index_by_devport(usrdata.l2addr[i].modid,usrdata.l2addr[i].port,&eth_g_index);
				  fdb[j].value = eth_g_index;
				  if(port != eth_g_index)
				  	continue;
			  }
		   }
		   else{
			  fdb[j].value = usrdata.l2addr[i].tgid;
		   }
		   fdb[j].vlanid =usrdata.l2addr[i].vid;
		   memcpy(fdb[j].ether_mac,usrdata.l2addr[i].mac,6);
		   fdb[j].type_flag = (usrdata.l2addr[i].flags & BCM_L2_STATIC)? 1 : 0;
		   if(j<size)
			   j++;
		   else
			   return j;
		}
		memset(&usrdata,0,sizeof(user_data));
	}
	return j;
#endif
}



/************************************************************
 *nam_show_fdb_vlan
 * 
 * dispaly avalid  fdb items belong to the vlan
 *
 *	input params:
 *		fdb -hold output fdb items pointer
 *		size- the maxnum dispaly count
 *         vlan-the valid vlan index
 * 
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/

unsigned int nam_show_fdb_vlan
(
	NPD_FDB *fdb,
	unsigned int size,
	unsigned short vlan
)
{
#ifdef DRV_LIB_BCM
	int i = 0,j = 0,unit = 0,num_of_asic = 0;
	int ret = 0;
	unsigned int eth_g_index = 0,old_value = 0;
	user_data usrdata;
#endif

#ifdef DRV_LIB_CPSS
    unsigned int 			index = 0;
	unsigned int 			eth_g_index;
	unsigned int			result;
	unsigned int         	i = 0,j=0;
	unsigned char           dev=0;
	unsigned char			trgDev = 0, trgPort = 0;
	unsigned int        	validPtr=0;
    unsigned int       	skipPtr= 0;
    NAM_BOOL      		agedPtr;
	unsigned char			associatedDevNumPtr;
	CPSS_MAC_ENTRY_EXT_STC	entryPtr;
	ETHERADDR	*macPtr = NULL;
	unsigned int            board_type = 0;
#endif

#ifdef DRV_LIB_CPSS
	nam_syslog_dbg("----------------------------------------------------------\n");
	nam_syslog_dbg("%-4s %-17s %-4s %-6s %-4s %-5s\n","ITEM","MAC Address","VLAN","DEVICE","PORT","ROUTE");
	nam_syslog_dbg("---- ----------------- ---- ------ ---- -----\n");
	if((BOARD_TYPE == BOARD_TYPE_AX81_SMU) || (BOARD_TYPE == BOARD_TYPE_AX71_CRSMU))
	{
		return 0;	/*if BOARD_TYPE is master  return zero fdb item*/
	}
	if(BOARD_TYPE == BOARD_TYPE_AX81_12X)
	{
		board_type = 1;/*used for lion 32K fdb table*/
	}
	else
	{
		size = 16384;
	}

		do{
			 /* call cpss api function */
			result = cpssDxChBrgFdbMacEntryRead(dev, index, &validPtr, &skipPtr,
											&agedPtr, &associatedDevNumPtr, &entryPtr);
			
			if((!validPtr) ||(skipPtr)) 
			{
					index++;
					continue;
			}
	        if(entryPtr.key.entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E)
			{
				index++;
				continue;
			}
			if(entryPtr.daCommand==CPSS_MAC_TABLE_CNTL_E)
			{
				index++;
				continue;
			}
			if((CPSS_MAC_TABLE_DROP_E == entryPtr.daCommand)||(CPSS_MAC_TABLE_DROP_E == entryPtr.saCommand))
			{
	           index++;
			   continue;
			}
			if(entryPtr.key.key.macVlan.vlanId >= 4094)
			{
				index++;
				continue;
			}
		 	if ((result == 0)&&(vlan==entryPtr.key.key.macVlan.vlanId)) 
			{
			 		
	 			macPtr = &(entryPtr.key.key.macVlan.macAddr);
	 			nam_syslog_dbg("%-4d %02X:%02X:%02X:%02X:%02X:%02X %-4d %-6d %-4d %-5s\n",i, \
						macPtr->arEther[0],macPtr->arEther[1],macPtr->arEther[2],	\
						macPtr->arEther[3],macPtr->arEther[4],macPtr->arEther[5],	\
						entryPtr.key.key.macVlan.vlanId,		\
						entryPtr.dstInterface.devPort.devNum,	\
						entryPtr.dstInterface.devPort.portNum,	\
						(entryPtr.daRoute ==1) ? "en":"dis");
				fdb[i].vlanid = entryPtr.key.key.macVlan.vlanId;
	            memcpy(fdb[i].ether_mac,entryPtr.key.key.macVlan.macAddr.arEther,6);

				trgDev = entryPtr.dstInterface.devPort.devNum;
				trgPort = entryPtr.dstInterface.devPort.portNum;

	        	if( entryPtr.dstInterface.type == CPSS_INTERFACE_PORT_E) 
				{
	            	fdb[i].inter_type = CPSS_INTERFACE_PORT_E;
					 NAM_CONVERT_BACK_DEV_PORT_MAC(entryPtr.dstInterface.devPort.devNum, \
							 				entryPtr.dstInterface.devPort.portNum);
					 if(CPU_PORT_ASIC_PORTNO == entryPtr.dstInterface.devPort.portNum)
					 { /* target is CPU*/
					 	eth_g_index = g_cpu_port_index;
					 }
					 else if (NPD_TRUE == npd_product_check_spi_port(trgDev, trgPort)) 
					 {
						 eth_g_index = g_spi_port_index;
					 }
					 else
					 {
					 	if(trgDev != g_dis_dev[0])
						{
							index++;
							continue;
						}
					 	switch(BOARD_TYPE)
						{
							case BOARD_TYPE_AX81_AC12C:
								if(entryPtr.dstInterface.devPort.portNum > 5)
								{
									index++;
									continue;
								}
								break;
							case BOARD_TYPE_AX81_AC8C:
								if(entryPtr.dstInterface.devPort.portNum > 3)
								{
									index++;
									continue;
								}
								break;
							case BOARD_TYPE_AX71_2X12G12S:
								if(entryPtr.dstInterface.devPort.portNum > 25)
								{
									index++;
									continue;
								}
								break;
							case BOARD_TYPE_AX81_1X12G12S:
								if(entryPtr.dstInterface.devPort.portNum > 23)
								{
									index++;
									continue;
								}
								break;
							default:
								nam_syslog_err("unknown board type !\n");
								break;
						}
	            		if (npd_get_global_index_by_devport(dev ,entryPtr.dstInterface.devPort.portNum ,&eth_g_index))
	 					{
	                        index++;
							continue;
						}
					}
				 	fdb[i].value= eth_g_index;
			 	}
			 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_TRUNK_E)
				{
					if(entryPtr.dstInterface.trunkId > 110)
					{
						index++;
						continue;
					}
			 		fdb[i].inter_type = CPSS_INTERFACE_TRUNK_E;
					fdb[i].value = entryPtr.dstInterface.trunkId;
			 	}
			 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_VID_E)
				{
			 		fdb[i].inter_type = CPSS_INTERFACE_VID_E;
			 		fdb[i].value = entryPtr.dstInterface.vlanId ;
				}
			 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_VIDX_E)
				{
			 		fdb[i].inter_type = CPSS_INTERFACE_VIDX_E;
			 		fdb[i].value = entryPtr.dstInterface.vidx;
				}
				else 
				{
				 	nam_syslog_err ("illegal desinterface type!/n");
				 	return 0;
				}
				i++;
			}
			else 
			{
				index++;
				continue;
			}
			index++;
		}while((!result)&&(i < size)&&(((board_type == 1)?(index<32768):(index<16384))));

	nam_syslog_dbg("i is %d\n",i);
    return i;	
#endif

#ifdef DRV_LIB_BCM
	memset(&usrdata,0,sizeof(user_data));
	num_of_asic = nam_asic_get_instance_num();
	for(unit = 0;unit < num_of_asic;unit++){
		ret = bcm_l2_traverse(unit,nam_l2_addr_show,&usrdata);
		sleep(1);
		if(NAM_E_NONE != ret){
		   syslog_ax_nam_fdb_err("Traverse fdb table failed,ret %d\n",ret);
		}
		if(0 >= usrdata.count)
			 continue;
		for(i = 0;i < usrdata.count;i++){
		   if((usrdata.l2addr[i].port < 0)||(usrdata.l2addr[i].port > 29)||((0 < usrdata.l2addr[i].port)&&(( usrdata.l2addr[i].port < 6)))){
			  /* syslog_ax_nam_fdb_err("error port %d read from bcm!\n",usrdata.l2addr[i].port);*/
			   continue;
		   }
		   else if((usrdata.l2addr[i].flags & BCM_L2_DISCARD_DST)||(usrdata.l2addr[i].flags & BCM_L2_DISCARD_SRC)){
			   /*syslog_ax_nam_fdb_dbg("Blacklist do not show!\n");*/
			   continue;
		   }
		   else if(unit != usrdata.l2addr[i].modid){/*do not show the remote fdb item*/
			  /* syslog_ax_nam_fdb_dbg("The item (modid %d)is on remote asic %d",usrdata.l2addr[i].modid,unit);*/
			   continue;
		   }
		   else if(vlan != usrdata.l2addr[i].vid){
               continue;		   
		   }
		   fdb[j].vlanid =usrdata.l2addr[i].vid;
		   fdb[j].inter_type = (usrdata.l2addr[i].flags & BCM_L2_TRUNK_MEMBER)? 1:0;
		   if(!(fdb[j].inter_type)){/*port type*/
			  if(usrdata.l2addr[i].port == 0){/*cpu port*/
				fdb[j].value = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX( \
							CPU_PORT_VIRTUAL_SLOT,CPU_PORT_VIRTUAL_PORT);/*tmp value for cpu identify*/
			  }
			  else{
				 /* npd_syslog_dbg("modid %d,port %d\n",usrdata.l2addr[i].modid,usrdata.l2addr[i].port);*/
				  npd_get_global_index_by_devport(usrdata.l2addr[i].modid,usrdata.l2addr[i].port,&eth_g_index);
				  fdb[j].value = eth_g_index;
			  }
		   }
		   else{
			  fdb[j].value = usrdata.l2addr[i].tgid;
		   }
		   memcpy(fdb[j].ether_mac,usrdata.l2addr[i].mac,6);
		   fdb[j].type_flag = (usrdata.l2addr[i].flags & BCM_L2_STATIC)? 1 : 0;
		   if(j<size)
			   j++;
		   else
			   return j;
		}
		memset(&usrdata,0,sizeof(user_data));
	}
	return j;
#endif
}

unsigned int nam_show_fdb_one (NPD_FDB* fdb,unsigned char macAddr[6],unsigned short	vlanId)
{
#ifdef DRV_LIB_BCM
	int i = 0,j = 0,unit = 0,num_of_asic = 0;
	int ret = 0;
	unsigned int eth_g_index = 0,old_value = 0;
	user_data usrdata;
#endif

#ifdef DRV_LIB_CPSS
  	unsigned char devNum,dev;
	unsigned char trgDev = 0, trgPort = 0;
	unsigned int eth_g_index;
	unsigned int i;
	unsigned int hashPtr,ret;
	
	unsigned int                    validPtr;
    unsigned int                    skipPtr;
    NAM_BOOL                         agedPtr;
    unsigned char                    associatedDevNumPtr;
	CPSS_MAC_ENTRY_EXT_KEY_STC       key;
    CPSS_MAC_ENTRY_EXT_STC           entryPtr;
#endif

#ifdef DRV_LIB_CPSS
	devNum = FDB_INIT;
	dev = 0;
	
	key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
	for (i=0;i<6;i++) {
   		key.key.macVlan.macAddr.arEther[i] = macAddr[i];
		
	}
	
    key.key.macVlan.vlanId = vlanId;
	
	
	ret=cpssDxChBrgFdbHashCalc(  dev,&key, &hashPtr);
	
	if( ret!=0){
		
		nam_syslog_err("Failed to get the hash ptr according the key (mac + vlan)\n");
		return FDB_CONFIG_FAIL;
	}
	
	 ret=cpssDxChBrgFdbMacEntryRead(dev, hashPtr,&validPtr,&skipPtr,&agedPtr,&associatedDevNumPtr, &entryPtr);
	 nam_syslog_dbg("read static fdb entry isStatic value is : %d\n",entryPtr.isStatic);
     if(ret!=0){
	 	
	 	nam_syslog_err("Read hw fdb entry error through hashPtr.\n");
		return FDB_CONFIG_FAIL;
	 }
     if((CPSS_MAC_TABLE_DROP_E == entryPtr.daCommand)||(CPSS_MAC_TABLE_DROP_E == entryPtr.saCommand)){
        return FDB_CONFIG_FAIL;
	}
	 if(vlanId != entryPtr .key.key.macVlan.vlanId){
	 	
	 	nam_syslog_err("The vlan read from hw is not equal with the vid transfered\n");
		return FDB_CONFIG_FAIL;
	 }
	 
	 fdb->vlanid =(unsigned int) (entryPtr .key.key.macVlan.vlanId);
	 
	 memcpy(fdb->ether_mac,entryPtr.key.key.macVlan.macAddr.arEther,6);
	
	trgDev = entryPtr.dstInterface.devPort.devNum;
	trgPort = entryPtr.dstInterface.devPort.portNum;

	 if( entryPtr.dstInterface.type == CPSS_INTERFACE_PORT_E){
	 	 fdb->inter_type = CPSS_INTERFACE_PORT_E;

		 if(CPU_PORT_ASIC_PORTNO == entryPtr.dstInterface.devPort.portNum){ /* target port is CPU*/
			eth_g_index = g_cpu_port_index;
		 }
		 else if (NPD_TRUE == npd_product_check_spi_port(trgDev, trgPort)) {
			 eth_g_index = g_spi_port_index;
		 }
		 else {
		     ret = npd_get_global_index_by_devport( devNum ,entryPtr.dstInterface.devPort.portNum ,&eth_g_index);
			 if(0 != ret){
				 nam_syslog_err ("Fail to get gloable index when show fdb one!/n");
				 return FDB_CONFIG_FAIL;	 
			 }
		 }	
		 fdb->value= eth_g_index;
	 }
	 else if (entryPtr.dstInterface.type == CPSS_INTERFACE_TRUNK_E){
	 	fdb->inter_type = CPSS_INTERFACE_TRUNK_E;
		fdb->value = entryPtr.dstInterface.trunkId;
	 }
	 else if (entryPtr.dstInterface.type == CPSS_INTERFACE_VID_E){
	 	fdb->inter_type = CPSS_INTERFACE_VID_E;
	 	fdb->value = entryPtr.dstInterface.vlanId ;
		}
	 else if (entryPtr.dstInterface.type == CPSS_INTERFACE_VIDX_E){
	 	fdb->inter_type = CPSS_INTERFACE_VIDX_E;
	 	fdb->value = entryPtr.dstInterface.vidx;
			}
	 else {
	 	nam_syslog_err ("illegal desinterface type!/n");
	 	return FDB_CONFIG_FAIL;
     }
#endif

#ifdef DRV_LIB_BCM
    memset(&usrdata,0,sizeof(user_data));
	num_of_asic = nam_asic_get_instance_num();
	for(unit = 0;unit < num_of_asic;unit++){
		ret = bcm_l2_traverse(unit,nam_l2_addr_show,&usrdata);
		sleep(1);
		if(NAM_E_NONE != ret){
		   syslog_ax_nam_fdb_err("Traverse fdb table failed,ret %d\n",ret);
		}
		if(0 >= usrdata.count)
			 continue;
		for(i = 0;i < usrdata.count;i++){
		   if((usrdata.l2addr[i].port < 0)||(usrdata.l2addr[i].port > 29)||((0 < usrdata.l2addr[i].port)&&(( usrdata.l2addr[i].port < 6)))){
			  /* syslog_ax_nam_fdb_err("error port %d read from bcm!\n",usrdata.l2addr[i].port);*/
			   continue;
		   }
		   else if((usrdata.l2addr[i].flags & BCM_L2_DISCARD_DST)||(usrdata.l2addr[i].flags & BCM_L2_DISCARD_SRC)){
			   /*syslog_ax_nam_fdb_dbg("Blacklist do not show!\n");*/
			   continue;
		   }
		   else if(unit != usrdata.l2addr[i].modid){/*do not show the remote fdb item*/
			   /*syslog_ax_nam_fdb_dbg("The item (modid %d)is on remote asic %d",usrdata.l2addr[i].modid,unit);*/
			   continue;
		   }
           if((!memcmp(macAddr,usrdata.l2addr[i].mac,6))&&(vlanId == usrdata.l2addr[i].vid)){
               memcpy(fdb->ether_mac,macAddr,6);
			   fdb->vlanid = vlanId;
               fdb->inter_type = (usrdata.l2addr[i].flags & BCM_L2_TRUNK_MEMBER)? 1:0;
		       if(!(fdb->inter_type)){/*port type*/
				   if(usrdata.l2addr[i].port == 0){/*cpu port*/
					 fdb->value = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX( \
								CPU_PORT_VIRTUAL_SLOT,CPU_PORT_VIRTUAL_PORT);/*tmp value for cpu identify*/
				   }
				   else{
					 /* npd_syslog_dbg("modid %d,port %d\n",usrdata.l2addr[i].modid,usrdata.l2addr[i].port);*/
					  npd_get_global_index_by_devport(usrdata.l2addr[i].modid,usrdata.l2addr[i].port,&eth_g_index);
					  fdb->value = eth_g_index;
				   }
			   }
			   else{
				  fdb->value = usrdata.l2addr[i].tgid;
			   }
			   fdb->type_flag = (usrdata.l2addr[i].flags & BCM_L2_STATIC)? 1 : 0;			   
               break;
		   }
		  continue;           
		}
		memset(&usrdata,0,sizeof(user_data));
	}
#endif
    return FDB_CONFIG_SUCCESS;
}

/************************************************************
 *nam_show_fdb_mac
 * 
 * dispaly avalid  fdb items have the mac address
 *
 *	input params:
 *		fdb -hold output fdb items pointer
 *		macAddr-input mac address which need dispaly
 *		size- the maxnum dispaly count
 * 
 *	output: none
 *
 * 	return:
 *		 1- if there exist erro
 *		 0- if operation ok
 *		
 *********************************************/

unsigned int nam_show_fdb_mac (NPD_FDB *fdb,unsigned char macAddr[6],unsigned int size)
{
#ifdef DRV_LIB_BCM
	int i = 0,j = 0,unit = 0,num_of_asic = 0;
	int ret = 0;
	unsigned int eth_g_index = 0,old_value = 0;
	user_data usrdata;
#endif

#ifdef DRV_LIB_CPSS
    unsigned int 			index = 0;
	unsigned int 			eth_g_index=0;
	unsigned int			result=0;
	unsigned int         	i = 0;
	unsigned char			devNum =0;
	unsigned char			trgDev = 0, trgPort = 0;
	unsigned int        	validPtr=0;
    unsigned int       	skipPtr= 0;
    NAM_BOOL      		agedPtr;
	unsigned char			associatedDevNumPtr;
	CPSS_MAC_ENTRY_EXT_STC	entryPtr;
	ETHERADDR	*macPtr = NULL;
	unsigned int            board_type = 0;
#endif

#ifdef DRV_LIB_CPSS
	nam_syslog_dbg("----------------------------------------------------------\n");
	nam_syslog_dbg("%-4s %-17s %-4s %-6s %-4s %-5s\n","ITEM","MAC Address","VLAN","DEVICE","PORT","ROUTE");
	nam_syslog_dbg("---- ----------------- ---- ------ ---- -----\n");
	if((BOARD_TYPE == BOARD_TYPE_AX81_SMU) || (BOARD_TYPE == BOARD_TYPE_AX71_CRSMU))
	{
		return 0;	/*if BOARD_TYPE is master  return zero fdb item*/
	}
	if(BOARD_TYPE == BOARD_TYPE_AX81_12X)
	{
		board_type = 1;/*used for lion 32K fdb table*/
	}
	else
	{
		size = 16384;
	}
	do{
		 /* call cpss api function */
		result = cpssDxChBrgFdbMacEntryRead(devNum, index, &validPtr, &skipPtr,
										&agedPtr, &associatedDevNumPtr, &entryPtr);
		 macPtr = &(entryPtr.key.key.macVlan.macAddr);
		if((!validPtr) ||(skipPtr)) {
				index++;
				continue;
		}
       if(entryPtr.key.entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E){
	   				index++;
					continue;
		}
	   	if((CPSS_MAC_TABLE_DROP_E == entryPtr.daCommand)||(CPSS_MAC_TABLE_DROP_E == entryPtr.saCommand)){
           index++;
		   continue;
		}
		if(entryPtr.key.key.macVlan.vlanId >= 4094)
		{
			index++;
			continue;
		}
	   if ((result == 0)&& (1==nam_mac_match( macAddr,macPtr->arEther,6)))
		 	{
		 		
	 			nam_syslog_dbg("%-4d %02X:%02X:%02X:%02X:%02X:%02X %-4d %-6d %-4d %-5s\n",i+1, \
						macPtr->arEther[0],macPtr->arEther[1],macPtr->arEther[2],	\
						macPtr->arEther[3],macPtr->arEther[4],macPtr->arEther[5],	\
						entryPtr.key.key.macVlan.vlanId,		\
						entryPtr.dstInterface.devPort.devNum,	\
						entryPtr.dstInterface.devPort.portNum,	\
						(entryPtr.daRoute ==1) ? "en":"dis");
				fdb[i].vlanid = entryPtr.key.key.macVlan.vlanId;
                memcpy(fdb[i].ether_mac,entryPtr.key.key.macVlan.macAddr.arEther,6);

				trgDev = entryPtr.dstInterface.devPort.devNum;
				trgPort = entryPtr.dstInterface.devPort.portNum;
  
            	if( entryPtr.dstInterface.type == CPSS_INTERFACE_PORT_E) {
	            		fdb[i].inter_type = CPSS_INTERFACE_PORT_E;
					NAM_CONVERT_BACK_DEV_PORT_MAC(entryPtr.dstInterface.devPort.devNum, 
							 				entryPtr.dstInterface.devPort.portNum);
					if(CPU_PORT_ASIC_PORTNO == entryPtr.dstInterface.devPort.portNum){ /* target is CPU*/
						eth_g_index = g_cpu_port_index;
					}
					else if (NPD_TRUE == npd_product_check_spi_port(trgDev, trgPort)) {
						eth_g_index = g_spi_port_index;
					}	
					else
					{
						switch(BOARD_TYPE)
						{
							case BOARD_TYPE_AX81_AC12C:
								if(entryPtr.dstInterface.devPort.portNum > 5)
								{
									index++;
									continue;
								}
								break;
							case BOARD_TYPE_AX81_AC8C:
								if(entryPtr.dstInterface.devPort.portNum > 3)
								{
									index++;
									continue;
								}
								break;
							case BOARD_TYPE_AX71_2X12G12S:
								if(entryPtr.dstInterface.devPort.portNum > 25)
								{
									index++;
									continue;
								}
								break;
							case BOARD_TYPE_AX81_1X12G12S:
								if(entryPtr.dstInterface.devPort.portNum > 23)
								{
									index++;
									continue;
								}
								break;
							case BOARD_TYPE_AX81_12X:
								if(entryPtr.dstInterface.devPort.portNum > 11)
								{
									index++;
									continue;
								}
								break;
							default:
								nam_syslog_err("unknown board type !\n");
								break;
						}
	            		if (npd_get_global_index_by_devport( devNum,entryPtr.dstInterface.devPort.portNum ,&eth_g_index))
	            		{
							/*code optimize: Structurally dead code
							zhangcl@autelan.com 2013-1-21*/
							index++;
							continue;
						}
					}
				 	fdb[i].value= eth_g_index;
			 	}
			 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_TRUNK_E){
			 		fdb[i].inter_type = CPSS_INTERFACE_TRUNK_E;
					fdb[i].value = entryPtr.dstInterface.trunkId;
			 	}
			 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_VID_E){
			 		fdb[i].inter_type = CPSS_INTERFACE_VID_E;
			 		fdb[i].value = entryPtr.dstInterface.vlanId ;
				}
			 	else if (entryPtr.dstInterface.type == CPSS_INTERFACE_VIDX_E){
			 		fdb[i].inter_type = CPSS_INTERFACE_VIDX_E;
			 		fdb[i].value = entryPtr.dstInterface.vidx;
				}
				else {
				 	nam_syslog_err ("illegal desinterface type!/n");
				 	return 0;
				}
				i++;
		 }
		else {
			index++;
			continue;
		}
		index++;
	}while((!result)&&(i < size)&&(((board_type == 1)?(index<32768):(index<16384))));

	nam_syslog_dbg("return i = %d\n ",i);
    return i;	
#endif

#ifdef DRV_LIB_BCM
	memset(&usrdata,0,sizeof(user_data));
	num_of_asic = nam_asic_get_instance_num();
	for(unit = 0;unit < num_of_asic;unit++){
		ret = bcm_l2_traverse(unit,nam_l2_addr_show,&usrdata);
		sleep(1);
		if(NAM_E_NONE != ret){
		   syslog_ax_nam_fdb_err("Traverse fdb table failed,ret %d\n",ret);
		}
		if(0 >= usrdata.count)
			 continue;
		for(i = 0;i < usrdata.count;i++){
		   if((usrdata.l2addr[i].port < 0)||(usrdata.l2addr[i].port > 29)||((0 < usrdata.l2addr[i].port)&&(( usrdata.l2addr[i].port < 6)))){
			  /* syslog_ax_nam_fdb_err("error port %d read from bcm!\n",usrdata.l2addr[i].port);*/
			   continue;
		   }
		   else if((usrdata.l2addr[i].flags & BCM_L2_DISCARD_DST)||(usrdata.l2addr[i].flags & BCM_L2_DISCARD_SRC)){
			   /*syslog_ax_nam_fdb_dbg("Blacklist do not show!\n");*/
			   continue;
		   }
		   else if(unit != usrdata.l2addr[i].modid){/*do not show the remote fdb item*/
			  /* syslog_ax_nam_fdb_dbg("The item (modid %d)is on remote asic %d",usrdata.l2addr[i].modid,unit);*/
			   continue;
		   }
		   else if(memcmp(macAddr,usrdata.l2addr[i].mac,6)){
               continue;		   
		   }
		   fdb[j].vlanid =usrdata.l2addr[i].vid;
		   fdb[j].inter_type = (usrdata.l2addr[i].flags & BCM_L2_TRUNK_MEMBER)? 1:0;
		   if(!(fdb[j].inter_type)){/*port type*/
			  if(usrdata.l2addr[i].port == 0){/*cpu port*/
				fdb[j].value = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX( \
							CPU_PORT_VIRTUAL_SLOT,CPU_PORT_VIRTUAL_PORT);/*tmp value for cpu identify*/
			  }
			  else{
				 /* npd_syslog_dbg("modid %d,port %d\n",usrdata.l2addr[i].modid,usrdata.l2addr[i].port);*/
				  npd_get_global_index_by_devport(usrdata.l2addr[i].modid,usrdata.l2addr[i].port,&eth_g_index);
				  fdb[j].value = eth_g_index;
			  }
		   }
		   else{
			  fdb[j].value = usrdata.l2addr[i].tgid;
		   }
		   memcpy(fdb[j].ether_mac,usrdata.l2addr[i].mac,6);
		   fdb[j].type_flag = (usrdata.l2addr[i].flags & BCM_L2_STATIC)? 1 : 0;
		   if(j<size)
			   j++;
		   else
			   return j;
		}
		memset(&usrdata,0,sizeof(user_data));
	}
	return j;
#endif
}



/*****************************************
*	
*	nam_static_fdb_entry_mac_set_for_l3
*
*	set static fdb entry for l3 interface
*
*	INPUT:
*		macAddr
*		vlanId
*		globle_index
*
*	RETURN
*
*		FDB_CONFIG_FAIL
*		FDB_CONFIG_SUCCESS
*	
******************************************/
int nam_static_fdb_entry_mac_set_for_l3
(
	/*unsigned char devNum,*/
	unsigned char macAddr[],
	unsigned short vlanId,
	unsigned int globle_index
) 
{
#ifdef DRV_LIB_BCM
	int ret = 0,i = 0,num_of_asic = 0,rv = 0;
	unsigned int modid = 0;
	unsigned short vid = 0;
	unsigned char unit = 0,port = 0;
	bcm_l2_addr_t l2addr,tmp;
	eth_mac_t mac_addr;
#endif

#ifdef DRV_LIB_CPSS
  	int	ret;
	unsigned char devNum = 0;
	unsigned char portNum = 0;
#endif

#ifdef DRV_LIB_CPSS	
	if(CPU_PORT_VINDEX == globle_index){
		devNum = CPU_PORT_ASIC_DEVNO;
		devNum = g_dis_dev[0];
		portNum = CPU_PORT_ASIC_PORTNO;
		syslog_ax_nam_fdb_dbg("devNum = %d,portNum = %d\n",devNum,portNum);
		ret= cpssDxChBrgStaticFdbMacEntrySet(CPU_PORT_GENERAL_ASIC_DEVNO, macAddr,vlanId, devNum,portNum,NPD_TRUE);
	}
	else if( HSC_PORT_VINDEX_AX7 == globle_index) {
		devNum = HSC_PORT_UPLINK_DEVNO_AX7;
		portNum = HSC_PORT_UPLINK_PORTNO_AX7;
		ret= cpssDxChBrgStaticFdbMacEntrySet(CPU_PORT_GENERAL_ASIC_DEVNO, macAddr,vlanId, devNum,portNum,NPD_FALSE);
	}
	else if( HSC_PORT_VINDEX_AX5 == globle_index) {
		devNum = HSC_PORT_UPLINK_DEVNO_AX5;
		portNum = HSC_PORT_UPLINK_PORTNO_AX5;
		ret= cpssDxChBrgStaticFdbMacEntrySet(CPU_PORT_GENERAL_ASIC_DEVNO, macAddr,vlanId, devNum,portNum,NPD_FALSE);
	}
	else if( HSC_PORT_VINDEX_AX5I == globle_index) {
		devNum = HSC_PORT_UPLINK_DEVNO_AX5I;
		portNum = HSC_PORT_UPLINK_PORTNO_AX5I;
		ret= cpssDxChBrgStaticFdbMacEntrySet(CPU_PORT_GENERAL_ASIC_DEVNO, macAddr,vlanId, devNum,portNum,NPD_FALSE);
	}
	else{
		ret = npd_get_devport_by_global_index(globle_index,&devNum, &portNum);
		if(NAM_OK != ret){
			syslog_ax_nam_fdb_err("eth_index %#x get asic port failed for ret %d\n",globle_index,ret);
			return FDB_CONFIG_FAIL;
		}
		ret= cpssDxChBrgStaticFdbMacEntrySet(devNum, macAddr,vlanId, devNum,portNum,NPD_TRUE);
	}
	
	if(0 != ret) {
		nam_syslog_err("add static fdb for l3 error %d\n", ret);
		return FDB_CONFIG_FAIL;
	}
	return FDB_CONFIG_SUCCESS;
#endif

#ifdef DRV_LIB_BCM
    memset(&mac_addr,0,sizeof(eth_mac_t));
	memset(&tmp,0,sizeof(bcm_l2_addr_t));
	memset(&l2addr,0,sizeof(bcm_l2_addr_t));
	l2addr.vid = vlanId;
	memcpy(l2addr.mac,macAddr,6);
	num_of_asic = nam_asic_get_instance_num();
	for(i = 0;i<num_of_asic;i++){
		if(0 == globle_index){
			syslog_ax_nam_fdb_dbg("set static fdb mac %02x:%02x:%02x:%02x:%02x:%02x vid %d\n",
							l2addr.mac[0],l2addr.mac[1],l2addr.mac[2],
							l2addr.mac[3],l2addr.mac[4],l2addr.mac[5],vlanId);
	        l2addr.flags |= (BCM_L2_STATIC | BCM_L2_LOCAL_CPU | BCM_L2_L3LOOKUP);
			l2addr.tgid = -1;
			l2addr.modid = i;
			l2addr.port = 0; /* port 0 as CPU*/
	        ret =  bcm_l2_addr_add(i,&l2addr);
		    if(NAM_E_NONE != ret){
               syslog_ax_nam_fdb_dbg("set l3 fdb failed ret %d ,unit %d\n",ret,i);
               return ret;
			}
		}
		else if( 0xffff == globle_index) {
			/*=========================================
			   To cpu port,what the dev and port number should be?
			==========================================*/		
			l2addr.flags |= (BCM_L2_STATIC | BCM_L2_COPY_TO_CPU | BCM_L2_L3LOOKUP);
			/*l2addr.bcm_unit = unit;*/
			 /*=========================================
			      SPI PORT SHOULD BE HERE
			 ===========================================*/;
			bcm_l2_addr_add(i,&l2addr);

		}
		else{
			ret = npd_get_devport_by_global_index(globle_index,&unit, &port);
			if(NAM_OK != ret){
				syslog_ax_nam_fdb_err("eth_index %#x get asic port failed for ret %d\n",globle_index,ret);
				return FDB_CONFIG_FAIL;
			}
			ret =  bcm_stk_modid_get(unit,&modid);
	        l2addr.flags |= BCM_L2_STATIC;
			l2addr.port = port;
			l2addr.modid = modid;
			/*l2addr.unit = unit;*/
	        bcm_l2_addr_add(i,&l2addr);		
		}
		if(0 != ret) {
			syslog_ax_nam_fdb_dbg("add static fdb for l3 unit %d error %d\n",i, ret);
		}
		NAM_RV_REPLACE_OK(ret, rv, NAM_E_UNAVAIL);
	}
	return rv;
#endif
}




/*****************************************
*	
*	nam_static_fdb_entry_mac_set_for_vrrp
*
*	set static fdb entry for l3 interface
*
*	INPUT:
*		vlanId
*		globle_index
*
*	RETURN
*
*		FDB_CONFIG_FAIL
*		FDB_CONFIG_SUCCESS
*	
******************************************/
int nam_static_fdb_entry_mac_set_for_vrrp
(
	unsigned short vlanId
) 

{

    unsigned char macAddr[6] = {0x01,0x00,0x5e,0x00,0x00,0x12};

#ifdef DRV_LIB_BCM
	int ret = 0,i = 0,num_of_asic = 0,rv = 0;
	unsigned int modid = 0;
	unsigned short vid = 0;
	unsigned char unit = 0,port = 0;
	bcm_l2_addr_t l2addr,tmp;
	eth_mac_t mac_addr;
#endif

#ifdef DRV_LIB_CPSS
  	int	ret;
	unsigned char devNum = 0;
	unsigned char portNum = 0;
#endif

#ifdef DRV_LIB_CPSS	
    CPSS_MAC_ENTRY_EXT_STC       macEntryPtr;
	
	nam_syslog_dbg("%02x:%02x:%02x:%02x:%02x:%02x\n",	\
					macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);

    macEntryPtr.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
    osMemCpy(&macEntryPtr.key.key.macVlan.macAddr,macAddr,6);
    macEntryPtr.key.key.macVlan.vlanId = vlanId;
    macEntryPtr.dstInterface.type = CPSS_INTERFACE_VID_E;
	macEntryPtr.dstInterface.vlanId = vlanId;
	#if 0
    macEntryPtr.dstInterface.devPort.devNum = CPU_PORT_ASIC_DEVNO;
	#endif
	macEntryPtr.dstInterface.devPort.devNum = g_dis_dev[0];
    macEntryPtr.dstInterface.devPort.portNum = CPU_PORT_ASIC_PORTNO;		
	NAM_CONVERT_DEV_PORT_MAC(macEntryPtr.dstInterface.devPort.devNum,
							 macEntryPtr.dstInterface.devPort.portNum);	
    macEntryPtr.isStatic = 1;/* static FDB */
    macEntryPtr.daCommand = CPSS_MAC_TABLE_CNTL_E;
    macEntryPtr.saCommand = CPSS_MAC_TABLE_FRWRD_E;
    macEntryPtr.daRoute = 0;
    macEntryPtr.mirrorToRxAnalyzerPortEn = 0;
    macEntryPtr.sourceID = 0;
    macEntryPtr.userDefined = 0;
    macEntryPtr.daQosIndex = 0;
    macEntryPtr.saQosIndex = 0;
    macEntryPtr.daSecurityLevel = 0;
    macEntryPtr.saSecurityLevel = 0;
    macEntryPtr.appSpecificCpuCode = 1;

	ret=cpssDxChBrgFdbMacEntrySet(devNum, &macEntryPtr);
	if(0 != ret) {
		nam_syslog_err("add static fdb for vrrp error %d\n", ret);
		return FDB_CONFIG_FAIL;
	}
	return FDB_CONFIG_SUCCESS;
#endif

#ifdef DRV_LIB_BCM
    memset(&mac_addr,0,sizeof(eth_mac_t));
	memset(&tmp,0,sizeof(bcm_l2_addr_t));
	memset(&l2addr,0,sizeof(bcm_l2_addr_t));
	l2addr.vid = vlanId;
	memcpy(l2addr.mac,macAddr,6);
	num_of_asic = nam_asic_get_instance_num();
	for(i = 0;i<num_of_asic;i++){
		syslog_ax_nam_fdb_dbg("set static fdb mac %02x:%02x:%02x:%02x:%02x:%02x vid %d\n",
						l2addr.mac[0],l2addr.mac[1],l2addr.mac[2],
						l2addr.mac[3],l2addr.mac[4],l2addr.mac[5],vlanId);
        l2addr.flags |= (BCM_L2_STATIC | BCM_L2_LOCAL_CPU | BCM_L2_L3LOOKUP);
		l2addr.tgid = -1;
		l2addr.modid = i;
		l2addr.port = 0; /* port 0 as CPU*/
        ret =  bcm_l2_addr_add(i,&l2addr);
	    if(NAM_E_NONE != ret){
           syslog_ax_nam_fdb_dbg("set l3 fdb failed ret %d ,unit %d\n",ret,i);
           return ret;
		}
		NAM_RV_REPLACE_OK(ret, rv, NAM_E_UNAVAIL);
	}
	return rv;
#endif
}


int nam_static_fdb_entry_mac_del_for_vrrp
(
    unsigned short vlanId
) 
{
   unsigned char macAddr[6] = {0x01,0x00,0x5e,0x00,0x00,0x12};
#ifdef DRV_LIB_BCM
    int rv= 0,ret = 0;
	int num_of_asic = 0,unit = 0,lport = 0;
	bcm_l2_addr_t		l2addr;
    memset(&l2addr,0,sizeof(bcm_l2_addr_t));
#endif

#ifdef DRV_LIB_CPSS
	unsigned char devNum;
	unsigned int hashPtr;
	int ret;
	unsigned int					 validPtr;
    unsigned int					 skipPtr;
    NAM_BOOL					 agedPtr;
    unsigned char					 associatedDevNumPtr;
    CPSS_MAC_ENTRY_EXT_STC       readEntryPtr;
	CPSS_MAC_ENTRY_EXT_KEY_STC		 entryPtr;
	ETHERADDR  *macPtr = NULL;
	devNum = FDB_INIT;
	unsigned int flag = 0;
#endif

#ifdef DRV_LIB_CPSS
	entryPtr.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
	macPtr = &(entryPtr.key.macVlan.macAddr);
    memcpy(macPtr->arEther,macAddr,6);
	
	entryPtr.key.macVlan.vlanId = vlanId;
	
	ret= cpssDxChBrgFdbStaticDelEnable( devNum,NAM_TRUE);
	if(0 != ret)
	{
		nam_syslog_err("npd_fdb:cpssDxChBrgFdbStaticDelEnable:FAIL.\n");
		nam_syslog_err("err vlaue is: %d\n",ret);
		return FDB_CONFIG_FAIL;
	}
	ret=cpssDxChBrgFdbHashCalc(devNum,&entryPtr, &hashPtr);
	if( ret!=0){
		nam_syslog_err("cpssDxChBrgFdbHashCalc erro is :%d",ret);
		return FDB_CONFIG_FAIL;
		}
	ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
                                         &skipPtr,&agedPtr,&associatedDevNumPtr,&readEntryPtr);
	
	if( ret!=0){
		nam_syslog_err("cpssDxChBrgFdbMacEntryRead erro is :%d",ret);
		return FDB_CONFIG_FAIL;
		}

	/* If find error, repeat the lookup progress max to  4*/
	while(((readEntryPtr.isStatic == 0)||(readEntryPtr.key.key.macVlan.vlanId != vlanId)||
               (memcmp(readEntryPtr.key.key.macVlan.macAddr.arEther,macAddr,6)))&&(flag < 3)){
		nam_syslog_err("This index %d has been used by another static entry and find another index !\n",hashPtr);
		hashPtr++;
		flag++;
		ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
												 &skipPtr,&agedPtr,&associatedDevNumPtr,&readEntryPtr); 
	}
	if( !(flag < 3)){
        nam_syslog_err("nam_static_fdb_entry_mac_del_for_vrrp find index for the static fdb set error!\n");
		return FDB_CONFIG_FAIL;
	}

	readEntryPtr.isStatic = 0;
	ret = cpssDxChBrgFdbMacEntryWrite(devNum,hashPtr,1,&readEntryPtr);
	if( ret!=0){
		nam_syslog_err("cpssDxChBrgFdbMacEntrywrite erro is :%d",ret);
		return FDB_CONFIG_FAIL;
	}
	
	return FDB_CONFIG_SUCCESS;
#endif

#ifdef DRV_LIB_BCM
	num_of_asic = nam_asic_get_instance_num();
	/*=====================================
	get vir port by globle index ,modified code here!
	npd_get_devport_by_global_index(globle_index,&unit,&lport);
	======================================*/
	for(unit = 0;unit < num_of_asic;unit++){
		ret = bcm_l2_addr_get(unit,macAddr,vlanId,&l2addr);
		if(NAM_OK != ret){
	       syslog_ax_nam_fdb_err("no static mac %02x:%02x:%02x:%02x:%02x:%02x vlanid %d unit %d not found in hw!\n", \
		   	           macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5], vlanId,unit);
		}
		else{/*there is already entry here,update it*/
		   ret = bcm_l2_addr_delete(unit,macAddr,vlanId);
		   if(NAM_OK != ret){
	        syslog_ax_nam_fdb_err("no static mac %02x:%02x:%02x:%02x:%02x:%02x vlanid %d unit %d failed in hw!\n", \
		   	           macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5], vlanId,unit);  
		   }
		}
	   NAM_RV_REPLACE_OK(ret, rv, NAM_E_UNAVAIL);
	   nam_dump_l2_addr("Static:",&l2addr);
	}
	return rv;
#endif
}

int nam_fdb_limit_set
(
    unsigned char     unit,
    nam_learn_limit_t limit
)
{
    int ret = 0,rv = 0;
#ifdef DRV_LIB_BCM	
	nam_learn_limit_t glimit;

	if(limit.flags & NAM_L2_LEARN_LIMIT_VLAN){
       return NAM_E_UNAVAIL;
	}
    ret = bcm_l2_learn_limit_set(unit,&limit);
	if(NAM_OK != ret){
        syslog_ax_nam_fdb_err("set fdb limit %d failed,ret %d\n",limit.limit,ret);
        return ret;
	}
	else{
		memset(&glimit,0,sizeof(nam_learn_limit_t));
		glimit.flags = limit.flags;
		glimit.port = limit.port;
		glimit.vlan = limit.vlan;
		glimit.trunk = limit.trunk;
        ret = bcm_l2_learn_limit_get(unit,&glimit);
		if(0 != ret){
          syslog_ax_nam_fdb_err("get fdb limit failed unit %d, ret %d\n",unit,ret);
          return ret;
		}
		else{
          syslog_ax_nam_fdb_dbg("get fdb limit unit %d,type %d,value %d\n",unit,glimit.flags,glimit.limit);
		}
	}
	NAM_RV_REPLACE_OK(ret, rv, NAM_E_UNAVAIL);
#endif
    return rv;
}

unsigned int nam_fdb_port_learn_status_set
(
    unsigned char devNum,
    unsigned char port,
    unsigned int status
)
{

   unsigned int ret = 0;
#ifdef DRV_LIB_CPSS
   CPSS_PORT_LOCK_CMD_ENT  cmd;
   unsigned int statusget = 0;
   
   nam_syslog_dbg("nam_fdb_port_learn_status_set:devNum is :%d,portNum is %d status %#x\n",devNum,port,status);

   ret = cpssDxChBrgFdbPortLearnStatusSet( devNum, port, status, CPSS_LOCK_FRWRD_E );
   if(0 != ret){
      nam_syslog_dbg("nam_fdb_port_learn_status_set:err in cpssDxChBrgFdbPortLearnStatusSet for %d\n",ret);
      return ret;
   }

   ret = cpssDxChBrgFdbPortLearnStatusGet(devNum, port,&statusget,&cmd);
   if(0 != ret){
      nam_syslog_dbg("nam_fdb_port_learn_status_set:err in cpssDxChBrgFdbPortLearnStatusGet for ret %d\n",ret);
      return ret;
   }

   else if(status != statusget){
   	  nam_syslog_dbg("nam_fdb_port_learn_status_set:err in cpssDxChBrgFdbPortLearnStatusGet for status %d\n",statusget);
      return status;
   }

   else if ( CPSS_LOCK_FRWRD_E != cmd){
   	 nam_syslog_dbg("nam_fdb_port_learn_status_set:err in cpssDxChBrgFdbPortLearnStatusGet for cmd %d\n",cmd);
     return cmd;
   }
#endif
   return ret;
}

unsigned int nam_fdb_na_msg_per_port_set
(
    unsigned char devNum,
    unsigned char port,
    unsigned int status
)
{
    unsigned int ret;
	unsigned int statusget;
    /* status == 1 is enable*/
#ifdef DRV_LIB_CPSS 

	ret = cpssDxChBrgFdbNaToCpuPerPortSet(devNum, port,status);
	if(0!= ret){
        nam_syslog_err("nam_fdb_na_msg_per_port_set: cpssDxChBrgFdbNaToCpuPerPortSet: err %d \n",ret);
		return ret;
	}
	ret =  cpssDxChBrgFdbNaToCpuPerPortGet(devNum,port,&statusget);
	if(0 != ret){
      nam_syslog_dbg("nam_fdb_na_msg_per_port_set:err in cpssDxChBrgFdbNaToCpuPerPortGet for ret %d\n",ret);
      return ret;
   }

   else if(status != statusget){
   	  nam_syslog_dbg("nam_fdb_na_msg_per_port_set:err in cpssDxChBrgFdbNaToCpuPerPortGet for status %d\n",statusget);
      return FDB_CONFIG_FAIL;
   }
#endif
   #if 0
   ret = cpssDxChBrgFdbNaStormPreventSet(devNum, port,status);
   if(0!= ret){
        nam_syslog_err("nam_fdb_na_msg_per_port_set: cpssDxChBrgFdbNaStormPreventSet: err %d \n",ret);
		return ret;
	}

   ret = cpssDxChBrgFdbNaStormPreventGet(devNum, port,&statusget);
   if(0 != ret){
		 nam_syslog_dbg("nam_fdb_na_msg_per_port_set:err in cpssDxChBrgFdbNaStormPreventGet for ret %d\n",ret);
		 return ret;
   }
   
   else if(status != statusget){
		 nam_syslog_dbg("nam_fdb_na_msg_per_port_set:err in cpssDxChBrgFdbNaStormPreventGet for status %d\n",statusget);
		 return status;
   }
   #endif
   
   return FDB_CONFIG_SUCCESS;
}

unsigned int nam_fdb_na_msg_vlan_set
(
   unsigned short vlanId,
   unsigned int status
)
{
   unsigned int ret = 0;
   /* status == 1 is enable */
#ifdef DRV_LIB_CPSS   
   ret = cpssDxChBrgVlanNaToCpuEnable(CPU_PORT_GENERAL_ASIC_DEVNO,vlanId,status);
   if( 0 != ret){
      nam_syslog_err("nam_fdb_na_msg_vlan_set: cpssDxChBrgFdbNaToCpuPerPortSet: err %d \n",ret);
	  return ret;
   }
#endif
   /*nam_syslog_dbg("Set vlan na msg to cpu status: %d success\n",status);*/
   return ret;
}

int nam_fdb_au_rate_set
(
    unsigned char  devNum,
    unsigned int   rate,
    unsigned int enable
)

{	
    unsigned int ret = 0;
#ifdef DRV_LIB_CPSS	
	 if(NPD_TRUE == enable){/*the au rate limit has not set*/
		     ret = cpssDxChBrgFdbAuMsgRateLimitSet(devNum,500,NAM_TRUE);                                
	 }
	 else{
		 ret = cpssDxChBrgFdbAuMsgRateLimitSet(devNum,0,enable);
	 }
#endif
	 return ret;
}

int  nam_fdb_hash_index
(
    ETHERADDR               *addrPtr,
    unsigned short          vid,
    unsigned int           *hashPtr
)

{
#ifdef DRV_LIB_CPSS

    unsigned int   hash_bit[12];
    unsigned int  D[3];
    unsigned int  lowWordMacAddress = GT_HW_MAC_LOW32(addrPtr);/* low 32 bits of mac
                                                         address */
    unsigned int  hiHalfWordMacAddress = GT_HW_MAC_HIGH16(addrPtr);/* hi 16 bits of mac
                                                         address*/
    unsigned int  ii;/* iterator */

    D[0] = (lowWordMacAddress    << 12) | vid;
    D[1] = (hiHalfWordMacAddress << 12) | (lowWordMacAddress >> 20);
    D[2] = 0;

   hash_bit[0] = /*
                 getbit(D[2],11) ^ getbit(D[2],8)  ^ getbit(D[2],6)  ^
                 getbit(D[2],5)  ^ getbit(D[2],2)  ^ getbit(D[2],0)  ^ */
                 getbit(D[1],31) ^ getbit(D[1],29) ^ getbit(D[1],28) ^
                 getbit(D[1],27) ^ getbit(D[1],26) ^ getbit(D[1],22) ^
                 getbit(D[1],20) ^ getbit(D[1],19) ^ getbit(D[1],18) ^
                 getbit(D[1],17) ^ getbit(D[1],16) ^ getbit(D[1],15) ^
                 getbit(D[1],12) ^ getbit(D[1],3)  ^ getbit(D[1],2)  ^
                 getbit(D[1],1)  ^ getbit(D[0],30) ^ getbit(D[0],29) ^
                 getbit(D[0],26) ^ getbit(D[0],25) ^ getbit(D[0],24) ^
                 getbit(D[0],23) ^ getbit(D[0],22) ^ getbit(D[0],17) ^
                 getbit(D[0],16) ^ getbit(D[0],15) ^ getbit(D[0],14) ^
                 getbit(D[0],13) ^ getbit(D[0],12) ^ getbit(D[0],11) ^
                 getbit(D[0],8)  ^ getbit(D[0],7)  ^ getbit(D[0],6)  ^
                 getbit(D[0],5)  ^ getbit(D[0],4)  ^ getbit(D[0],3)  ^
                 getbit(D[0],2)  ^ getbit(D[0],1)  ^ getbit(D[0],0)  ;

   hash_bit[1] = /*
                 getbit(D[2],11) ^ getbit(D[2],9)  ^ getbit(D[2],8)  ^
                 getbit(D[2],7)  ^ getbit(D[2],5)  ^ getbit(D[2],3)  ^
                 getbit(D[2],2)  ^ getbit(D[2],1)^*/ getbit(D[1],31) ^
                 getbit(D[1],30) ^ getbit(D[1],26) ^ getbit(D[1],23) ^
                 getbit(D[1],22) ^ getbit(D[1],21) ^ getbit(D[1],15) ^
                 getbit(D[1],13) ^ getbit(D[1],12) ^ getbit(D[1],4)  ^
                 getbit(D[1],1)  ^ getbit(D[0],31) ^ getbit(D[0],29) ^
                 getbit(D[0],27) ^ getbit(D[0],22) ^ getbit(D[0],18) ^
                 getbit(D[0],11) ^ getbit(D[0],9)  ^ getbit(D[0],0)  ;

   hash_bit[2] = /*
                 getbit(D[2],11) ^ getbit(D[2],10) ^ getbit(D[2],9)  ^
                 getbit(D[2],5)  ^ getbit(D[2],4)  ^ getbit(D[2],3)  ^*/
                 getbit(D[1],29) ^ getbit(D[1],28) ^ getbit(D[1],26) ^
                 getbit(D[1],24) ^ getbit(D[1],23) ^ getbit(D[1],20) ^
                 getbit(D[1],19) ^ getbit(D[1],18) ^ getbit(D[1],17) ^
                 getbit(D[1],15) ^ getbit(D[1],14) ^ getbit(D[1],13) ^
                 getbit(D[1],12) ^ getbit(D[1],5)  ^ getbit(D[1],3)  ^
                 getbit(D[1],1)  ^ getbit(D[1],0)  ^ getbit(D[0],29) ^
                 getbit(D[0],28) ^ getbit(D[0],26) ^ getbit(D[0],25) ^
                 getbit(D[0],24) ^ getbit(D[0],22) ^ getbit(D[0],19) ^
                 getbit(D[0],17) ^ getbit(D[0],16) ^ getbit(D[0],15) ^
                 getbit(D[0],14) ^ getbit(D[0],13) ^ getbit(D[0],11) ^
                 getbit(D[0],10) ^ getbit(D[0],8)  ^ getbit(D[0],7)  ^
                 getbit(D[0],6)  ^ getbit(D[0],5)  ^ getbit(D[0],4)  ^
                 getbit(D[0],3)  ^ getbit(D[0],2)  ^ getbit(D[0],0)  ;

   hash_bit[3] = /*
                 getbit(D[2],10) ^ getbit(D[2],8) ^ getbit(D[2],4)   ^
                 getbit(D[2],2)  ^ getbit(D[2],0)^*/ getbit(D[1],31) ^
                 getbit(D[1],30) ^ getbit(D[1],28) ^ getbit(D[1],26) ^
                 getbit(D[1],25) ^ getbit(D[1],24) ^ getbit(D[1],22) ^
                 getbit(D[1],21) ^ getbit(D[1],17) ^ getbit(D[1],14) ^
                 getbit(D[1],13) ^ getbit(D[1],12) ^ getbit(D[1],6)  ^
                 getbit(D[1],4)  ^ getbit(D[1],3)  ^ getbit(D[0],27) ^
                 getbit(D[0],24) ^ getbit(D[0],22) ^ getbit(D[0],20) ^
                 getbit(D[0],18) ^ getbit(D[0],13) ^ getbit(D[0],9)  ^
                 getbit(D[0],2)  ^ getbit(D[0],0)  ^
                 1;

   hash_bit[4] = /*
                 getbit(D[2],11) ^ getbit(D[2],9)  ^ getbit(D[2],5)  ^
                 getbit(D[2],3)  ^ getbit(D[2],1)  ^ getbit(D[2],0)  ^*/
                 getbit(D[1],31) ^ getbit(D[1],29) ^ getbit(D[1],27) ^
                 getbit(D[1],26) ^ getbit(D[1],25) ^ getbit(D[1],23) ^
                 getbit(D[1],22) ^ getbit(D[1],18) ^ getbit(D[1],15) ^
                 getbit(D[1],14) ^ getbit(D[1],13) ^ getbit(D[1],7)  ^
                 getbit(D[1],5)  ^ getbit(D[1],4)  ^ getbit(D[0],28) ^
                 getbit(D[0],25) ^ getbit(D[0],23) ^ getbit(D[0],21) ^
                 getbit(D[0],19) ^ getbit(D[0],14) ^ getbit(D[0],10) ^
                 getbit(D[0],3)  ^ getbit(D[0],1)  ;

   hash_bit[5] = /*
                 getbit(D[2],10) ^ getbit(D[2],6)  ^ getbit(D[2],4)  ^
                 getbit(D[2],2)  ^ getbit(D[2],1)  ^ getbit(D[2],0)  ^*/
                 getbit(D[1],30) ^ getbit(D[1],28) ^ getbit(D[1],27) ^
                 getbit(D[1],26) ^ getbit(D[1],24) ^ getbit(D[1],23) ^
                 getbit(D[1],19) ^ getbit(D[1],16) ^ getbit(D[1],15) ^
                 getbit(D[1],14) ^ getbit(D[1],8)  ^ getbit(D[1],6)  ^
                 getbit(D[1],5)  ^ getbit(D[0],29) ^ getbit(D[0],26) ^
                 getbit(D[0],24) ^ getbit(D[0],22) ^ getbit(D[0],20) ^
                 getbit(D[0],15) ^ getbit(D[0],11) ^ getbit(D[0],4)  ^
                 getbit(D[0],2)  ;

   hash_bit[6] = /*
                 getbit(D[2],11) ^ getbit(D[2],7)  ^ getbit(D[2],5)  ^
                 getbit(D[2],3)  ^ getbit(D[2],2)  ^ getbit(D[2],1)  ^*/
                 getbit(D[1],31) ^ getbit(D[1],29) ^ getbit(D[1],28) ^
                 getbit(D[1],27) ^ getbit(D[1],25) ^ getbit(D[1],24) ^
                 getbit(D[1],20) ^ getbit(D[1],17) ^ getbit(D[1],16) ^
                 getbit(D[1],15) ^ getbit(D[1],9)  ^ getbit(D[1],7)  ^
                 getbit(D[1],6)  ^ getbit(D[0],30) ^ getbit(D[0],27) ^
                 getbit(D[0],25) ^ getbit(D[0],23) ^ getbit(D[0],21) ^
                 getbit(D[0],16) ^ getbit(D[0],12) ^ getbit(D[0],5)  ^
                 getbit(D[0],3)  ;

   hash_bit[7] = /*
                 getbit(D[2],8)  ^ getbit(D[2],6)  ^ getbit(D[2],4)  ^
                 getbit(D[2],3)  ^ getbit(D[2],2)  ^ getbit(D[2],0)  ^*/
                 getbit(D[1],30) ^ getbit(D[1],29) ^ getbit(D[1],28) ^
                 getbit(D[1],26) ^ getbit(D[1],25) ^ getbit(D[1],21) ^
                 getbit(D[1],18) ^ getbit(D[1],17) ^ getbit(D[1],16) ^
                 getbit(D[1],10) ^ getbit(D[1],8)  ^ getbit(D[1],7)  ^
                 getbit(D[0],31) ^ getbit(D[0],28) ^ getbit(D[0],26) ^
                 getbit(D[0],24) ^ getbit(D[0],22) ^ getbit(D[0],17) ^
                 getbit(D[0],13) ^ getbit(D[0],6)  ^ getbit(D[0],4)  ;

   hash_bit[8] = /*
                 getbit(D[2],9)  ^ getbit(D[2],7)  ^ getbit(D[2],5)  ^
                 getbit(D[2],4)  ^ getbit(D[2],3)  ^ getbit(D[2],1)  ^*/
                 getbit(D[1],31) ^ getbit(D[1],30) ^ getbit(D[1],29) ^
                 getbit(D[1],27) ^ getbit(D[1],26) ^ getbit(D[1],22) ^
                 getbit(D[1],19) ^ getbit(D[1],18) ^ getbit(D[1],17) ^
                 getbit(D[1],11) ^ getbit(D[1],9)  ^ getbit(D[1],8)  ^
                 getbit(D[1],0)  ^ getbit(D[0],29) ^ getbit(D[0],27) ^
                 getbit(D[0],25) ^ getbit(D[0],23) ^ getbit(D[0],18) ^
                 getbit(D[0],14) ^ getbit(D[0],7)  ^ getbit(D[0],5)  ;

   hash_bit[9] = /*
                 getbit(D[2],10) ^ getbit(D[2],8)  ^ getbit(D[2],6)  ^
                 getbit(D[2],5)  ^ getbit(D[2],4)  ^ getbit(D[2],2)  ^
                 getbit(D[2],0)^*/ getbit(D[1],31) ^ getbit(D[1],30) ^
                 getbit(D[1],28) ^ getbit(D[1],27) ^ getbit(D[1],23) ^
                 getbit(D[1],20) ^ getbit(D[1],19) ^ getbit(D[1],18) ^
                 getbit(D[1],12) ^ getbit(D[1],10) ^ getbit(D[1],9)  ^
                 getbit(D[1],1)  ^ getbit(D[0],30) ^ getbit(D[0],28) ^
                 getbit(D[0],26) ^ getbit(D[0],24) ^ getbit(D[0],19) ^
                 getbit(D[0],15) ^ getbit(D[0],8)  ^ getbit(D[0],6)  ^
                 1  ;

   hash_bit[10] = /*
                  getbit(D[2],11) ^ getbit(D[2],9)  ^ getbit(D[2],7)  ^
                  getbit(D[2],6)  ^ getbit(D[2],5)  ^ getbit(D[2],3)  ^
                  getbit(D[2],1)  ^ getbit(D[2],0 ^*/ getbit(D[1],31) ^
                  getbit(D[1],29) ^ getbit(D[1],28) ^ getbit(D[1],24) ^
                  getbit(D[1],21) ^ getbit(D[1],20) ^ getbit(D[1],19) ^
                  getbit(D[1],13) ^ getbit(D[1],11) ^ getbit(D[1],10) ^
                  getbit(D[1],2)  ^ getbit(D[0],31) ^ getbit(D[0],29) ^
                  getbit(D[0],27) ^ getbit(D[0],25) ^ getbit(D[0],20) ^
                  getbit(D[0],16) ^ getbit(D[0],9)  ^ getbit(D[0],7)  ;

        hash_bit[11] = /*
                       getbit(D[2],11) ^ getbit(D[2],10) ^ getbit(D[2],7)  ^
                       getbit(D[2],5)  ^ getbit(D[2],4)  ^ getbit(D[2],1)  ^*/
                   getbit(D[1],31) ^ getbit(D[1],30) ^ getbit(D[1],28) ^
                   getbit(D[1],27) ^ getbit(D[1],26) ^ getbit(D[1],25) ^
                   getbit(D[1],21) ^ getbit(D[1],19) ^ getbit(D[1],18) ^
                   getbit(D[1],17) ^ getbit(D[1],16) ^ getbit(D[1],15) ^
                   getbit(D[1],14) ^ getbit(D[1],11) ^ getbit(D[1],2)  ^
                   getbit(D[1],1)  ^ getbit(D[1],0)  ^ getbit(D[0],29) ^
                   getbit(D[0],28) ^ getbit(D[0],25) ^ getbit(D[0],24) ^
                   getbit(D[0],23) ^ getbit(D[0],22) ^ getbit(D[0],21) ^
                   getbit(D[0],16) ^ getbit(D[0],15) ^ getbit(D[0],14) ^
                   getbit(D[0],13) ^ getbit(D[0],12) ^ getbit(D[0],11) ^
                   getbit(D[0],10) ^ getbit(D[0],7)  ^ getbit(D[0],6)  ^
                   getbit(D[0],5)  ^ getbit(D[0],4)  ^ getbit(D[0],3)  ^
                   getbit(D[0],2)  ^ getbit(D[0],1)  ^ getbit(D[0],0)  ;

    (*hashPtr) = 0;
    for(ii = 0 ; ii < 12; ii++)
    {
        (*hashPtr) |= (hash_bit[ii] & 0x01) << ii;
    }
	
#endif
    return NAM_OK;
}
#ifdef __cplusplus
}
#endif

