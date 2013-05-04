/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPtp.c
*
* DESCRIPTION:
*       CPSS DxCh Precision Time Protocol function implementations.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

/* get the device info and common info */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

#include <cpss/dxCh/dxChxGen/ptp/cpssDxChPtp.h>


/* number of Policer Counter sets */
#define PRV_CPSS_DXCH_POLICER_MAX_COUNTER_INDEX_CNS     16



/*******************************************************************************
* cpssDxChPtpEtherTypeSet
*
* DESCRIPTION:
*       Configure ethertype0/ethertype1 of PTP over Ethernet packets.
*       A packet is identified as PTP over Ethernet if its EtherType matches
*       one of the configured values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       etherTypeIndex  - PTP EtherType index, values 0..1
*       etherType       - PTP EtherType0 or EtherType1 according to the index,
*                         values 0..0xFFFF
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or etherTypeIndex
*       GT_OUT_OF_RANGE          - on wrong etherType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEtherTypeSet
(
    IN GT_U8    devNum,
    IN GT_U32   etherTypeIndex,
    IN GT_U32   etherType
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* field offset inside register     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    if (etherTypeIndex > 1)
    {
        return GT_BAD_PARAM;
    }

    if (etherType > 0xFFFF)
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpEthertypes;

    fieldOffset = (etherTypeIndex * 16);

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, 16,
                                     etherType);
}


/*******************************************************************************
* cpssDxChPtpEtherTypeGet
*
* DESCRIPTION:
*       Get ethertypes of PTP over Ethernet packets.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       etherTypeIndex  - PTP EtherType index, values 0..1
*
* OUTPUTS:
*       etherTypePtr    - (pointer to) PTP EtherType0 or EtherType1 according
*                          to the index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or etherTypeIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEtherTypeGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   etherTypeIndex,
    OUT GT_U32   *etherTypePtr
)
{
    GT_U32      regAddr;        /* register address                 */
    GT_U32      fieldOffset;    /* field offset inside register     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(etherTypePtr);

    if (etherTypeIndex > 1)
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpEthertypes;

    fieldOffset = (etherTypeIndex * 16);

    return prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, 16,
                                     etherTypePtr);
}

/*******************************************************************************
* cpssDxChPtpUdpDestPortsSet
*
* DESCRIPTION:
*       Configure UDP destination port0/port1 of PTP over UDP packets.
*       A packet is identified as PTP over UDP if it is a UDP packet, whose
*       destination port matches one of the configured ports.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       udpPortIndex    - UDP port index , values 0..1
*       udpPortNum      - UDP port1/port0 number according to the index,
*                         values 0..0xFFFF
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or udpPortIndex
*       GT_OUT_OF_RANGE          - on wrong udpPortNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpUdpDestPortsSet
(
    IN GT_U8    devNum,
    IN GT_U32   udpPortIndex,
    IN GT_U32   udpPortNum
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* field offset inside register     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    if (udpPortIndex > 1)
    {
        return GT_BAD_PARAM;
    }

    if (udpPortNum > 0xFFFF)
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpUdpDstPorts;

    fieldOffset = (udpPortIndex * 16);

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, 16,
                                     udpPortNum);
}

/*******************************************************************************
* cpssDxChPtpUdpDestPortsGet
*
* DESCRIPTION:
*       Get UDP destination port0/port1 of PTP over UDP packets.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       udpPortIndex    - UDP port index , values 0..1
*
* OUTPUTS:
*       udpPortNumPtr     - (pointer to) UDP port0/port1 number, according
*                           to the index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or udpPortIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpUdpDestPortsGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   udpPortIndex,
    OUT GT_U32   *udpPortNumPtr
)
{
    GT_U32      regAddr;        /* register address                 */
    GT_U32      fieldOffset;    /* field offset inside register     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(udpPortNumPtr);

    if (udpPortIndex > 1)
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpUdpDstPorts;

    fieldOffset = (udpPortIndex * 16);

    return prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, 16,
                                     udpPortNumPtr);
}


/*******************************************************************************
* cpssDxChPtpMessageTypeCmdSet
*
* DESCRIPTION:
*       Configure packet command per PTP message type.
*       The message type is extracted from the PTP header.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       messageType     - message type, values 0..15
*       command         - assigned command to the packet.
*                         Acceptable values: FORWARD, MIRROR, TRAP, HARD_DROP and
*                                            SOFT_DROP.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpMessageTypeCmdSet
(
    IN GT_U8                    devNum,
    IN GT_U32                   messageType,
    IN CPSS_PACKET_CMD_ENT      command
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* field offset inside register     */
    GT_U32    hwValue;          /* value to write to hw             */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    if (messageType > 15)
    {
        return GT_BAD_PARAM;
    }

    switch (command)
    {
        case CPSS_PACKET_CMD_FORWARD_E:
            hwValue = 0;
            break;
        case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
            hwValue = 1;
            break;
        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            hwValue = 2;
            break;
        case CPSS_PACKET_CMD_DROP_HARD_E:
            hwValue = 3;
            break;
        case CPSS_PACKET_CMD_DROP_SOFT_E:
            hwValue = 4;
            break;
        default:
            return GT_BAD_PARAM;
    }

    fieldOffset = (messageType % 10) * 3;

    if (messageType / 10)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        ttiRegs.ptpPacketCmdCfg1Reg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        ttiRegs.ptpPacketCmdCfg0Reg;
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, 3, hwValue);
}

