/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgGen.c
*
* DESCRIPTION:
*       CPSS Dx-Ch implementation for Bridge Generic APIs.
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgGen.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
#include <cpss/generic/bridge/cpssGenBrgGen.h>
/*******************************************************************************
* cpssDxChBrgGenIgmpSnoopEnable
*
* DESCRIPTION:
*       Enable/disable trapping all IPv4 IGMP packets to the CPU, based on
*       their Ingress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       port   - physical port number
*       status - GT_TRUE for enable and GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number or port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIgmpSnoopEnable
(
    IN GT_U8        devNum,
    IN GT_U8        port,
    IN GT_BOOL      status
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */
    GT_U32 portGroupId; /*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.portControl[port];

    value = BOOL2BIT_MAC(status);

    /* Ingress Port<n> Bridge Configuration Register0, field <IGMPTrapEn> */
    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 12, 1, value);
}

/*******************************************************************************
* cpssDxChBrgGenIgmpSnoopEnableGet
*
* DESCRIPTION:
*       Get status of trapping all IPv4 IGMP packets to the CPU, based on
*       their Ingress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       port   - physical port number
*
* OUTPUTS:
*       statusPtr - (pointer to ) GT_TRUE for enable and GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number or port
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIgmpSnoopEnableGet
(
    IN  GT_U8        devNum,
    IN  GT_U8        port,
    OUT GT_BOOL      *statusPtr
)
{
    GT_STATUS rc;
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */
    GT_U32 portGroupId; /*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.portControl[port];

    /* Ingress Port<n> Bridge Configuration Register0, field <IGMPTrapEn> */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 12, 1, &value);
    if (rc != GT_OK)
        return rc;

    *statusPtr = BIT2BOOL_MAC(value);
    return rc;
}

/*******************************************************************************
* cpssDxChBrgGenDropIpMcEnable
*
* DESCRIPTION:
*       Discard all non-Control-classified Ethernet packets
*       with a MAC Multicast DA corresponding to the IP Multicast range,
*       i.e. the MAC range 01-00-5e-00-00-00 to 01-00-5e-7f-ff-ff.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       state   - GT_TRUE, enable filtering mode
*                 GT_FALSE, disable filtering mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenDropIpMcEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  state
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.bridgeGlobalConfigRegArray[0];

    value = BOOL2BIT_MAC(state);

    /* Bridge Global Configuration Register0, field <DropIPMcEn> */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 25, 1, value);
}

/*******************************************************************************
* cpssDxChBrgGenDropNonIpMcEnable
*
* DESCRIPTION:
*       Discard all non-Control-classified Ethernet
*       packets with a MAC Multicast DA (but not the Broadcast MAC address)
*       not corresponding to the IP Multicast range
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       state   - GT_TRUE, enable filtering mode
*                 GT_FALSE, disable filtering mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenDropNonIpMcEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  state
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                bridgeRegs.bridgeGlobalConfigRegArray[0];
    value = BOOL2BIT_MAC(state);

    /* Bridge Global Configuration Register0, field <DropNonIPMcEn> */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 26, 1, value);
}

/*******************************************************************************
* cpssDxChBrgGenDropInvalidSaEnable
*
* DESCRIPTION:
*       Drop all Ethernet packets with MAC SA that are Multicast (MACDA[40] = 1)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE, enable filtering mode
*                GT_FALSE, disable filtering mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenDropInvalidSaEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32 regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                bridgeRegs.bridgeGlobalConfigRegArray[0];

    /* Bridge Global Configuration Register0, field <DropInvalidSA> */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 27, 1, BOOL2BIT_MAC(enable));
}

/*******************************************************************************
* cpssDxChBrgGenUcLocalSwitchingEnable
*
* DESCRIPTION:
*       Enable/disable local switching back through the ingress interface
*       for for known Unicast packets
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       port    - physical port number
*       enable  - GT_FALSE = Unicast packets whose bridging decision is to be
*                            forwarded back to its Ingress port or trunk
*                            are assigned with a soft drop command.
*                 GT_TRUE  = Unicast packets whose bridging decision is to be
*                            forwarded back to its Ingress port or trunk may be
                             forwarded back to their source.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. If the packet is received from a cascade port, the packet’s source
*       location is taken from the FORWARD DSA  tag and not according to the
*       local device and port number.
*       Routed packets are not subject to local switching configuration
*       constraints.
*
*       2. For xCat and above devices to enable  local switching of known
*       Unicast traffic, both ingress port configuration and VLAN entry (by
*       function cpssDxChBrgVlanLocalSwitchingEnableSet) must be enabled.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenUcLocalSwitchingEnable
(
    IN GT_U8        devNum,
    IN GT_U8        port,
    IN GT_BOOL      enable
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */
    GT_U32 portGroupId; /*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.portControl[port];

    value = BOOL2BIT_MAC(enable);

    /* Ingress Port<n> Bridge Configuration Register0, field <UcLocalEn> */
    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 1, 1, value);
}

/*******************************************************************************
* cpssDxChBrgGenIgmpSnoopModeSet
*
* DESCRIPTION:
*       Set global trap/mirror mode for IGMP snoop on specified device.
*       Relevant when IGMP Trap disabled by cpssDxChBrgGenIgmpSnoopEnable
*       and IGMP Trap/Mirror enabled on a VLAN by function
*       cpssDxChBrgVlanIpCntlToCpuSet
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       mode   - Trap mode: All IGMP packets are trapped to the CPU,
*                   regardless of their type.
*                IGMP Snoop mode: Query messages are mirrored to the CPU.
*                   Non-Query messages are Trapped to the CPU.
*                IGMP Router mode: All IGMP packets are mirrored to the CPU,
*                    regardless of their type.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number or IGMP snooping mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIgmpSnoopModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_IGMP_SNOOP_MODE_ENT     mode
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.bridgeGlobalConfigRegArray[0];
    switch (mode)
    {
        case CPSS_IGMP_ALL_TRAP_MODE_E:
            value = 0;
            break;
        case CPSS_IGMP_SNOOP_TRAP_MODE_E:
            value = 1;
            break;
        case CPSS_IGMP_ROUTER_MIRROR_MODE_E:
            value = 2;
            break;
        default:
            return GT_BAD_PARAM;
   }

    /* Bridge Global Configuration Register0, <IGMPMode> */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 12, 2, value);
}

/*******************************************************************************
* cpssDxChBrgGenArpBcastToCpuCmdSet
*
* DESCRIPTION:
*       Enables trapping or mirroring to CPU ARP Broadcast packets for all VLANs,
*       with with IPv4/IPv6 Control Traffic To CPU Enable set by function
*       cpssDxChBrgVlanIpCntlToCpuSet
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       cmd     - supported commands:
*                 CPSS_PACKET_CMD_FORWARD_E , CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                 CPSS_PACKET_CMD_TRAP_TO_CPU_E
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or control packet command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenArpBcastToCpuCmdSet
(
    IN GT_U8                devNum,
    IN CPSS_PACKET_CMD_ENT  cmd
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch (cmd)
    {
        case CPSS_PACKET_CMD_FORWARD_E:
            value = 0;
            break;
        case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
            value = 1;
            break;
        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            value = 2;
            break;
        default:
            return GT_BAD_PARAM;
    }

     regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.bridgeGlobalConfigRegArray[0];

     /* Bridge Global Configuration Register0, field <ARPBCCmd> */
     return prvCpssDrvHwPpSetRegField(devNum, regAddr, 7, 2, value);
}

/*******************************************************************************
* cpssDxChBrgGenRipV1MirrorToCpuEnable
*
* DESCRIPTION:
*       Enable/disable mirroring of IPv4 RIPv1 control messages to the CPU
*       for specified device for all VLANs,  with IPv4/IPv6 Control Traffic
*       To CPU Enable set by function cpssDxChBrgVlanIpCntlToCpuSet
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       enable - GT_FALSE = Forward normally
*                GT_TRUE  = Mirror-to-CPU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenRipV1MirrorToCpuEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.bridgeGlobalConfigRegArray[0];

    value = BOOL2BIT_MAC(enable);

    /* Bridge Global Configuration Register0, field <IPv4RIPv1MirrorEn> */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 28, 1, value);
}

/*******************************************************************************
* cpssDxChBrgGenIeeeReservedMcastTrapEnable
*
* DESCRIPTION:
*       Enables trapping or mirroring to CPU Multicast packets, with MAC_DA in
*       the IEEE reserved Multicast range (01-80-C2-00-00-xx).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       enable - GT_FALSE -
*                   Trapping or mirroring to CPU of packet with
*                   MAC_DA = 01-80-C2-00-00-xx disabled.
*                GT_TRUE -
*                   Trapping or mirroring to CPU of packet, with
*                   MAC_DA = 01-80-C2-00-00-xx enabled according to the setting
*                   of IEEE Reserved Multicast Configuration Register<n> (0<=n<16)
*                   by cpssDxChBrgGenIeeeReservedMcastProtCmdSet function
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIeeeReservedMcastTrapEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.bridgeGlobalConfigRegArray[0];

    value = BOOL2BIT_MAC(enable);

    /* Bridge Global Configuration Register0, field <IEEEReservedMulticastToCPUEn> */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 5, 1, value);
}

