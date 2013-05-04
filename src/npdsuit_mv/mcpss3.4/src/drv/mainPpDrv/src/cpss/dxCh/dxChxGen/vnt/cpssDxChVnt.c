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
* cpssDxChVnt.c
*
* DESCRIPTION:
*     CPSS DXCH Virtual Network Tester (VNT) Technology facility implementation.
*     VNT features:
*     - Operation, Administration, and Maintenance (OAM).
*     - Connectivity Fault Management (CFM).
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>
#include <cpss/dxCh/dxChxGen/vnt/cpssDxChVnt.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>


/*******************************************************************************
* cpssDxChVntOamPortLoopBackModeEnableSet
*
* DESCRIPTION:
*      Enable/Disable 802.3ah Loopback mode on the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      Loopback Mode is enabled for the port,
*      - All traffic destined for  this port but was not ingressed on this port
*        is discarded, except for FROM_CPU traffic - which is to allow the CPU
*        to send OAM control packets.
*      - Egress VLAN and spanning tree filtering is bypassed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number
*       port     - physical port number including CPU port.
*       enable   - GT_TRUE:  Enable OAM loopback mode.
*                  GT_FALSE: Disable OAM loopback mode.
*
*
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*      In the Policy TCAM, after the existing rule which traps OAMPDUs,
*      add a rule to redirect all traffic received on
*      the port back to the same port for getting loopback.
*      For xCat and above not need rule for OAM PDU trap.
*      Use cpssDxChVntOamPortPduTrapEnableSet for it.
*
*******************************************************************************/
GT_STATUS cpssDxChVntOamPortLoopBackModeEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_BOOL  enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    fieldOffset;       /* The start bit number in the register */
    GT_U32    data;              /* reg sub field data */
    GT_STATUS   rc;              /* return status                */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,port);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        if(CPSS_CPU_PORT_NUM_CNS == port)
        {
            fieldOffset = 31;
        }
        else
        {
            fieldOffset = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,port);
        }
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                           bridgeRegs.vntReg.vntOamLoopbackConfReg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.egr.filterConfig.oamLoopbackFilterEn[OFFSET_TO_WORD_MAC(port)];

        fieldOffset = OFFSET_TO_BIT_MAC(port);
    }

    data = (enable == GT_TRUE) ? 1 : 0;

    /* For xCat and above devices enable/disable OAM Link Layer Loopback */
    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* write to port-vlan-qos table  */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                             port,/* port */
                                             1,/* start at word 1 */
                                             30,/* start at bit 0*/
                                             1, /* 1 bit */
                                             data);
        if(rc !=GT_OK)
        {
            return rc;
        }
    }

    /* Enable/Disable Loopback mode */
    return prvCpssDxChHwPpPortGroupSetRegField(devNum,PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
            regAddr, fieldOffset, 1, data);

}

/*******************************************************************************
* cpssDxChVntOamPortLoopBackModeEnableGet
*
* DESCRIPTION:
*      Get 802.3ah Loopback mode on the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      Loopback Mode is enabled for the port,
*      - All traffic destined for  this port but was not ingressed on this port
*        is discarded, except for FROM_CPU traffic - which is to allow the CPU
*        to send OAM control packets.
*      - Egress VLAN and spanning tree filtering is bypassed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       port        - physical port number including CPU port.
*
* OUTPUTS:
*       enablePtr   - pointer to Loopback mode status:
*                     GT_TRUE:  Enable OAM loopback mode.
*                     GT_FALSE: Disable OAM loopback mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*      In the Policy TCAM, after the existing rule which traps OAMPDUs,
*      add a rule to redirect all traffic received on
*      the port back to the same port for getting loopback.
*      For xCat and above not need rule for OAM PDU trap.
*      Use cpssDxChVntOamPortPduTrapEnableSet for it.
*
*******************************************************************************/
GT_STATUS cpssDxChVntOamPortLoopBackModeEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    port,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32      regAddr;      /* register address             */
    GT_U32      fieldOffset;  /* The start bit number in the register */
    GT_STATUS   rc;           /* return status                */
    GT_U32      hwValue;      /* value to read from register  */
    GT_U32      hwData;       /* the data read from the table */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        if(CPSS_CPU_PORT_NUM_CNS == port)
        {
            fieldOffset = 31;
        }
        else
        {
            fieldOffset = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,port);
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                         bridgeRegs.vntReg.vntOamLoopbackConfReg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.egr.filterConfig.oamLoopbackFilterEn[OFFSET_TO_WORD_MAC(port)];

        fieldOffset = OFFSET_TO_BIT_MAC(port);
    }

    rc = prvCpssDxChHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
            regAddr, fieldOffset, 1, &hwValue);

    if(rc != GT_OK)
    {
        return rc;
    }

    /* For xCat and above devices get OAM Link Layer Loopback mode */
    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* read from port-vlan-qos table  */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                             port,/* port */
                                             1,/* start at word 1 */
                                             30,/* start at bit 0*/
                                             1, /* 1 bit */
                                             &hwData);
        if(rc !=GT_OK)
        {
            return rc;
        }

        /* To enable OAM Loopback mode for xCat and above devices,
           OAM Link Layer Loopback and OAM Loopback should be enabled */

        hwValue &= hwData;
    }

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;

}

