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
* nam_stp.c
*
*
* CREATOR:
*		chenb@autelan.com
*
* DESCRIPTION:
*		APIs used in NAM for ASIC driver.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.9 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "sysdef/npd_sysdef.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "nam_utilus.h"
#include "nam_asic.h"
#include "nam_log.h"
#include "nam_stp.h"

/*The 3 Funs below is used for STP Config */
int nam_asic_stp_init(void) {

	return 0;	
}
int 
nam_stg_check(
int unit ,
int stg
)
{
    int i = 0,rv = 0,count = 0;
	int *	list;
	
#ifdef DRV_LIB_BCM
	rv = bcm_stg_list(unit,&list,&count);
	if(NAM_E_NONE != rv)
		return rv;
	for(i = 0;i < count; i++){
      if(stg == list[i]){
         return NAM_E_NONE;
	  }
	}
	if(i >= count)
		return NAM_E_NOT_FOUND;
#endif
    return NAM_E_NONE;
}

/*config spanning_tree port enable|disable*/
int nam_asic_stp_port_enable
( 
    unsigned char devNum, 
    unsigned int portId,
    unsigned char stp_enable
)
{	
	unsigned short	vlanId ;
	unsigned int	stpState;
	int	ret = 0;
	
#ifdef DRV_LIB_BCM
	unsigned short	stpId = 1;/*in broadcom asic,stg 1 is default*/
	syslog_ax_nam_asic_dbg("STP PORT SET ::devNum %d portVid %d STGIndx %d.",devNum,vlanId,stpId);
	switch(stp_enable) {
		case 1:
			stpState = NAM_STG_STP_BLOCK;/*block  state*/			
			break;			
		case 0:
			stpState = NAM_STG_STP_FORWARD;/*default value & in broadcom,only forward state no drop non-bpdu packets*/
			break;
		default:
			break;
		}
	ret = bcm_stg_stp_set(devNum,stpId,portId,stpState);
	if(ret != 0) {
		syslog_ax_nam_asic_dbg("Nam_asic>>nam_asic_stp_port_enable::Brg STP State Set Error.");
		return -1;
	}	
#endif	

#ifdef DRV_LIB_CPSS
	unsigned short	stpId = 0;
	unsigned int	hwdata = 0x0;
	switch(stp_enable) {
		case 1:
			stpState = CPSS_RSTP_PORT_STATE_DISCARD_E;/*block listen state*/
			syslog_ax_nam_asic_dbg("##### portId = %d,stpId = %d,stpState = %d\n",portId,stpId,stpState);
			ret = cpssDxChBrgStpStateSet(devNum,(unsigned char)portId,stpId,stpState); 
			/*stpState can be 0(disable),1(block)*/

			if(ret != 0) {
				syslog_ax_nam_asic_err("Nam_asic>>nam_asic_stp_port_enable::Brg STP State Set Error.");
				return -1;
			}	
			syslog_ax_nam_asic_dbg("Nam_asic>>nam_asic_stp_port_enable::portVid Get Ok.");
			syslog_ax_nam_asic_dbg("Nam_asic>>nam_asic_stp_port_enable::devNum %d portId %d STGIndx %d stpState %d.",devNum,portId,stpId,stpState);
			/*disable Reg <egress routed Span FilterEn>*/
			hwdata	= 0x0;
			ret = cpssDxChBrgRoutedSpanEgressFilteringEnable(devNum,hwdata);
			if(ret != 0)
			{
				syslog_ax_nam_asic_err("Nam_asic>>nam_asic_stp_port_enable::Brg Routed Span Egress Filtering Set Error.");
				return -1;
			}

			ret = cpssDxChBrgGenBpduTrapEnableSet(devNum,stp_enable);
			if(ret != 0)
			{
				syslog_ax_nam_asic_err("Nam_asic>>nam_asic_stp_port_enable::Bpdu Trap Enable Set Error.");
				return -1;
			}
			break;
		case 0:
			stpState = CPSS_RSTP_PORT_STATE_DISABLE_E;/*disable state*/
			ret = cpssDxChBrgStpStateSet(devNum,(unsigned char)portId,stpId,stpState); 
			if(ret != 0) {
				syslog_ax_nam_asic_err("Nam_asic>>nam_asic_stp_port_enable::Brg STP State Disable Error.");
			}

			ret = cpssDxChBrgGenBpduTrapEnableSet(devNum,stp_enable);
			if(ret != 0)
			{
				syslog_ax_nam_asic_err("Nam_asic>>nam_asic_stp_port_enable::Bpdu Trap Enable Set Error.");
				return -1;
			}
			break;
		default:
			break;
	}
#endif
	return ret;
}
int  nam_vlan_stpid_get
(
     unsigned char devNum,
     unsigned short vid,
     unsigned int* stg
)
{
     int ret = 0;
	 unsigned int stg_ptr = 0;
	 
#ifdef DRV_LIB_BCM
	 ret = bcm_vlan_stg_get(devNum,vid,&stg_ptr);
     if(0 == ret){
        nam_syslog_dbg("Get unit %d,vlan %d stg %d\n",devNum,vid,stg_ptr);
		stg_ptr = stg_ptr - 1;
	 }
	 else{
        nam_syslog_dbg("Unit %d,vlan %d get stg failed,ret %d\n",devNum,vid ,ret);
        return ret;
	 }
#endif

#ifdef DRV_LIB_CPSS
	ret = cpssDxChBrgVlanStpIdGet(devNum,vid,&stg_ptr);/*when bind,mstid == stpId*/
#endif
	 if(NAM_OK != ret){
        nam_syslog_err("Get vid %d stg failed!\n",vid);
		return FDB_CONFIG_FAIL;
	 }
	 *stg = stg_ptr;
	 return FDB_CONFIG_SUCCESS;
}