/*******************************************************************************
* cpssDxChPtpMessageTypeCmdGet
*
* DESCRIPTION:
*       Get packet command per PTP message type.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       messageType     - message type, values 0..15.
*
* OUTPUTS:
*       commandPtr      - (pointer to) assigned command to the packet.
*                         Acceptable values: FORWARD, MIRROR, TRAP, HARD_DROP and
*                                            SOFT_DROP.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or messageType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on bad value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpMessageTypeCmdGet
(
    IN  GT_U8                    devNum,
    IN  GT_U32                   messageType,
    OUT CPSS_PACKET_CMD_ENT      *commandPtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* field offset inside register     */
    GT_U32    hwValue;          /* value to read from hw            */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(commandPtr);

    if (messageType > 15)
    {
        return GT_BAD_PARAM;
    }

    fieldOffset = (messageType % 10) * 3;

    if (messageType / 10)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        ttiRegs.ptpPacketCmdCfg1Reg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        ttiRegs.ptpPacketCmdCfg0Reg;
    }

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, 3, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (hwValue)
    {
        case 0:
            *commandPtr = CPSS_PACKET_CMD_FORWARD_E;
            break;
        case 1:
            *commandPtr = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
            break;
        case 2:
            *commandPtr = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            break;
        case 3:
            *commandPtr = CPSS_PACKET_CMD_DROP_HARD_E;
            break;
        case 4:
            *commandPtr = CPSS_PACKET_CMD_DROP_SOFT_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpCpuCodeBaseSet
*
* DESCRIPTION:
*       Set CPU code base assigned to trapped and mirrored PTP packets.
*       A PTP packet's CPU code is computed as base CPU code + messageType
*       (where message type is taken from PTP header).
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       cpuCode         - The base of CPU code assigned to PTP packets mirrored
*                         or trapped to CPU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or cpuCode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpCpuCodeBaseSet
(
    IN GT_U8                        devNum,
    IN CPSS_NET_RX_CPU_CODE_ENT     cpuCode
)
{
    GT_U32                                  regAddr;    /* register address */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT  dsaCpuCode; /* DSA code */
    GT_STATUS                               rc;         /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    rc = prvCpssDxChNetIfCpuToDsaCode(cpuCode,&dsaCpuCode);
    if( rc != GT_OK )
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpPacketCmdCfg1Reg;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 18, 8, (GT_U32)dsaCpuCode);
}