/*******************************************************************************
* cpssDxChBrgGenIeeeReservedMcastProtCmdSet
*
* DESCRIPTION:
*       Enables forwarding, trapping, or mirroring to the CPU any of the
*       packets with MAC DA in the IEEE reserved, Multicast addresses
*       for bridge control traffic, in the range of 01-80-C2-00-00-xx,
*       where 0<=xx<256
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       profileIndex - profile index (0..3). Parameter is relevant only for
*                      xCat and above. The parameter defines profile (table
*                      number) for the 256 protocols. Profile bound per port by
*                      <cpssDxChBrgGenPortIeeeReservedMcastProfileIndexSet>.
*       protocol     - specifies the Multicast protocol
*       cmd          - supported commands:
*                      CPSS_PACKET_CMD_FORWARD_E , CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                      CPSS_PACKET_CMD_TRAP_TO_CPU_E , CPSS_PACKET_CMD_DROP_SOFT_E
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number, protocol or control packet command
*                      of profileIndex.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DxCh1, DxCh2, DxCh3 devices:
*       BPDU packets with MAC DA = 01-80-C2-00-00-00 are not affect
*       by this mechanism. BPDUs are implicitly trapped to the CPU
*       if the ingress port span tree is not disabled.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIeeeReservedMcastProtCmdSet
(
    IN GT_U8                devNum,
    IN GT_U32               profileIndex,
    IN GT_U8                protocol,
    IN CPSS_PACKET_CMD_ENT  cmd
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 field;       /* register field offset */
    GT_U32 value;       /* value to write into register */
    GT_U32 regData;     /* register data */
    GT_U32 regMask;     /* register data mask*/
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch (cmd)
    {
        case CPSS_PACKET_CMD_FORWARD_E:
            value = 0;
            break;
        case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
            value = 1;
            break;
        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            value = 2;
            break;
        case CPSS_PACKET_CMD_DROP_SOFT_E:
            value = 3;
            break;
        default:
            return GT_BAD_PARAM;
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        /* ignore profile index input value but always use first register in the DB */
        profileIndex = 0;
    }
    else
    {
        /* xCat and above devices supports only 4 profiles */
        if (profileIndex >= 4)
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        bridgeRegs.macRangeFltrBase[profileIndex] + 0x1000 * (protocol / 16);

    field = 2 * (protocol % 16);

    /* IEEE Reserved Multicast Configuration Register<n> */

    /* IEEE Reserved Multicast Configuration registers read errata */
    if(PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
          PRV_CPSS_DXCH_XCAT_IEEE_RESERVED_MC_CONFG_REG_READ_WA_E) == GT_TRUE)
    {
        if(profileIndex > 1)
        {
            rc = prvCpssDrvHwPpReadRegister(devNum, (regAddr + 0x10), &regData);
            if(rc != GT_OK)
            {
                return rc;
            }

            regMask = ~(0x3 << field);
            regData &= regMask;
            regData |= value << field;
            return prvCpssDrvHwPpWriteRegister(devNum, regAddr, regData);
        }
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, field, 2, value);
}

/*******************************************************************************
* cpssDxChBrgGenIeeeReservedMcastProtCmdGet
*
* DESCRIPTION:
*       Gets command (forwarding, trapping, or mirroring to the CPU) any of the
*       packets with MAC DA in the IEEE reserved, Multicast addresses
*       for bridge control traffic, in the range of 01-80-C2-00-00-xx,
*       where 0<=xx<256
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       profileIndex - profile index (0..3). Parameter is relevant only for
*                      xCat and above. The parameter defines profile (table
*                      number) for the 256 protocols. Profile bound per port by
*                      <cpssDxChBrgGenPortIeeeReservedMcastProfileIndexSet>.
*       protocol     - specifies the Multicast protocol
*
* OUTPUTS:
*       cmdPtr       - (pointer to) command
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number, protocol or control packet command
*                      of profileIndex.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIeeeReservedMcastProtCmdGet
(
    IN GT_U8                devNum,
    IN GT_U32               profileIndex,
    IN GT_U8                protocol,
    OUT CPSS_PACKET_CMD_ENT *cmdPtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 field;       /* register field offset */
    GT_U32 value;       /* value to read from register */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(cmdPtr);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        /* ignore profile index input value but always use first register in the DB */
        profileIndex = 0;
    }
    else
    {
        /* xCat and above devices supports only 4 profiles */
        if (profileIndex >= 4)
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        bridgeRegs.macRangeFltrBase[profileIndex] + 0x1000 * (protocol / 16);

    /* IEEE Reserved Multicast Configuration registers read errata */
    if(PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
          PRV_CPSS_DXCH_XCAT_IEEE_RESERVED_MC_CONFG_REG_READ_WA_E) == GT_TRUE)
    {
        if(profileIndex > 1)
        {
            regAddr += 0x10;
        }
    }

    field = 2 * (protocol % 16);

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, field, 2, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (value)
    {
        case 0:
            *cmdPtr = CPSS_PACKET_CMD_FORWARD_E;
            break;
        case 1:
            *cmdPtr = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
            break;
        case 2:
            *cmdPtr = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            break;
        default:
            *cmdPtr = CPSS_PACKET_CMD_DROP_SOFT_E;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgGenCiscoL2ProtCmdSet
*
* DESCRIPTION:
*       Enables trapping or mirroring to the CPU packets,
        with MAC DA = 0x01-00-0C-xx-xx-xx running CISCO Layer 2 proprietary
        protocols for specified device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*      cmd     - supported commands:
*                 CPSS_PACKET_CMD_FORWARD_E , CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                 CPSS_PACKET_CMD_TRAP_TO_CPU_E
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or control packet command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenCiscoL2ProtCmdSet
(
    IN GT_U8                devNum,
    IN CPSS_PACKET_CMD_ENT  cmd
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch (cmd)
    {
        case CPSS_PACKET_CMD_FORWARD_E:
            value = 0;
            break;
        case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
            value = 1;
            break;
        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            value = 2;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.bridgeGlobalConfigRegArray[0];

    /* Bridge Global Configuration Register0, field <CiscoCommand> */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 9, 2, value);
}

/*******************************************************************************
* cpssDxChBrgGenIpV6IcmpTrapEnable
*
* DESCRIPTION:
*     Enable/disable IpV6 ICMP trapping per port for specified device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum    - device number
*        port   - physical port number
*        enable - GT_FALSE -
*                   IPv6 ICMP packets received on this port are not trapped
*                   to the CPU. These packets may be trapped or mirrored
*                   to the CPU based on the VID assigned to the packet
*                   see cpssDxChBrgVlanIpV6IcmpToCpuEnable function
*                 GT_TRUE -
*                   IPv6 ICMP packets with a ICMP type that matches one of the
*                   ICMP types configured in the IPv6 ICMP Message Type
*                   Configuration Register<n> by function
*                   cpssDxChBrgGenIcmpv6MsgTypeSet are trapped to the CPU.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIpV6IcmpTrapEnable
(
    IN GT_U8      devNum,
    IN GT_U8      port,
    IN GT_BOOL    enable
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */
    GT_U32 portGroupId; /*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.portControl[port];

    value = BOOL2BIT_MAC(enable);

    /* Ingress Port<n> Bridge Configuration Register0, field <ICMPTrapEn> */
    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 14, 1, value);
}

/*******************************************************************************
* cpssDxChBrgGenIpV6IcmpTrapEnableGet
*
* DESCRIPTION:
*     Gets status of IpV6 ICMP trapping per port for specified device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum    - device number
*        port   - physical port number
*
* OUTPUTS:
*        enablePtr - (pointer to)
*                   GT_FALSE -
*                   IPv6 ICMP packets received on this port are not trapped
*                   to the CPU. These packets may be trapped or mirrored
*                   to the CPU based on the VID assigned to the packet
*                   see cpssDxChBrgVlanIpV6IcmpToCpuEnable function
*                 GT_TRUE -
*                   IPv6 ICMP packets with a ICMP type that matches one of the
*                   ICMP types configured in the IPv6 ICMP Message Type
*                   Configuration Register<n> by function
*                   cpssDxChBrgGenIcmpv6MsgTypeSet are trapped to the CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIpV6IcmpTrapEnableGet
(
    IN GT_U8       devNum,
    IN GT_U8       port,
    OUT GT_BOOL    *enablePtr
)
{
    GT_STATUS rc;          /* return status */
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */
    GT_U32 portGroupId; /*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.portControl[port];

    /* Ingress Port<n> Bridge Configuration Register0, field <ICMPTrapEn> */
    rc  = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 14, 1, &value);
    if(rc != GT_OK)
        return rc;

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;

}

/*******************************************************************************
* cpssDxChBrgGenIcmpv6MsgTypeSet
*
* DESCRIPTION:
*       Set trap/mirror/forward command for specified ICMP message type.
*       To enable ICMPv6 trapping/mirroring on a VLAN interface,
*       set the <IPv6ICMP ToCPUEn> bit in the VLAN<n> Entry by function
*       cpssDxChBrgVlanIpV6IcmpToCpuEnable
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - device number
*       index   - the index of the message type(up to eight IPv6 ICMP types)
*                 values 0..7
*       msgType - ICMP message type
*       cmd     - supported commands:
*                 CPSS_PACKET_CMD_FORWARD_E , CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                 CPSS_PACKET_CMD_TRAP_TO_CPU_E
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or control packet command
*                      or index > 7
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIcmpv6MsgTypeSet
(
    IN GT_U8                devNum,
    IN GT_U32               index,
    IN GT_U8                msgType,
    IN CPSS_PACKET_CMD_ENT  cmd
)
{
    GT_U32  regAddr;                /* register address */
    GT_U32  value;                  /* value to write into register */
    GT_U32  field;                  /* register field offset */
    GT_U32  rc;                     /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if (index >= BIT_3)
    {
        return GT_BAD_PARAM;
    }

    switch (cmd)
    {
        case CPSS_PACKET_CMD_FORWARD_E:
            value = 0;
            break;

        case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
            value = 1;
            break;

        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            value = 2;
            break;

        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.ipv6IcmpMsgCmd;
    field = 2 * index;

    /* IPv6 ICMP Command Register <ICMPCmd> */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, field, 2, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        bridgeRegs.ipv6IcmpMsgTypeBase + 0x1000 * (index / 4);

    field = 8 * (index % 4);
    value = msgType;

    /* IPv6 ICMP Message Type Configuration Register<n> <ICMPMsgType<> */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, field, 8, value);
}

/*******************************************************************************
* cpssDxChBrgGenIpLinkLocalMirrorToCpuEnable
*
* DESCRIPTION:
*       Enable/disable mirroring to CPU for IP Link Local Control protocol.
*       IPv4 and IPv6 define link-local Multicast addresses that are used
*       by various protocols.
*       In IPv4, the link-local IANA Multicast range is 224.0.0.0/24.
*       In IPv6, the link-local IANA Multicast range is FF02::/16.
*       Relevant only if IP Control to CPU enabled by
*       cpssDxChBrgVlanIpCntlToCpuSet
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum         - device number
*        protocolStack  - IPv4 or IPv6 protocol version
*        enable         - enable/disable mirroring to CPU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number or IP protocol version
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIpLinkLocalMirrorToCpuEnable
(
    IN GT_U8                        devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN GT_BOOL                      enable
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into register */
    GT_U32 field;           /* register field offset */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch (protocolStack)
    {
        case CPSS_IP_PROTOCOL_IPV4_E:
            field = 23;
            break;
        case CPSS_IP_PROTOCOL_IPV6_E:
            field = 11;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                bridgeRegs.bridgeGlobalConfigRegArray[0];

    value = BOOL2BIT_MAC(enable);

    /* Bridge Global Configuration Register0, field <IPv4/6LinkLocalMirrorEn> */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, field, 1, value);
}