void nam_stp_convert_common2platform
(
     int stpstate,
     int* state 
)
{
#ifdef DRV_LIB_CPSS
	switch(stpstate){
	  case NAM_STP_PORT_STATE_DISABLE_E:
		*state = CPSS_RSTP_PORT_STATE_DISABLE_E;
		break;
	  case NAM_STP_PORT_STATE_DISCARD_E:
		*state = CPSS_RSTP_PORT_STATE_DISCARD_E;
		break;
	  case NAM_STP_PORT_STATE_LEARN_E:
		*state = CPSS_RSTP_PORT_STATE_LEARN_E;
		break;
	  case NAM_STP_PORT_STATE_FORWARD_E:
		*state = CPSS_RSTP_PORT_STATE_FORWARD_E;
		break;		
	}
#endif
#ifdef DRV_LIB_BCM
	switch(stpstate){
	  case NAM_STP_PORT_STATE_DISABLE_E: 	
		*state = BCM_STG_STP_FORWARD;
		break;
	  case NAM_STP_PORT_STATE_DISCARD_E:
		*state = BCM_STG_STP_BLOCK;/* !!!Attention:::now no use BCM_STG_STP_LISTEN*/
		break;
	  case NAM_STP_PORT_STATE_LEARN_E:  	
		*state = BCM_STG_STP_LEARN;
		break;
	  case NAM_STP_PORT_STATE_FORWARD_E: 
		*state = BCM_STG_STP_FORWARD;
		break;		
	}	
#endif

}
void nam_stp_convert_state2common
(
    int stpstate,
    int* state
)
{
#ifdef DRV_LIB_CPSS
    switch(stpstate){
      case CPSS_RSTP_PORT_STATE_DISABLE_E:
	  	*state = NAM_STP_PORT_STATE_DISABLE_E;
	    break;
      case CPSS_RSTP_PORT_STATE_DISCARD_E:
	  	*state = NAM_STP_PORT_STATE_DISCARD_E;
	    break;
      case CPSS_RSTP_PORT_STATE_LEARN_E:
	  	*state = NAM_STP_PORT_STATE_LEARN_E;
	    break;
      case CPSS_RSTP_PORT_STATE_FORWARD_E:
	  	*state = NAM_STP_PORT_STATE_FORWARD_E;
	    break;		
	}
#endif
#ifdef DRV_LIB_BCM
switch(stpstate){
  case BCM_STG_STP_DISABLE:
	*state = NAM_STP_PORT_STATE_DISABLE_E;
	break;
  case BCM_STG_STP_BLOCK:
  case BCM_STG_STP_LISTEN:
	*state = NAM_STP_PORT_STATE_DISCARD_E;
	break;
  case BCM_STG_STP_LEARN:
	*state = NAM_STP_PORT_STATE_LEARN_E;
	break;
  case BCM_STG_STP_FORWARD:
	*state = NAM_STP_PORT_STATE_FORWARD_E;
	break;		
}

#endif
}
int nam_stp_state_get
(
    unsigned char devNum,
    unsigned char port,
    unsigned short stg,
    unsigned int* stpState
)
{
   int ret = 0;
   unsigned int stp_state = 0,state = 0;
   
#ifdef DRV_LIB_BCM
    /**************************************************
       *in broadcom, the stg start from 1,so now just the stg,set the value
       *add 1.
       *Since get the stg from npd,which is from the mstid,so set here!!!
       ***************************************************/
   stg = stg + 1;/*stg is mstid + 1;*/
   ret = bcm_stg_stp_get(devNum,stg,port,&stp_state);
   if(NAM_OK != ret){
     syslog_ax_nam_fdb_err("Get stp state stg %d port %d failed!\n",stg,port);
	 return FDB_CONFIG_FAIL;
   }
   nam_stp_convert_state2common(stp_state,&state);
   *stpState = state;
#endif

#ifdef DRV_LIB_CPSS
	unsigned int stpEntryPtr[2] = {0};
	unsigned int wordOffset = 0,bitOffset = 0;

	ret = cpssDxChBrgStpEntryGet(devNum,stg,stpEntryPtr);
	if(0 == ret) {
		 /* calculate port stp state field offset in bits */
	     /* each word in the STP entry hold 16 STP ports */
	    wordOffset = (2 * port) >> 5;   /* / 32 */
	    bitOffset = (2 * port) & 0x1f;  /* % 32 */
		stp_state = (stpEntryPtr[wordOffset]&(3<<bitOffset))>>bitOffset;
        nam_stp_convert_state2common(stp_state,&state);
		*stpState = state;
		npd_syslog_dbg("Get stp port state::port(eth_g_index) %d,stpid %d,wordOffset %d,bitOffset %d,state %d\n", \
			            port,stg,wordOffset,bitOffset,*stpState);
        return NPD_SUCCESS;
	}
	else {
       return NPD_FAIL;
	}	
#endif
   return FDB_CONFIG_SUCCESS;
}
int nam_stp_state_set
(
    unsigned char devNum,
    unsigned char port,
    unsigned short stg,
    unsigned int stpState

)
{
	int ret = 0;
	int state = 0,stp_state = 0;
	nam_stp_convert_common2platform(stpState,&state);
#ifdef DRV_LIB_BCM
    /**************************************************
       *in broadcom, the stg start from 0,so now just the stg,if 0,
       *set to 1.
       *Since get the stg from npd,which is from the mstid,so set here!!!
       ***************************************************/
	stg = stg + 1;/*stg is mstid + 1;*/
    nam_syslog_dbg("SET stg %d,asic port(%d,%d) state %d\n",stg,devNum,port,state);
	ret = bcm_stg_stp_set(devNum,stg,port,state);
	if(NAM_OK != ret){
	  nam_syslog_dbg("Set stp state stg %d port %d stpsate %d failed!\n",stg,port,stpState);
	  return ret;
	}
	else{
       ret = bcm_stg_stp_get(devNum, stg,port,&stp_state);
	   if(0 == ret){
         nam_syslog_dbg("GET stg %d,asic port(%d,%d) state %d\n",stg,devNum,port,stp_state);
	   }
	   else{
         return ret;
	   }
	}
#endif

#ifdef DRV_LIB_CPSS
    ret = cpssDxChBrgStpStateSet(devNum,port,stg,state);
	if( 0 != ret){
	   nam_syslog_err("STP_STATE_UPDATE_E:: Set the stp port state error!\n");
	}
#endif
	return FDB_CONFIG_SUCCESS;

}

