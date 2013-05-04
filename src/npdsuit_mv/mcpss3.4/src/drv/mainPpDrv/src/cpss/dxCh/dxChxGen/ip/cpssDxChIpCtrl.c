/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIpCtrl.c
*
* DESCRIPTION:
*       the CPSS DXCH Ip HW control registers APIs
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <stdlib.h>

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpCtrl.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>

#define CHEETAH_MAX_MAC_QOS_ENTRIES_CNS   128

#define NUMBER_OF_EXCEPTION_DATABASES_CNS   5

/* check that the Qos profile Entry is in the valid range.
*/
#define PRV_CPSS_DXCH_QOS2MT_ENTRY_CHECK_MAC(qosProfile)  \
    if (qosProfile >= CHEETAH_MAX_MAC_QOS_ENTRIES_CNS ) \
    {                                                       \
        return GT_BAD_PARAM;                                \
    }



/*******************************************************************************
* cpssDxChIpSpecialRouterTriggerEnable
*
* DESCRIPTION:
*       Sets the special router trigger enable modes for packets with bridge
*       command other then FORWARD or MIRROR.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum              - the device number
*       bridgeExceptionCmd  - the bridge exception command the packet arrived to
*                             the router with.
*       enableRouterTrigger - enable /disable router trigger.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or bridgeExceptionCmd
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpSpecialRouterTriggerEnable
(
    IN GT_U8                           devNum,
    IN CPSS_DXCH_IP_BRG_EXCP_CMD_ENT   bridgeExceptionCmd,
    IN GT_BOOL                         enableRouterTrigger
)
{
    GT_U32    regAddr;              /* register address                   */

    GT_STATUS rc;                   /* return code                        */
    GT_U32      value;      /* value to write */
    GT_U32      offset;     /* offset in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    /* Get address of register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerGlobalReg;


    /* all bridgeExceptionCmd refer to the same register but in different offsets */
    /* find offset according to brgExcpCmd */


    switch (bridgeExceptionCmd)
    {
    case CPSS_DXCH_IP_BRG_UC_IPV4_TRAP_EXCP_CMD_E:
        offset = 26;
        break;
    case CPSS_DXCH_IP_BRG_UC_IPV6_TRAP_EXCP_CMD_E:
        offset = 24;
        break;
    case CPSS_DXCH_IP_BRG_UC_IPV4_SOFT_DROP_EXCP_CMD_E:
        offset = 22;
        break;
    case CPSS_DXCH_IP_BRG_UC_IPV6_SOFT_DROP_EXCP_CMD_E:
        offset = 20;
        break;
    case CPSS_DXCH_IP_BRG_MC_IPV4_TRAP_EXCP_CMD_E:
        offset = 25;
        break;
    case CPSS_DXCH_IP_BRG_MC_IPV6_TRAP_EXCP_CMD_E:
        offset = 23;
        break;
    case CPSS_DXCH_IP_BRG_MC_IPV4_SOFT_DROP_EXCP_CMD_E:
        offset = 21;
        break;
    case CPSS_DXCH_IP_BRG_MC_IPV6_SOFT_DROP_EXCP_CMD_E:
        offset = 19;
        break;
    case CPSS_DXCH_IP_BRG_ARP_TRAP_EXCP_CMD_E:
        offset = 28;
        break;
    case CPSS_DXCH_IP_BRG_ARP_SOFT_DROP_EXCP_CMD_E:
        offset = 27;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* write enable value at the calculated offset */
    value = (enableRouterTrigger == GT_TRUE) ? 1:0;
    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,offset,1,value);

    return rc;

}

/* Struct to hold exception command data */
typedef struct CPSS_DXCH_IP_EXCPTION_CMD_REG_INFO_STCT
{
    CPSS_DXCH_IP_EXCEPTION_TYPE_ENT excptType;
    CPSS_IP_PROTOCOL_STACK_ENT      protocolStack;
    CPSS_PACKET_CMD_ENT             cmd;
    GT_U8                           registerIndex;
    GT_U32                          offset;
    GT_U32                          length;
    GT_U32                          value;
} CPSS_DXCH_IP_EXCPTION_CMD_REG_INFO_STC;

