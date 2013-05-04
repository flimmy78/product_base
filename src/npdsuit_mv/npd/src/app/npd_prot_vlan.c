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
* npd_prot_vlan.c
*
*
* CREATOR:
* 		hanhui@autelan.com
*
* DESCRIPTION:
* 		npd implement for protocol vlan
*
* DATE:
*		15/4/2010
*
* FILE REVISION NUMBER:
*  		$Revision: 1.6 $
*
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linux/if_ether.h"
#include "sysdef/npd_sysdef.h"
#include "dbus/npd/npd_dbus_def.h"

#include "npd/nbm/npd_bmapi.h"
#include "npd_log.h"
#include "util/npd_list.h"
#include "npd_vlan.h"
#include "npd_product.h"
#include "npd_c_slot.h"
#include "npd_eth_port.h"
#include "sysdef/returncode.h"
#include "npd_prot_vlan.h"


void npd_prot_vlan_init(void)
{
    unsigned char devNum = 0;
    unsigned int ret = 0;
    unsigned char entryNum = 0;
    unsigned short ethType = 0;
    unsigned int isValid = 0;
    unsigned short protValueArray [PROT_TYPE_UDF+1] = {ETH_P_IP,ETH_P_IPV6,ETH_P_PPP_DISC,ETH_P_PPP_SES,0};

	if(PRODUCT_ID_AX5K_E == PRODUCT_ID || PRODUCT_ID_AX5608== PRODUCT_ID){/*5612e not support*/
		syslog_ax_vlan_dbg("this produc do not support protocol vlan.\n");
		return;
	}

    for(entryNum = 0;entryNum < PROT_TYPE_UDF;entryNum++){
        nam_prot_vlan_set_entry_eth_type(devNum,entryNum,protValueArray[entryNum]);
        nam_prot_vlan_get_entry_eth_type(devNum,entryNum,&ethType,&isValid);        
        prot_vlan_entry_protocol[entryNum] = ethType;
    }
    entryNum = PROT_TYPE_UDF;
    nam_prot_vlan_set_entry_eth_type(devNum,entryNum,protValueArray[entryNum]);  
    nam_prot_vlan_disable_by_entry(devNum,entryNum);
    nam_prot_vlan_get_entry_eth_type(devNum,entryNum,&ethType,&isValid);
    prot_vlan_entry_protocol[entryNum] = ethType;
    
}
DBusMessage * npd_dbus_prot_vlan_port_enable_config(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;

	unsigned int ret = 0;
    unsigned int isEnable = 0;
    unsigned int eth_g_index = 0;
    unsigned char devNum = 0,portNum = 0;
    struct eth_port_s *eth_port = NULL;
    struct prot_vlan_ctrl_t * protVlanCtrl = NULL;
    unsigned char slotNo = 0,portNo = 0;
    unsigned int isRgmii = 0;
    
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		         DBUS_TYPE_UINT32,&eth_g_index,
		         DBUS_TYPE_UINT32,&isEnable,
				 DBUS_TYPE_INVALID))) {
		syslog_ax_arpsnooping_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_arpsnooping_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
    
    if((TRUE == isEnable)||(FALSE == isEnable)){
        
        npd_eth_port_get_slotno_portno_by_eth_g_index(eth_g_index,&slotNo,&portNo);
        isRgmii = npd_eth_port_rgmii_type_check((unsigned int)slotNo,(unsigned int)portNo);
        if(!isRgmii){
            ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum); 
            
            if(NPD_SUCCESS == ret){

                eth_port = npd_get_port_by_index(eth_g_index);
                if(eth_port){
                    protVlanCtrl = (struct prot_vlan_ctrl_t *) eth_port->funcs.func_data[ETH_PORT_FUNC_PROTOCOL_BASED_VLAN];
                    if(protVlanCtrl){
                        protVlanCtrl->state = isEnable; /* enable */
                        ret = nam_prot_vlan_port_enable_set(devNum,portNum,isEnable);
                        if(PROTOCOL_VLAN_RETURN_CODE_SUCCESS != ret){/* config failed set to disable*/
                            protVlanCtrl->state = 0; /* disable */
                            nam_prot_vlan_port_enable_set(devNum,portNum,0);
                        }
                    }
                    else{
                        ret = COMMON_RETURN_CODE_NULL_PTR;
                    }
                }
                else{
                    ret = COMMON_RETURN_CODE_NULL_PTR;
                }            
                
            }
            else{
                ret = PROTOCOL_VLAN_RETURN_CODE_ERROR;
            }
        }
        else{
            ret = PROTOCOL_VLAN_RETURN_CODE_UNSUPPORT;
        }
    }
    else{
        
        ret = COMMON_RETURN_CODE_BADPARAM;
        
    }
    
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);									 
	return reply;								 

}