/*******************************************************************************
* cpssDxChBrgGenIpLinkLocalMirrorToCpuEnableGet
*
* DESCRIPTION:
*       Get enable/disable status of mirroring to CPU
*       for IP Link Local Control protocol.
*       IPv4 and IPv6 define link-local Multicast addresses that are used
*       by various protocols.
*       In IPv4, the link-local IANA Multicast range is 224.0.0.0/24.
*       In IPv6, the link-local IANA Multicast range is FF02::/16.
*       Relevant only if IP Control to CPU enabled by
*       cpssDxChBrgVlanIpCntlToCpuSet
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum         - device number
*        protocolStack  - IPv4 or IPv6 protocol version
*
* OUTPUTS:
*        enablePtr      - pointer to enable/disable status of mirroring to CPU.
*                         GT_TRUE  - mirroring to CPU for IP Link Local
*                                    Control protocol is enabled.
*                         GT_FALSE - mirroring to CPU for IP Link Local
*                                    Control protocol is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number or IP protocol version
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIpLinkLocalMirrorToCpuEnableGet
(
    IN  GT_U8                        devNum,
    IN  CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    OUT GT_BOOL                      *enablePtr
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into register */
    GT_U32 field;           /* register field offset */
    GT_STATUS rc;           /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    switch (protocolStack)
    {
        case CPSS_IP_PROTOCOL_IPV4_E:
            field = 23;
            break;
        case CPSS_IP_PROTOCOL_IPV6_E:
            field = 11;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                bridgeRegs.bridgeGlobalConfigRegArray[0];

    /* Bridge Global Configuration Register0, field <IPv4/6LinkLocalMirrorEn> */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, field, 1, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgGenIpLinkLocalProtMirrorToCpuEnable
*
* DESCRIPTION:
*       Enable/disable mirroring to CPU for IP Link Local Control protocol.
*       IPv4 Multicast addresses in the range 224.0.0.x and IPv6 Multicast
*       addresses in the range FF:02::/16 are reserved by IANA
*       for link-local control protocols.
*       If the VLAN is enabled for IP Control to CPU by
*       cpssDxChBrgVlanIpCntlToCpuSet and IPv4/6 Link Local Mirror enabled
*       by cpssDxChBrgGenIpLinkLocalMirrorToCpuEnable,
*       any of the IPv4/6 Multicast packets with a DIP in this range
*       may be mirrored to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       protocolStack - IPv4 or IPv6 protocol version
*       protocol    - link local IP protocol
*       enable      - enable/disable mirroring to CPU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number, IP protocol version or
                          Link Local IP protocol
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIpLinkLocalProtMirrorToCpuEnable
(
    IN GT_U8                        devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN GT_U8                        protocol,
    IN GT_BOOL                      enable
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into register */
    GT_U32 field;           /* register field offset */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch (protocolStack)
    {
        case CPSS_IP_PROTOCOL_IPV4_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bridgeRegs.ipv4McastLinkLocalBase + 0x1000 * (protocol / 32);
            break;
        case CPSS_IP_PROTOCOL_IPV6_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bridgeRegs.ipv6McastLinkLocalBase + 0x1000 * (protocol / 32);
            break;
        default:
            return GT_BAD_PARAM;
    }

    field = protocol % 32;
    value = BOOL2BIT_MAC(enable);

    /* IPv4/6 Multicast Link-Local Configuration Register<n> <IPv4MulLinkLocalMirrorEn> */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, field, 1, value);
}

/*******************************************************************************
* cpssDxChBrgGenIpV6SolicitedCmdSet
*
* DESCRIPTION:
*       Set trap/mirror command for IPv6 Neighbor Solicitation Protocol.
*       If the VLAN entry, indexed by the VID assigned to the packet,
*       IP Control to CPU enabled by cpssDxChBrgVlanIpCntlToCpuSet,
*       IPv6 Neighbor Solicitation messages may be trapped or mirrored
*       to the CPU, if the packets DIP matches the configured prefix
*       and according to the setting of IPv6 Neighbor Solicited Node Command.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*       cmd     - supported commands:   CPSS_PACKET_CMD_FORWARD_E,
*                                       CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                                       CPSS_PACKET_CMD_TRAP_TO_CPU_E.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM    - on wrong device number or packet control command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIpV6SolicitedCmdSet
(
    IN GT_U8                devNum,
    IN CPSS_PACKET_CMD_ENT  cmd
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch (cmd)
    {
        case CPSS_PACKET_CMD_FORWARD_E:
            value = 0;
            break;
        case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
            value = 1;
            break;
        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            value = 2;
            break;
        default:
            return GT_BAD_PARAM;
    }


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                bridgeRegs.bridgeGlobalConfigRegArray[0];

    /* Bridge Global Configuration Register0, field <IPv6NeighborSolicitedNodeCmd> */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 1, 2, value);
}

/*******************************************************************************
* cpssDxChBrgGenIpV6SolicitedNodeMcastPrefixSet
*
* DESCRIPTION:
*       Set IPv6 Solicited-Node Multicast Address Prefix Configuration and Mask
*       If the VLAN entry, indexed by the VID assigned to the packet,
*       IP Control to CPU enabled by cpssDxChBrgVlanIpCntlToCpuSet,
*       IPv6 Neighbor Solicitation messages may be trapped or mirrored
*       to the CPU, if the packets DIP matches the configured prefix
*       and according to the setting of IPv6 Neighbor Solicited Node Command.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*       addressPtr - (pointer to)IP address
*       maskPtr - (pointer to) mask of the address
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIpV6SolicitedNodeMcastPrefixSet
(
    IN GT_U8        devNum,
    IN GT_IPV6ADDR  *addressPtr,
    IN GT_IPV6ADDR  *maskPtr
)
{

    GT_U32 regAddr1, regAddr2;      /* register address                      */
    GT_U32 regNum;                  /* address and mask bits register number */
    GT_U32 regData;                 /* register's data                       */
    GT_U32 rc;                      /* function return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(addressPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

    /* IPv6 Solicited-Node Multicast Address Configuration Register0  */
    regAddr1 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                bridgeRegs.ipv6McastSolicitNodeAddrBase;

    /*IPv6 Solicited-Node Multicast Address Mask Register0*/
    regAddr2 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                bridgeRegs.ipv6McastSolicitNodeMaskBase;

    for(regNum = 0; regNum < 4; regNum++)
    {
        /* first register comprize LSB of IP address- bits 31:0
           second register comprize bits 63:32 and so on  */
        regData =  (addressPtr->arIP[((3 - regNum) * 4)]     << 24) |
                   (addressPtr->arIP[((3 - regNum) * 4) + 1] << 16) |
                   (addressPtr->arIP[((3 - regNum) * 4) + 2] << 8)  |
                   (addressPtr->arIP[((3 - regNum) * 4) + 3]);

        /* IPv6 Solicited-Node Multicast Address Configuration Register
           field  <IPv6SNMCAddr> */
        rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr1, regData);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* first register comprize LSB of IP mask - bits 31:0
           second register comprize bits 63:32 and so on  */
        regData =  (maskPtr->arIP[((3 - regNum) * 4)]     << 24) |
                   (maskPtr->arIP[((3 - regNum) * 4) + 1] << 16) |
                   (maskPtr->arIP[((3 - regNum) * 4) + 2] << 8)  |
                   (maskPtr->arIP[((3 - regNum) * 4) + 3]);

        /* IPv6 Solicited-Node Multicast Address Mask Register,
           field <IPv6SNMCAddrMask> */
        rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr2, regData);
        if (rc != GT_OK)
        {
            return rc;
        }

        regAddr1 += 0x4;
        regAddr2 += 0x4;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgGenRateLimitGlobalCfgSet
*
* DESCRIPTION:
*       Configures global ingress rate limit parameters - rate limiting mode,
*       Four configurable global time window periods are assigned to ports as
*       a function of their operating speed:
*       10 Gbps, 1 Gbps, 100 Mbps, and 10 Mbps.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion.
*
* INPUTS:
*       devNum             - device number
*       brgRateLimitPtr - pointer to global rate limit parameters structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   Supported windows for DxCh devices:
*       1000 Mbps: range - 256-16384 uSec    granularity - 256 uSec
*       100  Mbps: range - 256-131072 uSec   granularity - 256 uSec
*       10   Mbps: range - 256-1048570 uSec  granularity - 256 uSec
*       10   Gbps: range - 25.6-104857 uSec  granularity - 25.6 uSec
*       For DxCh3 and above: The granularity doesn't depend from PP core clock.
*       For DxCh1, DxCh2:
*       Time ranges and granularity specified for clock 200MHz(DxCh2 - 220 MHz)
*       for they changed linearly with the clock value.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenRateLimitGlobalCfgSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC     *brgRateLimitPtr
)
{
    GT_U32  regAddr;            /* register address */
    GT_STATUS rc;               /* function return code */
    GT_U32  value;              /* value to write into register */
    GT_U32  granularity;        /* rate limit window granularity */
    GT_U32  win1000Mbps = 0;    /* window time for 1000Gbps ports */
    GT_U32  win100Mbps = 0;     /* window time for 100Gbps ports */
    GT_U32  win10Mbps = 0;      /* window time for 10Mbps ports */
    GT_U32  win10Gbps = 0;      /* window time for 10Gbps ports */
    GT_U32  dropMode;           /* packet drop mode */
    GT_U32  countType;          /* rate limit drop counter type */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(brgRateLimitPtr);

    /*  The granularity doesn't depend from PP core clock for DxCh3 and above */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
    {
        granularity = 256;
    }
    else
    {
        granularity = (256 * PRV_CPSS_PP_MAC(devNum)->baseCoreClock) / PRV_CPSS_PP_MAC(devNum)->coreClock;

    }

    if (brgRateLimitPtr->win1000Mbps >= (2 * granularity))
    {
        win1000Mbps = (brgRateLimitPtr->win1000Mbps / granularity) - 1;
    }

    /* In Cheetah the maximum value of the 1000MWindow field, in the Ingress Rate
       limit Configuration Register0 is 31. (FEr#140) */
    if((PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,PRV_CPSS_DXCH_RATE_LIMIT_GIG_COUNTER_BITS_NUM_WA_E)
        == GT_TRUE) && (win1000Mbps > 0x1f))
    {
        return GT_OUT_OF_RANGE;
    }
    else
    {
        if (win1000Mbps > 0x3f)
        {
            return GT_OUT_OF_RANGE;
        }
    }


    if (brgRateLimitPtr->win100Mbps >= (2 * granularity))
    {
        win100Mbps = (brgRateLimitPtr->win100Mbps / granularity) - 1;
    }
    if (win100Mbps > 0x1ff)
    {
        return GT_OUT_OF_RANGE;
    }

    if(brgRateLimitPtr->win10Mbps >= (2 * granularity))
    {
        win10Mbps = (brgRateLimitPtr->win10Mbps / granularity) - 1;
    }
    if (win10Mbps > 0xfff)
    {
        return GT_OUT_OF_RANGE;
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_LION_RATE_LIMITER_10G_WINDOW_SIZE_WA_E))
    {
        if((brgRateLimitPtr->win10Gbps) >= (2 * granularity))
        {
            win10Gbps = (brgRateLimitPtr->win10Gbps / granularity) - 1;
        }
    }
    else
    {
        if((brgRateLimitPtr->win10Gbps) >= (2 * granularity/10))
        {
            win10Gbps = (brgRateLimitPtr->win10Gbps * 10 / granularity) - 1;
        }
    }
    if (win10Gbps > 0xfff)
    {
        return GT_OUT_OF_RANGE;
    }



    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                bridgeRegs.bridgeRateLimitConfigRegArray[0];

    value = (win100Mbps << 12) | (win1000Mbps << 21);

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_LION_RATE_LIMITER_10G_WINDOW_SIZE_WA_E))
    {
        value |= win10Gbps ;
    }
    else
    {
        value |= win10Mbps ;
    }

    /* Ingress Rate Limit Configuration Register0,
       fields <10MWindow>, <100MWindow>, <1000MWindow> */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 27, value);
    if (rc != GT_OK)
    {
        return GT_HW_ERROR;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                bridgeRegs.bridgeRateLimitConfigRegArray[1];

    switch(brgRateLimitPtr->dropMode)
    {
    case CPSS_DROP_MODE_HARD_E:
        dropMode = 1;
        break;
    case CPSS_DROP_MODE_SOFT_E:
        dropMode = 0;
        break;
    default:
        return GT_BAD_PARAM;
    }


    switch(brgRateLimitPtr->rMode)
    {
    case CPSS_RATE_LIMIT_PCKT_BASED_E:
        countType = 1;
        break;
    case CPSS_RATE_LIMIT_BYTE_BASED_E:
        countType = 0;
        break;
    default:
        return GT_BAD_PARAM;
    }


    value = win10Gbps | (countType << 12) | (dropMode << 13);

    /* Ingress Rate Limit Configuration Register1,
       fields <10GWindow>, <IngressRateLimitMode>, <IngressRateLimitDropMode> */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 14, value);
}

/*******************************************************************************
* cpssDxChBrgGenRateLimitGlobalCfgGet
*
* DESCRIPTION:
*     Get global ingress rate limit parameters
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       brgRateLimitPtr - pointer to global rate limit parameters structure
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DxCh3 and above: The granularity doesn't depend from PP core clock.
*       For DxCh1, DxCh2:
*       Time ranges and granularity specified for clock 200MHz(DxCh2 - 220 MHz)
*       for they changed linearly with the clock value.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenRateLimitGlobalCfgGet
(
    IN GT_U8                                devNum,
    OUT CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC    *brgRateLimitPtr
)
{
    GT_U32  regAddr;            /* register address */
    GT_STATUS rc;               /* function return code */
    GT_U32 value;               /* value to write into register */
    GT_U32  granularity;        /* rate limit window granularity */
    GT_U32  win1000Mbps;        /* window time for 1000Gbps ports */
    GT_U32  win100Mbps;         /* window time for 100Gbps ports */
    GT_U32  win10Mbps;          /* window time for 10Mbps ports */
    GT_U32  win10Gbps;          /* window time for 10Gbps ports */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(brgRateLimitPtr);


    /*  The granularity doesn't depend from PP core clock for DxCh3 and above */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
    {
        granularity = 256;
    }
    else
    {
        granularity = (256 * PRV_CPSS_PP_MAC(devNum)->baseCoreClock) / PRV_CPSS_PP_MAC(devNum)->coreClock;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                bridgeRegs.bridgeRateLimitConfigRegArray[0];

    /* Ingress Rate Limit Configuration Register0,
       fields <10MWindow>, <100MWindow>, <1000MWindow> */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 27, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    win10Mbps   = (value) & 0xfff;
    win100Mbps  = (value >> 12) & 0x1ff;
    win1000Mbps = (value >> 21) & 0x3f;

    brgRateLimitPtr->win10Mbps =   granularity + (win10Mbps * granularity);
    brgRateLimitPtr->win100Mbps =  granularity + (win100Mbps * granularity);
    brgRateLimitPtr->win1000Mbps = granularity + (win1000Mbps * granularity);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                bridgeRegs.bridgeRateLimitConfigRegArray[1];

    /* Ingress Rate Limit Configuration Register1,
       fields <10GWindow>, <IngressRateLimitMode>, <IngressRateLimitDropMode> */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 14, &value);

    win10Gbps = (value) & 0xfff;
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_LION_RATE_LIMITER_10G_WINDOW_SIZE_WA_E))
    {
        brgRateLimitPtr->win10Gbps = brgRateLimitPtr->win10Mbps;
    }
    else
    {
        brgRateLimitPtr->win10Gbps = (granularity/10) +
            (win10Gbps  * (granularity / 10));
    }

    brgRateLimitPtr->rMode = ((value >> 12) & 0x1) ?
        CPSS_RATE_LIMIT_PCKT_BASED_E : CPSS_RATE_LIMIT_BYTE_BASED_E;

    brgRateLimitPtr->dropMode = ((value >> 13) & 0x1) ?
        CPSS_DROP_MODE_HARD_E : CPSS_DROP_MODE_SOFT_E;


    return rc;
}

/*******************************************************************************
* cpssDxChBrgGenPortRateLimitSet
*
* DESCRIPTION:
*       Configures port ingress rate limit parameters
*       Each port maintains rate limits for unknown unicast packets,
*       known unicast packets, multicast packets and broadcast packets,
*       single configurable limit threshold value, and a single internal counter.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion.
*
* INPUTS:
*       devNum         - device number
*       port        - port number
*       portGfgPtr  - pointer to rate limit configuration for a port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_BAD_PARAM             - on wrong device number or port
*       GT_OUT_OF_RANGE          - on out of range rate limit values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortRateLimitSet
(
    IN GT_U8                                    devNum,
    IN GT_U8                                    port,
    IN CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC    *portGfgPtr
)
{
    GT_U32  regAddr;        /* register address */
    GT_STATUS rc;           /* function return code */
    GT_U32 value;           /* value to write into register */
    GT_U32 mask;            /* mask for register write */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_LION_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(portGfgPtr);

    if (portGfgPtr->rateLimit > 0xffff)
    {
        return GT_OUT_OF_RANGE;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.portControl[port];

    value =
        ((portGfgPtr->enableBc == GT_TRUE) ? 1 : 0);
    value |=
        ((portGfgPtr->enableUcKnown == GT_TRUE) ? 1 : 0) << 2;
    value |=
        ((portGfgPtr->enableUcUnk == GT_TRUE) ? 1 : 0) << 3;


    if (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum)) 
    {
        /* xCat2 */
        value |=
            ((portGfgPtr->enableMcReg == GT_TRUE) ? 1 : 0) << 1;
    }
    else
    {
        value |=
            ((portGfgPtr->enableMc == GT_TRUE) ? 1 : 0) << 1;
    }

    /* Ingress Port<n> Bridge Configuration Register0,
       fields <BcRateLimEn>, <McRateLimEn>, <UcRateLimEn>, <UnkUcRateLimEn> */
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 2, 4, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr += 0x10;
    value = portGfgPtr->rateLimit << 5;
    mask = 0x001FFFE0;

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum)) 
    {
        /* for xCat when rate limit is enabled on MC setting enable for unreg MC */
        /* is made explicitly */
        value |= ((portGfgPtr->enableMc == GT_TRUE) ? 1 : 0) << 24;
        mask |= ( 1 << 24 );
    }


    /* Ingress Port<n> Bridge Configuration Register1,
       fields <IngressLimit> and <UnregisteredMcRateLimEn> */
    return prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId,regAddr, 
                                                  mask, value);
}

/*******************************************************************************
* cpssDxChBrgGenPortRateLimitGet
*
* DESCRIPTION:
*     Get port ingress rate limit parameters
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion.
*
* INPUTS:
*       devNum         - device number
*       port        - port number
*
* OUTPUTS:
*       portGfgPtr  - pointer to rate limit configuration for a port
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_BAD_PARAM             - on wrong device or port number
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortRateLimitGet
(
    IN GT_U8                                    devNum,
    IN GT_U8                                    port,
    OUT CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC   *portGfgPtr
)
{
    GT_U32  regAddr;        /* register address */
    GT_U32 value;           /* value to read from register */
    GT_STATUS rc;           /* function return code */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_LION_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(portGfgPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.portControl[port];

    /* Ingress Port<n> Bridge Configuration Register0,
       fields <BcRateLimEn>, <McRateLimEn>, <UcRateLimEn>, <UnkUcRateLimEn> */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 2, 4, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    portGfgPtr->enableBc =
        (value & 0x1) ? GT_TRUE : GT_FALSE;
    portGfgPtr->enableUcKnown =
        ((value >> 2) & 0x1) ? GT_TRUE : GT_FALSE;
    portGfgPtr->enableUcUnk =
        ((value >> 3) & 0x1) ? GT_TRUE : GT_FALSE;

    if (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum)) 
    {
        portGfgPtr->enableMcReg =
            ((value >> 1) & 0x1) ? GT_TRUE : GT_FALSE;
    }
    else
    {
        portGfgPtr->enableMc =
            ((value >> 1) & 0x1) ? GT_TRUE : GT_FALSE;
    }

    regAddr += 0x10;
    /* Ingress Port<n> Bridge Configuration Register1,
       field <IngressLimit> */
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,regAddr, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    portGfgPtr->rateLimit = (value >> 5) & 0xFFFF;

    if (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum)) 
    {
        portGfgPtr->enableMc = BIT2BOOL_MAC((value >> 24) & 0x1);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgGenPortRateLimitTcpSynSet
*
* DESCRIPTION:
*     Enable or disable TCP SYN packets' rate limit for a port.
*     Value of rate limit is configured by cpssDxChBrgGenPortRateLimitSet and
*     cpssDxChBrgGenRateLimitGlobalCfgSet
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Lion.
*
* INPUTS:
*       devNum         - device number
*       port        - port number
*       enable      - enable/disable TCP SYN packets rate limit.
*                     GT_TRUE - enable rate limit, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_BAD_PARAM             - on wrong device or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortRateLimitTcpSynSet
(
    IN GT_U8      devNum,
    IN GT_U8      port,
    IN GT_BOOL    enable
)
{
    GT_U32  regAddr;            /* register address */
    GT_U32  value;              /* value to write from register */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_LION_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.portControl[port];

    value = BOOL2BIT_MAC(enable);
    regAddr += 0x10;

    /* Ingress Port<n> Bridge Configuration Register1, field <SYNRateLimEn> */
    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 23, 1, value);
}