/*******************************************************************************
* cpssDxChIpExceptionCommandSet
*
* DESCRIPTION:
*       set a specific exception command.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - the device number
*       exceptionType  - the exception type
*       protocolStack - whether to set for ipv4/v6 or both
*       command       - the command, for availble commands see
*                       CPSS_DXCH_IP_EXCEPTION_TYPE_ENT.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
* GalTis:
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpExceptionCommandSet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_IP_EXCEPTION_TYPE_ENT  exceptionType,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    IN CPSS_PACKET_CMD_ENT              command
)
{
    GT_STATUS   rc;             /* function return code */
    GT_U32      regAddr;        /* address of register to write to */
    GT_BOOL     ipv4EntryFound; /* ipv4 matching row was found */
    GT_U32      ipv6EntryFound; /* ipv6 matching row was found */
    GT_U32      i,j;            /* counters */

    /* static array that holds registers, offsets and values for each         */
    /* exception and each command supported; the structure is:                */
    /* <family type> <exception> <protocol> <cmd> <registerIndex> <offset> <length> <value> */

    /* exceptions entries that apply only to DxCh devices */
    static CPSS_DXCH_IP_EXCPTION_CMD_REG_INFO_STC dxChOnlyExceptions[] =
    {
        /* IPv4 Unicast Header Error command */
        {CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV4_E,          CPSS_PACKET_CMD_ROUTE_E,            5,16,1,0},
        {CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV4_E,          CPSS_PACKET_CMD_TRAP_TO_CPU_E,      5,16,1,1},
        /* IPv6 Unicast Header Error command */
        {CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV6_E,          CPSS_PACKET_CMD_ROUTE_E,            5,17,1,0},
        {CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV6_E,          CPSS_PACKET_CMD_TRAP_TO_CPU_E,      5,17,1,1},
        /* IPv4 Unicast Options/Hop-by-Hop Exception command */
        {CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_ROUTE_E,            5,20,1,0},
        {CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_TRAP_TO_CPU_E,      5,20,1,1},
        /* IPv6 Unicast Options/Hop-by-Hop Exception command */
        {CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV6_E,  CPSS_PACKET_CMD_ROUTE_E,            5,21,1,0},
        {CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV6_E,  CPSS_PACKET_CMD_TRAP_TO_CPU_E,      5,21,1,1},
        /* IPv4 Unicast TLL Exceeded command */
        {CPSS_DXCH_IP_EXCP_UC_TTL_EXCEED_E,CPSS_IP_PROTOCOL_IPV4_E,         CPSS_PACKET_CMD_ROUTE_E,            5,18,1,0},
        {CPSS_DXCH_IP_EXCP_UC_TTL_EXCEED_E,CPSS_IP_PROTOCOL_IPV4_E,         CPSS_PACKET_CMD_TRAP_TO_CPU_E,      5,18,1,1},
        /* IPv6 Unicast Hop Limit (TTL) Exceeded command */
        {CPSS_DXCH_IP_EXCP_UC_TTL_EXCEED_E,CPSS_IP_PROTOCOL_IPV6_E,         CPSS_PACKET_CMD_ROUTE_E,            5,19,1,0},
        {CPSS_DXCH_IP_EXCP_UC_TTL_EXCEED_E,CPSS_IP_PROTOCOL_IPV6_E,         CPSS_PACKET_CMD_TRAP_TO_CPU_E,      5,19,1,1}
    };

    /* exceptions entries that apply only to DxCh Devices supporting
       Policy based routing using PCL action as next hop */
    static CPSS_DXCH_IP_EXCPTION_CMD_REG_INFO_STC dxChPbrOnlyExceptions[] =
    {
        /* IPv4 Unicast Header Error command */
        {CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV4_E,          CPSS_PACKET_CMD_ROUTE_E,            6,16,1,0},
        {CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV4_E,          CPSS_PACKET_CMD_TRAP_TO_CPU_E,      6,16,1,1},
        /* IPv6 Unicast Header Error command */
        {CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV6_E,          CPSS_PACKET_CMD_ROUTE_E,            6,17,1,0},
        {CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV6_E,          CPSS_PACKET_CMD_TRAP_TO_CPU_E,      6,17,1,1},
        /* IPv4 Unicast Options/Hop-by-Hop Exception command */
        {CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_ROUTE_E,            6,20,1,0},
        {CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_TRAP_TO_CPU_E,      6,20,1,1},
        /* IPv6 Unicast Options/Hop-by-Hop Exception command */
        {CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV6_E,  CPSS_PACKET_CMD_ROUTE_E,            6,21,1,0},
        {CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV6_E,  CPSS_PACKET_CMD_TRAP_TO_CPU_E,      6,21,1,1},
        /* IPv4 Unicast TTL Exceeded command */
        {CPSS_DXCH_IP_EXCP_UC_TTL_EXCEED_E,CPSS_IP_PROTOCOL_IPV4_E,         CPSS_PACKET_CMD_ROUTE_E,            6,18,1,0},
        {CPSS_DXCH_IP_EXCP_UC_TTL_EXCEED_E,CPSS_IP_PROTOCOL_IPV4_E,         CPSS_PACKET_CMD_TRAP_TO_CPU_E,      6,18,1,1},
        /* IPv6 Unicast Hop Limit (TTL) Exceeded command */
        {CPSS_DXCH_IP_EXCP_UC_TTL_EXCEED_E,CPSS_IP_PROTOCOL_IPV6_E,         CPSS_PACKET_CMD_ROUTE_E,            6,19,1,0},
        {CPSS_DXCH_IP_EXCP_UC_TTL_EXCEED_E,CPSS_IP_PROTOCOL_IPV6_E,         CPSS_PACKET_CMD_TRAP_TO_CPU_E,      6,19,1,1}
    };


    /* exceptions entries that apply to DxCh2 and above devices */
    static CPSS_DXCH_IP_EXCPTION_CMD_REG_INFO_STC dxCh2AndAboveExceptions[] =
    {
        /* IPv4 Unicast Header Error command */
        {CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV4_E,          CPSS_PACKET_CMD_TRAP_TO_CPU_E,      0,15,3,2},
        {CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV4_E,          CPSS_PACKET_CMD_DROP_HARD_E,        0,15,3,3},
        {CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV4_E,          CPSS_PACKET_CMD_DROP_SOFT_E,        0,15,3,4},
        /* IPv6 Unicast Header Error command */
        {CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV6_E,          CPSS_PACKET_CMD_TRAP_TO_CPU_E,      1,15,3,2},
        {CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV6_E,          CPSS_PACKET_CMD_DROP_HARD_E,        1,15,3,3},
        {CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV6_E,          CPSS_PACKET_CMD_DROP_SOFT_E,        1,15,3,4},
        /* IPv4 Multicast Header Error command */
        {CPSS_DXCH_IP_EXCP_MC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV4_E,          CPSS_PACKET_CMD_TRAP_TO_CPU_E,      0,12,3,2},
        {CPSS_DXCH_IP_EXCP_MC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV4_E,          CPSS_PACKET_CMD_DROP_HARD_E,        0,12,3,3},
        {CPSS_DXCH_IP_EXCP_MC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV4_E,          CPSS_PACKET_CMD_DROP_SOFT_E,        0,12,3,4},
        {CPSS_DXCH_IP_EXCP_MC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV4_E,          CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E,0,12,3,5},
        {CPSS_DXCH_IP_EXCP_MC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV4_E,          CPSS_PACKET_CMD_BRIDGE_E ,          0,12,3,6},
        /* IPv6 Multicast Header Error command */
        {CPSS_DXCH_IP_EXCP_MC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV6_E,          CPSS_PACKET_CMD_TRAP_TO_CPU_E,      1,12,3,2},
        {CPSS_DXCH_IP_EXCP_MC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV6_E,          CPSS_PACKET_CMD_DROP_HARD_E,        1,12,3,3},
        {CPSS_DXCH_IP_EXCP_MC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV6_E,          CPSS_PACKET_CMD_DROP_SOFT_E,        1,12,3,4},
        {CPSS_DXCH_IP_EXCP_MC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV6_E,          CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E,1,12,3,5},
        {CPSS_DXCH_IP_EXCP_MC_HDR_ERROR_E,CPSS_IP_PROTOCOL_IPV6_E,          CPSS_PACKET_CMD_BRIDGE_E,           1,12,3,6},
        /* IPv4 Unicast Illegal Address command */
        {CPSS_DXCH_IP_EXCP_UC_ILLEGAL_ADDRESS_E,CPSS_IP_PROTOCOL_IPV4_E,    CPSS_PACKET_CMD_TRAP_TO_CPU_E,      0,27,3,2},
        {CPSS_DXCH_IP_EXCP_UC_ILLEGAL_ADDRESS_E,CPSS_IP_PROTOCOL_IPV4_E,    CPSS_PACKET_CMD_DROP_HARD_E,        0,27,3,3},
        {CPSS_DXCH_IP_EXCP_UC_ILLEGAL_ADDRESS_E,CPSS_IP_PROTOCOL_IPV4_E,    CPSS_PACKET_CMD_DROP_SOFT_E,        0,27,3,4},
        /* IPv6 Unicast Illegal Address command */
        {CPSS_DXCH_IP_EXCP_UC_ILLEGAL_ADDRESS_E,CPSS_IP_PROTOCOL_IPV6_E,    CPSS_PACKET_CMD_TRAP_TO_CPU_E,      1,27,3,2},
        {CPSS_DXCH_IP_EXCP_UC_ILLEGAL_ADDRESS_E,CPSS_IP_PROTOCOL_IPV6_E,    CPSS_PACKET_CMD_DROP_HARD_E,        1,27,3,3},
        {CPSS_DXCH_IP_EXCP_UC_ILLEGAL_ADDRESS_E,CPSS_IP_PROTOCOL_IPV6_E,    CPSS_PACKET_CMD_DROP_SOFT_E,        1,27,3,4},
        /* IPv4 Multicast Illegal Address command */
        {CPSS_DXCH_IP_EXCP_MC_ILLEGAL_ADDRESS_E,CPSS_IP_PROTOCOL_IPV4_E,    CPSS_PACKET_CMD_TRAP_TO_CPU_E,      0,24,3,2},
        {CPSS_DXCH_IP_EXCP_MC_ILLEGAL_ADDRESS_E,CPSS_IP_PROTOCOL_IPV4_E,    CPSS_PACKET_CMD_DROP_HARD_E,        0,24,3,3},
        {CPSS_DXCH_IP_EXCP_MC_ILLEGAL_ADDRESS_E,CPSS_IP_PROTOCOL_IPV4_E,    CPSS_PACKET_CMD_DROP_SOFT_E,        0,24,3,4},
        {CPSS_DXCH_IP_EXCP_MC_ILLEGAL_ADDRESS_E,CPSS_IP_PROTOCOL_IPV4_E,    CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E,0,24,3,5},
        {CPSS_DXCH_IP_EXCP_MC_ILLEGAL_ADDRESS_E,CPSS_IP_PROTOCOL_IPV4_E,    CPSS_PACKET_CMD_BRIDGE_E,           0,24,3,6},
        /* IPv6 Multicast Illegal Address command */
        {CPSS_DXCH_IP_EXCP_MC_ILLEGAL_ADDRESS_E,CPSS_IP_PROTOCOL_IPV6_E,    CPSS_PACKET_CMD_TRAP_TO_CPU_E,      1,24,3,2},
        {CPSS_DXCH_IP_EXCP_MC_ILLEGAL_ADDRESS_E,CPSS_IP_PROTOCOL_IPV6_E,    CPSS_PACKET_CMD_DROP_HARD_E,        1,24,3,3},
        {CPSS_DXCH_IP_EXCP_MC_ILLEGAL_ADDRESS_E,CPSS_IP_PROTOCOL_IPV6_E,    CPSS_PACKET_CMD_DROP_SOFT_E,        1,24,3,4},
        {CPSS_DXCH_IP_EXCP_MC_ILLEGAL_ADDRESS_E,CPSS_IP_PROTOCOL_IPV6_E,    CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E,1,24,3,5},
        {CPSS_DXCH_IP_EXCP_MC_ILLEGAL_ADDRESS_E,CPSS_IP_PROTOCOL_IPV6_E,    CPSS_PACKET_CMD_BRIDGE_E,           1,24,3,6},
        /* IPv4 UC DIP/DA Mismatch exception command */
        {CPSS_DXCH_IP_EXCP_UC_DIP_DA_MISMATCH_E,CPSS_IP_PROTOCOL_IPV4_E,    CPSS_PACKET_CMD_TRAP_TO_CPU_E,      2,0,3,2},
        {CPSS_DXCH_IP_EXCP_UC_DIP_DA_MISMATCH_E,CPSS_IP_PROTOCOL_IPV4_E,    CPSS_PACKET_CMD_DROP_HARD_E,        2,0,3,3},
        {CPSS_DXCH_IP_EXCP_UC_DIP_DA_MISMATCH_E,CPSS_IP_PROTOCOL_IPV4_E,    CPSS_PACKET_CMD_DROP_SOFT_E,        2,0,3,4},
        /* IPv6 UC DIP/DA Mismatch exception command */
        {CPSS_DXCH_IP_EXCP_UC_DIP_DA_MISMATCH_E,CPSS_IP_PROTOCOL_IPV6_E,    CPSS_PACKET_CMD_TRAP_TO_CPU_E,      3,0,3,2},
        {CPSS_DXCH_IP_EXCP_UC_DIP_DA_MISMATCH_E,CPSS_IP_PROTOCOL_IPV6_E,    CPSS_PACKET_CMD_DROP_HARD_E,        3,0,3,3},
        {CPSS_DXCH_IP_EXCP_UC_DIP_DA_MISMATCH_E,CPSS_IP_PROTOCOL_IPV6_E,    CPSS_PACKET_CMD_DROP_SOFT_E,        3,0,3,4},
        /* IPv4 MC DIP/DA Mismatch exception command */
        {CPSS_DXCH_IP_EXCP_MC_DIP_DA_MISMATCH_E,CPSS_IP_PROTOCOL_IPV4_E,    CPSS_PACKET_CMD_TRAP_TO_CPU_E,      2,3,3,2},
        {CPSS_DXCH_IP_EXCP_MC_DIP_DA_MISMATCH_E,CPSS_IP_PROTOCOL_IPV4_E,    CPSS_PACKET_CMD_DROP_HARD_E,        2,3,3,3},
        {CPSS_DXCH_IP_EXCP_MC_DIP_DA_MISMATCH_E,CPSS_IP_PROTOCOL_IPV4_E,    CPSS_PACKET_CMD_DROP_SOFT_E,        2,3,3,4},
        {CPSS_DXCH_IP_EXCP_MC_DIP_DA_MISMATCH_E,CPSS_IP_PROTOCOL_IPV4_E,    CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E,2,3,3,5},
        {CPSS_DXCH_IP_EXCP_MC_DIP_DA_MISMATCH_E,CPSS_IP_PROTOCOL_IPV4_E,    CPSS_PACKET_CMD_BRIDGE_E ,          2,3,3,6},
        /* IPv6 MC DIP/DA Mismatch exception command */
        {CPSS_DXCH_IP_EXCP_MC_DIP_DA_MISMATCH_E,CPSS_IP_PROTOCOL_IPV6_E,    CPSS_PACKET_CMD_TRAP_TO_CPU_E,      3,3,3,2},
        {CPSS_DXCH_IP_EXCP_MC_DIP_DA_MISMATCH_E,CPSS_IP_PROTOCOL_IPV6_E,    CPSS_PACKET_CMD_DROP_HARD_E,        3,3,3,3},
        {CPSS_DXCH_IP_EXCP_MC_DIP_DA_MISMATCH_E,CPSS_IP_PROTOCOL_IPV6_E,    CPSS_PACKET_CMD_DROP_SOFT_E,        3,3,3,4},
        {CPSS_DXCH_IP_EXCP_MC_DIP_DA_MISMATCH_E,CPSS_IP_PROTOCOL_IPV6_E,    CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E,3,3,3,5},
        {CPSS_DXCH_IP_EXCP_MC_DIP_DA_MISMATCH_E,CPSS_IP_PROTOCOL_IPV6_E,    CPSS_PACKET_CMD_BRIDGE_E ,          3,3,3,6},
        /* IPv4 Unicast all-zero SIP exception command */
        {CPSS_DXCH_IP_EXCP_UC_ALL_ZERO_SIP_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_DROP_SOFT_E,        4,3,1,1},
        {CPSS_DXCH_IP_EXCP_UC_ALL_ZERO_SIP_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_NONE_E,             4,3,1,0},
        /* IPv6 Unicast all-zero SIP exception command */
        {CPSS_DXCH_IP_EXCP_UC_ALL_ZERO_SIP_E,CPSS_IP_PROTOCOL_IPV6_E,       CPSS_PACKET_CMD_DROP_SOFT_E,        4,1,1,1},
        {CPSS_DXCH_IP_EXCP_UC_ALL_ZERO_SIP_E,CPSS_IP_PROTOCOL_IPV6_E,       CPSS_PACKET_CMD_NONE_E,             4,1,1,0},
        /* IPv6 Unicast MTU Exceeded command */
        {CPSS_DXCH_IP_EXCP_UC_MTU_EXCEEDED_E,CPSS_IP_PROTOCOL_IPV6_E,       CPSS_PACKET_CMD_TRAP_TO_CPU_E,      1,3,3,2},
        {CPSS_DXCH_IP_EXCP_UC_MTU_EXCEEDED_E,CPSS_IP_PROTOCOL_IPV6_E,       CPSS_PACKET_CMD_DROP_HARD_E,        1,3,3,3},
        {CPSS_DXCH_IP_EXCP_UC_MTU_EXCEEDED_E,CPSS_IP_PROTOCOL_IPV6_E,       CPSS_PACKET_CMD_DROP_SOFT_E,        1,3,3,4},
        /* IPv6 Multicast MTU Exceeded command */
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_E,CPSS_IP_PROTOCOL_IPV6_E,       CPSS_PACKET_CMD_TRAP_TO_CPU_E,      1,0,3,2},
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_E,CPSS_IP_PROTOCOL_IPV6_E,       CPSS_PACKET_CMD_DROP_HARD_E,        1,0,3,3},
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_E,CPSS_IP_PROTOCOL_IPV6_E,       CPSS_PACKET_CMD_DROP_SOFT_E,        1,0,3,4},
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_E,CPSS_IP_PROTOCOL_IPV6_E,       CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E,1,0,3,5},
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_E,CPSS_IP_PROTOCOL_IPV6_E,       CPSS_PACKET_CMD_BRIDGE_E ,          1,0,3,6},
        /* IPv4 Multicast all-zero SIP exception command */
        {CPSS_DXCH_IP_EXCP_MC_ALL_ZERO_SIP_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_BRIDGE_E,           4,4,1,1},
        {CPSS_DXCH_IP_EXCP_MC_ALL_ZERO_SIP_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_NONE_E,             4,4,1,0},
        /* IPv6 Multicast all-zero SIP exception command */
        {CPSS_DXCH_IP_EXCP_MC_ALL_ZERO_SIP_E,CPSS_IP_PROTOCOL_IPV6_E,       CPSS_PACKET_CMD_BRIDGE_E,           4,2,1,1},
        {CPSS_DXCH_IP_EXCP_MC_ALL_ZERO_SIP_E,CPSS_IP_PROTOCOL_IPV6_E,       CPSS_PACKET_CMD_NONE_E,             4,2,1,0},
        /* IPv4 Unicast Options/Hop-by-Hop Exception command */
        {CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_ROUTE_E,            0,9,3,0},
        {CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E, 0,9,3,1},
        {CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_TRAP_TO_CPU_E,      0,9,3,2},
        {CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_DROP_HARD_E,        0,9,3,3},
        {CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_DROP_SOFT_E,        0,9,3,4},
        /* IPv6 Unicast Options/Hop-by-Hop Exception command */
        {CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV6_E,  CPSS_PACKET_CMD_ROUTE_E,            1,9,3,0},
        {CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV6_E,  CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E, 1,9,3,1},
        {CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV6_E,  CPSS_PACKET_CMD_TRAP_TO_CPU_E,      1,9,3,2},
        {CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV6_E,  CPSS_PACKET_CMD_DROP_HARD_E,        1,9,3,3},
        {CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV6_E,  CPSS_PACKET_CMD_DROP_SOFT_E,        1,9,3,4},
        /* IPv4 Multicast Options/Hop-by-Hop Exception command */
        {CPSS_DXCH_IP_EXCP_MC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_ROUTE_E,            0,6,3,0},
        {CPSS_DXCH_IP_EXCP_MC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E, 0,6,3,1},
        {CPSS_DXCH_IP_EXCP_MC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_TRAP_TO_CPU_E,      0,6,3,2},
        {CPSS_DXCH_IP_EXCP_MC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_DROP_HARD_E,        0,6,3,3},
        {CPSS_DXCH_IP_EXCP_MC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_DROP_SOFT_E,        0,6,3,4},
        {CPSS_DXCH_IP_EXCP_MC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E,0,6,3,5},
        {CPSS_DXCH_IP_EXCP_MC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_BRIDGE_E ,          0,6,3,6},
        /* IPv6 Multicast Options/Hop-by-Hop Exception command */
        {CPSS_DXCH_IP_EXCP_MC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV6_E,  CPSS_PACKET_CMD_ROUTE_E,            1,6,3,0},
        {CPSS_DXCH_IP_EXCP_MC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV6_E,  CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E, 1,6,3,1},
        {CPSS_DXCH_IP_EXCP_MC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV6_E,  CPSS_PACKET_CMD_TRAP_TO_CPU_E,      1,6,3,2},
        {CPSS_DXCH_IP_EXCP_MC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV6_E,  CPSS_PACKET_CMD_DROP_HARD_E,        1,6,3,3},
        {CPSS_DXCH_IP_EXCP_MC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV6_E,  CPSS_PACKET_CMD_DROP_SOFT_E,        1,6,3,4},
        {CPSS_DXCH_IP_EXCP_MC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV6_E,  CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E,1,6,3,5},
        {CPSS_DXCH_IP_EXCP_MC_OPTION_HOP_BY_HOP_E,CPSS_IP_PROTOCOL_IPV6_E,  CPSS_PACKET_CMD_BRIDGE_E,           1,6,3,6},
        /* IPv6 Unicast non-Hop-by-hop extension exception command */
        {CPSS_DXCH_IP_EXCP_UC_NON_HOP_BY_HOP_EXT_E,CPSS_IP_PROTOCOL_IPV6_E, CPSS_PACKET_CMD_ROUTE_E,            3,20,3,0},
        {CPSS_DXCH_IP_EXCP_UC_NON_HOP_BY_HOP_EXT_E,CPSS_IP_PROTOCOL_IPV6_E, CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E, 3,20,3,1},
        {CPSS_DXCH_IP_EXCP_UC_NON_HOP_BY_HOP_EXT_E,CPSS_IP_PROTOCOL_IPV6_E, CPSS_PACKET_CMD_TRAP_TO_CPU_E,      3,20,3,2},
        {CPSS_DXCH_IP_EXCP_UC_NON_HOP_BY_HOP_EXT_E,CPSS_IP_PROTOCOL_IPV6_E, CPSS_PACKET_CMD_DROP_HARD_E,        3,20,3,3},
        {CPSS_DXCH_IP_EXCP_UC_NON_HOP_BY_HOP_EXT_E,CPSS_IP_PROTOCOL_IPV6_E, CPSS_PACKET_CMD_DROP_SOFT_E,        3,20,3,4},
        /* IPv6 Multicast non-Hop-by-hop extension exception command */
        {CPSS_DXCH_IP_EXCP_MC_NON_HOP_BY_HOP_EXT_E,CPSS_IP_PROTOCOL_IPV6_E, CPSS_PACKET_CMD_ROUTE_E,            3,17,3,0},
        {CPSS_DXCH_IP_EXCP_MC_NON_HOP_BY_HOP_EXT_E,CPSS_IP_PROTOCOL_IPV6_E, CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E, 3,17,3,1},
        {CPSS_DXCH_IP_EXCP_MC_NON_HOP_BY_HOP_EXT_E,CPSS_IP_PROTOCOL_IPV6_E, CPSS_PACKET_CMD_TRAP_TO_CPU_E,      3,17,3,2},
        {CPSS_DXCH_IP_EXCP_MC_NON_HOP_BY_HOP_EXT_E,CPSS_IP_PROTOCOL_IPV6_E, CPSS_PACKET_CMD_DROP_HARD_E,        3,17,3,3},
        {CPSS_DXCH_IP_EXCP_MC_NON_HOP_BY_HOP_EXT_E,CPSS_IP_PROTOCOL_IPV6_E, CPSS_PACKET_CMD_DROP_SOFT_E,        3,17,3,4},
        {CPSS_DXCH_IP_EXCP_MC_NON_HOP_BY_HOP_EXT_E,CPSS_IP_PROTOCOL_IPV6_E, CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E,3,17,3,5},
        {CPSS_DXCH_IP_EXCP_MC_NON_HOP_BY_HOP_EXT_E,CPSS_IP_PROTOCOL_IPV6_E, CPSS_PACKET_CMD_BRIDGE_E,           3,17,3,6},
        /* IPv4 Unicast TLL Exceeded command */
        {CPSS_DXCH_IP_EXCP_UC_TTL_EXCEED_E,CPSS_IP_PROTOCOL_IPV4_E,         CPSS_PACKET_CMD_TRAP_TO_CPU_E,      0,21,3,2},
        {CPSS_DXCH_IP_EXCP_UC_TTL_EXCEED_E,CPSS_IP_PROTOCOL_IPV4_E,         CPSS_PACKET_CMD_DROP_HARD_E,        0,21,3,3},
        {CPSS_DXCH_IP_EXCP_UC_TTL_EXCEED_E,CPSS_IP_PROTOCOL_IPV4_E,         CPSS_PACKET_CMD_DROP_SOFT_E,        0,21,3,4},
        /* IPv6 Unicast Hop Limit (TTL) Exceeded command */
        {CPSS_DXCH_IP_EXCP_UC_TTL_EXCEED_E,CPSS_IP_PROTOCOL_IPV6_E,         CPSS_PACKET_CMD_TRAP_TO_CPU_E,      1,21,3,2},
        {CPSS_DXCH_IP_EXCP_UC_TTL_EXCEED_E,CPSS_IP_PROTOCOL_IPV6_E,         CPSS_PACKET_CMD_DROP_HARD_E,        1,21,3,3},
        {CPSS_DXCH_IP_EXCP_UC_TTL_EXCEED_E,CPSS_IP_PROTOCOL_IPV6_E,         CPSS_PACKET_CMD_DROP_SOFT_E,        1,21,3,4},
        /* IPv4 Unicast RPF Fail command */
        {CPSS_DXCH_IP_EXCP_UC_RPF_FAIL_E,CPSS_IP_PROTOCOL_IPV4_E,           CPSS_PACKET_CMD_TRAP_TO_CPU_E,      2,9,3,2},
        {CPSS_DXCH_IP_EXCP_UC_RPF_FAIL_E,CPSS_IP_PROTOCOL_IPV4_E,           CPSS_PACKET_CMD_DROP_HARD_E,        2,9,3,3},
        {CPSS_DXCH_IP_EXCP_UC_RPF_FAIL_E,CPSS_IP_PROTOCOL_IPV4_E,           CPSS_PACKET_CMD_DROP_SOFT_E,        2,9,3,4},
        /* IPv6 Unicast RPF Fail command */
        {CPSS_DXCH_IP_EXCP_UC_RPF_FAIL_E,CPSS_IP_PROTOCOL_IPV6_E,           CPSS_PACKET_CMD_TRAP_TO_CPU_E,      3,9,3,2},
        {CPSS_DXCH_IP_EXCP_UC_RPF_FAIL_E,CPSS_IP_PROTOCOL_IPV6_E,           CPSS_PACKET_CMD_DROP_HARD_E,        3,9,3,3},
        {CPSS_DXCH_IP_EXCP_UC_RPF_FAIL_E,CPSS_IP_PROTOCOL_IPV6_E,           CPSS_PACKET_CMD_DROP_SOFT_E,        3,9,3,4},
        /* IPv4 SIP/SA Fail command */
        {CPSS_DXCH_IP_EXCP_UC_SIP_SA_FAIL_E,CPSS_IP_PROTOCOL_IPV4_E,        CPSS_PACKET_CMD_TRAP_TO_CPU_E,      2,13,3,2},
        {CPSS_DXCH_IP_EXCP_UC_SIP_SA_FAIL_E,CPSS_IP_PROTOCOL_IPV4_E,        CPSS_PACKET_CMD_DROP_HARD_E,        2,13,3,3},
        {CPSS_DXCH_IP_EXCP_UC_SIP_SA_FAIL_E,CPSS_IP_PROTOCOL_IPV4_E,        CPSS_PACKET_CMD_DROP_SOFT_E,        2,13,3,4},
        /* IPv6 SIP/SA Fail command */
        {CPSS_DXCH_IP_EXCP_UC_SIP_SA_FAIL_E,CPSS_IP_PROTOCOL_IPV6_E,        CPSS_PACKET_CMD_TRAP_TO_CPU_E,      3,13,3,2},
        {CPSS_DXCH_IP_EXCP_UC_SIP_SA_FAIL_E,CPSS_IP_PROTOCOL_IPV6_E,        CPSS_PACKET_CMD_DROP_HARD_E,        3,13,3,3},
        {CPSS_DXCH_IP_EXCP_UC_SIP_SA_FAIL_E,CPSS_IP_PROTOCOL_IPV6_E,        CPSS_PACKET_CMD_DROP_SOFT_E,        3,13,3,4}
    };

    /* exceptions entries that apply only to DxCh2 devices */
    static CPSS_DXCH_IP_EXCPTION_CMD_REG_INFO_STC dxCh2OnlyExceptions[] =
    {
        /* IPv4 Unicast MTU Exceeded command */
        {CPSS_DXCH_IP_EXCP_UC_MTU_EXCEEDED_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_TRAP_TO_CPU_E,      0,3,3,2},
        {CPSS_DXCH_IP_EXCP_UC_MTU_EXCEEDED_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_DROP_HARD_E,        0,3,3,3},
        {CPSS_DXCH_IP_EXCP_UC_MTU_EXCEEDED_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_DROP_SOFT_E,        0,3,3,4},
        /* IPv4 Multicast MTU Exceeded command */
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_TRAP_TO_CPU_E,      0,0,3,2},
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_DROP_HARD_E,        0,0,3,3},
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_DROP_SOFT_E,        0,0,3,4},
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E,0,0,3,5},
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_BRIDGE_E ,          0,0,3,6},
    };
    /* exceptions entries that apply to DxCh3 and above devices */
    static CPSS_DXCH_IP_EXCPTION_CMD_REG_INFO_STC dxCh3AndAboveExceptions[] =
    {
        /* IPv4 Unicast MTU Exceeded for non "don't fragment" (DF) packets command */
        {CPSS_DXCH_IP_EXCP_UC_MTU_EXCEEDED_FOR_NON_DF_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_TRAP_TO_CPU_E,      0,3,3,2},
        {CPSS_DXCH_IP_EXCP_UC_MTU_EXCEEDED_FOR_NON_DF_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_DROP_HARD_E,        0,3,3,3},
        {CPSS_DXCH_IP_EXCP_UC_MTU_EXCEEDED_FOR_NON_DF_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_DROP_SOFT_E,        0,3,3,4},
        /* IPv4 Multicast MTU Exceeded for non "don't fragment" (DF) packets command */
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_FOR_NON_DF_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_TRAP_TO_CPU_E,      0,0,3,2},
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_FOR_NON_DF_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_DROP_HARD_E,        0,0,3,3},
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_FOR_NON_DF_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_DROP_SOFT_E,        0,0,3,4},
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_FOR_NON_DF_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E,0,0,3,5},
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_FOR_NON_DF_E,CPSS_IP_PROTOCOL_IPV4_E,  CPSS_PACKET_CMD_BRIDGE_E ,          0,0,3,6},
        /* IPv4 Unicast MTU Exceeded for "don't fragment" (DF) packets command */
        {CPSS_DXCH_IP_EXCP_UC_MTU_EXCEEDED_FOR_DF_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_TRAP_TO_CPU_E,      2,19,3,2},
        {CPSS_DXCH_IP_EXCP_UC_MTU_EXCEEDED_FOR_DF_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_DROP_HARD_E,        2,19,3,3},
        {CPSS_DXCH_IP_EXCP_UC_MTU_EXCEEDED_FOR_DF_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_DROP_SOFT_E,        2,19,3,4},
        /* IPv4 Multicast MTU Exceeded for "don't fragment" (DF) packets command */
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_FOR_DF_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_TRAP_TO_CPU_E,      2,16,3,2},
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_FOR_DF_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_DROP_HARD_E,        2,16,3,3},
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_FOR_DF_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_DROP_SOFT_E,        2,16,3,4},
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_FOR_DF_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E,2,16,3,5},
        {CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_FOR_DF_E,CPSS_IP_PROTOCOL_IPV4_E,       CPSS_PACKET_CMD_BRIDGE_E ,          2,16,3,6},
    };

    /* array to hold the databases to scan */
    CPSS_DXCH_IP_EXCPTION_CMD_REG_INFO_STC *database[NUMBER_OF_EXCEPTION_DATABASES_CNS];
    /* size of the database  */
    GT_U32 databaseSize[NUMBER_OF_EXCEPTION_DATABASES_CNS];

    /* check parameters; note that the exception types */
    /* are checked while scanning the databases        */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    switch (protocolStack)
    {
        case CPSS_IP_PROTOCOL_IPV4_E:
        case CPSS_IP_PROTOCOL_IPV6_E:
        case CPSS_IP_PROTOCOL_IPV4V6_E:
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* initialize databases to scan according to device family type */
    for (i = 0 ; i < NUMBER_OF_EXCEPTION_DATABASES_CNS ; i++)
        database[i] = NULL;

    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH_E:
        database[0] = dxChOnlyExceptions;
        databaseSize[0] = sizeof(dxChOnlyExceptions) / sizeof(CPSS_DXCH_IP_EXCPTION_CMD_REG_INFO_STC);
        break;
    case CPSS_PP_FAMILY_CHEETAH2_E:
        database[0] = dxCh2AndAboveExceptions;
        databaseSize[0] = sizeof(dxCh2AndAboveExceptions) / sizeof(CPSS_DXCH_IP_EXCPTION_CMD_REG_INFO_STC);
        database[1] = dxCh2OnlyExceptions;
        databaseSize[1] = sizeof(dxCh2OnlyExceptions) / sizeof(CPSS_DXCH_IP_EXCPTION_CMD_REG_INFO_STC);
        break;
    case CPSS_PP_FAMILY_CHEETAH3_E:
        /* explicity fallthrough */
    case CPSS_PP_FAMILY_DXCH_LION_E:
        database[0] = dxCh2AndAboveExceptions;
        databaseSize[0] = sizeof(dxCh2AndAboveExceptions) / sizeof(CPSS_DXCH_IP_EXCPTION_CMD_REG_INFO_STC);
        database[1] = dxCh3AndAboveExceptions;
        databaseSize[1] = sizeof(dxCh3AndAboveExceptions) / sizeof(CPSS_DXCH_IP_EXCPTION_CMD_REG_INFO_STC);
        break;
    case CPSS_PP_FAMILY_DXCH_XCAT2_E:
        database[0] = dxChPbrOnlyExceptions;
        databaseSize[0] = sizeof(dxChPbrOnlyExceptions) / sizeof(CPSS_DXCH_IP_EXCPTION_CMD_REG_INFO_STC);
        break;
    case CPSS_PP_FAMILY_DXCH_XCAT_E:
        if (PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.ip.routingMode == CPSS_DXCH_POLICY_BASED_ROUTING_ONLY_E)
        {
            database[0] = dxChPbrOnlyExceptions;
            databaseSize[0] = sizeof(dxChPbrOnlyExceptions) / sizeof(CPSS_DXCH_IP_EXCPTION_CMD_REG_INFO_STC);
        }
        else
        {
            database[0] = dxCh2AndAboveExceptions;
            databaseSize[0] = sizeof(dxCh2AndAboveExceptions) / sizeof(CPSS_DXCH_IP_EXCPTION_CMD_REG_INFO_STC);
            database[1] = dxCh3AndAboveExceptions;
            databaseSize[1] = sizeof(dxCh3AndAboveExceptions) / sizeof(CPSS_DXCH_IP_EXCPTION_CMD_REG_INFO_STC);
        }
        break;
    default:
        return GT_NOT_SUPPORTED;
    }

    /* before start search for matching lines, mark that ipv4 matching */
    /* exception and ipv6 matching exception were not found yet        */
    ipv4EntryFound = GT_FALSE;
    ipv6EntryFound = GT_FALSE;

    /* search the databases for matching lines, remember 7 elements in each row*/
    for (i = 0 ; i < NUMBER_OF_EXCEPTION_DATABASES_CNS ; i++)
    {
        /* skip empty databases */
        if (database[i] == NULL)
            continue;


        /* now scan the current databse entries */
        for (j = 0 ; j < databaseSize[i]; j++)
        {
            /* check that excpType and cmd match */
            if ((exceptionType != database[i][j].excptType) || (command != database[i][j].cmd))
                continue;

            /* check if protocolStack doesn't match */
            if ((protocolStack == CPSS_IP_PROTOCOL_IPV4_E) &&
                (database[i][j].protocolStack == CPSS_IP_PROTOCOL_IPV6_E))
                continue;
            if ((protocolStack == CPSS_IP_PROTOCOL_IPV6_E) &&
                (database[i][j].protocolStack == CPSS_IP_PROTOCOL_IPV4_E))
                continue;

            /* a match was found, find register address from register index */
            switch (database[i][j].registerIndex)
            {
            case 0:
               regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv4Regs.ctrlReg0;
                break;
            case 1:
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv6Regs.ctrlReg0;
                break;
            case 2:
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv4Regs.ctrlReg1;
                break;
            case 3:
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv6Regs.ctrlReg1;
                break;
            case 4:
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerAdditionalCtrlReg;
                break;
            case 5:
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.ctrlReg0;
                break;
            case 6:
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.ucRoutingEngineConfigurationReg;
                break;
            default:
                return GT_FAIL;
            }

            /* write value to the register */
            rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,database[i][j].offset,
                                           database[i][j].length,database[i][j].value);
            if (rc != GT_OK)
                return rc;

            /* set index of protocol that was updated */
            if (database[i][j].protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
                ipv4EntryFound = GT_TRUE;
            else /* meaning database[i][j].protocolStack == GT_IP_PROTOCOL_IPV6 */
                ipv6EntryFound = GT_TRUE;
        }
    }

    /* if no matching line was found, return with error */
    if ((ipv4EntryFound == GT_FALSE) && (ipv6EntryFound == GT_FALSE))
        return GT_BAD_PARAM;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChIpUcRouteAgingModeSet
*
* DESCRIPTION:
*      Sets the global route aging modes.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum        - the device number
*       refreshEnable - Enables the global routing activity refresh mechanism
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpUcRouteAgingModeSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  refreshEnable
)
{

    GT_U32    regAddr;              /* register address                   */
    GT_STATUS rc;                   /* return code                        */
    GT_U32    value;                /* value to write                     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    /* Get address of register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerGlobalReg;


    /* update refreshEnable value */
    value = (refreshEnable == GT_TRUE) ? 1:0;
    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,0,1,value);

    return rc;
}

/*******************************************************************************
* cpssDxChIpRouterSourceIdSet
*
* DESCRIPTION:
*      set the router source id assignmnet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - the device number
*       ucMcSet       - weather to set it for unicast packets or multicast.
*       sourceId      - the assigned source id (0..31).
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterSourceIdSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet,
    IN  GT_U32                          sourceId
)
{
    GT_U32    regAddr;     /* register address  */
    GT_STATUS rc;          /* return code       */
    GT_U32    offset;      /* value to write    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(GT_FALSE == CHECK_BITS_DATA_RANGE_MAC (sourceId,5))
        return GT_OUT_OF_RANGE;

    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH_E:
        if (ucMcSet != CPSS_IP_UNICAST_E)
            return GT_BAD_PARAM;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.ctrlReg0;
        offset = 0;
        break;
    case CPSS_PP_FAMILY_DXCH_XCAT2_E:
        if (ucMcSet != CPSS_IP_UNICAST_E)
            return GT_NOT_APPLICABLE_DEVICE;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.ucRoutingEngineConfigurationReg;
        offset = 0;
        break;
    default:
        switch (ucMcSet)
        {
        case CPSS_IP_UNICAST_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerGlobalReg;
            offset = 1;
            break;

        case CPSS_IP_MULTICAST_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.mllGlobalReg;
            offset = 0;
            break;

        default:
            return GT_BAD_PARAM;
        }
        break;
    }

    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,offset,5,sourceId);
    return rc;
}

/*******************************************************************************
* cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet
*
* DESCRIPTION:
*      Sets the multi-target TC queue assigned to multi-target Control
*      packets and to RPF Fail packets where the RPF Fail Command is assigned
*      from the MLL entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       ctrlMultiTargetTCQueue    - The multi-target TC queue for control
*                                   traffic. There are 4 possible queues (0..3)
*       failRpfMultiTargetTCQueue - the multi-target TC queue for fail rpf
*                                   traffic. There are 4 possible queues (0..3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       A packet is considered as a multi-target control if it is a FROM_CPU DSA
*       Tagged with DSA<Use_Vidx> = 1 or a multi-target packet that is also to
*       be mirrored to the CPU
*
*******************************************************************************/
GT_STATUS cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  ctrlMultiTargetTCQueue,
    IN  GT_U32  failRpfMultiTargetTCQueue
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_STATUS rc;                   /* return code                        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.mtTcQueuesGlobalReg;

    /* update ctrlMultiTargetTCQueue value */
    if(ctrlMultiTargetTCQueue > 3)
        return GT_BAD_PARAM;

    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr, 16,2,ctrlMultiTargetTCQueue);
    if(GT_OK != rc)
        return rc;

    /* update failRpfMtTcQueue value */

    if(failRpfMultiTargetTCQueue > 3)
        return GT_BAD_PARAM;

    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,18,2,failRpfMultiTargetTCQueue);

    return rc;
}

/*******************************************************************************
* cpssDxChIpQosProfileToMultiTargetTCQueueMapSet
*
* DESCRIPTION:
*     Sets the Qos Profile to multi-target TC queue mapping.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum             - the device number
*       qosProfile         - QOS Profile index
*       multiTargetTCQueue - the multi-target TC queue. There are 4 possible
*                            queues.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpQosProfileToMultiTargetTCQueueMapSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  qosProfile,
    IN  GT_U32  multiTargetTCQueue
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_STATUS rc;                   /* return code                        */
    GT_U32    offset;               /* value to write                     */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_QOS2MT_ENTRY_CHECK_MAC(qosProfile);

    /* get reg_addr and offset within the address from qosProfile value */
    /* note that each QOS profile need 2 bits for Queue index so there  */
    /* are 16 entires in each 32bit register                            */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.qoSProfile2MTTCQueuesMapTable[(qosProfile/16)];

    offset = (qosProfile % 16) * 2;

    /* write mtTcQueue value */
    if( multiTargetTCQueue >3)
        return GT_BAD_PARAM;

    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,offset,2,multiTargetTCQueue);

    return rc;
}

/*******************************************************************************
* cpssDxChIpMultiTargetQueueFullDropCntGet
*
* DESCRIPTION:
*      Get the multi target queue full drop packet counter.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum      - the device number
*
* OUTPUTS:
*       dropPktsPtr - the number of counted dropped packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpMultiTargetQueueFullDropCntGet
(
    IN   GT_U8    devNum,
    OUT  GT_U32   *dropPktsPtr
)
{
    return cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                             dropPktsPtr);
}

/*******************************************************************************
* cpssDxChIpMultiTargetQueueFullDropCntSet
*
* DESCRIPTION:
*      set the multi target queue full drop packet counter.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum      - the device number
*       dropPkts    - the counter value to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpMultiTargetQueueFullDropCntSet
(
    IN GT_U8     devNum,
    IN GT_U32    dropPkts
)
{
    return cpssDxChIpPortGroupMultiTargetQueueFullDropCntSet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                             dropPkts);
}

/*******************************************************************************
* cpssDxChIpMultiTargetTCQueueSchedModeSet
*
* DESCRIPTION:
*      sets the multi-target TC queue scheduling mode.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum             - the device number
*       multiTargetTcQueue - the multi-target TC queue. There are 4 possible
*                            queues.
*       schedulingMode     - the scheduling mode.
*       queueWeight        - the queue weight for SDWRR scheduler (0..255)
*                           (relvant only if schedMode =
*                            CPSS_DXCH_IP_MT_TC_QUEUE_SDWRR_SCHED_MODE_E).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpMultiTargetTCQueueSchedModeSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  multiTargetTcQueue,
    IN  CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT schedulingMode,
    IN  GT_U32                                  queueWeight
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_STATUS rc;                   /* return code                        */
    GT_U32     value,offset;      /* value to write */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.mtTcQueuesWeightReg;

    if(multiTargetTcQueue > 3 )
        return GT_BAD_PARAM;

    /*if schedMode = CPSS_DXCH_IP_MT_TC_QUEUE_SDWRR_SCHED_MODE_E,
            write weight value */
    if (schedulingMode == CPSS_DXCH_IP_MT_TC_QUEUE_SDWRR_SCHED_MODE_E)
    {
        /* find offset of weight field according to mtTcQueue */
        offset = multiTargetTcQueue *8;

        if(GT_FALSE == CHECK_BITS_DATA_RANGE_MAC(queueWeight,8))
            return GT_OUT_OF_RANGE;
        /* write weight value */
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,
                             offset,8,queueWeight);
        if (rc != GT_OK)
            return rc;
    }

    /* find offset of schedulingMode field according to multiTargetTcQueue */
    offset = multiTargetTcQueue *1;

    /* find enable/disable value to write according to schedulingMode */
    switch (schedulingMode)
    {
        case CPSS_DXCH_IP_MT_TC_QUEUE_SDWRR_SCHED_MODE_E:
            value = 0;
            break;
        case CPSS_DXCH_IP_MT_TC_QUEUE_SP_SCHED_MODE_E:
            value = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }
    /* write schedulingMode value */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.mtTcQueuesSpEnableReg;
    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,offset,1,value);

    return rc;
}


