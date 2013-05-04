/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* cpssDxChCscd.c
*
* DESCRIPTION:
*       Cascading implementation -- Cheetah.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpss/dxCh/dxChxGen/trunk/private/prvCpssDxChTrunk.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>

/*******************************************************************************
 * External usage variables
 ******************************************************************************/
/* number of registers 'per target device' in lion */
#define LION_DEV_MAP_TABLE_REG_PER_DEV_CNS  8


typedef struct
{
    GT_U16 xsmiAddr;
    GT_U16 xsmiData;
} XSMI_STC;

static XSMI_STC xsmi10GActions[] =
{
    /* PRE-EMPHASIS & AMPLITUDE: Set each lane to Pre-Emphasis of 0% */
    {0x8000,0x800a}, {0x8001,0x800a}, {0x8002,0x800a}, {0x8003,0x800a},

    /* Interpolator input 6 bits control/Ampl=4/PreEmp=0/ */
    {0xff28,0xc000}, {0xff28,0xc000}, {0xff29,0xc000}, {0xff29,0xc000},
    {0xff2a,0xc000}, {0xff2a,0xc000}, {0xff2b,0xc000}, {0xff2b,0xc000},

    /* CABLE (1-5m) SETUP */
    {0xff28,0xf030}, {0xff28,0xf030}, {0xff29,0xf030}, {0xff29,0xf030},
    {0xff2a,0xf030}, {0xff2a,0xf030}, {0xff2b,0xf030}, {0xff2b,0xf030},

    /* Loop Gain=0x0 */
    {0xff2c,0x2c01}, {0xff2c,0x2c01},

    /* XG RX PCS reset */
    {0xff34,0xff00}, {0xff34,0xff00}, {0xff34,0xe000}};

static XSMI_STC xsmi12GActions[] =
{
    /* PRE-EMPHASIS & AMPLITUDE: Set each lane to Pre-Emphasis of 0% */
    {0x8000,0x800a}, {0x8001,0x800a}, {0x8002,0x800a}, {0x8003,0x800a},

    /* Interpolator input 6 bits control/Ampl=4/PreEmp=0/ */
    {0xff28,0xc000}, {0xff28,0xc000}, {0xff29,0xc000}, {0xff29,0xc000},
    {0xff2a,0xc000}, {0xff2a,0xc000}, {0xff2b,0xc000}, {0xff2b,0xc000},

    /* CABLE (1-5m) SETUP */
    {0xff28,0xf030}, {0xff28,0xf030}, {0xff29,0xf030}, {0xff29,0xf030},
    {0xff2a,0xf030}, {0xff2a,0xf030}, {0xff2b,0xf030}, {0xff2b,0xf030},

    /* Loop Gain=0x0 */
    {0xff2c,0x2c01}, {0xff2c,0x2c01},

    /* Speed - 3.75G, Pre-Emphasis - 54% */
    {0x8000,0x813a},

    /* CABLE (1-5m) SETUP */
    {0xff28,0xf030}, {0xff28,0xf030}, {0xff29,0xf030}, {0xff29,0xf030},
    {0xff2a,0xf030}, {0xff2a,0xf030}, {0xff2b,0xf030}, {0xff2b,0xf030},

    /* Analog Receiver/Transmit Control 3.75G */
    {0xff2c,0x0c22}, {0xff2c,0x0c22},

    /* RX EQUALIZER */
    {0xff40,0x003f}, {0xff41,0x003f}, {0xff41,0x003f}, {0xff41,0x003f},

    /* XG RX PCS reset */
    {0xff34,0xff00}, {0xff34,0xff00}, {0xff34,0xe000}
};

/**************************** Forward declaration******************************/

static GT_STATUS cscdXsmiPortConfigurationSet
(
    IN  GT_U8               devNum,
    IN  GT_U8               portNum,
    IN  XSMI_STC            *xsmiActionPtr,
    IN  GT_U32              xsmiActionSize
);

/*******************************************************************************
* prvCpssDxChCscdCutThroughSet
*
* DESCRIPTION:
*           Enable Cut Through forwarding for cascaded port.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - The port to be configured for Cut Through forwarding
*       portType - cascade  type regular/extended DSA tag port or network port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChCscdCutThroughSet
(
    IN GT_U8                        devNum,
    IN GT_U8                        portNum,
    IN CPSS_CSCD_PORT_TYPE_ENT      portType
)
{
    GT_U32          value;       /* value of field */
    GT_U32          fieldOffset; /* The start bit number in the register */
    GT_U32          regAddr;     /* address of register */
    GT_U32          portGroupId;      /*the port group Id - support multi port groups device */
    GT_U8           localPort;   /* local port - support multi-port-group device */

    if (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        return GT_OK;
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily > CPSS_PP_FAMILY_DXCH_LION_E)
    {
        return GT_OK;
    }

    if(portType == CPSS_CSCD_PORT_NETWORK_E)
    {
        value = 0;
    }
    else
    {
        value = 1;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi port group device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    fieldOffset = (localPort == CPSS_CPU_PORT_NUM_CNS) ? 15 : localPort;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                       cutThroughRegs.ctCascadingPortReg;

    /* Enable Cut Through forwarding on cascaded port */
    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                               regAddr, fieldOffset, 1, value);
}

