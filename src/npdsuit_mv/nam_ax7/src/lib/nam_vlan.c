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
* nam_vlan.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		APIs used in NAM by VLAN module.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.74 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "npd/nam/npd_amapi.h"
#include "nam_vlan.h"
#include "nam_asic.h"
#include "nam_eth.h"
#include "nam_log.h"

/*#include "cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h"*/
#ifdef DRV_LIB_BCM
#include <bcm/types.h>
#include <bcm/mcast.h>
#include <bcm/vlan.h>
#include <bcm/switch.h>
#include <bcm/l2.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/firebolt.h>
#include <soc/types.h>
#endif
extern enum product_id_e npd_query_product_id
(
	void
);

/**********************************************************************************
 * nam_vlan_convert_port_bitmap
 * 
 * convert vlan port index bitmap to driver port bitmap
 *
 *
 *	INPUT:
 *		from - global eth port index bitmap 
 *		to - driver port bitmap
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NAM_VLAN_ERR_NONE  - if no error occurred
 *		NAM_VLAN_ERR_PARAM - if parameter error(pointers null etc.)
 *		
 **********************************************************************************/
int nam_vlan_convert_port_bitmap
(
	struct vlan_ports_bmp  *from,
	struct DRV_VLAN_PORT_BMP_S 	**to
)
{
	int ret = NAM_VLAN_ERR_NONE;
	
#ifdef DRV_LIB_CPSS	
	unsigned char devNum,portNum;
	unsigned int eth_g_index = 0,i = 0,j = 0;
	unsigned int row = 0,column = 0;

	struct DRV_VLAN_PORT_BMP_S *tmpPtr = NULL;
	
	if((NULL == from)||(NULL == to)||(NULL == *to)) {
		ret = NAM_VLAN_ERR_PARAM;	
	}
	else {
		tmpPtr = *to;
		for(i = 0;i < NPD_VLAN_BITMAP_LENGTH;i++) {
			if(0 == from->bitmaps[i]) continue;/* all bits cleared, no check*/
				
			for(j=0; j<NPD_VLAN_BITMAP_WIDTH; j++) {
				if(!(from->bitmaps[i] & j))  continue;

				else { /* bit set*/
					eth_g_index = i*NPD_VLAN_BITMAP_WIDTH + j;
					npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
					if(!nam_asic_legal_device_number(devNum))	continue;

					/*fill in port bitmap*/
					row 	= portNum / NPD_VLAN_BITMAP_WIDTH;
					column 	= portNum % NPD_VLAN_BITMAP_WIDTH;
					if(row >= NPD_VLAN_DRV_BITMAP_LENGTH) continue; /* bitmap array out of range*/
					tmpPtr[devNum].ports[row] |= (1 << column);

					/*reset temporary variable*/
					devNum = portNum = row = column = 0;
				}
			}/* end port index bitmap column cycle*/
		}/* end port index bitmap row cycle*/
	}
#endif
	return ret;
}

/**********************************************************************************
 * nam_vlan_convert_port_bitmap
 * 
 * convert driver port bitmap to vlan port index bitmap 
 *
 *
 *	INPUT:
 *		from - driver port bitmap
 *		to - global eth port index bitmap 
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NAM_VLAN_ERR_NONE  - if no error occurred
 *		NAM_VLAN_ERR_PARAM - if parameter error(pointers null etc.)
 *		
 **********************************************************************************/
int nam_vlan_convert_port_index_bitmap
(
	unsigned char	devNum,
	struct DRV_VLAN_PORT_BMP_S 	*from,
	struct vlan_ports_bmp  *to
)
{
	int ret = NAM_VLAN_ERR_NONE;
	
#ifdef DRV_LIB_CPSS	
	unsigned int eth_g_index = 0,i = 0,j = 0;
	unsigned char portNum;
	unsigned int row = 0,column = 0;

	if((NULL == from)||(NULL == to)) {
		ret = NAM_VLAN_ERR_PARAM;	
	}
	else if(!nam_asic_legal_device_number(devNum)) {
			ret = NAM_VLAN_ERR_PARAM;
	}
	else {
		for(i = 0;i < NPD_VLAN_DRV_BITMAP_LENGTH; i++) {
			if(0 == from->ports[i]) continue;/* all bits cleared, no check*/
				
			for(j=0; j<NPD_VLAN_BITMAP_WIDTH; j++) {
				if(!(from->ports[i] & j))  continue;

				else { /* bit set*/
					portNum = i*NPD_VLAN_BITMAP_WIDTH + j;
					ret = npd_get_global_index_by_devport(devNum,portNum,&eth_g_index);
                    if(0 != ret){
                       syslog_ax_nam_vlan_err("Failed to get gloable index when convert bitmap!\n");
					   continue;
					}
					/*fill in port bitmap*/
					row 	= eth_g_index / NPD_VLAN_BITMAP_WIDTH;
					column 	= eth_g_index % NPD_VLAN_BITMAP_WIDTH;
					if(row >= NPD_VLAN_BITMAP_LENGTH) continue;/*bitmap array index out of range*/
					to->bitmaps[row] |= (1 << column);

					/*reset temporary variable*/
					portNum = row = column = eth_g_index = 0;
				}
			}/* end port bitmap column cycle*/
		}/* end port bitmap row cycle*/
	}
#endif
	return ret;
}
/**********************************************************************************
 * nam_vlan_port_index_bitmap_op
 * 
 * add port to or del port from vlan port index bitmap
 *
 *
 *	INPUT:
 *		container - global eth port index bitmap 
 *		eth_g_index - global eth port index stands for unique port
 *		action - add or del operation to bitmap
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NAM_VLAN_ERR_NONE  - if no error occurred
 *		NAM_VLAN_ERR_PARAM - if parameter error(pointers null, parameter out of range etc.)
 *		
 **********************************************************************************/
int nam_vlan_port_index_bitmap_op
(
	struct vlan_ports_bmp  	 *container,
	unsigned int 	eth_g_index,
	enum vlan_port_bmp_op_e	 action
)
{
	int ret = NAM_VLAN_ERR_NONE;
	
#ifdef DRV_LIB_CPSS	
	unsigned int row = 0,column = 0;
	
	if(NULL == container) {
		ret = NAM_VLAN_ERR_PARAM;	
	}
	else {
		row 	= eth_g_index / NPD_VLAN_BITMAP_WIDTH;
		column 	= eth_g_index % NPD_VLAN_BITMAP_WIDTH;
		if(row >= NPD_VLAN_BITMAP_LENGTH) 
			ret = NAM_VLAN_ERR_PARAM;
		else {
			if(VLAN_PORT_BMP_ADD == action)
				container->bitmaps[row] |= ( 1<<column );
			if(VLAN_PORT_BMP_DEL == action)
				container->bitmaps[row] &= ~( 1<<column );
		}
	}
#endif	
	return ret;
}

/**********************************************************************************
 * nam_vlan_port_bitmap_add
 * 
 * add port to or del port from driver layer port bitmap
 *
 *
 *	INPUT:
 *		container - driver port bitmap 
 *		portNum - virtual port number stands for unique port
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NAM_VLAN_ERR_NONE  - if no error occurred
 *		NAM_VLAN_ERR_PARAM - if parameter error(pointers null, parameter out of range etc.)
 *		
 **********************************************************************************/
int nam_vlan_port_bitmap_op
(
	struct DRV_VLAN_PORT_BMP_S *container,
	unsigned int portNum,
	enum vlan_port_bmp_op_e	action
)
{
	int ret = NAM_VLAN_ERR_NONE;
	
#ifdef DRV_LIB_CPSS	
	unsigned int row = 0,column = 0;
	
	if(NULL == container) {
		ret = NAM_VLAN_ERR_PARAM;	
	}
	else {
		row 	= portNum / NPD_VLAN_BITMAP_WIDTH;
		column 	= portNum % NPD_VLAN_BITMAP_WIDTH;
		if(row >= NPD_VLAN_DRV_BITMAP_LENGTH) 
			ret = NAM_VLAN_ERR_PARAM;
		else {
			if(VLAN_PORT_BMP_ADD == action)
				container->ports[row] |= ( 1<<column );
			if(VLAN_PORT_BMP_DEL == action)
				container->ports[row] &= ~( 1<<column );
		}
	}
#endif	
	return ret;
}
unsigned int nam_vlan_convert_port_bitmap_global_index
(
	struct DRV_VLAN_PORT_BMP_S *portbmp,
	unsigned int *eth_g_index,
	unsigned int *port_count
)
{
	int ret = NAM_VLAN_ERR_NONE;
	
#ifdef DRV_LIB_CPSS	
	unsigned char devNum = 0;
	unsigned int  i = 0,j = 0,indexCount = 0,portRshift =0;
	unsigned char portNum;

	if(NULL == portbmp) {
		ret = NAM_VLAN_ERR_PARAM;
		return ret;
	}
	else if(!nam_asic_legal_device_number(devNum)) {
			ret = NAM_VLAN_ERR_PARAM;
			return ret;
	}
	else 
	{
		for(i = 0;i < NPD_VLAN_DRV_BITMAP_LENGTH; i++) 
		{
			if(0 != portbmp->ports[i])
			{
				for(j=0; j<NPD_VLAN_BITMAP_WIDTH; j++) 
				{
					portRshift = (portbmp->ports[i]) >> j;
					if(portRshift & 0x1)
					{
						portNum = i*NPD_VLAN_BITMAP_WIDTH +j;
						if(indexCount <= NAM_PP_LOGIC_PORT_NUM_MAX)
						{
							ret = npd_get_global_index_by_devport(devNum,portNum,eth_g_index+indexCount);
                            if(0 != ret){
                               syslog_ax_nam_fdb_dbg("Failed when convert port bitmap to global index!\n");
                               continue;/*the index not add for this convert failed.*/
							}
							indexCount ++;
						}
						else 
						{
							break;
						}
					} 
					else {
							continue;
					}
				}
			}
			else 
			{
				continue;
			}
		}
	}
	*port_count = indexCount;
#endif
	return ret;
}

int clear_vlan_member_bmp
(
	struct DRV_VLAN_PORT_BMP_S *portMbrPtr,
	struct DRV_VLAN_PORT_BMP_S *portTagPtr
)
{
		
#ifdef DRV_LIB_CPSS
	portMbrPtr->ports[0] = 0;
	portMbrPtr->ports[1] = 0;
	portTagPtr->ports[0] = 0;
	portTagPtr->ports[1] = 0;
#endif	
	return 0;
}
/***********************************************************
*Func: nam_asic_vlan_entry_active
*Params: vlan ID
*Call: 	bcm_vlan_create
*Return:
*		VLAN_CONFIG_SUCCESS --vlan entry activate ok.
*		VLAN_CONFIG_FAIL	--vlan entry activate fail.
*Comments:
*		MacAddr Auto-Learn Enable every vlan validate. 
************************************************************/
unsigned int nam_asic_vlan_entry_active
(
	unsigned int product_id,
	unsigned short vlanId
) 
{
	unsigned long ret = 0;
	unsigned long	numOfPp = 0;/*number of asic instances*/
	unsigned char	i = 0;
	int asictype = 0;
	
#ifdef DRV_LIB_CPSS
	unsigned long	autolearn = 1;
	
	numOfPp = nam_asic_get_instance_num();

	for(i=0;i<numOfPp;i++){
		/*vlan entry ValidBit set 1*/
		if(0 != cpssDxChBrgVlanEntryValidate(i,vlanId)){
			ret = VLAN_CONFIG_FAIL;
		}
		/* autolearnDisable set 0 */
		if(0 != cpssDxChBrgVlanLearningStateSet(i,vlanId,autolearn)){
			ret = VLAN_CONFIG_FAIL;
		}
		/* NewSrcAddrIsNotSecurityBreach: yes */
		ret = cpssDxChBrgVlanNASecurEnable(i,vlanId,0);
		if(0 == ret){
			ret = VLAN_CONFIG_SUCCESS;
		}
		else{
			ret = VLAN_CONFIG_FAIL;
		}
		/* NA message to CPU disable*/
		asictype = nam_asic_get_asic_type(i);
		if((asictype < ASIC_TYPE_START_EXMX_E) || (asictype > ASIC_TYPE_LAST_E))
		{
			nam_syslog_err("get asic type fail !\n");
		}
		if(asictype == ASIC_TYPE_DXCH_LION_E)
		{
			ret = cpssDxChBrgVlanNaToCpuEnable(i,vlanId,1);
		}
		else
		{
			ret = cpssDxChBrgVlanNaToCpuEnable(i,vlanId,0);
		}
		if(PRODUCT_ID_AX5K_I == product_id) {
			ret = nam_asic_vlan_entry_ports_add(0,vlanId,24,1);
		}
	}
#endif

#ifdef DRV_LIB_BCM	
	
	bcm_pbmp_t pbmp,ut_pbmp;

	SOC_PBMP_CLEAR(pbmp);
	SOC_PBMP_CLEAR(ut_pbmp);	

	numOfPp = nam_asic_get_instance_num();
	for (i=0; i<numOfPp; i++) {
		/*create a new vlan entry*/
		ret = bcm_vlan_create(i, vlanId);
		if(0 == ret){
			ret = VLAN_CONFIG_SUCCESS;
		}else{
			ret = VLAN_CONFIG_FAIL;
			return ret;			
		}

		/*add port bitmap 4 to pbmp*/  
		/*SOC_PBMP_PORT_ADD(pbmp, 4);*/	
		/*add port bitmap 5 to pbmp*/		
		/*SOC_PBMP_PORT_ADD(pbmp, 5);*/
		
		/*add port bitmap 0 to pbmp*/		
		SOC_PBMP_PORT_ADD(pbmp, 0);
		
		/* portNum 4,5 add to all vlan created as tagged-
		 *  	-because of these two port connection to the two chips;
		 * add cpu port to all vlan created as tagged-
		 *      -because of need packet go through CPU*/		
		ret = bcm_vlan_port_add(i,vlanId,pbmp,ut_pbmp);
		if(0 == ret){
			ret = VLAN_CONFIG_SUCCESS;
		}else{
			ret = VLAN_CONFIG_FAIL;
			return ret;
		}		
	}
#endif	
	return ret;	
}
/**************************************
*Func: nam_asic_set_port_vlan_ingresflt
*Params:
*		devNum--0,1
*		portNum--[0,23]
*		enDis--vlan ingress fliter state
*Call: 
*		cpssDxChBrgVlanPortIngFltEnable
*Return:
*		VLAN_CONFIG_SUCCESS
*		VLAN_CONFIG_FAIL
***************************************/
unsigned int nam_asic_set_port_vlan_ingresflt
(	
	unsigned char devNum,
	unsigned char portNum,
	unsigned char enDis
)
{
	unsigned long ret = 0;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChBrgVlanPortIngFltEnable(devNum, portNum, enDis);
	if(0 == ret){
		ret = VLAN_CONFIG_SUCCESS;
	}
	else{
		ret = VLAN_CONFIG_FAIL;
	}
#endif
	return ret;
}
/******************************************
*Func: nam_asic_set_port_pvid
*Params:
*		devNum--0,1
*		portNum--[0,23]
*		pvid--[1,4095]
*Call: 
*		cpssDxChBrgVlanPortVidSet
*Return:
*		VLAN_CONFIG_SUCCESS
*		VLAN_CONFIG_FAIL
*****************************************/
unsigned int nam_asic_set_port_pvid
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned short pvid
)
{
	unsigned long ret = 0;
	
#ifdef DRV_LIB_CPSS
	ret = cpssDxChBrgVlanPortVidSet(devNum,portNum,pvid);
	if(0 == ret){
		ret = VLAN_CONFIG_SUCCESS;
	}
	else{
		ret = VLAN_CONFIG_FAIL;
	}
#endif

#ifdef DRV_LIB_BCM
	ret = bcm_port_untagged_vlan_set(devNum,portNum,pvid);
	if(0 == ret){
		ret = VLAN_CONFIG_SUCCESS;
	}else{
		ret = VLAN_CONFIG_FAIL;
		return ret;
	}
#endif
	return ret;
}
/**************************************
*Func: nam_asic_get_pvid
*Params:
*		devNum--0,1
*		portNum--[0,23]
*		pvid--[1,4095]
*Call: 
*		cpssDxChBrgVlanPortVidGet
*Return:
*		VLAN_CONFIG_SUCCESS
*		VLAN_CONFIG_FAIL
***************************************/
unsigned int nam_asic_get_port_pvid
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned short* pvid
)
{
	unsigned long ret = 0;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChBrgVlanPortVidGet(devNum,portNum,pvid);

	if(0 == ret){
		ret = VLAN_CONFIG_SUCCESS;
	}
	else{
		ret = VLAN_CONFIG_FAIL;
	}
#endif

#ifdef DRV_LIB_BCM
	ret = bcm_port_untagged_vlan_get(devNum,portNum,pvid);
	if(0 == ret){
		ret = VLAN_CONFIG_SUCCESS;
	}else{
		ret = VLAN_CONFIG_FAIL;
		return ret;
	}
#endif
	return ret;
}
/**********************************************
*Func: nam_asic_vlan_entry_ports_add
*Params: 
*		devNum--0,1
*		vlanId--[2,4095]
*		portNum--[0,23]
*		istagged--'untag','tag'
*Call: 
*		bcm_vlan_port_add
*Return:
*		VLAN_CONFIG_SUCCESS
*		VLAN_CONFGI_FAIL
*Comment:
*		when a port adds to vlan as untagged 
*	mode,it'll set the port's Pvid to be the
*	same with vlan ID.And at the same time,
*	the port'll be deleted from Default vlan
*	1.
*
**********************************************/
/*add or delete port to( or from) specify vlan*/

