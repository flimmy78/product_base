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
* nam_prot_vlan.c
*
*
* CREATOR:
* 		hanhui@autelan.com
*
* DESCRIPTION:
* 		nam implement for protocol vlan
*
* DATE:
*		15/4/2010
*
* FILE REVISION NUMBER:
*  		$Revision: 1.1 $
*
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include "sysdef/returncode.h"

#ifdef DRV_LIB_CPSS
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/ip/cpssIpTypes.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#endif

#include "nam_log.h"

/*************************************************************
 * DESCRIPTION:
 *              this function get ether type and isvalid info by entry 
 *  INPUT:
 *              devNum - uint8 :  dev num
 *              entryNum - unit32 : the entry who's info be want
 * OUTPUT:
 *              etherType  - uint16 : ether type of this entry
 *              isValid   - uint 32 : is this entry valid 
 *                                                       TRUE  - isValid
 *                                                       FALSE - isInvalid
 * RETURN:
 *              PROTOCOL_VLAN_RETURN_CODE_SUCCESS    -  operation success
 *              PROTOCOL_VLAN_RETURN_CODE_NAM_ERR    -   operation failed
 * NOTE:
 *
 *************************************************************/
int nam_prot_vlan_get_entry_eth_type
(
    unsigned char   devNum,
    unsigned int    entryNum,
    unsigned short *etherType,
    unsigned int   *isValid
)
{
    int ret = 0;
#ifdef DRV_LIB_CPSS
    CPSS_PROT_CLASS_ENCAP_STC        encListPtr;    

    memset(&encListPtr,0,sizeof(CPSS_PROT_CLASS_ENCAP_STC));
    ret = cpssDxChBrgVlanProtoClassGet(devNum,entryNum,etherType,&encListPtr,isValid);
    if(NAM_OK != ret){
        nam_syslog_err("get portocol vlan info error,devNum %d entryNum %d ret %#x \n",devNum,entryNum,ret);
        return PROTOCOL_VLAN_RETURN_CODE_NAM_ERR;       
    }
    nam_syslog_dbg("get protocol vlan info for devNum %d entryNum %d:\n" \
        "\tetherType %#4x %s encListPtr: ethV2 %d,nonLlcSnap %d,llcSnap %d\n",devNum,entryNum, \
        *etherType,(*isValid)?"VALID":"INVALID",encListPtr.ethV2,encListPtr.nonLlcSnap,encListPtr.llcSnap);
#endif
    return PROTOCOL_VLAN_RETURN_CODE_SUCCESS;
}

/*************************************************************
 * DESCRIPTION:
 *              this function set ether type by entry,and validate it
 *  INPUT:
 *              devNum - uint8 :  dev num
 *              entryNum - unit32 : the entry who's info be want to set
 *              etherType  - uint16 : ether type we want to set for this entry
 * OUTPUT:
 *
 * RETURN:
 *              PROTOCOL_VLAN_RETURN_CODE_SUCCESS    -  operation success
 *              PROTOCOL_VLAN_RETURN_CODE_NAM_ERR    -   operation failed
 * NOTE:
 *
 *************************************************************/
int nam_prot_vlan_set_entry_eth_type
(
    unsigned char  devNum,
    unsigned int   entryNum,
    unsigned short etherType
)
{
    int ret = 0;
#ifdef DRV_LIB_CPSS
    CPSS_PROT_CLASS_ENCAP_STC        encListPtr;
    
    memset(&encListPtr,0,sizeof(CPSS_PROT_CLASS_ENCAP_STC));
    encListPtr.ethV2 = 1;

    ret = cpssDxChBrgVlanProtoClassSet(devNum,entryNum,etherType,&encListPtr);
    if(NAM_OK != ret){
        nam_syslog_err("set portocol vlan info error,devNum %d entryNum %d ret %#x \n",devNum,entryNum,ret);
        return PROTOCOL_VLAN_RETURN_CODE_NAM_ERR;
    }
    nam_syslog_dbg("set protocol vlan info for devNum %d entryNum %d success:\n" \
        "\tetherType %#4x encListPtr: ethV2 %d,nonLlcSnap %d,llcSnap %d\n",devNum,entryNum, \
        etherType,encListPtr.ethV2,encListPtr.nonLlcSnap,encListPtr.llcSnap);
#endif
    return PROTOCOL_VLAN_RETURN_CODE_SUCCESS;
}

