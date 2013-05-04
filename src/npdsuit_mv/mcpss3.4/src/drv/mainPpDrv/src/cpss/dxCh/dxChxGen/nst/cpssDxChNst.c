/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChNst.c
*
* DESCRIPTION:
*       Function implementation for Network Shield Technology configuration.
*
* DEPENDENCIES:
*
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>
#include <cpss/dxCh/dxChxGen/nst/cpssDxChNst.h>
/* check errata */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChErrataMng.h>

#define PRV_CPSS_DXCH_NST_MAC_ACCESS_LEVELS_MAX_NUM_CNS (8)

/* check access level validity */
#define PRV_CPSS_DXCH2_NST_CHECK_MAC_ACCESS_LEVEL_MAC(accessLevel)        \
    if((accessLevel) >= PRV_CPSS_DXCH_NST_MAC_ACCESS_LEVELS_MAX_NUM_CNS)  \
    {                                                                     \
        return GT_BAD_PARAM;                                              \
    }

/* set default access level field offset according to the type */
#define PRV_CPSS_DXCH2_NST_DEFAULT_ACCESS_LEVEL_FIELD_OFFSET_SET_MAC(        \
    _paramType, _fieldOffset)                                                \
    switch (_paramType)                                                      \
    {                                                                        \
       case CPSS_NST_AM_SA_AUTO_LEARNED_E:                                   \
           _fieldOffset = 8;                                                 \
           break;                                                            \
       case CPSS_NST_AM_DA_AUTO_LEARNED_E:                                   \
           _fieldOffset = 12;                                                \
           break;                                                            \
       case CPSS_NST_AM_SA_UNKNOWN_E:                                        \
           _fieldOffset = 0;                                                 \
           break;                                                            \
       case CPSS_NST_AM_DA_UNKNOWN_E:                                        \
           _fieldOffset = 4;                                                 \
           break;                                                            \
       default:                                                              \
           return GT_BAD_PARAM;                                              \
    }

/* set sanity check field offset and index for register address setting
   according sanity check type */
#define PRV_CPSS_DXCH2_NST_SANITY_CHECK_TYPE_FIELD_OFFSET_SET_MAC(           \
    _checkType, _index, _fieldOffset)                                        \
    switch(_checkType)                                                       \
    {                                                                        \
        case CPSS_NST_CHECK_TCP_SYN_DATA_E:                                  \
            _fieldOffset = 29;                                               \
            _index = 0;                                                      \
            break;                                                           \
        case CPSS_NST_CHECK_TCP_OVER_MAC_MC_BC_E:                            \
            _fieldOffset = 17;                                               \
            _index = 0;                                                      \
            break;                                                           \
        case CPSS_NST_CHECK_TCP_FLAG_ZERO_E:                                   \
            _fieldOffset = 4;                                                \
            _index = 3;                                                      \
            break;                                                           \
        case CPSS_NST_CHECK_TCP_FLAGS_FIN_URG_PSH_E:                         \
            _fieldOffset = 3;                                                \
            _index = 3;                                                      \
            break;                                                           \
        case CPSS_NST_CHECK_TCP_FLAGS_SYN_FIN_E:                             \
            _fieldOffset = 2;                                                \
            _index = 3;                                                      \
            break;                                                           \
        case CPSS_NST_CHECK_TCP_FLAGS_SYN_RST_E:                             \
            _fieldOffset = 1;                                                \
            _index = 3;                                                      \
            break;                                                           \
        case CPSS_NST_CHECK_TCP_UDP_PORT_ZERO_E:                             \
            _fieldOffset = 0;                                                \
            _index = 3;                                                      \
            break;                                                           \
        case CPSS_NST_CHECK_FRAG_IPV4_ICMP_E:                                \
            _fieldOffset = 5;                                                \
            _index = 3;                                                      \
            break;                                                           \
        case CPSS_NST_CHECK_ARP_MAC_SA_MISMATCH_E:                           \
            _fieldOffset = 16;                                               \
            _index = 0;                                                      \
            break;                                                           \
        default:                                                             \
            return GT_BAD_PARAM;                                             \
    }