unsigned int nam_asic_vlan_entry_ports_add
(
	unsigned char devNum,
	unsigned short vlanId,
	unsigned char portNum,
	unsigned char isTagged
) 
{
	unsigned long ret = 0;
	int asictype = 0;

#ifdef DRV_LIB_CPSS	
	CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT portTaggingCmd;
	 nam_syslog_dbg("nam add %s port %d on device %d to vlan entry %d\n",isTagged ? "tagged":"untagged",portNum,devNum,vlanId); 
#ifdef DRV_LIB_CPSS_3_4	 
	if(isTagged) {
		ret = cpssDxChBrgVlanMemberAdd (devNum, vlanId, portNum, isTagged, CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E);
	}
	else if(!isTagged) {
		ret = cpssDxChBrgVlanMemberAdd (devNum, vlanId, portNum, isTagged, CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E);
	}
#else
	ret = cpssDxChBrgVlanMemberAdd (devNum, vlanId, portNum, isTagged);
#endif
	if (0 == ret) {
		ret = VLAN_CONFIG_SUCCESS;
	}
	else {
		 syslog_ax_nam_vlan_err("add %s port %d on device %d to vlan %d error %d\n",isTagged ? "tagged":"untagged",portNum,devNum,vlanId,ret);
		ret = VLAN_CONFIG_FAIL;
		return ret;
	}
	asictype = nam_asic_get_asic_type(devNum);
	if((asictype < ASIC_TYPE_START_EXMX_E) || (asictype > ASIC_TYPE_LAST_E))
	{
		nam_syslog_err("get asic type fail !\n");
	}
	if(asictype == ASIC_TYPE_DXCH_LION_E)
	{
		ret = cpssDxChBrgFdbPortLearnStatusSet(devNum,portNum,0,CPSS_LOCK_FRWRD_E);
		if(0 != ret){
			 nam_syslog_dbg("cpssDxChBrgFdbPortLearnStatusSet  error!\n");
			return ret;
		}
	}
	else
	{
		
		ret = cpssDxChBrgFdbPortLearnStatusSet(devNum,portNum,1,CPSS_LOCK_FRWRD_E);
		if(0 != ret){
			 nam_syslog_dbg("cpssDxChBrgFdbPortLearnStatusSet  error!\n");
			return ret;
		}
	}
	
	/*port untagged added to vlan,at default the port PVID Set be equal to vlanId*/
	if(!isTagged) {
		ret = cpssDxChBrgVlanPortVidSet(devNum,portNum,vlanId);
		if (0 == ret) {
			/*cpssDxChBrgVlanPortVidGet(devNum, portNum, &vid);*/
			ret = VLAN_CONFIG_SUCCESS;
		}
		else {
			 syslog_ax_nam_vlan_err("set port %d pvid on device %d error %d\n",portNum,devNum,ret);
			ret = VLAN_CONFIG_FAIL;
			/*return ret;*/
		}
		vlanId = 1;
		ret = cpssDxChBrgVlanPortDelete(devNum,vlanId,portNum);
		if (0 == ret) {
			ret = VLAN_CONFIG_SUCCESS;
		}
		else {
			 syslog_ax_nam_vlan_err("delete port %d on device %d from default vlan error %d\n",portNum,devNum,ret);
			ret = VLAN_CONFIG_FAIL;
			/*return ret;*/
		}
	}
#endif

#ifdef DRV_LIB_BCM
	bcm_pbmp_t pbmp ,ut_pbmp ;
	bcm_pbmp_t pbmp1,ut_pbmp1;
	bcm_pbmp_t defbmp,defut_pbmp;

	SOC_PBMP_CLEAR(pbmp);
	SOC_PBMP_CLEAR(ut_pbmp);
	SOC_PBMP_CLEAR(pbmp1);
	SOC_PBMP_CLEAR(ut_pbmp1);
	SOC_PBMP_CLEAR(defbmp);
	SOC_PBMP_CLEAR(defut_pbmp);

	if(isTagged) {
		/*port add to vlan as tagged*/

		/*get ports from default vlan and show on debug*/		
		ret = bcm_vlan_port_get(devNum,1,&pbmp1,&ut_pbmp1);
		if(0 == ret){
			ret = VLAN_CONFIG_SUCCESS;
		}else{
			ret = VLAN_CONFIG_FAIL;
			return ret;
		}
		syslog_ax_nam_vlan_dbg("default vlan ports %#x untagports %#x\n",
							pbmp1.pbits[0],ut_pbmp1.pbits[0]);			
		
		/*add port bitmap: portNum to pbmp*/
		SOC_PBMP_PORT_ADD(pbmp, portNum);

		syslog_ax_nam_vlan_dbg("vlan %d add ports %#x untag ports %#x\n",
					vlanId,pbmp.pbits[0],ut_pbmp.pbits[0]);
		
		/*add this port as tagged*/
		ret = bcm_vlan_port_add(devNum,vlanId,pbmp,ut_pbmp);
		if(0 == ret){
			ret = VLAN_CONFIG_SUCCESS;
		}else{
			ret = VLAN_CONFIG_FAIL;
			return ret;
		}
		
		/*get ports from vlan and show on debug*/
		ret = bcm_vlan_port_get(devNum,vlanId,&pbmp,&ut_pbmp);
		if(0 == ret){
			ret = VLAN_CONFIG_SUCCESS;
		}else{
			ret = VLAN_CONFIG_FAIL;
			return ret;
		}
		syslog_ax_nam_vlan_dbg("add to vlan %d ports %#x untagports %#x\n",
							vlanId,pbmp.pbits[0],ut_pbmp.pbits[0]);	
	}		
	else if(!isTagged){
		/*port add to vlan as untagged*/
		
		/*get ports from default vlan and show on debug*/
		ret = bcm_vlan_port_get(devNum,1,&pbmp1,&ut_pbmp1);
		if(0 == ret){
			ret = VLAN_CONFIG_SUCCESS;
		    syslog_ax_nam_vlan_dbg("get default vlan ports %#x untagports %#x\n",
					pbmp1.pbits[0],ut_pbmp1.pbits[0]);
		}else{
			ret = VLAN_CONFIG_FAIL;
			return ret;
		}

		/*add port bitmap: portNum to pbmp and ut_pbmp*/
		SOC_PBMP_PORT_ADD(pbmp, portNum);
		SOC_PBMP_PORT_ADD(ut_pbmp, portNum);
		
		syslog_ax_nam_vlan_dbg("vlan %d add ports %#x untag ports %#x\n",
					vlanId,pbmp.pbits[0],ut_pbmp.pbits[0]);

		/*add this port as untagged*/		
		ret = bcm_vlan_port_add(devNum,vlanId,pbmp,ut_pbmp);
		if(0 == ret){
			ret = VLAN_CONFIG_SUCCESS;
		}else{
			ret = VLAN_CONFIG_FAIL;
			return ret;
		}
		
		/*get ports from vlan and show on debug*/
		ret = bcm_vlan_port_get(devNum,vlanId,&pbmp,&ut_pbmp);
		if(0 == ret){
			ret = VLAN_CONFIG_SUCCESS;
		}else{
			ret = VLAN_CONFIG_FAIL;
			return ret;
		}
		syslog_ax_nam_vlan_dbg("add to vlan %d ports %#x,untagports %#x\n",
						vlanId,pbmp.pbits[0],ut_pbmp.pbits[0]);

		/*add port bitmap: portNum to defbmp and defut_pbmp*/
		SOC_PBMP_PORT_ADD(defbmp, portNum);
		SOC_PBMP_PORT_ADD(defut_pbmp, portNum);

		/*if port add to vlan as untagged,this port should delete from default vlan*/
		ret = bcm_vlan_port_remove(devNum,1,defbmp);
		if(0 == ret){
			ret = VLAN_CONFIG_SUCCESS;
		}else{
			ret = VLAN_CONFIG_FAIL;
			return ret;
		}

		/*get ports from default vlan and show on debug*/		
		ret = bcm_vlan_port_get(devNum,1,&defbmp,&defut_pbmp);
		if(0 == ret){
			ret = VLAN_CONFIG_SUCCESS;
		}else{
			ret = VLAN_CONFIG_FAIL;
			return ret;
		}
		syslog_ax_nam_vlan_dbg("get default vlan ports %#x untagports %#x\n",
			         			defbmp.pbits[0],defut_pbmp.pbits[0]);		

		/*port untagged added to vlan,port PVID should be equal to vlanId*/
		ret = bcm_port_untagged_vlan_set(devNum,portNum,vlanId);
		if(0 == ret){
			ret = VLAN_CONFIG_SUCCESS;
		}else{
			ret = VLAN_CONFIG_FAIL;
			return ret;
		}
	}
#endif
	return ret;	
}
/**********************************************
*Func: nam_asic_vlan_entry_ports_del
*Params: 
*		devNum--0,1
*		vlanId--[2,4095]
*		portNum--[0,23]
*		needJoin - need add to default vlan untagged or not
*Call: 
*		cpssDxChBrgVlanMemberAdd
*		cpssDxChBrgVlanPortVidSet
*		cpssDxChBrgVlanPortDelete
*Return:
*		VLAN_CONFIG_SUCCESS
*		VLAN_CONFGI_FAIL
*Comment:
*		when a port adds to vlan as untagged 
*	mode,it'll set the port's Pvid to be the
*	same with vlan ID.And at the same time,
*	the port'll be deleted from Default vlan
*	1.
*
**********************************************/
/*for delete port from vlan*/
unsigned int nam_asic_vlan_entry_ports_del
(
	unsigned short vlanId,
	unsigned char  devNum,
	unsigned char  portNum,
	unsigned char  needJoin
)
{
	unsigned long ret = 0;

#ifdef DRV_LIB_CPSS
	unsigned int  portTagmode =0;
	struct DRV_VLAN_PORT_BMP_S portMbrPtr;
	struct DRV_VLAN_PORT_BMP_S porttagPtr;
	struct brg_vlan_drv_info_s vlanInfoPtr;
	CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT portTaggingCmd;
	CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmdPtr;
	
	unsigned long isValide;

	ret = cpssDxChBrgVlanPortDelete(devNum,vlanId,portNum );
	if (0 == ret) {
		ret = VLAN_CONFIG_SUCCESS;
	}
	else {
		ret = VLAN_CONFIG_FAIL;
		return ret;
	}
	nam_syslog_dbg("nam_asic_vlan_entry_ports_del vlan %d port %d on device %d\n",vlanId,portNum,devNum); 

	if(needJoin){
#ifdef DRV_LIB_CPSS_3_4
		ret = cpssDxChBrgVlanEntryRead(devNum, vlanId, &portMbrPtr, &porttagPtr, &vlanInfoPtr, &isValide, &portsTaggingCmdPtr);
#else
		ret = cpssDxChBrgVlanEntryRead(devNum, vlanId, &portMbrPtr, &porttagPtr, &vlanInfoPtr, &isValide);
#endif
		if (0 == ret) {
#ifdef DRV_LIB_CPSS_3_4
			ret = cpssDxChBrgVlanMemberAdd(devNum, 1, portNum, 0, CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E);/* 0 - untagged*/
			ret = cpssDxChBrgVlanPortVidSet(devNum,portNum,1);
			if(0 != ret) {
				ret = VLAN_CONFIG_FAIL;
				return ret;
			}
#else
			/* get port tag mode by bitmap
			 * portTagmode = 1 : port is tagged in vlan 
			 * portTagmode = 0 : port is untagged in vlan */
			portTagmode = (porttagPtr.ports[0] & (1 << portNum));
			nam_syslog_dbg("porttagPtr.ports[0] %#x portRshift %d portNum %d\n",porttagPtr.ports[0],portTagmode,portNum);
			if(!portTagmode) {
				ret = cpssDxChBrgVlanMemberAdd(devNum, 1, portNum, 0);/* 0 - untagged*/
				ret = cpssDxChBrgVlanPortVidSet(devNum,portNum,1);
				if(0 != ret) {
					ret = VLAN_CONFIG_FAIL;
					return ret;
				}
			}
#endif
			/*nam_asic_igmp_trap_set_by_devport(devNum,portNum,0);*//*disable igmp trap on port.*/
			return VLAN_CONFIG_SUCCESS;
		}
		else {
			nam_syslog_dbg("read vlan %d entry error\n",vlanId);				
			return VLAN_CONFIG_FAIL;
		}	
	}
#endif

#ifdef DRV_LIB_BCM	

	bcm_pbmp_t pbmp,ut_pbmp;

	SOC_PBMP_CLEAR(pbmp);
	SOC_PBMP_CLEAR(ut_pbmp);

	/*add port bitmap: portNum to pbmp and ut_pbmp*/
	SOC_PBMP_PORT_ADD(pbmp, portNum);
	SOC_PBMP_PORT_ADD(ut_pbmp, portNum);

	/*delete port from vlan entry*/	
	ret = bcm_vlan_port_remove(devNum, vlanId, pbmp);
	if (0 == ret) {
		ret = VLAN_CONFIG_SUCCESS;
	}
	else {
		ret = VLAN_CONFIG_FAIL;
		syslog_ax_nam_vlan_dbg("port %d remove from vlan %d error ret %d.\n",portNum,vlanId,ret);
		return ret;
	}

	/*if delete port as untagged,this port should be add to default vlan as untagged*/
	if(needJoin){
		/*set this port PVID should be equal to default vlan*/
		ret = bcm_port_untagged_vlan_set(devNum,portNum,1);
		if(0 == ret){
			ret = VLAN_CONFIG_SUCCESS;
		}else{
			ret = VLAN_CONFIG_FAIL;
			syslog_ax_nam_vlan_dbg("port %d set PVID 1 error ret %d.\n",portNum,ret);			
			return ret;
		}

		/*add to default vlan as untagged*/
	 	ret = bcm_vlan_port_add(devNum,1,pbmp,ut_pbmp);
		if (0 == ret) {
			ret = VLAN_CONFIG_SUCCESS;
		}
		else {
			ret = VLAN_CONFIG_FAIL;
			syslog_ax_nam_vlan_dbg("port %d add to vlan 1 error ret %d.\n",portNum,ret);		
			return ret;
		}		
	}
#endif	
	return VLAN_CONFIG_SUCCESS;
}