/*******************************************************************************
* cpssDxChCscdPortTypeSet
*
* DESCRIPTION:
*            Configure a PP port to be a cascade port. Application is responsible
*            for setting the default values of the port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - The port to be configured as cascade
*       portType - cascade  type regular/extended DSA tag port or network port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For Lion:
*       This function also configures insertion of DSA tag for PFC frames.
* 
*******************************************************************************/
GT_STATUS cpssDxChCscdPortTypeSet
(
    IN GT_U8                        devNum,
    IN GT_U8                        portNum,
    IN CPSS_CSCD_PORT_TYPE_ENT      portType
)
{
    GT_STATUS   rc;
    GT_U32          regAddr;  /* address of register */
    GT_U32          value;    /* value of field */
    GT_U32          index;    /* index of bit in the register */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */
    CPSS_DXCH_PORT_FC_MODE_ENT portFcMode;
    GT_BOOL enablePfcCascade;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    index = (localPort == CPSS_CPU_PORT_NUM_CNS) ?
                 PRV_CPSS_DXCH_CH1_SPECIAL_BIT_HA_CPU_PORT_BIT_INDEX_MAC(devNum,27) :
                 localPort;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                cscdHeadrInsrtConf[OFFSET_TO_WORD_MAC(index)];

    if((portType == CPSS_CSCD_PORT_DSA_MODE_REGULAR_E) ||
       (portType == CPSS_CSCD_PORT_DSA_MODE_EXTEND_E))
    {
        /* The port is a cascading port used for inter-device connectivity and
           all packets received by it and transmitted by it must be DSA-tagged*/
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, OFFSET_TO_BIT_MAC(index), 1, 1) != GT_OK)
            return GT_HW_ERROR;


        /* set DSA support mode */
        value = (portType == CPSS_CSCD_PORT_DSA_MODE_REGULAR_E) ? 0 : 1;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
            eggrDSATagTypeConf[OFFSET_TO_WORD_MAC(index)];

        /* set relevant bit of Egress DSA tag type Configuration register */
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, OFFSET_TO_BIT_MAC(index), 1, value) != GT_OK)
            return GT_HW_ERROR;

    }
    else if(portType == CPSS_CSCD_PORT_NETWORK_E)
    {
        /* The port is a regular network port and all packets received by it
           are standard Ethernet packets.  */
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, OFFSET_TO_BIT_MAC(index), 1, 0) != GT_OK)
            return GT_HW_ERROR;

    }
    else
    {
        return GT_BAD_PARAM;
    }

    rc = prvCpssDxChTrunkTrafficToCpuWaCpuPortEnableSet(devNum,portNum,
            (portType == CPSS_CSCD_PORT_NETWORK_E) ?
            GT_FALSE : GT_TRUE);
    if (rc != GT_OK)
          return rc;

    rc = prvCpssDxChCscdCutThroughSet(devNum, portNum, portType);
    if (rc != GT_OK)
    {
        return rc;
    }
    
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {   
        /* for cascad port and pfc mode, enable insertion of DSA tag to PFC packets */
        if (portType != CPSS_CSCD_PORT_NETWORK_E)
        {
            rc = cpssDxChPortFlowControlModeGet(devNum,portNum,&portFcMode);
            if (rc != GT_OK)
            {
                return rc;
            }
            enablePfcCascade = (portFcMode == CPSS_DXCH_PORT_FC_MODE_PFC_E)
               ? GT_TRUE : GT_FALSE;
        }
        else
        {
            enablePfcCascade = GT_FALSE;
        }

        rc = prvCpssDxChPortPfcCascadeEnableSet(devNum,portNum,enablePfcCascade);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCscdPortTypeGet
*
* DESCRIPTION:
*            Retreive a PP port cascade port configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - port number or CPU port
*
* OUTPUTS:
*       portTypePtr - (pointer to) cascade type regular/extended DSA tag port
*                     or network port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortTypeGet
(
    IN GT_U8                        devNum,
    IN GT_U8                        portNum,
    OUT CPSS_CSCD_PORT_TYPE_ENT     *portTypePtr
)
{
    GT_STATUS       rc;       /* return code */
    GT_U32          regAddr;  /* address of register */
    GT_U32          value;    /* value of field */
    GT_U32          index;    /* index of bit in the register */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(portTypePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    index = (localPort == CPSS_CPU_PORT_NUM_CNS) ?
                 PRV_CPSS_DXCH_CH1_SPECIAL_BIT_HA_CPU_PORT_BIT_INDEX_MAC(devNum,27) :
                 localPort;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                cscdHeadrInsrtConf[OFFSET_TO_WORD_MAC(index)];

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, OFFSET_TO_BIT_MAC(index), 1, &value);
    if( rc != GT_OK )
    {
        return rc;
    }

    if( value == 0)
    {
        *portTypePtr = CPSS_CSCD_PORT_NETWORK_E;
    }
    else /* value == 1 */
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
            eggrDSATagTypeConf[OFFSET_TO_WORD_MAC(index)];
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, OFFSET_TO_BIT_MAC(index), 1, &value);
        if( rc != GT_OK )
        {
            return rc;
        }

        if( value == 0 )
        {
            *portTypePtr = CPSS_CSCD_PORT_DSA_MODE_REGULAR_E;
        }
        else
        {
            *portTypePtr = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChCscdDevMapTableIndexGet
*
* DESCRIPTION:
*       Get index in index in Device Map table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number
*       targetDevNum   - the remote device to be reached via cascade (0..31)
*       targetPortNum  - target port number 
*
* OUTPUTS:
*       indexPtr       - pointer to index in Device Map table.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or target device 
*                                  or bad target Port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChCscdDevMapTableIndexGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     targetDevNum,
    IN  GT_U8     targetPortNum,
    OUT GT_U32    *indexPtr
)
{
    CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT  mode;     /* device Map lookup mode. */
    GT_STATUS                          rc;       /* return status */  

    rc = cpssDxChCscdDevMapLookupModeGet(devNum, &mode);
    if(rc != GT_OK)
    {
        return rc;
    }
    if(mode == CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E)
    {
        if(targetPortNum >= BIT_6)
        {
            return GT_BAD_PARAM;
        }
        *indexPtr = targetPortNum | (targetDevNum << 6);
    }
    else
    {
        *indexPtr = targetDevNum;
    }

    return GT_OK;

}

/*******************************************************************************
* prvCpssDxChCscdDevMapTableEntryHwFormatGet
*
* DESCRIPTION:
*       Get Cascade Device Map table entry in HW format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - physical device number
*       cascadeLinkPtr - (pointer to)A structure holding the cascade link type
*                      (port (0..63) or trunk(0..127)) and the link number
*                      leading to the target device.
*       srcPortTrunkHashEn - Relevant when (cascadeLinkPtr->linkType ==
*                       CPSS_CSCD_LINK_TYPE_TRUNK_E)
*                       Enabled to set the load balancing trunk hash for packets
*                       forwarded via an trunk uplink to be based on the
*                       packet’s source port, and not on the packets data.
*                       Indicates the type of uplink.
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E -
*                       Load balancing trunk hash is based on the ingress pipe
*                       hash mode as configured by function
*                       cpssDxChTrunkHashGeneralModeSet(...)
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E - Load
*                       balancing trunk hash for this cascade trunk interface is
*                       based on the packet’s source port, regardless of the
*                       ingress pipe hash mode
*                       NOTE : this parameter is relevant only to DXCH2
*                       and above devices
*
* OUTPUTS:
*       hwDataPtr       - Pointer to Device Map Table entry format data.  
*       numBitsPtr      - Pointer to number of bits in the entry.                           
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad trunk hash mode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChCscdDevMapTableEntryHwFormatGet
(
    IN  GT_U8                        devNum,
    IN  CPSS_CSCD_LINK_TYPE_STC      *cascadeLinkPtr,
    IN  CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT srcPortTrunkHashEn,
    OUT GT_U32                       *hwDataPtr,
    OUT GT_U32                       *numBitsPtr
)
{

    CPSS_NULL_PTR_CHECK_MAC(cascadeLinkPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        *numBitsPtr = 10;
    }
    else
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        *numBitsPtr = 8;
    }
    else
    {
        *numBitsPtr = 9;
    }

    switch(cascadeLinkPtr->linkType)
    {
        case CPSS_CSCD_LINK_TYPE_TRUNK_E:
            if(cascadeLinkPtr->linkNum >= BIT_7)
            {
                return GT_BAD_PARAM;
            }
            *hwDataPtr = BIT_0;

            if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
            {
                switch(srcPortTrunkHashEn)
                {
                    case CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E:
                        break;
                    case CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E:
                        *hwDataPtr |= BIT_8;
                        break;
                    case CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_DST_PORT_E:
                        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
                        {
                            return GT_BAD_PARAM;
                        }
                        else
                        {
                            *hwDataPtr |= BIT_9;
                        }
                        break;
                    default:
                        return GT_BAD_PARAM;
                }
            }

            break;
        case CPSS_CSCD_LINK_TYPE_PORT_E:
            if(cascadeLinkPtr->linkNum >= BIT_6)
            {
                return GT_BAD_PARAM;
            }
            *hwDataPtr = 0;

            break;
        default:
            return GT_BAD_PARAM;
    }


    *hwDataPtr |= (cascadeLinkPtr->linkNum << 1);  /* set bits 1..7 */

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCscdDevMapTableSet
*
* DESCRIPTION:
*            Set the cascade map table . the map table define the local port or
*            trunk that packets destined to a destination device
*            should be transmitted to.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number
*       targetDevNum   - the device to be reached via cascade (0..31)
*       targetPortNum  - target port number
*                        Relevant only for Lion and
*                        CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E mode,
*                        otherwise ignored.
*       cascadeLinkPtr - (pointer to)A structure holding the cascade link type
*                      (port (0..63) or trunk(0..127)) and the link number
*                      leading to the target device.
*       srcPortTrunkHashEn - Relevant when (cascadeLinkPtr->linkType ==
*                       CPSS_CSCD_LINK_TYPE_TRUNK_E)
*                       Enabled to set the load balancing trunk hash for packets
*                       forwarded via an trunk uplink to be based on the
*                       packet’s source port, and not on the packets data.
*                       Indicates the type of uplink.
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E -
*                       Load balancing trunk hash is based on the ingress pipe
*                       hash mode as configured by function
*                       cpssDxChTrunkHashGeneralModeSet(...)
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E - Load
*                       balancing trunk hash for this cascade trunk interface is
*                       based on the packet’s source port, regardless of the
*                       ingress pipe hash mode
*                       NOTE : this parameter is relevant only to DXCH2
*                       and above devices
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on wrong device or target device or target port
*                          or bad trunk hash mode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdDevMapTableSet
(
    IN GT_U8                        devNum,
    IN GT_U8                        targetDevNum,
    IN GT_U8                        targetPortNum,
    IN CPSS_CSCD_LINK_TYPE_STC      *cascadeLinkPtr,
    IN CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT srcPortTrunkHashEn
)
{
    GT_U32       value;
    GT_U32       regAddr;      /* register address */
    GT_U32       numBits;      /* number of bit to update */
    GT_STATUS    rc;           /* return status */  
    GT_U32       index = 0;    /* index in device Map table */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(cascadeLinkPtr);

    if(targetDevNum >= BIT_5)
    {
        return GT_BAD_PARAM;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = prvCpssDxChCscdDevMapTableIndexGet(devNum, targetDevNum,
                                                targetPortNum, &index);
        if(rc != GT_OK)
        {
            return rc;
        }

    }

    /* Get Cascade Device Map table entry in HW format */
    rc = prvCpssDxChCscdDevMapTableEntryHwFormatGet(devNum, cascadeLinkPtr, 
                                                    srcPortTrunkHashEn,
                                                    &value, &numBits);
    if(rc != GT_OK)
    {
        return rc;

    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.sht.egressTables.deviceMapTableBaseAddress
            +  0x4 * index;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.devMapTbl[targetDevNum];
    }

        return prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, numBits, value);
    }