/* set ingress forward filter register address according to filter type */
#define PRV_CPSS_DXCH2_NST_PORT_INGRESS_FRW_FILTER_REG_ADDRESS_SET_MAC(      \
    _devNum, _filterType, _regAddr)                                          \
   switch(_filterType)                                                       \
   {                                                                         \
       case CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E:                        \
           _regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->                  \
                             bridgeRegs.nstRegs.netIngrFrwFltConfReg;        \
           break;                                                            \
       case CPSS_NST_INGRESS_FRW_FILTER_TO_CPU_E:                            \
           _regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->                  \
                             bridgeRegs.nstRegs.cpuIngrFrwFltConfReg;        \
           break;                                                            \
       case CPSS_NST_INGRESS_FRW_FILTER_TO_ANALYZER_E:                       \
           _regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->                  \
                             bridgeRegs.nstRegs.analyzerIngrFrwFltConfReg;   \
           break;                                                            \
       default:                                                              \
           return GT_BAD_PARAM;                                              \
   }


/* set egress forward filter register address according to filter type */
#define PRV_CPSS_DXCH2_NST_PORT_EGRESS_FRW_FILTER_REG_ADDRESS_SET_MAC(         \
    _devNum, _filterType, _regAddr)                                            \
   switch(_filterType)                                                         \
   {                                                                           \
       case CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E:                             \
             _regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->                  \
                                     bridgeRegs.nstRegs.cpuPktsFrwFltConfReg;  \
            break;                                                             \
       case CPSS_NST_EGRESS_FRW_FILTER_BRIDGED_E:                              \
            _regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->                   \
                                     bridgeRegs.nstRegs.brgPktsFrwFltConfReg;  \
            break;                                                             \
       case CPSS_NST_EGRESS_FRW_FILTER_ROUTED_E:                               \
            _regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->                   \
                                     bridgeRegs.nstRegs.routePktsFrwFltConfReg;\
            break;                                                             \
       default:                                                                \
            return GT_BAD_PARAM;                                               \
   }

/* for TXQ ver 1 : get egress forward filter register address and bit offset
    according to filter type , and port */
#define TXQ_DISTIBUTER_FORWARD_RESTRICTED_FILTER_REG_ADDR_AND_BIT_MAC(         \
    _devNum, _portNum,_filterType, _regAddr,_bitOffset)                        \
                                                                               \
   _bitOffset = OFFSET_TO_BIT_MAC(_portNum);                                   \
   switch(_filterType)                                                         \
   {                                                                           \
       case CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E:                             \
             _regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->                  \
                        txqVer1.queue.distributor.                             \
                        fromCpuForwardRestricted[OFFSET_TO_WORD_MAC(_portNum)];\
            break;                                                             \
       case CPSS_NST_EGRESS_FRW_FILTER_BRIDGED_E:                              \
            _regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->                   \
                        txqVer1.queue.distributor.                             \
                        bridgedForwardRestricted[OFFSET_TO_WORD_MAC(_portNum)];\
            break;                                                             \
       case CPSS_NST_EGRESS_FRW_FILTER_ROUTED_E:                               \
            _regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->                   \
                        txqVer1.queue.distributor.                             \
                        routedForwardRestricted[OFFSET_TO_WORD_MAC(_portNum)]; \
            break;                                                             \
       default:                                                                \
            return GT_BAD_PARAM;                                               \
   }