/**********************************************
*Func: nam_asic_vlan_entry_trunk_mbr_add
*Params: 
*		devNum--0,1
*		vlanId--[2,4095]
*		trunkId--[1,127]
*		istagged--'untag','tag'
*Call: 
*		cpssDxChBrgVlanMemberAdd
*		cpssDxChBrgVlanPortVidSet
*		cpssDxChBrgVlanPortDelete
*Return:
*		VLAN_CONFIG_SUCCESS
*		VLAN_CONFGI_FAIL
*Comment:
*		when a trunk adds to vlan as untagged 
*	mode,it'll set the port's Pvid to be the
*	same with vlan ID.And at the same time,
*	the port'll be deleted from Default vlan
*	1.
*
**********************************************/
/*add trunk to( or from) specify vlan*/

unsigned int nam_asic_vlan_entry_trunk_mbr_add
(
	unsigned short vlanId,
	unsigned short trunkId,
	unsigned int   trunkMbrBmp0,
	unsigned int   trunkMbrBmp1,
	unsigned char isTagged
) 
{
	/*NAM_DEBUG(("Enter add trunk member to vlan..."));*/
	unsigned long	ret = 0;

#ifdef DRV_LIB_CPSS
	unsigned int	i,numOfPp;
	struct DRV_VLAN_PORT_BMP_S  portsMembers;
	struct DRV_VLAN_PORT_BMP_S  portsTagging;
	struct brg_vlan_drv_info_s  vlanInfo;
	unsigned long isValid = 0;
	unsigned int trunkMbrBmp[NAM_PP_DEV_NUM];
	trunkMbrBmp[0] = trunkMbrBmp0;
	trunkMbrBmp[1] = trunkMbrBmp1;
	CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmd;
	/*NAM_DEBUG(("trunkMbrBmp[0] = %0x,trunkMbrBmp[1] = %0x",trunkMbrBmp[0],trunkMbrBmp[1]));*/
	numOfPp = nam_asic_get_instance_num();
	/*NAM_DEBUG(("numOfPp = %d",numOfPp));*/
	for (i=0; i<numOfPp; i++) {
#ifdef DRV_LIB_CPSS_3_4		
		ret = cpssDxChBrgVlanEntryRead(i, \
									  vlanId, \
									  &portsMembers, \
									  &portsTagging, \
									  &vlanInfo, \
									  &isValid,   \
									  &portsTaggingCmd);
#else
		ret = cpssDxChBrgVlanEntryRead(i, \
									  vlanId, \
									  &portsMembers, \
									  &portsTagging, \
									  &vlanInfo, \
									  &isValid);
#endif
		/*
		NAM_DEBUG(("dev %d portMbrBmp %0x portTagBmp %0x,isValid %d",i,\
												 portsMembers.ports[0],\
												 portsTagging.ports[0],
												 isValid));
		*/
		if (0 != ret || 1 != isValid) {
			/*NAM_DEBUG(("vlan entry read err OR the vlan entry is Invalid"));*/
			break;
		}
		else {
			portsMembers.ports[0] |= trunkMbrBmp[i];
			/*NAM_DEBUG(("dev %d add trunk member:portsMbrBmp.ports[0] %0x", 
							i,portsMembers.ports[0]));*/
			if(isTagged) {
				portsTagging.ports[0] |= trunkMbrBmp[i];
#ifdef DRV_LIB_CPSS_3_4				
				ret = cpssDxChBrgVlanEntryWrite(i, \
												vlanId, \
												&portsMembers, \
												&portsTagging, \
												&vlanInfo,  \
												&portsTaggingCmd);
#else
				ret = cpssDxChBrgVlanEntryWrite(i, \
												vlanId, \
												&portsMembers, \
												&portsTagging, \
												&vlanInfo);
#endif
			}
			else {
#ifdef DRV_LIB_CPSS_3_4				
				/*NAM_DEBUG(("add trunk untag"));*/
				ret = cpssDxChBrgVlanEntryWrite(i, \
												vlanId, \
												&portsMembers, \
												&portsTagging, \
												&vlanInfo,  \
												&portsTaggingCmd);
#else
				ret = cpssDxChBrgVlanEntryWrite(i, \
												vlanId, \
												&portsMembers, \
												&portsTagging, \
												&vlanInfo);
#endif
			}
		}
	}
#endif
	return ret;
}
/**********************************************
*Func: nam_asic_vlan_entry_trunk_mbr_del
*Params: 
*		devNum--0,1
*		vlanId--[2,4095]
*		trunkId--[1,127]
*		istagged--'untag','tag'
*Call: 
*		cpssDxChBrgVlanMemberAdd
*		cpssDxChBrgVlanPortVidSet
*		cpssDxChBrgVlanPortDelete
*Return:
*		VLAN_CONFIG_SUCCESS
*		VLAN_CONFGI_FAIL
*Comment:
*		when a trunk adds to vlan as untagged 
*	mode,it'll set the port's Pvid to be the
*	same with vlan ID.And at the same time,
*	the port'll be deleted from Default vlan
*	1.
*
**********************************************/
/* delete trunk to  (from) specify vlan*/

unsigned int nam_asic_vlan_entry_trunk_mbr_del
(
	unsigned short vlanId,
	unsigned short trunkId,
	unsigned int	trunkMbrBmp0,
	unsigned int	trunkMbrBmp1,
	unsigned char isTagged
) 
{
	unsigned long	ret = 0;

#ifdef DRV_LIB_CPSS
	unsigned int	i,portsMbrTmp = 0,portsTagTmp = 0,numOfPp = 0;
	struct DRV_VLAN_PORT_BMP_S  portsMembers;
	struct DRV_VLAN_PORT_BMP_S  portsTagging;
	struct brg_vlan_drv_info_s  vlanInfo;
	unsigned long isValid;
	unsigned int trunkMbrBmp[NAM_PP_DEV_NUM];	
	CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmd;
	trunkMbrBmp[0] = trunkMbrBmp0;
	trunkMbrBmp[1] = trunkMbrBmp1;
	numOfPp = nam_asic_get_instance_num();

	for (i=0; i<numOfPp; i++) {
#ifdef DRV_LIB_CPSS_3_4		
		ret = cpssDxChBrgVlanEntryRead(i, \
									  vlanId, \
									  &portsMembers, \
									  &portsTagging, \
									  &vlanInfo, \
									  &isValid,   \
									  &portsTaggingCmd);
#else
		ret = cpssDxChBrgVlanEntryRead(i, \
									  vlanId, \
									  &portsMembers, \
									  &portsTagging, \
									  &vlanInfo, \
									  &isValid);
#endif
		if (0 != ret || 1 != isValid) {break;}
		else {
			portsMbrTmp = portsMembers.ports[0] ^ trunkMbrBmp[i]; 	
			portsMembers.ports[0] = portsMbrTmp;	/*get port member left in vlan*/
			if(isTagged) {
				portsTagTmp = portsTagging.ports[0] ^ trunkMbrBmp[i]; 	
				portsTagging.ports[0] = portsTagTmp; /*get tag port left in vlan*/
#ifdef DRV_LIB_CPSS_3_4		
				ret = cpssDxChBrgVlanEntryWrite(i, \
												vlanId, \
												&portsMembers, \
												&portsTagging, \
												&vlanInfo,  \
												&portsTaggingCmd);
#else
				ret = cpssDxChBrgVlanEntryWrite(i, \
												vlanId, \
												&portsMembers, \
												&portsTagging, \
												&vlanInfo);
#endif
			}
			else {
#ifdef DRV_LIB_CPSS_3_4				
				ret = cpssDxChBrgVlanEntryWrite(i, \
												vlanId, \
												&portsMembers, \
												&portsTagging, \
												&vlanInfo,  \
												&portsTaggingCmd);
#else
				ret = cpssDxChBrgVlanEntryWrite(i, \
												vlanId, \
												&portsMembers, \
												&portsTagging, \
												&vlanInfo);
#endif
			}
		}
	}
#endif
	return ret;
}

/*********************************************
*Func: nam_asic_vlan_entry_delete
*Params: 
*		vlanId--[0,4095]
*Call:
*		bcm_vlan_destroy
*Return:
*		VLAN_CONFIG_SUCCESS
*		VLAN_CONFIG_FAIL
*Comment:
*		Delete a vlan,it should do Ops below
*		1)Find out all untag member of the vlan
*		  and add these port to Default vlan  
*		2)Reset the ports' Pvid to 1
*		3)Clear the vlan entry
*		4)Invalidate the vlan
*
**********************************************/
/*delete one vlan entry on special dev*/
unsigned int nam_asic_vlan_entry_delete(unsigned short vlanId) 
{
	unsigned long ret = 0;
	
#ifdef DRV_LIB_CPSS
	unsigned long numOfPp = 0;

#ifdef DRV_LIB_CPSS_3_4
	unsigned int i = 0,j = 0;
	struct vlan_ports untagPorts,tagPorts;	
	unsigned char devNum = 0,virPortNo =0;
	unsigned int needJoin = 0;

	memset(&untagPorts,0,sizeof(struct vlan_ports));
	memset(&tagPorts,0,sizeof(struct vlan_ports));

	numOfPp = nam_asic_get_instance_num();
	for (i=0; i<numOfPp; i++) {
		/*delete all ports in this vlan before destroy the vlan*/
		
		/*get all ports in this vlan*/
		ret = npd_vlan_get_all_ports(vlanId,&untagPorts,&tagPorts);
		syslog_ax_nam_vlan_dbg("get vlan %d untag port number %d tag port number %d\n"
								,vlanId,untagPorts.count,tagPorts.count);
		
		/*loop the number of untagPorts*/
		for(j = 0;j < untagPorts.count;j ++) {
			devNum = 0;
			virPortNo =0;
			needJoin = 1;/*port delete from vlan as untagged need to add to default vlan*/
			/*get ports device number and virtual port info from ethernet global index*/
			ret = npd_get_devport_by_global_index(untagPorts.ports[j],&devNum,&virPortNo);
			if(0 != ret) {
				syslog_ax_nam_vlan_dbg("get devport Error.\n");
			}
			else {
				/*delete this(these) port(s) from this vlan*/
				ret = nam_asic_vlan_entry_ports_del(vlanId,devNum,virPortNo,needJoin);
				if(0 != ret) {
					syslog_ax_nam_vlan_dbg("delete vlan %d untag port %d error ret %d.\n",vlanId,virPortNo,ret);
				}
			}
		}

		/*loop the number of tagPorts*/ 	
		for(j = 0;j < tagPorts.count;j ++) {
			devNum = 0;
			virPortNo = 0;
			needJoin = 0;/*port delete from vlan as tagged do not add to default vlan*/
			/*get ports device number and virtual port info from ethernet global index*/			
			ret = npd_get_devport_by_global_index(tagPorts.ports[j],&devNum,&virPortNo);
			if(0 != ret){
				syslog_ax_nam_vlan_dbg("get devport Error.\n");
			}
			else {
				/*delete this(these) port(s) from this vlan*/
				ret = nam_asic_vlan_entry_ports_del(vlanId,devNum,virPortNo,needJoin);
				if(0 != ret) {
					syslog_ax_nam_vlan_dbg("delete vlan %d tag port %d error ret %d.\n",vlanId,virPortNo,ret);
				}
			}
		}
		ret = cpssDxChBrgVlanEntryInvalidate(i,vlanId);
		if (0 == ret) {
			ret = VLAN_CONFIG_SUCCESS;
		}
		else {		
			ret = VLAN_CONFIG_FAIL;
			return ret;
		}		
	}	
#else
	unsigned short i,portNum = 0;
	unsigned int   BmpRshift = 0;
	struct DRV_VLAN_PORT_BMP_S	portsMbrPtr[2];
	struct DRV_VLAN_PORT_BMP_S	portsTagPtr[2];
	struct DRV_VLAN_PORT_BMP_S	portsUntagPtr[2];
	struct DRV_VLAN_PORT_BMP_S	portsMbrPtr_v1[2];
	struct DRV_VLAN_PORT_BMP_S	portsTagPtr_v1[2];
	struct brg_vlan_drv_info_s	vlanInfoPtr,vlanInfoPtr_v1;
	unsigned long  isValidPtr;
	numOfPp = nam_asic_get_instance_num();
	for(i=0;i<numOfPp;i++) {
		ret = cpssDxChBrgVlanEntryRead(i, vlanId, &portsMbrPtr[i], &portsTagPtr[i], &vlanInfoPtr, &isValidPtr);
		portsUntagPtr[i].ports[0]= portsMbrPtr[i].ports[0]^portsTagPtr[i].ports[0]; /*untag belonged to vlan,these ports'll add to vlan1.*/
		portsUntagPtr[i].ports[1]= portsMbrPtr[i].ports[1]^portsTagPtr[i].ports[1];
		ret = cpssDxChBrgVlanEntryRead(i, 1, &portsMbrPtr_v1[i], &portsTagPtr_v1[i], &vlanInfoPtr_v1,&isValidPtr);
		portsMbrPtr_v1[i].ports[0] |= portsUntagPtr[i].ports[0];
		portsMbrPtr_v1[i].ports[1] |= portsUntagPtr[i].ports[1];
		/*untag port of this vlan return to Default vlan.*/
		ret = cpssDxChBrgVlanEntryWrite(i, 1, &portsMbrPtr_v1[i],&portsTagPtr_v1[i], &vlanInfoPtr);

		for(portNum =0;portNum<NPD_VLAN_BITMAP_WIDTH;portNum++) {
			BmpRshift = portsUntagPtr[i].ports[0] >> portNum;
			if(BmpRshift & 0x1) {
				cpssDxChBrgVlanPortVidSet(i,portNum,1);
			}
		}
		/*set the bitmap to 0*/
		memset(&portsMbrPtr[0],0,sizeof(struct DRV_VLAN_PORT_BMP_S));
		memset(&portsTagPtr[0],0,sizeof(struct DRV_VLAN_PORT_BMP_S));

		/*clear portmembre bitmap of this vlan!*/
		ret = cpssDxChBrgVlanEntryWrite(i, vlanId, &portsMbrPtr[0], &portsTagPtr[0], &vlanInfoPtr);

		/*compare to original member bitmap,NOT necessary!*/
		ret = cpssDxChBrgVlanEntryRead(0,vlanId,&portsMbrPtr[0],&portsTagPtr[0],&vlanInfoPtr,&isValidPtr);
		ret = cpssDxChBrgVlanEntryInvalidate(i,vlanId);
		if (0 == ret) {
			ret = VLAN_CONFIG_SUCCESS;
		}
		else {
			ret = VLAN_CONFIG_FAIL;
			return ret;
		}
	}
#endif
#endif
#ifdef DRV_LIB_BCM	
	unsigned long numOfPp = 0;
	unsigned int i = 0,j = 0;
	struct vlan_ports untagPorts,tagPorts;	
	unsigned char devNum = 0,virPortNo =0;
	unsigned int needJoin = 0;

	memset(&untagPorts,0,sizeof(struct vlan_ports));
	memset(&tagPorts,0,sizeof(struct vlan_ports));
	
	numOfPp = nam_asic_get_instance_num();
	for (i=0; i<numOfPp; i++) {
		/*delete all ports in this vlan before destroy the vlan*/
		
		/*get all ports in this vlan*/
		ret = npd_vlan_get_all_ports(vlanId,&untagPorts,&tagPorts);
		syslog_ax_nam_vlan_dbg("get vlan %d untag port number %d tag port number %d\n"
								,vlanId,untagPorts.count,tagPorts.count);
		
		/*loop the number of untagPorts*/
		for(j = 0;j < untagPorts.count;j ++) {
			devNum = 0;
			virPortNo =0;
			needJoin = 1;/*port delete from vlan as untagged need to add to default vlan*/
			/*get ports device number and virtual port info from ethernet global index*/
			ret = npd_get_devport_by_global_index(untagPorts.ports[j],&devNum,&virPortNo);
			if(0 != ret){
				syslog_ax_nam_vlan_dbg("get devport Error.\n");
			}
			else {
				/*delete this(these) port(s) from this vlan*/
				ret = nam_asic_vlan_entry_ports_del(vlanId,devNum,virPortNo,needJoin);
				if(0 != ret) {
					syslog_ax_nam_vlan_dbg("delete vlan %d untag port %d error ret %d.\n",vlanId,virPortNo,ret);
				}
			}
		}

		/*loop the number of tagPorts*/		
		for(j = 0;j < tagPorts.count;j ++) {
			devNum = 0;
			virPortNo = 0;
			needJoin = 0;/*port delete from vlan as tagged do not add to default vlan*/
			/*get ports device number and virtual port info from ethernet global index*/			
			ret = npd_get_devport_by_global_index(tagPorts.ports[j],&devNum,&virPortNo);
			if(0 != ret){
				syslog_ax_nam_vlan_dbg("get devport Error.\n");
			}
			else {
				/*delete this(these) port(s) from this vlan*/
				ret = nam_asic_vlan_entry_ports_del(vlanId,devNum,virPortNo,needJoin);
				if(0 != ret) {
					syslog_ax_nam_vlan_dbg("delete vlan %d tag port %d error ret %d.\n",vlanId,virPortNo,ret);
				}
			}
		}	
		/*destroy this vlan entry*/
		ret = bcm_vlan_destroy(i, vlanId);
		if (0 == ret) {
			ret = VLAN_CONFIG_SUCCESS;
		}
		else {
			syslog_ax_nam_vlan_dbg("destroy the vlan %d error ret %d.\n",vlanId,ret);			
			ret = VLAN_CONFIG_FAIL;
			return ret;
		}		
	}	
#endif	
	return ret;
}