/*******************************************************************************
* cpssDxChPtpCpuCodeBaseGet
*
* DESCRIPTION:
*       Get CPU code base assigned to trapped and mirrored PTP packets.
*       A PTP packet's CPU code is computed as base CPU code + messageType
*       (where message type is taken from PTP header).
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       cpuCodePtr      - (pointer to) The base of CPU code assigned to PTP
*                           packets mirrored or trapped to CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpCpuCodeBaseGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_NET_RX_CPU_CODE_ENT     *cpuCodePtr
)
{

    GT_U32                                  regAddr;    /* register address */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT  dsaCpuCode; /* DSA code */
    GT_U32                                  tempCode;   /* dsa code */
    GT_STATUS                               rc;         /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(cpuCodePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpPacketCmdCfg1Reg;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 18, 8, &tempCode);
    if (rc != GT_OK)
    {
        return rc;
    }

    dsaCpuCode = (PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT)tempCode;

    return prvCpssDxChNetIfDsaToCpuCode(dsaCpuCode,cpuCodePtr);
}

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionSet
*
* DESCRIPTION:
*       The function configures type of TOD counter action that will be
*       performed once triggered by cpssDxChPtpTodCounterFunctionTriggerSet()
*       API or by pulse from an external interface.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       function        - One of the four possible TOD counter functions
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*                                  or function
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterFunctionSet
(
    IN GT_U8                                    devNum,
    IN CPSS_PORT_DIRECTION_ENT                  direction,
    IN CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT       function
)
{
    return cpssDxChPtpPortGroupTodCounterFunctionSet(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            direction,
                                            function);
}

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionGet
*
* DESCRIPTION:
*       Get type of TOD counter action that will be performed once triggered by
*       cpssDxChPtpTodCounterFunctionTriggerSet() API or by pulse from an
*       external interface.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*
* OUTPUTS:
*       functionPtr     - (pointer to) One of the four possible TOD counter
*                         functions
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterFunctionGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT                 direction,
    OUT CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT      *functionPtr
)
{
    return cpssDxChPtpPortGroupTodCounterFunctionGet(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            direction,
                                            functionPtr);
}

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionTriggerSet
*
* DESCRIPTION:
*       Trigger TOD (Time of Day) counter function accorging to the function set
*       by cpssDxChPtpTodCounterFunctionSet().
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - when previous TOD triggered action is not finished yet
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterFunctionTriggerSet
(
    IN GT_U8                    devNum,
    IN CPSS_PORT_DIRECTION_ENT  direction
)
{
    return cpssDxChPtpPortGroupTodCounterFunctionTriggerSet(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            direction);
}

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionTriggerGet
*
* DESCRIPTION:
*       Get status of trigger TOD (Time of Day) counter function.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - The trigger is on, the TOD
*                                                function is not finished.
*                         GT_FALSE - The trigger is off, the TOD function is
*                                   finished.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterFunctionTriggerGet
(
    IN GT_U8                    devNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_BOOL                 *enablePtr
)
{
    return cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            direction,
                                            enablePtr);
}

/*******************************************************************************
* cpssDxChPtpTodCounterShadowSet
*
* DESCRIPTION:
*       Configure TOD timestamping shadow counter values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       todCounterPtr   - pointer to TOD counter shadow
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterShadowSet
(
    IN GT_U8                        devNum,
    IN CPSS_PORT_DIRECTION_ENT      direction,
    IN CPSS_DXCH_PTP_TOD_COUNT_STC  *todCounterPtr
)
{
    return cpssDxChPtpPortGroupTodCounterShadowSet(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            direction,
                                            todCounterPtr);
}

/*******************************************************************************
* cpssDxChPtpTodCounterShadowGet
*
* DESCRIPTION:
*       Get TOD timestamping shadow counter values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter shadow
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterShadowGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_DIRECTION_ENT         direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
)
{
    return cpssDxChPtpPortGroupTodCounterShadowGet(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            direction,
                                            todCounterPtr);
}

/*******************************************************************************
* cpssDxChPtpTodCounterGet
*
* DESCRIPTION:
*       Get TOD timestamping counter value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterGet
(
    IN GT_U8                            devNum,
    IN CPSS_PORT_DIRECTION_ENT          direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
)
{
    return cpssDxChPtpPortGroupTodCounterGet(devNum,
                                             CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                             direction,
                                             todCounterPtr);
}

/*******************************************************************************
* cpssDxChPtpPortTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable port for PTP timestamping.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       portNum         - port number
*                         Note: For Ingress timestamping the trigger is based
*                               on the local source port.
*                               For Egress timestamping the trigger is based
*                               on the local target port.
*       enable          - GT_TRUE - The port is enabled for ingress/egress
*                                   timestamping.
*                         GT_FALSE - The port is disabled for ingress/egress
*                                   timestamping.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, direction or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortTimeStampEnableSet
(
    IN GT_U8                    devNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U8                    portNum,
    IN GT_BOOL                  enable
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* field offset inside register     */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    portGroupId;      /* the port group Id - support
                                            multi-port-groups device */
    GT_U8     localPort;        /* local port - support
                                            multi-port-groups device */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
        case CPSS_PORT_DIRECTION_TX_E:
        case CPSS_PORT_DIRECTION_BOTH_E:
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    fieldOffset = OFFSET_TO_BIT_MAC(localPort);
    hwData = BOOL2BIT_MAC(enable);


    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            ttiRegs.ptpTimeStampPortEnReg[OFFSET_TO_WORD_MAC(localPort)];

        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                fieldOffset, 1, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            haRegs.ptpTimeStampPortEnReg[OFFSET_TO_WORD_MAC(localPort)];

        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                fieldOffset, 1, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpPortTimeStampEnableGet
*
* DESCRIPTION:
*       Get port PTP timestamping status (enabled/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       portNum         - port number
*                         Note: For Ingress timestamping the trigger is based
*                               on the local source port.
*                               For Egress timestamping the trigger is based
*                               on the local target port.
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - The port is enabled for
*                                                ingress/egress timestamping.
*                         GT_FALSE - The port is disabled for ingress/egress
*                                   timestamping.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, direction or portNum.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortTimeStampEnableGet
(
    IN GT_U8                    devNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U8                    portNum,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* field offset inside register     */
    GT_U32    hwData;           /* data to read from hw             */
    GT_U32    portGroupId;      /* the port group Id - support
                                            multi-port-groups device */
    GT_U8     localPort;        /* local port - support
                                            multi-port-groups device */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    fieldOffset = OFFSET_TO_BIT_MAC(localPort);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                ttiRegs.ptpTimeStampPortEnReg[OFFSET_TO_WORD_MAC(localPort)];
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                haRegs.ptpTimeStampPortEnReg[OFFSET_TO_WORD_MAC(localPort)];
            break;
        default:
            return GT_BAD_PARAM;
    }

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                            fieldOffset, 1, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwData);

    return GT_OK;
}


/*******************************************************************************
* cpssDxChPtpEthernetTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Timestamping over Ethernet packets.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       enable          - GT_TRUE - PTP Timestamping is enabled over Ethernet.
*                         GT_FALSE - PTP Timestamping is disabled over Ethernet.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEthernetTimeStampEnableSet
(
    IN GT_U8                    devNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_BOOL                  enable
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
        case CPSS_PORT_DIRECTION_TX_E:
        case CPSS_PORT_DIRECTION_BOTH_E:
            break;
        default:
            return GT_BAD_PARAM;
    }

    hwData = BOOL2BIT_MAC(enable);


    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalConfig;

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 1, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalConfig;

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 1, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChPtpEthernetTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Timestamping status over Ethernet packets (enable/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Timestamping is enabled
*                                                over Ethernet.
*                         GT_FALSE - PTP Timestamping is disabled over Ethernet.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEthernetTimeStampEnableGet
(
    IN  GT_U8                    devNum,
    IN  CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_BOOL                  *enablePtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalConfig;
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalConfig;
            break;
        default:
            return GT_BAD_PARAM;
    }

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 1, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwData);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpUdpTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Timestamping over UDP packets.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       protocolStack   - types of IP to set: IPV4 or IPV6.
*       enable          - GT_TRUE - PTP Timestamping is enabled over UDP (IPV4/IPV6).
*                         GT_FALSE - PTP Timestamping is disabled over UDP (IPV4/IPV6).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, protocolStack or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpUdpTimeStampEnableSet
(
    IN  GT_U8                        devNum,
    IN  CPSS_PORT_DIRECTION_ENT      direction,
    IN  CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN  GT_BOOL                      enable
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* the offset of the field          */
    GT_U32    filedLength;      /* the length of the field          */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
        case CPSS_PORT_DIRECTION_TX_E:
        case CPSS_PORT_DIRECTION_BOTH_E:
            break;
        default:
            return GT_BAD_PARAM;
    }


    hwData = BOOL2BIT_MAC(enable);
    filedLength = 1;

    switch (protocolStack)
    {
        case CPSS_IP_PROTOCOL_IPV4_E:
            fieldOffset  = 1;
            break;
        case CPSS_IP_PROTOCOL_IPV6_E:
            fieldOffset = 2;
            break;
        case CPSS_IP_PROTOCOL_IPV4V6_E:
            fieldOffset = 1;
            filedLength = 2;
            hwData |= (hwData << 1);
            break;
        default:
            return GT_BAD_PARAM;
    }


    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalConfig;

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset,
                                       filedLength, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalConfig;

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, filedLength, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpUdpTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Timestamping status over UDP packets (enable/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       protocolStack   - types of IP to set: IPV4 or IPV6.
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Timestamping is enabled
*                                               over UDP (IPV4/IPV6).
*                         GT_FALSE - PTP Timestamping is disabled over UDP (IPV4/IPV6).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, protocolStack or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpUdpTimeStampEnableGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_DIRECTION_ENT         direction,
    IN  CPSS_IP_PROTOCOL_STACK_ENT      protocolStack,
    OUT GT_BOOL                         *enablePtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* the offset of the field          */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalConfig;
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalConfig;
            break;
        default:
            return GT_BAD_PARAM;
    }

    switch (protocolStack)
    {
        case CPSS_IP_PROTOCOL_IPV4_E:
            fieldOffset  = 1;
            break;
        case CPSS_IP_PROTOCOL_IPV6_E:
            fieldOffset = 2;
            break;
        default:
            return GT_BAD_PARAM;
    }

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwData);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpMessageTypeTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Timestamping for specific message type of the packet.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       messageType     - message type (0..15).
*       enable          - GT_TRUE - PTP Timestamping is enabled for specific message type.
*                         GT_FALSE - PTP Timestamping is disabled for specific message type.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpMessageTypeTimeStampEnableSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  messageType,
    IN  GT_BOOL                 enable
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* the offset of the field          */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    if (messageType > 15)
    {
        return GT_BAD_PARAM;
    }

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
        case CPSS_PORT_DIRECTION_TX_E:
        case CPSS_PORT_DIRECTION_BOTH_E:
            break;
        default:
            return GT_BAD_PARAM;
    }

    fieldOffset = messageType + 16;
    hwData = BOOL2BIT_MAC(enable);


    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpEnableTimeStamp;

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, 1, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpEnableTimeStamp;

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, 1, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpMessageTypeTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Timestamping status for specific message type of the packet
*       (enable/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       messageType     - message type (0..15).
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Timestamping is enabled for
*                                                specific message type.
*                         GT_FALSE - PTP Timestamping is disabled for specific
*                                    message type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpMessageTypeTimeStampEnableGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  messageType,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* the offset of the field          */
    GT_U32    hwData;           /* data to read from hw             */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (messageType > 15)
    {
        return GT_BAD_PARAM;
    }

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpEnableTimeStamp;
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpEnableTimeStamp;
            break;
        default:
            return GT_BAD_PARAM;
    }

    fieldOffset = messageType + 16;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwData);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTransportSpecificCheckEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Timestamping check of the packet's transport specific value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number