/*******************************************************************************
* prvCpssDxChCscdDevMapTableEntryGet
*
* DESCRIPTION:
*            Get the cascade map table entry data from hardware format. 
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number
*       hwData         - hardware format data
*
* OUTPUTS:
*       cascadeLinkPtr - (pointer to)A structure holding the cascade link type
*                      (port (0..63) or trunk(0..127)) and the link number
*                      leading to the target device.
*       srcPortTrunkHashEnPtr - Relevant when (cascadeLinkPtr->linkType ==
*                       CPSS_CSCD_LINK_TYPE_TRUNK_E)
*                       (pointer to) Enabled to set the load balancing trunk
*                       hash for packets forwarded via an trunk uplink to be
*                       based on the packet’s source port, and not on the
*                       packets data.
*                       Indicates the type of uplink.
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E -
*                       Load balancing trunk hash is based on the ingress pipe
*                       hash mode as configured by function
*                       cpssDxChTrunkHashGeneralModeSet(...)
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E - Load
*                       balancing trunk hash for this cascade trunk interface is
*                       based on the packet’s source port, regardless of the
*                       ingress pipe hash mode
*                       NOTE : this parameter is relevant only to DXCH2 and
*                       above devices
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChCscdDevMapTableEntryGet 
(
    IN GT_U8                         devNum,
    IN GT_U32                        hwData, 
    OUT CPSS_CSCD_LINK_TYPE_STC      *cascadeLinkPtr,
    OUT CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT *srcPortTrunkHashEnPtr
)
{

    CPSS_NULL_PTR_CHECK_MAC(cascadeLinkPtr);

    cascadeLinkPtr->linkType = U32_GET_FIELD_MAC(hwData,0,1) ?
                    CPSS_CSCD_LINK_TYPE_TRUNK_E :
                    CPSS_CSCD_LINK_TYPE_PORT_E;
    cascadeLinkPtr->linkNum = U32_GET_FIELD_MAC(hwData,1,7);

    if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* check null pointer only when cheetah2 and above */
        CPSS_NULL_PTR_CHECK_MAC(srcPortTrunkHashEnPtr);

        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
        {
            /* set the parameter regardless to the port/trunk target */
            *srcPortTrunkHashEnPtr = U32_GET_FIELD_MAC(hwData,8,1) ?
                CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E :
                CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E;
        }
        else
        {
            switch (U32_GET_FIELD_MAC(hwData,8,2))
            {
                case 0:
                    *srcPortTrunkHashEnPtr = CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E;
                    break;
                case 1:
                    *srcPortTrunkHashEnPtr = CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E;
                    break;
                case 2:
                    *srcPortTrunkHashEnPtr = CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_DST_PORT_E;
                    break;
                default:
                    break;
            }
        }
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChCscdDevMapTableGet
*
* DESCRIPTION:
*            Get the cascade map table . the map table define the local port or
*            trunk that packets destined to a destination device 
*            should be transmitted to.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number
*       targetDevNum   - the device to be reached via cascade (0..31)
*       targetPortNum  - target port number
*                        Used only for Lion and
*                        CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E mode, 
*                        otherwise ignored. 
*
* OUTPUTS:
*       cascadeLinkPtr - (pointer to)A structure holding the cascade link type
*                      (port (0..63) or trunk(0..127)) and the link number
*                      leading to the target device.
*       srcPortTrunkHashEnPtr - Relevant when (cascadeLinkPtr->linkType ==
*                       CPSS_CSCD_LINK_TYPE_TRUNK_E)
*                       (pointer to) Enabled to set the load balancing trunk
*                       hash for packets forwarded via an trunk uplink to be
*                       based on the packet’s source port, and not on the
*                       packets data.
*                       Indicates the type of uplink.
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E -
*                       Load balancing trunk hash is based on the ingress pipe
*                       hash mode as configured by function
*                       cpssDxChTrunkHashGeneralModeSet(...)
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E - Load
*                       balancing trunk hash for this cascade trunk interface is
*                       based on the packet’s source port, regardless of the
*                       ingress pipe hash mode
*                       NOTE : this parameter is relevant only to DXCH2 and
*                       above devices
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on wrong device or target device or target port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdDevMapTableGet
(
    IN GT_U8                         devNum,
    IN GT_U8                         targetDevNum,
    IN GT_U8                         targetPortNum,   
    OUT CPSS_CSCD_LINK_TYPE_STC      *cascadeLinkPtr,
    OUT CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT *srcPortTrunkHashEnPtr
)
{
    GT_STATUS           rc;       /* return code */
    GT_U32              value;    /* register value */
    GT_U32              regAddr;  /* register address */
    GT_U32              index;    /* index in device Map table */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(targetDevNum >= BIT_5)
    {
        return GT_BAD_PARAM;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.devMapTbl[targetDevNum];
        rc = prvCpssDxChHwPpGetRegField(devNum,regAddr, 0, 9, &value);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        rc = prvCpssDxChCscdDevMapTableIndexGet(devNum, targetDevNum,
                                                targetPortNum, &index);
        if(rc != GT_OK)
        {
            return rc;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.sht.egressTables.deviceMapTableBaseAddress
            +  0x4 * index;

        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 10, &value);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* Get the cascade map table entry data from hardware format. */
    return prvCpssDxChCscdDevMapTableEntryGet(devNum, value, cascadeLinkPtr,
                                              srcPortTrunkHashEnPtr);

}



/*******************************************************************************
* cpssDxChCscdRemapQosModeSet
*
*
* DESCRIPTION:
*       Enables/disables remapping of Tc and Dp for Data and Control Traffic
*       on a port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - physical device number
*       portNum      - physical port number
*       remapType    - traffic type to remap
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdRemapQosModeSet
(
   IN GT_U8                             devNum,
   IN GT_U8                             portNum,
   IN CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT remapType
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;    /* register address */
    GT_U32      fieldValue; /* registers field value */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if(remapType > CPSS_DXCH_CSCD_QOS_REMAP_ALL_E)
        return GT_BAD_PARAM;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    switch (remapType)
    {
        /* Disable TC and DP remapping */
        case CPSS_DXCH_CSCD_QOS_REMAP_DISABLED_E:
            fieldValue = 0;
            break;
        /* Enable TC and DP remap for control packets */
        case CPSS_DXCH_CSCD_QOS_REMAP_CNTRL_ONLY_E:
            fieldValue = 1;
            break;
        /* Enable TC and DP remap for data packets */
        case CPSS_DXCH_CSCD_QOS_REMAP_DATA_ONLY_E:
            fieldValue = 2;
            break;
        /* Enable TC and DP remapping */
        case CPSS_DXCH_CSCD_QOS_REMAP_ALL_E:
            fieldValue = 3;
            break;
        /* Wrong QoS remap mode type */
        default:
            return GT_BAD_PARAM;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        PRV_CPSS_DXCH_PORT_TXQ_REG_MAC(devNum,portNum,&regAddr);

        /* Enable TC and DP remapping */
        return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 22, 2, fieldValue);
    }
    else
    {
        /* control remap */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.distributor.
            controlTcRemapEn[OFFSET_TO_WORD_MAC(portNum)];

        rc =  prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId,
                regAddr,OFFSET_TO_BIT_MAC(portNum), 1,
                (fieldValue & 1)? 1 : 0);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* data remap */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.distributor.
            dataTcRemapEn[OFFSET_TO_WORD_MAC(portNum)];

        rc =  prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId,
                regAddr,OFFSET_TO_BIT_MAC(portNum), 1,
                (fieldValue & 2) ? 1 : 0);

        return rc;
    }
}