/*******************************************************************************
* cpssDxChBrgGenPortRateLimitTcpSynGet
*
* DESCRIPTION:
*     Get the status of TCP SYN packets' rate limit for a port (enable/disable).
*     Value of rate limit can be read by cpssDxChBrgGenPortRateLimitGet and
*     cpssDxChBrgGenRateLimitGlobalCfgGet.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Lion.
*
* INPUTS:
*       devNum      - device number
*       port        - port number
*
* OUTPUTS:
*       enablePtr    - pointer to TCP SYN packets rate limit status.
*                      GT_TRUE - enable rate limit, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_BAD_PARAM             - on wrong device or port number
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortRateLimitTcpSynGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      port,
    OUT GT_BOOL    *enablePtr
)
{
    GT_U32  regAddr;            /* register address */
    GT_U32  value;              /* value to write from register */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_STATUS rc;           /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_LION_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);

    /* Ingress Port<n> Bridge Configuration Register1, field <SYNRateLimEn> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlanRegs.portPrvVlan[port];

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 23, 1, &value);
    if (rc != GT_OK) 
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgGenUdpBcDestPortCfgSet
*
* DESCRIPTION:
*     Configure UDP Broadcast Destination Port configuration table.
*     It's possible to configure several UDP destination ports with their Trap
*     or Mirror to CPU command and CPU code. See datasheet of device for maximal
*     number of UDP destination ports, which may be configured. The feature may
*     be enabled or disabled per VLAN by cpssDxChBrgVlanUdpBcPktsToCpuEnable
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
*
* INPUTS:
*       devNum      - device number
*       entryIndex  - index in the global UDP Broadcast Port Table, allows to
*                     application to configure up to 12 UDP ports (0..11)
*       udpPortNum  - UDP destination port number
*       cpuCode     - CPU code for packets trapped or mirrored by the feature.
*                     Acceptable CPU codes are: CPSS_NET_UDP_BC_MIRROR_TRAP0_E,
*                     CPSS_NET_UDP_BC_MIRROR_TRAP1_E, CPSS_NET_UDP_BC_MIRROR_TRAP2_E,
*                     CPSS_NET_UDP_BC_MIRROR_TRAP3_E
*       cmd         - packet command for UDP Broadcast packets
*                     either CPSS_DXCH_BRG_GEN_CNTRL_TRAP_E
*                     or CPSS_DXCH_BRG_GEN_CNTRL_MIRROR_E
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device or CPU code or cmd or
*                                  entryIndex > 11
*       GT_HW_ERROR              - failed to write to hw.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenUdpBcDestPortCfgSet
(
   IN GT_U8                     devNum,
   IN GT_U32                    entryIndex,
   IN GT_U16                    udpPortNum,
   IN CPSS_NET_RX_CPU_CODE_ENT  cpuCode,
   IN CPSS_PACKET_CMD_ENT       cmd
)
{
    GT_U32  value;                  /* value to write from register */
    GT_U32  regAddr;                /* register address */
    GT_U32  cpuCodeIndex;           /* cpu code index */
    GT_U32  hwPacketCmd;            /* packet command to be written to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    if(entryIndex >= 12)
    {
        return GT_BAD_PARAM;
    }

    switch(cpuCode)
    {
        case CPSS_NET_UDP_BC_MIRROR_TRAP0_E:
             cpuCodeIndex = 0;
             break;

        case CPSS_NET_UDP_BC_MIRROR_TRAP1_E:
             cpuCodeIndex = 1;
             break;

        case CPSS_NET_UDP_BC_MIRROR_TRAP2_E:
             cpuCodeIndex = 2;
             break;

        case CPSS_NET_UDP_BC_MIRROR_TRAP3_E:
             cpuCodeIndex = 3;
             break;

        default:
            return GT_BAD_PARAM;
    }

    switch(cmd)
    {

        case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
             hwPacketCmd = 0;
             break;

        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
             hwPacketCmd = 1;
             break;

        default:
            return GT_BAD_PARAM;
    }

    /*          Port        Valid        Packet Cmd            CPU Code Index */
    value = udpPortNum | (1 << 16) | (hwPacketCmd << 17) | (cpuCodeIndex << 18);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.udpBcDestPortConfigReg[entryIndex];

    /* UDP Broadcast Destination Port<n> Configuration Table,
       fields <UDPDestPort>, <UDPDestPortValid>, UDPDestPortCmd,
       <UDPDestPortCPUCodeIndex>*/
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 20, value);
}

/*******************************************************************************
* cpssDxChBrgGenUdpBcDestPortCfgGet
*
* DESCRIPTION:
*     Get UDP Broadcast Destination Port configuration table.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
*
* INPUTS:
*       devNum         - device number
*       entryIndex     - index in the global UDP Broadcast Port Table, allows to
*                        application to configure up to 12 UDP ports (0..11)
*
* OUTPUTS:
*       validPtr        - (pointer to) entry validity:
*                         GT_TRUE  - valid entry,
*                         GT_FALSE - invalid entry.
*       udpPortNumPtr   - (pointer to) UDP destination port number
*       cpuCodePtr      - (pointer to) CPU code for packets trapped or mirrored
*                         by the feature. Acceptable CPU codes are:
*                         CPSS_NET_UDP_BC_MIRROR_TRAP0_E,
*                         CPSS_NET_UDP_BC_MIRROR_TRAP1_E,
*                         CPSS_NET_UDP_BC_MIRROR_TRAP2_E,
*                         CPSS_NET_UDP_BC_MIRROR_TRAP3_E
*       cmdPtr          - (pointer to) packet command for UDP Broadcast packets
*                         either CPSS_DXCH_BRG_GEN_CNTRL_TRAP_E
*                         or CPSS_DXCH_BRG_GEN_CNTRL_MIRROR_E
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on error
*       GT_BAD_PARAM                - on wrong device or entryIndex > 11
*       GT_HW_ERROR                 - failed to read from hw.
*       GT_BAD_PTR                  - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenUdpBcDestPortCfgGet
(
   IN GT_U8                     devNum,
   IN GT_U32                    entryIndex,
   OUT GT_BOOL                  *validPtr,
   OUT GT_U16                   *udpPortNumPtr,
   OUT CPSS_NET_RX_CPU_CODE_ENT *cpuCodePtr,
   OUT CPSS_PACKET_CMD_ENT      *cmdPtr
)
{
    GT_STATUS rc;                   /* return code */
    GT_U32  regAddr;                /* register address */
    GT_U32  regData;                /* register data    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    CPSS_NULL_PTR_CHECK_MAC(udpPortNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(cpuCodePtr);
    CPSS_NULL_PTR_CHECK_MAC(cmdPtr);

    if(entryIndex >= 12)
    {
        return GT_BAD_PARAM;
    }

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.udpBcDestPortConfigReg[entryIndex];

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 20, &regData);
    if ( GT_OK != rc)
    {
        return rc;
    }

    *validPtr = BIT2BOOL_MAC( U32_GET_FIELD_MAC(regData, 16, 1) );

    *udpPortNumPtr = (GT_U16)U32_GET_FIELD_MAC(regData, 0, 16);

    switch (U32_GET_FIELD_MAC(regData, 18, 2))
    {
        case 0: *cpuCodePtr = CPSS_NET_UDP_BC_MIRROR_TRAP0_E;
                break;

        case 1: *cpuCodePtr = CPSS_NET_UDP_BC_MIRROR_TRAP1_E;
                break;

        case 2: *cpuCodePtr = CPSS_NET_UDP_BC_MIRROR_TRAP2_E;
                break;

        case 3: *cpuCodePtr = CPSS_NET_UDP_BC_MIRROR_TRAP3_E;
                break;

        default: return GT_FAIL;
                 break;
    }

    *cmdPtr = ( U32_GET_FIELD_MAC(regData, 17, 1) == 0 ) ?
                CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
                CPSS_PACKET_CMD_TRAP_TO_CPU_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgGenUdpBcDestPortCfgInvalidate
*
* DESCRIPTION:
*       Invalidate UDP Broadcast Destination Port configuration table.
*       All fields in entry will be reset.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
*
* INPUTS:
*       devNum        - device number
*       entryIndex - index in the global UDP Broadcast Port Table, allows to
*                 application to configure up to 12 UDP ports
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device or intryIndex > 11
*       GT_HW_ERROR              - failed to write to hw.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenUdpBcDestPortCfgInvalidate
(
   IN GT_U8                                     devNum,
   IN GT_U32                                    entryIndex
)
{
    GT_U32  value;                  /* value to write from register */
    GT_U32  regAddr;                /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    if(entryIndex > 11)
    {
        return GT_BAD_PARAM;
    }

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.udpBcDestPortConfigReg[entryIndex];

    value = 0;

    /* UDP Broadcast Destination Port<n> Configuration Table */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 20, value);
}