int nam_stp_vlan_bind_stg
(
   unsigned char devNum,
   unsigned short vlanid,
   unsigned int stg
)
{
   int ret = 0;
   int unit = 0,num_of_asic = 0;
   
#ifdef DRV_LIB_BCM
    /**************************************************
       *in broadcom, the stg start from 0,so now just the stg,if 0,
       *set to 1.
       *Since get the stg from npd,which is from the mstid,so set here!!!
       ***************************************************/
   stg = stg + 1;/*stg is mstid + 1;  */ 
   num_of_asic = nam_asic_get_instance_num();
   for(unit = 0;unit < num_of_asic;unit++){
	   if(NAM_OK != (ret = nam_stg_check(unit,stg))){
	     syslog_ax_nam_fdb_err("Trying to bind one vlan to none exist stg %d\n",stg);
		 if(NAM_OK != (ret = bcm_stg_create_id(unit,stg))){
	        syslog_ax_nam_fdb_err("Failed to create stg %d for rt %d\n",stg,ret);
			return FDB_CONFIG_FAIL;
		 }
	   }
	   ret = bcm_stg_vlan_add(unit,stg,vlanid);
	   if(NAM_OK != ret){
	      syslog_ax_nam_fdb_err("bind vlan %d to stg %d failed for ret %d\n",vlanid,stg,ret);
		  return FDB_CONFIG_FAIL;
	   }
   }
#endif

#ifdef DRV_LIB_CPSS
  	if(0 != cpssDxChBrgVlanToStpIdBind(devNum, vlanid,(unsigned short)stg)) {
		nam_syslog_err("npd_mstp_vlan_bind_to_stpid::Brg Vlan%d Bind to STPid %d Error\n",vlanid,(unsigned short)stg);
		return FDB_CONFIG_FAIL;
	}
	else{
        nam_syslog_dbg("npd mst vlan %d bind to stg %d\n",vlanid,(unsigned short)stg);
	}
#endif
   return FDB_CONFIG_SUCCESS;
}
#ifdef __cplusplus
}
#endif