/*******************************************************************************
* cpssDxChIpBridgeServiceEnable
*
* DESCRIPTION:
*      enable/disable a router bridge service.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum            - the device number
*       bridgeService     - the router bridge service
*       enableDisableMode - Enable/Disable mode of this function (weather
*                           to enable/disable for ipv4/ipv6/arp)
*       enableService     - weather to enable the service for the above more.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpBridgeServiceEnable
(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_IP_BRG_SERVICE_ENT                        bridgeService,
    IN  CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT    enableDisableMode,
    IN  GT_BOOL                                             enableService
)
{
    GT_U32      value;      /* value to write */
    GT_STATUS   rc;         /* function return code */

    GT_U32    regAddr;              /* register address                   */
    GT_U32    offset = 0;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerGlobalReg;

    value = (enableService == GT_TRUE) ? 1:0;

    /* update enable values according to bridgeService */
    switch (bridgeService)
    {
        case CPSS_DXCH_IP_HEADER_CHECK_BRG_SERVICE_E:
            switch(enableDisableMode)
            {
                case CPSS_DXCH_IP_BRG_SERVICE_IPV4_ENABLE_DISABLE_E:
                    /* update ipv4Enable value */
                    offset = 14;
                    break;

                case CPSS_DXCH_IP_BRG_SERVICE_IPV6_ENABLE_DISABLE_E:
                    /* update ipv6Enable value */
                    offset = 15;
                    break;

                case CPSS_DXCH_IP_BRG_SERVICE_ARP_ENABLE_DISABLE_E: /* Not Valid value. */

                default:
                    return GT_BAD_PARAM;
            }
        break;

        case CPSS_DXCH_IP_UC_RPF_CHECK_BRG_SERVICE_E:
            switch(enableDisableMode)
            {
                case CPSS_DXCH_IP_BRG_SERVICE_IPV4_ENABLE_DISABLE_E:
                    /* update ipv4Enable value */
                    offset = 8;
                    break;

                case CPSS_DXCH_IP_BRG_SERVICE_IPV6_ENABLE_DISABLE_E:
                    /* update ipv6Enable value */
                    offset = 10;
                    break;

                case CPSS_DXCH_IP_BRG_SERVICE_ARP_ENABLE_DISABLE_E:
                    /* update arpEnable value */
                    offset = 9;
                    break;
                default:
                    return GT_BAD_PARAM;
            }
        break;

        case CPSS_DXCH_IP_SIP_SA_CHECK_BRG_SERVICE_E:
            switch(enableDisableMode)
            {
                case CPSS_DXCH_IP_BRG_SERVICE_IPV4_ENABLE_DISABLE_E:
                    /* update ipv4Enable value */
                    offset = 11;
                    break;
                case CPSS_DXCH_IP_BRG_SERVICE_IPV6_ENABLE_DISABLE_E:
                    /* update ipv6Enable value */
                    offset = 13;
                    break;
                case CPSS_DXCH_IP_BRG_SERVICE_ARP_ENABLE_DISABLE_E:
                    /* update arpEnable value */
                    offset = 12;
                    break;
                default:
                    return GT_BAD_PARAM;
            }
        break;

    case CPSS_DXCH_IP_SIP_FILTER_BRG_SERVICE_E:
        switch(enableDisableMode)
        {
            case CPSS_DXCH_IP_BRG_SERVICE_IPV4_ENABLE_DISABLE_E:
                /* update ipv4Enable value */
                offset = 16;
                break;

            case CPSS_DXCH_IP_BRG_SERVICE_IPV6_ENABLE_DISABLE_E:
                /* update ipv6Enable value */
                offset = 18;
                break;

            case CPSS_DXCH_IP_BRG_SERVICE_ARP_ENABLE_DISABLE_E:
                /* update arpEnable value */
                offset = 17;
                break;

            default:
                return GT_BAD_PARAM;
        }

        break;

    default:
        return GT_BAD_PARAM;
    }

    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,offset,1,value);
    return rc;
}

/*******************************************************************************
* cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntSet
*
* DESCRIPTION:
*      Set exception counter for Special Services for Bridged Traffic.
*      it counts the number of Bridged packets failing any of the following checks:
*        - SIP Filter check for bridged IPv4/6 packets
*        - IP Header Check for bridged IPv4/6 packets
*        - Unicast RPF check for bridged IPv4/6 and ARP packets
*        - Unicast SIP/SA check for bridged IPv4/6 and ARP packets
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       portGroupsBmp             - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       routerBridgedExceptionPkts - the counter value to set
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad  devNum
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntSet
(
    IN GT_U8                devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN GT_U32               routerBridgedExceptionPkts
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_STATUS rc;                   /* return code                        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerBridgedPktExcptCnt;

    /* set the counter */
    rc = prvCpssDxChPortGroupsBmpCounterSet(devNum, portGroupsBmp, regAddr,
                                             0, 32, routerBridgedExceptionPkts);

    return rc;
}

/*******************************************************************************
* cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet
*
* DESCRIPTION:
*      Get exception counter for Special Services for Bridged Traffic.
*      it counts the number of Bridged packets failing any of the following checks:
*        - SIP Filter check for bridged IPv4/6 packets
*        - IP Header Check for bridged IPv4/6 packets
*        - Unicast RPF check for bridged IPv4/6 and ARP packets
*        - Unicast SIP/SA check for bridged IPv4/6 and ARP packets
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum         - the device number
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       routerBridgedExceptionPktsPtr - the number of counted router bridged
*                                       exception  packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad  devNum
*       GT_BAD_PTR               - on illegal pointer value
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *routerBridgedExceptionPktsPtr
)
{
    GT_STATUS rc;                   /* return code                        */
    GT_U32    regAddr;              /* register address                   */

    CPSS_NULL_PTR_CHECK_MAC(routerBridgedExceptionPktsPtr);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerBridgedPktExcptCnt;

    rc = prvCpssDxChPortGroupsBmpCounterSummary(devNum,
                                                portGroupsBmp,
                                                regAddr,
                                                0,
                                                32,
                                                routerBridgedExceptionPktsPtr,
                                                NULL);
    return rc;
}

/*******************************************************************************
* cpssDxChIpRouterBridgedPacketsExceptionCntGet
*
* DESCRIPTION:
*      Get exception counter for Special Services for Bridged Traffic.
*      it counts the number of Bridged packets failing any of the following checks:
*        - SIP Filter check for bridged IPv4/6 packets
*        - IP Header Check for bridged IPv4/6 packets
*        - Unicast RPF check for bridged IPv4/6 and ARP packets
*        - Unicast SIP/SA check for bridged IPv4/6 and ARP packets
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum      - the device number
*
* OUTPUTS:
*       routerBridgedExceptionPktsPtr - the number of counted router bridged
*                                       exception  packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad  devNum
*       GT_BAD_PTR               - on illegal pointer value
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterBridgedPacketsExceptionCntGet
(
    IN   GT_U8    devNum,
    OUT  GT_U32   *routerBridgedExceptionPktsPtr
)
{
    return cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet(devNum,
                                                                  CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                                  routerBridgedExceptionPktsPtr);
}


/*******************************************************************************
* cpssDxChIpRouterBridgedPacketsExceptionCntSet
*
* DESCRIPTION:
*      Set exception counter for Special Services for Bridged Traffic.
*      it counts the number of Bridged packets failing any of the following checks:
*        - SIP Filter check for bridged IPv4/6 packets
*        - IP Header Check for bridged IPv4/6 packets
*        - Unicast RPF check for bridged IPv4/6 and ARP packets
*        - Unicast SIP/SA check for bridged IPv4/6 and ARP packets
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                        - the device number
*       routerBridgedExceptionPkts    - the counter value to set
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad  devNum
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterBridgedPacketsExceptionCntSet
(
    IN   GT_U8    devNum,
    IN  GT_U32    routerBridgedExceptionPkts
)
{
    return cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntSet(devNum,
                                                                  CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                                  routerBridgedExceptionPkts);
}

/*******************************************************************************
* cpssDxChIpMllBridgeEnable
*
* DESCRIPTION:
*      enable/disable MLL based bridging.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       mllBridgeEnable - enable /disable MLL based bridging.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpMllBridgeEnable
(
    IN   GT_U8      devNum,
    IN   GT_BOOL    mllBridgeEnable
)
{

    GT_U32    regAddr;          /* register address   */
    GT_U32    data;             /* value to write     */
    GT_STATUS rc;               /* return code        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    /* Get address of MLL Global Control Register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.mllGlobalReg;

    data = (GT_TRUE == mllBridgeEnable)? 1:0;

    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,13,1,data);
    return rc;
}

/*******************************************************************************
* cpssDxChIpMultiTargetRateShaperSet
*
* DESCRIPTION:
*      set the Multi target Rate shaper params.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum     - the device number
*       multiTargetRateShaperEnable - weather to enable (disable) the Multi
*                                     target Rate shaper
*       windowSize - if enabled then this is the shaper window size (0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpMultiTargetRateShaperSet
(
    IN   GT_U8      devNum,
    IN   GT_BOOL    multiTargetRateShaperEnable,
    IN   GT_U32     windowSize
)
{
    return cpssDxChIpPortGroupMultiTargetRateShaperSet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               multiTargetRateShaperEnable,
                                               windowSize);
}

/*******************************************************************************
* cpssDxChIpMultiTargetUcSchedModeSet
*
* DESCRIPTION:
*      set the Multi target/unicast sheduler mode.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum    - the device number
*       ucSPEnable- weather the Unicast uses SP , if GT_FALSE then it means
*                   both the Unicast and multi target uses DSWRR scheduling
*       ucWeight  - if DSWRR scheduler is used (ucSPEnable==GT_FALSE) then this
*                   is the unicast weight (0..255).
*       mcWeight  - if DSWRR scheduler is used (ucSPEnable==GT_FALSE) then this
*                   is the multi target weight (0..255).
*       schedMtu  - The MTU used by the scheduler
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpMultiTargetUcSchedModeSet
(
    IN   GT_U8                              devNum,
    IN   GT_BOOL                            ucSPEnable,
    IN   GT_U32                             ucWeight,
    IN   GT_U32                             mcWeight,
    IN   CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT   schedMtu
)
{
    GT_U32    regAddr;              /* register address  */
    GT_U32    data;                 /* value to write    */
    GT_STATUS rc;                   /* return code       */
    GT_U32    schedMtuData;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    /* Get address of Multi Target Vs. Unicast SDWRR and Strict Priority Scheduler register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.mtUcSchedulerModeReg;

    switch (schedMtu)
    {
        case CPSS_DXCH_IP_MT_UC_SCHED_MTU_2K_E:
            schedMtuData = 0;
            break;
        case CPSS_DXCH_IP_MT_UC_SCHED_MTU_8K_E:
            schedMtuData = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    data = (GT_TRUE == ucSPEnable)? 1:0;

    /* Enable/disable  Strict Priority for Unicast packets*/
    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,16,1,data);
    if (GT_OK != rc)
        return rc;

    if (GT_FALSE == ucSPEnable)
    {
        if ( GT_FALSE == CHECK_BITS_DATA_RANGE_MAC(ucWeight,8))
            return GT_OUT_OF_RANGE;

        if ( GT_FALSE == CHECK_BITS_DATA_RANGE_MAC(mcWeight,8))
            return GT_OUT_OF_RANGE;

        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,8,8,ucWeight);
        if (GT_OK != rc)
            return rc;

        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,0,8,mcWeight);
        if (GT_OK != rc)
            return rc;
    }

    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,17,1,schedMtuData);

    return rc;
}

/*******************************************************************************
* cpssDxChIpArpBcModeSet
*
* DESCRIPTION:
*       set a arp broadcast mode.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum       - the device number
*       arpBcMode    - the arp broadcast command. Possible Commands:
*                      CPSS_PACKET_CMD_NONE_E,CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                      CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
* GalTis:
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpArpBcModeSet
(
    IN GT_U8               devNum,
    IN CPSS_PACKET_CMD_ENT arpBcMode
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_U32    data;
    GT_STATUS rc;                   /* return code                        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    /* Get address of IPv4 Control Register0 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv4Regs.ctrlReg0;

    switch (arpBcMode)
    {
        case CPSS_PACKET_CMD_NONE_E:
            data = 0;
            break;
        case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
            data = 1;
            break;
        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            data = 2;
            break;
        default:
            return GT_BAD_PARAM;
    }

    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,30,2,data);
    return rc;
}



/*******************************************************************************
* cpssDxChIpPortRoutingEnable
*
* DESCRIPTION:
*       Enable multicast/unicast IPv4/v6 routing on a port.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum        - the device number
*       portNum       - the port to enable on
*       ucMcEnable    - routing type to enable Unicast/Multicast
*       protocolStack - what type of traffic to enable ipv4 or ipv6 or both.
*       enableRouting - enable IP routing for this port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpPortRoutingEnable
(
    IN GT_U8                            devNum,
    IN GT_U8                            portNum,
    IN CPSS_IP_UNICAST_MULTICAST_ENT    ucMcEnable,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    IN GT_BOOL                          enableRouting
)
{
    GT_U32    regAddr[2] ={0,0};              /* registers address             */
    GT_U32    data, i;
    GT_STATUS rc = GT_OK;                     /* return code                   */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
        localPort = 31;
    else
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    data = (GT_TRUE == enableRouting)? 1:0;

    switch (protocolStack)
    {
        case CPSS_IP_PROTOCOL_IPV4_E:
            switch (ucMcEnable)
            {
                case CPSS_IP_UNICAST_E:
                    /* Unicast Enable Control Register*/
                    regAddr[0] = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv4Regs.ucEnCtrlReg;
                    break;
                case CPSS_IP_MULTICAST_E:
                    /* MC Enable Control Register*/
                    regAddr[0] = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv4Regs.mcEnCtrlReg;
                    break;
                default:
                    return GT_BAD_PARAM;
            }
        break;

        case CPSS_IP_PROTOCOL_IPV6_E:
            switch (ucMcEnable)
            {
                case CPSS_IP_UNICAST_E:
                    /* Unicast Enable Control Register*/
                    regAddr[0] = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv6Regs.ucEnCtrlReg;
                    break;
                case CPSS_IP_MULTICAST_E:
                    /* MC Enable Control Register*/
                    regAddr[0] = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv6Regs.mcEnCtrlReg;
                    break;
                default:
                    return GT_BAD_PARAM;
            }

        break;
        case CPSS_IP_PROTOCOL_IPV4V6_E:
            switch (ucMcEnable)
            {
                case CPSS_IP_UNICAST_E:
                    /*  Unicast Enable Control Register for Ipv6*/
                    regAddr[0] = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv6Regs.ucEnCtrlReg;

                    /* Unicast Enable Control Register*/
                    regAddr[1] = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv4Regs.ucEnCtrlReg;

                    break;

                case CPSS_IP_MULTICAST_E:
                    /* for Ipv6*/
                    regAddr[0] = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv6Regs.mcEnCtrlReg;
                    /* MC Enable Control Register*/
                    regAddr[1] = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv4Regs.mcEnCtrlReg;
                    break;
                default:
                    return GT_BAD_PARAM;
             }

        break;
        default:
            return GT_BAD_PARAM;
    }

    /* now write one or two registers */
    for (i = 0;(rc == GT_OK) && (i <2) ; i++)
    {
        if (regAddr[i] != 0)
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr[i],(GT_U32)localPort ,1,
                                           data);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChIpPortRoutingEnableGet
*
* DESCRIPTION:
*       Get status of multicast/unicast IPv4/v6 routing on a port.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum        - the device number
*       portNum       - the port to enable on
*       ucMcEnable    - routing type to enable Unicast/Multicast
*       protocolStack - what type of traffic to enable ipv4 or ipv6 or both.
*
* OUTPUTS:
*       enableRoutingPtr - (pointer to)enable IP routing for this port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - wrong protocolStack
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       CPSS_IP_PROTOCOL_IPV4V6_E is not supported in this get API.
*       Can not get both values for ipv4 and ipv6 in the same get.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPortRoutingEnableGet
(
    IN  GT_U8                            devNum,
    IN  GT_U8                            portNum,
    IN  CPSS_IP_UNICAST_MULTICAST_ENT    ucMcEnable,
    IN  CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    OUT GT_BOOL                          *enableRoutingPtr
)
{
    GT_U32    regAddr;              /* registers address */
    GT_U32    data=0;
    GT_STATUS rc = GT_OK;           /* return code */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(enableRoutingPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
        localPort = 31;
    else
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    switch (protocolStack)
    {
        case CPSS_IP_PROTOCOL_IPV4_E:
            switch (ucMcEnable)
            {
                case CPSS_IP_UNICAST_E:
                    /* Unicast Enable Control Register*/
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv4Regs.ucEnCtrlReg;
                    break;
                case CPSS_IP_MULTICAST_E:
                    /* MC Enable Control Register*/
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv4Regs.mcEnCtrlReg;
                    break;
                default:
                    return GT_BAD_PARAM;
            }
        break;

        case CPSS_IP_PROTOCOL_IPV6_E:
            switch (ucMcEnable)
            {
                case CPSS_IP_UNICAST_E:
                    /* Unicast Enable Control Register*/
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv6Regs.ucEnCtrlReg;
                    break;
                case CPSS_IP_MULTICAST_E:
                    /* MC Enable Control Register*/
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv6Regs.mcEnCtrlReg;
                    break;
                default:
                    return GT_BAD_PARAM;
            }

        break;
        case CPSS_IP_PROTOCOL_IPV4V6_E:
            /* can not get both values for ipv4 and ipv6 in the same get*/
            return GT_BAD_PARAM;

        default:
            return GT_BAD_PARAM;
    }

    /* now read the register */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,regAddr,(GT_U32)localPort ,1,&data);

    if(rc != GT_OK)
        return rc;

    *enableRoutingPtr = BIT2BOOL_MAC(data);

    return rc;
}
/*******************************************************************************
* cpssDxChIpQosProfileToRouteEntryMapSet
*
* DESCRIPTION:
*       Sets the QoS profile to route entry offset mapping table.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum           - the device number
*       qosProfile       - QOS Profile index
*       routeEntryOffset - The offset in the array of route entries to be
*                          selected for this QOS profile
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In QoS-based routing, the Route table entry is selected according to the
*       following index calculation:
*       1. in DxCh2, DxCh3, xCat: <Route Entry Index> +
*                               (QoSProfile-to-Route-Block-Offset(QoSProfile) %
*                               (<Number of Paths> + 1)
*       2. in Lion : <Route Entry Index> +
*                    Floor(QoSProfile-to-Route-Block-Offset(QoSProfile) *
*                         (<Number of Paths> + 1) / 8)
*
*******************************************************************************/
GT_STATUS cpssDxChIpQosProfileToRouteEntryMapSet
(
    IN  GT_U8                devNum,
    IN  GT_U32               qosProfile,
    IN  GT_U32               routeEntryOffset
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_STATUS rc;                   /* return code                        */
    GT_U32    offset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_QOS2MT_ENTRY_CHECK_MAC(qosProfile);

    /* Get address of the register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.qoSProfile2RouteBlockOffsetMapTable[qosProfile / 8];

    offset = (qosProfile % 8) * 4;
    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,offset,3,routeEntryOffset);

    return rc;
}

/*******************************************************************************
* cpssDxChIpRoutingEnable
*
* DESCRIPTION:
*       globally enable/disable routing.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum        - the device number
*       enableRouting - enable /disable global routing
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - wrong devNum
*
* COMMENTS:
*       the ASIC defualt is routing enabled.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpRoutingEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enableRouting
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_U32    data;
    GT_STATUS rc;                   /* return code                        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    data = (GT_TRUE == enableRouting)? 1:0;
    /* Get address of  register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerGlobalReg;
    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr, 6,1,data);

    return rc;
}

/*******************************************************************************
* cpssDxChIpRoutingEnableGet
*
* DESCRIPTION:
*       Get global routing status
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum              - the device number
*
* OUTPUTS:
*       enableRoutingPtr    -  (pointer to)enable /disable global routing
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChIpRoutingEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enableRoutingPtr
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_U32    data;
    GT_STATUS rc;                   /* return code                        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(enableRoutingPtr);

    /* Get address of  register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerGlobalReg;
    rc = prvCpssDrvHwPpGetRegField(devNum,regAddr, 6,1,&data);

    if(rc != GT_OK)
        return rc;

    *enableRoutingPtr = BIT2BOOL_MAC(data);

    return rc;
}


/*******************************************************************************
* cpssDxChIpCntGet
*
* DESCRIPTION:
*       Return the IP counter set requested.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum  - the device number
*       cntSet - counters set to retrieve.
*
* OUTPUTS:
*       countersPtr - (pointer to)struct contains the counter values.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function doesn't handle counters overflow.
*
*******************************************************************************/
GT_STATUS cpssDxChIpCntGet
(
    IN  GT_U8                        devNum,
    IN  CPSS_IP_CNT_SET_ENT          cntSet,
    OUT CPSS_DXCH_IP_COUNTER_SET_STC *countersPtr
)
{
    return cpssDxChIpPortGroupCntGet(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                     cntSet, countersPtr);
}

/*******************************************************************************
* cpssDxChIpCntSetModeSet
*
* DESCRIPTION:
*      Sets a counter set's bounded inteface and interface mode.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum        - the device number
*       cntSet        - the counter set
*       cntSetMode    - the counter set bind mode (interface or Next hop)
*       interfaceModeCfgPtr - if cntSetMode = CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E
*                          this is the interface configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpCntSetModeSet
(
    IN  GT_U8                                      devNum,
    IN  CPSS_IP_CNT_SET_ENT                        cntSet,
    IN  CPSS_DXCH_IP_CNT_SET_MODE_ENT              cntSetMode,
    IN  CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC *interfaceModeCfgPtr
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_U32    data;
    GT_STATUS rc;                   /* return code                        */
    GT_U8 setNum;
    GT_U32  portGroupId,portPortGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  portValue;    /* port value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    if(cntSetMode != CPSS_DXCH_IP_CNT_SET_ROUTE_ENTRY_MODE_E)
        CPSS_NULL_PTR_CHECK_MAC(interfaceModeCfgPtr);


    switch (cntSet)
    {
        case CPSS_IP_CNT_SET0_E:
            setNum =0;
            break;
        case CPSS_IP_CNT_SET1_E:
            setNum =1;
            break;
        case CPSS_IP_CNT_SET2_E:
            setNum =2;
            break;
        case CPSS_IP_CNT_SET3_E:
            setNum =3;
            break;
        default:
            return GT_BAD_PARAM;
     }

     regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngCntSetCfgRegs[setNum];

     /* support multi-port-groups device - same configuration to all port groups */
     portPortGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

     switch (cntSetMode)
     {
         /* Interface counter */
        case CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E:
            rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,29,1,0);
            if(GT_OK != rc)
                return GT_FAIL;

            /* Set Port/Trunk mode*/
            switch (interfaceModeCfgPtr->portTrunkCntMode)
            {
                case  CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E:
                    data = 0;
                    break;
                case CPSS_DXCH_IP_PORT_CNT_MODE_E:

                    if(interfaceModeCfgPtr->portTrunk.port >= BIT_6)
                    {
                        /* 6 bits in HW */
                        return GT_BAD_PARAM;
                    }

                    if(interfaceModeCfgPtr->devNum >= BIT_5)
                    {
                        /* 5 bits in HW */
                        return GT_BAD_PARAM;
                    }

                    /* hold 'port filter' */
                    if(PRV_CPSS_HW_DEV_NUM_MAC(devNum) == interfaceModeCfgPtr->devNum)
                    {
                        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
                        portPortGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                            interfaceModeCfgPtr->portTrunk.port);
                    }

                    data = 1;

                    /* loop on all port groups :
                        on the port group that 'own' the port , set the needed configuration
                        on other port groups put 'NULL port'
                    */
                    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

                    {
                        if(PRV_CPSS_HW_DEV_NUM_MAC(devNum) != interfaceModeCfgPtr->devNum)
                        {
                            /* no conversion needed for remote device */
                            portValue = interfaceModeCfgPtr->portTrunk.port;
                        }
                        else if(portPortGroupId == portGroupId)
                        {
                            /* convert global to local port */
                            portValue = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,
                                interfaceModeCfgPtr->portTrunk.port);
                        }
                        else
                        {
                            /* use NULL port , so no counting will happen */
                            portValue = PRV_CPSS_DXCH_NULL_PORT_NUM_CNS;
                        }

                        /* Set 6 bits of the port\trunk filed and bits 7,8 are 0. */
                        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr, 0, 8,
                                        portValue);
                        if(GT_OK != rc)
                            return GT_FAIL;
                    }
                    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

                    /* Set device num*/
                    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,8,5,
                                                   interfaceModeCfgPtr->devNum);
                    if(GT_OK != rc)
                        return GT_FAIL;
                    break;

                case CPSS_DXCH_IP_TRUNK_CNT_MODE_E:
                    if(interfaceModeCfgPtr->portTrunk.trunk >= BIT_7)
                    {
                        /* 7 bits in HW */
                        return GT_BAD_PARAM;
                    }
                    /* Set 7 bits of the port\trunk filed and the 8th is 0. */
                    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 8,
                                  interfaceModeCfgPtr->portTrunk.trunk);
                    if(GT_OK != rc)
                        return GT_FAIL;
                    data = 2;
                    break;
                default:
                    return GT_BAD_PARAM;
            }
            rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,30,2,data);
            if(GT_OK != rc)
                return GT_FAIL;
            /* Set IpMode */
            switch (interfaceModeCfgPtr->ipMode)
            {
                case CPSS_IP_PROTOCOL_IPV4_E:
                    data = 1;
                    break;
                case CPSS_IP_PROTOCOL_IPV6_E:
                    data = 2;
                    break;
                case CPSS_IP_PROTOCOL_IPV4V6_E:
                    data = 0;
                    break;
                default:
                    return GT_BAD_PARAM;
            }
            rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,27,2,data);
            if(GT_OK != rc)
                return GT_FAIL;
                /* Set VLAN count mode */
            switch ( interfaceModeCfgPtr->vlanMode)
            {
                case CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E:
                    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,26,1,0);
                    break;
                case CPSS_DXCH_IP_USE_VLAN_CNT_MODE_E:
                    if(interfaceModeCfgPtr->vlanId >= BIT_12)
                    {
                        /* 12 bits in HW */
                        return GT_BAD_PARAM;
                    }
                    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,26,1,1);
                    if(GT_OK != rc)
                        return GT_FAIL;

                    /* Set VID*/
                    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,14,12,
                                                   interfaceModeCfgPtr->vlanId);
                    break;
                default:
                    return GT_BAD_PARAM;
            }

            break;

            case CPSS_DXCH_IP_CNT_SET_ROUTE_ENTRY_MODE_E:
                rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,29,1,1);
                break;

            default:
                return GT_BAD_PARAM;
     }

     return rc;

}

/*******************************************************************************
* cpssDxChIpCntSet
*
* DESCRIPTION:
*       set the requested IP counter set.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum       - the device number
*       cntSet       - counters set to clear.
*       countersPtr  - the counter values to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpCntSet
(
    IN GT_U8                         devNum,
    IN CPSS_IP_CNT_SET_ENT           cntSet,
    IN CPSS_DXCH_IP_COUNTER_SET_STC *countersPtr
)
{
    return cpssDxChIpPortGroupCntSet(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                     cntSet, countersPtr);

}

/*******************************************************************************
* cpssDxChIpSetMllCntInterface
*
* DESCRIPTION:
*      Sets a mll counter set's bounded inteface.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       mllCntSet       - the mll counter set out of the 2.
*       interfaceCfgPtr - the mll counter interface configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpSetMllCntInterface
(
    IN GT_U8                                       devNum,
    IN GT_U32                                      mllCntSet,
    IN CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC *interfaceCfgPtr
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_STATUS rc;                   /* return code                        */
    GT_U32          fieldData = 0;  /* Data to read/write */

    CPSS_NULL_PTR_CHECK_MAC(interfaceCfgPtr);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    /* Cheetah2 supports 2 mll counters*/
    if(mllCntSet > 1)
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.mllOutInterfaceCfg[mllCntSet];

    switch (interfaceCfgPtr->portTrunkCntMode)
    {

        case CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E:
            /*counter-set counts packets Received/Transmitted via binded Port+Dev*/
            rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,30,2,0);
            break;
        case CPSS_DXCH_IP_TRUNK_CNT_MODE_E:
            if(interfaceCfgPtr->portTrunk.trunk >= BIT_7)
            {
                /* 7 bits in HW */
                return GT_BAD_PARAM;
            }

            /* Set port\trunk mode to trunk */
            rc = prvCpssDrvHwPpSetRegField(devNum,regAddr, 30, 2, 2);
            if(GT_OK != rc)
                return rc;
            /* Set 7 bits of the port\trunk filed and the 8th is 0. */
            rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,0, 8,
                                           interfaceCfgPtr->portTrunk.trunk);

            break;
        case CPSS_DXCH_IP_PORT_CNT_MODE_E:

            if(interfaceCfgPtr->portTrunk.port >= BIT_6)
            {
                /* 6 bits in HW */
                return GT_BAD_PARAM;
            }

            if(interfaceCfgPtr->devNum >= BIT_5)
            {
                /* 5 bits in HW */
                return GT_BAD_PARAM;
            }

            /* Set port\trunk mode to port */
            rc = prvCpssDrvHwPpSetRegField(devNum,regAddr, 30, 2, 1);
            if(GT_OK != rc)
                return rc;

            /* Set 6 bits of the port\trunk filed and bits 7,8 are 0. */
            rc = prvCpssDrvHwPpSetRegField(devNum,regAddr, 0, 8,interfaceCfgPtr->portTrunk.port);
            if(GT_OK != rc)
                return rc;

            /* Set Dev*/
            rc = prvCpssDrvHwPpSetRegField(devNum,regAddr, 8, 5,
                                           interfaceCfgPtr->devNum);

            break;
        default:
            return GT_BAD_PARAM;

    }

    if(GT_OK != rc)
        return rc;

    /* Set IpMode */
    switch(interfaceCfgPtr->ipMode)
    {
    case CPSS_IP_PROTOCOL_IPV4_E:
        fieldData = 1;
        break;
    case CPSS_IP_PROTOCOL_IPV6_E:
        fieldData = 2;
        break;
    case CPSS_IP_PROTOCOL_IPV4V6_E:
        fieldData = 0;
        break;
    default:
        return GT_BAD_PARAM;
    }
    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,27,2,fieldData);
    if(GT_OK != rc)
        return rc;


    /* Set vlan mode & vid*/
    switch (interfaceCfgPtr->vlanMode)
    {
        case CPSS_DXCH_IP_USE_VLAN_CNT_MODE_E:
            if(interfaceCfgPtr->vlanId >= BIT_12)
            {
                /* 12 bits in HW */
                return GT_BAD_PARAM;
            }
            rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,26,1,1);
            if(GT_OK != rc)
                return rc;

            rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,14,12,
                                           interfaceCfgPtr->vlanId);
            break;

        case CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E:
            rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,26,1,0);
            break;

        default:
            return GT_BAD_PARAM;
    }

    return rc;

}

/*******************************************************************************
* cpssDxChIpMllCntGet
*
* DESCRIPTION:
*      Get the mll counter.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum     - the device number.
*       mllCntSet  - the mll counter set out of the 2
*
* OUTPUTS:
*       mllOutMCPktsPtr - According to the configuration of this cnt set, The
*                      number of routed IP Multicast packets Duplicated by the
*                      MLL Engine and transmitted via this interface
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpMllCntGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  mllCntSet,
    OUT GT_U32  *mllOutMCPktsPtr
)
{
    return cpssDxChIpPortGroupMllCntGet(devNum,
                                        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        mllCntSet, mllOutMCPktsPtr);
}

/*******************************************************************************
* cpssDxChIpMllCntSet
*
* DESCRIPTION:
*      set an mll counter.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       mllCntSet       - the mll counter set out of the 2
*       mllOutMCPkts    - the counter value to set
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpMllCntSet
(
    IN GT_U8    devNum,
    IN GT_U32   mllCntSet,
    IN GT_U32   mllOutMCPkts
)
{
    return cpssDxChIpPortGroupMllCntSet(devNum,
                                        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        mllCntSet, mllOutMCPkts);
}

/*******************************************************************************
* cpssDxChIpDropCntSet
*
* DESCRIPTION:
*      set the drop counter.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum      - the device number
*       dropPkts - the counter value to set
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpDropCntSet
(
    IN GT_U8 devNum,
    IN GT_U32 dropPkts
)
{
    return cpssDxChIpPortGroupDropCntSet(devNum,
                                         CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                         dropPkts);
}

/*******************************************************************************
* cpssDxChIpSetDropCntMode
*
* DESCRIPTION:
*      Sets the drop counter count mode.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum      - the device number
*       dropCntMode - the drop counter count mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpSetDropCntMode
(
    IN GT_U8                          devNum,
    IN CPSS_DXCH_IP_DROP_CNT_MODE_ENT dropCntMode
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_U32    value;                /* value to write to register         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerDropCntCfgReg;

    switch (dropCntMode)
    {
        case CPSS_DXCH_IP_DROP_CNT_COUNT_ALL_MODE_E             :
            value = 0;
            break;
        case CPSS_DXCH_IP_DROP_CNT_IP_HEADER_MODE_E             :
            value = 1;
            break;
        case CPSS_DXCH_IP_DROP_CNT_DIP_DA_MISMATCH_MODE_E       :
            value = 2;
            break;
        case CPSS_DXCH_IP_DROP_CNT_ILLEGAL_ADDRESS_MODE_E       :
            value = 3;
            break;
        case CPSS_DXCH_IP_DROP_CNT_SIP_ALL_ZEROS_MODE_E         :
            value = 4;
            break;
        case CPSS_DXCH_IP_DROP_CNT_SIP_SA_MISMATCH_MODE_E       :
            value = 5;
            break;
        case CPSS_DXCH_IP_DROP_CNT_UC_RPF_MODE_E                :
            value = 6;
            break;
        case CPSS_DXCH_IP_DROP_CNT_NH_CMD_MODE_E                :
            value = 7;
            break;
        case CPSS_DXCH_IP_DROP_CNT_MC_RPF_MODE_E                :
            value = 8;
            break;
        case CPSS_DXCH_IP_DROP_CNT_TTL_HOP_LIMIT_EXCEEDED_MODE_E:
            value = 9;
            break;
        case CPSS_DXCH_IP_DROP_CNT_MTU_EXCEEDED_MODE_E          :
            value = 10;
            break;
        case CPSS_DXCH_IP_DROP_CNT_OPTION_MODE_E                :
            value = 11;
            break;
        case CPSS_DXCH_IP_DROP_CNT_IPV6_SCOPE_MODE_E            :
            value = 13;
            break;
        case CPSS_DXCH_IP_DROP_CNT_UC_SIP_FILTER_MODE_E         :
            value = 14;
            break;
        case CPSS_DXCH_IP_DROP_CNT_ACCESS_MATRIX_MODE_E         :
            value = 15;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return prvCpssDrvHwPpSetRegField(devNum,regAddr, 0,4, value);
}

/*******************************************************************************
* cpssDxChIpDropCntGet
*
* DESCRIPTION:
*      Get the drop counter.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       dropPktsPtr - the number of counted dropped packets according to the
*                     drop counter mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpDropCntGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *dropPktsPtr
)
{
    return cpssDxChIpPortGroupDropCntGet(devNum,
                                         CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                         dropPktsPtr);
}

/*******************************************************************************
* cpssDxChIpMtuProfileSet
*
* DESCRIPTION:
*      Sets the global MTU profile limit values.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       mtuProfileIndex - the mtu profile index out of the possible 8 (0..7).
*       mtu             - the maximum transmission unit
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpMtuProfileSet
(
    IN GT_U8  devNum,
    IN GT_U32 mtuProfileIndex,
    IN GT_U32 mtu
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_STATUS rc;                   /* return code                        */
    GT_U32  i,bits_offset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    if(mtuProfileIndex > 7)
        return GT_BAD_PARAM;


    i = mtuProfileIndex/2;
    bits_offset = mtuProfileIndex % 2;

    /* for index =0,1 -> offset 0x4, 1,2 - 0x8 end and so on*/
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMtuCfgRegs[i];

    bits_offset *=14;

    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,bits_offset ,14,mtu);
    return rc;
}