/*******************************************************************************
* cpssDxChVntOamPortUnidirectionalEnableSet
*
* DESCRIPTION:
*      Enable/Disable the port for unidirectional transmit.
*      If unidirectional transmit enabled, sending
*      OAM (Operation, Administration, and Maintenance) control packets
*      and data traffic over failed links (ports with link down) is allowed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number
*       port     - physical port number including CPU port.
*       enable   - GT_TRUE:   Enable the port for unidirectional transmit.
*                  GT_FALSE:  Disable the port for unidirectional transmit.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntOamPortUnidirectionalEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_BOOL  enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */
    GT_U32     portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_STATUS rc;
    GT_U32 offset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,port);

    data = (enable == GT_TRUE) ? 1 : 0;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);        

    /* Enable/Disable the port for unidirectional transmit */
	
#ifdef __AX_PLATFORM__
			for(portMacType = PRV_CPSS_PORT_GE_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
			{
#else
			portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,port);
#endif
				if((port == CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
#ifdef __AX_PLATFORM__
					continue;
#else
					return GT_OK;
#endif

    
        /* 10 Gb port setting */
        if(portMacType >= PRV_CPSS_PORT_XG_E)
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,port,portMacType,&regAddr);
            offset=8;
        }
    
        else
        {
            PRV_CPSS_DXCH_PORT_SERIAL_PARAM_CTRL_REG_MAC(devNum,port,&regAddr);
            offset=0;
        }

        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            rc =  prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, offset, 1, data);
    
            if (rc != GT_OK) 
            {
                return rc;
            }
        }
#ifdef __AX_PLATFORM__
    }
#endif
    return GT_OK;
}

/*******************************************************************************
* cpssDxChVntOamPortUnidirectionalEnableGet
*
* DESCRIPTION:
*      Gets the current status of unidirectional transmit for the port.
*      If unidirectional transmit enabled, sending
*      OAM (Operation, Administration, and Maintenance) control packets
*      and data traffic over failed links (ports with link down) is allowed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       port        - physical port number including CPU port.
*
* OUTPUTS:
*       enablePtr   - pointer to current status of unidirectional transmit:
*                     GT_TRUE:   Enable the port for unidirectional transmit.
*                     GT_FALSE:  Disable the port for unidirectional transmit.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntOamPortUnidirectionalEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    port,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32      regAddr;           /* register address */
    GT_U32      hwValue;           /* value to read from register  */
    GT_STATUS   rc;                /* return status                */
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32        offset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,port);
      
    /* Get current status of unidirectional transmit */

    /* 10 Gb ports */
    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,port,portMacType,&regAddr);
        offset=8;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_SERIAL_PARAM_CTRL_REG_MAC(devNum,port,&regAddr);
        offset=0;
    }

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
                                  regAddr,
                                  offset,
                                  1,
                                  &hwValue);

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;
}

/*******************************************************************************
* cpssDxChVntCfmEtherTypeSet
*
* DESCRIPTION:
*       Sets the EtherType to identify CFM (Connectivity Fault Management) PDUs.
*       If the packet EtherType matches the CFM EtherType, the ingress Policy
*       key implicitly uses the three User-Defined-Byte (UDB0, UDB1, UDB2) to
*       contain the three CFM data fields, MD Level, Opcode, and Flags,
*       respectively.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       etherType   - CFM (Connectivity Fault Management) EtherType,
*                     values 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum.
*       GT_OUT_OF_RANGE          - on wrong etherType values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntCfmEtherTypeSet
(
    IN GT_U8    devNum,
    IN GT_U32   etherType
)
{
    GT_U32    regAddr;           /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    if(etherType > 0xFFFF)
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                       bridgeRegs.vntReg.vntCfmEtherTypeConfReg;

    /* Set CFM EtherType */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 16, etherType);

}