#define UNTAG 0
#define TAG   1

DBusMessage * npd_dbus_prot_vlan_config_vid_by_port_entry(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;

	unsigned int ret = 0;
    unsigned short vlanId = 0;
    unsigned int eth_g_index = 0;
    unsigned char devNum = 0,portNum = 0;
    prot_type_e entryNum = 0;
    struct eth_port_s *eth_port = NULL;
    struct prot_vlan_ctrl_t * protVlanCtrl = NULL;
    unsigned short oldVlanId = 0;
    unsigned int oldIsValid = 0;
    unsigned int isRgmii = 0;
    unsigned char slotNo = 0,portNo = 0;
    unsigned int isValid = 0;
    unsigned char tagMode = 0;
    unsigned int addToVlan = FALSE;
    unsigned int untagBmp = 0,tagBmp = 0;
    
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		         DBUS_TYPE_UINT32,&eth_g_index,
		         DBUS_TYPE_UINT32,&entryNum,
		         DBUS_TYPE_UINT16,&vlanId,
				 DBUS_TYPE_INVALID))) {
		syslog_ax_arpsnooping_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_arpsnooping_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
    
    if((0 <= entryNum)&&(entryNum < PROTOCOL_VLAN_MAP_MAX)&&\
        
        ((vlanId >= DEFAULT_VLAN_ID)&&\
        (vlanId <= NPD_PORT_L3INTF_VLAN_ID))){
        
        npd_eth_port_get_slotno_portno_by_eth_g_index(eth_g_index,&slotNo,&portNo);
        isRgmii = npd_eth_port_rgmii_type_check((unsigned int)slotNo,(unsigned int)portNo);
        if(!isRgmii){
            ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum); 
            
            if(NPD_SUCCESS == ret){
                
                eth_port = npd_get_port_by_index(eth_g_index);
                if(eth_port){
                    protVlanCtrl = (struct prot_vlan_ctrl_t *) eth_port->funcs.func_data[ETH_PORT_FUNC_PROTOCOL_BASED_VLAN];
                    if(protVlanCtrl){
                        oldVlanId = protVlanCtrl->vinfo[entryNum].vid;
                        oldIsValid = protVlanCtrl->vinfo[entryNum].flag;
                        ret = nam_prot_vlan_set_vlan_by_port_entry(devNum,portNum,entryNum,vlanId);
                        if(PROTOCOL_VLAN_RETURN_CODE_SUCCESS != ret){/* if set failed invalid it */
                            protVlanCtrl->vinfo[entryNum].vid = oldVlanId;
                            protVlanCtrl->vinfo[entryNum].flag = oldIsValid;     /* invalid on this port */
                            nam_prot_vlan_set_vlan_by_port_entry(devNum,portNum,entryNum,oldVlanId);
                            if(!oldIsValid){
                                nam_prot_vlan_disable_by_port_entry(devNum,portNum,entryNum);
                            }
                        }
                        else if(PROT_TYPE_PPPOE_D == entryNum){
                            entryNum = PROT_TYPE_PPPOE_S;                        
                            ret = nam_prot_vlan_set_vlan_by_port_entry(devNum,portNum,entryNum,vlanId);
rollback:                                       

                            if(PROTOCOL_VLAN_RETURN_CODE_SUCCESS != ret){/* if set failed set to orignal  */  
                                /* set to orignal on this port */
                                nam_prot_vlan_set_vlan_by_port_entry(devNum,portNum,entryNum,oldVlanId);
                                if(!oldIsValid){
                                    nam_prot_vlan_disable_by_port_entry(devNum,portNum,entryNum);
                                }
                                if((PROT_TYPE_PPPOE_D == ret)||(PROT_TYPE_PPPOE_S == ret)){
                                    entryNum = PROT_TYPE_PPPOE_D + PROT_TYPE_PPPOE_S - entryNum; 
                                }                                
                                nam_prot_vlan_set_vlan_by_port_entry(devNum,portNum,entryNum,oldVlanId);
                                if(!oldIsValid){
                                    nam_prot_vlan_disable_by_port_entry(devNum,portNum,entryNum);
                                }
                            }
                        }
                        if(PROTOCOL_VLAN_RETURN_CODE_SUCCESS == ret){    
                            if(TRUE == npd_check_vlan_real_exist(vlanId)){                                    
                                ret = npd_vlan_check_contain_port(vlanId,eth_g_index,&tagMode);
                                if(NPD_TRUE == ret){
                                    if(UNTAG == tagMode){/* nothing to do*/
                                        ret = PROTOCOL_VLAN_RETURN_CODE_SUCCESS;
                                    }
                                    else{/* can't config , error info */
                                        ret = PROTOCOL_VLAN_RETURN_CODE_ALREADY_TAG_MEMBER;
                                    }
                                }
                                else{ /* add port to vlan, only hardware*/
                                    ret = PROTOCOL_VLAN_RETURN_CODE_SUCCESS;
                                    addToVlan = TRUE;
                                }
                            }
                            else{/* create vlan for hardware only*/
                                ret = nam_asic_vlan_entry_active(PRODUCT_ID, vlanId);
                                if(VLAN_CONFIG_SUCCESS == ret){
                                    ret = PROTOCOL_VLAN_RETURN_CODE_SUCCESS;
                                    addToVlan = TRUE;
                                }
                                else{
                                    ret = PROTOCOL_VLAN_RETURN_CODE_VLAN_CREATE_FAILED;
                                }
                            }
                            if((PROTOCOL_VLAN_RETURN_CODE_SUCCESS == ret)&&(TRUE == addToVlan)){
                    		    ret = nam_asic_vlan_entry_ports_add(devNum, vlanId, portNum, UNTAG);
                        		if (VLAN_CONFIG_SUCCESS == ret) {
                        			npd_vlan_set_port_vlan_ingresfilter(eth_g_index, NPD_TRUE);
                                    ret = PROTOCOL_VLAN_RETURN_CODE_SUCCESS;
                        		}
                                else{
                                    ret = PROTOCOL_VLAN_RETURN_CODE_ADD_PORT_TO_VLAN_FAILED;
                                }
                            }
                            if(PROTOCOL_VLAN_RETURN_CODE_SUCCESS == ret){
                                if(TRUE == npd_check_vlan_real_exist(oldVlanId)){
                                    ret = npd_vlan_check_contain_port(oldVlanId,eth_g_index,&tagMode);
                                    if(NPD_TRUE == ret){
                                        if(UNTAG == tagMode){/* nothing to do*/
                                            ret = PROTOCOL_VLAN_RETURN_CODE_SUCCESS;
                                        }
                                        else{/* can't config , error info */
                                            ret = PROTOCOL_VLAN_RETURN_CODE_ERROR;
                                        }
                                    }
                                    else{ /* del port from vlan, only hardware*/
                                        ret = PROTOCOL_VLAN_RETURN_CODE_SUCCESS;
                                        nam_asic_vlan_entry_ports_del(oldVlanId,devNum,portNum,0);
                                    }
                                }
                                else{/* maybe vlan is auto  created by protocol-vlan */
                                    nam_asic_vlan_entry_ports_del(oldVlanId,devNum,portNum,0);
                                    nam_asic_vlan_get_port_members_bmp(oldVlanId,&untagBmp,&tagBmp); /*ignore failed*/                                   
                                    if(!(untagBmp && tagBmp)){ /* no other port member inactive the vlan */
                                        nam_asic_vlan_entry_delete(oldVlanId);
                                    } 
                                }
                            }
                            if(PROTOCOL_VLAN_RETURN_CODE_SUCCESS != ret){
                                goto rollback;
                            }
                            
                        }
                        nam_prot_vlan_get_vlan_by_port_entry(devNum,portNum,entryNum,&vlanId,&isValid);
                        protVlanCtrl->vinfo[entryNum].vid = vlanId;
                        protVlanCtrl->vinfo[entryNum].flag = isValid;
                        if((PROT_TYPE_PPPOE_D == entryNum)||(PROT_TYPE_PPPOE_S == entryNum)){ /*if is pppop_d or pppop_s set the other*/
                            entryNum = PROT_TYPE_PPPOE_S + PROT_TYPE_PPPOE_D - entryNum;
                            nam_prot_vlan_get_vlan_by_port_entry(devNum,portNum,entryNum,&vlanId,&isValid);
                            protVlanCtrl->vinfo[entryNum].vid = vlanId;
                            protVlanCtrl->vinfo[entryNum].flag = isValid;
                        }
                    }
                    else{
                        ret = COMMON_RETURN_CODE_NULL_PTR;
                    }                    
                }
                else{
                    ret = COMMON_RETURN_CODE_NULL_PTR;
                }
                           
            }
            else{
                ret = PROTOCOL_VLAN_RETURN_CODE_ERROR;
            }
        }
        else{
            ret = PROTOCOL_VLAN_RETURN_CODE_UNSUPPORT;
        }
    }
    else{
        
        ret = COMMON_RETURN_CODE_BADPARAM;
        
    }
    
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);									 
	return reply;								 

}