/*******************************************************************************
* cpssDxChCscdCtrlQosSet
*
* DESCRIPTION:
*       Set control packets TC and DP if Control Remap QoS enabled on a port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number
*       ctrlTc      - The TC assigned to control packets forwarded to
*                     cascading ports (0..7)
*       ctrlDp      - The DP assigned to CPU-to-network control traffic or
*                     network-to-CPU traffic.
*                     DxCh devices support only :
*                     CPSS_DP_RED_E and CPSS_DP_GREEN_E
*       cpuToCpuDp  - The DP assigned to CPU-to-CPU control traffic
*                     DxCh devices support only :
*                     CPSS_DP_RED_E and CPSS_DP_GREEN_E
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_VALUE     - on wrong ctrlDp or cpuToCpuDp level value or tc value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCtrlQosSet
(
   IN GT_U8             devNum,
   IN GT_U8             ctrlTc,
   IN CPSS_DP_LEVEL_ENT ctrlDp,
   IN CPSS_DP_LEVEL_ENT cpuToCpuDp
)
{
    GT_STATUS   status;
    GT_U32      regAddr;    /* register address                 */
    GT_U32      value;      /* field value */
    GT_U32      startBit;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_DXCH_COS_CHECK_DP_MAC(cpuToCpuDp);
    PRV_CPSS_DXCH_COS_CHECK_DP_MAC(ctrlDp);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(ctrlTc);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;
        startBit = 6;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.distributor.generalConfig;
        startBit = 0;
    }

    /* The TC assigned to control packets forwarded to cascading ports. */
    status = prvCpssDxChHwPpSetRegField(devNum, regAddr, startBit + 0, 3, ctrlTc);
    if (status != GT_OK)
    {
        return status;
    }

    if (cpuToCpuDp == CPSS_DP_GREEN_E)
    {
        value = 0;
    }
    else
    {
        value = 1;
    }
    /* The DP assigned to CPU-to-CPU control traffic */
    status = prvCpssDxChHwPpSetRegField(devNum, regAddr, startBit + 3, 1, value);
    if (status != GT_OK)
    {
        return status;
    }

    if (ctrlDp == CPSS_DP_GREEN_E)
    {
        value = 0;
    }
    else
    {
        value = 1;
    }

    /* The DP assigned to CPU-to-network control traffic or
       network-to-CPU traffic. */
    return prvCpssDxChHwPpSetRegField(devNum, regAddr, startBit + 5, 1, value);
}

/*******************************************************************************
* cpssDxChCscdRemapDataQosTblSet
*
* DESCRIPTION:
*       Set table to remap Data packets QoS parameters
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number
*       tc          - original packet TC (0..7)
*       dp          - original packet DP
*                     DxCh devices support only :
*                     CPSS_DP_RED_E and CPSS_DP_GREEN_E
*       remapTc     - TC assigned to data packets with DP and TC (0..7)
*       remapDp     - DP assigned to data packets with DP and TC
*                     DxCh devices support only :
*                     CPSS_DP_RED_E and CPSS_DP_GREEN_E
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_VALUE             - on wrong DP or dp level value or tc value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdRemapDataQosTblSet
(
   IN GT_U8              devNum,
   IN GT_U8              tc,
   IN CPSS_DP_LEVEL_ENT  dp,
   IN GT_U8              remapTc,
   IN CPSS_DP_LEVEL_ENT  remapDp
)
{
    GT_STATUS   status;
    GT_U32      regAddr;    /* register address */
    GT_U32      offset;     /* field offset */
    GT_U32      value;      /* field value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_COS_CHECK_DP_MAC(dp);
    PRV_CPSS_DXCH_COS_CHECK_DP_MAC(remapDp);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tc);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(remapTc);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        if (dp == CPSS_DP_GREEN_E)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.tcDp0CscdDataRemap;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.tcDp1CscdDataRemap;
        }
    }
    else
    {
        if (dp == CPSS_DP_GREEN_E)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.distributor.tcDpRemapping[0];
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.distributor.tcDpRemapping[1];
        }

    }

    offset = 3 * tc;
    value = remapTc;
    /* Set TC field */
    status = prvCpssDxChHwPpSetRegField(devNum, regAddr, offset, 3, value);
    if (status != GT_OK)
    {
        return status;
    }

    offset = 24 + tc;
    value = (remapDp == CPSS_DP_GREEN_E) ? 0 : 1;
    /* Set DP field */
    status = prvCpssDxChHwPpSetRegField(devNum, regAddr, offset, 1, value);

    return status;
}

/*******************************************************************************
* cpssDxChCscdDsaSrcDevFilterSet
*
* DESCRIPTION:
*       Enable/Disable filtering the ingress DSA tagged packets in which
*       source id equals to local device number.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - physical device number
*       enableOwnDevFltr - enable/disable ingress DSA loop filter
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*******************************************************************************/
GT_STATUS cpssDxChCscdDsaSrcDevFilterSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enableOwnDevFltr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      value;      /* field value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* getting register address */
    regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
               bridgeRegs.bridgeGlobalConfigRegArray[1];

    /* getting the bit value to be set */
    value = (enableOwnDevFltr == GT_TRUE) ? 0 : 1;

    /* set the DSA loop filter DISABLE bit */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 31, 1, value);
}

/*******************************************************************************
* cpssDxChCscdHyperGInternalPortModeSet
*
* DESCRIPTION:
*       Change HyperG.stack internal port speed.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum       - physical device number
*       portNum      - cascade HyperG port to be configured
*       mode         - HyperG.Stack Port Speed 10Gbps or 12Gbps
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - the request is not supported
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdHyperGInternalPortModeSet
(
    IN  GT_U8                                devNum,
    IN  GT_U8                                portNum,
    IN  CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT mode
)
{
    GT_STATUS status;
    GT_U32    value;
    GT_U32    regAddr;
    GT_U32    xsmiArrSize;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* check mode */
    switch (mode)
    {
        case CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ACCELERATED_E: 
        case CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_STANDART_E:
            break;
        default: 
            return GT_BAD_PARAM;
    }

    if(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType <
       PRV_CPSS_PORT_XG_E)
    {
        /* supported only on XG ports */
        return GT_NOT_SUPPORTED;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
              smiRegs.smi10GePhyConfig0[portNum];

    /* read XAUI PHY External Configuration */
    status = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,regAddr, &value);
    if (status != GT_OK)
    {
        return status;
    }

    /*  Set XAUI PHY External Configuration register
        1. Set PHY to Reset state - clear bit 0
        2. Clear SpeedSelect field - bits 6,7 */
    value &= 0xFFFFFF3E;


    /* HyperG.Stack Port Speed 10Gbps/12Gbps */
    if (mode == CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ACCELERATED_E)
    {
        value |= 0x40;
    }

    /* write register to set PHY status to RESET and configure speed */
    status = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, value);
    if (status != GT_OK)
    {
        return status;
    }

    /* set first bit to Unreset PHY*/
    value |= 1;

    
#ifndef __AX_PLATFORM__
	/* set second bit to PowerUP XAUI PHY */
	value |= 0x2;
#endif

    /* Unreset XAUI PHY */
    status = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, value);
    if (status != GT_OK)
    {
        return status;
    }

    
#ifdef AX_FULL_DEBUG
	osPrintf("cpssDxChCscdHyperGInternalPortModeSet::write reg %#0x data %#0x ",regAddr,value);
	osPrintf("and then read %#0x\r\n",value);
#endif
#ifndef __AX_PLATFORM__
	/*
	  * set Port<n> XAUI PHY External Configuration Register1
	  * bit5	-AnyDevAddrEn,value 1.(responds to SMI transaction with any other device address)
	  * bit4:0	-Ignored when bit5 is 1.
	  */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GePhyConfig1[portNum];
	value = 0;
    /* read XAUI PHY External Configuration Register1*/
    status = prvCpssDrvHwPpReadRegister(devNum, regAddr, &value);
    if (status != GT_OK)
    {
        return status;
    }

	/* set bit5 to 1 */
	value |= (1<<5);
    /* set XAUI PHY Register1 AnyDevAddrEn */
    status = prvCpssDrvHwPpWriteRegister(devNum, regAddr, value);
    if (status != GT_OK)
    {
        return status;
    }
	prvCpssDrvHwPpReadRegister(devNum,regAddr,&value);