*       direction   - Tx, Rx or both directions
*       enable      - GT_TRUE - PTP Timestamping check of the packet's transport
*                               specific value is enabled.
*                     GT_FALSE - PTP Timestamping check of the packet's transport
*                                specific value is disabled.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTransportSpecificCheckEnableSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_BOOL                 enable
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
        case CPSS_PORT_DIRECTION_TX_E:
        case CPSS_PORT_DIRECTION_BOTH_E:
            break;
        default:
            return GT_BAD_PARAM;
    }

    hwData = BOOL2BIT_MAC(enable);


    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalConfig;

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 3, 1, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalConfig;

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 3, 1, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChPtpTransportSpecificCheckEnableGet
*
* DESCRIPTION:
*       Get status of PTP Timestamping check of the packet's transport specific
*       value (the check is enabled/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number
*       direction   - Tx or Rx
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - PTP Timestamping check of the
*                                   packet's transport specific value is enabled.
*                     GT_FALSE - PTP Timestamping check of the packet's transport
*                                specific value is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTransportSpecificCheckEnableGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalConfig;
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalConfig;
            break;
        default:
            return GT_BAD_PARAM;
    }

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 3, 1, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BOOL2BIT_MAC(hwData);

    return GT_OK;
}


/*******************************************************************************
* cpssDxChPtpTransportSpecificTimeStampEnableSet
*
* DESCRIPTION:
*       Enable PTP Timestamping for transport specific value of the packet.
*       Note: Relevant only if global configuration determines whether the
*       transport specific field should be checked by
*       cpssDxChPtpTransportSpecificCheckEnableSet().
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum                  - device number
*       direction               - Tx, Rx or both directions
*       transportSpecificVal    - transport specific value taken from PTP header
*                                 bits [7..4]. Acceptable values (0..15).
*       enable                  - GT_TRUE - PTP Timestamping is enabled for this
*                                           transport specific value.
*                                GT_FALSE - PTP Timestamping is disabled for this
*                                           transport specific value.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, transportSpecificVal or
*                                  direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTransportSpecificTimeStampEnableSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  transportSpecificVal,
    IN  GT_BOOL                 enable
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    if (transportSpecificVal > 15)
    {
        return GT_BAD_PARAM;
    }

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
        case CPSS_PORT_DIRECTION_TX_E:
        case CPSS_PORT_DIRECTION_BOTH_E:
            break;
        default:
            return GT_BAD_PARAM;
    }

    hwData = BOOL2BIT_MAC(enable);


    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpEnableTimeStamp;

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, transportSpecificVal,
                                       1, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpEnableTimeStamp;

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, transportSpecificVal,
                                       1, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChPtpTransportSpecificTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Timestamping status for specific message type of the packet
*       (enable/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       transportSpecificVal    - transport specific value taken from PTP header
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Timestamping is enabled for
*                                                this transport specific value.
*                          GT_FALSE - PTP Timestamping is disabled for this
*                                     transport specific value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, transportSpecificVal or
*                                  direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTransportSpecificTimeStampEnableGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  transportSpecificVal,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwData;           /* data to read from hw             */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (transportSpecificVal > 15)
    {
        return GT_BAD_PARAM;
    }

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpEnableTimeStamp;
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpEnableTimeStamp;
            break;
        default:
            return GT_BAD_PARAM;
    }

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, transportSpecificVal,
                                   1, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwData);

    return GT_OK;
}