/****************************************************************************************
 * OUTPUT:
 *                              PROTOCOL_VLAN_RETURN_CODE_SUCCESS
 *                              COMMON_RETURN_CODE_BADPARAM
 *                              PROTOCOL_VLAN_RETURN_CODE_NAM_ERR
 *                              PROTOCOL_VLAN_RETURN_CODE_VLAN_NOT_MATCH
 *****************************************************************************************/

DBusMessage * npd_dbus_prot_vlan_no_vid_by_port_entry(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;

	unsigned int ret = 0;
    unsigned short vlanId = 0,tempVlanId = 0;
    unsigned int eth_g_index = 0;
    unsigned int isValid = 0;
    unsigned char devNum = 0,portNum = 0;
    prot_type_e entryNum = 0;
    struct eth_port_s *eth_port = NULL;
    struct prot_vlan_ctrl_t * protVlanCtrl = NULL;
    unsigned char slotNo = 0,portNo = 0;
    unsigned int isRgmii = 0;
    unsigned char tagMode = 0;
    unsigned int untagBmp = 0,tagBmp = 0;
    
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		         DBUS_TYPE_UINT32,&eth_g_index,
		         DBUS_TYPE_UINT32,&entryNum,
		         DBUS_TYPE_UINT16,&vlanId,
				 DBUS_TYPE_INVALID))) {
		syslog_ax_arpsnooping_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_arpsnooping_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
    
    if((0 <= entryNum)&&(entryNum < PROTOCOL_VLAN_MAP_MAX)&&\        
        ((vlanId >= DEFAULT_VLAN_ID)&&\
        (vlanId <= NPD_PORT_L3INTF_VLAN_ID))){
        
        npd_eth_port_get_slotno_portno_by_eth_g_index(eth_g_index,&slotNo,&portNo);
        isRgmii = npd_eth_port_rgmii_type_check((unsigned int)slotNo,(unsigned int)portNo);
        if(!isRgmii){
            ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum); 
            
            if(NPD_SUCCESS == ret){
                
                eth_port = npd_get_port_by_index(eth_g_index);
                if(eth_port){
                    protVlanCtrl = (struct prot_vlan_ctrl_t *) eth_port->funcs.func_data[ETH_PORT_FUNC_PROTOCOL_BASED_VLAN];
                    if(protVlanCtrl){
                        tempVlanId = protVlanCtrl->vinfo[entryNum].vid;
                        if(0 == tempVlanId){
                            ret = PROTOCOL_VLAN_RETURN_CODE_VID_NOT_SET;
                        }
                        else if(vlanId != tempVlanId){
                            ret = PROTOCOL_VLAN_RETURN_CODE_VID_NOT_MATCH;                
                        }
                        else{
                            
                            protVlanCtrl->vinfo[entryNum].vid = 0;
                            protVlanCtrl->vinfo[entryNum].flag = 0;     /* invalid on this port */

                            nam_prot_vlan_set_vlan_by_port_entry(devNum,portNum,entryNum,0);

                            ret = nam_prot_vlan_disable_by_port_entry(devNum,portNum,entryNum);
                            
                            if((PROTOCOL_VLAN_RETURN_CODE_SUCCESS == ret)&&\
                                (PROT_TYPE_PPPOE_D == entryNum)){

                                entryNum = PROT_TYPE_PPPOE_S;
                                
                                protVlanCtrl->vinfo[entryNum].vid = 0;
                                protVlanCtrl->vinfo[entryNum].flag = 0;     /* invalid on this port */

                                nam_prot_vlan_set_vlan_by_port_entry(devNum,portNum,entryNum,0);

                                ret = nam_prot_vlan_disable_by_port_entry(devNum,portNum,entryNum);
                                
                            }
                            if(TRUE == npd_check_vlan_real_exist(vlanId)){
                                ret = npd_vlan_check_contain_port(vlanId,eth_g_index,&tagMode);
                                if(NPD_TRUE == ret){
                                    if(UNTAG == tagMode){/* nothing to do*/
                                        ret = PROTOCOL_VLAN_RETURN_CODE_SUCCESS;
                                    }
                                    else{/* can't config , error info */
                                        ret = PROTOCOL_VLAN_RETURN_CODE_ERROR;
                                    }
                                }
                                else{ /* del port from vlan, only hardware*/
                                    ret = PROTOCOL_VLAN_RETURN_CODE_SUCCESS;
                                    nam_asic_vlan_entry_ports_del(vlanId,devNum,portNum,0);
                                }
                            }
                            else{/* maybe vlan is auto  created by protocol-vlan */
                                nam_asic_vlan_entry_ports_del(vlanId,devNum,portNum,0);
                                nam_asic_vlan_get_port_members_bmp(vlanId,&untagBmp,&tagBmp); /*ignore failed*/                                   
                                if(!(untagBmp && tagBmp)){ /* no other port member inactive the vlan */
                                    nam_asic_vlan_entry_delete(vlanId);
                                } 
                            }
                            nam_prot_vlan_get_vlan_by_port_entry(devNum,portNum,entryNum,&vlanId,&isValid);
                            protVlanCtrl->vinfo[entryNum].vid = vlanId;
                            protVlanCtrl->vinfo[entryNum].flag = isValid;
                            if((PROT_TYPE_PPPOE_D == entryNum)||(PROT_TYPE_PPPOE_S == entryNum)){ /*if is pppop_d or pppop_s set the other*/
                                entryNum = PROT_TYPE_PPPOE_S + PROT_TYPE_PPPOE_D - entryNum;
                                nam_prot_vlan_get_vlan_by_port_entry(devNum,portNum,entryNum,&vlanId,&isValid);
                                protVlanCtrl->vinfo[entryNum].vid = vlanId;
                                protVlanCtrl->vinfo[entryNum].flag = isValid;
                            }
                        }
                    }
                    else{
                        ret = COMMON_RETURN_CODE_NULL_PTR;
                    }
                }
                else{
                    ret = COMMON_RETURN_CODE_NULL_PTR;
                }
                
            }
            else{
                ret = PROTOCOL_VLAN_RETURN_CODE_ERROR;
            }
        }
        else{
            ret = PROTOCOL_VLAN_RETURN_CODE_UNSUPPORT;
        }
    }
    else{        
        ret = COMMON_RETURN_CODE_BADPARAM;        
    }
    
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;								 

}