/**********************************
*Func: nam_asic_vlan_update_vid
*Params: 
*		vid_old--[2,4095]
*		vid_new--[2,4095]
*Call: 
*		cpssDxChBrgVlanEntryRead
*		prvCpssDxChWriteTableEntryField
*Return:
*		VLAN_CONFGI_SUCCESS
*		VLAN_CONFGI_FAIL
*Comment:
*		Read original vlan Fields,those
*		fields'll be write in new vlan
*		entry.
*		After reading the original vlan,
*		it'll be invalidate, 
*		After writing new vlan fields,it
*		will be validate.
*		The last,the original vlan's untag
*		port members' pvid'll modified to 
*		new vlanId.
**********************************/
unsigned int nam_vlan_update_vid
(
	unsigned int product_id,
	unsigned short vid_old,
	unsigned short vid_new
)
{
	unsigned long ret = 0;

#ifdef DRV_LIB_CPSS
	unsigned int  i,j,rShift = 1,numOfPp;
	struct DRV_VLAN_PORT_BMP_S portMbrPtr;
	struct DRV_VLAN_PORT_BMP_S portTagPtr;
	struct brg_vlan_drv_info_s vlanInfoPtr;
	unsigned int untagbmp = 0;
	unsigned long isValide = 1;
	CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmd;
	numOfPp = nam_asic_get_instance_num();
	for(i=0;i<numOfPp;i++){
#ifdef DRV_LIB_CPSS_3_4		
		ret = cpssDxChBrgVlanEntryRead(i,vid_old, &portMbrPtr, &portTagPtr, &vlanInfoPtr, &isValide, &portsTaggingCmd);
#else
		ret = cpssDxChBrgVlanEntryRead(i,vid_old, &portMbrPtr, &portTagPtr, &vlanInfoPtr, &isValide);
#endif
		if(VLAN_CONFIG_FAIL == ret){
			 syslog_ax_nam_vlan_err("read original vlan %d on dev %d Error!",vid_old,i);
			continue;
		}
		else if(!isValide) {
			 syslog_ax_nam_vlan_err("original vlan %d on dev %s isValid = %d!",vid_old,i,isValide);
			continue;
		}
		else {
			syslog_ax_nam_vlan_err("original vlan %d on dev %d isValid = %d",vid_old,i,isValide);

			if( 0 != cpssDxChBrgVlanEntryInvalidate(i,vid_old)){
				ret = VLAN_CONFIG_FAIL;
			}
			if( 0 != cpssDxChBrgVlanEntryValidate(i, vid_new)){
				ret = VLAN_CONFIG_FAIL;
			}
#ifdef DRV_LIB_CPSS_3_4					
			ret = cpssDxChBrgVlanEntryWrite(i,vid_new, &portMbrPtr, &portTagPtr, &vlanInfoPtr, &portsTaggingCmd);
#else
			ret = cpssDxChBrgVlanEntryWrite(i,vid_new, &portMbrPtr, &portTagPtr, &vlanInfoPtr);
#endif
			if(VLAN_CONFIG_FAIL == ret) {
				 syslog_ax_nam_vlan_err("write new vlan %d on dev %d Error!",vid_new,i);
				continue;
			}
			else {
				/*untag member pvid update*/
				untagbmp = portMbrPtr.ports[0]^portTagPtr.ports[0];
				 syslog_ax_nam_vlan_err("original vlan untagbmp = %x.",untagbmp);
				for(j=0;j<32;j++){
					rShift = rShift<<j;
					if(untagbmp&rShift) {
						ret = cpssDxChBrgVlanPortVidSet(i, j, vid_new);
						 syslog_ax_nam_vlan_err("untag port vid update %d<->%d",vid_old,vid_new);
					}
					rShift = 1;
				}
			} 
			/*clear original vlan Member Bitmap 0*/
			clear_vlan_member_bmp(&portMbrPtr,&portTagPtr);
#ifdef DRV_LIB_CPSS_3_4								
			ret = cpssDxChBrgVlanEntryWrite(i, vid_old, &portMbrPtr, &portTagPtr, &vlanInfoPtr, &portsTaggingCmd);
#else 	
			ret = cpssDxChBrgVlanEntryWrite(i, vid_old, &portMbrPtr, &portTagPtr, &vlanInfoPtr);
#endif
		}
	}
#endif

#ifdef DRV_LIB_BCM
	/* update the vlanId should remove the ports in the old vlan and destroy this vlan entry
	   then create the new vlan which is updated and add these ports which belong to the old
	   vlan to this new vlan*/
	unsigned int i = 0;
	struct vlan_ports untagPorts,tagPorts;	
	unsigned char devNum = 0,virPortNo =0;
	bcm_pbmp_t pbmp,ut_pbmp;

	memset(&untagPorts,0,sizeof(struct vlan_ports));
	memset(&tagPorts,0,sizeof(struct vlan_ports));

	SOC_PBMP_CLEAR(pbmp);
	SOC_PBMP_CLEAR(ut_pbmp);		

	ret = nam_asic_vlan_entry_active(product_id, vid_new);
	if (0 != ret) {
		 syslog_ax_nam_vlan_err("nam_asic_vlan_entry_active::vlanId %d error.\n",vid_new);
	}
	ret = npd_vlan_get_all_ports(vid_new,&untagPorts,&tagPorts);	
	
	/*destory the old vlan entry,ports in old vlan release to default vlan*/
	ret = nam_asic_vlan_entry_delete(vid_old);
	if (0 != ret) {
		 syslog_ax_nam_vlan_err("nam_asic_vlan_entry_delete::vlanId %d error.\n",vid_old);
	}	

	/*loop the number of untagPorts*/
	for(i = 0;i < untagPorts.count;i ++) {
		devNum = 0;
		virPortNo = 0;
		/*get ports device number and virtual port info from ethernet global index*/
		ret = npd_get_devport_by_global_index(untagPorts.ports[i],&devNum,&virPortNo);
		if(0 != ret){
			syslog_ax_nam_vlan_dbg("get devport Error.\n");
		}
		
		/*add ports which belong to the old vlan before to the new vlan*/
		ret = nam_asic_vlan_entry_ports_add(devNum,vid_new,virPortNo,0);
		if(0 != ret){
			syslog_ax_nam_vlan_dbg("add untagged ports to new vlan %d error.\n",vid_new);
		}		
		syslog_ax_nam_vlan_dbg("get vlan %d untagPorts count %d uport[%d] %d.\n"
								,vid_new,untagPorts.count,i,virPortNo);
	}

	/*loop the number of tagPorts*/		
	for(i = 0;i < tagPorts.count;i ++) {
		devNum = 0;
		virPortNo = 0;
		/*get ports device number and virtual port info from ethernet global index*/			
		ret = npd_get_devport_by_global_index(tagPorts.ports[i],&devNum,&virPortNo);
		if(0 != ret){
			syslog_ax_nam_vlan_dbg("get devport Error.\n");
		}

		/*add ports which belong to the old vlan before to the new vlan*/
		ret = nam_asic_vlan_entry_ports_add(devNum,vid_new,virPortNo,1);
		if(0 != ret){
			syslog_ax_nam_vlan_dbg("add tagged ports to new vlan %d error.\n",vid_new);
		}	
		syslog_ax_nam_vlan_dbg("get vlan %d tagport count %d tport[%d] %d.\n"
								,vid_new,tagPorts.count,i,virPortNo);
	}		
#endif
	return ret;
}

/*************************************
*Func: nam_asic_vlan_port_route_en
*Params: 
*		devNum--...
*		portNum--...
*Call: 
*		cpssDxChIpPortRoutingEnable
*Return:
*		VLAN_CONFGI_SUCCESS
*		VLAN_CONFGI_FAIL
*
**************************************/

unsigned int nam_asic_vlan_port_route_en
(
	unsigned char devNum,
	unsigned char portNum
)
{
	unsigned long ret = VLAN_CONFIG_SUCCESS;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChIpPortRoutingEnable(devNum,portNum,CPSS_IP_UNICAST_E,CPSS_IP_PROTOCOL_IPV4_E, 1);
	if(VLAN_CONFIG_SUCCESS != ret) {
		ret = VLAN_CONFIG_FAIL;
	}
#endif
	return ret;
}

unsigned int nam_asic_vlan_port_route_dis
(
	unsigned char devNum,
	unsigned char portNum
)
{
	unsigned long ret = VLAN_CONFIG_SUCCESS;
	
#ifdef DRV_LIB_CPSS
	ret = cpssDxChIpPortRoutingEnable(devNum,portNum,CPSS_IP_UNICAST_E,CPSS_IP_PROTOCOL_IPV4_E, 0);
	if(VLAN_CONFIG_SUCCESS != ret) {
		ret = VLAN_CONFIG_FAIL;
	}
#endif
	return ret;
}
unsigned int nam_asic_port_l3intf_vlan_entry_set
(
	unsigned int product_id,
	unsigned short vlanId
)

{
	unsigned long ret = NPD_SUCCESS;
    int rc = 0;
#ifdef DRV_LIB_CPSS
	unsigned int i,numOfPp;
	struct DRV_VLAN_PORT_BMP_S untaggedportBmp;/*no used*/
	struct DRV_VLAN_PORT_BMP_S taggedportBmp;
	
	/*clear the membership *//*init default value 0.*/
	untaggedportBmp.ports[0]=untaggedportBmp.ports[1]=0;
	taggedportBmp.ports[0]=taggedportBmp.ports[1]=0;

	
	/*enable the vlan*/
	numOfPp = nam_asic_get_instance_num();
	for(i=0;i<numOfPp;i++){
		/*set vlan range*/
		/*
		* 1:CPSS_VLAN_ETHERTYPE0_E
		*	
		*/
		cpssDxChBrgVlanEtherTypeSet(i,1,0x8100,0xfff);
		cpssDxChBrgVlanEtherTypeSet(i,2,0x8100,0xfff);

		/*enable vlan :set vlan entry validBit =1*/
		cpssDxChBrgVlanEntryValidate(i, vlanId);
		
		/*unknow Mac SA security breach*/
		cpssDxChBrgVlanNASecurEnable(i, vlanId, NPD_FALSE);

		/*set 'autolearnDis' to FALSE,set to auto lean disabled*/
		cpssDxChBrgVlanLearningStateSet(i,vlanId,NPD_FALSE);
		/*disable na Msg to Cpu*/
		cpssDxChBrgVlanNaToCpuEnable(i,vlanId, NPD_FALSE);
		/*Drop unknow or Unregister packet*/
		
		cpssDxChBrgVlanUnkUnregFilterSet(i,vlanId, \
										 CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E, \
										 CPSS_PACKET_CMD_DROP_HARD_E);
		cpssDxChBrgVlanUnkUnregFilterSet(i,vlanId, \
										CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_BCAST_E,\
										/*CPSS_PACKET_CMD_DROP_HARD_E);  
											for unreg ipv4 mcast to cpu*/
										CPSS_PACKET_CMD_TRAP_TO_CPU_E);
		cpssDxChBrgVlanUnkUnregFilterSet(i,vlanId, \
										CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_MCAST_E,\
										/*CPSS_PACKET_CMD_DROP_HARD_E);  
											for unreg ipv4 mcast to cpu*/
										CPSS_PACKET_CMD_TRAP_TO_CPU_E);
		cpssDxChBrgVlanUnkUnregFilterSet(i,vlanId, \
										CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV6_MCAST_E,\
										/*CPSS_PACKET_CMD_DROP_HARD_E);  
											for unreg ipv4 mcast to cpu*/
										CPSS_PACKET_CMD_TRAP_TO_CPU_E);
		cpssDxChBrgVlanUnkUnregFilterSet(i,vlanId, \
										CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IP_MCAST_E,\
										CPSS_PACKET_CMD_DROP_HARD_E);

		/*Trap NON IPv4 BCast*/
		cpssDxChBrgVlanUnkUnregFilterSet(i,vlanId, \
										CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IPV4_BCAST_E,\
										CPSS_PACKET_CMD_TRAP_TO_CPU_E);
		
		/*drop IPv4 Igmp*/
		cpssDxChBrgVlanIgmpSnoopingEnable(i,vlanId,NPD_FALSE);

		/*disable mirror rx analyzerPort*/
		cpssDxChBrgVlanIngressMirrorEnable(i,vlanId, NPD_FALSE);
		/*disable Ipv6 Icmp to Cpu*/
		cpssDxChBrgVlanIpV6IcmpToCpuEnable(i,vlanId, NPD_FALSE);

		/*enable IpCtrl trap to Cpu
		cpssDxChBrgVlanIpCntlToCpuSet(i, vlanId, CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E);*/
		/*disable Ipm Bridge*/
		cpssDxChBrgVlanIpmBridgingEnable(i,vlanId, CPSS_IP_PROTOCOL_IPV4V6_E, NPD_FALSE);

		/*IPv4/6 route set default*/
	}

#endif
#ifdef DRV_LIB_BCM
    rc = nam_asic_vlan_entry_active(product_id, vlanId);
    if(0 == rc){
       nam_syslog_dbg("create vlan %d on asic success!\n",vlanId);
       ret = (unsigned long)rc;
	}
	else{
       nam_syslog_err("create vlan %d on asic failed,ret %d!\n",vlanId,rc);
	   ret = (unsigned long)rc;
	}
#endif
	return ret;
}
#if 0
/***************************************
*Func: nam_vlan_trunk_member_get
*Params:
*		trunkId--[1,127]
*		memberCount--[0,8]
*		memberArray[]--[0,23]:portNum
*Call: 
*		cpssDxChTrunkTableEntryGet
*Return:
*		VLAN_CONFGI_SUCCESS
*		VLAN_CONFGI_FAIL
*Comment:
*		Only support dev0 Now.	
*
****************************************/
unsigned int nam_asic_trunk_member_get
(
	unsigned short trunkId,
	unsigned int *memberCount,
	unsigned char memberArray[TRUNK_MEMBER_NUM_MAX]
)
{
	unsigned long ret;
	unsigned int  i,devNum = 0;
	CPSS_TRUNK_MEMBER_STC devport[TRUNK_MEMBER_NUM_MAX];
	
	ret = cpssDxChTrunkTableEntryGet(devNum, trunkId, memberCount, devport);
	if(0 == ret) {
		for (i=0;i<=(*memberCount);i++) {
			memberArray[i] = devport[i].portNum;
		}
	}
	return ret;
}
#endif