/*******************************************************************************
* cpssDxChPtpTimestampEntryGet
*
* DESCRIPTION:
*       Read current Timestamp entry from the timestamp FIFO queue.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
* OUTPUTS:
*       entryPtr        - (pointer to) timestamp entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE               - the action succeeded and there are no more waiting
*                                  Timestamp entries
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTimestampEntryGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_DIRECTION_ENT             direction,
    OUT CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC   *entryPtr
)
{
    GT_U32    regAddr;          /* register address of LSB word     */
    GT_U32    regAddr1;         /* register address of MSB word     */
    GT_U32    hwData0 = 0x0;    /* data to read from hw             */
    GT_U32    hwData1 = 0x0;    /* data to read from hw             */
    GT_U32    portGroupId;      /* the port group Id - support multi-
                                                port-groups device  */
    GT_U32    activePortGroupsBmp; /* bitmap of active Port Groups  */
    GT_BOOL   entryFound;       /* flag for finding an entry        */
    GT_U32    portGroupsCheckedBmp; /* port groups bitmap that have
                                       been checked this time       */
    GT_U32    rc = GT_OK;       /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalFifoCurEntry[0];
            regAddr1 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalFifoCurEntry[1];
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalFifoCurEntry[0];
            regAddr1 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalFifoCurEntry[1];
            break;
        default:
            return GT_BAD_PARAM;
    }

    entryFound = GT_FALSE;
    portGroupsCheckedBmp = 0x0;

    /* get the current port group ID to serve */
    if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        portGroupId = PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.firstActivePortGroup;
    }
    else
    {
        portGroupId =
            PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.ptpNextPortGroupToServe;
    }


    while (entryFound == GT_FALSE)
    {
        /* start with bmp of all active port groups */
        activePortGroupsBmp = PRV_CPSS_PP_MAC(devNum)->
                                    portGroupsInfo.activePortGroupsBmp;

        /* look for port group to query */
        while(0 == (activePortGroupsBmp & (1 << portGroupId)))
        {
            if(portGroupId > PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.lastActivePortGroup)
            {
                portGroupId = PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.firstActivePortGroup;
                break;
            }

            portGroupId++;
        }


        /* Read the Lsb bits of the message */
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &hwData0);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Read the Msb bits of the message */
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr1, &hwData1);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* check if it is the last entry in this port group */
        if (((hwData0 & 0x3FFFFFF) == 0x3FFFFFF) & ((hwData1 & 0x3FFFFFFF) == 0x3FFFFFFF))
        {
            if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
            {
                break;
            }

            portGroupsCheckedBmp |= (1 << portGroupId);

            if (portGroupsCheckedBmp == activePortGroupsBmp)
            {
                /* getting here means, that all active port groups were checked
                   in current round and no entries were found */
                break;
            }

            portGroupId++;
        }
        else
        {
            entryFound = GT_TRUE;
        }
    }

    if (entryFound == GT_TRUE)
    {
        entryPtr->portNum =
            (GT_U8)PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(
                                    devNum, portGroupId, (hwData0 & 0x3f));
        entryPtr->messageType = ((hwData0 >> 6) & 0xf);
        entryPtr->sequenceId = ((hwData0 >> 10) & 0xffff);
        entryPtr->timeStampVal = (hwData1 & 0x3fffffff);

        /* save the info about the next port group to be served */
        PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.secureBreachNextPortGroupToServe =
            portGroupId + 1;
    }
    else
    {
        rc = GT_NO_MORE;
    }

    return rc;
}



/******************************************************************************/
/******************************************************************************/
/******* start of functions with portGroupsBmp parameter **********************/
/******************************************************************************/
/******************************************************************************/