/*******************************************************************************
* prvCpssDxChNstAccessMatrixCmdSet
*
* DESCRIPTION:
*     Set bridge/ip access matrix entry.
*     The packet is assigned two access levels based on its MAC SA/SIP and MAC
*     DA/DIP.
*     The device supports up to 8  SA and up to 8 DA levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      saAccessLevel  - Source address access level
*      daAccessLevel  - Destination address access level
*      command        - command assigned to a packet
*      isBridge       - weather to set the bridge or the ip access matrix.
*
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel,
*                            command.
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChNstAccessMatrixCmdSet
(
    IN GT_U8                       devNum,
    IN GT_U32                      saAccessLevel,
    IN GT_U32                      daAccessLevel,
    IN CPSS_PACKET_CMD_ENT         command,
    IN GT_BOOL                     isBridge
)
{
    GT_U32    matrixAccessRegAddr;/* register address */
    GT_U32    hwValueOfCmd;       /* Hardware value of command  */
    GT_U32    hwData;             /* data that is read/written from/to register */
    GT_STATUS rc;                 /* return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH2_NST_CHECK_MAC_ACCESS_LEVEL_MAC(saAccessLevel);
    PRV_CPSS_DXCH2_NST_CHECK_MAC_ACCESS_LEVEL_MAC(daAccessLevel);

    switch(command)
    {
        case CPSS_PACKET_CMD_FORWARD_E:
            hwValueOfCmd = 0;
            break;
        case CPSS_PACKET_CMD_DROP_SOFT_E:
            hwValueOfCmd = 1;
            break;
        case CPSS_PACKET_CMD_DROP_HARD_E:
            hwValueOfCmd = 2;
            break;
        default:
            return GT_BAD_PARAM;
    }

    if (isBridge == GT_TRUE)
    {

        matrixAccessRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                           bridgeRegs.nstRegs.brgAccessMatrix;
    }
    else
    {
        matrixAccessRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                           ipRegs.routerAccessMatrixBase;
    }

    /* FEr#1006 - Bridge Access Matrix Line <7> register is write only.
       Shadow of the register should be used by SW.*/
    if((PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
          PRV_CPSS_DXCH2_BRIDGE_ACCESS_MATRIX_LINE7_WRITE_WA_E) == GT_TRUE) &&
          (isBridge == GT_TRUE) && (saAccessLevel == 7))
    {
        hwData =  (PRV_CPSS_DXCH_PP_MAC(devNum))->
                                          errata.bridgeAccessMatrixLine7RegData;

        U32_SET_FIELD_MASKED_MAC(hwData,(daAccessLevel * 2),2,hwValueOfCmd);

        rc = prvCpssDrvHwPpWriteRegister(devNum,
                                 (matrixAccessRegAddr + saAccessLevel * 0x4),
                                 hwData);
        if(rc != GT_OK)
            return rc;
        (PRV_CPSS_DXCH_PP_MAC(devNum))->
            errata.bridgeAccessMatrixLine7RegData = hwData;

        return GT_OK;
    }
    else
    {

        /* Set bridge access matrix entry */
        return prvCpssDrvHwPpSetRegField(devNum,
                                 (matrixAccessRegAddr + saAccessLevel * 0x4),
                                  daAccessLevel * 2, 2, hwValueOfCmd);
    }

}