/*************************************************************
 * DESCRIPTION:
 *              this function disable protocol vlan by entry
 *  INPUT:
 *              devNum - uint8 :  dev num
 *              entryNum - unit32 : the entry who's info be want
 * OUTPUT:
 *
 * RETURN:
 *              PROTOCOL_VLAN_RETURN_CODE_SUCCESS    -  operation success
 *              PROTOCOL_VLAN_RETURN_CODE_NAM_ERR    -   operation failed
 * NOTE:
 *
 *************************************************************/
int nam_prot_vlan_disable_by_entry
(
    unsigned char devNum,
    unsigned int  entryNum
)
{
    int ret = 0;
#ifdef DRV_LIB_CPSS
    ret = cpssDxChBrgVlanProtoClassInvalidate(devNum,entryNum);
    if(NAM_OK != ret){
        nam_syslog_err("prot vlan disable by entry failed,entry %d ret %#x\n",entryNum,ret);
        return PROTOCOL_VLAN_RETURN_CODE_NAM_ERR;
    }
    nam_syslog_dbg("prot vlan disable by entry devNum %d entryNum %d success",devNum,entryNum);
#endif    
    return PROTOCOL_VLAN_RETURN_CODE_SUCCESS;
}

/*************************************************************
 * DESCRIPTION:
 *              this function set protocol vlan ID for the very port and entry
 *  INPUT:
 *              devNum - uint8 :  dev num
 *              portNum - uint8 : port num
 *              entryNum - unit32 : the entry who's info be want
 *              vlanId - uint16 : set to this vlanId
 * OUTPUT:
 *
 * RETURN:
 *              PROTOCOL_VLAN_RETURN_CODE_SUCCESS    -  operation success
 *              PROTOCOL_VLAN_RETURN_CODE_NAM_ERR    -   operation failed
 * NOTE:
 *
 *************************************************************/
int nam_prot_vlan_set_vlan_by_port_entry
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned int  entryNum,
    unsigned short  vlanId
)
{
    int ret = 0;
#ifdef DRV_LIB_CPSS
    CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC    vlanCfgPtr;
    CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC     qosCfgPtr;
    
    memset(&vlanCfgPtr,0,sizeof(CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC));
    memset(&qosCfgPtr,0,sizeof(CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC));
    vlanCfgPtr.vlanId = vlanId;
    vlanCfgPtr.vlanIdAssignCmd = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E;
    ret = cpssDxChBrgVlanPortProtoVlanQosSet(devNum,portNum,entryNum,&vlanCfgPtr,&qosCfgPtr);
    if(NAM_OK != ret){
        nam_syslog_err("prot vlan set vlan by devNum %d port %d entry %d failed,ret %#x\n",devNum,portNum,entryNum,ret);
        return PROTOCOL_VLAN_RETURN_CODE_NAM_ERR;       
    }
    nam_syslog_dbg("prot vlan set vlan by port entry success,devNum %d port %d entry %d vlan %d\n",devNum,portNum,entryNum,vlanId);
#endif
    return PROTOCOL_VLAN_RETURN_CODE_SUCCESS;
}

/*************************************************************
 * DESCRIPTION:
 *              this function get protocol vlan ID and isvalid of the very port and entry
 *  INPUT:
 *              devNum - uint8 :  dev num
 *              portNum - uint8 : port num
 *              entryNum - unit32 : the entry who's info be want
 * OUTPUT:
 *              vlanId  - uint16 : the vlanId we got
 *              isValid  - uint32 : is valid ?
 *                                          TRUE  - is valid
 *                                          FALSE  - is invalid
 *
 * RETURN:
 *              PROTOCOL_VLAN_RETURN_CODE_SUCCESS    -  operation success
 *              PROTOCOL_VLAN_RETURN_CODE_NAM_ERR    -   operation failed
 * NOTE:
 *
 *************************************************************/