/*can return DRV_VLAN_PORT_BMP_S port bitmap*/ 
unsigned int nam_asic_vlan_get_drv_port_bmp
(
	unsigned short	vlanId,
	struct DRV_VLAN_PORT_BMP_S *mbrBmp,
	struct DRV_VLAN_PORT_BMP_S *tagBmp
)
{
	unsigned long ret = 0;
		
#ifdef DRV_LIB_CPSS
	unsigned char devNum = 0;
	struct DRV_VLAN_PORT_BMP_S MbrBmpTmp,TagBmpTmp;
	struct brg_vlan_drv_info_s vlanInfoPtr ;
	unsigned long	isValidPtr;
	CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmd;
	unsigned int devCount = nam_asic_get_instance_num();

	memset(&MbrBmpTmp,0,sizeof(struct DRV_VLAN_PORT_BMP_S));	
	memset(&TagBmpTmp,0,sizeof(struct DRV_VLAN_PORT_BMP_S));
	memset(&vlanInfoPtr,0,sizeof(struct brg_vlan_drv_info_s));
	memset(&portsTaggingCmd,0,sizeof(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC));
	for(devNum = 0;devNum < devCount;devNum++){
#ifdef DRV_LIB_CPSS_3_4	
		ret = cpssDxChBrgVlanEntryRead(devNum,vlanId,&MbrBmpTmp,&TagBmpTmp,&vlanInfoPtr,&isValidPtr,&portsTaggingCmd);
#else
		ret = cpssDxChBrgVlanEntryRead(devNum,vlanId,&MbrBmpTmp,&TagBmpTmp,&vlanInfoPtr,&isValidPtr);
#endif
		if (0 == ret) {
			if (isValidPtr) {
				ret = VLAN_CONFIG_SUCCESS;
				mbrBmp->ports[devNum] = MbrBmpTmp.ports[0]^TagBmpTmp.ports[0];
				tagBmp->ports[devNum] = TagBmpTmp.ports[0];
			}
			else {
				ret = VLAN_CONFIG_SUCCESS +1;
			}
		}
		else {
			osPrintfDbg("cpssDxChBrgVlanEntryRead error,ret %#x\n",ret);
			ret = VLAN_CONFIG_FAIL;
		}
	}
#endif
	return ret;
}

/******************************************
*Func: nam_asic_convert_mbrbmp_slotportbmp
*Params: 
*		IN: struct DRV_VLAN_PORT_BMP_S mbrBmp,
*		OUT:unsigned int *mbrSlotPortBmp
*Descp: convert DRV port Bitmap to Slot/Port
*		Bitmap.
*
*******************************************/
unsigned int nam_asic_convert_mbrbmp_slotportbmp
(
	struct DRV_VLAN_PORT_BMP_S mbrBmp,
	unsigned int *mbrSlotPortBmp
)
{		
#ifdef DRV_LIB_CPSS
	unsigned int i,devNum = 0,slotNo,localPortNo;
	unsigned int portNum = 0;
	unsigned int retBmp = 0;
	unsigned int product_id = PRODUCT_ID_NONE;

	product_id = npd_query_product_id();
	mbrBmp.ports[0] &= 0x0ffffff;
	for(i = 0;i<64;i++) {
		portNum = 1<<(i%32);
		if(mbrBmp.ports[i/32] & portNum) {
			if(NAM_ERR_NONE !=nam_get_slotport_by_devport(devNum,i,product_id,&slotNo,&localPortNo)) {
				 syslog_ax_nam_vlan_err("get slot port for dev %d port %d (untagged) error",devNum,i);
				return NAM_ERR_NO_MATCH;    
				/*port26 lead to NO match error,so can NOT return.Or untagBmpTmp>port[0]&0x3ffffff.*/
			}
			 nam_syslog_dbg("find untagged vlan Member : dev %d port %d on %d/%d",devNum,i,slotNo,localPortNo);
			retBmp |= 1<<((slotNo-1)*8+localPortNo);
		}
	}
	*mbrSlotPortBmp = retBmp;
#endif
	return NPD_TRUE;
}
/******************************************
*
*
*
*
*
*
*******************************************/
/*can return port bitmap*/ 
/*Get Slot_Port BitMap*/
unsigned int nam_asic_vlan_get_port_members_bmp
(
	unsigned short	vlanId,
	unsigned int	*untagBmp,
	unsigned int	*tagBmp
)
{
	unsigned long ret = 0;
		
#ifdef DRV_LIB_CPSS
	unsigned char devNum = 0;
	struct DRV_VLAN_PORT_BMP_S mbrBmpTmp,tagBmpTmp, untagBmpTmp;
	struct brg_vlan_drv_info_s vlanInfoPtr ;
	unsigned long	isValidPtr;
	unsigned int tmpBmp = 0;
	CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmd;

	memset(&mbrBmpTmp,0,sizeof(struct DRV_VLAN_PORT_BMP_S));
	memset(&tagBmpTmp,0,sizeof(struct DRV_VLAN_PORT_BMP_S));	
	memset(&untagBmpTmp,0,sizeof(struct DRV_VLAN_PORT_BMP_S));
#ifdef DRV_LIB_CPSS_3_4
	ret = cpssDxChBrgVlanEntryRead(devNum,vlanId,&mbrBmpTmp,&tagBmpTmp,&vlanInfoPtr,&isValidPtr,&portsTaggingCmd);
#else
	ret = cpssDxChBrgVlanEntryRead(devNum,vlanId,&mbrBmpTmp,&tagBmpTmp,&vlanInfoPtr,&isValidPtr);
#endif
	if (0 == ret) {
		if (isValidPtr) {
			 nam_syslog_dbg("vlanId %d :mbrBmp.ports[0] 0x%x,mbrBmp.ports[1] 0x%x\n",\
															vlanId,\
															mbrBmpTmp.ports[0],\
															mbrBmpTmp.ports[1]);
			 nam_syslog_dbg("vlanId %d :tagBmp.ports[0] 0x%x,tagBmp.ports[1] 0x%x\n",\
															vlanId,\
															tagBmpTmp.ports[0],\
															tagBmpTmp.ports[1]);
			/* TODO: convert HW untagged port bitmap to SW bitmap*/
			untagBmpTmp.ports[0] = mbrBmpTmp.ports[0]^tagBmpTmp.ports[0];
			untagBmpTmp.ports[1] = mbrBmpTmp.ports[1]^tagBmpTmp.ports[1];
			nam_syslog_dbg("vlanId %d :untagBmp.ports[0] 0x%x,untagBmp.ports[1] 0x%x\n",\
															vlanId,\
															untagBmpTmp.ports[0],\
															untagBmpTmp.ports[1]);

			nam_asic_convert_mbrbmp_slotportbmp(untagBmpTmp,&tmpBmp);
			*untagBmp = tmpBmp;
			 nam_syslog_dbg("vlan %d :untagBmp %#0X passing to NPD.\n",vlanId,*untagBmp);
			tmpBmp = 0;
			nam_asic_convert_mbrbmp_slotportbmp(tagBmpTmp,&tmpBmp);
			*untagBmp = tmpBmp;
			ret = VLAN_CONFIG_SUCCESS;
		}
		else {
			ret = VLAN_CONFIG_SUCCESS +1;
		}
	}
	else {
		ret = VLAN_CONFIG_FAIL;
	}	
#endif

#ifdef DRV_LIB_BCM
	unsigned long numOfPp = 0;
	int i = 0;
	bcm_pbmp_t pbmp,ut_pbmp;

	SOC_PBMP_CLEAR(pbmp);
	SOC_PBMP_CLEAR(ut_pbmp);

	numOfPp = nam_asic_get_instance_num();
	for (i=0; i<numOfPp; i++) {
		/*get tag ports and untag port from special vlan*/		
		ret = bcm_vlan_port_get(i,vlanId,&pbmp,&ut_pbmp);
		if(0 == ret){
			ret = VLAN_CONFIG_SUCCESS;
		}else{
			ret = VLAN_CONFIG_FAIL;
			return ret;
		}
		tagBmp[i] = pbmp.pbits[0];
		untagBmp[i] = ut_pbmp.pbits[0];
		syslog_ax_nam_vlan_dbg("vlan %d ports %#x untagports %#x\n",
						vlanId,tagBmp[i],untagBmp[i]);			
	}
#endif
	return ret;
}

/*never used*/
#if 0
/*show a special vlan  membership--slot/port*/
unsigned int nam_asic_vlan_get_port_members
(
	unsigned short	vlanId,
	unsigned int	*untagmbrIndxArr,
	unsigned int	*tagmbrIndxArr
	
)
{
		
#ifdef DRV_LIB_CPSS
	printf("Nam_vlan652::Enter nam asic get vlan member...\n");
	unsigned long ret;
	//unsigned int	i;
	unsigned char devNum = 0;
	struct DRV_VLAN_PORT_BMP_S untagmbrBmp;
	struct DRV_VLAN_PORT_BMP_S tagmbrBmp;
	struct DRV_VLAN_PORT_BMP_S tmpBmp;
	struct brg_vlan_drv_info_s vlanInfoPtr ;
	unsigned long	isValidPtr;


	ret = cpssDxChBrgVlanEntryRead(devNum,vlanId,&untagmbrBmp,&tagmbrBmp,&vlanInfoPtr,&isValidPtr);
	if (0 == ret) {
		if (isValidPtr) {
			//printf("\nNam_vlan664::show_vlan_port_members::show vlan %d success.\n",vlanId);
			//printf("Nam_vlan665::show_vlan_port_members::port member bit map %x\n",untagmbrBmp.ports[0]);
			//printf("Nam_vlan666::show_vlan_port_members::port member bit map %x\n",tagmbrBmp.ports[0]);
			//printf("Nam_vlan::show_vlan_port_members::vlan entry  is %s\n",isValidPtr?"Valid":"Invalid");
			tmpBmp.ports[0] = untagmbrBmp.ports[0]&tagmbrBmp.ports[0]; //tagged port bitmap
			tmpBmp.ports[1] = untagmbrBmp.ports[1]&tagmbrBmp.ports[1];
			untagmbrBmp.ports[0] = untagmbrBmp.ports[0]^tagmbrBmp.ports[0];// untagged port bitmap
			untagmbrBmp.ports[1] = untagmbrBmp.ports[1]^tagmbrBmp.ports[1];
			tagmbrBmp.ports[0] = tmpBmp.ports[0];
			tagmbrBmp.ports[1] = tmpBmp.ports[1];
			//printf("Nam_vlan675::nam_asic_vlan_get_port_members::untag member port bitmap0 %0x\n",untagmbrBmp.ports[0]);
			//printf(", bitmap1 %0x\n",untagmbrBmp.ports[1]);
			//printf("Nam_vlan677::nam_asic_vlan_get_port_members::tag member port bitmap0 %0x\n",tagmbrBmp.ports[0]);
			//printf(", bitmap1 %0x\n",tagmbrBmp.ports[1]);
			/*for(i=0;i<=NAM_PP_LOGIC_PORT_NUM_MAX;i++)*/
			//printf("Nam_vlan680:untagmbrIndexArr %p\n",untagmbrIndxArr);
			//printf("Nam_vlan681:tagmbrIndexArr %p\n",tagmbrIndxArr);
			
			ret = (unsigned long)nam_vlan_convert_port_bitmap_global_index(&untagmbrBmp,untagmbrIndxArr);
			if(0 != ret) {
				printf("Nam_vlan::convert_untag_port_bitmap_global_index:Erro & retVal %ld\n",ret);
				return ret;
			}
			printf("Nam_vlan::convert_untagport_bitmap_global_index:OK  & retVal %ld\n",ret);
			ret = (unsigned long)nam_vlan_convert_port_bitmap_global_index(&tagmbrBmp,tagmbrIndxArr);
			if(0 != ret) {
				printf("Nam_vlan::convert_tag_port_bitmap_global_index: Erro & retVal %ld\n",ret);
				return ret;
			}
			printf("Nam_vlan::convert_tagport_bitmap_global_index:OK & retVal %ld\n",ret);
			ret = VLAN_CONFIG_SUCCESS;
		}
		else {
			printf("Npd_vlan::show_vlan_port_members:: vlan entry %d is not valid.\n",vlanId);
			ret = VLAN_CONFIG_SUCCESS;
		}
	}
	else {
		printf("Npd_vlan::show_vlan_port_member::show vlan entry %d fail. \n",vlanId);
		ret = VLAN_CONFIG_FAIL;
	}
	printf("Npd_vlan712::return Here,retVal %ld!\n",ret);
	return ret;
#endif	
}
#endif

unsigned int nam_vlan_na_msg_enable
(
    unsigned char devNum,
    unsigned short vlanId,
    unsigned int  enable
)
{
    unsigned int ret;
	
#ifdef DRV_LIB_CPSS	
	ret = cpssDxChBrgVlanNaToCpuEnable( devNum,vlanId,enable);
	if(0!=ret){
		return NAM_VLAN_FAIL;
	}
#endif	
	return NAM_VLAN_SUCCESS;
}