/*******************************************************************************
* prvCpssDxChNstAccessMatrixCmdGet
*
* DESCRIPTION:
*     Get bridge/ip access matrix entry.
*     The packet is assigned two access levels based on its MAC SA/SIP and MAC
*     DA/DIP.
*     The device supports up to 8  SA and up to 8 DA levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      saAccessLevel  - Source address access level
*      daAccessLevel  - Destination address access level
*      isBridge       - weather to set the bridge or the ip access matrix.
*
* OUTPUTS:
*      commandPtr     - command assigned to a packet
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PTR         - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChNstAccessMatrixCmdGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      saAccessLevel,
    IN  GT_U32                      daAccessLevel,
    OUT CPSS_PACKET_CMD_ENT         *commandPtr,
    IN  GT_BOOL                     isBridge
)
{
    GT_U32    matrixAccessRegAddr;/* register address */
    GT_U32    hwValueOfCmd;       /* Hardware value of command  */
    GT_U32    regData;            /* register data */
    GT_STATUS rc;                 /* return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH2_NST_CHECK_MAC_ACCESS_LEVEL_MAC(saAccessLevel);
    PRV_CPSS_DXCH2_NST_CHECK_MAC_ACCESS_LEVEL_MAC(daAccessLevel);
    CPSS_NULL_PTR_CHECK_MAC(commandPtr);


    if (isBridge == GT_TRUE)
    {

        matrixAccessRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                           bridgeRegs.nstRegs.brgAccessMatrix;
    }
    else
    {
        matrixAccessRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                           ipRegs.routerAccessMatrixBase;
    }

    /* Get bridge access matrix entry */
    /* FEr#1006 - Bridge Access Matrix Line <7> register is write only.
       Shadow of the register should be used by SW.*/
    if((PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
          PRV_CPSS_DXCH2_BRIDGE_ACCESS_MATRIX_LINE7_WRITE_WA_E) == GT_TRUE) &&
          (isBridge == GT_TRUE) && (saAccessLevel == 7))
    {
        regData =  (PRV_CPSS_DXCH_PP_MAC(devNum))->
                                          errata.bridgeAccessMatrixLine7RegData;
        hwValueOfCmd = U32_GET_FIELD_MAC(regData,(daAccessLevel * 2),2);

    }

    else
    {
        /* Get bridge access matrix entry */
        rc = prvCpssDrvHwPpGetRegField(devNum,
                                 (matrixAccessRegAddr + saAccessLevel * 0x4),
                                  daAccessLevel * 2, 2, &hwValueOfCmd);
        if(rc != GT_OK)
            return rc;
    }

    switch(hwValueOfCmd)
    {
        case 0:
            *commandPtr = CPSS_PACKET_CMD_FORWARD_E;
            break;
        case 1:
            *commandPtr = CPSS_PACKET_CMD_DROP_SOFT_E;
            break;
        case 2:
            *commandPtr = CPSS_PACKET_CMD_DROP_HARD_E;
            break;
        default:
            *commandPtr = hwValueOfCmd;
    }

    return GT_OK;

}

/*******************************************************************************
* cpssDxChNstBridgeAccessMatrixCmdSet
*
* DESCRIPTION:
*     Set bridge access matrix entry.
*     The packet is assigned two access levels based on its MAC SA/SIP and MAC
*     DA/DIP.
*     The device supports up to 8  SA and up to 8 DA levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      saAccessLevel  - Source address access level
*      daAccessLevel  - Destination address access level
*      command        - command assigned to a packet
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstBridgeAccessMatrixCmdSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      saAccessLevel,
    IN  GT_U32                      daAccessLevel,
    IN  CPSS_PACKET_CMD_ENT         command
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    return prvCpssDxChNstAccessMatrixCmdSet(devNum,saAccessLevel,daAccessLevel,
                                            command,GT_TRUE);

}

/*******************************************************************************
* cpssDxChNstBridgeAccessMatrixCmdGet
*
* DESCRIPTION:
*     Get bridge access matrix entry.
*     The packet is assigned two access levels based on its MAC SA/SIP and MAC
*     DA/DIP.
*     The device supports up to 8  SA and up to 8 DA levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      saAccessLevel  - Source address access level
*      daAccessLevel  - Destination address access level
*
* OUTPUTS:
*      commandPtr     - command assigned to a packet
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstBridgeAccessMatrixCmdGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      saAccessLevel,
    IN  GT_U32                      daAccessLevel,
    OUT CPSS_PACKET_CMD_ENT         *commandPtr
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    return prvCpssDxChNstAccessMatrixCmdGet(devNum,saAccessLevel,daAccessLevel,
                                            commandPtr,GT_TRUE);
}

/*******************************************************************************
* cpssDxChNstDefaultAccessLevelsSet
*
* DESCRIPTION:
*     Set default access levels for  Bridge Access Matrix Configuration table
*     that controls which access level pairs can communicate with each other.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      paramType    -   type of bridge acess matrix global parameter to
*                       configure default access level .
*      accessLevel  -  default security access level for parameter
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum,paramType,accessLevel
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstDefaultAccessLevelsSet
(
    IN GT_U8                    devNum,
    IN CPSS_NST_AM_PARAM_ENT    paramType,
    IN GT_U32                   accessLevel
)
{
    GT_U32 secureLevelConfRegAddr;    /* register address */
    GT_U32 fieldOffset;               /* the start bit number in register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH2_NST_CHECK_MAC_ACCESS_LEVEL_MAC(accessLevel);


    secureLevelConfRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                     bridgeRegs.nstRegs.brgSecureLevelConfReg;


    PRV_CPSS_DXCH2_NST_DEFAULT_ACCESS_LEVEL_FIELD_OFFSET_SET_MAC(paramType,
                                                             fieldOffset);
    return /* Set default access levels*/
        prvCpssDrvHwPpSetRegField(devNum,secureLevelConfRegAddr,fieldOffset,
                                                               3,accessLevel);

}