DBusMessage * npd_dbus_prot_vlan_config_udf_ethtype(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;

	unsigned int ret = 0;
    unsigned char devNum = 0;
    unsigned int ethType = 0;
    prot_type_e entryNum = 0;
    unsigned int isValid = 0;
    
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		         DBUS_TYPE_UINT32,&ethType,
				 DBUS_TYPE_INVALID))) {
		syslog_ax_arpsnooping_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_arpsnooping_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

    entryNum = PROT_TYPE_UDF;
    if((0 != ethType)&&\
        (ethType < 1536)){
        ret = PROTOCOL_VLAN_RETURN_CODE_ETH_TYPE_RANG_ERR;
    }
    else if(((ETH_P_IP == ethType)||\
         (ETH_P_IPV6 == ethType)||\
         (ETH_P_PPP_DISC == ethType)||\
         (ETH_P_PPP_SES == ethType))){
         ret = PROTOCOL_VLAN_RETURN_CODE_ETH_TYPE_EXISTS;
    }
    else{        
        ret = nam_prot_vlan_set_entry_eth_type(devNum,entryNum,ethType);
        
        if(0 == ethType){
            nam_prot_vlan_disable_by_entry(devNum,entryNum);
        }
        
        ret = nam_prot_vlan_get_entry_eth_type(devNum,entryNum,&ethType,&isValid);
        
        prot_vlan_entry_protocol[entryNum] = ethType;
    }
    
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);									 
	return reply;								 

}