/*******************************************************************************
* cpssDxChBrgGenBpduTrapEnableSet
*
* DESCRIPTION:
*     Enable or disable trapping of BPDU Multicast MAC address 01-80-C2-00-00-00
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE - trap packets to CPU with BPDU Multicast MAC address
*                01-80-C2-00-00-00 on a ingress port whose span state
*                is not disabled.
*                GT_FALSE - packets with BPDU Multicast MAC address
*                01-80-C2-00-00-00 are not affected by ingress port's span state
*                and forwarded as standard Multicast frames
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenBpduTrapEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_U32  regAddr;                    /* register address */
    GT_U32  value;                      /* value to write from register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.bridgeGlobalConfigRegArray[0];

    value = BOOL2BIT_MAC(enable);

    /* Bridge Global Configuration Register0, field <CustomerBPDUTrapEn> */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 21, 1, value);
}

/*******************************************************************************
* cpssDxChBrgGenArpTrapEnable
*
* DESCRIPTION:
*       Enable/disable trapping ARP Broadcast packets to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       port   - physical port number
*       enable - GT_TRUE - ARP Broadcast packets received on this port
*                        are trapped to the CPU.
*                GT_FALSE - ARP Broadcast packets received on this port
*                        are not trapped to the CPU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenArpTrapEnable
(
    IN GT_U8        devNum,
    IN GT_U8        port,
    IN GT_BOOL      enable
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */
    GT_U32 portGroupId; /*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.portControl[port];

    value = BOOL2BIT_MAC(enable);

    /* Ingress Port<n> Bridge Configuration Register0, field <ARPBCTrapEn> */
    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 13, 1, value);
}

/*******************************************************************************
* cpssDxChBrgGenArpTrapEnableGet
*
* DESCRIPTION:
*       Get trapping ARP Broadcast packets to the CPU status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       port   - physical port number
*
* OUTPUTS:
*       enablePtr -  pointer to trapping ARP Broadcast packets status
*                GT_TRUE - ARP Broadcast packets received on this port
*                          are trapped to the CPU.
*                GT_FALSE - ARP Broadcast packets received on this port
*                        are not trapped to the CPU.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenArpTrapEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     port,
    OUT GT_BOOL   *enablePtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to read from register */
    GT_U32 portGroupId; /* the port group Id - support multi-port-groups device */
    GT_STATUS rc;       /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.portControl[port];

    /* Ingress Port<n> Bridge Configuration Register0, field <ARPBCTrapEn> */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 13, 1, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgGenDropIpMcModeSet
*
* DESCRIPTION:
*       Set drop mode for IP Multicast packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       mode         - drop mode
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant when Drop IP multicast is enabled.
*       See cpssDxChBrgGenDropIpMcEnable.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenDropIpMcModeSet
(
    IN GT_U8                    devNum,
    IN CPSS_DROP_MODE_TYPE_ENT  mode
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch (mode)
    {
        case CPSS_DROP_MODE_SOFT_E:
            value = 0;
            break;
        case CPSS_DROP_MODE_HARD_E:
            value = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.bridgeGlobalConfigRegArray[1];

    /* Bridge Global Configuration Register1, field <IP MC Drop Mode> */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 28, 1, value);


}

/*******************************************************************************
* cpssDxChBrgGenDropIpMcModeGet
*
* DESCRIPTION:
*       Get drop mode for IP Multicast packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number
*
*
* OUTPUTS:
*       modePtr       - pointer to drop mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant when Drop IP multicast is enabled.
*       See cpssDxChBrgGenDropIpMcEnable.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenDropIpMcModeGet
(
    IN  GT_U8                    devNum,
    OUT CPSS_DROP_MODE_TYPE_ENT  *modePtr
)
{
    GT_U32    regAddr;     /* register address */
    GT_U32    value;       /* value to read from register */
    GT_STATUS rc;          /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.bridgeGlobalConfigRegArray[1];

    /* Bridge Global Configuration Register1, field <IP MC Drop Mode> */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 28, 1, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *modePtr = (value == 0) ? CPSS_DROP_MODE_SOFT_E : CPSS_DROP_MODE_HARD_E;

    return GT_OK;

}


/*******************************************************************************
* cpssDxChBrgGenDropNonIpMcModeSet
*
* DESCRIPTION:
*       Set drop mode for non-IP Multicast packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       mode      - drop mode
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant when Drop non-IP multicast is enabled.
*       See cpssDxChBrgGenDropNonIpMcEnable.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenDropNonIpMcModeSet
(
    IN GT_U8                    devNum,
    IN CPSS_DROP_MODE_TYPE_ENT  mode
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch (mode)
    {
        case CPSS_DROP_MODE_SOFT_E:
            value = 0;
            break;
        case CPSS_DROP_MODE_HARD_E:
            value = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.bridgeGlobalConfigRegArray[1];

    /* Bridge Global Configuration Register1, field <Non-IP MC Drop Mode> */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 29, 1, value);


}

/*******************************************************************************
* cpssDxChBrgGenDropNonIpMcModeGet
*
* DESCRIPTION:
*       Get drop mode for non-IP Multicast packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*
*
* OUTPUTS:
*       modePtr    - pointer to drop mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant when Drop non-IP multicast is enabled.
*       See cpssDxChBrgGenDropNonIpMcEnable.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenDropNonIpMcModeGet
(
    IN  GT_U8                    devNum,
    OUT CPSS_DROP_MODE_TYPE_ENT  *modePtr
)
{
    GT_U32    regAddr;     /* register address */
    GT_U32    value;       /* value to read from register */
    GT_STATUS rc;          /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.bridgeGlobalConfigRegArray[1];

    /* Bridge Global Configuration Register1, field <Non-IP MC Drop Mode> */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 29, 1, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *modePtr = (value == 0) ? CPSS_DROP_MODE_SOFT_E : CPSS_DROP_MODE_HARD_E;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgGenCfiRelayEnableSet
*
* DESCRIPTION:
*       Enable / Disable CFI bit relay.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE -  Tagged packets transmitted from the device have
*                            have their CFI bit set according to received
*                            packet CFI bit if the received packet was tagged
*                            or set to 0 if the received packet is untagged.
*               - GT_FALSE - Tagged packets transmitted from the device
*                            have their CFI bit set to 0.
*
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenCfiRelayEnableSet
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */
    GT_U32 offset;      /* field offset */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
        offset = 28;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        offset = 22;
    }

    value = BOOL2BIT_MAC(enable);

    /* Enable / Disable CFI bit relay */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, offset, 1, value);

}

/*******************************************************************************
* cpssDxChBrgGenCfiRelayEnableGet
*
* DESCRIPTION:
*       Get CFI bit relay status.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  -  pointer to CFI bit relay status.
*                  - GT_TRUE -  Tagged packets transmitted from the device have
*                               have their CFI bit set according to received
*                               packet CFI bit if the received packet was tagged
*                               or set to 0 if the received packet is untagged.
*                  - GT_FALSE - Tagged packets transmitted from the device
*                               have their CFI bit set to 0.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenCfiRelayEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_U32    regAddr;     /* register address */
    GT_U32    value;       /* value to read from register */
    GT_STATUS rc;          /* return status */
    GT_U32    offset;      /* field offset */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
        offset = 28;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        offset = 22;
    }

    /* Get CFI bit relay status */
    rc =  prvCpssDrvHwPpGetRegField(devNum, regAddr, offset, 1, &value);

    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgGenPortIeeeReservedMcastProfileIndexSet
*
* DESCRIPTION:
*       Select the IEEE Reserved Multicast profile (table) associated with port
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number
*       port         - port number (including CPU port)
*       profileIndex - profile index (0..3). The parameter defines profile (table
*                      number) for the 256 protocols.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port
*       GT_OUT_OF_RANGE          - for profileIndex
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortIeeeReservedMcastProfileIndexSet
(
    IN GT_U8                devNum,
    IN GT_U8                port,
    IN GT_U32               profileIndex
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 field;       /* register field offset */
    GT_U32 portGroupId; /*the port group Id - support multi-port-groups device */
    GT_U8  localPort;   /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,port);

    if (profileIndex > 3)
    {
        return GT_OUT_OF_RANGE;
    }

    if (localPort < 16)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        bridgeRegs.ieeeReservedMcastProfileSelect0;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        bridgeRegs.ieeeReservedMcastProfileSelect1;
    }

    if (localPort == CPSS_CPU_PORT_NUM_CNS)
    {
        field = 30;
    }
    else
    {
        field = 2 * (localPort % 16);
    }

    /* set IEEE table select0/1 Register */
    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, field, 2, profileIndex);
}

/*******************************************************************************
* cpssDxChBrgGenPortIeeeReservedMcastProfileIndexGet
*
* DESCRIPTION:
*       Get the IEEE Reserved Multicast profile (table) associated with port.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum             - device number
*       port            - port number (including CPU port)
*
* OUTPUTS:
*       profileIndexPtr - (pointer to) profile index (0..3). The parameter
*                         defines profile (table number) for the 256 protocols.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or profileId
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortIeeeReservedMcastProfileIndexGet
(
    IN GT_U8                devNum,
    IN GT_U8                port,
    OUT GT_U32              *profileIndexPtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 field;       /* register field offset */
    GT_U32 portGroupId; /*the port group Id - support multi-port-groups device */
    GT_U8  localPort;   /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(profileIndexPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,port);

    if (localPort < 16)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        bridgeRegs.ieeeReservedMcastProfileSelect0;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        bridgeRegs.ieeeReservedMcastProfileSelect1;
    }

    if (localPort == CPSS_CPU_PORT_NUM_CNS)
    {
        field = 30;
    }
    else
    {
        field = 2 * (localPort % 16);
    }

    /* get IEEE table select0/1 Register */
    return prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, field, 2, profileIndexPtr);
}

/*******************************************************************************
* cpssDxChBrgGenPortOversizeUntaggedPacketsFilterEnableSet
*
* DESCRIPTION:
*     Enable/disable dropping all ingress untagged packets received
*     by specified port with byte count more then 1518 bytes.
*     This feature is additional to cpssDxChPortMruSet that sets
*     MRU at the MAC level and is not aware of packet tagging.
*     If MAC level MRU is 1522, untagged packets with size 1519..1522 bytes
*     are not dropped at the MAC level as needed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*        devNum    - device number
*        port   - port number (including CPU port)
*        enable - GT_FALSE -
*                   untagged packets with size more then 1518 bytes not dropped
*                 GT_TRUE -
*                   untagged packets with size more then 1518 bytes dropped
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortOversizeUntaggedPacketsFilterEnableSet
(
    IN GT_U8      devNum,
    IN GT_U8      port,
    IN GT_BOOL    enable
)
{
    GT_U32 bitNum; /* number of "OverSizeUntaggedPacketsFilterEn" bit */
                   /* in VLAN QoS entry                               */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above devices */
        bitNum = 9;
    }
    else
    {
        /* DxCh3 devices */
        bitNum = 14;
    }

    return prvCpssDxChWriteTableEntryField(
        devNum, PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E, port,
        0 /*fieldWordNum*/,   bitNum /*fieldOffset*/,
        1 /*fieldLength*/,    ((enable == GT_FALSE) ? 0 : 1) /*fieldValue*/);

}