/*******************************************************************************
* cpssDxChVntCfmEtherTypeGet
*
* DESCRIPTION:
*       Gets the current EtherType to identify CFM
*       (Connectivity Fault Management) PDUs.
*       If the packet EtherType matches the CFM EtherType, the ingress Policy
*       key implicitly uses the three User-Defined-Byte (UDB0, UDB1, UDB2) to
*       contain the three CFM data fields, MD Level, Opcode, and Flags,
*       respectively.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       etherTypePtr - pointer to CFM (Connectivity Fault Management) EtherType.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntCfmEtherTypeGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *etherTypePtr
)
{
    GT_U32    regAddr;           /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(etherTypePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                       bridgeRegs.vntReg.vntCfmEtherTypeConfReg;

    /* Get CFM EtherType */
    return prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 16, etherTypePtr);

}

/*******************************************************************************
* cpssDxChVntCfmLbrOpcodeSet
*
* DESCRIPTION:
*      Sets the CFM (Connectivity Fault Management)
*      LBR (Loopback Response) opcode.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       opcode      - CFM LBR Opcode, values 0..0xFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, opcode.
*       GT_OUT_OF_RANGE          - on wrong opcode values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntCfmLbrOpcodeSet
(
    IN GT_U8    devNum,
    IN GT_U32   opcode
)
{
    GT_U32    regAddr;           /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    if(opcode > 0xFF)
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;

    /* Set LBR Opcode */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 2, 8, opcode);

}

/*******************************************************************************
* cpssDxChVntCfmLbrOpcodeGet
*
* DESCRIPTION:
*      Gets the current CFM (Connectivity Fault Management)
*      LBR (Loopback Response) opcode.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       opcodePtr   - pointer to CFM LBR Opcode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntCfmLbrOpcodeGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *opcodePtr
)
{
    GT_U32    regAddr;           /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(opcodePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;

    /* Get LBR Opcode */
    return prvCpssDrvHwPpGetRegField(devNum, regAddr, 2, 8, opcodePtr);

}

/*******************************************************************************
* cpssDxChVntLastReadTimeStampGet
*
* DESCRIPTION:
*      Gets the last-read-time-stamp counter value that represent the exact
*      timestamp of the last read operation to the PP registers.
*
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       timeStampValuePtr - pointer to timestamp of the last read operation.
*                           200  MHz:   granularity - 5 nSec
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntLastReadTimeStampGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *timeStampValuePtr
)
{
    GT_U32    regAddr;           /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(timeStampValuePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.lastReadTimeStampReg;

    /* Gets the last-read-time-stamp counter value */
    return prvCpssDrvHwPpReadRegister(devNum, regAddr, timeStampValuePtr);

}

/*******************************************************************************
* cpssDxChVntOamPortPduTrapEnableSet
*
* DESCRIPTION:
*      Enable/Disable trap to CPU of 802.3ah Link Layer Control protocol on
*      the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      PDU trap is enabled for the port:
*      - Packets identified as OAM-PDUs destined for this port are trapped to the CPU.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*       port     - physical port number including CPU port.
*       enable   - GT_TRUE:  Enable trap to CPU of 802.3ah Link Layer Control protocol.
*                  GT_FALSE: Disable trap to CPU of 802.3ah Link Layer Control protocol.
*
*
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVntOamPortPduTrapEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_BOOL  enable
)
{
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,port);

    data = (enable == GT_TRUE) ? 1 : 0;

    /* write to port-vlan-qos table  */
    return prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,/* port */
                                         1,/* start at word 1 */
                                         31,/* start at bit 0*/
                                         1, /* 1 bit */
                                         data);

}

/*******************************************************************************
* cpssDxChVntOamPortPduTrapEnableGet
*
* DESCRIPTION:
*      Get trap to CPU status(Enable/Disable) of 802.3ah Link Layer Control protocol on
*      the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      PDU trap is enabled for the port:
*      - Packets identified as OAM-PDUs are trapped to the CPU.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*       port     - physical port number including CPU port.
*
*
*
* OUTPUTS:
*       enablePtr   - pointer to Loopback mode status:
*                     GT_TRUE:  Enable trap to CPU of 802.3ah Link Layer Control protocol.
*                     GT_FALSE: Disable trap to CPU of 802.3ah Link Layer Control protocol..
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on bad pointer of enablePtr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVntOamPortPduTrapEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    port,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS   rc;           /* return status                */
    GT_U32      hwValue;      /* value to read from port-vlan-qos table  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* read from port-vlan-qos table  */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,/* port */
                                         1,/* start at word 1 */
                                         31,/* start at bit 0*/
                                         1, /* 1 bit */
                                         &hwValue);

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;

}