/*******************************************
*
*  IGMP Snooping  
*				configuration
*
*
********************************************/

unsigned int nam_asic_igmp_trap_set_by_vid
(
    unsigned char   devNum,
    unsigned short  vlanId,
    unsigned long 	enable
)
{
	unsigned long ret = BRG_MC_SUCCESS;
	
	if(1 == vlanId){
		/*default vlan trap igmp */
		#ifdef DRV_LIB_CPSS
		ret = cpssDxChBrgVlanIgmpSnoopingEnable(devNum, vlanId, enable);
		ret |= cpssDxChBrgVlanIpV6IcmpToCpuEnable(devNum, vlanId, enable);
		#endif
		#ifdef DRV_LIB_BCM
		#if 0
		bcm_vlan_control_vlan_t vlan_ctl;
		ret = bcm_vlan_control_vlan_get(devNum, vlanId, &vlan_ctl);
		if (enable) {
			vlan_ctl.flags &= 0xFFFFF7FF;
		}else if (!enable) {
			vlan_ctl.flags |= BCM_VLAN_IGMP_SNOOP_DISABLE;
		}
		ret = bcm_vlan_control_vlan_set(devNum, vlanId, vlan_ctl);
		#endif
		#endif
		if(0 != ret){
			nam_syslog_dbg("nam_asic_igmp_trap_set_by_vid: device %d, vlanid %d %s\n",
							devNum, vlanId, enable ? "enable" : "disable");
			ret = BRG_MC_FAIL;
		}
	}
	return ret;
}
unsigned int nam_asic_igmp_trap_set_by_devport
(
	unsigned short	vid,
	unsigned char   devNum,
    unsigned char  portNum,
    unsigned long 	enable
)
{
	unsigned long ret = BRG_MC_SUCCESS;	

	#ifdef DRV_LIB_CPSS
	/*for vid =1 (Default vlan),must not set trap on port.*/
	if (1 != vid) {
		nam_syslog_dbg("set vlan %d asic-port(%d,%d) igmp trap %s\n", vid ,devNum, portNum, enable ? "enable":"disable");
		ret = cpssDxChBrgGenIgmpSnoopEnable(devNum, portNum, enable);
		ret |= cpssDxChBrgGenIpV6IcmpTrapEnable(devNum, portNum, enable);
		if(0 != ret){
			ret = BRG_MC_FAIL;
		}
	}
	#endif
	#ifdef DRV_LIB_BCM
	/* enable/disable IGMP packet to CPU*/
	/*for vid =1(Default vlan), bcm asic not support vlan trap to cpu, so set trap on port.*/
	nam_syslog_dbg("set vlan %d asic-port(%d,%d) igmp trap %s\n", vid ,devNum, portNum, enable ? "enable":"disable");
	ret = bcm_switch_control_port_set(devNum, portNum, bcmSwitchIgmpPktToCpu, enable);
	if (ret != NAM_OK) {
		nam_syslog_err("bcm_switch_control_port_set: asic-port(%d/%d), %s IgmpPktToCpu error, ret %d\n",
						devNum, portNum, enable ? "enable" : "disable", ret);
		ret = BRG_MC_FAIL;
	}

	nam_syslog_dbg("bcm_switch_control_port_set: vlan %d asic-port(%d/%d), %s IgmpPktToCpu ok\n",
					vid, devNum, portNum, enable ? "enable" : "disable");
	#endif

	return ret;
}
unsigned int nam_mc_entry_read
(
   unsigned char devNum,
   unsigned short vidx,
   CPSS_PORTS_BMP_STC * portBitmapPtr
)
{
	#ifdef DRV_LIB_CPSS
	return cpssDxChBrgMcEntryRead(devNum,vidx, portBitmapPtr);
	#endif
	#ifdef DRV_LIB_BCM
	unsigned int ret = NAM_OK;
	unsigned int unit = 0;
	unsigned int dev_num = 0;
	unsigned int bit = 0;
	unsigned int wordOffset = 0;
	unsigned int slotNo = 0;
	unsigned int localPortNo = 0;
	unsigned int product_id = PRODUCT_ID_NONE;
	bcm_port_t port;
	bcm_mcast_addr_t mcaddr;
	CPSS_PORTS_BMP_STC tmpportBitmap;
	int l2mc_index = 0;

	memset(&mcaddr, 0, sizeof(bcm_mcast_addr_t));
	memset(&tmpportBitmap, 0, sizeof(CPSS_PORTS_BMP_STC));

	l2mc_index = vidx;
	product_id = npd_query_product_id();

	dev_num = nam_asic_get_instance_num();
	for (unit = 0; unit < dev_num; unit ++) {
		ret = _bcm_xgs3_mcast_index_port_get(unit, l2mc_index, &mcaddr);
		if (0 != ret) {
			nam_syslog_err("get vidx %d multicast-group's members on unit %d Error, ret %d\n",
							l2mc_index,	unit, ret);
			return NAM_ERR;
		}

		/* remove port 4, 5 from pbmp*/
		BCM_PBMP_PORT_REMOVE(mcaddr.pbmp, 4);
		BCM_PBMP_PORT_REMOVE(mcaddr.pbmp, 5);
		
		if (!BCM_PBMP_IS_NULL(mcaddr.pbmp)) {
			BCM_PBMP_ITER(mcaddr.pbmp, port) {
				ret = nam_get_slotport_by_devport(devNum, port, product_id, &slotNo, &localPortNo);
				if (ret != 0) {
					syslog_ax_nam_vlan_err("get slotport(%d/%d) by devport(%d/%d) on unit %d error,ret %d\n",
											slotNo, localPortNo, devNum, port, unit, ret);
					return NAM_ERR;
				}

				wordOffset = ((slotNo - 1) * 8 + localPortNo) / 32;
				bit = ((slotNo - 1) * 8 + localPortNo) % 32;	
				if (1 >= wordOffset) {
					tmpportBitmap.ports[wordOffset] |= (1 << bit);
				}
			}
		}
	}

	portBitmapPtr->ports[0] = tmpportBitmap.ports[0];
	portBitmapPtr->ports[1] = tmpportBitmap.ports[1];

	return ret;
	#endif
}
unsigned int nam_asic_vlan_ipv4_mc_enalbe
(
	unsigned char devNum,	
	unsigned short vlanId,	
	unsigned long enable
)
{
	unsigned long ret = VLAN_CONFIG_SUCCESS;
		
	nam_syslog_dbg("set device %d vlan %d ipv4v6 mc %s\n",devNum,vlanId,enable ? "enable":"disable");

	#ifdef DRV_LIB_CPSS
	CPSS_IP_PROTOCOL_STACK_ENT ipVer;
	ipVer = CPSS_IP_PROTOCOL_IPV4V6_E;
	
	ret = cpssDxChBrgVlanIpmBridgingEnable(devNum,vlanId, ipVer, enable);
	if(0 != ret){
		ret = VLAN_CONFIG_FAIL;
	}
	#endif
	#ifdef DRV_LIB_BCM
	int unit = 0;
	unsigned int num_asic = 0;
	bcm_vlan_control_vlan_t vlan_ctl;

	num_asic = nam_asic_get_instance_num();

	for (unit = 0; unit < num_asic; unit++) {
		memset(&vlan_ctl, 0, sizeof(bcm_vlan_control_vlan_t));
		
		ret = bcm_esw_vlan_control_vlan_get(unit, vlanId, &vlan_ctl);
		if (ret != 0) {
			nam_syslog_err("get device %d vlan %d %s mc mode error\n",
							unit, vlanId);
			return VLAN_CONFIG_FAIL;
		}
		nam_syslog_dbg("get mc mode:flag %x unit %d vlan %d\n",
						vlan_ctl.flags, unit, vlanId);

		if (enable) {
			vlan_ctl.flags |= BCM_VLAN_IP4_MCAST_DISABLE;
		}
		else {
			vlan_ctl.flags &= (~BCM_VLAN_IP4_MCAST_DISABLE);
		}
		ret = bcm_esw_vlan_control_vlan_set(unit, vlanId, vlan_ctl);
		if (ret != 0) {
			nam_syslog_err("%s device %d vlan %d mc mode error\n",
							enable ? "enable" : "disable",
							unit, vlanId);
			return VLAN_CONFIG_FAIL;
		}
		nam_syslog_dbg("set mc mode:flag %x unit %d vlan %d\n",
						vlan_ctl.flags, unit, vlanId);
	}
	#endif

	nam_syslog_dbg("return value :%ld\n",ret);

	return ret;
}

unsigned int nam_asic_vlan_ipv4_mcmode_set
(
	unsigned char devNum,
	unsigned short vlanId
)
{
	unsigned long ret = VLAN_CONFIG_SUCCESS;
		
	#ifdef DRV_LIB_CPSS
	CPSS_IP_PROTOCOL_STACK_ENT ipVer;
	CPSS_BRG_IPM_MODE_ENT ipmMode;

	ipVer = CPSS_IP_PROTOCOL_IPV4V6_E;
	ipmMode = CPSS_BRG_IPM_GV_E;

	 nam_syslog_dbg("set device %d vlan %d %s mc mode %s\n", \
				 	devNum,vlanId,(CPSS_IP_PROTOCOL_IPV4_E == ipVer) ? "IPv4": \
				 	(CPSS_IP_PROTOCOL_IPV6_E == ipVer) ? "IPv6": "IPv4v6", \
				 	(CPSS_BRG_IPM_SGV_E == ipmMode) ? "SGV":"GV");
	ret = cpssDxChBrgVlanIpmBridgingModeSet(devNum, vlanId, ipVer, ipmMode);
	if(0 != ret ){
		ret = VLAN_CONFIG_FAIL;
	}
	#endif
	#ifdef DRV_LIB_BCM
	#if 0
	int unit = 0;
	unsigned int num_asic = 0;
	bcm_vlan_control_vlan_t vlan_ctl;

	num_asic = nam_asic_get_instance_num();

	for (unit = 0; unit < num_asic; unit++) {
		memset(&vlan_ctl, 0, sizeof(bcm_vlan_control_vlan_t));

		ret = _bcm_xgs3_vlan_control_vlan_get(unit, vlanId, &vlan_ctl);
		if (ret != 0)
		{
			nam_syslog_err("get device %d vlan %d %s mc mode error\n",
							unit, vlanId);
			return VLAN_CONFIG_FAIL;
		}
		nam_syslog_dbg("get device %d vlan %d forward mode %d flag %x\n",
						unit, vlanId, 
						((vlan_ctl.forwarding_mode == bcmVlanForwardBridging) ? 0 :
							((vlan_ctl.forwarding_mode == bcmVlanForwardSingleCrossConnect) ? 1 : 2)),
						vlan_ctl.flags);

		vlan_ctl.forwarding_mode = bcmVlanForwardBridging;

		ret = _bcm_xgs3_vlan_control_vlan_set(unit, vlanId, vlan_ctl);
		if (ret != 0)
		{
			nam_syslog_err("set device %d vlan %d %s forward mode <DMAC, VID> error\n",
							unit, vlanId);
			return VLAN_CONFIG_FAIL;
		}
		nam_syslog_dbg("set device %d vlan %d mc mode <DMAC, VID>\n",
						unit, vlanId);
	}
	#endif
	#endif

	return ret;
}

unsigned int nam_asic_vlan_igmp_enable_drop_unmc( unsigned char devNum,unsigned short vlanId)
{
	unsigned long ret = 0;
		
	#ifdef DRV_LIB_CPSS
	ret = cpssDxChBrgVlanUnkUnregFilterSet(devNum,vlanId, \
									CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_MCAST_E,\
									CPSS_PACKET_CMD_DROP_HARD_E);
	if(0 != ret )
		return VLAN_CONFIG_FAIL;
	ret = cpssDxChBrgVlanUnkUnregFilterSet(devNum,vlanId, \
									CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV6_MCAST_E,\
									CPSS_PACKET_CMD_DROP_HARD_E);
	if(0 != ret )
		return VLAN_CONFIG_FAIL;

	ret = cpssDxChBrgVlanUnkUnregFilterSet(devNum,vlanId, \
									CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IP_MCAST_E,\
									CPSS_PACKET_CMD_DROP_HARD_E);
	if(0 != ret )
		return VLAN_CONFIG_FAIL;
	 nam_syslog_dbg("set vlan %d unkown & unregister Multicast packet Drop OK.\n",vlanId);
	#endif
	#ifdef DRV_LIB_BCM
	int unit = 0;
	unsigned int num_asic = 0;
	bcm_vlan_mcast_flood_t mode;

	num_asic = nam_asic_get_instance_num();

	for (unit = 0; unit < num_asic; unit++) {
		mode = BCM_VLAN_MCAST_FLOOD_NONE;
		ret = bcm_vlan_mcast_flood_set(unit, vlanId, mode);
		if (0 != ret) {
			nam_syslog_dbg("set PFM BCM_VLAN_MCAST_FLOOD_NONE on unit %d vlan %d error!",
							unit, vlanId);
			return VLAN_CONFIG_FAIL;
		}
		nam_syslog_dbg("set PFM BCM_VLAN_MCAST_FLOOD_NONE on unit %d vlan %d, unkown Multicast packet Drop!",
						unit, vlanId);
	}
	#endif

	return VLAN_CONFIG_SUCCESS;
}

unsigned int nam_asic_vlan_igmp_enable_forword_unmc( unsigned char devNum,unsigned short vlanId)
{
	unsigned long ret = 0;
		
	#ifdef DRV_LIB_CPSS
	ret = cpssDxChBrgVlanUnkUnregFilterSet(devNum,vlanId, \
									CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_MCAST_E,\
									CPSS_PACKET_CMD_FORWARD_E);
	if(0 != ret )
		return VLAN_CONFIG_FAIL;
	ret = cpssDxChBrgVlanUnkUnregFilterSet(devNum,vlanId, \
									CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV6_MCAST_E,\
									CPSS_PACKET_CMD_FORWARD_E);
	if(0 != ret )
		return VLAN_CONFIG_FAIL;

	ret = cpssDxChBrgVlanUnkUnregFilterSet(devNum,vlanId, \
									CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IP_MCAST_E,\
									CPSS_PACKET_CMD_FORWARD_E);
	if(0 != ret )
		return VLAN_CONFIG_FAIL;
	 nam_syslog_dbg("set vlan %d unkown & unregister Multicast packet Forward OK.\n", vlanId);
	#endif
	#ifdef DRV_LIB_BCM
	int unit = 0;
	unsigned int num_asic = 0;
	bcm_vlan_mcast_flood_t mode;

	num_asic = nam_asic_get_instance_num();

	for (unit = 0; unit < num_asic; unit++) {
		mode = BCM_VLAN_MCAST_FLOOD_UNKNOWN;
		ret = bcm_vlan_mcast_flood_set(unit, vlanId, mode);
		if (0 != ret) {
			nam_syslog_dbg("set PFM BCM_VLAN_MCAST_FLOOD_UNKNOWN on unit %d vlan %d error!",
							unit, vlanId);
			return VLAN_CONFIG_FAIL;
		}
		nam_syslog_dbg("set PFM BCM_VLAN_MCAST_FLOOD_UNKNOWN on unit %d vlan %d, unkown Multicast packet flood in vlan!",
						unit, vlanId);
	}
	#endif

	return VLAN_CONFIG_SUCCESS;
}

