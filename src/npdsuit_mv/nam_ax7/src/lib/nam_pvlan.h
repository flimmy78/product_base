#ifndef __NAM_PVLAN_H__
#define __NAM_PVLAN_H__

#define PVE_CONFIG_ERR 1
#define PVE_CONFIG_SUCESS 0
#ifdef DRV_LIB_CPSS
#define NAM_CONVERT_BACK_DEV_PORT_MAC(devNum, portNum) \
{                                                  \
   gtPortsDevConvertBack(&devNum, &portNum);       \
}

#define NAM_CONVERT_DEV_PORT_MAC(devNum, portNum) \
{                                             \
   gtPortsDevConvert(&devNum, &portNum);      \
}
#endif

typedef enum
{
    NAM_PVE_FALSE = 0,
    NAM_PVE_TRUE  = 1
} NAM_BOOL;


typedef struct{
    unsigned int   ports[2];
}CPSS_PORTS_BMP_STC;
#ifdef DRV_LIB_CPSS
/*
 * Typedef: CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT
 *
 * Description:
 * This enum defines Bridge Source Id Assignment Mode.
 *
 * Fields:
 *  CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E - 
 *                                     If MAC Source address 
 *                                     found in FDB then packet assigned with 
 *                                     Source ID from FDB entry, 
 *                                     else packet assigned with port's 
 *                                     default Source ID.     
 *  CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E -
 *                                     Packet assigned with port's 
 *                                     default Source ID regardless FDB   
 *       
 */

typedef enum 
{
    CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E,
    CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E
}CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT;


/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanEnable
*
* DESCRIPTION:
*       This function enables/disables the Private Edge VLAN on
*       specified device
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE to enable the feature,
*                GT_FALSE to disable the feature
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad devNum
*
* COMMENTS:
*
*******************************************************************************/

extern unsigned int cpssDxChBrgPrvEdgeVlanEnable
(
    unsigned char        devNum,
    NAM_BOOL             enable
);


/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanPortEnable
*
* DESCRIPTION:
*       Enable/Disable a specified port to operate in Private Edge VLAN mode.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number to set state.
*       enable    - GT_TRUE for enabled, GT_FALSE for disabled
*       dstPort   - the destination "uplink" physical port to which all traffic
*                   received on srcPort&srcDev is forwarded.  This parameter
*                   is ignored if disabling the mode.
*       dstDev    - the destination "uplink" physical device to which all
*                   traffic received on srcPort&srcDev is forwarded.  This
*                   parameter is ignored if disabling the mode.
*       dstTrunk  - the destination is a trunk member. This parameter
*                   is ignored if disabling the mode.
*                   GT_TRUE - dstPort hold value of trunkId , and dstDev is
*                             ignored
*                   GT_FALSE - dstPort hold value of port in device dstDev
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on bad portNum or devNum or dstPort or dstDev
*
* COMMENTS:
*
*******************************************************************************/

extern unsigned int cpssDxChBrgPrvEdgeVlanPortEnable
(
    unsigned char     	 devNum,
    unsigned char     	 portNum,
    NAM_BOOL    		 enable,
    unsigned char        dstPort,
    unsigned char        dstDev,
    NAM_BOOL    		 dstTrunk
);
extern unsigned int cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet
(                                                     
    unsigned char                            devNum,    
    CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  		 mode
);
extern unsigned int cpssDxChBrgSrcIdPortDefaultSrcIdSet
(
    unsigned char   devNum,    
    unsigned char   portNum,
    unsigned int  	defaultSrcId
);

extern unsigned int cpssDxChBrgSrcIdGlobalUcastEgressFilterSet
(    
    unsigned char    devNum,    
    NAM_BOOL         enable
);
extern unsigned int cpssDxChBrgSrcIdPortSrcIdAssignModeSet
(                                                     
    unsigned char                            devNum,    
    unsigned char                            portNum,
    CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT 		 mode
);
extern unsigned int cpssDxChBrgSrcIdPortUcastEgressFilterSet
(    
    unsigned char    devNum,    
    unsigned char    portNum,
    NAM_BOOL  enable
);
extern unsigned int prvCpssDxChBrgSrcIdGroupTableSet
(
    unsigned char   devNum,
    unsigned int  	sourceId,
    unsigned char   portNum,
    NAM_BOOL 		action
);
extern unsigned int cpssDxChBrgSrcIdGroupPortAdd
(
    unsigned char   devNum,
    unsigned int  	sourceId,
    unsigned char   portNum
);
extern unsigned int cpssDxChBrgSrcIdGroupPortDelete
(
    unsigned char   devNum,
    unsigned int    sourceId,
    unsigned char   portNum
);

extern unsigned int cpssDxChBrgSrcIdGroupEntrySet
(
    unsigned char               devNum,
    unsigned int              	sourceId,
    NAM_BOOL            	 	cpuSrcIdMember,  
    CPSS_PORTS_BMP_STC*			portsMembersPtr
);

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet 
*
* DESCRIPTION:
*       Enable/Disable per port forwarding control taffic to Private Edge 
*       VLAN Uplink. PVE port can be disabled from trapping or mirroring 
*       bridged packets to the CPU. In this case, as long as the packet is not 
*       assigned a HARD DROP or SOFT DROP, the packet is unconditionally 
*       assigned a FORWARD command with the ingress ports configured 
*       PVE destination (PVLAN Uplink).
*
* APPLICABLE DEVICES:  All DXCH2 devices
*
* INPUTS:
*       devNum  - device number
*       port    - physical or CPU port to set.
*       enable  - GT_TRUE  - forward control traffic to PVE Uplink
*                 GT_FALSE - not forward control traffic to PVE Uplink,
*                            trapped or mirrored to CPU packets are sent to
*                            CPU port
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong devNum, port
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*
*
*******************************************************************************/
extern unsigned int cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet 
(
    unsigned char       devNum,
    unsigned char       port,   
    NAM_BOOL			enable
);
#if 1
extern unsigned int (*npd_get_devport_by_global_index)
(
	unsigned int eth_g_index,
	unsigned char *devNum,
	unsigned char *portNum
);
#else
extern unsigned int npd_get_devport_by_global_index
(
	unsigned int eth_g_index,
	unsigned char *devNum,
	unsigned char *portNum
);
#endif
extern void gtPortsDevConvertBack
(
   unsigned char *devNum,
   unsigned char *portNum
);

#endif
int nam_pvlan_port_config(unsigned int eth_g_index1,unsigned int eth_g_index2);
int nam_pvlan_port_config_spi(unsigned int eth_g_index1,unsigned char dev,unsigned char port);

int nam_pvlan_trunk_config(unsigned int eth_g_index1,unsigned char trunkid);

int nam_pvlan_control_config(unsigned int eth_g_index1,int flag);

int nam_pvlan_port_delete(unsigned int eth_g_index1);

int nam_pvlan_create(unsigned int pvlanid);
int nam_pvlan_add_port(unsigned int eth_g_index1);
int nam_pvlan_delete_port(unsigned int eth_g_index1);
int nam_pvlan_cpu_port(unsigned int flag);

#endif