#ifdef AX_FULL_DEBUG
	osPrintf("cpssDxChCscdHyperGInternalPortModeSet::write reg %#0x data %#0x ",regAddr,value);
	osPrintf("and then read %#0x\r\n",value);
#endif
#endif


    /* Set XAUI PHYs for specific device */
    if (mode == CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_STANDART_E)
    {
        xsmiArrSize =
            sizeof(xsmi10GActions)/sizeof(XSMI_STC);

        status = cscdXsmiPortConfigurationSet(devNum,portNum,
                                xsmi10GActions, xsmiArrSize);
    }
    else
    {
        xsmiArrSize =
            sizeof(xsmi12GActions)/sizeof(XSMI_STC);

        status = cscdXsmiPortConfigurationSet(devNum,portNum,
                                xsmi12GActions, xsmiArrSize);
    }

    if (status != GT_OK)
    {
        return status;
    }

    /* configure IPG and preamble to get maximal bandwidth on cascade ports. */
    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);

    if (mode == CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ACCELERATED_E)
    {
        /* Fixed IPG uses 8 bytes as the base IPG. */
        value = 0x400;
    }
    else
    {   /* Fixed IPG uses 12 bytes as the base IPG */
        value = 0x0;
    }

    /* Set fixed ipg base */
    status = prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId,regAddr, 0x400, value);
    if (status != GT_OK)
    {
        return status;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);


    if (mode == CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ACCELERATED_E)
    {
        /* 1. Fixed IPG LAN mode - Adds up to three idle symbols to
             <FixedIPGBase> = 8 bytes, to align the start symbol to lane 0.
           2. Preamble length for Tx and Rx 4 bytes
              (including SFD and Start Symbol)*/
        value = 0x80;
    }
    else
    {
        /* 1. LAN PHY - Maintains a Deficit Idle Count (DIC) which is used to
              decide whether to add or delete idle characters and maintain an
              average IPG 96BT to 192BT, in steps of 32BT.
          2. Preamble length for Tx 8 bytes for Rx 4 bytes
              (including SFD and Start Symbol)*/
        value = 0x2000;
    }

    /* Set IPG mode + preamble mode */
    return prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId,regAddr, 0x30f0, value);
}

/*******************************************************************************
* cpssDxChCscdHyperGPortCrcModeSet
*
* DESCRIPTION:
*       Set CRC mode to be standard 4 bytes or proprietary one byte CRC mode.
*
* APPLICABLE DEVICES:
*        DxCh2; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3.
*
* INPUTS:
*       devNum           - physical device number
*       portNum          - port number
*       portDirection    - TX/RX cascade port direction
*       crcMode          - TX/RX HyperG.link CRC Mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdHyperGPortCrcModeSet
(
    IN GT_U8                             devNum,
    IN GT_U8                             portNum,
    IN CPSS_PORT_DIRECTION_ENT           portDirection,
    IN CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT  crcMode
)
{
    GT_STATUS status;               /* return status */
    GT_U32 value;                   /* write register's value */
    GT_U32 hwCrcMode;               /* CRC bit value */
    GT_U32 hwIvalidCrcMode;         /* invalid CRC mode value */
    GT_BOOL inPort = GT_FALSE;      /* ingress or egress port */
    GT_U32 regAddr;                 /* register address */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    switch (crcMode)
    {
        case CPSS_DXCH_CSCD_PORT_CRC_FOUR_BYTES_E:
            hwCrcMode = 1;
            break;
        case CPSS_DXCH_CSCD_PORT_CRC_ONE_BYTE_E:
            hwCrcMode = 0;
            break;
        default:
            return GT_BAD_PARAM;
    }

    switch (portDirection)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            value = hwCrcMode << 12;
            inPort = GT_TRUE;
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            value = hwCrcMode << 13;
            break;
        case CPSS_PORT_DIRECTION_BOTH_E:
            value = (hwCrcMode << 12) | (hwCrcMode << 13);
            inPort = GT_TRUE;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);

    status = prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId,regAddr, 0x3000, value);
    if (status != GT_OK)
    {
        return status;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
              haRegs.invalidCrcModeConfigReg[OFFSET_TO_WORD_MAC(localPort)];

    hwIvalidCrcMode = (crcMode == CPSS_DXCH_CSCD_PORT_CRC_FOUR_BYTES_E) ? 0 : 1;
    if (inPort == GT_TRUE)
    {
        /* Set invalid CRC format for relevant ingress port */
        status = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr,
            OFFSET_TO_BIT_MAC(localPort), 1, hwIvalidCrcMode);

    }

    return status;
}

/*******************************************************************************
* cpssDxChCscdFastFailoverFastStackRecoveryEnableSet
*
* DESCRIPTION:
*       Enable/Disable fast stack recovery.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       enable           - GT_TRUE: enable fast stack recovery
*                          GT_FALSE: disable fast stack recovery
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverFastStackRecoveryEnableSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      hwValue;    /* hw field value   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    /* getting register address */
    regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.fastStack;

    /* getting the bit value to be set */
    hwValue = BOOL2BIT_MAC(enable);

    /* set the Fast Stack Enable bit */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 1, hwValue);
}

/*******************************************************************************
* cpssDxChCscdFastFailoverFastStackRecoveryEnableGet
*
* DESCRIPTION:
*       Get the status of fast stack recovery (Enabled/Disabled).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       enablePtr        - (pointer to) fast stack recovery status
*                          GT_TRUE: fast stack recovery enabled
*                          GT_FALSE: fast stack recovery disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverFastStackRecoveryEnableGet
(
    IN  GT_U8        devNum,
    OUT GT_BOOL      *enablePtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      hwValue;    /* hw field value   */
    GT_STATUS   rc;         /* return code      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* getting register address */
    regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.fastStack;

    /* get the Fast Stack Enable bit */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 1, &hwValue);

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;
}

/*******************************************************************************
* cpssDxChCscdFastFailoverSecondaryTargetPortMapSet
*
* DESCRIPTION:
*       Set secondary target port map.
*       If the device is the device where the ring break occured, the
*       packet is looped on the ingress port and is egressed according to
*       Secondary Target Port Map. Also "packetIsLooped" bit is
*       set in DSA tag.
*       If the device receives a packet with "packetIsLooped" bit is set
*       in DSA tag, the packet is forwarded according to Secondary Target Port
*       Map.
*       Device MAP table (cpssDxChCscdDevMapTableSet) is not used in the case.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - ingress port number
*       secondaryTargetPort - secondary target port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - when secondaryTargetPort is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverSecondaryTargetPortMapSet
(
    IN GT_U8        devNum,
    IN GT_U8        portNum,
    IN GT_U8        secondaryTargetPort
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  portGroupId;/*the port group Id of primary port - support multi-port-groups device */
    GT_U32  secondaryPortGroupId;/*the port group Id of secondary port - support multi-port-groups device */
    GT_U32  localSecondaryPort; /* the local port number of the secondary port */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {

        /* getting register address */
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.
                                                secondaryTargetPortMapTbl[portNum];

        localSecondaryPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,
                                                                            secondaryTargetPort);

        if( localSecondaryPort >= BIT_5 )
        {
            return GT_BAD_PARAM;
        }

        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_FAST_FAILOVER_WA_E))
        {
            secondaryPortGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                                     secondaryTargetPort);
            /* the secondary port must be in the same port group as the primary one */
            if( portGroupId != secondaryPortGroupId )
            {
                return GT_BAD_PARAM;
            }
        }

        /* set the Fast Stack Failover secondary port */
        return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 0, 5, localSecondaryPort);
    }
    else
    {
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.
            devMapTableConfig.secondaryTargetPortTable[portNum];

        if( secondaryTargetPort >= BIT_6 )
        {
            return GT_BAD_PARAM;
        }

        /* set the Fast Stack Failover secondary port */
        return prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 0, 6, secondaryTargetPort);
    }

}