/*******************************************************************************
* cpssDxChIpv6AddrPrefixScopeSet
*
* DESCRIPTION:
*       Defines a prefix of a scope type.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum           - the device number
*       prefix           - an IPv6 address prefix
*       prefixLen        - length of the prefix (0 - 16)
*       addressScope     - type of the address scope spanned by the prefix
*       prefixScopeIndex - index of the new prefix scope entry (0..4)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - case that the prefix length is out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Configures an entry in the prefix look up table
*
*******************************************************************************/
GT_STATUS cpssDxChIpv6AddrPrefixScopeSet
(
    IN  GT_U8                           devNum,
    IN  GT_IPV6ADDR                     prefix,
    IN  GT_U32                          prefixLen,
    IN  CPSS_IPV6_PREFIX_SCOPE_ENT      addressScope,
    IN  GT_U32                          prefixScopeIndex
)
{

    GT_U32    regAddr;              /* register address                   */
    GT_STATUS rc;                   /* return code                        */
    GT_STATUS addressScopeValue;

    GT_U32      value=0, mask=0, prefixValue=0;     /* value to write   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    if (prefixLen > 16)
        return (GT_BAD_PARAM);

    switch(addressScope)
    {
    case CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E:
        addressScopeValue = 0;
        break;
    case CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E:
        addressScopeValue = 1;
        break;
    case CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E:
        addressScopeValue = 2;
        break;
    case CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E:
        addressScopeValue = 3;
        break;
    default:
        return GT_BAD_PARAM;
    }

    if(prefixScopeIndex > 3)
        return (GT_BAD_PARAM);

    mask = 0xFFFF << (16-prefixLen);
    mask = mask << 16;
    prefixValue = prefix.arIP[0] << 8;
    prefixValue = prefixValue | prefix.arIP[1];
    prefixValue = 0x0000FFFF & prefixValue;
    value = prefixValue | mask;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv6Regs.ipScopeRegs[prefixScopeIndex];

    rc = prvCpssDrvHwPpWriteRegister(devNum,regAddr,value);
    if (rc != GT_OK)
        {
        return (GT_FAIL);
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv6Regs.ipScopeLevel[prefixScopeIndex];

    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 2, addressScopeValue);

    return rc;

}


/*******************************************************************************
* cpssDxChIpv6UcScopeCommandSet
*
* DESCRIPTION:
*       sets the ipv6 Unicast scope commands.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum           - the device number
*       addressScopeSrc  - IPv6 address scope of source address
*       addressScopeDest - IPv6 address scope of destination address
*       borderCrossed    - GT_TRUE if source site ID is diffrent to destination
*                          site ID
*       scopeCommand     - action to be done on a packet that match the above
*                          scope configuration.
*                          possible commands:
*                           CPSS_PACKET_CMD_ROUTE_E,
*                           CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E,
*                           CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                           CPSS_PACKET_CMD_DROP_SOFT_E,
*                           CPSS_PACKET_CMD_DROP_HARD_E
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_NO_RESOURCE           - if failed to allocate CPU memory
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpv6UcScopeCommandSet
(
    IN GT_U8                      devNum,
    IN CPSS_IPV6_PREFIX_SCOPE_ENT addressScopeSrc,
    IN CPSS_IPV6_PREFIX_SCOPE_ENT addressScopeDest,
    IN GT_BOOL                    borderCrossed,
    IN CPSS_PACKET_CMD_ENT        scopeCommand
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_STATUS rc;                   /* return code                        */
    GT_U32      value=0;        /* value to write   */
    GT_U32   offset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    if ((addressScopeSrc != CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E) &&
        (addressScopeSrc != CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E) &&
        (addressScopeSrc != CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E) &&
        (addressScopeSrc != CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E))
        return GT_BAD_PARAM;

    if ((addressScopeDest != CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E) &&
        (addressScopeDest != CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E) &&
        (addressScopeDest != CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E) &&
        (addressScopeDest != CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E))
        return GT_BAD_PARAM;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv6Regs.ucScopeTblRegs[addressScopeSrc];

    /* if the border was not crossed, meaning source Site ID == Dest Site ID
       the offset=4. if the border was crossed the offset=0 */
    if (borderCrossed == GT_FALSE)
    {
        offset = 4;
    }
    else
    {
        offset = 0;
    }

    offset += (addressScopeDest*8);

    switch (scopeCommand)
    {
    case CPSS_PACKET_CMD_ROUTE_E:
        value = 0;
        break;

    case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
        value = 2;
        break;

    case CPSS_PACKET_CMD_DROP_SOFT_E:
        value = 4;
        break;

    case CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E:
        value = 1;
        break;

    case CPSS_PACKET_CMD_DROP_HARD_E:
        value = 3;
        break;

    default:
        return GT_BAD_PARAM;
    }

    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, offset, 3, value);

    return rc;
}

/*******************************************************************************
* cpssDxChIpv6McScopeCommandSet
*
* DESCRIPTION:
*       sets the ipv6 Multicast scope commands.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum           - the device number
*       addressScopeSrc  - IPv6 address scope of source address
*       addressScopeDest - IPv6 address scope of destination address
*       borderCrossed    - GT_TRUE if source site ID is diffrent to destination
*                          site ID
*       scopeCommand     - action to be done on a packet that match the above
*                          scope configuration.
*                          possible commands:
*                           CPSS_PACKET_CMD_ROUTE_E,
*                           CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E,
*                           CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                           CPSS_PACKET_CMD_DROP_SOFT_E,
*                           CPSS_PACKET_CMD_DROP_HARD_E,
*                           CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E,
*                           CPSS_PACKET_CMD_BRIDGE_E
*       mllSelectionRule - rule for choosing MLL for IPv6 Multicast propogation
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_NO_RESOURCE           - if failed to allocate CPU memory
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpv6McScopeCommandSet
(
    IN GT_U8                            devNum,
    IN CPSS_IPV6_PREFIX_SCOPE_ENT       addressScopeSrc,
    IN CPSS_IPV6_PREFIX_SCOPE_ENT       addressScopeDest,
    IN GT_BOOL                          borderCrossed,
    IN CPSS_PACKET_CMD_ENT              scopeCommand,
    IN CPSS_IPV6_MLL_SELECTION_RULE_ENT mllSelectionRule
)
{

    GT_U32    regAddr;              /* register address                   */
    GT_STATUS rc;                   /* return code                        */
    GT_U32      value=0;        /* value to write   */
    GT_U32   offset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    if ((addressScopeSrc != CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E) &&
        (addressScopeSrc != CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E) &&
        (addressScopeSrc != CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E) &&
        (addressScopeSrc != CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E))
        return GT_BAD_PARAM;

    if ((addressScopeDest != CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E) &&
        (addressScopeDest != CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E) &&
        (addressScopeDest != CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E) &&
        (addressScopeDest != CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E))
        return GT_BAD_PARAM;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipv6Regs.mcScopeTblRegs[addressScopeSrc];

    switch (scopeCommand)
    {
        case CPSS_PACKET_CMD_ROUTE_E:
            value = 0;
            break;
        case CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E:
            value = 1;
            break;
        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            value = 2;
            break;
        case CPSS_PACKET_CMD_DROP_HARD_E:
            value = 3;
            break;

        case CPSS_PACKET_CMD_DROP_SOFT_E:
            value = 4;
            break;

        case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            value = 5;
            break;
        case CPSS_PACKET_CMD_BRIDGE_E:
            value = 6;
                break;
        default:
            return GT_BAD_PARAM;
    }

    /* if the border was not crossed, meaning source Site ID == Dest Site ID
       the offset=4. if the border was crossed the offset=0 */
    if (borderCrossed == GT_FALSE)
    {
        offset = 4;
    }
    else
    {
        offset = 0;
    }

    offset += (addressScopeDest*8);

    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, offset, 3, value);
    if (rc != GT_OK)
    {
        return (GT_FAIL);
    }

        /* write MLL Select */

    switch(mllSelectionRule)
    {
        case CPSS_IPV6_MLL_SELECTION_RULE_LOCAL_E:
             value = 0;
             break;
        case CPSS_IPV6_MLL_SELECTION_RULE_GLOBAL_E:
             value = 1;
             break;
        default:
            return GT_BAD_PARAM;

    }

    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, offset+3, 1, value);
    return rc;
}


/*******************************************************************************
* cpssDxChIpRouterMacSaBaseSet
*
* DESCRIPTION:
*       Sets 40 MSBs of Router MAC SA Base address on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - the device number
*       macPtr - (pointer to)The system Mac address to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterMacSaBaseSet
(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *macPtr
)
{
    GT_U32  macBytes1234;          /* Holds bits 8 - 39 of mac     */
    GT_U32  macMsbByte;            /* Holds bits 40 - 47 of mac    */
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(macPtr);

    macBytes1234 = (macPtr->arEther[4]        |
                  (macPtr->arEther[3] << 8)  |
                  (macPtr->arEther[2] << 16) |
                  (macPtr->arEther[1] << 24));
    macMsbByte =  macPtr->arEther[0];

    rc = prvCpssDrvHwPpWriteRegister(devNum,
           PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.routerMacSaBaseReg[0],macBytes1234);
    if(rc != GT_OK)
        return rc;

    rc = prvCpssDrvHwPpSetRegField(devNum,
           PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.routerMacSaBaseReg[1],0,8,macMsbByte);
    if(rc != GT_OK)
        return rc;

    return rc;
}

/*******************************************************************************
* cpssDxChIpRouterMacSaBaseGet
*
* DESCRIPTION:
*       Get 40 MSBs of Router MAC SA Base address on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - the device number
*
* OUTPUTS:
*       macPtr - (pointer to)The system Mac address to set.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterMacSaBaseGet
(
    IN  GT_U8           devNum,
    OUT GT_ETHERADDR    *macPtr
)
{
    GT_STATUS status = GT_OK;
    GT_U32  macBytes1234 = 0;          /* Holds bits 8 - 39 of mac     */
    GT_U32  macMsbByte = 0;            /* Holds bits 40 - 47 of mac    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(macPtr);


    status = prvCpssDrvHwPpReadRegister(devNum,
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.routerMacSaBaseReg[0],&macBytes1234);
    if(status != GT_OK)
        return status;

    status = prvCpssDrvHwPpGetRegField(devNum,
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.routerMacSaBaseReg[1],0,8,&macMsbByte);
    if(status != GT_OK)
        return status;

    macPtr->arEther[4] = (GT_U8)(macBytes1234 & 0xFF);
    macPtr->arEther[3] = (GT_U8)((macBytes1234 & 0xFF00) >> 8);
    macPtr->arEther[2] = (GT_U8)((macBytes1234 & 0xFF0000) >> 16);
    macPtr->arEther[1] = (GT_U8)((macBytes1234 & 0xFF000000) >> 24);
    macPtr->arEther[0] = (GT_U8)macMsbByte;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChIpRouterMacSaLsbModeSet
*
* DESCRIPTION:
*       Sets the mode in which the device sets the packet's MAC SA least
*       significant bytes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       saLsbMode       - The MAC SA least-significant bit mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum/saLsbMode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The device 5 most significant bytes are set by cpssDxChIpRouterMacSaBaseSet().
*
*       The least significant bytes are set by cpssDxChIpRouterPortVlanMacSaLsbSet().
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterMacSaLsbModeSet
(
    IN  GT_U8                        devNum,
    IN  CPSS_MAC_SA_LSB_MODE_ENT     saLsbMode
)
{
    GT_U32      hwData; /* value to write into register   */
    GT_STATUS   rc ;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);


     /* setting the Lsb mode */
    switch (saLsbMode)
    {
        case CPSS_SA_LSB_PER_PORT_E:
            hwData = 2;
            break;
        case CPSS_SA_LSB_PER_PKT_VID_E:
            hwData = 0;
            break;
        case CPSS_SA_LSB_PER_VLAN_E:
            hwData = 1;
            break;
        default:

        return GT_BAD_PARAM;
    }

    rc = prvCpssDrvHwPpWriteRegister(
               devNum,
               PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig,
               hwData);
    return rc;
}

/*******************************************************************************
* cpssDxChIpRouterMacSaLsbModeGet
*
* DESCRIPTION:
*       Gets the mode in which the device sets the packet's MAC SA least
*       significant bytes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum          - the device number
*
* OUTPUTS:
*       saLsbModePtr    - (pointer to) The MAC SA least-significant bit mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum/saLsbMode.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterMacSaLsbModeGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_MAC_SA_LSB_MODE_ENT     *saLsbModePtr
)
{
    GT_U32      hwData; /* value to write into register   */
    GT_STATUS   rc ;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(saLsbModePtr);

    rc = prvCpssDrvHwPpReadRegister(devNum,
           PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig,
           &hwData);

    if(rc != GT_OK)
        return rc;

     /* setting the Lsb mode */
    switch (hwData)
    {
        case 2:
            *saLsbModePtr = CPSS_SA_LSB_PER_PORT_E;
            break;
        case 0:
            *saLsbModePtr = CPSS_SA_LSB_PER_PKT_VID_E;
            break;
        case 1:
            *saLsbModePtr = CPSS_SA_LSB_PER_VLAN_E;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChIpPortRouterMacSaLsbModeSet
*
* DESCRIPTION:
*       Sets the mode, per port, in which the device sets the packet's MAC SA
*       least significant bytes.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - the device number
*       portNum         - the port number
*       saLsbMode       - The MAC SA least-significant bit mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum/saLsbMode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The device 5 most significant bytes are set by cpssDxChIpRouterMacSaBaseSet().
*
*       The least significant bytes are set by:
*       Port mode is set by cpssDxChIpRouterPortMacSaLsbSet().
*       Vlan mode is set by cpssDxChIpRouterVlanMacSaLsbSet().
*
*******************************************************************************/
GT_STATUS cpssDxChIpPortRouterMacSaLsbModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_MAC_SA_LSB_MODE_ENT    saLsbMode
)
{
    GT_U32      hwData; /* value to write into register   */
    GT_U32      regAddr;
    GT_U32      offset;
    GT_STATUS   rc ;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

     /* setting the Lsb mode */
    switch (saLsbMode)
    {
        case CPSS_SA_LSB_PER_PORT_E:
            hwData = 2;
            break;
        case CPSS_SA_LSB_PER_PKT_VID_E:
            hwData = 0;
            break;
        case CPSS_SA_LSB_PER_VLAN_E:
            hwData = 1;
            break;
        default:

        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.routerHdrAltMacSaModifyMode[localPort / 16];
    offset = (localPort % 16) * 2;
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,offset,2,hwData);

    return rc;
}