DBusMessage * npd_dbus_prot_vlan_show_udf_ethtype(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;

    prot_type_e entryNum = 0;
    unsigned short ethType = 0;
    
	dbus_error_init(&err);
	
	entryNum = PROT_TYPE_UDF;
    
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &prot_vlan_entry_protocol[entryNum]);									 
	return reply;								 

}


DBusMessage * npd_dbus_prot_vlan_show_port_cfg(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;

	unsigned int ret = 0;
    unsigned int eth_g_index = 0;
    unsigned char slotNo = 0,portNo = 0;
    prot_type_e entryNum = 0;
    struct eth_port_s *eth_port = NULL;
    struct prot_vlan_ctrl_t * protVlanCtrl = NULL;
    unsigned short vlanId = 0;
    unsigned int isValid = 0;
    unsigned int isGlobalIndex = 0;
    unsigned int portValue = 0;
    unsigned int isRgmii = 0;
    
    
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		         DBUS_TYPE_UINT32,&isGlobalIndex,
		         DBUS_TYPE_UINT32,&portValue,
				 DBUS_TYPE_INVALID))) {
		syslog_ax_arpsnooping_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_arpsnooping_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
    
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
    
    if(isGlobalIndex){
        eth_g_index = portValue;
        npd_eth_port_get_slotno_portno_by_eth_g_index(eth_g_index,&slotNo,&portNo);
    }
    else{
        slotNo = (portValue >> 8) & 0xff;
        portNo = (portValue & 0xff);
        eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotNo,portNo);
    }
    isRgmii = npd_eth_port_rgmii_type_check((unsigned int)slotNo,(unsigned int)portNo);
    
    if(isRgmii){
        ret = PROTOCOL_VLAN_RETURN_CODE_UNSUPPORT;
    }
    else{
        eth_port = npd_get_port_by_index(eth_g_index);
        
        if(eth_port){
            protVlanCtrl = (struct prot_vlan_ctrl_t *) eth_port->funcs.func_data[ETH_PORT_FUNC_PROTOCOL_BASED_VLAN];
            if(protVlanCtrl){
                ret = PROTOCOL_VLAN_RETURN_CODE_SUCCESS;                            		
            }
            else{
                ret = COMMON_RETURN_CODE_NULL_PTR;
            }        
        }
        else{
            ret = COMMON_RETURN_CODE_NULL_PTR;
        }
    }
    
    dbus_message_iter_append_basic (&iter,
                                         DBUS_TYPE_UINT32,
                                         &ret);
    if(PROTOCOL_VLAN_RETURN_CODE_SUCCESS == ret){
        
        dbus_message_iter_append_basic (&iter,
                                     DBUS_TYPE_UINT16,
                                     &prot_vlan_entry_protocol[PROT_TYPE_UDF]);
    
        dbus_message_iter_append_basic (&iter,
    									 DBUS_TYPE_BYTE,
    									 &slotNo);
        
        dbus_message_iter_append_basic (&iter,
    									 DBUS_TYPE_BYTE,
    									 &portNo);
        
        dbus_message_iter_append_basic (&iter,
    									 DBUS_TYPE_BYTE,
    									 &(protVlanCtrl->state));
        
        for(entryNum = 0;entryNum <= PROT_TYPE_UDF;entryNum++){
            
            dbus_message_iter_append_basic (&iter,
                                            DBUS_TYPE_STRING,
                                            &prot_vlan_prot_name[entryNum]);
            
            vlanId = protVlanCtrl->vinfo[entryNum].vid;
            dbus_message_iter_append_basic (&iter,
    									 DBUS_TYPE_UINT16,
    									 &vlanId);
            
            isValid = protVlanCtrl->vinfo[entryNum].flag;
            dbus_message_iter_append_basic (&iter,
    									 DBUS_TYPE_UINT32,
    									 &isValid);
            if(PROT_TYPE_PPPOE_D == entryNum){
                entryNum++;
            }
        }
    }   
    
	return reply;

}