int nam_prot_vlan_get_vlan_by_port_entry
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned int  entryNum,
    unsigned short *vlanId,
    unsigned int *isValid
)
{
    int ret = 0;
    unsigned int vlanCmd = 0;
#ifdef DRV_LIB_CPSS
    CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC    vlanCfgPtr;
    CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC     qosCfgPtr;
    
    memset(&vlanCfgPtr,0,sizeof(CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC));
    memset(&qosCfgPtr,0,sizeof(CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC));
    ret = cpssDxChBrgVlanPortProtoVlanQosGet(devNum,portNum,entryNum,&vlanCfgPtr,&qosCfgPtr,isValid);
    if(NAM_OK != ret){
        nam_syslog_err("prot vlan get vlan by devNum %d port %d entry %d ,ret %#x\n",devNum,portNum,entryNum,ret);
        return PROTOCOL_VLAN_RETURN_CODE_NAM_ERR;       
    }
    *vlanId = vlanCfgPtr.vlanId;
    vlanCmd = vlanCfgPtr.vlanIdAssignCmd;
    nam_syslog_dbg("prot vlan get vlan by port entry success,devNum %d port %d entry %d vlan %d cmd %d %s\n",devNum,portNum,entryNum,(*vlanId),vlanCmd,(*isValid)?"VALID":"INVALID");
#endif
    return PROTOCOL_VLAN_RETURN_CODE_SUCCESS;
}

/*************************************************************
 * DESCRIPTION:
 *              this function set port protocol vlan to enable/disable
 *  INPUT:
 *              devNum - uint8 :  dev num
 *              portNum - uint8 : port num
 *              isEnable - unint32 : is set to enable or disable
 * OUTPUT:
 *
 * RETURN:
 *              PROTOCOL_VLAN_RETURN_CODE_SUCCESS    -  operation success
 *              PROTOCOL_VLAN_RETURN_CODE_NAM_ERR    -   operation failed
 * NOTE:
 *
 *************************************************************/

int nam_prot_vlan_port_enable_set
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned int  isEnable
)
{
    int ret = 0;
#ifdef DRV_LIB_CPSS
    ret = cpssDxChBrgVlanPortProtoClassVlanEnable(devNum,portNum,isEnable);
    if(NAM_OK != ret){
        nam_syslog_err("prot vlan %s by port entry failed,devNum %d portNum %d ret %#x\n",isEnable ? "ENABLE":"DISABLE",devNum,portNum,ret);
        return PROTOCOL_VLAN_RETURN_CODE_NAM_ERR;
    }
    nam_syslog_err("prot vlan %s by port entry success,devNum %d portNum %d\n",isEnable ? "ENABLE":"DISABLE",devNum,portNum);
#endif
    return PROTOCOL_VLAN_RETURN_CODE_SUCCESS;
}

/*************************************************************
 * DESCRIPTION:
 *              this function invalidate protocol vlan by port and entry
 *  INPUT:
 *              devNum - uint8 :  dev num
 *              portNum - uint8 : port num
 *              entryNum - unit32 : the entry we want to set
 * OUTPUT:
 *
 * RETURN:
 *              PROTOCOL_VLAN_RETURN_CODE_SUCCESS    -  operation success
 *              PROTOCOL_VLAN_RETURN_CODE_NAM_ERR    -   operation failed
 * NOTE:
 *
 *************************************************************/

int nam_prot_vlan_disable_by_port_entry
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned int  entryNum
)
{
    int ret = 0;
#ifdef DRV_LIB_CPSS
    ret = cpssDxChBrgVlanPortProtoVlanQosInvalidate(devNum,portNum,entryNum);
    if(NAM_OK != ret){
        nam_syslog_err("prot vlan disable by port entry failed,devNum %d portNum %d entryNum %d ret %#x\n",devNum,portNum,entryNum,ret);
        return PROTOCOL_VLAN_RETURN_CODE_NAM_ERR;
    }
    nam_syslog_err("prot vlan disable by port entry success,devNum %d portNum %d entryNum %d\n",devNum,portNum,entryNum);
#endif
    return PROTOCOL_VLAN_RETURN_CODE_SUCCESS;
}

#ifdef __cplusplus
}
#endif