/*******************************************************************************
* cpssDxChIpPortRouterMacSaLsbModeGet
*
* DESCRIPTION:
*       Gets the mode, per port, in which the device sets the packet's MAC SA least
*       significant bytes.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - the device number
*       portNum         - the port number
*
* OUTPUTS:
*       saLsbModePtr    - (pointer to) The MAC SA least-significant bit mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum/saLsbMode.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None;
*
*******************************************************************************/
GT_STATUS cpssDxChIpPortRouterMacSaLsbModeGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    OUT CPSS_MAC_SA_LSB_MODE_ENT    *saLsbModePtr
)
{
    GT_U32      hwData; /* value to write into register   */
    GT_U32      regAddr;
    GT_U32      offset;
    GT_STATUS   rc ;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(saLsbModePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.routerHdrAltMacSaModifyMode[localPort / 16];
    offset = (localPort % 16) * 2;
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,regAddr,offset,2,&hwData);
    if(rc != GT_OK)
        return rc;

     /* setting the Lsb mode */
    switch (hwData)
    {
        case 2:
            *saLsbModePtr = CPSS_SA_LSB_PER_PORT_E;
            break;
        case 0:
            *saLsbModePtr = CPSS_SA_LSB_PER_PKT_VID_E;
            break;
        case 1:
            *saLsbModePtr = CPSS_SA_LSB_PER_VLAN_E;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return rc;
}


/*******************************************************************************
* cpssDxChIpRouterPortVlanMacSaLsbSet
*
* DESCRIPTION:
*       Sets the 8 LSB Router MAC SA for this VLAN / EGGRESS PORT.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*     devNum          - the device number
*     vlanPortId      - VLAN Id or Eggress Port Id
*     saMac           - The 8 bits SA mac value to be written to the SA bits of
*                       routed packet if SA alteration mode is configured to
*                       take LSB according to VLAN id / Eggress Port Id.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK                    - on success
*     GT_FAIL                  - on error.
*     GT_HW_ERROR              - on hardware error
*     GT_BAD_PARAM             - wrong devNum or vlanId
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterPortVlanMacSaLsbSet
(
    IN GT_U8   devNum,
    IN GT_U16  vlanPortId,
    IN GT_U8   saMac
)
{
    GT_U32      hwData[1]; /* data to write to PP's table */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* zhangdi cancel for xcat debug, as there is have not the following code in 275 */
#if 0
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);
#endif
    
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanPortId);

    hwData[0] = saMac;

    rc = prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E,
                                    (GT_U32)vlanPortId, hwData);
    return rc;
}

/*******************************************************************************
* cpssDxChIpRouterPortVlanMacSaLsbGet
*
* DESCRIPTION:
*       Gets the 8 LSB Router MAC SA for this VLAN / EGGRESS PORT.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*     devNum          - the device number
*     vlanPortId      - VLAN Id or Eggress Port Id
*
* OUTPUTS:
*     saMacPtr        - (pointer to) The 8 bits SA mac value written to the SA
*                       bits of routed packet if SA alteration mode is configured
*                       to take LSB according to VLAN id / Eggress Port Id.
*
* RETURNS:
*     GT_OK                    - on success
*     GT_FAIL                  - on error.
*     GT_HW_ERROR              - on hardware error
*     GT_BAD_PARAM             - wrong devNum or vlanId
*     GT_BAD_PTR               - one of the parameters is NULL pointer
*     GT_TIMEOUT       - after max number of retries checking if PP ready
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterPortVlanMacSaLsbGet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanPortId,
    OUT GT_U8   *saMacPtr
)
{
    GT_U32      hwData[1]; /* data read from PP's table */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanPortId);
    CPSS_NULL_PTR_CHECK_MAC(saMacPtr);

    rc = prvCpssDxChReadTableEntry(devNum,
                                    PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E,
                                    (GT_U32)vlanPortId, hwData);

    if(rc != GT_OK)
        return rc;

    *saMacPtr = (GT_U8)hwData[0];

    return rc;
}

/*******************************************************************************
* cpssDxChIpRouterPortMacSaLsbSet
*
* DESCRIPTION:
*       Sets the 8 LSB Router MAC SA for this EGGRESS PORT.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*     devNum          - the device number
*     portNum         - Eggress Port number
*     saMac           - The 8 bits SA mac value to be written to the SA bits of
*                       routed packet if SA alteration mode is configured to
*                       take LSB according to Eggress Port number.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK                    - on success
*     GT_FAIL                  - on error.
*     GT_HW_ERROR              - on hardware error
*     GT_BAD_PARAM             - wrong devNum
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterPortMacSaLsbSet
(
    IN GT_U8   devNum,
    IN GT_U8   portNum,
    IN GT_U8   saMac
)
{
    GT_U32      hwData[1]; /* data to write to PP's table */
    GT_STATUS   rc;
    GT_U32      tableIndex;

    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    hwData[0] = saMac;

    /* port entries start after 4096 vlan entries */
    tableIndex = localPort + 4096;

    rc = prvCpssDxChPortGroupWriteTableEntry(devNum,portGroupId,
                                    PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E,
                                    tableIndex, hwData);
    return rc;
}

/*******************************************************************************
* cpssDxChIpRouterPortMacSaLsbGet
*
* DESCRIPTION:
*       Gets the 8 LSB Router MAC SA for this EGGRESS PORT.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*     devNum          - the device number
*     portNum         - Eggress Port number
*
* OUTPUTS:
*     saMacPtr        - (pointer to) The 8 bits SA mac value written to the SA
*                       bits of routed packet if SA alteration mode is configured
*                       to take LSB according to Eggress Port number.
*
* RETURNS:
*     GT_OK                    - on success
*     GT_FAIL                  - on error.
*     GT_HW_ERROR              - on hardware error
*     GT_BAD_PARAM             - wrong devNum
*     GT_BAD_PTR               - one of the parameters is NULL pointer
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterPortMacSaLsbGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_U8   *saMacPtr
)
{
    GT_U32      hwData[1]; /* data read from PP's table */
    GT_STATUS   rc;
    GT_U32      tableIndex;
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8       localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(saMacPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    /* port entries start after 4096 vlan entries */
    tableIndex = localPort + 4096;

    rc = prvCpssDxChPortGroupReadTableEntry(devNum,portGroupId,
                                    PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E,
                                    tableIndex, hwData);

    if(rc != GT_OK)
        return rc;

    *saMacPtr = (GT_U8)hwData[0];

    return rc;
}

/*******************************************************************************
* cpssDxChIpRouterVlanMacSaLsbSet
*
* DESCRIPTION:
*       Sets the 8 LSB Router MAC SA for this VLAN.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*     devNum          - the device number
*     vlan            - VLAN Id
*     saMac           - The 8 bits SA mac value to be written to the SA bits of
*                       routed packet if SA alteration mode is configured to
*                       take LSB according to VLAN.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK                    - on success
*     GT_FAIL                  - on error.
*     GT_HW_ERROR              - on hardware error
*     GT_BAD_PARAM             - wrong devNum or vlanId
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterVlanMacSaLsbSet
(
    IN GT_U8   devNum,
    IN GT_U16  vlan,
    IN GT_U8   saMac
)
{
    GT_U32      hwData[1]; /* data to write to PP's table */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlan);

    hwData[0] = saMac;

    rc = prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E,
                                    (GT_U32)vlan, hwData);
    return rc;
}

/*******************************************************************************
* cpssDxChIpRouterVlanMacSaLsbGet
*
* DESCRIPTION:
*       Gets the 8 LSB Router MAC SA for this VLAN.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*     devNum          - the device number
*     vlan            - VLAN Id
*
* OUTPUTS:
*     saMacPtr        - (pointer to) The 8 bits SA mac value written to the SA
*                       bits of routed packet if SA alteration mode is configured
*                       to take LSB according to VLAN.
*
* RETURNS:
*     GT_OK                    - on success
*     GT_FAIL                  - on error.
*     GT_HW_ERROR              - on hardware error
*     GT_BAD_PARAM             - wrong devNum or vlanId
*     GT_BAD_PTR               - one of the parameters is NULL pointer
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterVlanMacSaLsbGet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlan,
    OUT GT_U8   *saMacPtr
)
{
    GT_U32      hwData[1]; /* data read from PP's table */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlan);
    CPSS_NULL_PTR_CHECK_MAC(saMacPtr);

    rc = prvCpssDxChReadTableEntry(devNum,
                                    PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E,
                                    (GT_U32)vlan, hwData);

    if(rc != GT_OK)
        return rc;

    *saMacPtr = (GT_U8)hwData[0];

    return rc;
}


/*******************************************************************************
* cpssDxChIpRouterMacSaModifyEnable
*
* DESCRIPTION:
*      Per Egress port bit Enable Routed packets MAC SA Modification
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - the device number
*       portNum  - physical or CPU port number.
*       enable   - GT_FALSE: MAC SA Modification of routed packets is disabled
*                  GT_TRUE: MAC SA Modification of routed packets is enabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum/portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterMacSaModifyEnable
(
    IN  GT_U8                      devNum,
    IN  GT_U8                      portNum,
    IN  GT_BOOL                    enable
)
{
    GT_U32  bitNum;
    GT_U32      regAddr;
    GT_U32      value;
    GT_STATUS   rc ;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    value = (enable == GT_TRUE) ? 1 : 0;

    bitNum = (localPort == CPSS_CPU_PORT_NUM_CNS) ?
                 PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum) :
                 localPort;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                    routerHdrAltEnMacSaModifyReg[OFFSET_TO_WORD_MAC(bitNum)];

    rc =  prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,
                OFFSET_TO_BIT_MAC(bitNum),1,value);

    return rc;
}

/*******************************************************************************
* cpssDxChIpRouterMacSaModifyEnableGet
*
* DESCRIPTION:
*      Per Egress port bit Get Routed packets MAC SA Modification State
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - the device number
*       portNum       - physical or CPU port number.
*
* OUTPUTS:
*       enablePtr     - GT_FALSE: MAC SA Modification of routed packets is disabled
*                  GT_TRUE: MAC SA Modification of routed packets is enabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum/portNum.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterMacSaModifyEnableGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    OUT GT_BOOL                     *enablePtr
)
{
    GT_U32  bitNum;
    GT_U32      regAddr;
    GT_U32      value;
    GT_STATUS   rc ;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    bitNum = (localPort == CPSS_CPU_PORT_NUM_CNS) ?
                 PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum) :
                 localPort;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                    routerHdrAltEnMacSaModifyReg[OFFSET_TO_WORD_MAC(bitNum)];

    rc =  prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,regAddr,
                OFFSET_TO_BIT_MAC(bitNum),1,&value);
    if(rc != GT_OK)
        return rc;

    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return rc;
}

/*******************************************************************************
* cpssDxChIpEcmpUcRpfCheckEnableSet
*
* DESCRIPTION:
*       Globally enables/disables ECMP/QoS unicast RPF check.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum    - the device number
*       enable    - enable / disable ECMP/Qos unicast RPF check
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpEcmpUcRpfCheckEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
)
{
    GT_U32      value;
    GT_STATUS   rc ;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT2_E);


    value = (enable == GT_TRUE) ? 1 : 0;

    rc =  prvCpssDrvHwPpSetRegField(devNum,
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerAdditionalCtrlReg,
                5,1,value);

    return rc;
}

/*******************************************************************************
* cpssDxChIpEcmpUcRpfCheckEnableGet
*
* DESCRIPTION:
*       Gets globally enables/disables ECMP/QoS unicast RPF check state.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       enablePtr - (points to) enable / disable ECMP/Qos unicast RPF check
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpEcmpUcRpfCheckEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32      value;
    GT_STATUS   rc ;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    rc =  prvCpssDrvHwPpGetRegField(devNum,
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerAdditionalCtrlReg,
                5,1,&value);
    if(rc != GT_OK)
        return rc;

    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return rc;
}

/*******************************************************************************
* cpssDxChIpPortGroupCntSet
*
* DESCRIPTION:
*       set the requested IP counter set.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum         - the device number
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       cntSet       - counters set to clear.
*       countersPtr  - the counter values to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPortGroupCntSet
(
    IN GT_U8                         devNum,
    IN GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN CPSS_IP_CNT_SET_ENT           cntSet,
    IN CPSS_DXCH_IP_COUNTER_SET_STC *countersPtr
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_STATUS rc;                   /* return code                        */
    GT_U8 setNum;

    CPSS_NULL_PTR_CHECK_MAC(countersPtr);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    switch(cntSet)
    {
        case CPSS_IP_CNT_SET0_E:
            setNum =0;
            break;
        case CPSS_IP_CNT_SET1_E:
            setNum =1;
            break;
        case CPSS_IP_CNT_SET2_E:
            setNum =2;
            break;
        case CPSS_IP_CNT_SET3_E:
            setNum =3;
            break;
        default:
            return GT_BAD_PARAM;

    }

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /* Router Management In Unicast Packets Counter Set<n> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngInUcPktCntSet[setNum];
    rc = prvCpssDxChPortGroupsBmpCounterSet(devNum, portGroupsBmp, regAddr,
                                             0, 32, countersPtr->inUcPkts);

    if(GT_OK != rc)
        return GT_FAIL;

    /* Router Management In Multicast Packets Counter Set<n> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngMcPktCntSet[setNum];
    rc = prvCpssDxChPortGroupsBmpCounterSet(devNum, portGroupsBmp, regAddr,
                                             0, 32, countersPtr->inMcPkts);

    if(GT_OK != rc)
        return GT_FAIL;

    /*Router Management In Unicast Non-Routed Non-Exception Packets
     Counter Set<n> */

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngInUcNonRoutedNonExcptPktCntSet[setNum];
    rc = prvCpssDxChPortGroupsBmpCounterSet(devNum, portGroupsBmp, regAddr,
                                            0, 32,
                                   countersPtr->inUcNonRoutedNonExcpPkts);

    if(GT_OK != rc)
        return GT_FAIL;

    /*Router Management In Unicast Non-Routed Exception Packets Counter Set<n> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngInUcNonRoutedExcptPktCntSet[setNum];
    rc = prvCpssDxChPortGroupsBmpCounterSet(devNum, portGroupsBmp, regAddr,
                                             0, 32,
                                   countersPtr->inUcNonRoutedExcpPkts);

    if(GT_OK != rc)
        return GT_FAIL;

    /*Router Management In Multicast Non-Routed Non-Exception Packets
                                           Counter Set<n> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngInMcNonRoutedNonExcptPktCntSet[setNum];
    rc = prvCpssDxChPortGroupsBmpCounterSet(devNum, portGroupsBmp, regAddr,
                                             0, 32,
                                   countersPtr->inMcNonRoutedNonExcpPkts);

    if(GT_OK != rc)
        return GT_FAIL;
    /*Router Management In Multicast Non-Routed Exception Packets Counter
                                              Set<n> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngInMcNonRoutedExcptCntSet[setNum];
    rc = prvCpssDxChPortGroupsBmpCounterSet(devNum, portGroupsBmp, regAddr,
                                            0, 32,
                                   countersPtr->inMcNonRoutedExcpPkts);
    if(GT_OK != rc)
        return GT_FAIL;

    /* Router Management In Unicast Trapped / Mirrored Packet Counter Set<n> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngInUcTrapMrrPktCntSet[setNum];
    rc = prvCpssDxChPortGroupsBmpCounterSet(devNum,portGroupsBmp, regAddr,
                                             0, 32,
                                   countersPtr->inUcTrappedMirrorPkts);
    if(GT_OK != rc)
        return GT_FAIL;

    /* Router Management In Multicast Trapped / Mirrored Packet Counter Set<n> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngInMcTrapMrrPktCntSet[setNum];
    rc = prvCpssDxChPortGroupsBmpCounterSet(devNum, portGroupsBmp, regAddr,
                                            0, 32,
                                   countersPtr->inMcTrappedMirrorPkts);
    if(GT_OK != rc)
        return GT_FAIL;

    /* Router Management In Multicast RPF Fail Counter Set<n> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngInMcRPFFailCntSet[setNum];
    rc = prvCpssDxChPortGroupsBmpCounterSet(devNum, portGroupsBmp, regAddr,
                                             0, 32,
                                   countersPtr->mcRfpFailPkts);
    if(GT_OK != rc)
        return GT_FAIL;

    /* Router Management Out Unicast Packet Counter Set<n> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngOutUcPktCntSet[setNum];

    rc = prvCpssDxChPortGroupsBmpCounterSet(devNum, portGroupsBmp, regAddr,
                                            0, 32,
                                   countersPtr->outUcRoutedPkts);
    if(GT_OK != rc)
        return GT_FAIL;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChIpPortGroupCntGet
*
* DESCRIPTION:
*       Return the IP counter set requested.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum         - the device number
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       cntSet - counters set to retrieve.
*
* OUTPUTS:
*       countersPtr - (pointer to)struct contains the counter values.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function doesn't handle counters overflow.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPortGroupCntGet
(
    IN  GT_U8                        devNum,
    IN  GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN  CPSS_IP_CNT_SET_ENT          cntSet,
    OUT CPSS_DXCH_IP_COUNTER_SET_STC *countersPtr
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_STATUS rc;                   /* return code                        */
    GT_U8 setNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    CPSS_NULL_PTR_CHECK_MAC(countersPtr);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    switch(cntSet)
    {
        case CPSS_IP_CNT_SET0_E:
            setNum =0;
            break;
        case CPSS_IP_CNT_SET1_E:
            setNum =1;
            break;
        case CPSS_IP_CNT_SET2_E:
            setNum =2;
            break;
        case CPSS_IP_CNT_SET3_E:
            setNum =3;
            break;
        default:
            return GT_BAD_PARAM;

    }
    /* Router Management In Unicast Packets Counter Set<n> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngInUcPktCntSet[setNum];
    rc = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp, regAddr,
                                            0,32,&(countersPtr->inUcPkts),NULL);

    if(GT_OK != rc)
        return rc;

    /* Router Management In Multicast Packets Counter Set<n> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngMcPktCntSet[setNum];
    rc = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp, regAddr,
                                            0,32,&(countersPtr->inMcPkts),NULL);

    if(GT_OK != rc)
        return rc;

    /*Router Management In Unicast Non-Routed Non-Exception Packets
     Counter Set<n> */

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngInUcNonRoutedNonExcptPktCntSet[setNum];
    rc = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp, regAddr, 0,32,
                                   &(countersPtr->inUcNonRoutedNonExcpPkts),NULL);

    if(GT_OK != rc)
        return rc;

    /*Router Management In Unicast Non-Routed Exception Packets Counter Set<n> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngInUcNonRoutedExcptPktCntSet[setNum];
    rc = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp, regAddr, 0,32,
                                   &(countersPtr->inUcNonRoutedExcpPkts),NULL);

    if(GT_OK != rc)
        return rc;

    /*Router Management In Multicast Non-Routed Non-Exception Packets
                                           Counter Set<n> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngInMcNonRoutedNonExcptPktCntSet[setNum];
    rc = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp, regAddr, 0,32,
                                   &(countersPtr->inMcNonRoutedNonExcpPkts),NULL);

    if(GT_OK != rc)
        return rc;
   /*Router Management In Multicast Non-Routed Exception Packets Counter
                                          Set<n> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngInMcNonRoutedExcptCntSet[setNum];
    rc = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp, regAddr, 0,32,
                                   &(countersPtr->inMcNonRoutedExcpPkts),NULL);
    if(GT_OK != rc)
        return rc;

    /* Router Management In Unicast Trapped / Mirrored Packet Counter Set<n> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngInUcTrapMrrPktCntSet[setNum];
    rc = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp, regAddr, 0,32,
                                   &(countersPtr->inUcTrappedMirrorPkts),NULL);
    if(GT_OK != rc)
        return rc;

    /* Router Management In Multicast Trapped / Mirrored Packet Counter Set<n> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngInMcTrapMrrPktCntSet[setNum];
    rc = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp, regAddr, 0,32,
                                   &(countersPtr->inMcTrappedMirrorPkts),NULL);
    if(GT_OK != rc)
        return rc;

    /* Router Management In Multicast RPF Fail Counter Set<n> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngInMcRPFFailCntSet[setNum];
    rc = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp, regAddr, 0,32,
                                   &(countersPtr->mcRfpFailPkts),NULL);
    if(GT_OK != rc)
        return rc;

    /* Router Management Out Unicast Packet Counter Set<n> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerMngOutUcPktCntSet[setNum];

    rc = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp, regAddr, 0,32,
                                   &(countersPtr->outUcRoutedPkts),NULL);
    if(GT_OK != rc)
        return rc;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChIpPortGroupDropCntSet
*
* DESCRIPTION:
*      set the drop counter.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum         - the device number
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       dropPkts - the counter value to set
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPortGroupDropCntSet
(
    IN GT_U8                devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN GT_U32               dropPkts
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_STATUS rc;                   /* return code                        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerDropCnt;

    /* set the counter */
    rc = prvCpssDxChPortGroupsBmpCounterSet(devNum, portGroupsBmp, regAddr,
                                             0, 32, dropPkts);

    return rc;
}