/*******************************************************************************
* cpssDxChNstDefaultAccessLevelsGet
*
* DESCRIPTION:
*     Get default access levels for  Bridge Access Matrix Configuration table
*     that controls which access level pairs can communicate with each other.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      paramType    -   type of bridge acess matrix global parameter to
*                       configure default access level .
* OUTPUTS:
*      accessLevelPtr - (pointer to) default security access level for parameter
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum,paramType
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstDefaultAccessLevelsGet
(
    IN  GT_U8                    devNum,
    IN  CPSS_NST_AM_PARAM_ENT    paramType,
    OUT GT_U32                   *accessLevelPtr
)
{
    GT_U32 secureLevelConfRegAddr;    /* register address */
    GT_U32 fieldOffset;               /* the start bit number in register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH2_NST_DEFAULT_ACCESS_LEVEL_FIELD_OFFSET_SET_MAC(paramType,
                                                             fieldOffset);
    CPSS_NULL_PTR_CHECK_MAC(accessLevelPtr);


    secureLevelConfRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                     bridgeRegs.nstRegs.brgSecureLevelConfReg;

    /* Get default access levels */
    return  prvCpssDrvHwPpGetRegField(devNum,secureLevelConfRegAddr,
                                       fieldOffset,3,accessLevelPtr);

}


/*******************************************************************************
* cpssDxChNstProtSanityCheckSet
*
* DESCRIPTION:
*      Set packet sanity checks.
*      Sanity Check engine identifies "suspicious" packets and
*      provides an option for assigning them a Hard Drop packet command.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum      - physical device number
*      checkType - the type of sanity check
*      enable    - GT_TRUE  enable packet's sanity check. Packet that not passed
*                  check will be dropped and treated as security breach event.
*                - GT_FALSE disable packet's sanity check.
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum,checkType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstProtSanityCheckSet
(
    IN GT_U8                 devNum,
    IN CPSS_NST_CHECK_ENT    checkType,
    IN GT_BOOL               enable
)
{
    GT_U32 regAddr;       /* register address */
    GT_U32 data;          /* reg subfield data */
    GT_U32 fieldOffset;   /* the start bit number in register */
    GT_U32 index;         /* number of bridge global configuration register */
    GT_STATUS rc;         /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                     bridgeRegs.bridgeGlobalConfigRegArray[0];


    /* set all TCP Sanity checks */
    if(checkType == CPSS_NST_CHECK_TCP_ALL_E)
    {
        data = (enable == GT_TRUE) ? 0xFFFFFFFF : 0;
        rc = prvCpssDrvHwPpWriteRegBitMask(devNum,regAddr,0x20020000,data);
        if(rc != GT_OK)
            return rc;

        return prvCpssDrvHwPpWriteRegBitMask(devNum,
                                             regAddr + 0xC,0x0000001F,data);
    }

    data = (enable == GT_TRUE) ? 1 : 0;

    PRV_CPSS_DXCH2_NST_SANITY_CHECK_TYPE_FIELD_OFFSET_SET_MAC(
        checkType, index, fieldOffset);

    return /* set sanity check */
        prvCpssDrvHwPpSetRegField(devNum,regAddr + index * 4,fieldOffset,1,data);

}