/*******************************************************************************
* cpssDxChCscdFastFailoverSecondaryTargetPortMapGet
*
* DESCRIPTION:
*       Get Secondary Target Port Map for given device.
*       If the device is the device where the ring break occured, the
*       packet is looped on the ingress port and is egressed according to
*       Secondary Target Port Map. Also "packetIsLooped" bit is
*       set in DSA tag.
*       If the device receives a packet with "packetIsLooped" bit is set
*       in DSA tag, the packet is forwarded according to Secondary Target Port
*       Map.
*       Device MAP table (cpssDxChCscdDevMapTableSet) is not used in the case.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - ingress port number
*
* OUTPUTS:
*       secondaryTargetPortPtr - (pointer to) secondary target port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverSecondaryTargetPortMapGet
(
    IN  GT_U8        devNum,
    IN  GT_U8        portNum,
    OUT GT_U8        *secondaryTargetPortPtr
)
{
    GT_U32      regAddr;    /* register address             */
    GT_U32      hwValue;    /* value to read from register  */
    GT_STATUS   rc;         /* return code                  */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(secondaryTargetPortPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* getting register address */
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.
                                            secondaryTargetPortMapTbl[portNum];

        /* get the Secondary target port */
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0, 5, &hwValue);

        *secondaryTargetPortPtr =
            PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum, portGroupId, (GT_U8)hwValue);
    }
    else
    {
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.
            devMapTableConfig.secondaryTargetPortTable[portNum];

        /* set the Fast Stack Failover secondary port */
        rc = prvCpssDxChHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 0, 6, &hwValue);

        *secondaryTargetPortPtr = (GT_U8)hwValue;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableSet
*
* DESCRIPTION:
*       Enable/Disable fast failover loopback termination
*       for single-destination packets.
*       There are two configurable options for forwarding single-destination
*       packets that are looped-back across the ring:
*       - Termination Disabled.
*         Unconditionally forward the looped-back packet to the configured
*         backup ring port (for the given ingress ring port) on all the ring
*         devices until it reaches the far-end device where it is again
*         internally looped-back on the ring port and then forward it normally.
*       - Termination Enabled.
*         The looped-back packet passes through the
*         ring until it reaches the target device where it is egressed on its
*         target port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       enable           - GT_TRUE: enable terminating local loopback
*                                   for fast stack recovery
*                          GT_FALSE: disable terminating local loopback
*                                   for fast stack recovery
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
)
{
    GT_U32      regAddr;    /* register address             */
    GT_U32      hwValue;    /* value to write to register   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    hwValue = BOOL2BIT_MAC(enable);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* getting register address */
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.
                                                            txQueueDpToCfiReg;
        /* set the local loopback termination for fast stack bit */
        return prvCpssDrvHwPpSetRegField(devNum, regAddr, 4, 1, hwValue);
    }
    else
    {
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.
            filterConfig.globalEnables;

        /* set the Enable ending of loopback on Fast Stack Failover */
        return prvCpssDxChHwPpSetRegField(devNum, regAddr, 2, 1, hwValue);
    }
}

/*******************************************************************************
* cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet
*
* DESCRIPTION:
*       Get the status of fast failover loopback termination
*       for single-destination packets (Enabled/Disabled).
*       There are two configurable options for forwarding single-destination
*       packets that are looped-back across the ring:
*       - Termination Disabled.
*         Unconditionally forward the looped-back packet to the configured
*         backup ring port (for the given ingress ring port) on all the ring
*         devices until it reaches the far-end device where it is again
*         internally looped-back on the ring port and then forward it normally.
*       - Termination Enabled.
*         The looped-back packet passes through the
*         ring until it reaches the target device where it is egressed on its
*         target port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       enablePtr        - GT_TRUE: terminating local loopback
*                                   for fast stack recovery enabled
*                          GT_FALSE: terminating local loopback
*                                   for fast stack recovery disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet
(
    IN  GT_U8        devNum,
    OUT GT_BOOL      *enablePtr
)
{
    GT_U32      regAddr;    /* register address             */
    GT_U32      hwValue;    /* value to write to register   */
    GT_STATUS   rc;         /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* getting register address */
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.
                                                            txQueueDpToCfiReg;
        /* get the local loopback termination for fast stack bit */
        rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 4, 1, &hwValue);
    }
    else
    {
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.
            filterConfig.globalEnables;
        /* get the Enable ending of loopback on Fast Stack Failover */
        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 2, 1, &hwValue);
    }

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;
}

/*******************************************************************************
* cpssDxChCscdFastFailoverPortIsLoopedSet
*
* DESCRIPTION:
*       Enable/disable Fast Failover on a failed port.
*       When port is looped and get packet with DSA tag <Packet is Looped> = 0,
*       then device do next:
*        - set DSA tag <Packet is Looped> = 1
*        - bypass ingress and egress processing
*        - send packet through egress port that defined in secondary target
*          port map (cpssDxChCscdFastFailoverSecondaryTargetPortMapSet).
*       When port is looped and get packet with DSA tag <Packet is Looped> = 1,
*       then device do next:
*        - set DSA tag <Packet is Looped> = 0
*        - Apply usual ingress and egress processing
*       When port is not looped and get packet then device do next:
*        - Apply usual ingress and egress processing
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number (including CPU)
*       isLooped         -  GT_FALSE - Port is not looped.
*                           GT_TRUE - Port is looped.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverPortIsLoopedSet
(
  IN  GT_U8      devNum,
  IN  GT_U8      portNum,
  IN GT_BOOL     isLooped
)
{
    GT_U32      regAddr;    /* register address             */
    GT_U32      hwValue;    /* value to write to register   */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    /* getting register address */
    regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.loopPortReg;

    hwValue = BOOL2BIT_MAC(isLooped);

    if (localPort == CPSS_CPU_PORT_NUM_CNS)
    {
        localPort = 31;
    }

    /* set port is looped per port bit for fast stack failover */
    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, localPort, 1, hwValue);
}

/*******************************************************************************
* cpssDxChCscdFastFailoverPortIsLoopedGet
*
* DESCRIPTION:
*       Get status (Enable/Disable) of Fast Failover on the failed port.
*       When port is looped and get packet with DSA tag <Packet is Looped> = 0,
*       then device do next:
*        - set DSA tag <Packet is Looped> = 1
*        - bypass ingress and egress processing
*        - send packet through egress port that defined in secondary target
*          port map (cpssDxChCscdFastFailoverSecondaryTargetPortMapSet).
*       When port is looped and get packet with DSA tag <Packet is Looped> = 1,
*       then device do next:
*        - set DSA tag <Packet is Looped> = 0
*        - Apply usual ingress and egress processing
*       When port is not looped and get packet then device do next:
*        - Apply usual ingress and egress processing
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number (including CPU)
*
* OUTPUTS:
*       isLoopedPtr       - (pointer to) Is Looped
*                           GT_FALSE - Port is not looped.
*                           GT_TRUE - Port is looped.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverPortIsLoopedGet
(
  IN GT_U8      devNum,
  IN GT_U8      portNum,
  OUT GT_BOOL    *isLoopedPtr
)
{
    GT_U32      regAddr;    /* register address             */
    GT_U32      hwValue;    /* value to write to register   */
    GT_STATUS   rc;         /* return code                  */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(isLoopedPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    /* getting register address */
    regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.loopPortReg;

    if (localPort == CPSS_CPU_PORT_NUM_CNS)
    {
        localPort = 31;
    }

    /* get port is looped per port bit for fast stack failover */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, localPort, 1, &hwValue);

    *isLoopedPtr = BIT2BOOL_MAC(hwValue);

    return rc;
}

/****************************** Private Functions *****************************/