/*******************************************************************************
* cpssDxChIpPortGroupDropCntGet
*
* DESCRIPTION:
*      Get the drop counter.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum         - the device number
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       dropPktsPtr - the number of counted dropped packets according to the
*                     drop counter mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPortGroupDropCntGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *dropPktsPtr
)
{
    GT_STATUS rc;                   /* return code                        */
    GT_U32    regAddr;              /* register address                   */

    CPSS_NULL_PTR_CHECK_MAC(dropPktsPtr);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerDropCnt;

    rc = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp, regAddr,
                                                 0, 32, dropPktsPtr, NULL);
    return rc;
}

/*******************************************************************************
* cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet
*
* DESCRIPTION:
*      Get the multi target queue full drop packet counter.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum         - the device number
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       dropPktsPtr - the number of counted dropped packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet
(
    IN   GT_U8                  devNum,
    IN   GT_PORT_GROUPS_BMP     portGroupsBmp,
    OUT  GT_U32                 *dropPktsPtr
)
{
    GT_STATUS rc;                   /* return code                        */
    GT_U32    regAddr;              /* register address                   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(dropPktsPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.mllMcFifoFullDropCnt;

    /* summary the counter from all port groups */
    rc = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp, regAddr,
                                                0, 32, dropPktsPtr, NULL);

    return rc;

}

/*******************************************************************************
* cpssDxChIpPortGroupMultiTargetQueueFullDropCntSet
*
* DESCRIPTION:
*      set the multi target queue full drop packet counter.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum         - the device number
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       dropPkts    - the counter value to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPortGroupMultiTargetQueueFullDropCntSet
(
    IN GT_U8                 devNum,
    IN GT_PORT_GROUPS_BMP    portGroupsBmp,
    IN GT_U32                dropPkts
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_STATUS rc;                   /* return code                        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.mllMcFifoFullDropCnt;

    /* set the counter */
    rc =  prvCpssDxChPortGroupsBmpCounterSet(devNum, portGroupsBmp, regAddr,
                                             0, 32, dropPkts);
    return rc;

}

/*******************************************************************************
* cpssDxChIpPortGroupMllCntGet
*
* DESCRIPTION:
*      Get the mll counter.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number.
*       portGroupsBmp   - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       mllCntSet  - the mll counter set out of the 2
*
* OUTPUTS:
*       mllOutMCPktsPtr - According to the configuration of this cnt set, The
*                      number of routed IP Multicast packets Duplicated by the
*                      MLL Engine and transmitted via this interface
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPortGroupMllCntGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  mllCntSet,
    OUT GT_U32                  *mllOutMCPktsPtr
)
{
    GT_STATUS rc;                   /* return code                        */
    GT_U32    regAddr;              /* register address                   */

    CPSS_NULL_PTR_CHECK_MAC(mllOutMCPktsPtr);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    if(mllCntSet > 1)
        return GT_BAD_PARAM;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.mllOutMcPktCnt[mllCntSet];

    rc = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp, regAddr,
                                                0, 32, mllOutMCPktsPtr, NULL);
    return rc;

}

/*******************************************************************************
* cpssDxChIpPortGroupMllCntSet
*
* DESCRIPTION:
*      set an mll counter.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       portGroupsBmp   - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       mllCntSet       - the mll counter set out of the 2
*       mllOutMCPkts    - the counter value to set
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPortGroupMllCntSet
(
    IN GT_U8                    devNum,
    IN GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN GT_U32                   mllCntSet,
    IN GT_U32                   mllOutMCPkts
)
{

    GT_U32    regAddr;              /* register address                   */

    GT_STATUS rc;                   /* return code                        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    if(mllCntSet > 1)
        return GT_BAD_PARAM;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.mllOutMcPktCnt[mllCntSet];
    /* set the counter */
    rc = prvCpssDxChPortGroupsBmpCounterSet(devNum, portGroupsBmp, regAddr,
                                            0, 32, mllOutMCPkts);

    return rc;

}

/*******************************************************************************
* cpssDxChIpUcRpfVlanModeSet
*
* DESCRIPTION:
*       Defines the uRPF check mode for a given VID.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum    - the device number
*       vid       - Vlan Id
*       uRpfMode  - CPSS_DXCH_IP_VLAN_URPF_DISABLE_MODE_E = uRPF check is disabled
*                        for this VID.
*                   CPSS_DXCH_IP_VLAN_URPF_VLAN_MODE_E= Vlan: If ECMP uRPF is
*                                        globally enabled, then uRPF check is done
*                                        by comparing the packet VID to the VID
*                                        in the additional route entry, otherwise
*                                        it is done using the SIP-Next Hop Entry VID.
*                   CPSS_DXCH_IP_VLAN_URPF_PORT_MODE_E = uRPF check is done by
*                        comparing the packet source {device,port}/Trunk to
*                              the SIP-Next Hop Entry {device,port}/Trunk.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device, vid or uRpfMode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If ucRPFCheckEnable field configured in cpssDxChIpLttWrite is enabled
*       then VLAN-based uRPF check is performed, regardless of this configuration.
*       Otherwise, uRPF check is performed for this VID according to this
*       configuration.
*       Port-based uRPF mode is not supported if the address is associated
*       with an ECMP/QoS block of nexthop entries.
*
*******************************************************************************/
GT_STATUS cpssDxChIpUcRpfVlanModeSet
(
    IN  GT_U8                           devNum,
    IN  GT_U16                          vid,
    IN  CPSS_DXCH_IP_VLAN_URPF_MODE_ENT uRpfMode
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      entryIndex;
    GT_U32      fieldOffset;
    GT_U32      fieldWordNum;
    GT_U32      fieldLength;
    GT_U32      fieldValue;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vid);

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                               PRV_CPSS_DXCH_LION_URPF_PER_VLAN_NOT_SUPPORTED_WA_E))
    {
        return GT_NOT_SUPPORTED;
    }

    switch(uRpfMode)
    {
    case CPSS_DXCH_IP_VLAN_URPF_DISABLE_MODE_E:
        fieldValue = 0;
        break;
    case CPSS_DXCH_IP_VLAN_URPF_VLAN_MODE_E:
        fieldValue = 1;
        break;
    case CPSS_DXCH_IP_VLAN_URPF_PORT_TRUNK_MODE_E:
        fieldValue = 2;
        break;
    default:
        return GT_BAD_PARAM;
    }

    entryIndex      =   (GT_U32)(vid / 16);    /* 0 - 255 number of lines   */
    fieldOffset     =   (GT_U32)((vid % 16)*2);/* 16 vlans in each lines    */
    fieldWordNum    =   0;                     /* one word in each line     */
    fieldLength     =   2;                     /* 2 bits to hold uRpf VLAN mode */

    rc = prvCpssDxChWriteTableEntryField(devNum,
                                      PRV_CPSS_DXCH_LION_TABLE_ROUTER_VLAN_URPF_STC_E,
                                      entryIndex,
                                      fieldWordNum,
                                      fieldOffset,
                                      fieldLength,
                                      fieldValue);
    return rc;
}

/*******************************************************************************
* cpssDxChIpUcRpfVlanModeGet
*
* DESCRIPTION:
*       Read uRPF check mode for a given VID.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum    - the device number
*       vid       - Vlan Id
*
* OUTPUTS:
*       uRpfModePtr  -  CPSS_DXCH_IP_VLAN_URPF_DISABLE_MODE_E = uRPF check is disabled
*                           for this VID.
*                       CPSS_DXCH_IP_VLAN_URPF_VLAN_MODE_E= Vlan: If ECMP uRPF is
*                                        globally enabled, then uRPF check is done
*                                        by comparing the packet VID to the VID
*                                        in the additional route entry, otherwise
*                                        it is done using the SIP-Next Hop Entry VID.
*                       CPSS_DXCH_IP_VLAN_URPF_PORT_MODE_E = uRPF check is done by
*                           comparing the packet source {device,port}/Trunk to
*                           the SIP-Next Hop Entry {device,port}/Trunk.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       If ucRPFCheckEnable field configured in cpssDxChIpLttWrite is enabled
*       then VLAN-based uRPF check is performed, regardless of this configuration.
*       Otherwise, uRPF check is performed for this VID according to this
*       configuration.
*       Port-based uRPF mode is not supported if the address is associated
*       with an ECMP/QoS block of nexthop entries.
*
*******************************************************************************/
GT_STATUS cpssDxChIpUcRpfVlanModeGet
(
    IN  GT_U8                               devNum,
    IN  GT_U16                              vid,
    OUT CPSS_DXCH_IP_VLAN_URPF_MODE_ENT     *uRpfModePtr
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      entryIndex;
    GT_U32      fieldOffset;
    GT_U32      fieldWordNum;
    GT_U32      fieldLength;
    GT_U32      fieldValue;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vid);
    CPSS_NULL_PTR_CHECK_MAC(uRpfModePtr);

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                               PRV_CPSS_DXCH_LION_URPF_PER_VLAN_NOT_SUPPORTED_WA_E))
    {
        return GT_NOT_SUPPORTED;
    }


    entryIndex      =   (GT_U32)(vid / 16);    /* 0 - 255 number of lines   */
    fieldOffset     =   (GT_U32)((vid % 16)*2);/* 16 vlans in each lines    */
    fieldWordNum    =   0;                     /* one word in each line     */
    fieldLength     =   2;                     /* 2 bits to hold uRpf VLAN mode */

    rc = prvCpssDxChReadTableEntryField(devNum,
                                      PRV_CPSS_DXCH_LION_TABLE_ROUTER_VLAN_URPF_STC_E,
                                      entryIndex,
                                      fieldWordNum,
                                      fieldOffset,
                                      fieldLength,
                                      &fieldValue);
    if(rc != GT_OK)
        return rc;

    switch(fieldValue)
    {
    case 0:
        *uRpfModePtr = CPSS_DXCH_IP_VLAN_URPF_DISABLE_MODE_E;
        break;
    case 1:
        *uRpfModePtr = CPSS_DXCH_IP_VLAN_URPF_VLAN_MODE_E;
        break;
    case 2:
        *uRpfModePtr = CPSS_DXCH_IP_VLAN_URPF_PORT_TRUNK_MODE_E;
        break;
    default:
        return GT_BAD_STATE;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChIpPortSipSaEnableSet
*
* DESCRIPTION:
*       Enable SIP/SA check for packets received from the given port.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum    - the device number
*       portNum   - the port number (including CPU port)
*       enable    - GT_FALSE: disable SIP/SA check on the port
*                   GT_TRUE:  enable SIP/SA check on the port
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
*       SIP/SA check is triggered only if either this flag or
*       the sipSaCheckMismatchEnable field configured in cpssDxChIpLttWrite
*       are enabled.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPortSipSaEnableSet
(
    IN  GT_U8                        devNum,
    IN  GT_U8                        portNum,
    IN  GT_BOOL                      enable
)
{
    GT_U32      regAddr;
    GT_STATUS   rc = GT_OK; /* return code */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8       localPort;  /* local port - support multi-port-groups device */
    GT_U32      data;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
        localPort = 31;
    else
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    data = BOOL2BIT_MAC(enable);

    /* Get address of register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerPerPortSipSaEnable0;

    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,localPort,1,data);

    return rc;
}

/*******************************************************************************
* cpssDxChIpPortSipSaEnableGet
*
* DESCRIPTION:
*       Return the SIP/SA check status for packets received from the given port.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum    - the device number
*       portNum   - the port number (including CPU port)
*
* OUTPUTS:
*       enablePtr - GT_FALSE: SIP/SA check on the port is disabled
*                   GT_TRUE:  SIP/SA check on the port is enabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       SIP/SA check is triggered only if either this flag or
*       the sipSaCheckMismatchEnable field configured in cpssDxChIpLttWrite
*       are enabled.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPortSipSaEnableGet
(
    IN  GT_U8                        devNum,
    IN  GT_U8                        portNum,
    OUT GT_BOOL                      *enablePtr
)
{
    GT_U32      regAddr;
    GT_STATUS   rc = GT_OK; /* return code */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8       localPort;  /* local port - support multi-port-groups device */
    GT_U32      data;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
        localPort = 31;
    else
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    /* Get address of register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerPerPortSipSaEnable0;

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,regAddr,localPort,1,&data);

    if(rc != GT_OK)
        return rc;

    *enablePtr = BIT2BOOL_MAC(data);

    return rc;
}

/*******************************************************************************
* cpssDxChIpPortGroupMultiTargetRateShaperSet
*
* DESCRIPTION:
*      set the Multi target Rate shaper params.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum     - the device number
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       multiTargetRateShaperEnable - weather to enable (disable) the Multi
*                                     target Rate shaper
*       windowSize - if enabled then this is the shaper window size (0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi port-group device, the multi-target rate shaping
*       shapes a traffic on per-port-group basis according to the
*       source port of the packet.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPortGroupMultiTargetRateShaperSet
(
    IN  GT_U8              devNum,
    IN  GT_PORT_GROUPS_BMP portGroupsBmp,
    IN  GT_BOOL            multiTargetRateShaperEnable,
    IN  GT_U32             windowSize
)
{
    GT_U32    regAddr;         /* register address  */
    GT_U32    data;            /* value to write    */
    GT_U32    portGroupId;/*the port group Id - support multi-port-groups device */
    GT_STATUS rc;              /* return code       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    if ((multiTargetRateShaperEnable == GT_TRUE) &&
        (GT_FALSE == CHECK_BITS_DATA_RANGE_MAC(windowSize,16)))
        return GT_OUT_OF_RANGE;

    /* Get address of Multi Target Rate Shaping Register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.multiTargetRateShapingReg;

    data = (GT_TRUE == multiTargetRateShaperEnable)? 1:0;

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,
                                                    24,1,data);
        if (GT_OK != rc)
        {
            return rc;
        }

        /*if enabled then this is the shaper window size*/
        if (GT_TRUE == multiTargetRateShaperEnable)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,
                                                        0,16,windowSize);
        }

        if (GT_OK != rc)
        {
            return rc;
        }

    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}