/*******************************************************************************
* cpssDxChNstProtSanityCheckGet
*
* DESCRIPTION:
*      Get packet sanity checks.
*      Sanity Check engine identifies "suspicious" packets and
*      provides an option for assigning them a Hard Drop packet command.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum      - physical device number
*      checkType - the type of sanity check
* OUTPUTS:
*      enablePtr - GT_TRUE  enable packet's sanity check. Packet that not passed
*                  check will be dropped and treated as security breach event.
*                - GT_FALSE disable packet's sanity check.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum,checkType
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstProtSanityCheckGet
(
    IN  GT_U8                 devNum,
    IN  CPSS_NST_CHECK_ENT    checkType,
    OUT GT_BOOL               *enablePtr
)
{
    GT_U32 regAddr;       /* register address */
    GT_U32 data;          /* reg subfield data */
    GT_U32 fieldOffset;   /* the start bit number in register */
    GT_U32 index;         /* number of bridge global configuration register */
    GT_STATUS rc;         /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                     bridgeRegs.bridgeGlobalConfigRegArray[0];


    PRV_CPSS_DXCH2_NST_SANITY_CHECK_TYPE_FIELD_OFFSET_SET_MAC(
        checkType, index, fieldOffset);
     /* get sanity check */
    rc = prvCpssDrvHwPpGetRegField(devNum,regAddr + index * 4,
                                   fieldOffset,1,&data);
    if(rc != GT_OK)
        return rc;

    *enablePtr = (data == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChNstPortIngressFrwFilterSet
*
* DESCRIPTION:
*     Set port ingress forwarding filter.
*     For a given ingress port Enable/Disable traffic if it is destinied to:
*     CPU, ingress analyzer, network.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*       port        - port number
*       filterType  - ingress filter type
*       enable      - GT_TRUE - enable ingress forwarding restiriction according
*                     to filterType, filterred packets will be dropped.
*                   - GT_FALSE - disable ingress forwarding restiriction
*                     according to filterType
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, port, filterType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIngressFrwFilterSet
(
    IN GT_U8                            devNum,
    IN GT_U8                            port,
    IN CPSS_NST_INGRESS_FRW_FILTER_ENT  filterType,
    IN GT_BOOL                          enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg subfield data */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,port);

    data = (enable == GT_TRUE) ? 0 : 1;

    PRV_CPSS_DXCH2_NST_PORT_INGRESS_FRW_FILTER_REG_ADDRESS_SET_MAC(devNum,
                                                               filterType,
                                                               regAddr);
    /* Set port ingress forwarding filter */
    return prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, regAddr, localPort, 1, data);

}

/*******************************************************************************
* cpssDxChNstPortIngressFrwFilterGet
*
* DESCRIPTION:
*     Get port ingress forwarding filter.
*     For a given ingress port Enable/Disable traffic if it is destinied to:
*     CPU, ingress analyzer, network.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*       port        - port number
*       filterType  - ingress filter type
* OUTPUTS:
*       enablePtr   - GT_TRUE - enable ingress forwarding restiriction according
*                     to filterType, filterred packets will be dropped.
*                   - GT_FALSE - disable ingress forwarding restiriction
*                     according to filterType
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, port, filterType
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIngressFrwFilterGet
(
    IN  GT_U8                            devNum,
    IN  GT_U8                            port,
    IN  CPSS_NST_INGRESS_FRW_FILTER_ENT  filterType,
    OUT GT_BOOL                          *enablePtr
)
{
    GT_U32    regAddr;           /* register address */
    GT_STATUS rc;                /* return code */
    GT_U32    data;              /* reg subfield data */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,port);

    PRV_CPSS_DXCH2_NST_PORT_INGRESS_FRW_FILTER_REG_ADDRESS_SET_MAC(devNum,
                                                               filterType,
                                                               regAddr);
    /* Set port ingress forwarding filter */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, localPort, 1, &data);
    if(rc != GT_OK)
        return rc;

    *enablePtr = (data == 1) ? GT_FALSE : GT_TRUE;

    return GT_OK;

}