DBusMessage * npd_dbus_prot_vlan_show_port_cfg_list(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
 	DBusMessageIter	 iter, iter_struct, iter_array;
	DBusError err;

	unsigned int ret = 0;    
    int i = 0,j = 0;
    unsigned char slot_count = CHASSIS_SLOT_COUNT;
    unsigned char slotno =  0;
    unsigned char local_port_count = 0;
    unsigned char local_port_no = 0; 
    unsigned int eth_g_index = 0;
    prot_type_e entryNum = 0;
    struct eth_port_s *eth_port = NULL;
    struct prot_vlan_ctrl_t * protVlanCtrl = NULL;
    unsigned short vlanId = 0;
    unsigned int isValid = 0;
    unsigned char protName[21] = {0};
    unsigned char * protNamePtr = protName;
    unsigned int isRgmii = 0;
    
	dbus_error_init(&err);	

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

    dbus_message_iter_append_basic (&iter,
                                     DBUS_TYPE_UINT16,
                                     &prot_vlan_entry_protocol[PROT_TYPE_UDF]);

    dbus_message_iter_append_basic (&iter,
                                     DBUS_TYPE_BYTE,
                                     &slot_count);
    

    for (i = 0; i < slot_count; i++ ) {
        slotno =  CHASSIS_SLOT_INDEX2NO(i);
        local_port_count = ETH_LOCAL_PORT_COUNT(i);   
        
        dbus_message_iter_append_basic
                (&iter,
                  DBUS_TYPE_BYTE,
                  &(slotno));
        
        dbus_message_iter_append_basic
                (&iter,
                  DBUS_TYPE_BYTE,
                  &(local_port_count));
        
        for (j = 0; j < local_port_count; j++ ) {  

            local_port_no = ETH_LOCAL_INDEX2NO(i,j); 
            eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotno,local_port_no);
            isRgmii = npd_eth_port_rgmii_type_check((unsigned int)slotno,(unsigned int)local_port_no);
            
            dbus_message_iter_append_basic (&iter,
        									 DBUS_TYPE_BYTE,
        									 &local_port_no);
                
            dbus_message_iter_append_basic (&iter,
                                             DBUS_TYPE_UINT32,
                                             &isRgmii);
            
            if(!isRgmii){
                
                eth_port = npd_get_port_by_index(eth_g_index);
                
                if(eth_port){
                    protVlanCtrl = (struct prot_vlan_ctrl_t *) eth_port->funcs.func_data[ETH_PORT_FUNC_PROTOCOL_BASED_VLAN];
                    if(protVlanCtrl){
                        ret = PROTOCOL_VLAN_RETURN_CODE_SUCCESS;
                    }
                    else{
                        ret = COMMON_RETURN_CODE_NULL_PTR;
                    }        
                }
                else{
                    ret = COMMON_RETURN_CODE_NULL_PTR;
                }
            
                dbus_message_iter_append_basic (&iter,
                                                 DBUS_TYPE_UINT32,
                                                 &ret);
                
                if(PROTOCOL_VLAN_RETURN_CODE_SUCCESS == ret){
                    
                    dbus_message_iter_append_basic (&iter,
                									 DBUS_TYPE_BYTE,
                									 &(protVlanCtrl->state));
                    
                    dbus_message_iter_open_container (&iter,
    											   DBUS_TYPE_ARRAY,
    											   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
    											            DBUS_TYPE_UINT16_AS_STRING
    											            DBUS_TYPE_UINT32_AS_STRING		
    											   DBUS_STRUCT_END_CHAR_AS_STRING,
    											   &iter_array);
                    
                    for(entryNum = 0;entryNum <= PROT_TYPE_UDF;entryNum++){                    
                        if(PROT_TYPE_PPPOE_S == entryNum){
                            continue;
                        }
                        dbus_message_iter_open_container (&iter_array,
    												   DBUS_TYPE_STRUCT,
    												   NULL,
    												   &iter_struct);
                                                
                        vlanId = protVlanCtrl->vinfo[entryNum].vid;
                        dbus_message_iter_append_basic (&iter_struct,
                									 DBUS_TYPE_UINT16,
                									 &vlanId);

                        isValid = protVlanCtrl->vinfo[entryNum].flag;
                        dbus_message_iter_append_basic (&iter_struct,
                									 DBUS_TYPE_UINT32,
                									 &isValid);
                        
                        dbus_message_iter_close_container (&iter_array, &iter_struct);
                        
                    }
                    dbus_message_iter_close_container (&iter, &iter_array);
                }   
            }
        }
    }
    
	return reply;
}