/*******************************************************************************
* cscdXsmiPortConfigurationSet
*
* DESCRIPTION:
*       Set XSMI registers for XG PHY ports
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - physical device number
*       portNum          - cascade HyperG port to be configured
*       xsmiActionPtr    - pointer to XSMI address and value array
*       xsmiActionSize   - XSMI address and value array size
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS cscdXsmiPortConfigurationSet
(
    IN  GT_U8               devNum,
    IN  GT_U8               portNum,
    IN  XSMI_STC            *xsmiActionPtr,
    IN  GT_U32              xsmiActionSize
)
{
    GT_U32      i;  /* iterator */
    GT_STATUS   rc; /* return code */

    for (i = 0; i < xsmiActionSize; i++)
    {
        rc = cpssDxChPhyPort10GSmiRegisterWrite(devNum,
                                              portNum,
                                              0, /* any value for internal PHY*/
                                              GT_FALSE,
                                              xsmiActionPtr[i].xsmiAddr,
                                              5, /* Phy Device number - 5 */
                                              xsmiActionPtr[i].xsmiData);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCscdQosPortTcRemapEnableSet
*
* DESCRIPTION:
*       Enable/Disable Traffic Class Remapping on cascading port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - cascading port number (CPU port or 0-27)
*       enable           - GT_TRUE: enable Traffic Class remapping
*                          GT_FALSE: disable Traffic Class remapping
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdQosPortTcRemapEnableSet
(
    IN GT_U8        devNum,
    IN GT_U8        portNum,
    IN GT_BOOL      enable
)
{
    GT_U32      regAddr;    /* register address             */
    GT_U32      hwValue;    /* value to write to register   */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    hwValue = BOOL2BIT_MAC(enable);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        /* getting register address */
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.stackRemapPortEnReg;

        if (localPort == CPSS_CPU_PORT_NUM_CNS)
        {
            localPort = 31;
        }

        /* enable/disable stack tc remap per port */
        return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, localPort, 1, hwValue);
    }
    else
    {
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
            distributor.stackRemapEn[OFFSET_TO_WORD_MAC(portNum)];

        return prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId,regAddr,
            OFFSET_TO_BIT_MAC(portNum), 1, hwValue);
    }

}

/*******************************************************************************
* cpssDxChCscdQosPortTcRemapEnableGet
*
* DESCRIPTION:
*       Get the status of Traffic Class Remapping on cascading port
*       (Enabled/Disabled).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - cascading port number (CPU port or 0-27)
*
* OUTPUTS:
*       enablePtr        - GT_TRUE: Traffic Class remapping enabled
*                          GT_FALSE: Traffic Class remapping disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdQosPortTcRemapEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32      regAddr;    /* register address             */
    GT_U32      hwValue;    /* value to write to register   */
    GT_STATUS   rc;         /* return code                  */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        /* getting register address */
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.stackRemapPortEnReg;

        if (localPort == CPSS_CPU_PORT_NUM_CNS)
        {
            localPort = 31;
        }

        /* get enable/disable stack tc remap per port */
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, localPort, 1, &hwValue);
    }
    else
    {
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
            distributor.stackRemapEn[OFFSET_TO_WORD_MAC(portNum)];

        rc = prvCpssDxChHwPpPortGroupGetRegField(devNum, portGroupId,regAddr,
            OFFSET_TO_BIT_MAC(portNum), 1, &hwValue);
    }

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;
}

/*******************************************************************************
* cpssDxChCscdQosTcRemapTableSet
*
* DESCRIPTION:
*       Remap Traffic Class on cascading port to new Traffic Classes,
*       for each DSA tag type and for source port type (local or cascading).
*       If the source port is enabled for Traffic Class remapping, then traffic
*       will egress with remapped Traffic Class.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       tc              - Traffic Class of the packet on the source port (0..7)
*       tcMappingsPtr   - (pointer to )remapped traffic Classes
*                         for ingress Traffic Class
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdQosTcRemapTableSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  tc,
    IN  CPSS_DXCH_CSCD_QOS_TC_REMAP_STC    *tcMappingsPtr
)
{
    GT_U32      regAddr;    /* register address             */
    GT_U32      data;       /* data to write to register   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tc);
    CPSS_NULL_PTR_CHECK_MAC(tcMappingsPtr);

    /* check validity of Traffic Classess */
    if ((tcMappingsPtr->forwardLocalTc >= BIT_3) ||
        (tcMappingsPtr->forwardStackTc >= BIT_3) ||
        (tcMappingsPtr->toAnalyzerLocalTc >= BIT_3) ||
        (tcMappingsPtr->toAnalyzerStackTc >= BIT_3) ||
        (tcMappingsPtr->toCpuLocalTc >= BIT_3) ||
        (tcMappingsPtr->toCpuStackTc >= BIT_3) ||
        (tcMappingsPtr->fromCpuLocalTc >= BIT_3) ||
        (tcMappingsPtr->fromCpuStackTc >= BIT_3))
    {
        return GT_OUT_OF_RANGE;
    }

    /* getting register address */
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.
                                                stackTcPerProfileRemapTbl[tc];
    }
    else
    {
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
            distributor.stackTcRemapping[tc];
    }

    /* build data to write to register */
    data = (tcMappingsPtr->forwardLocalTc |
           (tcMappingsPtr->toAnalyzerLocalTc << 3) |
           (tcMappingsPtr->toCpuLocalTc << 6) |
           (tcMappingsPtr->fromCpuLocalTc << 9) |
           (tcMappingsPtr->forwardStackTc << 12) |
           (tcMappingsPtr->toAnalyzerStackTc << 15) |
           (tcMappingsPtr->toCpuStackTc << 18) |
           (tcMappingsPtr->fromCpuStackTc << 21));


    /* write data to Stack TC per Profile remap table */
    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 24, data);
}

/*******************************************************************************
* cpssDxChCscdQosTcRemapTableGet
*
* DESCRIPTION:
*       Get the remapped Traffic Classes for given Traffic Class.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       tc              - Traffic Class of the packet on the source port (0..7)
*
* OUTPUTS:
*       tcMappingsPtr   - (pointer to )remapped Traffic Classes
*                         for ingress Traffic Class
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdQosTcRemapTableGet
(
    IN   GT_U8                                  devNum,
    IN   GT_U32                                 tc,
    OUT  CPSS_DXCH_CSCD_QOS_TC_REMAP_STC   *tcMappingsPtr
)
{
    GT_U32      regAddr;    /* register address             */
    GT_U32      data;       /* data to write to register    */
    GT_STATUS   rc;         /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tc);
    CPSS_NULL_PTR_CHECK_MAC(tcMappingsPtr);

    /* getting register address */
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.
                                                stackTcPerProfileRemapTbl[tc];
    }
    else
    {
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
            distributor.stackTcRemapping[tc];
    }

    /* read data from Stack TC per Profile remap table */
    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 24, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set Traffic Classes according to register data */
    tcMappingsPtr->forwardLocalTc = (data & 0x7);
    tcMappingsPtr->toAnalyzerLocalTc = ((data >> 3) & 0x7);
    tcMappingsPtr->toCpuLocalTc = ((data >> 6) & 0x7);
    tcMappingsPtr->fromCpuLocalTc = ((data >> 9) & 0x7);
    tcMappingsPtr->forwardStackTc = ((data >> 12) & 0x7);
    tcMappingsPtr->toAnalyzerStackTc = ((data >> 15) & 0x7);
    tcMappingsPtr->toCpuStackTc = ((data >> 18) & 0x7);
    tcMappingsPtr->fromCpuStackTc = ((data >> 21) & 0x7);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCscdPortBridgeBypassEnableSet
*
* DESCRIPTION:
*       The function enables/disables bypass of the bridge engine per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  -  PP's device number.
*       portNum -  physical port number or CPU port
*       enable  -  GT_TRUE  - Enable bypass of the bridge engine.
*                  GT_FALSE - Disable bypass of the bridge engine.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If the DSA tag is not extended Forward, the bridging decision
*       is performed regardless of the setting.
*       When bypass enabled the Bridge engine still learn MAC source addresses,
*       but will not modify the packet command, attributes (or forwarding 
*       decision).
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortBridgeBypassEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_BOOL     enable
)
{
    GT_U32  data;  /* data to write to HW */
    GT_U32  fieldOffset;    /* field offset */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    /* Convert boolean value to the HW format */
    data = BOOL2BIT_MAC(enable);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        fieldOffset = 8;
    }
    else
    {
        fieldOffset = 21;
    }

    /* Update port mode in the VLAN and QoS Configuration Entry */
    return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                           (GT_U32)portNum,
                                           0,       /* start at word 0 */
                                           fieldOffset, /* field offset */
                                           1,       /* 1 bit */
                                           data);

}

/*******************************************************************************
* cpssDxChCscdPortBridgeBypassEnableGet
*
* DESCRIPTION:
*       The function gets bypass of the bridge engine per port
*       configuration status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - PP's device number.
*       portNum - physical port number or CPU port
*
* OUTPUTS:
*       enablePtr  -  pointer to bypass of the bridge engine per port
*                     configuration status.
*                     GT_TRUE  - Enable bypass of the bridge engine.
*                     GT_FALSE - Disable bypass of the bridge engine.
*
*
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortBridgeBypassEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32  data;           /* data to read from HW */
    GT_STATUS   rc;         /* return code      */
    GT_U32      fieldOffset;    /* field offset */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        fieldOffset = 8;
    }
    else
    {
        fieldOffset = 21;
    }

    /* Read port mode in the VLAN and QoS Configuration Entry */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                           (GT_U32)portNum,
                                           0,       /* start at word 0 */
                                           fieldOffset, /* field offset */
                                           1,       /* 1 bit */
                                           &data);

    if(rc != GT_OK)
    {
        return rc;
    }

    /* Convert HW format to the boolean value */
    *enablePtr = BIT2BOOL_MAC(data);

    return rc;

}