unsigned int nam_asic_l2mc_member_add
(
	unsigned char devNum,
	unsigned short vidx,
	unsigned char portNum,
	unsigned int group_ip,
	unsigned short vlanId
)
{
	unsigned long ret = 0;
		
	#ifdef DRV_LIB_CPSS
	ret = cpssDxChBrgMcMemberAdd(devNum,vidx,portNum);
	if(0 != ret){
		 syslog_ax_nam_vlan_err("cpssDxChBrgMcMemberAdd::devNum %d,vidx %d,portNum %d ERROR!\n",devNum,vidx,portNum);
		ret = BRG_MC_FAIL;
	}
	 syslog_ax_nam_vlan_dbg("cpssDxChBrgMcMemberAdd::devNum %d,vidx %d,portNum %d OK!\n",devNum,vidx,portNum);
	#endif
	#ifdef DRV_LIB_BCM
	int unit = 0;
	unsigned int i;
	unsigned char dmip[4]={0};
	unsigned int dip = 0x0;
	bcm_mcast_addr_t mcaddr;
	bcm_mac_t mcMac = {0x01,0x00,0x5e,0x7f,0xff,0xff};

	memset(&mcaddr, 0, sizeof(bcm_mcast_addr_t));

	dip = htonl(group_ip);
	syslog_ax_nam_vlan_dbg("multicast group-ip 0x%x\n", dip);

	for (i = 0; i < 4; i++) {
		dmip[i] = (dip >> (i * 8)) & 0xff; /*group_ip -->dip*/
	}
	mcMac[3] &= dmip[2];
	mcMac[4] &= dmip[1];
	mcMac[5] &= dmip[0];

	syslog_ax_nam_vlan_dbg("multicast group-addr %02x:%02x:%02x:%02x:%02x:%02x\n",
							mcMac[0],mcMac[1],mcMac[2],
							mcMac[3],mcMac[4],mcMac[5]);

	for (i = 0; i < 6; i++) {
		mcaddr.mac[i] = mcMac[i];
	}

	mcaddr.l2mc_index = vidx;
	mcaddr.vid = vlanId;

	BCM_PBMP_PORT_ADD(mcaddr.pbmp, portNum);
	unit = devNum;

	syslog_ax_nam_vlan_dbg("mcaddr.mac %02x:%02x:%02x:%02x:%02x:%02x\n",
				mcaddr.mac[0], mcaddr.mac[1], mcaddr.mac[2],
				mcaddr.mac[3], mcaddr.mac[4], mcaddr.mac[5]);

	ret = bcm_xgs3_mcast_port_add(unit, &mcaddr);
	if (ret != 0) {
		syslog_ax_nam_vlan_err("add port %d to multicast-group %02x:%02x:%02x:%02x:%02x:%02x"	\
								" on unit %d vlanId %d Error, ret %d\n",
								portNum,
								mcMac[0],mcMac[1],mcMac[2],
								mcMac[3],mcMac[4],mcMac[5],
								devNum, vlanId, ret);
		ret = BRG_MC_FAIL;
		return ret;
	}

	
	syslog_ax_nam_vlan_dbg("bcm_mcast_port_add::devNum %d,vidx %d,portNum %d OK!\n",
	 						devNum, vidx, portNum);
	#endif
	return ret;
}
unsigned int nam_asic_l2mc_member_del
(
	unsigned char devNum,
	unsigned short vidx,
	unsigned char portNum,
	unsigned int group_ip,
	unsigned short vlanId,
	unsigned char *hasMbr
)
{
	unsigned long ret = BRG_MC_SUCCESS;
	unsigned char mbrLeft = TRUE;
	
	#ifdef DRV_LIB_CPSS
	CPSS_PORTS_BMP_STC portBitmapPtr ;
	portBitmapPtr.ports[0] = 0;
	portBitmapPtr.ports[1] = 0;
	ret = cpssDxChBrgMcMemberDelete(devNum,vidx,portNum);
	if(0 == ret){
		ret = cpssDxChBrgMcEntryRead(devNum, vidx, &portBitmapPtr);
		if(0 == ret)
		{ 
			if(0 == portBitmapPtr.ports[0]){
				cpssDxChBrgMcGroupDelete(devNum, vidx);
				mbrLeft = FALSE;
			}
			else{
				mbrLeft = TRUE;
			}
		}
		else{
			ret = BRG_MC_FAIL;
		}
	}
	else{
		 syslog_ax_nam_vlan_err("cpssDxChBrgMcMemberDelete::devNum %d,vidx %d,portNum %d ERROR!\n",devNum,vidx,portNum);
		ret = BRG_MC_FAIL;
	}
	#endif
	#ifdef DRV_LIB_BCM
	int unit = 0;
	unsigned int i;
	unsigned char dmip[4]={0};
	unsigned int dip = 0x0;
	bcm_mcast_addr_t mcaddr;
	bcm_mcast_addr_t tmpmcaddr0, tmpmcaddr1;
	unsigned int flag0 = 0, flag1 = 0;
	bcm_mac_t mcMac = {0x01,0x00,0x5e,0x7f,0xff,0xff};

	memset(&mcaddr, 0, sizeof(bcm_mcast_addr_t));
	memset(&tmpmcaddr0, 0, sizeof(bcm_mcast_addr_t));
	memset(&tmpmcaddr1, 0, sizeof(bcm_mcast_addr_t));

	dip = htonl(group_ip);
	syslog_ax_nam_vlan_dbg("multicast group-ip 0x%x\n", dip);

	for (i = 0; i < 4; i++) {
		dmip[i] = (dip >> (i * 8)) & 0xff; /*group_ip -->dip*/
	}
	mcMac[3] &= dmip[2];
	mcMac[4] &= dmip[1];
	mcMac[5] &= dmip[0];

	for (i = 0; i < 6; i++) {
		mcaddr.mac[i] = mcMac[i];
	}

	mcaddr.l2mc_index = vidx;
	mcaddr.vid = vlanId;
	BCM_PBMP_PORT_ADD(mcaddr.pbmp, portNum);
	unit = devNum;

	/*ret = bcm_xgs3_mcast_port_remove(unit, &mcaddr);*/
	/*ret = bcm_mcast_port_remove(unit, &mcaddr);*/
	ret = bcm_xgs3_mcast_port_remove(unit, &mcaddr);
	if (ret == 0) {
		ret = bcm_xgs3_mcast_port_get(0, mcMac, vlanId, &tmpmcaddr0);
		if ((0 != ret) && (-7 != ret)) {
			syslog_ax_nam_vlan_err("get multicast-group %02x:%02x:%02x:%02x:%02x:%02x members"	\
									" on unit %d vlanId %d Error, ret %d\n",
									mcMac[0],mcMac[1],mcMac[2], mcMac[3],mcMac[4],mcMac[5],
									0, vlanId, ret);
			return BRG_MC_FAIL;
		}

		/* remove port 4, 5 from pbmp*/
		BCM_PBMP_PORT_REMOVE(tmpmcaddr0.pbmp, 4);
		BCM_PBMP_PORT_REMOVE(tmpmcaddr0.pbmp, 5);

		if (BCM_PBMP_IS_NULL(tmpmcaddr0.pbmp)) {
			flag0 = 1;			/* in device 0, multcast-group's member is null*/
		}else {
			flag0 = 0;
		}
		
		ret = bcm_xgs3_mcast_port_get(1, mcMac, vlanId, &tmpmcaddr1);
		if ((0 != ret) && (-7 != ret)) { 
			syslog_ax_nam_vlan_err("get multicast-group %02x:%02x:%02x:%02x:%02x:%02x members"	\
									" on unit %d vlanId %d Error, ret %d\n",
									mcMac[0],mcMac[1],mcMac[2], mcMac[3],mcMac[4],mcMac[5],
									0, vlanId, ret);
			return BRG_MC_FAIL;
		}

		/* remove port 4, 5 from pbmp*/
		BCM_PBMP_PORT_REMOVE(tmpmcaddr1.pbmp, 4);
		BCM_PBMP_PORT_REMOVE(tmpmcaddr1.pbmp, 5);

		if (BCM_PBMP_IS_NULL(tmpmcaddr1.pbmp)) {
			flag1 = 1;			/* in device 1, multcast-group's member is null*/
		}else {
			flag1 = 0;
		}

		if ((flag0 == 1) && (flag1 == 1)) {
			/* remove multicast-group in device 0*/
			ret = bcm_mcast_addr_remove(0, mcMac, vlanId);
			if ((0 != ret) && (-7 != ret)) { 
				syslog_ax_nam_vlan_err("delete null multicast-group %02x:%02x:%02x:%02x:%02x:%02x"	\
										" on unit %d vlanId %d Error, ret %d\n",
										mcMac[0],mcMac[1],mcMac[2],
										mcMac[3],mcMac[4],mcMac[5],
										0, vlanId, ret);
				/*return BRG_MC_FAIL;*/
			}
			/* remove multicast-group in device 1*/
			ret = bcm_mcast_addr_remove(1, mcMac, vlanId);
			if ((0 != ret) && (-7 != ret)) { 
				syslog_ax_nam_vlan_err("delete null multicast-group %02x:%02x:%02x:%02x:%02x:%02x"	\
										" on unit %d vlanId %d Error, ret %d\n",
										mcMac[0],mcMac[1],mcMac[2],
										mcMac[3],mcMac[4],mcMac[5],
										1, vlanId, ret);
				/*return BRG_MC_FAIL;*/
			}
			
			syslog_ax_nam_vlan_dbg("delete null multicast-group %02x:%02x:%02x:%02x:%02x:%02x"	\
									" on unit 0 and unit 1 vlanId %d\n",
									mcMac[0],mcMac[1],mcMac[2],
									mcMac[3],mcMac[4],mcMac[5],
									vlanId);

			mbrLeft = FALSE;
		}
		else{
			mbrLeft = TRUE;
		}
		ret = BRG_MC_SUCCESS;
	}
	else if(0 != ret){
		 syslog_ax_nam_vlan_err("bcm_mcast_port_remove::devNum %d,vidx %d,portNum %d ERROR, ret %d!\n",
		 						devNum, vidx, portNum, ret);
		return BRG_MC_FAIL;
	}
	syslog_ax_nam_vlan_dbg("bcm_mcast_port_remove::devNum %d,vidx %d,portNum %d OK!\n",
	 						devNum, vidx, portNum);
	#endif

	*hasMbr = mbrLeft;

	return ret;
}
unsigned int nam_asic_l2mc_group_del
(
	unsigned char devNum,
	unsigned short vidx,
	unsigned int   group_ip,
	unsigned short vlanId
)
{
	unsigned long ret = BRG_MC_SUCCESS;
		
	#ifdef DRV_LIB_CPSS
	ret = cpssDxChBrgMcGroupDelete(devNum, vidx);
	if(0 != ret){
		ret = BRG_MC_FAIL;
	}
	#endif
	#ifdef DRV_LIB_BCM
	int unit = 0;
	unsigned int i = 0;
	unsigned int num_asic= 0;
	unsigned char dmip[4]={0};
	unsigned int dip = 0x0;
	bcm_mac_t mcMac = {0x01,0x00,0x5e,0x7f,0xff,0xff};

	dip = htonl(group_ip);
	syslog_ax_nam_vlan_dbg("multicast group-ip 0x%x\n", dip);

	for (i = 0; i < 4; i++) {
		dmip[i] = (dip >> (i * 8)) & 0xff; /*group_ip -->dip*/
	}
	mcMac[3] &= dmip[2];
	mcMac[4] &= dmip[1];
	mcMac[5] &= dmip[0];

	num_asic = nam_asic_get_instance_num();

	for (unit = 0; unit < num_asic; unit++) {	
		ret = bcm_mcast_addr_remove(unit, mcMac, vlanId);
		if (0 != ret) {
			if (-7 == ret) {
				ret = BRG_MC_SUCCESS;
				continue;
			}else{
				syslog_ax_nam_vlan_err("delete null multicast-group %02x:%02x:%02x:%02x:%02x:%02x"	\
										" on unit %d vlanId %d Error, ret %d\n",
										mcMac[0],mcMac[1],mcMac[2],
										mcMac[3],mcMac[4],mcMac[5],
										unit, vlanId, ret);
				return ret;
			}
		}
	}
	#endif
	return ret;
}

unsigned int nam_asic_group_mbr_bmp
(
	unsigned char devNum,
	unsigned short vidx,
	unsigned int *groupMbrBmp
)
{
	 nam_syslog_dbg("Enter :nam_asic_group_mbr_bmp...\r\n");
	unsigned long ret = BRG_MC_SUCCESS;
#ifdef DRV_LIB_CPSS
	unsigned int i,tmpBmp = 0;
	unsigned int portNum = 0,slotNo = 0,localPortNo = 0;
	unsigned int product_id = PRODUCT_ID_NONE;
	CPSS_PORTS_BMP_STC mbrBmp;
	mbrBmp.ports[0] = 0;
	mbrBmp.ports[1] = 0;

	product_id = npd_query_product_id();
	ret = cpssDxChBrgMcEntryRead(devNum,vidx,&mbrBmp);
	if(0 == ret){
		mbrBmp.ports[0] &= 0x3ffffff;
		 nam_syslog_dbg("NAM>>cpssDxChBrgMcEntryRead :get mbrBmp.ports[0] = 0x%x,mbrBmp.ports[1] = 0x%x.\n",\
					mbrBmp.ports[0],mbrBmp.ports[1]);
	
		for(i = 0;i<64;i++) {
			portNum = 1<<(i%32);
			if(mbrBmp.ports[i/32] & portNum) {
				if(NAM_ERR_NONE !=nam_get_slotport_by_devport(devNum,i,product_id,&slotNo,&localPortNo)) {
					 syslog_ax_nam_vlan_err("get slot port for dev %d port %d  error",devNum,i);
					return NAM_ERR_NO_MATCH;    
					/*port26 lead to NO match error,so can NOT return.Or untagBmpTmp>port[0]&0x3ffffff.*/
				}
				 nam_syslog_dbg("NAM>>find group portmember: dev %d port %d on %d/%d",devNum,i,slotNo,localPortNo);
				tmpBmp |= 1<<((slotNo-1)*8+localPortNo);
			}
		}
		*groupMbrBmp = tmpBmp;
		 nam_syslog_dbg("Trans mbrBmp.port[0] 0x%x TO tmpBmp 0x%x,(groupMbrBmp 0x%x).\r\n",mbrBmp.ports[0],tmpBmp,*groupMbrBmp);
	}
	else {
		ret = BRG_MC_FAIL;
	}
#endif
	return ret;
}