void npd_prot_vlan_config_udf_ethtype_save_cfg
(
	char** showStr,
	int* avalidLen
)
{
	int len = 0;
	char * cfgStr =		"config prot-vlan udf-ethtype";
	if((NULL == showStr )||(NULL == *showStr)||(NULL == avalidLen)) {
		return ;
	}
	else {
        if(*avalidLen < strlen(cfgStr + strlen(" 0x8888\n"))){
            return ;
        }
        if(prot_vlan_entry_protocol[PROT_TYPE_UDF]){
			len = sprintf((*showStr),"%s %#x\n",cfgStr,prot_vlan_entry_protocol[PROT_TYPE_UDF]);
			*showStr += len;
			*avalidLen -= len;
		}
	}    
	return ;
}

int np_prot_vlan_udf_ethtype_vid_enable_set
(
    unsigned int isEnable
)
{
	unsigned int ret = 0;    
    int i = 0,j = 0;
    unsigned char slot_count = CHASSIS_SLOT_COUNT;
    unsigned char slotno =  0;
    unsigned char local_port_count = 0;
    unsigned char local_port_no = 0; 
    unsigned int eth_g_index = 0;
    struct eth_port_s *eth_port = NULL;
    struct prot_vlan_ctrl_t * protVlanCtrl = NULL;
    unsigned short vlanId = 0;
    unsigned int isValid = 0;
    unsigned int isRgmii = 0;
    prot_type_e entryNum = PROT_TYPE_UDF;
    for (i = 0; i < slot_count; i++ ) {
        slotno =  CHASSIS_SLOT_INDEX2NO(i);
        local_port_count = ETH_LOCAL_PORT_COUNT(i);   
        
        for (j = 0; j < local_port_count; j++ ) {  

            local_port_no = ETH_LOCAL_INDEX2NO(i,j); 
            eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotno,local_port_no);
            isRgmii = npd_eth_port_rgmii_type_check((unsigned int)slotno,(unsigned int)local_port_no);
                        
            if(!isRgmii){                
                eth_port = npd_get_port_by_index(eth_g_index);                
                if(eth_port){
                    protVlanCtrl = (struct prot_vlan_ctrl_t *) eth_port->funcs.func_data[ETH_PORT_FUNC_PROTOCOL_BASED_VLAN];
                    if(protVlanCtrl){
                        if(protVlanCtrl->vinfo[PROT_TYPE_UDF].vid){
                            protVlanCtrl->vinfo[PROT_TYPE_UDF].flag = isEnable;
                        }
                        ret = PROTOCOL_VLAN_RETURN_CODE_SUCCESS;
                    }
                }
            }
        }
    }

    return ret;  /* code optimize: Missing return statement. zhangdi@autelan.com 2013-01-18 */
}