/*******************************************************************************
* cpssDxChBrgGenPortOversizeUntaggedPacketsFilterEnableGet
*
* DESCRIPTION:
*     Get Enable/disable status of dropping all ingress untagged packets received
*     by specified port with byte count more then 1518 bytes.
*     This feature is additional to cpssDxChPortMruSet that sets
*     MRU at the MAC level and is not aware of packet tagging.
*     If MAC level MRU is 1522, untagged packets with size 1519..1522 bytes
*     are not dropped at the MAC level as needed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*        devNum    - device number
*        port   - port number (including CPU port)
*
* OUTPUTS:
*        enablePtr - (pointer to)
*                   GT_FALSE -
*                      untagged packets with size more then 1518 bytes not dropped
*                   GT_TRUE -
*                      untagged packets with size more then 1518 bytes dropped
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortOversizeUntaggedPacketsFilterEnableGet
(
    IN GT_U8       devNum,
    IN GT_U8       port,
    OUT GT_BOOL    *enablePtr
)
{
    GT_U32    hwValue; /* harware value                                   */
    GT_STATUS rc;      /* return code                                     */
    GT_U32    bitNum;  /* number of "OverSizeUntaggedPacketsFilterEn" bit */
                       /* in VLAN QoS entry                               */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above devices */
        bitNum = 9;
    }
    else
    {
        /* DxCh3 devices */
        bitNum = 14;
    }

    rc = prvCpssDxChReadTableEntryField(
        devNum, PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E, port,
        0 /*fieldWordNum*/,   bitNum /*fieldOffset*/,
        1 /*fieldLength*/,    &hwValue /*fieldValue*/);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (hwValue == 0) ? GT_FALSE : GT_TRUE;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgGenRateLimitSpeedCfgEnableSet
*
* DESCRIPTION:
*     Enable/Disable the port speed configuration that will be used by the
*     Bridge ingress rate limiter, regardless of the actual speed in which
*     a port actually works.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*        devNum    - device number
*        enable - GT_FALSE -  Port speed for bridge rate limit
*                             calculations is according to the actual
*                             port speed.
*                  GT_TRUE - Port speed for bridge rate limit
*                            calculations is taken from a per-port configuration
*                            cpssDxChBrgGenPortRateLimitSpeedGranularitySet()
*                            and NOT from the port actual speed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In xCat devices the feature is not relevant for Flex link ports.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenRateLimitSpeedCfgEnableSet
(
    IN GT_U8      devNum,
    IN GT_BOOL    enable
)
{
    GT_U32  regAddr;  /* register address */
    GT_U32  regValue; /* register value */
    GT_U32  bitNum;   /* bit number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    if(PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat2 and above devices */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.stackSpeedGranularity;
        bitNum  = 31;
    }
    else
    {
        /* xCat devices */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.dft1Reg;
        bitNum  = 25;
    }

    regValue = BOOL2BIT_MAC(enable);

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, bitNum, 1, regValue);
}