/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterFunctionSet
*
* DESCRIPTION:
*       The function configures type of TOD counter action that will be
*       performed once triggered by cpssDxChPtpTodCounterFunctionTriggerSet()
*       API or by pulse from an external interface.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx, Rx or both directions
*       function        - One of the four possible TOD counter functions
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*                                  or function
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterFunctionSet
(
    IN GT_U8                                    devNum,
    IN GT_PORT_GROUPS_BMP                       portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT                  direction,
    IN CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT       function
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    portGroupId;      /* the port group Id - support multi-
                                                port-groups device  */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
        case CPSS_PORT_DIRECTION_TX_E:
        case CPSS_PORT_DIRECTION_BOTH_E:
            break;
        default:
            return GT_BAD_PARAM;
    }

    switch (function)
    {
        case CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E:
            hwData = 0;
            break;
        case CPSS_DXCH_PTP_TOD_COUNTER_FUNC_INCREMENT_E:
            hwData = 1;
            break;
        case CPSS_DXCH_PTP_TOD_COUNTER_FUNC_CAPTURE_E:
            hwData = 2;
            break;
        case CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GENERATE_E:
            hwData = 3;
            break;
        default:
            return GT_BAD_PARAM;

    }

    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalConfig;

        /* loop on all active port groups in the bmp */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                    regAddr, 4, 2, hwData);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalConfig;

        /* loop on all active port groups in the bmp */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                    regAddr, 4, 2, hwData);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterFunctionGet
*
* DESCRIPTION:
*       Get type of TOD counter action that will be performed once triggered by
*       cpssDxChPtpTodCounterFunctionTriggerSet() API or by pulse from an
*       external interface.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx or Rx
*
* OUTPUTS:
*       functionPtr     - (pointer to) One of the four possible TOD counter functions
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterFunctionGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PORT_GROUPS_BMP                      portGroupsBmp,
    IN  CPSS_PORT_DIRECTION_ENT                 direction,
    OUT CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT      *functionPtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    portGroupId;      /* the port group Id - support multi-
                                                port-groups device  */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(functionPtr);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.
                ptpGlobalConfig;
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                ptpGlobalConfig;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Get the first active port group */
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(devNum, portGroupsBmp,
                                                        portGroupId);

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,
                                            regAddr, 4, 2, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }


    switch (hwData)
    {
        case 0:
            *functionPtr = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E;
            break;
        case 1:
            *functionPtr = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_INCREMENT_E;
            break;
        case 2:
            *functionPtr = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_CAPTURE_E;
            break;
        case 3:
            *functionPtr = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GENERATE_E;
            break;
        default:
            /* should never get here */
            return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterFunctionTriggerSet
*
* DESCRIPTION:
*       Trigger TOD (Time of Day) counter function accorging to the function set
*       by cpssDxChPtpTodCounterFunctionSet().
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx, Rx or both directions
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - when previous TOD triggered action is not finished yet
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterFunctionTriggerSet
(
    IN GT_U8                    devNum,
    IN GT_PORT_GROUPS_BMP       portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT  direction
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    portGroupId;      /* the port group Id - support multi-
                                                port-groups device  */
    GT_BOOL   isTriggerOn;   /* trigger status flag: GT_TRUE - trigger is on */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
        case CPSS_PORT_DIRECTION_TX_E:
        case CPSS_PORT_DIRECTION_BOTH_E:
            break;
        default:
            return GT_BAD_PARAM;
    }


    /* Check trigger status for Rx/Tx or both directions */

    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        /* read trigger status */
        rc = cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(
                                                devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                CPSS_PORT_DIRECTION_RX_E,
                                                &isTriggerOn);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Check if trigger was already cleared (action finished) */
        if (isTriggerOn == GT_TRUE)
        {
            return GT_BAD_STATE;
        }
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        /* read trigger status */
        rc = cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(
                                                devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                CPSS_PORT_DIRECTION_TX_E,
                                                &isTriggerOn);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Check if trigger was already cleared (TOD function finished) */
        if (isTriggerOn == GT_TRUE)
        {
            return GT_BAD_STATE;
        }
    }

    /* Set the trigger */

    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalConfig;

        /* loop on all active port groups in the bmp */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                    regAddr, 6, 1, 1);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalConfig;

        /* loop on all active port groups in the bmp */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                    regAddr, 6, 1, 1);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterFunctionTriggerGet
*
* DESCRIPTION:
*       Get status of trigger TOD (Time of Day) counter function.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx or Rx
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - The trigger is on, the TOD
*                                               function is not finished.
*                         GT_FALSE - The trigger is off, the TOD function is
*                                   finished.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterFunctionTriggerGet
(
    IN GT_U8                    devNum,
    IN GT_PORT_GROUPS_BMP       portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.
                ptpGlobalConfig;
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                ptpGlobalConfig;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* check that all port groups are ready */
    rc = prvCpssDxChPortGroupBusyWait(devNum, portGroupsBmp, regAddr, 6,
                                      GT_TRUE);/* only to check the bit --> no 'busy wait' */
    if(rc == GT_BAD_STATE)
    {
        rc = GT_OK;
        /* not all port groups has the bit with value 0 */
        *enablePtr = GT_TRUE;
    }
    else
    {
        *enablePtr = GT_FALSE;
    }

    return rc;
}