/*******************************************************************************
* cpssDxChNstPortEgressFrwFilterSet
*
* DESCRIPTION:
*     Set port egress forwarding filter.
*     For a given egress port Enable/Disable traffic if the packet was:
*     sent from CPU with FROM_CPU DSA tag, bridged or policy switched,
*     routed or policy routed.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*       port        - port number
*       filterType  - egress filter type
*       enable      - GT_TRUE - enable egress forwarding restiriction according
*                     to filterType, filterred packets will be dropped.
*                   - GT_FALSE - disable egress forwarding restiriction
*                     according to filterType.
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, port, filterType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortEgressFrwFilterSet
(
    IN GT_U8                            devNum,
    IN GT_U8                            port,
    IN CPSS_NST_EGRESS_FRW_FILTER_ENT   filterType,
    IN GT_BOOL                          enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg subfield data */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */
    GT_U32  bitOffset;/* bit offset in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,port);

    data = (enable == GT_TRUE) ? 1 : 0;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* Set port egress forwarding filter */
        PRV_CPSS_DXCH2_NST_PORT_EGRESS_FRW_FILTER_REG_ADDRESS_SET_MAC(devNum,
                                                                  filterType,
                                                                  regAddr);
        return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, localPort, 1, data);
    }
    else
    {
        TXQ_DISTIBUTER_FORWARD_RESTRICTED_FILTER_REG_ADDR_AND_BIT_MAC(devNum,
                                                                  port,
                                                                  filterType,
                                                                  regAddr,
                                                                  bitOffset);
        return prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, bitOffset, 1, data);
    }
}

/*******************************************************************************
* cpssDxChNstPortEgressFrwFilterGet
*
* DESCRIPTION:
*     Get port egress forwarding filter.
*     For a given egress port Enable/Disable traffic if the packet was:
*     sent from CPU with FROM_CPU DSA tag, bridged or policy switched,
*     routed or policy routed.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*       port        - port number
*       filterType  - egress filter type
* OUTPUTS:
*       enablePtr   - GT_TRUE - enable egress forwarding restiriction according
*                     to filterType, filterred packets will be dropped.
*                   - GT_FALSE - disable egress forwarding restiriction
*                     according to filterType.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, port, filterType
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortEgressFrwFilterGet
(
    IN  GT_U8                            devNum,
    IN  GT_U8                            port,
    IN  CPSS_NST_EGRESS_FRW_FILTER_ENT   filterType,
    OUT GT_BOOL                          *enablePtr
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg subfield data */
    GT_STATUS rc;                /* return code */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */
    GT_U32  bitOffset;/* bit offset in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,port);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        PRV_CPSS_DXCH2_NST_PORT_EGRESS_FRW_FILTER_REG_ADDRESS_SET_MAC(devNum,
                                                                  filterType,
                                                                  regAddr);
        /* Get port egress forwarding filter */
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, localPort, 1, &data);
    }
    else
    {
        TXQ_DISTIBUTER_FORWARD_RESTRICTED_FILTER_REG_ADDR_AND_BIT_MAC(devNum,
                                                                  port,
                                                                  filterType,
                                                                  regAddr,
                                                                  bitOffset);
        rc =  prvCpssDxChHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, bitOffset, 1, &data);
    }

    if(rc != GT_OK)
    {
        return rc;
    }



    *enablePtr = (data == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChNstIngressFrwFilterDropCntrSet
*
* DESCRIPTION:
*     Set the global ingress forwarding restriction drop packet counter.
*     This counter counts the number of packets dropped due to Ingress forward
*     restrictions.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*       ingressCnt  - the counter value
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstIngressFrwFilterDropCntrSet
(
    IN GT_U8       devNum,
    IN GT_U32      ingressCnt
)
{
    return cpssDxChNstPortGroupIngressFrwFilterDropCntrSet(devNum,
                                              CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                           ingressCnt);
}

/*******************************************************************************
* cpssDxChNstIngressFrwFilterDropCntrGet
*
* DESCRIPTION:
*     Reads the global ingress forwarding restriction drop packet counter.
*     This counter counts the number of packets dropped due to Ingress forward
*     restrictions.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*      ingressCntPtr - the counter value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstIngressFrwFilterDropCntrGet
(
    IN GT_U8       devNum,
    OUT GT_U32     *ingressCntPtr
)
{
    return cpssDxChNstPortGroupIngressFrwFilterDropCntrGet(devNum, 
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS, 
                                                           ingressCntPtr);
}

/*******************************************************************************
* cpssDxChNstRouterAccessMatrixCmdSet
*
* DESCRIPTION:
*     Set Routers access matrix entry.
*     The packet is assigned two access levels based on its SIP and DIP.
*     The device supports up to 8  SIP and up to 8 DIP levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*      devNum          - physical device number
*      sipAccessLevel  - the sip access level
*      dipAccessLevel  - the dip access level
*      command         - command assigned to a packet
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstRouterAccessMatrixCmdSet
(
    IN  GT_U8                      devNum,
    IN  GT_U32                     sipAccessLevel,
    IN  GT_U32                     dipAccessLevel,
    IN  CPSS_PACKET_CMD_ENT        command
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    /* Set router access matrix entry */
    return prvCpssDxChNstAccessMatrixCmdSet(devNum,sipAccessLevel,dipAccessLevel,
                                            command,GT_FALSE);
}