unsigned int nam_asic_group_mbr_bmp_v1
(
	unsigned char devNum,
	unsigned short vidx,
	unsigned int   groupAddr,
	unsigned short vlanId,
	PORT_MEMBER_BMP* groupMbrBmp
)
{
	 nam_syslog_dbg("Enter :nam_asic_group_mbr_bmp...\r\n");
	unsigned long ret = BRG_MC_SUCCESS;
		
#ifdef DRV_LIB_CPSS
	unsigned int i = 0;
	unsigned int portNum = 0,slotNo = 0,localPortNo = 0;
	unsigned int product_id = PRODUCT_ID_NONE;
	unsigned int  tmpBmp[2];
	CPSS_PORTS_BMP_STC mbrBmp;
	mbrBmp.ports[0] = 0;
	mbrBmp.ports[1] = 0;

	memset(&tmpBmp,0,sizeof(tmpBmp));
	product_id = npd_query_product_id();
	ret = cpssDxChBrgMcEntryRead(devNum,vidx,&mbrBmp);
	if(0 == ret){
		mbrBmp.ports[0] &= 0x3ffffff;
		 nam_syslog_dbg("NAM>>cpssDxChBrgMcEntryRead :get mbrBmp.ports[0]=0x%x, mbrBmp.ports[1]=0x%x.\n",\
					mbrBmp.ports[0],mbrBmp.ports[1]);
	
		for(i = 0;i<64;i++) {
			portNum = 1<<(i%32);
			if(mbrBmp.ports[i/32] & portNum) {
				if(NAM_ERR_NONE !=nam_get_slotport_by_devport(devNum,i,product_id,&slotNo,&localPortNo)) {
					 syslog_ax_nam_vlan_err("get slot port for dev %d port %d  error",devNum,i);
					return NAM_ERR_NO_MATCH;    
					/*port26 lead to NO match error,so can NOT return.Or untagBmpTmp>port[0]&0x3ffffff.*/
				}
				 nam_syslog_dbg("NAM>>find group portmember: dev %d port %d on %d/%d",devNum,i,slotNo,localPortNo);
				#if 0
				tmpBmp[i/32] |= 1<<((slotNo-1)*8+localPortNo%32);
				#endif
				tmpBmp[localPortNo/32] |= 1<<(localPortNo%32);

			}
		}
		/* *groupMbrBmp = tmpBmp;*/
		groupMbrBmp->portMbr[0] = tmpBmp[0];
		groupMbrBmp->portMbr[1] = tmpBmp[1];
		nam_syslog_dbg("Trans mbrBmp.port[0] 0x%x TO tmpBmp[0] 0x%x, mbrBmp.port[1] 0x%x TO tmpBmp[1] 0x%x,(groupMbrBmp[0] 0x%x).\r\n",
		 				mbrBmp.ports[0],tmpBmp[0],
		 				mbrBmp.ports[1],tmpBmp[1],
		 				groupMbrBmp->portMbr[0]);
	}
	else {
		ret = BRG_MC_FAIL;
	}
	#endif
	#ifdef DRV_LIB_BCM
	unsigned int i = 0;
	unsigned int bit = 0;
	unsigned int wordOffset = 0;
	unsigned int unit = devNum;
	unsigned int slotNo = 0;
	unsigned int localPortNo = 0;
	unsigned char dmip[4]={0};
	unsigned int dip = 0x0;
	bcm_port_t port;
	bcm_mcast_addr_t mcaddr;
	bcm_mac_t mcMac = {0x01,0x00,0x5e,0x7f,0xff,0xff};

	unsigned int product_id = PRODUCT_ID_NONE;
	product_id = npd_query_product_id();

	memset(&mcaddr, 0, sizeof(bcm_mcast_addr_t));

	dip = htonl(groupAddr);
	syslog_ax_nam_vlan_dbg("multicast group-ip 0x%x\n", dip);

	for (i = 0; i < 4; i++) {
		dmip[i] = (dip >> (i * 8)) & 0xff; /*groupAddr -->dip*/
	}
	mcMac[3] &= dmip[2];
	mcMac[4] &= dmip[1];
	mcMac[5] &= dmip[0];
	
	ret = bcm_mcast_port_get(unit, mcMac, vlanId, &mcaddr);
	if ((ret != NAM_OK) && (ret != -7)) {
		syslog_ax_nam_vlan_err("get multicast-group %02x:%02x:%02x:%02x:%02x:%02x member bitmaps"	\
								" on device %d vlan %d error, ret %d\n",
								mcMac[0], mcMac[1], mcMac[2],
								mcMac[3], mcMac[4], mcMac[5],
								devNum, vlanId, ret);
		return BRG_MC_FAIL;
	}
	syslog_ax_nam_vlan_dbg("get multicast-group %02x:%02x:%02x:%02x:%02x:%02x member bitmaps %x untagbitmaps %x"	\
							" on device %d vlan %d ret %d\n",
							mcMac[0], mcMac[1], mcMac[2],
							mcMac[3], mcMac[4], mcMac[5],
							mcaddr.pbmp.pbits[0], mcaddr.ubmp.pbits[0],
							devNum, vlanId, ret);

	/* remove port 4, 5 from pbmp*/
	BCM_PBMP_PORT_REMOVE(mcaddr.pbmp, 4);
	BCM_PBMP_PORT_REMOVE(mcaddr.pbmp, 5);

	if (!BCM_PBMP_IS_NULL(mcaddr.pbmp)) {
		BCM_PBMP_ITER(mcaddr.pbmp, port) {
			ret = nam_get_slotport_by_devport(devNum, port, product_id, &slotNo, &localPortNo);
			if (ret != 0) {
				syslog_ax_nam_vlan_err("get slotport(%d/%d) by devport(%d/%d) in vlan %d error,ret %d\n",
										slotNo, localPortNo, devNum, port, vlanId, ret);
				return BRG_MC_FAIL;
			}

			wordOffset = ((slotNo - 1) * 8 + localPortNo) / 32;
			bit = ((slotNo - 1) * 8 + localPortNo) % 32;	
			if (1 >= wordOffset) {
				groupMbrBmp->portMbr[wordOffset] |= (1 << bit);
			}
		}
	}else {
		ret = -7; /* not found*/
	}
	
	syslog_ax_nam_vlan_dbg("groupMbrBmp->portMbr[%d] %x on device %d vlan %d ret %d\n",
						devNum, groupMbrBmp->portMbr[devNum],
						devNum, vlanId, ret);
	#endif
	return ret;
}

/*added by zhengcs*/

unsigned int nam_asic_vlan_autolearn_set
(
    unsigned char devNum,
    unsigned short vlanId,
    unsigned int  autolearn
)
{
    unsigned int ret = 0;
		
#ifdef DRV_LIB_CPSS
	ret = cpssDxChBrgVlanLearningStateSet(devNum, vlanId,autolearn);
	if(0 != ret){
        syslog_ax_nam_vlan_err("The vlan auto learn status set err: %d",ret);
		return ret;
	}
#endif
	return ret;
}

int nam_config_vlan_mtu
(
	unsigned char devNum,
	unsigned int mtuIndex,
	unsigned int mtuValue
)
{
	int ret = 0;
		
#ifdef DRV_LIB_CPSS	
	/* set bridge engine VLAN mtu value */
	ret = cpssDxChBrgVlanMruProfileValueSet(devNum,mtuIndex,mtuValue);
	if(0 != ret) {
		NAM_DEBUG(("%s :: config mtu error %d\n",__func__,ret));
	}
#endif
	return ret;
}

int nam_config_vlan_egress_filter
(
	unsigned char devNum,
	unsigned int isable
)
{
	int ret = 0;
		
#ifdef DRV_LIB_CPSS
	ret = cpssDxChBrgVlanEgressFilteringEnable(devNum,isable);
	if(0 != ret) {
		NAM_DEBUG(("%s :: config egress filter error %d\n",__func__,ret));
	}
#endif
	return ret;
}

/**********************************************
*Func: nam_config_route_mtu
*Params: 
*
*Call: 
*
*Return:
*
*Comment:
*
*
**********************************************/
int nam_config_route_mtu
(
	unsigned char devNum,
	unsigned int mtuIndex,
	unsigned int mtuValue
)
{
	int ret = 0;
		
#ifdef DRV_LIB_CPSS
	/* set IP route engine mtu value */
	ret = cpssDxChIpMtuProfileSet(devNum,mtuIndex,mtuValue);
	if(0 != ret) {
		NAM_DEBUG(("%s :: config route mtu index %d value %d error %d\n",__func__,mtuIndex,mtuValue,ret));
	}
#endif
	return ret;
}
int nam_config_vlan_filter
(
	unsigned short vlanId,
	unsigned int filterType,
	unsigned int enDis
)
{
	int i,numOfPp = 0,ret = 0;
		
#ifdef DRV_LIB_CPSS
	CPSS_PACKET_CMD_ENT cmd = 0;
	numOfPp = nam_asic_get_instance_num();
	NAM_DEBUG(("Pp devCount %d",numOfPp));
	if(enDis){
		cmd = CPSS_PACKET_CMD_FORWARD_E ;
	}
	else {
		cmd = CPSS_PACKET_CMD_DROP_HARD_E ;
	}
	
	for(i=0;i<numOfPp;i++){
		ret = cpssDxChBrgVlanUnkUnregFilterSet(i,vlanId,filterType,cmd);
		if(0 != ret) {
			NAM_DEBUG(("vlan filter set fail,retrun %d.",ret));
			break;
		}
	}
#endif
	return ret;
}

int nam_asic_bcast_rate_limiter_config(	unsigned int enDis )
{
	int i,numOfPp = 0,ret = 0;
		
#ifdef DRV_LIB_CPSS
    CPSS_NET_RX_CPU_CODE_ENT	cpuCode = CPSS_NET_ALL_CPU_OPCODES_E;
	CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC entryInfoPtr;

	unsigned int rateLimiterIndex, windowSize, pktLimit;
	
	numOfPp = nam_asic_get_instance_num();
	NAM_DEBUG(("Pp devCount %d",numOfPp));

	/****set cpu code, index & rate limiting Mode********/
	cpuCode = CPSS_NET_ARP_BC_TO_ME_E;
	entryInfoPtr.tc = 0;    /* code optimize: Using uninitialized value "entryInfoPtr.tc". zhangdi@autelan.com 2013-01-18 */
	entryInfoPtr.truncate = NPD_FALSE;
	entryInfoPtr.cpuCodeStatRateLimitIndex = 0;
	entryInfoPtr.cpuRateLimitMode = CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E;
	if(enDis){
		entryInfoPtr.cpuCodeRateLimiterIndex = 1;
	}
	else {
		entryInfoPtr.cpuCodeRateLimiterIndex = 0;/*CpuCode not bond any rateLimiter,rate of packet with this code will not limited.*/
	}
	/****set rateLimiterIndex, windowSize, & packetLimite**********/
	rateLimiterIndex = entryInfoPtr.cpuCodeRateLimiterIndex;
	windowSize = 1;/*(0x0b050000)Pre-Egress Global config Reg<ToCPURLWindowResolution>--Default value*/
	pktLimit = 1024;/**/
	for(i=0;i<numOfPp;i++){
		ret = cpssDxChNetIfCpuCodeTableSet(numOfPp, cpuCode, &entryInfoPtr);
		if(0 != ret){
			NAM_DEBUG(("set dev %d CpuCode Table Fail.",numOfPp));
			continue;
		}
		ret = cpssDxChNetIfCpuCodeRateLimiterTableSet(numOfPp, rateLimiterIndex, windowSize, pktLimit);
		if(0 != ret){
			NAM_DEBUG(("set dev %d CpuCodeRateLimiter Table Fail.",numOfPp));
			continue;
		}
	}
#endif
	return ret;
}

unsigned long nam_vlan_qinq_endis(unsigned char devNum,unsigned short	vlanId,unsigned char portNum,unsigned char isEnable,unsigned char isTag)
{

	unsigned long ret = 0;
#ifdef DRV_LIB_CPSS
	 CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT tagCmd;
	if(isEnable)
	{
		tagCmd = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E;
	}
	else
	{
		tagCmd = CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E;
	}
	if(!isTag)
	{
		tagCmd = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
	}
	ret = cpssDxChBrgVlanMemberSet(devNum,vlanId,portNum,1,isTag,tagCmd);

	
#endif
	return ret;
}

unsigned long nam_vlan_qinq_tpid_entry_set(unsigned char devNum,VLAN_DIRECTION_ENT direction,unsigned long entryIndex,unsigned short etherType)
{
	unsigned long ret = 0;
#ifdef DRV_LIB_CPSS
	ret = cpssDxChBrgVlanTpidEntrySet(devNum,direction,entryIndex,etherType);
#endif
	return ret;
}
unsigned long nam_vlan_qinq_tpid_entry_get(unsigned char devNum,VLAN_DIRECTION_ENT direction,unsigned long entryIndex,unsigned short *etherType)
{
	unsigned long ret = 0;
#ifdef DRV_LIB_CPSS
	ret = cpssDxChBrgVlanTpidEntryGet(devNum,direction,entryIndex,etherType);
#endif
	return ret;
}

unsigned long nam_vlan_qinq_port_ingress_tpid_set(unsigned char devNum,unsigned char portNum,CPSS_ETHER_MODE_ENT ethMode,unsigned long tpidBmp)
{
	unsigned long ret = 0;
#ifdef DRV_LIB_CPSS
	ret = cpssDxChBrgVlanPortIngressTpidSet(devNum,portNum,ethMode,tpidBmp);
#endif
	return ret;
}

unsigned long nam_vlan_qinq_port_egress_tpid_set(unsigned char devNum,unsigned char portNum,CPSS_ETHER_MODE_ENT ethMode,unsigned long tpidentryIndex)
{
	unsigned long ret = 0;
#ifdef DRV_LIB_CPSS
	 ret = cpssDxChBrgVlanPortEgressTpidSet(devNum,portNum,ethMode,tpidentryIndex);
#endif
	return ret;
}

unsigned long nam_port_qinq_tpid_set()
{
	int ret = 0;
#ifdef DRV_LIB_CPSS
 	ret = prvCpssDrvHwPpSetRegField(0,0x0F000440,0,32,0x249249);
	if(ret !=0)
	{
		syslog_ax_nam_vlan_err("qinq set register filed failed !\n");
		return ret;
	}
	ret = prvCpssDrvHwPpSetRegField(0,0x0F000444,0,32,0x249249);
	if(ret !=0)
	{
		syslog_ax_nam_vlan_err("qinq set register filed failed !\n");
		return ret;
	}
	ret = prvCpssDrvHwPpSetRegField(0,0x0F000448,0,32,0x249249);
	if(ret !=0)
	{
		syslog_ax_nam_vlan_err("qinq set register filed failed !\n");
		return ret;
	}
	ret = prvCpssDrvHwPpSetRegField(0,0x0F00044c,0,32,0x249249);
	if(ret !=0)
	{
		syslog_ax_nam_vlan_err("qinq set register filed failed !\n");
		return ret;
	}
#endif

	return ret;

}

unsigned int nam_vlan_config_phy_88x2140
(
	unsigned char devNum,
	unsigned char portNum
)
{
	int ret;
    ret = phy_88x2140_enable_set(devNum, portNum, 0);
    osTimerWkAfter(100);
	ret = phy_88x2140_enable_set(devNum, portNum, 1);
	return 0;	
}
unsigned int nam_vlan_config_serdes_power_states
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned char powerup
)
{   
	int ret;
	ret = cpssDxChPortSerdesPowerStatusSet(devNum, portNum, 2, 0x3, powerup);
	return 0;
}
unsigned int nam_vlan_config_12x_port_states
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned char link_up   
)
{
	int ret;
    if(link_up == 1)
    {
        ret = cpssDxChPortForceLinkDownEnableSet(devNum, portNum,0);
        ret = cpssDxChPortForceLinkPassEnableSet(devNum, portNum,1);
	}
	else
	{
        ret = cpssDxChPortForceLinkPassEnableSet(devNum, portNum,0);
		ret = cpssDxChPortForceLinkDownEnableSet(devNum, portNum,1);
	}
	return 0;
}
#ifdef __cplusplus
}
#endif