int npd_prot_vlan_vid_check_for_all_port
(
    unsigned short vlanId
)
{
    int i = 0,j = 0;
    unsigned char slot_count = CHASSIS_SLOT_COUNT;
    unsigned char slotno =  0;
    unsigned char local_port_count = 0;
    unsigned char local_port_no = 0; 
    for (i = 0; i < slot_count; i++ ) {
        slotno =  CHASSIS_SLOT_INDEX2NO(i);
        local_port_count = ETH_LOCAL_PORT_COUNT(i);   
        
        for (j = 0; j < local_port_count; j++ ) {  

            local_port_no = ETH_LOCAL_INDEX2NO(i,j); 
            if(npd_prot_vlan_vid_check_by_slot_port(slotno,local_port_no,vlanId)){
                return TRUE;
            }
        }
    }
    return FALSE;
}

int npd_prot_vlan_vid_check_by_slot_port
(
    unsigned char slotno,
    unsigned char local_port_no,
    unsigned short vlanId
)
{
    unsigned int eth_g_index = 0;
    struct eth_port_s *eth_port = NULL;
    struct prot_vlan_ctrl_t * protVlanCtrl = NULL;
    unsigned int isRgmii = 0;
    prot_type_e entryNum = 0;
    
    eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotno,local_port_no);
    isRgmii = npd_eth_port_rgmii_type_check((unsigned int)slotno,(unsigned int)local_port_no);
                
    if(!isRgmii){
        eth_port = npd_get_port_by_index(eth_g_index);                
        if(eth_port){
            protVlanCtrl = (struct prot_vlan_ctrl_t *) eth_port->funcs.func_data[ETH_PORT_FUNC_PROTOCOL_BASED_VLAN];
            if(protVlanCtrl){
                for(entryNum = 0;entryNum <= PROT_TYPE_UDF;entryNum++){
                    if(vlanId == (protVlanCtrl->vinfo[entryNum].vid)){
                        return TRUE;
                    }
                }
            }
        }
    }
    return FALSE;    
}

int npd_prot_vlan_vid_check_by_global_index
(
    unsigned int eth_g_index,
    unsigned short vlanId
)
{
    unsigned char slotNo = 0,portNo = 0;
    struct eth_port_s *eth_port = NULL;
    struct prot_vlan_ctrl_t * protVlanCtrl = NULL;
    unsigned int isRgmii = 0;
    prot_type_e entryNum = 0;
    
    npd_eth_port_get_slotno_portno_by_eth_g_index(eth_g_index,&slotNo,&portNo);
    isRgmii = npd_eth_port_rgmii_type_check((unsigned int)slotNo,(unsigned int)portNo);
                
    if(!isRgmii){
        eth_port = npd_get_port_by_index(eth_g_index);
        if(eth_port){
            protVlanCtrl = (struct prot_vlan_ctrl_t *) eth_port->funcs.func_data[ETH_PORT_FUNC_PROTOCOL_BASED_VLAN];
            if(protVlanCtrl){
                for(entryNum = 0;entryNum <= PROT_TYPE_UDF;entryNum++){
                    if(vlanId == (protVlanCtrl->vinfo[entryNum].vid)){
                        return TRUE;
                    }
                }
            }
        }
    }
    return FALSE;  
}

#ifdef __cplusplus
}
#endif