/*******************************************************************************
* cpssDxChBrgGenRateLimitSpeedCfgEnableGet
*
* DESCRIPTION:
*     Get the status of the port speed configuration (Enabled/Disabled) that
*     will be used by the Bridge ingress rate limiter, regardless of the actual
*     speed in which a port actually works.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*        devNum    - device number
*
* OUTPUTS:
*        enablePtr - GT_FALSE -  Port speed for bridge rate limit
*                                calculations is according to the actual
*                                port speed.
*                    GT_TRUE -   Port speed for bridge rate limit
*                                calculations is taken from a per-port configuration
*                                cpssDxChBrgGenPortRateLimitSpeedGranularitySet()
*                                and NOT from the port actual speed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In xCat devices the feature is not relevant for Flex link ports.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenRateLimitSpeedCfgEnableGet
(
    IN  GT_U8      devNum,
    OUT GT_BOOL    *enablePtr
)
{

    GT_U32  regAddr;  /* register address */
    GT_U32  regValue; /* register value */
    GT_U32  bitNum;   /* bit number */
    GT_STATUS rc;     /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat2 and above devices */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.stackSpeedGranularity;
        bitNum  = 31;
    }
    else
    {
        /* xCat devices */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.dft1Reg;
        bitNum  = 25;
    }

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, bitNum, 1, &regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(regValue);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgGenPortRateLimitSpeedGranularitySet
*
* DESCRIPTION:
*     Configure 100Mbps/10Mbps/1Gbps/10Gbps port speed for the use of the bridge
*     ingress rate limit calculations.
*     If port speed for Bridge ingress rate limit calculations is enabled,
*     cpssDxChBrgGenRateLimitSpeedCfgEnableSet() API, then this configuration
*     is used.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*        devNum             - device number
*        portNum            - port number
*                             xCat devices support only ports 0..23
*        speedGranularity   - Port speed granularity
*                            (10Mbps/100Mbps/1Gbps/10Gbps) that
*                             will be used for bridge ingress rate limit
*                             calculations.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portNum or speedGranularity
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In xCat devices the feature is not relevant for Flex link ports.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortRateLimitSpeedGranularitySet
(
    IN GT_U8                    devNum,
    IN GT_U8                    portNum,
    IN CPSS_PORT_SPEED_ENT      speedGranularity
)
{
    GT_U32  regAddrMii;    /* MII speed register address */
    GT_U32  regAddrGmii;   /* GMII speed register address */
    GT_U32  regAddrStack;  /* Stack port speed register address */
    GT_U32  stackValue;    /* stack port register value */
    GT_U32  bitOffset;     /* bit offset inside register */
    GT_U32  miiSpeed;      /* MII speed (10/100) selection value */
    GT_U32  gmiiSpeed;     /* GMII speed (1000) selection value */
    GT_U32  xgSpeed;       /* XG speed (10 000) selection value */
    GT_STATUS rc;          /* return code  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

    if(PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat2 and above devices */
        regAddrMii = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.miiSpeedGranularity;
        regAddrGmii = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.gmiiSpeedGranularity;

        /* MII bit offset */
        bitOffset = portNum;
    }
    else
    {
        /* xCat supports only first 24 ports */
        if (portNum >= PRV_DXCH_FIRST_XG_CAPABLE_PORT_CNS)
        {
            return GT_BAD_PARAM;
        }

        /* xCat devices */
        regAddrMii = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.dft1Reg;
        regAddrGmii = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.mllMetalFix;

        /* MII bit offset */
        bitOffset = (portNum + 1);
    }

    switch (speedGranularity)
    {
        /* 10 Mbps */
        case CPSS_PORT_SPEED_10_E:
            miiSpeed  = 0;
            gmiiSpeed = 0;
            xgSpeed   = 1;
       break;

        /* 100 Mbps*/
        case CPSS_PORT_SPEED_100_E:
            miiSpeed  = 1;
            gmiiSpeed = 0;
            xgSpeed   = 1;
            break;

        /* 1 Gbps*/
        case CPSS_PORT_SPEED_1000_E:
            miiSpeed  = 0; /* don't care in 1000 mode */
            gmiiSpeed = 1;
            xgSpeed   = 1;
            break;

        /* 10 Gbps*/
        case CPSS_PORT_SPEED_10000_E:
            /* 10G window supported only for stack ports 24..27 */
            if (portNum < PRV_DXCH_FIRST_XG_CAPABLE_PORT_CNS)
            {
                return GT_BAD_PARAM;
            }
            miiSpeed  = 0; /* don't care in 10000 mode */
            gmiiSpeed = 0; /* don't care in 10000 mode */
            xgSpeed   = 0;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* network and stacking ports have different registers for configuration */
    if (portNum < PRV_DXCH_FIRST_XG_CAPABLE_PORT_CNS)
    {
        /* skip MII speed configuration for 1000.
           10000 speed is ignored because it's not supported
           for network ports. */
        if (gmiiSpeed == 0)
        {
            /* Configure MII speed select register */
            rc = prvCpssDrvHwPpSetRegField(devNum, regAddrMii, bitOffset, 1, miiSpeed);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* Configure GMII speed select register */
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddrGmii, portNum, 1, gmiiSpeed);
    }
    else
    {  /* stack port configuration */
        regAddrStack = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.stackSpeedGranularity;
        /* the stack port has 3 bits for speed selection:
            - bit#0 is 1000 speed selection.
            - bit#1 is 10/100 speed selection.
            - bit#2 is 10 000 speed selection. */
        stackValue = gmiiSpeed | (miiSpeed << 1) | (xgSpeed << 2);
        bitOffset = (portNum - PRV_DXCH_FIRST_XG_CAPABLE_PORT_CNS) * 3;

        /* Configure Stack port speed select register */
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddrStack, bitOffset, 3, stackValue);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChBrgGenPortRateLimitSpeedGranularityGet
*
* DESCRIPTION:
*     Get port speed configuration. This configuration is used for bridge
*     ingress rate limit calculations.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*        devNum             - device number
*        portNum            - port number
*                             xCat devices support only ports 0..23
*
* OUTPUTS:
*        speedGranularityPtr  - (pointer to) port speed granularity
*                               (10Mbps/100Mbps/1Gbps/10Gbps) that will be
*                               used for bridge ingress rate limit calculations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In xCat devices the feature is not relevant for Flex link ports.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortRateLimitSpeedGranularityGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    OUT CPSS_PORT_SPEED_ENT         *speedGranularityPtr
)
{
    GT_U32  regValue;     /* register value */
    GT_U32  bitOffset;    /* bit offset inside MII register */
    GT_U32  regAddrMii;   /* MII speed register address */
    GT_U32  regAddrGmii;  /* GMII speed register address */
    GT_U32  regAddrStack; /* Stack port speed register address */
    GT_STATUS rc;         /* return code  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(speedGranularityPtr);

    if(PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat2 and above devices */
        regAddrMii = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.miiSpeedGranularity;
        regAddrGmii = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.gmiiSpeedGranularity;

        /* MII bit offset */
        bitOffset = portNum;
    }
    else
    {
        /* xCat supports only first 24 ports */
        if (portNum >= PRV_DXCH_FIRST_XG_CAPABLE_PORT_CNS)
        {
            return GT_BAD_PARAM;
        }

        /* xCat devices */
        regAddrMii = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.dft1Reg;
        regAddrGmii = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.mllMetalFix;

        /* MII bit offset */
        bitOffset = (portNum + 1);
    }

    /* network and stacking ports have different registers for configuration */
    if (portNum < PRV_DXCH_FIRST_XG_CAPABLE_PORT_CNS)
    {
        /* read GMII register first. */
        rc = prvCpssDrvHwPpGetRegField(devNum, regAddrGmii, portNum, 1, &regValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (regValue == 1)
        {
            *speedGranularityPtr = CPSS_PORT_SPEED_1000_E;
            return GT_OK;
        }

        /* read MII speed register */
        rc = prvCpssDrvHwPpGetRegField(devNum, regAddrMii, bitOffset, 1, &regValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        *speedGranularityPtr = (regValue == 1) ? CPSS_PORT_SPEED_100_E:
                                         CPSS_PORT_SPEED_10_E;
    }
    else
    {  /* stack port  */
        regAddrStack = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.stackSpeedGranularity;
        bitOffset = (portNum - PRV_DXCH_FIRST_XG_CAPABLE_PORT_CNS) * 3;

        /* read Stack port speed select register */
        rc = prvCpssDrvHwPpGetRegField(devNum, regAddrStack, bitOffset, 3, &regValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* the stack port has 3 bits for speed selection:
            - bit#0 is 1000 speed selection.
            - bit#1 is 10/100 speed selection.
            - bit#2 is 10 000 speed selection.*/

        /* check XG bit */
        if ((regValue & 0x4) == 0)
        {
            *speedGranularityPtr = CPSS_PORT_SPEED_10000_E;
        }
        /* check GMII bit */
        else if ((regValue & 0x1) == 1)
        {
            *speedGranularityPtr = CPSS_PORT_SPEED_1000_E;
        }
        else
        {
            /* the bit#2 is 1 and bit#0 is 0.
              the regValue may be 6 for 100 speed and 4 for 10 speed */
            *speedGranularityPtr = (regValue != 4) ? CPSS_PORT_SPEED_100_E:
                                             CPSS_PORT_SPEED_10_E;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgGenExcludeLinkLocalMcFromUnregMcFilterEnableSet
*
* DESCRIPTION:
*     Enable / Disable excluding unregistered IPv4 / IPv6 Link Local Multicast
*     packets from the per VLAN Unregistered IPv4 / IPv6 Multicast command.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*        devNum         - device number
*        protocolStack  - IPv4 or IPv6 protocol version
*        enable         - GT_TRUE -
*                               exclude unregistered IPv4 / IPv6
*                               Link Local Multicast packets from the per VLAN
*                               Unregistered IPv4 / IPv6 Multicast command
*                        - GT_FALSE -
*                               Unregistered link Local Multicast packets
*                               are handled according to the
*                               Unregistered IPv4 / IPv6 Multicast command.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, protocolStack
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenExcludeLinkLocalMcFromUnregMcFilterEnableSet
(
    IN GT_U8                        devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN GT_BOOL                      enable
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  bitOffset;  /* bit offset inside register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);


    switch (protocolStack)
    {
        case CPSS_IP_PROTOCOL_IPV4_E:
            bitOffset = 6;
            break;
        case CPSS_IP_PROTOCOL_IPV6_E:
            bitOffset = 7;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                bridgeRegs.bridgeGlobalConfigRegArray[2];

    regValue = (enable == GT_TRUE) ? 1 : 0;

    /* Bridge Global Configuration Register2,
     field <ExcludeLLMCFromUnregIPv4MCFilter / ExcludeLLMCFromUnregIPv6MCFilter> */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, bitOffset, 1, regValue);

}

/*******************************************************************************
* cpssDxChBrgGenExcludeLinkLocalMcFromUnregMcFilterEnableGet
*
* DESCRIPTION:
*     Get enable / disable status for excluding unregistered IPv4 / IPv6
*     Link Local Multicast packets from the per VLAN
*     Unregistered IPv4 / IPv6 Multicast command.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*        devNum          - device number
*        protocolStack   - IPv4 or IPv6 protocol version
*
* OUTPUTS:
*        enablePtr       - pointer to status for excluding unregistered
*                          IPv4 / IPv6 Link Local Multicast packets from the
*                          per VLAN Unregistered IPv4 / IPv6 Multicast command.
*                        - GT_TRUE -
*                               exclude unregistered IPv4 / IPv6
*                               Link Local Multicast packets from the per VLAN
*                               Unregistered IPv4 / IPv6 Multicast command
*                        - GT_FALSE -
*                               Unregistered link Local Multicast packets
*                               are handled according to the
*                               Unregistered IPv4 / IPv6 Multicast command.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, protocolStack
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenExcludeLinkLocalMcFromUnregMcFilterEnableGet
(
    IN  GT_U8                        devNum,
    IN  CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    OUT GT_BOOL                      *enablePtr
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  bitOffset;  /* bit offset inside register */
    GT_STATUS rc;       /* return code  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    switch (protocolStack)
    {
        case CPSS_IP_PROTOCOL_IPV4_E:
            bitOffset = 6;
            break;
        case CPSS_IP_PROTOCOL_IPV6_E:
            bitOffset = 7;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                bridgeRegs.bridgeGlobalConfigRegArray[2];

    /* Bridge Global Configuration Register2,
     field <ExcludeLLMCFromUnregIPv4MCFilter / ExcludeLLMCFromUnregIPv6MCFilter> */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, bitOffset, 1, &regValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (regValue == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}


/*******************************************************************************
* cpssDxChBrgGenPortIeeeReservedMcastLearningEnableSet
*
* DESCRIPTION:
*     Enable/Disable MAC SA learning on reserved IEEE MC packets, that are
*     trapped to CPU.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*        devNum         - device number
*        portNum        - port number 
*        enable         - GT_TRUE - learning is performed also for IEEE MC
*                               trapped packets.
*                       - GT_FALSE - No learning is performed for IEEE MC
*                               trapped packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortIeeeReservedMcastLearningEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
{
    GT_U32  regAddr;            /* register address */
    GT_U32  value;              /* value to write from register */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | 
                                          CPSS_LION_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlanRegs.portPrvVlan[portNum];

    value = BOOL2BIT_MAC(enable);

    /* Ingress Port<n> Bridge Configuration Register1, 
       field <EnLearnOnTrapIEEEReservedMC> */
    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 
                                              25, 1, value);
}

/*******************************************************************************
* cpssDxChBrgGenPortIeeeReservedMcastLearningEnableGet
*
* DESCRIPTION:
*     Get status of MAC SA learning on reserved IEEE MC packets, that are
*     trapped to CPU (enabled/disabled).
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*        devNum         - device number
*        portNum        - port number 
*
* OUTPUTS:
*        enablePtr      pointer to status of MAC SA learning on reserved IEEE
*                       MC packets trapped packets:
*                       - GT_TRUE - learning is performed also for IEEE MC
*                               trapped packets.
*                       - GT_FALSE - No learning is performed for IEEE MC
*                               trapped packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortIeeeReservedMcastLearningEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32  regAddr;            /* register address */
    GT_U32  value;              /* value to write from register */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_STATUS rc;               /* return code      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | 
                                          CPSS_LION_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlanRegs.portPrvVlan[portNum];

    /* Ingress Port<n> Bridge Configuration Register1, 
       field <EnLearnOnTrapIEEEReservedMC> */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 25, 1, &value);
    if (rc != GT_OK) 
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}


#ifndef __AX_PLATFORM__
/*******************************************************************************
* cpssDxChBrgGenVlanValidCheckEnable
*
* DESCRIPTION:
*       VLAN ingress check if packet assigned vlan is valid or not.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev - device number
*		 enable - enable or not
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on bad device number or control packet command
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenVlanValidCheckEnable
(
    IN GT_U8               dev,
    IN GT_BOOL			  enable
)
{
    GT_U32 regAddr;     /* register address */
    unsigned int value; /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->
                                    bridgeRegs.bridgeGlobalConfigRegArray[0];

    value = BOOL2BIT(enable);

    /* Bridge Global Configuration Register0, field <VlanValidCheckEn> */
    return prvCpssDrvHwPpSetRegField(dev, regAddr, 4, 1, value);
}

/*******************************************************************************
* cpssDxChBrgGenArpSaCheckEnable
*
* DESCRIPTION:
*       Arp Sa check if MAC SA is equal to the ARP payload data 
*		contains a field called <source hardware address> or not.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev - device number
*		 enable - enable or not
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on bad device number or control packet command
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenArpSaCheckEnable
(
    IN GT_U8               dev,
    IN GT_BOOL			  enable
)
{
    GT_U32 regAddr;     /* register address */
    unsigned int value; /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->
                                    bridgeRegs.bridgeGlobalConfigRegArray[0];

    value = BOOL2BIT(enable);

    /* Bridge Global Configuration Register0, field <ARPSAMismatchDropEn> */
    return prvCpssDrvHwPpSetRegField(dev, regAddr, 16, 1, value);

}

/*******************************************************************************
* cpssDxChBrgGenIPv4LinkLocalMirrorEnable
*
* DESCRIPTION:
*       Global enable to mirror IPv4 link local packets with DIP = 224.0.0.x, 
*		  according to the setting of IPv4 Multicast Link-Local Configuration Register<n>
*		
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev - device number
*		 enable - enable or not
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on bad device number or control packet command
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIPv4LinkLocalMirrorEnable
(
    IN GT_U8               dev,
    IN GT_BOOL			  enable
)
{
    GT_U32 regAddr;     /* register address */
    unsigned int value; /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->
                                    bridgeRegs.bridgeGlobalConfigRegArray[0];

    value = BOOL2BIT(enable);

    /* Bridge Global Configuration Register0, field <IPv4LinkLocalMirrorEn> */
    return prvCpssDrvHwPpSetRegField(dev, regAddr, 23, 1, value);

}

#endif