/*******************************************************************************
* cpssDxChNstRouterAccessMatrixCmdGet
*
* DESCRIPTION:
*     Get Routers access matrix entry.
*     The packet is assigned two access levels based on its SIP and DIP.
*     The device supports up to 8  SIP and up to 8 DIP levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*      devNum         - physical device number
*      sipAccessLevel  - the sip access level
*      dipAccessLevel  - the dip access level
*
* OUTPUTS:
*      commandPtr     - command assigned to a packet
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstRouterAccessMatrixCmdGet
(
    IN  GT_U8                      devNum,
    IN  GT_U32                     sipAccessLevel,
    IN  GT_U32                     dipAccessLevel,
    OUT  CPSS_PACKET_CMD_ENT       *commandPtr
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    /* Get router access matrix entry */
    return prvCpssDxChNstAccessMatrixCmdGet(devNum,sipAccessLevel,dipAccessLevel,
                                            commandPtr,GT_FALSE);
}

/*******************************************************************************
* cpssDxChNstPortGroupIngressFrwFilterDropCntrSet
*
* DESCRIPTION:
*     Set the global ingress forwarding restriction drop packet counter.
*     This counter counts the number of packets dropped due to Ingress forward
*     restrictions.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum          - physical device number
*       ingressCnt      - the counter value
*       portGroupsBmp   - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortGroupIngressFrwFilterDropCntrSet
(
    IN GT_U8                   devNum,
    IN GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN GT_U32                  ingressCnt
)
{
    GT_U32    regAddr;           /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                          bridgeRegs.nstRegs.ingressFrwDropCounter;

    /* set ingress drop counter */
    return prvCpssDxChPortGroupsBmpCounterSet(devNum, portGroupsBmp,
                                              regAddr,0, 32, ingressCnt);
}

/*******************************************************************************
* cpssDxChNstPortGroupIngressFrwFilterDropCntrGet
*
* DESCRIPTION:
*     Reads the global ingress forwarding restriction drop packet counter.
*     This counter counts the number of packets dropped due to Ingress forward
*     restrictions.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*      ingressCntPtr - the counter value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortGroupIngressFrwFilterDropCntrGet
(
    IN GT_U8                    devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *ingressCntPtr
)
{
    GT_U32    regAddr;           /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(ingressCntPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                          bridgeRegs.nstRegs.ingressFrwDropCounter;

    /* read ingress drop counter */
    return prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp, regAddr, 
                                                  0, 32, 
                                                  ingressCntPtr, NULL);
}