/*******************************************************************************
* cpssDxChCscdOrigSrcPortFilterEnableSet
*
* DESCRIPTION:
*       Enable/Disable filtering the multi-destination packet that was received
*       by the local device, sent to another device, and sent back to this
*       device, from being sent back to the network port at which it was
*       initially received.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       enable   - GT_TRUE - filter and drop the packet
*                - GT_FALSE - don't filter the packet.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdOrigSrcPortFilterEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32  regAddr;      /* register address */
    GT_U32  data;         /* reg subfield data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    data = BOOL2BIT_MAC(enable);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;
        return prvCpssDrvHwPpSetRegField(devNum, regAddr, 5, 1, data);
    }
    else
    {
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.filterConfig.globalEnables;
        return prvCpssDxChHwPpSetRegField(devNum, regAddr, 3, 1, data);
    }
}


/*******************************************************************************
* cpssDxChCscdOrigSrcPortFilterEnableGet
*
* DESCRIPTION:
*       Get the status of filtering the multi-destination packet that was
*       received  by the local device, sent to another device, and sent back to
*       this device, from being sent back to the network port at which it was
*       initially received.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - filter and drop the packet
*                   - GT_FALSE - don't filter the packet.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdOrigSrcPortFilterEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32      regAddr;    /* register address     */
    GT_U32      data;       /* reg subfield data    */
    GT_STATUS   rc;         /* return code          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;

        rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 5, 1, &data);
    }
    else
    {
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.filterConfig.globalEnables;
        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 3, 1, &data);
    }

    *enablePtr = BIT2BOOL_MAC(data);

    return rc;
}

/*******************************************************************************
* cpssDxChCscdDevMapLookupModeSet
*
* DESCRIPTION:
*        Set lookup mode for accessing the Device Map table. 
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number
*       mode         - device Map lookup mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong device or mode 
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdDevMapLookupModeSet
(
    IN GT_U8                               devNum,
    IN CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT   mode
)
{
    GT_U32      regAddr;    /* register address     */
    GT_U32      data;       /* reg subfield data    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    switch(mode)
    {
        case CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_E:
            data = 0;
            break;
        case CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E:
            data = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.egr.devMapTableConfig.addrConstruct;

    /* Set lookup mode for accessing the Device Map table. */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 4, data);

}

/*******************************************************************************
* cpssDxChCscdDevMapLookupModeGet
*
* DESCRIPTION:
*       Get lookup mode for accessing the Device Map table. 
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       modePtr      - pointer to device Map lookup mode
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong device 
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_BAD_STATE     - wrong hardware value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdDevMapLookupModeGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT   *modePtr
)
{
    GT_U32      regAddr;    /* register address     */
    GT_U32      data;       /* reg subfield data    */
    GT_STATUS   rc;         /* return code          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.egr.devMapTableConfig.addrConstruct;

    /* Get lookup mode for accessing the Device Map table. */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 4, &data);
    if(rc != GT_OK)
    {
        return rc;
    }

    switch(data)
    {
        case 0:
            *modePtr = CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_E;
            break;
      
        case 1:
            *modePtr = CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E;
            break;

        default: 
            return GT_BAD_STATE;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCscdPortLocalDevMapLookupEnableSet 
*
* DESCRIPTION:
*       Enable / Disable the local target port for device map lookup 
*       for local device.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number or CPU port
*       portDirection   - port's direction:
*                         CPSS_DIRECTION_INGRESS_E - source port 
*                         CPSS_DIRECTION_EGRESS_E  -  target port
*                         CPSS_DIRECTION_BOTH_E    - both source and target ports
*       enable       - GT_TRUE  - the port is enabled for device map lookup
*                                 for local device. 
*                    - GT_FALSE - No access to Device map table for
*                                 local devices.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong device or port number or portDirection
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To enable access to the Device Map Table for the local target devices
*       -  Enable the local source port for device map lookup     
*       -  Enable the local target port for device map lookup
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortLocalDevMapLookupEnableSet
(
    IN GT_U8                devNum,
    IN GT_U8                portNum,
    IN CPSS_DIRECTION_ENT   portDirection,   
    IN GT_BOOL              enable
)
{
    GT_U32  regAddr;            /* register address     */
    GT_U32  regAddrTrg = 0;     /* register address     */
    GT_U32  data;               /* reg subfield data    */
    GT_U32  portGroupId;        /* the port group Id - support multi-port-groups device */
    GT_U32  fieldOffset;        /* register field offset */ 
    GT_STATUS   rc;             /* return status                */
    
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    switch(portDirection)
    {
        case CPSS_DIRECTION_BOTH_E:
        case CPSS_DIRECTION_INGRESS_E:
            if(portDirection == CPSS_DIRECTION_BOTH_E)
            {
                regAddrTrg = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                             txqVer1.egr.devMapTableConfig.
                           localTrgPortMapOwnDevEn[OFFSET_TO_WORD_MAC(portNum)];
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.egr.devMapTableConfig.
                               localSrcPortMapOwnDevEn[OFFSET_TO_WORD_MAC(portNum)];
            break;
        case CPSS_DIRECTION_EGRESS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.egr.devMapTableConfig.
                               localTrgPortMapOwnDevEn[OFFSET_TO_WORD_MAC(portNum)];
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    fieldOffset = OFFSET_TO_BIT_MAC(portNum);

    data = (enable == GT_TRUE) ? 1 : 0;

    /* Enable / Disable the local target / source port for device map lookup 
       for local device. */
    rc =  prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                               fieldOffset, 1, data);
    if(rc != GT_OK)
    {
        return rc;
    }
    if(portDirection == CPSS_DIRECTION_BOTH_E)
    {
        return prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId,
                                                   regAddrTrg,
                                                   fieldOffset, 1, data);
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChCscdPortLocalDevMapLookupEnableGet
*
* DESCRIPTION:
*       Get status of enabling / disabling the local target port 
*       for device map lookup for local device.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number or CPU port
*       portDirection   - port's direction:
*                         CPSS_DIRECTION_INGRESS_E - source port 
*                         CPSS_DIRECTION_EGRESS_E  -  target port
*                         CPSS_DIRECTION_BOTH_E    - both source and target ports
*
* OUTPUTS:
*       enablePtr    - pointer to status of enabling / disabling the local 
*                      target port for device map lookup for local device.
*                    - GT_TRUE  - target port is enabled for device map lookup
*                                 for local device. 
*                    - GT_FALSE - No access to Device map table for
*                                 local devices.
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong device or port number or portDirection
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortLocalDevMapLookupEnableGet
(
    IN  GT_U8               devNum,
    IN  GT_U8               portNum,
    IN CPSS_DIRECTION_ENT   portDirection,   
    OUT GT_BOOL             *enablePtr
)
{
    GT_U32  regAddr;       /* register address     */
    GT_U32  data;          /* reg subfield data    */
    GT_U32  portGroupId;   /* the port group Id - support multi-port-groups device */
    GT_U32  fieldOffset;   /* register field offset */ 
    GT_STATUS   rc;        /* return status                */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    switch(portDirection)
    {
        case CPSS_DIRECTION_INGRESS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.egr.devMapTableConfig.
                               localSrcPortMapOwnDevEn[OFFSET_TO_WORD_MAC(portNum)];
            break;
        case CPSS_DIRECTION_EGRESS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.egr.devMapTableConfig.
                               localTrgPortMapOwnDevEn[OFFSET_TO_WORD_MAC(portNum)];
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    fieldOffset = OFFSET_TO_BIT_MAC(portNum);

    /* Get status of enabling / disabling the local target port
       for device map lookup for local device. */                 
    rc = prvCpssDxChHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                             fieldOffset, 1, &data);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(data);

    return GT_OK;
}