/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterShadowSet
*
* DESCRIPTION:
*       Configure TOD timestamping shadow counter values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx, Rx or both directions
*       todCounterPtr   - pointer to TOD counter shadow
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterShadowSet
(
    IN GT_U8                        devNum,
    IN GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT      direction,
    IN CPSS_DXCH_PTP_TOD_COUNT_STC  *todCounterPtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    portGroupId;      /* the port group Id - support multi-
                                                port-groups device  */
    GT_U32    i;                /* loop iterator                    */
    GT_U32    nanoSecHwVal;     /* hw value of nanoseconds          */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(todCounterPtr);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
        case CPSS_PORT_DIRECTION_TX_E:
        case CPSS_PORT_DIRECTION_BOTH_E:
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Check if TOD nanoseconds errata is enabled */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                               PRV_CPSS_DXCH_XCAT_TOD_NANO_SEC_SET_WA_E))
    {
        /* nanoseconds value should be multiplies of 20ns */
        nanoSecHwVal = (todCounterPtr->nanoSeconds / 20);
        nanoSecHwVal *= 20;
    }
    else
    {
        nanoSecHwVal = todCounterPtr->nanoSeconds;
    }


    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        /* loop on all active port groups in the bmp */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpTodCntrShadowNanoSeconds;

            /* Write nanoseconds */
            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,
                                                      regAddr, nanoSecHwVal);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* Write two words of seconds */
            for (i = 0; i < 2; i++)
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpTodCntrShadowSeconds[i];

                rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,
                                                          regAddr, todCounterPtr->seconds.l[i]);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        /* loop on all active port groups in the bmp */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpTodCntrShadowNanoSeconds;

            /* Write nanoseconds */
            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,
                                                      regAddr, nanoSecHwVal);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* Write two words of seconds */
            for (i = 0; i < 2; i++)
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpTodCntrShadowSeconds[i];

                rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,
                                                          regAddr, todCounterPtr->seconds.l[i]);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterShadowGet
*
* DESCRIPTION:
*       Get TOD timestamping shadow counter values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter shadow
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterShadowGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  CPSS_PORT_DIRECTION_ENT         direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
)
{
    GT_U32    regAddrSec;       /* register address                 */
    GT_U32    regAddrNanoSec;   /* register address                 */
    GT_U32    hwData;           /* data to read from hw             */
    GT_U32    portGroupId;      /* the port group Id - support multi-
                                                port-groups device  */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(todCounterPtr);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddrNanoSec = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                ttiRegs.ptpTodCntrShadowNanoSeconds;
            regAddrSec = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                ttiRegs.ptpTodCntrShadowSeconds[0];
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            regAddrNanoSec = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                haRegs.ptpTodCntrShadowNanoSeconds;
            regAddrSec = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                haRegs.ptpTodCntrShadowSeconds[0];
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Get the first active port group */
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(devNum, portGroupsBmp,
                                                        portGroupId);

    /* Read nanoseconds */
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddrNanoSec, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }
    todCounterPtr->nanoSeconds = hwData;

    /* Read two words of seconds */
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddrSec, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }
    todCounterPtr->seconds.l[0] = hwData;
    regAddrSec += 4;

    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddrSec, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }
    todCounterPtr->seconds.l[1] = hwData;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterGet
*
* DESCRIPTION:
*       Get TOD timestamping counter value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterGet
(
    IN GT_U8                            devNum,
    IN GT_PORT_GROUPS_BMP               portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT          direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
)
{
    GT_U32    regAddrSec;       /* register address                 */
    GT_U32    regAddrNanoSec;   /* register address                 */
    GT_U32    hwData;           /* data to read from hw             */
    GT_U32    portGroupId;      /* the port group Id - support multi-
                                                port-groups device  */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(todCounterPtr);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddrNanoSec = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                ttiRegs.ptpTodCntrNanoSeconds;
            regAddrSec = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                ttiRegs.ptpTodCntrSeconds[0];
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            regAddrNanoSec = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                haRegs.ptpTodCntrNanoSeconds;
            regAddrSec = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                haRegs.ptpTodCntrSeconds[0];
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Get the first active port group */
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(devNum, portGroupsBmp,
                                                        portGroupId);

    /* Read nanoseconds */
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddrNanoSec, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }
    todCounterPtr->nanoSeconds = hwData;

    /* Read two words of seconds */
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddrSec, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }
    todCounterPtr->seconds.l[0] = hwData;
    regAddrSec += 4;

    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddrSec, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }
    todCounterPtr->seconds.l[1] = hwData;

    return GT_OK;
}
