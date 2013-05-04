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
* cpssDXCHBrgVlan.c
*
* DESCRIPTION:
*       CPSS DxCh VLAN facility implementation
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.4 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>

/* max index of entry that may be configured to support
   port protocol based VLANs and QoS */
#define PRV_CPSS_DXCH_PORT_PROT_ENTRY_NUM_MAX_CNS  (7)

/* for xCat: max index of entry that may be configured
   to support port protocol based VLANs and QoS */
#define PRV_CPSS_DXCH_XCAT_PORT_PROT_ENTRY_NUM_MAX_CNS  (11)

/*  check Port Protocol range, only 8 and 12(for xCat and above)
    protocols are supported */
#define PRV_CPSS_DXCH_BRG_PORT_PROTOCOL_CHECK_MAC(_devNum, _entryNum)    \
    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(_devNum))              \
    {                                                                    \
        if((_entryNum) > PRV_CPSS_DXCH_XCAT_PORT_PROT_ENTRY_NUM_MAX_CNS) \
            return GT_BAD_PARAM;                                         \
    }                                                                    \
    else                                                                 \
    {                                                                    \
        if((_entryNum) > PRV_CPSS_DXCH_PORT_PROT_ENTRY_NUM_MAX_CNS)      \
            return GT_BAD_PARAM;                                         \
    }

/* max number of MRU indexes */
#define PRV_CPSS_DXCH_BRG_MRU_INDEX_MAX_CNS     (7)

/* check MRU indx range */
#define PRV_CPSS_DXCH_BRG_MRU_INDEX_CHECK_MAC(mruInd)   \
    if ((mruInd) > PRV_CPSS_DXCH_BRG_MRU_INDEX_MAX_CNS) \
        return GT_BAD_PARAM;

/* check and convert IPM Bridging mode */
#define PRV_DXCH_BRG_VLAN_IPM_MODE_CONVERT_MAC(_ipmBrgMode, _hwValue) \
    switch(_ipmBrgMode)                                           \
    {                                                             \
        case CPSS_BRG_IPM_SGV_E: _hwValue = 0; break;             \
        case CPSS_BRG_IPM_GV_E:  _hwValue = 1; break;             \
        default: return GT_BAD_PARAM;                             \
    }

/* Converts flood VIDX mode to hardware value */
#define PRV_CPSS_DXCH_CONVERT_FLOOD_VIDX_MODE_TO_HW_VAL_MAC(_val, _cmd) \
    switch (_cmd)                                                       \
    {                                                                   \
        case CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E:             \
            _val = 0;                                                   \
            break;                                                      \
        case CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E:  \
            _val = 1;                                                   \
            break;                                                      \
        default:                                                        \
            return GT_BAD_PARAM;                                        \
    }

#define PRV_CPSS_DXCH_CONVERT_PORT_ISOLATION_MODE_TO_HW_VAL_MAC(_val, _cmd)  \
    switch (_cmd)                                                            \
    {                                                                        \
        case CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E:                \
            _val = 0;                                                        \
            break;                                                           \
        case CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_CMD_E:                     \
            _val = 1;                                                        \
            break;                                                           \
        case CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L3_CMD_E:                     \
            _val = 2;                                                        \
            break;                                                           \
        case CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_L3_CMD_E:                  \
            _val = 3;                                                        \
            break;                                                           \
        default:                                                             \
            return GT_BAD_PARAM;                                             \
    }

#define PRV_CPSS_DXCH_CONVERT_VLAN_TAG_MODE_TO_HW_VAL_MAC(_val, _cmd)       \
        switch(_cmd)                                                        \
        {                                                                   \
            case CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E                :    \
                _val = 0;                                                   \
                break;                                                      \
            case CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E                    :    \
                _val = 1;                                                   \
                break;                                                      \
            case CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E                    :    \
                _val = 2;                                                   \
                break;                                                      \
            case CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E   :    \
                _val = 3;                                                   \
                break;                                                      \
            case CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E   :    \
                _val = 4;                                                   \
                break;                                                      \
            case CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E               :    \
                _val = 5;                                                   \
                break;                                                      \
            case CPSS_DXCH_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E           :    \
                _val = 6;                                                   \
                break;                                                      \
            default:                                                        \
                return GT_BAD_PARAM;                                        \
        }

#define PRV_CPSS_DXCH_CONVERT_HW_VAL_TO_VLAN_TAG_MODE_MAC(_val, _cmd)       \
            switch(_val)                                                    \
            {                                                               \
                case 0:                                                     \
                    _cmd = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;          \
                    break;                                                  \
                case 1:                                                     \
                    _cmd = CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E;              \
                    break;                                                  \
                case 2:                                                     \
                    _cmd = CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E;              \
                    break;                                                  \
                case 3:                                                     \
                    _cmd = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E;\
                    break;                                                  \
                case 4:                                                     \
                    _cmd = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E;\
                    break;                                                  \
                case 5:                                                     \
                    _cmd = CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E;         \
                    break;                                                  \
                case 6:                                                     \
                    _cmd = CPSS_DXCH_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E;     \
                    break;                                                  \
                default:                                                    \
                    _cmd = (CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT)_val;       \
                    break;                                                  \
            }

/* maximal number of words in the DxCh and above vlan entry */
#define PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS      (6)

/* number of words in the Lion and above ingress vlan entry */
#define VLAN_INGRESS_ENTRY_WORDS_NUM_CNS     (5)
/* number of words in the Lion and above Egress vlan entry */
#define VLAN_EGRESS_ENTRY_WORDS_NUM_CNS      (9)

static GT_STATUS vlanEntrySplitTableWrite
(
    IN  GT_U8                                 devNum,
    IN  GT_U16                                vlanId,
    IN  CPSS_PORTS_BMP_STC                    *portsMembersPtr,
    IN  CPSS_PORTS_BMP_STC                    *portsTaggingPtr,
    IN  CPSS_DXCH_BRG_VLAN_INFO_STC           *vlanInfoPtr,
    IN  CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr
);

static GT_STATUS vlanEntrySplitTableRead
(
    IN  GT_U8                                devNum,
    IN  GT_U16                               vlanId,
    OUT CPSS_PORTS_BMP_STC                   *portsMembersPtr,
    OUT CPSS_PORTS_BMP_STC                   *portsTaggingPtr,
    OUT CPSS_DXCH_BRG_VLAN_INFO_STC          *vlanInfoPtr,
    OUT GT_BOOL                              *isValidPtr,
    OUT CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC *portsTaggingCmdPtr
);

/*******************************************************************************
* cpssDxChBrgVlanInit
*
* DESCRIPTION:
*       Initialize VLAN for specific device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on system init error.
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanInit
(
    IN GT_U8    devNum
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    return GT_OK;
}

/*******************************************************************************
* prvCpssBrgVlanHwEntryBuild
*
* DESCRIPTION:
*       Builds vlan entry to buffer.
*
*       NOTE: for multi-port-groups device , this function called per port group , with the
*             per port group info
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*                             CPU port supported
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan -
*                             the relevant parameter for DxCh3 and beyond and
*                             not relevant for TR101 supported devices.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       portsTaggingCmdPtr  - (pointer to) ports tagging commands in the vlan -
*                             the relevant parameter for xCat and above
*                             with TR101 feature support.
*
* OUTPUTS:
*       hwDataArr[PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS]
*                           - pointer to hw VLAN entry.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*                         or vlanInfoPtr->stgId
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssBrgVlanHwEntryBuild
(
    IN GT_U8                        devNum,
    IN CPSS_PORTS_BMP_STC           *portsMembersPtr,
    IN CPSS_PORTS_BMP_STC           *portsTaggingPtr,
    IN CPSS_DXCH_BRG_VLAN_INFO_STC  *vlanInfoPtr,
    IN CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr,
    OUT GT_U32                      hwDataArr[PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS]
)
{
    GT_U32      value;          /* for port vlan info building */
    GT_U32      value1;         /* for port vlan info building */
    GT_U32      fieldOffset;
    GT_U32      wordOffset;
    GT_U32      bitOffset;
    GT_U32      ii;
    GT_U32      maxPortNum;   /* max port number that vlan entry support */
    GT_BOOL     tr101Supported;   /* TR101 Feature Support */

    if(vlanInfoPtr->stgId >= BIT_8)/* 8 bits in HW */
    {
        return GT_OUT_OF_RANGE;
    }

    switch(PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        case CPSS_PP_FAMILY_CHEETAH_E:
            maxPortNum = 27;
            break;
        case CPSS_PP_FAMILY_DXCH_LION_E:
            maxPortNum = 12;/* per port group -- this function called per port group
                               , with the per port group info */
            break;
        default:
            maxPortNum = 28;
            break;
    }

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    if (portsMembersPtr->ports[0] >= ((GT_U32)(1 << maxPortNum)))
    {
        return GT_OUT_OF_RANGE;
    }

    if ((tr101Supported == GT_FALSE) && (portsTaggingPtr->ports[0] >= ((GT_U32)(1 << maxPortNum))))
    {
        return GT_OUT_OF_RANGE;
    }

    /* Cheetah related checks */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        if (/* IP Control To CPU - CH has only two values: ALL or NONE */
            (vlanInfoPtr->ipCtrlToCpuEn == CPSS_DXCH_BRG_IP_CTRL_IPV4_E) ||
            (vlanInfoPtr->ipCtrlToCpuEn == CPSS_DXCH_BRG_IP_CTRL_IPV6_E)
           )
        {
            return GT_BAD_PARAM;
        }

        /* all next are ignored and may be set to any values:
           - Na Msg to CPU enable */
    }

    /* check ipCtrlToCpuEn */
    if(vlanInfoPtr->ipCtrlToCpuEn > CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E ||
       vlanInfoPtr->ipCtrlToCpuEn < CPSS_DXCH_BRG_IP_CTRL_NONE_E)
    {
        return GT_BAD_PARAM;
    }

    /* reset all words in hwDataArr */
    cpssOsMemSet((char *) &hwDataArr[0], 0, PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS * sizeof(GT_U32));

    /* Set Word0 */

    /* IPv6 Site ID - only for CH2 and above. CH and xCat2 set the bit to 0 */
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E) &&
        (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        switch(vlanInfoPtr->ipv6SiteIdMode)
        {
            case CPSS_IP_SITE_ID_INTERNAL_E: value = 0; break;
            case CPSS_IP_SITE_ID_EXTERNAL_E: value = 1; break;
            default: return GT_BAD_PARAM;
        }

        hwDataArr[0] |= (value << 23);
    }

    /* IPv6 IPM Bridging enable */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv6IpmBrgEn);
    hwDataArr[0] |= (value << 22);

    /* IPv4 IPM Bridging enable */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv4IpmBrgEn);
    hwDataArr[0] |= (value << 21);

    /* IPv6 IPM Bridging mode */
    if(GT_TRUE == vlanInfoPtr->ipv6IpmBrgEn)
    {
        PRV_DXCH_BRG_VLAN_IPM_MODE_CONVERT_MAC(vlanInfoPtr->ipv6IpmBrgMode, value);
    hwDataArr[0] |= (value << 20);
    }

    /* IPv4 IPM Bridging mode */
    if(GT_TRUE == vlanInfoPtr->ipv4IpmBrgEn)
    {
        PRV_DXCH_BRG_VLAN_IPM_MODE_CONVERT_MAC(vlanInfoPtr->ipv4IpmBrgMode, value);
    hwDataArr[0] |= (value << 19);
    }

    /* IPv4 Control traffic to CPU enable */
    if ((vlanInfoPtr->ipCtrlToCpuEn == CPSS_DXCH_BRG_IP_CTRL_IPV4_E) ||
        (vlanInfoPtr->ipCtrlToCpuEn == CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E))
    {
        value = 1;
    }
    else
        value = 0;
    hwDataArr[0] |= (value << 18);

    /* IPv6 ICMP to CPU enable */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv6IcmpToCpuEn);
    hwDataArr[0] |= (value << 17);

    /* Mirror to Ingress Analyzer */
    value = BOOL2BIT_MAC(vlanInfoPtr->mirrToRxAnalyzerEn);
    hwDataArr[0] |= (value << 16);

    /* IPv4 IGMP to CPU enable */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv4IgmpToCpuEn);
    hwDataArr[0] |= (value << 15);

    /* Unknown unicast cmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unkUcastCmd))
    hwDataArr[0] |= (value << 12);

    /* Unregistered IPv6 multicast cmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unregIpv6McastCmd))
    hwDataArr[0] |= (value << 9);

    /* Unregistered IPv4 multicast cmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unregIpv4McastCmd))
    hwDataArr[0] |= (value << 6);

    /* Unregistered Non IP multicast cmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unregNonIpMcastCmd))
    hwDataArr[0] |= (value << 3);


    /* NewSrc Address Is Not Security - revert the bit */
    value = 1 - (BOOL2BIT_MAC(vlanInfoPtr->unkSrcAddrSecBreach));
    hwDataArr[0] |= (value << 1);

    /* valid bit is set to 1 */
    hwDataArr[0] |= 1;


    /* Set Word2 */

    /* Span State Group Index */
    hwDataArr[2] |= ((vlanInfoPtr->stgId & 0xff) << 24);

    /* Unregistered Non IPv4 BC Cmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                            vlanInfoPtr->unregNonIpv4BcastCmd);
    hwDataArr[2] |= (value << 21);

    /* Unregistered IPv4 BC Cmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                            vlanInfoPtr->unregIpv4BcastCmd);
    hwDataArr[2] |= (value << 18);

    if ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E) &&
        (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        /* IPv6 Multicast Route enable */
        value = BOOL2BIT_MAC(vlanInfoPtr->ipv6McastRouteEn);
        hwDataArr[2] |= (value << 17);

        /* IPv4 Multicast Route enable */
        value = BOOL2BIT_MAC(vlanInfoPtr->ipv4McastRouteEn);
        hwDataArr[2] |= (value << 15);
    }

    /* IPv6 Unicast Route enable */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv6UcastRouteEn);
    hwDataArr[2] |= (value << 16);

    /* IPv4 Unicast Route enable */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv4UcastRouteEn);
    hwDataArr[2] |= (value << 14);


    /* Set Word3 - Cheetah has not Word 3 */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* IPv6 Control traffic to CPU enable */
        if ((vlanInfoPtr->ipCtrlToCpuEn == CPSS_DXCH_BRG_IP_CTRL_IPV6_E) ||
            (vlanInfoPtr->ipCtrlToCpuEn == CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E))
        {
            value = 1;
        }
        else
            value = 0;
        hwDataArr[3] |= (value << 7);

        /* BCU DP Trap Mirror enable */
        value = BOOL2BIT_MAC(vlanInfoPtr->bcastUdpTrapMirrEn);
        hwDataArr[3] |= (value << 6);

        /* MRU Index (bit5 is reserved) */
        value = vlanInfoPtr->mruIdx;
        /* check MRU index */
        PRV_CPSS_DXCH_BRG_MRU_INDEX_CHECK_MAC(value);
        hwDataArr[3] |= (value << 2);

        /* Na Msg to CPU enable */
        value = BOOL2BIT_MAC(vlanInfoPtr->naMsgToCpuEn);
        hwDataArr[3] |= (value << 1);

        /* Auto Learning disable */
        value = BOOL2BIT_MAC(vlanInfoPtr->autoLearnDisable);
        hwDataArr[3] |= value;

        if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E) ||
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E&&
             PRV_CPSS_PP_MAC(devNum)->revision == 0))
        {

            /* add constant 0xfff to bits 117:106 */
            hwDataArr[3] |= (0xFFF << 10);
        }
        else if(PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
            {
                /* xCat2 support constant 4095 (0xFFF) and values in range 0..1023 */
                if ((vlanInfoPtr->floodVidx != 0xFFF) &&
                    (vlanInfoPtr->floodVidx >= BIT_10))
                {
                    return GT_OUT_OF_RANGE;
                }
            }
            else
            {
                if (vlanInfoPtr->floodVidx >= BIT_12)
                {
                    return GT_OUT_OF_RANGE;
                }
            }

            /* Flood Vidx */
            hwDataArr[3] |= (vlanInfoPtr->floodVidx << 8);

            /* Multicast Local Switching Enable */
            hwDataArr[3] |= (BOOL2BIT_MAC(vlanInfoPtr->mcastLocalSwitchingEn) << 20);

            /* Port Isolation VLAN command */
            PRV_CPSS_DXCH_CONVERT_PORT_ISOLATION_MODE_TO_HW_VAL_MAC(value, vlanInfoPtr->portIsolationMode);
            hwDataArr[3] |= ( value << 21);

            /* set word5 for xCat */

            /* Unicast Local Switching Enable */
            hwDataArr[5] |= (BOOL2BIT_MAC(vlanInfoPtr->ucastLocalSwitchingEn) << 17);

            /* Flood VIDX Mode */
            PRV_CPSS_DXCH_CONVERT_FLOOD_VIDX_MODE_TO_HW_VAL_MAC(value, vlanInfoPtr->floodVidxMode);
            hwDataArr[5] |= (value << 18);
        }
    }



    /* build ports info */
    for (ii = 0; (ii < PRV_CPSS_PP_MAC(devNum)->numOfPorts); ii++)
    {
        /* get port information: member and tagging */
        value  = (CPSS_PORTS_BMP_IS_PORT_SET_MAC(portsMembersPtr, ii) ? 1 : 0);

        if ( tr101Supported == GT_FALSE )
        {
            value |= (CPSS_PORTS_BMP_IS_PORT_SET_MAC(portsTaggingPtr, ii) ? 2 : 0);

            value1 = 0;
        }
        else
        {
            value |= ((portsTaggingCmdPtr->portsCmd[ii] & 1) ? 2 : 0);

            value1 = (portsTaggingCmdPtr->portsCmd[ii] >> 1);
        }

        if((value == 0) && (value1 == 0))
        {
            continue;
        }

        if(ii == 27) /* DxCh2 and above has 28 ports */
        {
            if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                /* port 27 data is placed in word 3 bits[23:24] */
                wordOffset = 3;
                bitOffset  = 23;
            }
            else
            {
                /* port 27 data is placed in word 3 bits[8:9] */
                wordOffset = 3;
                bitOffset  = 8;
            }
        }
        else
        {
            /* calculate first field offset in bits */
            fieldOffset = 24 + (ii * 2);    /* 24 first bits */
            wordOffset = fieldOffset >> 5;  /* / 32 */
            bitOffset  = fieldOffset & 0x1f; /* % 32 */
        }
        hwDataArr[wordOffset] |= value << bitOffset;

        /* set port tagging command for xCat and above devices */
        /* words 3,4,5 */
        if ( (tr101Supported != GT_FALSE) && (value1 != 0))
        {
            fieldOffset = 25 + (ii * 2);
            wordOffset = 3 + (fieldOffset >> 5);
            bitOffset  = fieldOffset & 0x1f;

            if (bitOffset == 31)
            {
                hwDataArr[wordOffset] |= (value1 & 1) << bitOffset;
                hwDataArr[wordOffset + 1] |= (value1 >> 1);
            }
            else
            {
                hwDataArr[wordOffset] |= value1 << bitOffset;
            }
        }
    }

    if (CPSS_PORTS_BMP_IS_PORT_SET_MAC(
        portsMembersPtr, CPSS_CPU_PORT_NUM_CNS))
    {
        /* CPU port is a member of VLAN */
        hwDataArr[0] |= (1 << 2);
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssBrgVlanHwEntryParse
*
* DESCRIPTION:
*       parse vlan entry from buffer.
*
*       NOTE: for multi-port-groups device , this function called per port group , to get the
*             per port group info
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       hwDataArr - pointer to hw VLAN entry.
*
* OUTPUTS:
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*                             CPU port supported
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan -
*                             the relevant parameter for DxCh3 and beyond and
*                             not relevant for TR101 supported devices.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       isValidPtr          - (pointer to) VLAN entry status
*                           GT_TRUE = Vlan is valid
*                           GT_FALSE = Vlan is not Valid
*       portsTaggingCmdPtr  - (pointer to) ports tagging commands in the vlan -
*                             the relevant parameter for xCat and above
*                             with TR101 feature support.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssBrgVlanHwEntryParse
(
    IN GT_U8                        devNum,
    IN GT_U32                       hwDataArr[PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS],
    OUT CPSS_PORTS_BMP_STC          *portsMembersPtr,
    OUT CPSS_PORTS_BMP_STC          *portsTaggingPtr,
    OUT CPSS_DXCH_BRG_VLAN_INFO_STC *vlanInfoPtr,
    OUT GT_BOOL                     *isValidPtr,
    OUT CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr
)
{
    GT_U32      ii;
    GT_U32      value;
    GT_U32      value1 = 0; /* Variable used for words 4,5 parsing. Algorithm uses
                            assumption that the variable is initialized to 0 */
    GT_U32      fieldOffset;
    GT_U32      wordOffset;
    GT_U32      bitOffset;
    GT_BOOL     tr101Supported;   /* TR101 Feature Support */
    GT_U32      maxPortNum;   /* max port number that vlan entry support */

    *isValidPtr = BIT2BOOL_MAC(hwDataArr[0] & 0x1);

    cpssOsMemSet(vlanInfoPtr,0,sizeof(*vlanInfoPtr));

    portsMembersPtr->ports[0] = 0;
    portsMembersPtr->ports[1] = 0;

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    if (tr101Supported != GT_FALSE)
    {
        cpssOsMemSet(portsTaggingCmdPtr,0,sizeof(*portsTaggingCmdPtr));
    }
    else
    {
        portsTaggingPtr->ports[0] = 0;
        portsTaggingPtr->ports[1] = 0;
    }

    /* Parse Word0 */

    /* NewSrc Address Is Not Security Breach - revert the value */
    value = U32_GET_FIELD_MAC(hwDataArr[0] , 1 , 1);
    vlanInfoPtr->unkSrcAddrSecBreach = BIT2BOOL_MAC(value - 1);

    /* Unregistered Non IP multicast cmd */
    value = U32_GET_FIELD_MAC(hwDataArr[0] , 3 , 3);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregNonIpMcastCmd,
                                              value)

    /* Unregistered IPv4 multicast cmd */
    value = U32_GET_FIELD_MAC(hwDataArr[0] , 6 , 3);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregIpv4McastCmd,
                                              value)

    /* Unregistered IPv6 multicast cmd */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 9, 3);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregIpv6McastCmd,
                                              value);

    /* Unknown unicast cmd */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 12, 3);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unkUcastCmd,
                                              value);

    /* IPv4 IGMP to CPU enable */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 15, 1);
    vlanInfoPtr->ipv4IgmpToCpuEn = BIT2BOOL_MAC(value);

    /* Mirror to Ingress Analyzer */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 16, 1);
    vlanInfoPtr->mirrToRxAnalyzerEn = BIT2BOOL_MAC(value);

    /* IPv6 ICMP to CPU enable */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 17, 1);
    vlanInfoPtr->ipv6IcmpToCpuEn = BIT2BOOL_MAC(value);

    /* IPv4 Control to CPU enable, the IPv6 Control to Cpu will be read later
       and the ipCtrlToCpuEn will be changed accordingly */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 18, 1);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* for CH there are only two values: ALL or NONE */
        vlanInfoPtr->ipCtrlToCpuEn = (value == 0x1) ?
                                        CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E :
                                        CPSS_DXCH_BRG_IP_CTRL_NONE_E;
    }
    else
    {
        vlanInfoPtr->ipCtrlToCpuEn = (value == 0x1) ?
                                        CPSS_DXCH_BRG_IP_CTRL_IPV4_E :
                                        CPSS_DXCH_BRG_IP_CTRL_NONE_E;
    }

    /* IPv4 IPM Bridging mode */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 19, 1);
    vlanInfoPtr->ipv4IpmBrgMode = (value == 0x0) ? CPSS_BRG_IPM_SGV_E :
                                                 CPSS_BRG_IPM_GV_E;

    /* IPv6 IPM Bridging mode */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 20, 1);
    vlanInfoPtr->ipv6IpmBrgMode = (value == 0x0) ? CPSS_BRG_IPM_SGV_E :
                                                 CPSS_BRG_IPM_GV_E;

    /* IPv4 IPM Bridging enable */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 21, 1);
    vlanInfoPtr->ipv4IpmBrgEn = BIT2BOOL_MAC(value);

    /* IPv6 IPM Bridging enable */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 22, 1);
    vlanInfoPtr->ipv6IpmBrgEn = BIT2BOOL_MAC(value);

     /* IPv6 Site ID */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* Cheetah has not this bit
           - always return CPSS_IP_SITE_ID_INTERNAL_E */
        value = 0;
    }
    else
    {
        value = U32_GET_FIELD_MAC(hwDataArr[0], 23, 1);
    }

    vlanInfoPtr->ipv6SiteIdMode = (value == 0x0) ?
        CPSS_IP_SITE_ID_INTERNAL_E :
        CPSS_IP_SITE_ID_EXTERNAL_E;


    /* Parse Word2 */

    /* IPv4/6 Multicast Route enable */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* IPM disabled for CH */
        vlanInfoPtr->ipv4McastRouteEn = GT_FALSE;
        vlanInfoPtr->ipv6McastRouteEn = GT_FALSE;
    }
    else
    {
        /* IPv4 Multicast Route enable */
        value = U32_GET_FIELD_MAC(hwDataArr[2], 15, 1);
        vlanInfoPtr->ipv4McastRouteEn = BIT2BOOL_MAC(value);

        /* IPv6 Multicast Route enable */
        value = U32_GET_FIELD_MAC(hwDataArr[2], 17, 1);
        vlanInfoPtr->ipv6McastRouteEn = BIT2BOOL_MAC(value);

    }


    /* IPv4 Unicast Route enable */
    value = U32_GET_FIELD_MAC(hwDataArr[2], 14, 1);
    vlanInfoPtr->ipv4UcastRouteEn = BIT2BOOL_MAC(value);

    /* IPv6 Unicast Route enable */
    value = U32_GET_FIELD_MAC(hwDataArr[2], 16, 1);
    vlanInfoPtr->ipv6UcastRouteEn = BIT2BOOL_MAC(value);

    /* Unregistered IPv4 BC Cmd */
    value = U32_GET_FIELD_MAC(hwDataArr[2], 18, 3);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregIpv4BcastCmd,
                                              value)
    /* Unregistered Non IPv4 BC Cmd */
    value = U32_GET_FIELD_MAC(hwDataArr[2], 21, 3);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregNonIpv4BcastCmd,
                                              value)

    /* Span State Group Index */
    vlanInfoPtr->stgId = U32_GET_FIELD_MAC(hwDataArr[2], 24, 8);


    /* Parse Word3 - Cheetah has not the Word 3*/

    /* check device type */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* disabled for CH */
        vlanInfoPtr->autoLearnDisable = GT_FALSE;
        vlanInfoPtr->naMsgToCpuEn = GT_TRUE;
        vlanInfoPtr->bcastUdpTrapMirrEn = GT_FALSE;
    }
    else
    {
        /* get IPv6 Control traffic to CPU enable */
        /* set IP Control to CPU enable */
        value = U32_GET_FIELD_MAC(hwDataArr[3], 7, 1);
        if (vlanInfoPtr->ipCtrlToCpuEn == CPSS_DXCH_BRG_IP_CTRL_IPV4_E)
        {
            if (value == 1)
                vlanInfoPtr->ipCtrlToCpuEn = CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E;
        }
        else
        {
            if (value == 1)
                vlanInfoPtr->ipCtrlToCpuEn = CPSS_DXCH_BRG_IP_CTRL_IPV6_E;
        }

        /* Auto Learning disable */
        value = U32_GET_FIELD_MAC(hwDataArr[3], 0, 1);
        vlanInfoPtr->autoLearnDisable = BIT2BOOL_MAC(value);

        /* Na Msg to CPU enable */
        value = U32_GET_FIELD_MAC(hwDataArr[3], 1, 1);
        vlanInfoPtr->naMsgToCpuEn = BIT2BOOL_MAC(value);

        /* MRU Index (bit5 is reserved) */
        vlanInfoPtr->mruIdx = U32_GET_FIELD_MAC(hwDataArr[3], 2, 3);

        /* BCU DP Trap Mirror enable */
        value = U32_GET_FIELD_MAC(hwDataArr[3], 6, 1);
        vlanInfoPtr->bcastUdpTrapMirrEn = BIT2BOOL_MAC(value);
    }

    /* xCat and above fields */
    if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* Flood VIDX */
        vlanInfoPtr->floodVidx = (GT_U16) U32_GET_FIELD_MAC(hwDataArr[3], 8, 12);

        /* Multicast Local Switching Enable */
        value = U32_GET_FIELD_MAC(hwDataArr[3], 20, 1);
        vlanInfoPtr->mcastLocalSwitchingEn = BIT2BOOL_MAC(value);

        /* Port Isolation L2 VLAN command */
        value = U32_GET_FIELD_MAC(hwDataArr[3], 21, 2);
        switch (value)
        {
            case 0:
                vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E;
                break;
            case 1:
                vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_CMD_E;
                break;
            case 2:
                vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L3_CMD_E;
                break;
            default:
                vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_L3_CMD_E;
        }

        /* Unicast Local Switching Enable */
        value = U32_GET_FIELD_MAC(hwDataArr[5], 17, 1);
        vlanInfoPtr->ucastLocalSwitchingEn = BIT2BOOL_MAC(value);

        /* Flood VIDX Mode */
        value = U32_GET_FIELD_MAC(hwDataArr[5], 18, 1);
        vlanInfoPtr->floodVidxMode = (value == 0) ? CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E
                                                  : CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E;
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        maxPortNum = 12;/* per port group -- this function called per port group
                           , with the per port group info */
    }
    else
    {
        maxPortNum = PRV_CPSS_PP_MAC(devNum)->numOfPorts;
    }

    /* get ports info */
    for (ii = 0; ii < maxPortNum; ii++)
    {
        if(ii == 27)
        {
            if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                /* port 27 data is placed in word 3 bits[23:24] */
                wordOffset = 3;
                bitOffset  = 23;
            }
            else
            {
                /* port 27 data is placed in word 3 bits[8:9] */
                wordOffset = 3;
                bitOffset  = 8;
            }
        }
        else
        {
            /* calculate first field offset in bits */
            fieldOffset = 24 + (ii * 2);    /* 24 first bits */
            wordOffset = fieldOffset >> 5;  /* / 32 */
            bitOffset  = fieldOffset & 0x1f; /* % 32 */
        }

        value = (hwDataArr[wordOffset] >> bitOffset) & 0x3;

        if (tr101Supported != GT_FALSE)
        {
            /* calculate first field offset in bits */
            fieldOffset = 25 + (ii * 2);    /* 24 first bits */
            wordOffset = 3 + (fieldOffset >> 5);  /* / 32 */
            bitOffset  = fieldOffset & 0x1f; /* % 32 */

            if (bitOffset == 31)
            {
                value1 = (hwDataArr[wordOffset] >> bitOffset);
                value1 |= ((hwDataArr[wordOffset + 1] & 1) << 1 );
            }
            else
            {
                value1 = (hwDataArr[wordOffset] >> bitOffset) & 0x3;
            }
        }

        if ((value == 0) && (value1 == 0))
        {
            continue;
        }

        /* set port information: member and tagging */
        if (value & 1)
            CPSS_PORTS_BMP_PORT_SET_MAC(portsMembersPtr,ii) ;

        if (tr101Supported != GT_FALSE)
        {
            portsTaggingCmdPtr->portsCmd[ii] = value >> 1 ;
            portsTaggingCmdPtr->portsCmd[ii] |= (value1 << 1);
        }
        else
        {
            if (value & 2)
            {
                CPSS_PORTS_BMP_PORT_SET_MAC(portsTaggingPtr,ii) ;
            }
        }
    }

    if ((hwDataArr[0] & (1 << 2)) != 0)
    {
        /* CPU port is a member of VLAN */
        CPSS_PORTS_BMP_PORT_SET_MAC(
            portsMembersPtr, CPSS_CPU_PORT_NUM_CNS);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanEntryWrite
*
* DESCRIPTION:
*       Builds and writes vlan entry to HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       vlanId              - VLAN Id
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*                             CPU port supported
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan -
*                             The parameter is relevant for DxCh1, DxCh2
*                             and DxCh3 devices.
*                             The parameter is relevant for xCat and
*                             above devices without TR101 feature support
*                             The parameter is not relevant and ignored for
*                             xCat and above devices with TR101 feature
*                             support.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       portsTaggingCmdPtr  - (pointer to) ports tagging commands in the vlan -
*                             The parameter is relevant only for xCat and
*                             above with TR101 feature support.
*                             The parameter is not relevant and ignored for
*                             other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_OUT_OF_RANGE          - length of portsMembersPtr, portsTaggingPtr
*                                  or vlanInfoPtr->stgId is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEntryWrite
(
    IN  GT_U8                                 devNum,
    IN  GT_U16                                vlanId,
    IN  CPSS_PORTS_BMP_STC                    *portsMembersPtr,
    IN  CPSS_PORTS_BMP_STC                    *portsTaggingPtr,
    IN  CPSS_DXCH_BRG_VLAN_INFO_STC           *vlanInfoPtr,
    IN  CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr
)
{
    GT_U32      hwData[PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS];
                                  /* buffer to build entry; size is max  */
                                  /* possible size in Prestera system device */
    GT_BOOL     tr101Supported;   /* TR101 Feature Support */

    GT_STATUS   rc = GT_OK;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);
    CPSS_NULL_PTR_CHECK_MAC(portsMembersPtr);
    CPSS_NULL_PTR_CHECK_MAC(vlanInfoPtr);

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    if (tr101Supported != GT_FALSE)
    {
        /* for TR101 feature supported tagging command parameter must be valid */
        CPSS_NULL_PTR_CHECK_MAC(portsTaggingCmdPtr);
    }
    else
    {
        /* Simple port tagging */
        CPSS_NULL_PTR_CHECK_MAC(portsTaggingPtr);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = vlanEntrySplitTableWrite(devNum,vlanId,
                                     portsMembersPtr,
                                     portsTaggingPtr,
                                     vlanInfoPtr,
                                     portsTaggingCmdPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        /* build VLAN entry */
        rc = prvCpssBrgVlanHwEntryBuild(devNum,
                                        portsMembersPtr,
                                        portsTaggingPtr,
                                        vlanInfoPtr,
                                        portsTaggingCmdPtr,
                                        hwData);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* write VLAN entry to the VLAN Table */
        rc = prvCpssDxChWriteTableEntry(devNum,
                                        PRV_CPSS_DXCH_TABLE_VLAN_E,
                                        (GT_U32)vlanId,/* vid */
                                        hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* check for Cheetah3 and above device
       xCat2 does not support VRF ID */
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E) &&
        (0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum)) &&
        (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        if(vlanInfoPtr->vrfId >= BIT_12 )
        {
            return GT_OUT_OF_RANGE;
        }

        /* write VRF-ID entry */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                              PRV_CPSS_DXCH3_TABLE_VRF_ID_E,
                                              (GT_U32)vlanId,
                                              0,
                                              0,
                                              12,
                                              vlanInfoPtr->vrfId);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanEntriesRangeWrite
*
* DESCRIPTION:
*       Function set multiple vlans with the same configuration
*       function needed for performances , when the device use "indirect" access
*       to the vlan table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number.
*       vlanId              - first VLAN Id
*       numOfEntries        - number of entries (vlans) to set
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*                             CPU port supported
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan -
*                             The parameter is relevant for DxCh1, DxCh2
*                             and DxCh3 devices.
*                             The parameter is relevant for xCat and
*                             above devices without TR101 feature support
*                             The parameter is not relevant and ignored for
*                             xCat and above devices with TR101 feature
*                             support.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       portsTaggingCmdPtr  - (pointer to) ports tagging commands in the vlan -
*                             The parameter is relevant only for xCat and
*                             above with TR101 feature support.
*                             The parameter is not relevant and ignored for
*                             other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong device number or vlanId
*       GT_OUT_OF_RANGE          - length of portsMembersPtr, portsTaggingPtr
*                                  or vlanInfoPtr->stgId is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEntriesRangeWrite
(
    IN  GT_U8                                 devNum,
    IN  GT_U16                                vlanId,
    IN  GT_U32                                numOfEntries,
    IN  CPSS_PORTS_BMP_STC                    *portsMembersPtr,
    IN  CPSS_PORTS_BMP_STC                    *portsTaggingPtr,
    IN  CPSS_DXCH_BRG_VLAN_INFO_STC           *vlanInfoPtr,
    IN  CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr
)
{
    GT_U32      hwData[PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS];
                                  /* buffer to build entry; size is max  */
                                  /* possible size in Prestera system device */
    GT_STATUS   rc = GT_OK;       /* return code*/
    GT_U32      i;                /* iterator */
    PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr; /* pointer to table info */
    GT_BOOL     tr101Supported;   /* TR101 Feature Support */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC((GT_U32)((GT_U32)vlanId + (numOfEntries - 1)));
    CPSS_NULL_PTR_CHECK_MAC(portsMembersPtr);
    CPSS_NULL_PTR_CHECK_MAC(vlanInfoPtr);

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    if (tr101Supported != GT_FALSE)
    {
        /* for TR101 feature supported tagging command parameter must be valid */
        CPSS_NULL_PTR_CHECK_MAC(portsTaggingCmdPtr);
    }
    else
    {
        /* Simple port tagging */
        CPSS_NULL_PTR_CHECK_MAC(portsTaggingPtr);
    }

    if (numOfEntries == 0)
    {
        return GT_BAD_PARAM;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        for(i = 0; i < numOfEntries; i++)
        {
            rc = vlanEntrySplitTableWrite(devNum,(GT_U16)(vlanId + i),
                                         portsMembersPtr,
                                         portsTaggingPtr,
                                         vlanInfoPtr,
                                         portsTaggingCmdPtr);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        return GT_OK;
    }
    else
    {
        /* build VLAN entry */
        rc = prvCpssBrgVlanHwEntryBuild(devNum,
                                        portsMembersPtr,
                                        portsTaggingPtr,
                                        vlanInfoPtr,
                                        portsTaggingCmdPtr,
                                        hwData);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* write VLAN entry to the VLAN Table */
        rc = prvCpssDxChWriteTableEntry(devNum,
                                        PRV_CPSS_DXCH_TABLE_VLAN_E,
                                        (GT_U32)vlanId,/* vid */
                                        hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* check for Cheetah3 and above device
       xCat2 does not support VRF ID */
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E) &&
        (0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum)) &&
        (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        if(vlanInfoPtr->vrfId >= BIT_12 )
        {
            return GT_OUT_OF_RANGE;
        }

        /* write VRF-ID entry */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                              PRV_CPSS_DXCH3_TABLE_VRF_ID_E,
                                              (GT_U32)vlanId,
                                              0,
                                              0,
                                              12,
                                              vlanInfoPtr->vrfId);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    tableInfoPtr = &(PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_VLAN_E]);

     /* Write the above entry to all vlans */
    for(i = 1; i < numOfEntries; i++)
    {
        if(tableInfoPtr->writeAccessType == PRV_CPSS_DXCH_DIRECT_ACCESS_E)
        {
            /* write full vlan entry */
            rc = prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH_TABLE_VLAN_E,
                                    (vlanId + i),
                                    hwData);
        }
        else
        {
            /*
                NOTE: the multi-port-groups device not need special case ,
                      because in indirect access , we need to loop on all port groups
                      without any change of 'data' , only 'control' changes.
                      and this loop on port groups will be done inside the cpssDriver
                      called from prvCpssDxChWriteTableEntry(...)
            */


            /* The indirect access implementation use NULL pointer to entry as
               sign to write only to control word. This result in all VLAN entries
               get same data written above.  */
            rc = prvCpssDxChWriteTableEntry(devNum,
                                        PRV_CPSS_DXCH_TABLE_VLAN_E,
                                        (vlanId + i),
                                        NULL);
        }

        if (rc != GT_OK)
        {
            return rc;
        }

        /* check for Cheetah3 and above device
           xCat2 does not support VRF ID */
        if ((PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E) &&
            (0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum)) &&
            (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            /* write VRF-ID entry */
            rc = prvCpssDxChWriteTableEntryField(devNum,
                                                  PRV_CPSS_DXCH3_TABLE_VRF_ID_E,
                                                  (vlanId + i),
                                                  0,
                                                  0,
                                                  12,
                                                  vlanInfoPtr->vrfId);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanEntryRead
*
* DESCRIPTION:
*       Read vlan entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       vlanId              - VLAN Id
*
* OUTPUTS:
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*                             CPU port supported
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan -
*                             The parameter is relevant for DxCh1, DxCh2
*                             and DxCh3 devices.
*                             The parameter is relevant for xCat and
*                             above devices without TR101 feature support
*                             The parameter is not relevant and ignored for
*                             xCat and above devices with TR101 feature
*                             support.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       isValidPtr          - (pointer to) VLAN entry status
*                                 GT_TRUE = Vlan is valid
*                                 GT_FALSE = Vlan is not Valid
*       portsTaggingCmdPtr  - (pointer to) ports tagging commands in the vlan -
*                             The parameter is relevant only for xCat and
*                             above with TR101 feature support.
*                             The parameter is not relevant and ignored for
*                             other devices.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT               - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEntryRead
(
    IN  GT_U8                                devNum,
    IN  GT_U16                               vlanId,
    OUT CPSS_PORTS_BMP_STC                   *portsMembersPtr,
    OUT CPSS_PORTS_BMP_STC                   *portsTaggingPtr,
    OUT CPSS_DXCH_BRG_VLAN_INFO_STC          *vlanInfoPtr,
    OUT GT_BOOL                              *isValidPtr,
    OUT CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC *portsTaggingCmdPtr
)
{
    GT_U32  hwData[PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS];
                                    /* buffer to build entry; size is max     */
                                    /* possible size in Prestera system device*/
    GT_BOOL     tr101Supported;   /* TR101 Feature Support */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);
    CPSS_NULL_PTR_CHECK_MAC(portsMembersPtr);
    CPSS_NULL_PTR_CHECK_MAC(vlanInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(isValidPtr);

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    if (tr101Supported != GT_FALSE)
    {
        /* for TR101 feature supported tagging command parameter must be valid */
        CPSS_NULL_PTR_CHECK_MAC(portsTaggingCmdPtr);
    }
    else
    {
        /* Simple port tagging */
        CPSS_NULL_PTR_CHECK_MAC(portsTaggingPtr);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = vlanEntrySplitTableRead(devNum,vlanId,
                                     portsMembersPtr,
                                     portsTaggingPtr,
                                     vlanInfoPtr,
                                     isValidPtr,
                                     portsTaggingCmdPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        /* read entry from HW */
        rc = prvCpssDxChReadTableEntry(devNum,
                                       PRV_CPSS_DXCH_TABLE_VLAN_E,
                                       (GT_U32)vlanId,/* vid */
                                       hwData);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssBrgVlanHwEntryParse(devNum,
                                        hwData,
                                        portsMembersPtr,
                                        portsTaggingPtr,
                                        vlanInfoPtr,
                                        isValidPtr,
                                        portsTaggingCmdPtr);

        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* check for Cheetah3 and above device
       xCat2 does not support VRF ID */
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E) &&
        (0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum)) &&
        (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        /* read VRF-ID entry */
        return prvCpssDxChReadTableEntryField(devNum,
                                              PRV_CPSS_DXCH3_TABLE_VRF_ID_E,
                                              (GT_U32)vlanId,
                                              0,
                                              0,
                                              12,
                                              &(vlanInfoPtr->vrfId));
    }

    return GT_OK;
}

/*******************************************************************************
* vlanMemberSet
*
* DESCRIPTION:
*       set port as vlan member/not and it's tag mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       vlanId          - VLAN Id
*       portNum         - port number
*       isMember        - is port member of the vlan
*       isTagged        - GT_TRUE, to set the port as tagged member,
*                         GT_FALSE, to set the port as untagged
*                         The parameter is relevant for DxCh1, DxCh2
*                         and DxCh3 devices.
*                         The parameter is relevant for xCat and
*                         above devices without TR101 feature support
*                         The parameter is not relevant and ignored for
*                         xCat and above devices with TR101 feature
*                         support.
*       portTaggingCmd  - port tagging command
*                         The parameter is relevant only for xCat and above
*                         with TR101 feature support.
*                         The parameter is not relevant and ignored for
*                         other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vid or portTaggingCmd
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS vlanMemberSet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanId,
    IN  GT_U8   portNum,
    IN  GT_BOOL isMember,
    IN  GT_BOOL isTagged,
    IN  CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT portTaggingCmd
)
{
    GT_U32      hwData;             /* buffer to build part entry          */
    GT_U32      fieldOffset;        /* port info first field position      */
    GT_U32      wordOffset;         /* current word offset                 */
    GT_U32      bitOffset;          /* the offset in the word              */
    GT_STATUS   rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */
    CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT currTagCmd;
    GT_U32      hwDataTagCmd = 0;       /* hw format for currTagCmd */
    GT_BOOL     tr101Supported;   /* TR101 Feature Support */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    if (tr101Supported == GT_FALSE)
    {
        currTagCmd = (isTagged == GT_TRUE) ? CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E :
            CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
    }
    else
    {
        currTagCmd = portTaggingCmd;
    }

    if (portNum != CPSS_CPU_PORT_NUM_CNS)
    {
        PRV_CPSS_DXCH_CONVERT_VLAN_TAG_MODE_TO_HW_VAL_MAC(hwDataTagCmd,currTagCmd);
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        hwData = BOOL2BIT_MAC(isMember);

        if (portNum == CPSS_CPU_PORT_NUM_CNS)
        {
            /* Write one bit for specific CPU port membership to HW */
            /* Tagging command ignored                              */
            return prvCpssDxChPortGroupWriteTableEntryField(
                devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                PRV_CPSS_DXCH_TABLE_VLAN_E,
                (GT_U32)vlanId,/* vid */
                0 /*wordOffset*/,
                2 /*bitOffset*/,
                1 /*bitLengrh*/,
                hwData /*hwData*/);
        }

        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        /* calculate the word number and the bit offset to read */
        if (localPort < 27)
        {
            /* calculate first field offset in bits */
            fieldOffset = 24 + (2 * localPort);   /* 24 first byte */
            wordOffset = fieldOffset >> 5;      /* / 32 */
            bitOffset = fieldOffset & 0x1f;     /* % 32 */
        }
        else
        {
            if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                /* port 27 data is placed in word 3 bits[23:24] */
                wordOffset = 3;
                bitOffset  = 23;
            }
            else
            {
                /* port 27 data is placed in word 3 bits[8:9] */
                wordOffset = 3;
                bitOffset = 8;
            }
        }

        /* build the data */
        if (tr101Supported != GT_FALSE)
        {
            hwData |= ((hwDataTagCmd & 1) << 1) ;
        }
        else
        {
            hwData |= ((BOOL2BIT_MAC(isTagged)) << 1);
        }

        /* write two bits for specific port to HW */
        rc = prvCpssDxChPortGroupWriteTableEntryField(devNum,portGroupId,
                                            PRV_CPSS_DXCH_TABLE_VLAN_E,
                                            (GT_U32)vlanId,/* vid */
                                            wordOffset,
                                            bitOffset,
                                            2,
                                            hwData);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (tr101Supported != GT_FALSE)
        {
            /* build the data */
            hwData = (hwDataTagCmd >> 1);

            /* calculate first field offset in bits */
            fieldOffset = 25 + (2 * localPort);   /* 24 first byte */
            wordOffset = 3 + (fieldOffset >> 5);      /* / 32 */
            bitOffset = fieldOffset & 0x1f;     /* % 32 */

            if (bitOffset == 31)
            {
                /* write 1 bit for specific port to HW */
                rc = prvCpssDxChPortGroupWriteTableEntryField(devNum,portGroupId,
                                                     PRV_CPSS_DXCH_TABLE_VLAN_E,
                                                     (GT_U32)vlanId,/* vid */
                                                     wordOffset,
                                                     bitOffset,
                                                     1,
                                                     (hwData & 1));

                /* write 1 bit for specific port to HW */
                rc = prvCpssDxChPortGroupWriteTableEntryField(devNum,portGroupId,
                                                     PRV_CPSS_DXCH_TABLE_VLAN_E,
                                                     (GT_U32)vlanId,/* vid */
                                                     wordOffset + 1,
                                                     0,
                                                     1,
                                                     (hwData >> 1));
            }
            else
            {
                /* write two bits for specific port to HW */
                rc = prvCpssDxChPortGroupWriteTableEntryField(devNum,portGroupId,
                                                     PRV_CPSS_DXCH_TABLE_VLAN_E,
                                                     (GT_U32)vlanId,/* vid */
                                                     wordOffset,
                                                     bitOffset,
                                                     2,
                                                     hwData);
            }
        }
    }
    else
    {
        if(portNum >= 64)
        {
            /* values of 0..63 supported */
            return GT_BAD_PARAM;
        }

        /* set port as member in ingress table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                              PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                              (GT_U32)vlanId,
                                              PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                              2 + portNum,
                                              1, /* 1 bit */
                                              BOOL2BIT_MAC(isMember));
        if(rc != GT_OK)
        {
            return rc;
        }

        /* set port as member in egress table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                              PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                              (GT_U32)vlanId,
                                              PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                              1 + portNum,
                                              1, /* 1 bit */
                                              BOOL2BIT_MAC(isMember));
        if(rc != GT_OK)
        {
            return rc;
        }

        if(portNum != CPSS_CPU_PORT_NUM_CNS)
        {
            /* set port tagCmp */
            rc = prvCpssDxChWriteTableEntryField(devNum,
                                                  PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                                  (GT_U32)vlanId,
                                                  PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                                  65 + (3 * portNum),
                                                  3, /* 3 bits */
                                                  hwDataTagCmd);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanMemberAdd
*
* DESCRIPTION:
*       Add new port member to vlan entry. This function can be called only for
*       add port belongs to device that already member of the vlan.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       vlanId          - VLAN Id
*       portNum         - port number
*                         CPU port supported
*       isTagged        - GT_TRUE, to set the port as tagged member,
*                         GT_FALSE, to set the port as untagged
*                         The parameter is relevant for DxCh1, DxCh2
*                         and DxCh3 devices.
*                         The parameter is relevant for xCat and
*                         above devices without TR101 feature support
*                         The parameter is not relevant and ignored for
*                         xCat and above devices with TR101 feature
*                         support.
*       portTaggingCmd  - port tagging command
*                         The parameter is relevant only for xCat and above
*                         with TR101 feature support.
*                         The parameter is not relevant and ignored for
*                         other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vid or portTaggingCmd
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In case that added port belongs to device that is new device for vlan
*       other function is used - cpssDxChBrgVlanEntryWrite.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanMemberAdd
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanId,
    IN  GT_U8   portNum,
    IN  GT_BOOL isTagged,
    IN  CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT portTaggingCmd
)
{
    return vlanMemberSet(devNum,vlanId,portNum,GT_TRUE,isTagged,portTaggingCmd);
}


/*******************************************************************************
* cpssDxChBrgVlanPortDelete
*
* DESCRIPTION:
*       Delete port member from vlan entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN Id
*       port        - port number
*                     CPU port supported
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortDelete
(
    IN GT_U8          devNum,
    IN GT_U16         vlanId,
    IN GT_U8          port
)
{
    return vlanMemberSet(devNum,vlanId,port,GT_FALSE,GT_FALSE,
                        CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E);

}

/*******************************************************************************
* cpssDxChBrgVlanMemberSet
*
* DESCRIPTION:
*       Set specific member at VLAN entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       vlanId          - VLAN Id
*       portNum         - port number
*                         CPU port supported
*       isMember        - GT_TRUE,  port would set as member
*                         GT_FALSE, port would set as not-member
*       isTagged        - GT_TRUE, to set the port as tagged member,
*                         GT_FALSE, to set the port as untagged
*                         The parameter is relevant for DxCh1, DxCh2
*                         and DxCh3 devices.
*                         The parameter is relevant for xCat and
*                         above devices without TR101 feature support
*                         The parameter is not relevant and ignored for
*                         xCat and above devices with TR101 feature
*                         support.
*       taggingCmd      - port tagging command
*                         The parameter is relevant only for xCat and above
*                         with TR101 feature support.
*                         The parameter is not relevant and ignored for
*                         other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or taggingCmd
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanMemberSet
(
    IN  GT_U8 devNum,
    IN  GT_U16 vlanId,
    IN  GT_U8 portNum,
    IN  GT_BOOL isMember,
    IN  GT_BOOL isTagged,
    IN  CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT taggingCmd
)
{
    return vlanMemberSet(devNum,vlanId,portNum,isMember,isTagged,taggingCmd);
}

/*******************************************************************************
* cpssDxChBrgVlanPortVidGet
*
* DESCRIPTION:
*       Get port's default VLAN Id.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       port        - port number
*
* OUTPUTS:
*       vidPtr   - default VLAN ID.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVidGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    OUT GT_U16  *vidPtr
)
{
    GT_STATUS   rc;
    GT_U32      startWord; /* the table word at which the field starts */
    GT_U32      startBit;  /* the word's bit at which the field starts */
    GT_U32      hwData;/*data to write to Hw*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);
    CPSS_NULL_PTR_CHECK_MAC(vidPtr);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        startWord = 0;
        startBit  = 15;
    }
    else
    {
        startWord = 1;
        startBit  = 0;
    }

    /* read from port-vlan-qos table  */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        port,/* port */
                                        startWord,
                                        startBit,
                                        12, /* 12 bits */
                                        &hwData);

    *vidPtr = (GT_U16)hwData;

    return rc;

}

/*******************************************************************************
* multiPortGroupDebugBrgVlanPortVidSet
*
* DESCRIPTION:
*       Set port's default VLAN Id.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum   - device number
*       port     - port number
*       vlanId      - VLAN Id
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum port, or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS multiPortGroupDebugBrgVlanPortVidSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    IN  GT_U16  vlanId
)
{
    GT_STATUS   rc;
    GT_U32  ii,jj; /*iterators*/
    GT_U32  globalPort,tmpPort;/* uplink global port , tmp interconnection port */


    /* in this function we assume that application will set all the uplinks with
       the same PVID , so for a particular uplink we set only it's interconnection
       ports , and not all interconnection ports (when multi-uplinks) */


    /* check if this port is one of the uplink ports ,if so */
    /* loop on all it's interconnection ports , and set them with the same PVID */
    for(ii = 0 ;
        ii < PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->numOfUplinkPorts;
        ii++)
    {
        globalPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,
            PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->uplinkInfoArr[ii].uplinkPortGroupPortInfo.portGroupId,
            PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->uplinkInfoArr[ii].uplinkPortGroupPortInfo.localPort);
        if(globalPort != port)
        {
            continue;
        }

        /* the PVID on this port need to be set on other ports as well */
        for(jj = 0 ;
            jj < PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->uplinkInfoArr[ii].numberOfInternalConnections;
            jj++)
        {
            /*convert localPort, portGroupId to global port */
            tmpPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,
                PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->uplinkInfoArr[ii].internalConnectionsArr[jj].portGroupId,
                PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->uplinkInfoArr[ii].internalConnectionsArr[jj].localPort);

            rc = cpssDxChBrgVlanPortVidSet(devNum,(GT_U8)tmpPort,vlanId);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        /* we handled the 'Interconnection ports' of this uplink ,
           so no more needed */
        break;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanPortVidSet
*
* DESCRIPTION:
*       Set port's default VLAN Id.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       port     - port number
*       vlanId      - VLAN Id
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum port, or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVidSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    IN  GT_U16  vlanId
)
{
    GT_STATUS   rc;
    GT_U32      startWord; /* the table word at which the field starts */
    GT_U32      startBit;  /* the word's bit at which the field starts */
    GT_U32      hwData; /* data to write to Hw */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    hwData = vlanId;

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        startWord = 0;
        startBit  = 15;
    }
    else
    {
        startWord = 1;
        startBit  = 0;
    }

    /* write to port-vlan-qos table  */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,/* port */
                                         startWord,
                                         startBit,
                                         12, /* 12 bits */
                                         hwData);

    if(rc == GT_OK &&
       PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->debugInfoValid == GT_TRUE)
    {
        rc = multiPortGroupDebugBrgVlanPortVidSet(devNum,port,vlanId);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortIngFltEnable
*
* DESCRIPTION:
*       Enable/disable Ingress Filtering for specific port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       port      - port number
*       enable    - GT_TRUE, enable ingress filtering
*                   GT_FALSE, disable ingress filtering
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortIngFltEnable
(
    IN GT_U8   devNum,
    IN GT_U8   port,
    IN GT_BOOL enable
)
{
    GT_U32 val;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);

    val = BOOL2BIT_MAC(enable);

    return prvCpssDrvHwPpPortGroupSetRegField(devNum,PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
                  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.portControl[port],
                  21, 1, val);
}

/*******************************************************************************
* cpssDxChBrgVlanPortProtoClassVlanEnable
*
* DESCRIPTION:
*       Enable/Disable Port Protocol VLAN assignment to packets received
*       on this port,according to it's Layer2 protocol
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       port   - port number
*       enable  - GT_TRUE for enable, otherwise GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortProtoClassVlanEnable
(
    IN GT_U8   devNum,
    IN GT_U8   port,
    IN GT_BOOL enable
)
{
    GT_U32      hwData;     /* data to write into register */
    GT_U32      startWord;  /* the table word at which the field starts */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);

    hwData = BOOL2BIT_MAC(enable);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        startWord = 0;
        startBit  = 30;
    }
    else
    {
        startWord = 0;
        startBit  = 18;
    }

    /* write to port-vlan-qos table  */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,/* port */
                                         startWord,
                                         startBit,
                                         1, /* 1 bit */
                                         hwData);
    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortProtoClassQosEnable
*
* DESCRIPTION:
*       Enable/Disable Port Protocol QoS Assignment to packets received on this
*       port, according to it's Layer2 protocol
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       port   - port number
*       enable  - GT_TRUE for enable, otherwise GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortProtoClassQosEnable
(
    IN GT_U8   devNum,
    IN GT_U8   port,
    IN GT_BOOL enable
)
{
    GT_U32      hwData;     /* data to write into register */
    GT_U32      startWord;  /* the table word at which the field starts */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);

    hwData = BOOL2BIT_MAC(enable);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        startWord = 1;
        startBit  = 11;
    }
    else
    {
        startWord = 0;
        startBit  = 19;
    }

    /* write to port-vlan-qos table  */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,/* port */
                                         startWord,
                                         startBit,
                                         1, /* 1 bit */
                                         hwData);
    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanProtoClassSet
*
* DESCRIPTION:
*       Write etherType and encapsulation of Protocol based classification
*       for specific device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       entryNum    - entry number (0..7)
*       etherType   - Ether Type or DSAP/SSAP
*       encListPtr  - encapsulation.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanProtoClassSet
(
    IN GT_U8                            devNum,
    IN GT_U32                           entryNum,
    IN GT_U16                           etherType,
    IN CPSS_PROT_CLASS_ENCAP_STC        *encListPtr
)
{
    GT_U32      hwData;              /* value to write into register    */
    GT_U32      regAddr;             /* register address                */
    GT_U32      wordOffset;          /* offset of register              */
    GT_U32      offset;              /* offset in a register            */
    GT_STATUS   rc ;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(encListPtr);
    PRV_CPSS_DXCH_BRG_PORT_PROTOCOL_CHECK_MAC(devNum, entryNum);

    /* build encapsulation bitmap */
    hwData = (BOOL2BIT_MAC(encListPtr->ethV2) |
              ((BOOL2BIT_MAC(encListPtr->nonLlcSnap)) << 1) |
              ((BOOL2BIT_MAC(encListPtr->llcSnap)) << 2));

    if (entryNum > 7)
    {
        /* get address of Protocols Encapsulation Register1 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlanRegs.protoEncBased1;

        /* write encapsulation */
        offset = ((entryNum - 8) * 3);
    }
    else
    {
        /* get address of Protocols Encapsulation Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlanRegs.protoEncBased;

        /* write encapsulation */
        offset = (entryNum * 3);
    }

    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, offset, 3, hwData);
    if (rc != GT_OK)
        return rc;

    if (entryNum > 7)
    {
        /* write validity bit */
        offset = (entryNum - 8) + 12;
    }
    else
    {
        /* write validity bit */
        offset = entryNum + 24;
    }

    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, offset, 1, 1);
    if (rc != GT_OK)
        return rc;

    /* get address of Protocols Configuration Register */
    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlanRegs.protoTypeBased;

    /* write Ether Type */
    hwData = etherType;
    wordOffset = entryNum >> 1; /* /2 */
    offset = (entryNum & 0x1) ? 16 : 0;

    regAddr += (wordOffset * 4);
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, offset, 16, hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanProtoClassGet
*
* DESCRIPTION:
*       Read etherType and encapsulation of Protocol based classification
*       for specific device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       entryNum     - entry number [0..7]
*
* OUTPUTS:
*       etherTypePtr - (pointer to) Ether Type or DSAP/SSAP
*       encListPtr   - (pointer to) encapsulation.
*       validEntryPtr - (pointer to) validity bit
*                                   GT_TRUE - the entry is valid
*                                   GT_FALSE - the entry is invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanProtoClassGet
(
    IN GT_U8                            devNum,
    IN GT_U32                           entryNum,
    OUT GT_U16                          *etherTypePtr,
    OUT CPSS_PROT_CLASS_ENCAP_STC       *encListPtr,
    OUT GT_BOOL                         *validEntryPtr
)
{
    GT_U32      hwData;              /* value to write into register    */
    GT_U32      regAddr;             /* register address                */
    GT_U32      wordOffset;          /* offset of register              */
    GT_U32      offset;              /* offset in a register            */
    GT_STATUS   rc ;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_BRG_PORT_PROTOCOL_CHECK_MAC(devNum, entryNum);
    CPSS_NULL_PTR_CHECK_MAC(encListPtr);
    CPSS_NULL_PTR_CHECK_MAC(validEntryPtr);
    CPSS_NULL_PTR_CHECK_MAC(etherTypePtr);

    if (entryNum > 7)
    {
        /* get address of Protocols Encapsulation Register1 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlanRegs.protoEncBased1;

        /* read validity bit */
        offset = (entryNum - 8) + 12;
    }
    else
    {
        /* get address of Protocols Encapsulation Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlanRegs.protoEncBased;

        /* read validity bit */
        offset = entryNum + 24;
    }

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, offset, 1, &hwData);
    if (rc != GT_OK)
        return rc;
    *validEntryPtr = BIT2BOOL_MAC(hwData & 0x1);

    if (entryNum > 7)
    {
        /* read encapsulation */
        offset = ((entryNum - 8) * 3);
    }
    else
    {
        /* read encapsulation */
        offset = (entryNum * 3);
    }

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, offset, 3, &hwData);
    if (rc != GT_OK)
        return rc;

    /* build encapsulation */
    encListPtr->ethV2 = BIT2BOOL_MAC(hwData & 0x1);
    encListPtr->nonLlcSnap = BIT2BOOL_MAC((hwData >> 1) & 0x1);
    encListPtr->llcSnap = BIT2BOOL_MAC((hwData >> 2) & 0x1);

    /* get address of Protocols Configuration Register */
    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.vlanRegs.protoTypeBased;

    /* get Ether Type */
    wordOffset = entryNum >> 1; /* /2 */
    offset = (entryNum & 0x1) ? 16 : 0;

    regAddr += (wordOffset * 4);
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, offset, 16, &hwData);
    if (rc != GT_OK)
        return rc;

    *etherTypePtr = (GT_U16)hwData;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanProtoClassInvalidate
*
* DESCRIPTION:
*       Invalidates entry of Protocol based classification for specific device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       entryNum     - entry number [0..7]
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanProtoClassInvalidate
(
    IN GT_U8                            devNum,
    IN GT_U32                           entryNum
)
{
    GT_U32      hwData;              /* value to write into register    */
    GT_U32      regAddr;             /* register address                */
    GT_U32      offset;              /* offset in a register            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_BRG_PORT_PROTOCOL_CHECK_MAC(devNum, entryNum);

    if (entryNum > 7)
    {
        /* get address of Protocols Encapsulation Register1 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlanRegs.protoEncBased1;

        /* write validity bit */
        offset = (entryNum - 8) + 12;
    }
    else
    {
        /* get address of Protocols Encapsulation Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlanRegs.protoEncBased;

        /* write validity bit */
        offset = entryNum + 24;
    }

    /* invalidate validity bit */
    hwData = 0;
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, offset, 1, hwData);
}

/*******************************************************************************
* cpssDxChBrgVlanPortProtoVlanQosSet
*
* DESCRIPTION:
*       Write Port Protocol VID and QOS Assignment values for specific device
*       and port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       port                - physical port number
*       entryNum            - entry number [0..7]
*       vlanCfgPtr          - pointer to VID parameters
*       qosCfgPtr           - pointer to QoS parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or vlanId
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       It is possible to update only VLAN or QOS parameters, when one
*       of those input pointers is NULL.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortProtoVlanQosSet
(
    IN GT_U8                                    devNum,
    IN GT_U8                                    port,
    IN GT_U32                                   entryNum,
    IN CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC    *vlanCfgPtr,
    IN CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC     *qosCfgPtr
)
{
    GT_U32      hwData;         /* data to read/write from/to register */
    GT_U32      value;          /* value to update */
    GT_U32      tableEntryIndex;/* index in VLAN_PORT_PROTOCOL table */
    GT_U32      wordNumber;     /* word number */
    GT_U32      validBitNum;    /* position of the Valid bit */
    GT_U32      cmdFirstBit;    /* VLAN/QoS command field first bit */
    GT_U32      precedenceFirstBit; /* VLAN/QoS precedence field first bit */
    GT_U32      qosProfileFirstBit; /* QoS profile field first bit */
    GT_U32      modifyDscpFirstBit; /* QoS enable modify DSCP field first bit */
    GT_U32      modifyUpFirstBit;   /* QoS enable modify UP field first bit */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);
    PRV_CPSS_DXCH_BRG_PORT_PROTOCOL_CHECK_MAC(devNum, entryNum);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* table is actually a matrix of Port (0..63) and Protocol (0..11).
           for every port are allocated 16 protocol entries (12 valid) */
        tableEntryIndex = port * 16 + entryNum;
        /* the entry has only one word */
        wordNumber = 0;

        /* Valid bit number */
        validBitNum = 15;
    }
    else
    {
        /* table entry includes all protocol info per port */
        tableEntryIndex = port;
        /* wordNumber is protocol index */
        wordNumber = entryNum;

        /* Valid bit number */
        validBitNum = 27;
    }

    if ((vlanCfgPtr == NULL) && (qosCfgPtr == NULL))
    {
        /* at least one configuration need to be defined */
        return GT_BAD_PARAM;
    }

    /* Read entry from Port Protocol VID and QOS Register */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
                                        tableEntryIndex, /* table index */
                                        wordNumber,  /* word number */
                                        0,/* start at bit 0*/
                                        28, /* 28 bits */
                                        &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* check if both VID and QOS parameters should be set,
       if not mask the appropriate bits */
    if (vlanCfgPtr == NULL)
    {
        /* reset all QoS related bits and don't change VLAN related bits */
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat and above.
               First 14 bits are related to VLAN, keep them unchanged */
            hwData &= 0x00007FFF;
        }
        else
        {
            hwData &= 0x01600FFF;
        }
    }
    else if (qosCfgPtr == NULL)
    {
        /* reset all VLAN related bits and don't change QOS related bits */
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat and above.
               Bits 16-27 are related to QOS, keep them unchanged */
            hwData &= 0x0FFF0000;
        }
        else
        {
            hwData &= 0x069FF000;
        }
    }
    else
    {
        /* reset all fields */
        hwData = 0;
    }

    if (vlanCfgPtr != NULL)
    {
        /* check validity VLAN id */
        PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanCfgPtr->vlanId);

        /* setting <ProtocolVID> */
        hwData |= vlanCfgPtr->vlanId;

        /* define field positions */
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {   /* xCat and above */
            cmdFirstBit = 12;
            precedenceFirstBit = 14;
        }
        else
        {
            cmdFirstBit = 21;
            precedenceFirstBit = 24;
        }

        /* setting <VLANCmd> */
        PRV_CPSS_CONVERT_ATTR_ASSIGN_CMD_TO_HW_VAL_MAC(
                                           value, vlanCfgPtr->vlanIdAssignCmd);
        hwData |= (value << cmdFirstBit);

        /* <VIDPrecedence> */
        PRV_CPSS_CONVERT_PRECEDENCE_TO_HW_VAL_MAC(
                                     value, vlanCfgPtr->vlanIdAssignPrecedence);
        hwData |= (value << precedenceFirstBit);
    }

    if (qosCfgPtr != NULL)
    {
        /* check profile ID */
        PRV_CPSS_DXCH_QOS_PROFILE_ID_CHECK_MAC(devNum, qosCfgPtr->qosProfileId);

        /* define field positions */
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {  /* xCat and above */
            cmdFirstBit         = 18;
            precedenceFirstBit  = 27;
            qosProfileFirstBit  = 20;
            modifyDscpFirstBit  = 17;
            modifyUpFirstBit    = 16;
        }
        else
        {
            cmdFirstBit         = 25;
            precedenceFirstBit  = 23;
            qosProfileFirstBit  = 12;
            modifyDscpFirstBit  = 19;
            modifyUpFirstBit    = 20;
        }

        /* setting <ProtocolQosProfile> */
        hwData |= (qosCfgPtr->qosProfileId << qosProfileFirstBit);

        /* setting <ModifyDSCP> */
        value = BOOL2BIT_MAC(qosCfgPtr->enableModifyDscp);
        hwData |= (value  << modifyDscpFirstBit);

        /* setting <ModifyUP> */
        value = BOOL2BIT_MAC(qosCfgPtr->enableModifyUp);
        hwData |= (value  << modifyUpFirstBit);

        /* setting <ProtocolQoSPresence> */
        PRV_CPSS_CONVERT_PRECEDENCE_TO_HW_VAL_MAC(value,
                                                  qosCfgPtr->qosAssignPrecedence);
        hwData |= (value << precedenceFirstBit);

        /* setting the Protocol <QoSProfile Cmd> */
        PRV_CPSS_CONVERT_ATTR_ASSIGN_CMD_TO_HW_VAL_MAC(value,
                                                       qosCfgPtr->qosAssignCmd);
        hwData |= (value << cmdFirstBit);
    }

    /* set validity bit */
    hwData |= (0x1 << validBitNum);

    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
                                         tableEntryIndex, /* table index */
                                         wordNumber,  /* word number */
                                         0,/* start at bit 0*/
                                         28, /* 28 bits */
                                         hwData);
    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortProtoVlanQosGet
*
* DESCRIPTION:
*       Get Port Protocol VID and QOS Assignment values for specific device
*       and port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       port                - physical port number
*       entryNum            - entry number [0..7]
*
* OUTPUTS:
*       vlanCfgPtr          - pointer to VID parameters (can be NULL)
*       qosCfgPtr           - pointer to QoS parameters (can be NULL)
*       validEntryPtr       - pointer to validity bit
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or vlanId
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      It is possible to get only one of VLAN or QOS parameters by receiving
*      one of those pointers as NULL pointer or get only validity bit.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortProtoVlanQosGet
(
    IN GT_U8                                    devNum,
    IN GT_U8                                    port,
    IN GT_U32                                   entryNum,
    OUT CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC   *vlanCfgPtr,
    OUT CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC    *qosCfgPtr,
    OUT GT_BOOL                                 *validEntryPtr
)
{
    GT_U32      hwData;         /* data to read from register */
    GT_U32      value;          /* temporary data */
    GT_U32      tableEntryIndex;/* index in VLAN_PORT_PROTOCOL table */
    GT_U32      wordNumber;     /* word number */
    GT_U32      validBitNum;    /* position of the Valid bit */
    GT_U32      cmdFirstBit;    /* VLAN/QoS command field first bit */
    GT_U32      precedenceFirstBit; /* VLAN/QoS precedence field first bit */
    GT_U32      qosProfileFirstBit; /* QoS profile field first bit */
    GT_U32      modifyDscpFirstBit; /* QoS enable modify DSCP field first bit */
    GT_U32      modifyUpFirstBit;   /* QoS enable modify UP field first bit */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);
    PRV_CPSS_DXCH_BRG_PORT_PROTOCOL_CHECK_MAC(devNum, entryNum);
    CPSS_NULL_PTR_CHECK_MAC(validEntryPtr);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* table is actually a matrix of Port (0..63) and Protocol (0..11).
           for every port are allocated 16 protocol entries (12 valid) */
        tableEntryIndex = port * 16 + entryNum;
        /* the entry has only one word */
        wordNumber = 0;

        /* Valid bit number */
        validBitNum = 15;
    }
    else
    {
        /* table entry includes all protocol info per port */
        tableEntryIndex = port;
        /* wordNumber is protocol index */
        wordNumber = entryNum;

        /* Valid bit number */
        validBitNum = 27;
    }

    /* Read entry from Port Protocol VID and QOS Register */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
                                        tableEntryIndex, /* table index */
                                        wordNumber,  /* word number */
                                        0,/* start at bit 0*/
                                        28, /* 28 bits */
                                        &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* get validity bit */
    value = (hwData >> validBitNum) & 0x1;
    *validEntryPtr = BIT2BOOL_MAC(value);

    /* check if VLAN is not NULL pointer */
    if (vlanCfgPtr != NULL)
    {
        /* define field positions */
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {   /* xCat and above */
            cmdFirstBit = 12;
            precedenceFirstBit = 14;
        }
        else
        {
            cmdFirstBit = 21;
            precedenceFirstBit = 24;
        }

        /* get VLAN entry parameters */

        /* get Protocol VID */
        vlanCfgPtr->vlanId = (GT_U16)(hwData & 0xFFF);

        /* get (VLANCmd> */
        value = (hwData >> cmdFirstBit) & 0x3;
        PRV_CPSS_CONVERT_HW_VAL_TO_ATTR_ASSIGN_CMD_MAC(vlanCfgPtr->vlanIdAssignCmd,
                                                       value);
        /* get <VIDPrecedence> */
        value = (hwData >> precedenceFirstBit) & 0x1;
        PRV_CPSS_CONVERT_HW_VAL_TO_PRECEDENCE_MAC(vlanCfgPtr->vlanIdAssignPrecedence,
                                                  value);
    }

    /* check if QOS pointer is not NULL*/
    if (qosCfgPtr != NULL)
    {
        /* define field positions */
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {   /* xCat and above */
            cmdFirstBit         = 18;
            precedenceFirstBit  = 27;
            qosProfileFirstBit  = 20;
            modifyDscpFirstBit  = 17;
            modifyUpFirstBit    = 16;
        }
        else
        {
            cmdFirstBit         = 25;
            precedenceFirstBit  = 23;
            qosProfileFirstBit  = 12;
            modifyDscpFirstBit  = 19;
            modifyUpFirstBit    = 20;
        }

        /* get QOS entry parameters */

        /* get <ProtocolQosProfile> */
        qosCfgPtr->qosProfileId = (hwData >> qosProfileFirstBit) & 0x7f;

        /* get <ModifyDSCP> */
        qosCfgPtr->enableModifyDscp = BIT2BOOL_MAC((hwData >> modifyDscpFirstBit) & 0x1);

        /* get <ModifyUP> */
        qosCfgPtr->enableModifyUp = BIT2BOOL_MAC((hwData >> modifyUpFirstBit) & 0x1);

        /* get <ProtocolQoSPresence> */
        value = (hwData >> precedenceFirstBit) & 0x1;
        PRV_CPSS_CONVERT_HW_VAL_TO_PRECEDENCE_MAC(qosCfgPtr->qosAssignPrecedence,
                                                  value);
        /* get Protocol <QoSProfile Cmd> */
        value = (hwData >> cmdFirstBit) & 0x3;
        PRV_CPSS_CONVERT_HW_VAL_TO_ATTR_ASSIGN_CMD_MAC(qosCfgPtr->qosAssignCmd,
                                                       value);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanPortProtoVlanQosInvalidate
*
* DESCRIPTION:
*       Invalidate protocol based QoS and VLAN assignment entry for
*       specific port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       port      - physical port number
*       entryNum  - entry number [0..7]
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortProtoVlanQosInvalidate
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_U32   entryNum
)
{
    GT_STATUS   rc;
    GT_U32      hwData;          /* value to write into HW   */
    GT_U32      tableEntryIndex; /* index in VLAN_PORT_PROTOCOL table */
    GT_U32      wordNumber;      /* word number */
    GT_U32      bitNum;          /* isValid bit number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);
    PRV_CPSS_DXCH_BRG_PORT_PROTOCOL_CHECK_MAC(devNum, entryNum);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* table is actually a matrix of Port (0..63) and Protocol (0..11).
           for every port are allocated 16 protocol entries (12 valid) */
        tableEntryIndex = port * 16 + entryNum;
        /* the entry has only one word */
        wordNumber = 0;
        bitNum = 15;
    }
    else
    {
        /* table entry includes all protocol info per port */
        tableEntryIndex = port;
        /* wordNumber is protocol index */
        wordNumber = entryNum;
        bitNum = 27;
    }

    hwData = 0;

    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
                                         tableEntryIndex, /* table index */
                                         wordNumber,  /* word number */
                                         bitNum,/* first bit */
                                         1, /* 1 bit */
                                         hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanLearningStateSet
*
* DESCRIPTION:
*       Sets state of VLAN based learning to specified VLAN on specified device.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - device number
*       vlanId    - vlan Id
*       status    - GT_TRUE for enable  or GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanLearningStateSet
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId,
    IN GT_BOOL  status
)
{
    GT_STATUS   rc;      /* return code*/
    GT_U32      hwData;  /* value to write to VLAN entry*/

    /* check for device type */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    /* if the user want to enable learning on a vlan then we :
    set Automatic Learning Disable - bit 0 in word 3 */
    hwData = (status == GT_TRUE) ? 0 : 1;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_VLAN_E,
                                             vlanId,/* vid */
                                             3,/* word 3 */
                                             0,/* start at bit 0*/
                                             1, /* one bit */
                                             hwData);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            97,/* start at bit 97*/
                                            1, /* one bit */
                                            hwData);
    }

    return rc;
}


/*******************************************************************************
* cpssDxChBrgVlanIsDevMember
*
* DESCRIPTION:
*       Checks if specified device has members belong to specified vlan
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE   - if port is member of vlan
*       GT_FALSE  - if port does not member of vlan
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vlanId
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL cpssDxChBrgVlanIsDevMember
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId
)
{
    GT_STATUS   rc;
    GT_U32      hwData[PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS]; /* word to read
                                                            /write entry info */
    GT_U32      hwDataMask[PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS]; /* word Mask
                                                     to read/write entry info */
    GT_BOOL     result = GT_FALSE;     /* device membership result */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* mask the vlan ports membership only --- without the tagging */
        hwDataMask[0] = 0x55000000; /* 4 ports -- 8 bits */
        hwDataMask[1] = 0x55555555; /* 16 ports -- 32 bits */
        hwDataMask[2] = 0x00001555; /* 7 ports -- 14 bits */

        if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            /* port 27, for Cheetah2 only */
            hwDataMask[3] = 0x00000100; /* 1 port -- 2 bits */
        }
        else
        {
            hwDataMask[3] = 0;
        }

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            /* the port group of lion supports 12 ports */
            hwDataMask[1] = 0x00005555; /* 8 ports -- 16 bits */
            hwDataMask[2] = 0;
            hwDataMask[3] = 0;
        }

        /* get the entry from all port groups , each port group get different ports */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
        {
            /* check if we have local port in the vlan */
            rc = prvCpssDxChPortGroupReadTableEntry(devNum,portGroupId,
                                           PRV_CPSS_DXCH_TABLE_VLAN_E,
                                           (GT_U32)vlanId,/* vlanId */
                                           hwData);
            if(rc != GT_OK)
            {
                return GT_FALSE;
            }

            result = ((hwData[0] & hwDataMask[0]) ||
                      (hwData[1] & hwDataMask[1]) ||
                      (hwData[2] & hwDataMask[2]) ||
                      (hwData[3] & hwDataMask[3]) ) ?
                    GT_TRUE : GT_FALSE;

            if(result == GT_TRUE)
            {
                /* no need to continue loop port groups , this port group already has port member */
                return GT_TRUE;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    }
    else
    {
        /* mask the vlan ports membership only --- without the tagging */

        /* members are in bits 2..65 */

        hwDataMask[0] = 0xFFFFFFFC; /* 30 ports */
        hwDataMask[1] = 0xFFFFFFFF; /* 32 ports */
        hwDataMask[2] = 0x00000003; /* 2 ports  */

        /* check if we have local port in the vlan */
        rc = prvCpssDxChReadTableEntry(devNum,
                                       PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                       (GT_U32)vlanId,/* vlanId */
                                       &hwData[0]);
        if(rc != GT_OK)
        {
            return GT_FALSE;
        }

        result = ((hwData[0] & hwDataMask[0]) ||
                  (hwData[1] & hwDataMask[1]) ||
                  (hwData[2] & hwDataMask[2]) ) ?
                GT_TRUE : GT_FALSE;
    }

    return result;
}

/*******************************************************************************
* cpssDxChBrgVlanToStpIdBind
*
* DESCRIPTION:
*       Bind VLAN to STP Id.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device Id
*       vlanId  - vlan Id
*       stpId   - STP Id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_OUT_OF_RANGE          - out of range stpId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanToStpIdBind
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId,
    IN GT_U16   stpId
)
{
    GT_STATUS   rc;
    GT_U32      hwData;     /* word to read/write entry info   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    if(stpId >= BIT_8)/* 8 bits in HW */
    {
        return GT_OUT_OF_RANGE;
    }

    hwData = stpId;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_VLAN_E,
                                             (GT_U32)vlanId,/* vid */
                                             2,/* start at word 2 */
                                             24,/* start at bit 24 */
                                             8, /* 8 bits to write */
                                             hwData);
    }
    else
    {
        /* write to ingress vlan */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            130, /* start at bit 130 */
                                            8, /* 8 bits */
                                            hwData);

        if(rc != GT_OK)
        {
            return rc;
        }

        /* write to egress vlan */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            254, /* start at bit 254 */
                                            8, /* 8 bits */
                                            hwData);
    }

    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanStpIdGet
*
* DESCRIPTION:
*       Read STP Id that bind to specified VLAN.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device Id
*       vlanId  - vlan Id
*
* OUTPUTS:
*       stpIdPtr   - pointer to STP Id
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanStpIdGet
(
    IN  GT_U8    devNum,
    IN  GT_U16   vlanId,
    OUT GT_U16   *stpIdPtr
)
{
    GT_U32      hwData;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);
    CPSS_NULL_PTR_CHECK_MAC(stpIdPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = prvCpssDxChReadTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_VLAN_E,
                                         (GT_U32)vlanId,/* vid */
                                         2,/* start at word 2 */
                                         24,/* start at bit 24 */
                                         8, /* 8 bits to read */
                                         &hwData);
    }
    else
    {
        rc = prvCpssDxChReadTableEntryField(devNum,
                                              PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                              (GT_U32)vlanId,
                                              PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                              130,/* start at bit 130 */
                                              8, /* 12 bits */
                                              &hwData);
    }

    *stpIdPtr = (GT_U16)hwData;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanEtherTypeSet
*
* DESCRIPTION:
*       Sets vlan Ether Type mode and vlan range.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* INPUTS:
*       devNum        - device number
*       etherTypeMode - mode of tagged vlan etherType. May be either
*                       CPSS_VLAN_ETHERTYPE0_E or CPSS_VLAN_ETHERTYPE1_E.
*                       See comment.
*       etherType     - the EtherType to be recognized tagged packets. See
*                       comment.
*       vidRange      - the maximum vlan range to accept for the specified
*                       etheType.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal vidRange value
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vidRange
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If the vid of the incoming packet is greater or equal (>=) than the vlan
*       range value, the packet is filtered.
*
*       Configuration of vid and range for CPSS_VLAN_ETHERTYPE1_E should be done
*       only when nested vlan is used. User can alter the etherType field of
*       the encapsulating vlan by setting a number other then 0x8100 in the
*       parameter <ethertype>.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEtherTypeSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_ETHER_MODE_ENT     etherTypeMode,
    IN  GT_U16                  etherType,
    IN  GT_U16                  vidRange
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;         /* register address */
    GT_U32      value;           /* value to write into register */
    GT_U32      offset;          /* offset in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* add for xcat, and we need just set vid range. zhangdi 2011-03-12*/
    if( (PRV_CPSS_PP_MAC(devNum)->appDevFamily) && CPSS_XCAT_E )
    {
		if(etherTypeMode == 2)
		{
			/* need not run twice on XCAT */
			return GT_OK;
		}
        /* set ether type, how to decide port0 port1 on XCAT ??? */
        /*The BobCat implements two Gigabit Ethernet (GbE) controllers that support Port0 RGMII, port1 
         *RGMIIA, this is not needed, because in ARM CPU
         */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlan1;
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr ,0, 16, etherType);
        if(rc != GT_OK)
        {
            return rc;
        }
		
		/* set vid range */
		regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlan0;
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 12, vidRange);
        if(rc != GT_OK)
        {
            return rc;
        }
		nam_syslog_dbg("*******set vlan max vid 4095 XCAT ! \n");		
		return rc;
    }
    /* add end */
	
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vidRange);
    PRV_CPSS_ETHER_TYPE_MODE_CHECK_MAC(etherTypeMode);

    /* set ether type */
    value = etherType;
    offset = (etherTypeMode == CPSS_VLAN_ETHERTYPE0_E) ? 0 : 16;

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlan1;
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr ,offset, 16, value);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* for vlan etherType coherency between ingress and egress vlan etherType
    configuration register must be configured to match the ingressVlanSel */
    regAddr =
      PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.egressVlanEtherTypeConfig;
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, offset, 16, value);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set vid range */
    offset = (etherTypeMode == CPSS_VLAN_ETHERTYPE0_E) ? 0 : 12;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlan0;

    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, offset, 12, vidRange);

    return rc ;
}

/*******************************************************************************
* cpssDxChBrgVlanTableInvalidate
*
* DESCRIPTION:
*       This function invalidates VLAN Table entries by writing 0 to the
*       first word.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - otherwise
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTableInvalidate
(
    IN GT_U8 devNum
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      hwData;          /* for read/write entry word             */
    GT_U16      vlanId;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    hwData = 0 ;

    /* reset all valid entries in vlan table */
    for(vlanId = 0 ; vlanId < PRV_CPSS_MAX_NUM_VLANS_CNS ; vlanId ++)
    {
        /* write to word 0 bit 0 in HW */
        rc = cpssDxChBrgVlanEntryInvalidate(devNum, vlanId);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Disable ingress mirroring of packets to the Ingress Analyzer */
        rc = cpssDxChBrgVlanIngressMirrorEnable(devNum, vlanId, GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanEntryValidate
*
* DESCRIPTION:
*       This function validates VLAN entry.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - device number.
*       vlanId     - VLAN id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - otherwise
*       GT_BAD_PARAM     - wrong devNum or vid
*
* COMMENTS:
*       Updated for mcpss3.4, by Autelan.
* 
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEntryValidate
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      hwData;          /* for write entry word */
    
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    hwData = 1 ;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* write to word 0 bit 0 in HW */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_VLAN_E,
                                         (GT_U32)vlanId,/* vid */
                                         0,/* start at word 0 */
                                         0,/* start at bit 0 */
                                         1, /* 1 bit */
                                         hwData);
    }
    else
    {
        /* invalidate the ingress vlan table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                              PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                              (GT_U32)vlanId,
                                              PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                              0,
                                              1,
                                              hwData);
        
        if(rc != GT_OK)
        {
            return rc;
        }

        /* invalidate the egress vlan table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                              PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                              (GT_U32)vlanId,
                                              PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                              0,
                                              1,
                                              hwData);
    }
    return rc;
}


/*******************************************************************************
* cpssDxChBrgVlanEntryInvalidate
*
* DESCRIPTION:
*       This function invalidates VLAN entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number.
*       vlanId     - VLAN id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - otherwise
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEntryInvalidate
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      hwData;          /* for write entry word */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    hwData = 0 ;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* write to word 0 bit 0 in HW */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_VLAN_E,
                                         (GT_U32)vlanId,/* vid */
                                         0,/* start at word 0 */
                                         0,/* start at bit 0 */
                                         1, /* 1 bit */
                                         hwData);
    }
    else
    {
        /* invalidate the ingress vlan table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                              PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                              (GT_U32)vlanId,
                                              PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                              0,
                                              1,
                                              0);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* invalidate the egress vlan table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                              PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                              (GT_U32)vlanId,
                                              PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                              0,
                                              1,
                                              0);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanUnkUnregFilterSet
*
* DESCRIPTION:
*       Set per VLAN filtering command for specified Unknown or Unregistered
*       packet type
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number.
*       vlanId      - VLAN ID
*       packetType  - packet type
*       cmd         - command for the specified packet type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanUnkUnregFilterSet
(
    IN GT_U8                                devNum,
    IN GT_U16                               vlanId,
    IN CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT   packetType,
    IN CPSS_PACKET_CMD_ENT                  cmd

)
{
    GT_STATUS   rc;
    GT_U32      hwValue;             /* hardware value of the command */
    GT_U32      wordOffset;          /* the word number in the VLAN Table */
    GT_U32      bitOffset;           /* bit offset in the word */
    GT_U32      globalOffset;/* relevant only to splitTablesSupported */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    /* check the type of the packet */
    switch (packetType)
    {
        case CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E:
             /* set unknown unicast - word 0, bits 12-14 */
            wordOffset = 0;
            bitOffset = 12;

            globalOffset = 75;
            break;

        case CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IP_MCAST_E:
            /* set unregistered non IP multicast command - word 0, bits 3-5 */
            wordOffset = 0;
            bitOffset = 3;

            globalOffset = 66;
            break;

        case CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_MCAST_E:
            /* set unregistered IPv4 multicast command - word 0, bits 6-8 */
            wordOffset = 0;
            bitOffset = 6;

            globalOffset = 69;
            break;

        case CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV6_MCAST_E:
            /* set unregistered IPv6 multicast command - word 0, bits 9-11 */
            wordOffset = 0;
            bitOffset = 9;

            globalOffset = 72;
            break;

        case CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IPV4_BCAST_E:
            /* set unregistered non IPv4 broadcast command - word 2,
                                                            bits 21-23 */
            wordOffset = 2;
            bitOffset = 21;

            globalOffset = 89;
            break;

        case CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_BCAST_E:
            /* set unregistered IPv4 broadcast command - word 2, bits 18-20 */
            wordOffset = 2;
            bitOffset = 18;

            globalOffset = 86;
            break;
        default:
            return GT_BAD_PARAM;

    }

    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(hwValue, cmd)

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                             (GT_U32)vlanId,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             globalOffset,
                                             3, /* 3 bits */
                                             hwValue);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_VLAN_E,
                                             (GT_U32)vlanId,/* vid */
                                             wordOffset,    /* word # */
                                             bitOffset,     /* start at bit */
                                             3, /* 3 bits */
                                             hwValue);
        return rc;
    }

}

/*******************************************************************************
* cpssDxChBrgVlanForcePvidEnable
*
* DESCRIPTION:
*       Set Port VID Assignment mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       port     - port number
*       enable   - GT_TRUE  - PVID is assigned to all packets.
*                  GT_FALSE - PVID is assigned to untagged or priority tagged
*                             packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanForcePvidEnable
(
    IN GT_U8   devNum,
    IN GT_U8   port,
    IN GT_BOOL enable
)
{
    GT_STATUS   rc;
    GT_U32      startWord;  /* the table word at which the field starts */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_U32      hwData;     /* data to write to Hw */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);

    hwData = BOOL2BIT_MAC(enable);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        startWord = 0;
        startBit  = 27;
    }
    else
    {
        startWord = 1;
        startBit  = 15;
    }

    /* write the value "forceEn" to <PVIDMode> field of port-vlan-qos table  */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,/* port */
                                         startWord,
                                         startBit,
                                         1, /* set 1 bits */
                                         hwData);
     return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanForcePvidEnableGet
*
* DESCRIPTION:
*       Get Port VID Assignment mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       port     - port number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE  - PVID is assigned to all packets.
*                     GT_FALSE - PVID is assigned to untagged or priority tagged
*                                packets
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanForcePvidEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS   rc;
    GT_U32      startWord;  /* the table word at which the field starts */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_U32      hwData;     /* data to write to Hw */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        startWord = 0;
        startBit  = 27;
    }
    else
    {
        startWord = 1;
        startBit  = 15;
    }

    /* read the value "forceEn" from <PVIDMode> field of port-vlan-qos table  */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        port,/* port */
                                        startWord,
                                        startBit,
                                        1, /* set 1 bits */
                                        &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwData);

     return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanPortVidPrecedenceSet
*
* DESCRIPTION:
*       Set Port VID Precedence.
*       Relevant for packets with assigned VLAN ID of Pvid.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       port        - port number
*       precedence  - precedence type - soft or hard
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVidPrecedenceSet
(
    IN GT_U8                                        devNum,
    IN GT_U8                                        port,
    IN CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  precedence
)
{
    GT_STATUS   rc;
    GT_U32      startWord;  /* the table word at which the field starts */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_U32      hwData;/*data to write to Hw*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);

    PRV_CPSS_CONVERT_PRECEDENCE_TO_HW_VAL_MAC(hwData, precedence);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        startWord = 0;
        startBit  = 29;
    }
    else
    {
        startWord = 1;
        startBit  = 16;
    }

    /* write the value to <PVIDPrecedence> field of port-vlan-qos table */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         port,/* port */
                                         startWord,
                                         startBit,
                                         1, /* 1 bit */
                                         hwData);
    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortVidPrecedenceGet
*
* DESCRIPTION:
*       Get Port VID Precedence.
*       Relevant for packets with assigned VLAN ID of Pvid.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       port        - port number
*
* OUTPUTS:
*       precedencePtr - (pointer to) precedence type - soft or hard
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVidPrecedenceGet
(
    IN GT_U8                                        devNum,
    IN GT_U8                                        port,
    OUT CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  *precedencePtr
)
{
    GT_STATUS   rc;
    GT_U32      startWord;  /* the table word at which the field starts */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_U32      hwData;/*data to read from Hw*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(precedencePtr);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        startWord = 0;
        startBit  = 29;
    }
    else
    {
        startWord = 1;
        startBit  = 16;
    }

    /* read the value to <PVIDPrecedence> field of port-vlan-qos table */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        port,/* port */
                                        startWord,
                                        startBit,
                                        1, /* 1 bit */
                                        &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_CONVERT_HW_VAL_TO_PRECEDENCE_MAC(*precedencePtr, hwData)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanIpUcRouteEnable
*
* DESCRIPTION:
*       Enable/Disable IPv4/Ipv6 Unicast Routing on Vlan
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan ID
*       protocol    - ipv4 or ipv6
*       enable      - GT_TRUE - enable ip unicast routing;
*                     GT_FALSE -  disable ip unicast routing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpUcRouteEnable
(
    IN GT_U8                                    devNum,
    IN GT_U16                                   vlanId,
    IN CPSS_IP_PROTOCOL_STACK_ENT               protocol,
    IN GT_BOOL                                  enable
)
{
    GT_U32      globalOffset;/* relevant only to splitTablesSupported */
    GT_U32      bitOffset;      /* bit offset in the VLAN Entry word */
    GT_U32      hwData;         /* data to write to register */
    GT_STATUS   rc;             /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    /* when the user want to Ip routing or disable */
    hwData = BOOL2BIT_MAC(enable);
    rc = GT_OK;

    switch(protocol)
    {
        case CPSS_IP_PROTOCOL_IPV4_E :
            bitOffset = 14;
            globalOffset = 92;
            break;
        case CPSS_IP_PROTOCOL_IPV4V6_E:
            /* ipv4 */
            rc = cpssDxChBrgVlanIpUcRouteEnable(devNum,vlanId,
                                                 CPSS_IP_PROTOCOL_IPV4_E,
                                                 enable);
            if(GT_OK != rc)
            {
                return rc;
            }

            /* ipv6 */
            return cpssDxChBrgVlanIpUcRouteEnable(devNum,vlanId,
                                                 CPSS_IP_PROTOCOL_IPV6_E,
                                                 enable);
        case CPSS_IP_PROTOCOL_IPV6_E :
            bitOffset = 16;
            globalOffset = 94;
            break;
        default:
            return GT_BAD_PARAM;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_VLAN_E,
                                           (GT_U32)vlanId,/* vid */
                                           2,             /* word 2 */
                                           bitOffset,     /* offset */
                                           1,             /* 1 bit */
                                           hwData);
    }
    else
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                               (GT_U32)vlanId,/* vid */
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                               globalOffset,  /* offset */
                                               1,             /* 1 bit */
                                               hwData);
    }
}

/*******************************************************************************
* cpssDxChBrgVlanIpMcRouteEnable
*
* DESCRIPTION:
*       Enable/Disable IPv4/Ipv6 Multicast Routing on Vlan
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan ID
*       protocol    - ipv4 or ipv6
*       enable      - GT_TRUE - enable ip multicast routing;
*                     GT_FALSE -  disable ip multicast routing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpMcRouteEnable
(
    IN GT_U8                            devNum,
    IN GT_U16                           vlanId,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocol,
    IN GT_BOOL                          enable
)
{
    GT_U32      globalOffset;/* relevant only to splitTablesSupported */
    GT_U32      bitOffset;    /* bit offset in the VLAN Entry word */
    GT_U32      hwData;       /* data to write to register */
    GT_STATUS   rc;           /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    /* when the user want to Ip routing or disable */
    hwData = BOOL2BIT_MAC(enable);
    rc = GT_OK;

    switch(protocol)
    {
        case CPSS_IP_PROTOCOL_IPV4_E :
            bitOffset = 15;
            globalOffset = 93;
            break;

        case CPSS_IP_PROTOCOL_IPV4V6_E:
            /* ipv4 */
            rc = cpssDxChBrgVlanIpMcRouteEnable(devNum,vlanId,
                                                 CPSS_IP_PROTOCOL_IPV4_E,
                                                 enable);
            if(GT_OK != rc)
            {
                return rc;
            }

            /* ipv6 */
            return cpssDxChBrgVlanIpMcRouteEnable(devNum,vlanId,
                                                 CPSS_IP_PROTOCOL_IPV6_E,
                                                 enable);
        case CPSS_IP_PROTOCOL_IPV6_E :
            bitOffset = 17;
            globalOffset = 95;
            break;

        default:
            return GT_BAD_PARAM;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_VLAN_E,
                                           (GT_U32)vlanId,/* vid */
                                           2,             /* word 2 */
                                           bitOffset,     /* offset */
                                           1,             /* 1 bit */
                                           hwData);
    }
    else
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                               (GT_U32)vlanId,/* vid */
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                               globalOffset,  /* offset */
                                               1,             /* 1 bit */
                                               hwData);
    }
}

/*******************************************************************************
* cpssDxChBrgVlanNASecurEnable
*
* DESCRIPTION:
*      This function enables/disables per VLAN the generation of
*      security breach event for packets with unknown Source MAC addresses.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN Id
*       enable  - GT_TRUE - enable generation of security breach event
*                 GT_FALSE- disable generation of security breach event
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanNASecurEnable
(
    IN GT_U8           devNum,
    IN GT_U16          vlanId,
    IN GT_BOOL         enable
)
{
    GT_U32  hwData;         /* for write entry word */
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    /* when the user want to enable NA security breach on the vlan we
       clear the bit of NA NOT security breach */
    hwData = (enable == GT_TRUE) ? 0 : 1;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* set bit 1 in word 0 accordingly */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_VLAN_E,
                                         (GT_U32)vlanId,/* vid */
                                         0,/* word 0 */
                                         1,/* start at bit 1*/
                                         1, /* one bit */
                                         hwData);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                             (GT_U32)vlanId,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             1,
                                             1, /* 1 bit */
                                             hwData);
    }

    return rc;
}



/*******************************************************************************
* cpssDxChBrgVlanIgmpSnoopingEnable
*
* DESCRIPTION:
*       Enable/Disable IGMP trapping or mirroring to the CPU according to the
*       global setting.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - the VLAN-ID in which IGMP trapping is enabled/disabled.
*       enable  - GT_TRUE - enable
*                 GT_FALSE - disable
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIgmpSnoopingEnable
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_BOOL enable
)
{
    GT_U32      value;  /* value to write into register */
    GT_STATUS   rc;     /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    value = BOOL2BIT_MAC(enable);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* VLAN<vlanId> Entry Word<0>, field <IPv4IGMPToCPUEn> */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                    PRV_CPSS_DXCH_TABLE_VLAN_E,
                                    (GT_U32)vlanId,
                                    0,
                                    15,
                                    1,
                                    value);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        (GT_U32)vlanId,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                        78,
                                        1,
                                        value);
    }


    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanIgmpSnoopingEnableGet
*
* DESCRIPTION:
*       Gets the status of IGMP trapping or mirroring to the CPU according to the
*       global setting.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - the VLAN-ID in which IGMP trapping is enabled/disabled.
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - enable
*                    GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIgmpSnoopingEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanId,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;  /* value to write into register */
    GT_STATUS   rc;     /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* VLAN<vlanId> Entry Word<0>, field <IPv4IGMPToCPUEn> */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                    PRV_CPSS_DXCH_TABLE_VLAN_E,
                                    (GT_U32)vlanId,
                                    0,
                                    15,
                                    1,
                                    &value);
    }
    else
    {
        rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        (GT_U32)vlanId,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                        78,
                                        1,
                                        &value);
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*****************************************************************************
* cpssDxChBrgVlanIpCntlToCpuSet
*
* DESCRIPTION:
*       Enable/disable IP control traffic trapping/mirroring to CPU.
*       When set, this enables the following control traffic to be eligible
*       for mirroring/trapping to the CPU:
*        - ARP
*        - IPv6 Neighbor Solicitation
*        - IPv4/v6 Control Protocols Running Over Link-Local Multicast
*        - RIPv1 MAC Broadcast
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN ID
*       ipCntrlType - IP control type
*                     DXCH devices support : CPSS_DXCH_BRG_IP_CTRL_NONE_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E
*                     DXCH2 and above devices support: like DXCH device ,and also
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV6_E
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or IP control type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpCntlToCpuSet
(
    IN GT_U8                               devNum,
    IN GT_U16                              vlanId,
    IN CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT      ipCntrlType
)
{
    GT_U32      value;  /* value to write into register */
    GT_U32      value1; /* value to write into register */
    GT_STATUS   rc;     /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    switch(ipCntrlType)
    {

        case CPSS_DXCH_BRG_IP_CTRL_NONE_E:
            value = 0;
            value1 = 0;
            break;

        case CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E:
            value = 1;
            value1 = 1;
            break;

        case CPSS_DXCH_BRG_IP_CTRL_IPV4_E:
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
            {
                return GT_BAD_PARAM;
            }
            value = 1;
            value1 = 0;
            break;

        case CPSS_DXCH_BRG_IP_CTRL_IPV6_E:
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
            {
                return GT_BAD_PARAM;
            }
            value = 0;
            value1 = 1;
            break;

        default:
            return GT_BAD_PARAM;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* VLAN<vlanId> Entry Word<0>, field <IPv4ControlToCPUEnable> */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_TABLE_VLAN_E,
                                            (GT_U32)vlanId,
                                            0,
                                            18,
                                            1,
                                            value);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            /* VLAN<vlanId> Entry Word<3>, field <IPv6ControlToCPUEnable> */
            rc = prvCpssDxChWriteTableEntryField(devNum,
                                                PRV_CPSS_DXCH_TABLE_VLAN_E,
                                                (GT_U32)vlanId,
                                                3,
                                                7,
                                                1,
                                                value1);

        }
    }
    else
    {
        /*Ipv4ControlToCpuEn*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            83,
                                            1,
                                            value);
        if(rc != GT_OK)
        {
            return rc;
        }

        /*Ipv6ControlToCpuEn*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            103,
                                            1,
                                            value1);
    }

    return rc;
}

/*****************************************************************************
* cpssDxChBrgVlanIpCntlToCpuGet
*
* DESCRIPTION:
*       Gets IP control traffic trapping/mirroring to CPU status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN ID
*
* OUTPUTS:
*       ipCntrlTypePtr - IP control type
*                     DXCH devices support : CPSS_DXCH_BRG_IP_CTRL_NONE_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E
*                     DXCH2 and above devices support: like DXCH device ,and also
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV6_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or IP control type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpCntlToCpuGet
(
    IN  GT_U8                               devNum,
    IN  GT_U16                              vlanId,
    OUT CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT      *ipCntrlTypePtr
)
{
    GT_U32      value=0;  /* value to read from register */
    GT_U32      value1=0; /* value to read from register */
    GT_STATUS   rc;      /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);
    CPSS_NULL_PTR_CHECK_MAC(ipCntrlTypePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* VLAN<vlanId> Entry Word<0>, field <IPv4ControlToCPUEnable> */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_TABLE_VLAN_E,
                                            (GT_U32)vlanId,
                                            0,
                                            18,
                                            1,
                                            &value);

        if(rc != GT_OK)
            return rc;

        if(PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            /* VLAN<vlanId> Entry Word<3>, field <IPv6ControlToCPUEnable> */
            rc = prvCpssDxChReadTableEntryField(devNum,
                                                PRV_CPSS_DXCH_TABLE_VLAN_E,
                                                (GT_U32)vlanId,
                                                3,
                                                7,
                                                1,
                                                &value1);
            if(rc != GT_OK)
                return rc;

        }
    }
    else
    {
        /*Ipv4ControlToCpuEn*/
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            83,
                                            1,
                                            &value);
        if(rc != GT_OK)
        {
            return rc;
        }

        /*Ipv6ControlToCpuEn*/
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            103,
                                            1,
                                            &value1);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if((value == 0)&&(value1 == 0))
    {
        *ipCntrlTypePtr = CPSS_DXCH_BRG_IP_CTRL_NONE_E;
    }
    else
    {
        if((value == 1)&&(value1 == 1))
        {
            *ipCntrlTypePtr = CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E;
        }
        else
        {
            if((value == 1)&&(value1 == 0))
            {
                if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
                {
                    return GT_BAD_STATE;
                }
                else
                {
                    *ipCntrlTypePtr = CPSS_DXCH_BRG_IP_CTRL_IPV4_E;
            }
            }
            else/* ((value == 0)&&(value1 == 1)) */
            {
                if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
                {
                    return GT_BAD_STATE;
                }
                else
                {
                    *ipCntrlTypePtr = CPSS_DXCH_BRG_IP_CTRL_IPV6_E;
            }
        }
    }
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanIpV6IcmpToCpuEnable
*
* DESCRIPTION:
*       Enable/Disable ICMPv6 trapping or mirroring to
*       the CPU, according to global ICMPv6 message type
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum   - device number
*        vlanId   - VLAN ID
*        enable   - GT_TRUE - enable
*                   GT_FALSE - disable
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpV6IcmpToCpuEnable
(
    IN  GT_U8      devNum,
    IN  GT_U16     vlanId,
    IN  GT_BOOL    enable
)
{
    GT_U32      hwData;     /* data to write into register */
    GT_STATUS   rc;         /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    hwData = BOOL2BIT_MAC(enable);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* VLAN<vlanId> Entry Word<0>, field <IPv6ICMPToCPUEn> */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_VLAN_E,
                                        (GT_U32)vlanId,
                                        0,  /* word0 */
                                        17,
                                        1,
                                        hwData);
    }
    else
    {
        /* <Ipv6IcmpToCpuEn> */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            82,
                                            1,
                                            hwData);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanIpV6IcmpToCpuEnableGet
*
* DESCRIPTION:
*       Gets status of ICMPv6 trapping or mirroring to
*       the CPU, according to global ICMPv6 message type
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum   - device number
*        vlanId   - VLAN ID
*
* OUTPUTS:
*        enablePtr   - GT_TRUE - enable
*                      GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpV6IcmpToCpuEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U16     vlanId,
    OUT GT_BOOL    *enablePtr
)
{
    GT_U32      hwData;     /* data to write into register */
    GT_STATUS   rc;         /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* VLAN<vlanId> Entry Word<0>, field <IPv6ICMPToCPUEn> */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_VLAN_E,
                                        (GT_U32)vlanId,
                                        0,  /* word0 */
                                        17,
                                        1,
                                        &hwData);
    }
    else
    {
        /* <Ipv6IcmpToCpuEn> */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            82,
                                            1,
                                            &hwData);
    }

    *enablePtr = BIT2BOOL_MAC(hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanUdpBcPktsToCpuEnable
*
* DESCRIPTION:
*     Enable or disable per Vlan the Broadcast UDP packets Mirror/Trap to the
*     CPU based on their destination UDP port.  Destination UDP port, command
*     (trap or mirror) and CPU code configured by
*     cpssDxChBrgVlanUdpBcPktsToCpuEnable
*
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*       vlanId -  vlan id
*       enable - Enable/disable UDP Trap Mirror.
*                GT_TRUE - enable trapping or mirroring, GT_FALSE - disable.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanUdpBcPktsToCpuEnable
(
    IN  GT_U8       devNum,
    IN  GT_U16      vlanId,
    IN  GT_BOOL     enable
)
{
    GT_U32      hwData;     /* data to write into register */

    /* only for Cheetah2 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    hwData = BOOL2BIT_MAC(enable);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* Enable/disable trapping or mirroring BC UDP packets based on the
        destination UDP port for UDP relay support */
        return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_VLAN_E,
                                           (GT_U32)vlanId,
                                           3,   /* word3 */
                                           6,
                                           1,
                                           hwData);
    }
    else
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                               (GT_U32)vlanId,
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                               102,
                                               1,
                                               hwData);
    }
}

/*******************************************************************************
* cpssDxChBrgVlanUdpBcPktsToCpuEnableGet
*
* DESCRIPTION:
*     Gets per Vlan if the Broadcast UDP packets are Mirror/Trap to the
*     CPU based on their destination UDP port.
*
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*       vlanId -  vlan id
*
* OUTPUTS:
*       enablePtr - UDP Trap Mirror status
*                   GT_TRUE - enable trapping or mirroring, GT_FALSE - disable.

*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanUdpBcPktsToCpuEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U16      vlanId,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS   rc;
    GT_U32      hwData;     /* data to write into register */

    /* only for Cheetah2 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = prvCpssDxChReadTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_VLAN_E,
                                           (GT_U32)vlanId,
                                           3,   /* word3 */
                                           6,
                                           1,
                                           &hwData);
    }
    else
    {
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            102,
                                            1,
                                            &hwData);
    }


    *enablePtr = BIT2BOOL_MAC(hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanIpv6SourceSiteIdSet
*
* DESCRIPTION:
*       Sets a vlan ipv6 site id
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - the vlan.
*       siteId          - the site id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpv6SourceSiteIdSet
(
    IN GT_U8                                devNum,
    IN GT_U16                               vlanId,
    IN CPSS_IP_SITE_ID_ENT                  siteId
)
{
    GT_U32      hwData;         /* data to write to register */
    GT_STATUS   rc;     /* status to return */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    if(siteId != CPSS_IP_SITE_ID_INTERNAL_E &&
       siteId != CPSS_IP_SITE_ID_EXTERNAL_E)
    {
        return GT_BAD_PARAM;
    }

    /* when the user want to Ip routing or disable */
    hwData = (siteId == CPSS_IP_SITE_ID_INTERNAL_E) ? 0 : 1;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_VLAN_E,
                                         (GT_U32)vlanId,/* vid */
                                         0,             /* word 0 */
                                         23,            /* bit 23*/
                                         1,             /* one bit */
                                         hwData);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                             (GT_U32)vlanId,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             96,
                                             1,             /* one bit */
                                             hwData);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanIpmBridgingEnable
*
* DESCRIPTION:
*       IPv4/Ipv6 Multicast Bridging Enable
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device id
*       vlanId  - Vlan Id
*       ipVer   - IPv4 or Ipv6
*       enable  - GT_TRUE - IPv4 multicast packets are bridged
*                   according to ipv4IpmBrgMode
*                 GT_FALSE - IPv4 multicast packets are bridged
*                   according to MAC DA
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpmBridgingEnable
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      vlanId,
    IN  CPSS_IP_PROTOCOL_STACK_ENT  ipVer,
    IN  GT_BOOL                     enable
)
{
    GT_U32  bitOffset;      /* bit offset in the VLAN Entry word */
    GT_U32  dataLength;     /* length of write data */
    GT_U32  hwData;         /* data to write to register */
    GT_U32  globalOffset;/* relevant only to splitTablesSupported */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    hwData = BOOL2BIT_MAC(enable);

    switch (ipVer)
    {
        case CPSS_IP_PROTOCOL_IPV6_E:
            bitOffset = 22;
            dataLength = 1;
            globalOffset = 85;
            break;
        case CPSS_IP_PROTOCOL_IPV4_E:
            bitOffset = 21;
            dataLength = 1;
            globalOffset = 84;
            break;
        case CPSS_IP_PROTOCOL_IPV4V6_E:
            bitOffset = 21;
            dataLength = 2;
            hwData = hwData | (hwData << 1);
            globalOffset = 84;
            break;
        default:
            return GT_BAD_PARAM;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_VLAN_E,
                                           (GT_U32)vlanId, /* vid    */
                                           0,              /* word 0 */
                                           bitOffset,      /* offset */
                                           dataLength,
                                           hwData);
    }
    else
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                               (GT_U32)vlanId,
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                               globalOffset,
                                               dataLength,
                                               hwData);
    }
}

/*******************************************************************************
* cpssDxChBrgVlanIpmBridgingModeSet
*
* DESCRIPTION:
*       Sets the IPM bridging mode of Vlan. Relevant when IPM Bridging enabled.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device id
*       vlanId      - Vlan Id
*       ipVer       - IPv4 or Ipv6
*       ipmMode    - IPM bridging mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpmBridgingModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      vlanId,
    IN  CPSS_IP_PROTOCOL_STACK_ENT  ipVer,
    IN  CPSS_BRG_IPM_MODE_ENT       ipmMode
)
{
    GT_U32  bitOffset;      /* bit offset in the VLAN Entry word */
    GT_U32  dataLength;     /* length of write data */
    GT_U32  hwData;         /* data to write to register */
    GT_U32  globalOffset;/* relevant only to splitTablesSupported */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    switch (ipmMode)
    {
        case CPSS_BRG_IPM_SGV_E:
            hwData = 0;
            break;
        case CPSS_BRG_IPM_GV_E:
            hwData = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    switch (ipVer)
    {
        case CPSS_IP_PROTOCOL_IPV6_E:
            bitOffset = 20;
            dataLength = 1;
            globalOffset = 80;
            break;
        case CPSS_IP_PROTOCOL_IPV4_E:
            bitOffset = 19;
            dataLength = 1;
            globalOffset = 79;
            break;
        case CPSS_IP_PROTOCOL_IPV4V6_E:
            bitOffset = 19;
            dataLength = 2;
            hwData = hwData | (hwData << 1);
            globalOffset = 79;
            break;
        default:
            return GT_BAD_PARAM;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_VLAN_E,
                                           (GT_U32)vlanId,  /* vid    */
                                           0,               /* word 0 */
                                           bitOffset,       /* offset */
                                           dataLength,
                                           hwData);
    }
    else
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                               (GT_U32)vlanId,
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                               globalOffset,
                                               dataLength,
                                               hwData);
    }
}

/*******************************************************************************
* cpssDxChBrgVlanIngressMirrorEnable
*
* DESCRIPTION:
*       Enable/Disable ingress mirroring of packets to the Ingress Analyzer
*       port for packets assigned the given VLAN-ID.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device id
*       vlanId  - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*       enable  - GT_TRUE - Ingress mirrored traffic assigned to
*                          this Vlan to the analyzer port
*                 GT_FALSE - Don't ingress mirrored traffic assigned
*                          to this Vlan to the analyzer port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIngressMirrorEnable
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_BOOL enable
)
{
    GT_U32      hwData;   /* data to write to Hw */
    GT_STATUS   rc;      /* function call return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    hwData = BOOL2BIT_MAC(enable);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_VLAN_E,
                                         (GT_U32)vlanId,
                                         0,
                                         16,
                                         1,
                                         hwData);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                             (GT_U32)vlanId,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             81,
                                             1,
                                             hwData);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortAccFrameTypeSet
*
* DESCRIPTION:
*       Set port access frame type.
*       There are three types of port configuration:
*        - Admit only Vlan tagged frames
*        - Admit only tagged and priority tagged,
*        - Admit all frames.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*       frameType -
*               CPSS_PORT_ACCEPT_FRAME_TAGGED_E    -
*                           All Untagged/Priority Tagged packets received on
*                           this port are discarded. Only Tagged accepted.
*               CPSS_PORT_ACCEPT_FRAME_ALL_E       -
*                           Both Tagged and Untagged packets are accepted
*                           on the port.
*               CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E  -
*                           All Tagged packets received on this port are
*                           discarded. Only Untagged/Priority Tagged accepted.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortAccFrameTypeSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               portNum,
    IN  CPSS_PORT_ACCEPT_FRAME_TYPE_ENT     frameType
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      hwData;    /* data to write to register */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    /* get address of Protocols Encapsulation Register */
    regAddr =
        (PRV_CPSS_DXCH_DEV_REGS_MAC(devNum))->
                        bridgeRegs.vlanRegs.portPrvVlan[portNum];

    switch (frameType)
    {
        case CPSS_PORT_ACCEPT_FRAME_ALL_E:
            hwData = 0;
            break;
        case CPSS_PORT_ACCEPT_FRAME_TAGGED_E:
            hwData = 1;
            break;
        case CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E:
            hwData = 2;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Set port access frame type. */
    return prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, regAddr, 21, 2, hwData);
}

/*******************************************************************************
* cpssDxChBrgVlanPortAccFrameTypeGet
*
* DESCRIPTION:
*       Get port access frame type.
*       There are three types of port configuration:
*        - Admit only Vlan tagged frames
*        - Admit only tagged and priority tagged,
*        - Admit all frames.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*
* OUTPUTS:
*       frameTypePtr -
*               CPSS_PORT_ACCEPT_FRAME_TAGGED_E    -
*                           All Untagged/Priority Tagged packets received on
*                           this port are discarded. Only Tagged accepted.
*               CPSS_PORT_ACCEPT_FRAME_ALL_E       -
*                           Both Tagged and Untagged packets are accepted
*                           on the port.
*               CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E  -
*                           All Tagged packets received on this port are
*                           discarded. Only Untagged/Priority Tagged accepted.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on bad value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortAccFrameTypeGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               portNum,
    OUT CPSS_PORT_ACCEPT_FRAME_TYPE_ENT     *frameTypePtr
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      hwData;    /* data to write to register */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    GT_STATUS   rc;     /* return code  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(frameTypePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    /* get address of Protocols Encapsulation Register */
    regAddr =
        (PRV_CPSS_DXCH_DEV_REGS_MAC(devNum))->
                        bridgeRegs.vlanRegs.portPrvVlan[portNum];


    /* Set port access frame type. */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, 21, 2, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (hwData)
    {
        case 0:
            *frameTypePtr = CPSS_PORT_ACCEPT_FRAME_ALL_E;
            break;
        case 1:
            *frameTypePtr = CPSS_PORT_ACCEPT_FRAME_TAGGED_E;
            break;
        case 2:
            *frameTypePtr = CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgVlanMruProfileIdxSet
*
* DESCRIPTION:
*     Set Maximum Receive Unit MRU profile index for a VLAN.
*     MRU VLAN profile sets maximum packet size that can be received
*     for the given VLAN.
*     Value of MRU for profile is set by cpssDxChBrgVlanMruProfileValueSet.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - vlan id
*       mruIndex - MRU profile index [0..7]
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanMruProfileIdxSet
(
    IN GT_U8     devNum,
    IN GT_U16    vlanId,
    IN GT_U32    mruIndex
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);
    PRV_CPSS_DXCH_BRG_MRU_INDEX_CHECK_MAC(mruIndex);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* Set Maximum Receive Unit MRU profile index for the VLAN */
        return prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_VLAN_E,
                                         (GT_U32)vlanId,/* vid */
                                         3,/* start at word 3 */
                                         2,/* start at bit 2*/
                                         3, /* 3 bits */
                                         mruIndex);
    }
    else
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                             (GT_U32)vlanId,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             99,
                                             3, /* 3 bits */
                                             mruIndex);
    }

}

/*******************************************************************************
* cpssDxChBrgVlanMruProfileValueSet
*
* DESCRIPTION:
*     Set MRU value for a VLAN MRU profile.
*     MRU VLAN profile sets maximum packet size that can be received
*     for the given VLAN.
*     cpssDxChBrgVlanMruProfileIdxSet set index of profile for a specific VLAN.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       mruIndex - MRU profile index [0..7]
*       mruValue - MRU value in bytes [0..0xFFFF]
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanMruProfileValueSet
(
    IN GT_U8     devNum,
    IN GT_U32    mruIndex,
    IN GT_U32    mruValue
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      fieldOffset;    /* field offset of the data in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_BRG_MRU_INDEX_CHECK_MAC(mruIndex);

    if(mruValue >= BIT_16)
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                bridgeRegs.vlanMruProfilesConfigReg;

    regAddr = regAddr + (mruIndex / 2) * 0x4;
    fieldOffset = (mruIndex % 2) * 16;

    /* Set Maximum Receive Unit MRU profile value for the index */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset,
                                     16, mruValue);

}

/*******************************************************************************
* cpssDxChBrgVlanNaToCpuEnable
*
* DESCRIPTION:
*     Enable/Disable New Address (NA) Message Sending to CPU per VLAN  .
*     To send NA to CPU both VLAN and port must be set to send NA to CPU.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - vlan id
*       enable   - GT_TRUE  - enable New Address Message Sending to CPU
*                  GT_FALSE - disable New Address Message Sending to CPU
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanNaToCpuEnable
(
    IN GT_U8     devNum,
    IN GT_U16    vlanId,
    IN GT_BOOL   enable
)
{
    GT_U32      hwData; /* data to write to register */
    GT_STATUS   rc;     /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    hwData = BOOL2BIT_MAC(enable);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_VLAN_E,
                                         (GT_U32)vlanId,
                                         3, /* word 3 */
                                         1, /* start at bit 1 */
                                         1, /* 1 bit */
                                         hwData);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                             (GT_U32)vlanId,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             98,
                                             1, /* 1 bit */
                                             hwData);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanVrfIdSet
*
* DESCRIPTION:
*       Sets vlan Virtual Router ID
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - the vlan id.
*       vrfId           - the virtual router id (0..4095).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_OUT_OF_RANGE          - when vrfId is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanVrfIdSet
(
    IN GT_U8                   devNum,
    IN GT_U16                  vlanId,
    IN GT_U32                  vrfId
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT2_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    /* check vrf ID */
    if (vrfId >= BIT_12)
    {
        return GT_BAD_PARAM;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                          PRV_CPSS_DXCH3_TABLE_VRF_ID_E,
                                          (GT_U32)vlanId,
                                          0, /* word 0 */
                                          0,
                                          12, /* 12 bits */
                                          vrfId);
    }
    else
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                              PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                              (GT_U32)vlanId,
                                              PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                              116,
                                              12, /* 12 bits */
                                              vrfId);
    }
}

/*******************************************************************************
* cpssDxChBrgVlanBridgingModeSet
*
* DESCRIPTION:
*     Set bridging mode either VLAN-aware (802.1Q) or VLAN-unaware (802.1D) mode
*     The device supports a VLAN-unaware mode for 802.1D bridging.
*     When this mode is enabled:
*      - In VLAN-unaware mode, the device does not perform any packet
*        modifications. Packets are always transmitted as-received, without any
*        modification (i.e., packets received tagged are transmitted tagged;
*        packets received untagged are transmitted untagged).
*      - Packets are implicitly assigned with VLAN-ID 1, regardless of
*        VLAN-assignment mechanisms.
*      - Packets are implicitly assigned a VIDX Multicast group index 0xFFF,
*        indicating that the packet flood domain is according to the VLAN-in
*        this case VLAN 1. Registered Multicast is not supported in this mode.
*      All other features are operational in this mode, including internal
*      packet QoS, trapping, filtering, policy, etc.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       brgMode  - bridge mode: IEEE 802.1Q bridge or IEEE 802.1D bridge
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanBridgingModeSet
(
    IN GT_U8                devNum,
    IN CPSS_BRG_MODE_ENT    brgMode
)
{
    GT_U32      hwData;     /* data to write to register */
    GT_U32      regAddr;    /* hw register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch (brgMode)
    {
        case CPSS_BRG_MODE_802_1Q_E:
            hwData = 0;
            break;
        case CPSS_BRG_MODE_802_1D_E:
            hwData = 1;
            break;
        default:
            return GT_BAD_PARAM;

    }

    /* get address of Global Control Register */
    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl;

    /* set Vlan - unaware mode in Global Control Register */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 3, 1, hwData);
}

/*******************************************************************************
* cpssDxChBrgVlanPortTranslationEnableSet
*
* DESCRIPTION:
*       Enable/Disable Vlan Translation per ingress or egress port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - the device number
*       port            - physical or CPU port number for egress direction.
*       direction       - ingress or egress
*       enable          - enable/disable ingress/egress Vlan Translation.
*                         GT_TRUE: enable Vlan translation
*                         GT_FALSE: disable Vlan translation
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortTranslationEnableSet
(
    IN GT_U8                        devNum,
    IN GT_U8                        port,
    IN CPSS_DIRECTION_ENT           direction,
    IN GT_BOOL                      enable
)
{

    GT_U32  regAddr;
    GT_U32  hwValue;
    GT_U32  startWord; /* the table word at which the field starts */
    GT_U32  startBit;  /* the word's bit at which the field starts */
    GT_STATUS   rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    if (GT_TRUE != PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.vlanTranslationSupported)
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    hwValue = BOOL2BIT_MAC(enable);

    switch (direction)
    {
    case CPSS_DIRECTION_INGRESS_E:
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat and above */
            startWord = 0;
            startBit  = 31;
        }
        else
        {
            startWord = 0;
            startBit  = 15;
        }

        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                             (GT_U32)port,
                                             startWord,
                                             startBit,
                                             1, /* 1 bit */
                                             hwValue);
        break;

    case CPSS_DIRECTION_EGRESS_E:
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,port);

        /* get the start bit 0..63 (in the bmp of registers) */
        startBit = (localPort == CPSS_CPU_PORT_NUM_CNS) ?
                 PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum) :
                 localPort;

        /* get address of Header Alteration Vlan Translation Enable register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                haVlanTransEnReg[OFFSET_TO_WORD_MAC(startBit)];

        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, OFFSET_TO_BIT_MAC(startBit), 1, hwValue);
        break;
    default:
        return GT_BAD_PARAM;
    }

    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanPortTranslationEnableGet
*
* DESCRIPTION:
*       Get the status of Vlan Translation (Enable/Disable) per ingress or
*       egress port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - the device number
*       direction       - ingress or egress
*       port            - physical or CPU port number for egress direction.
*
* OUTPUTS:
*       enablePtr       - (pointer to) ingress/egress Vlan Translation status
*                         GT_TRUE: enable Vlan translation
*                         GT_FALSE: disable Vlan translation
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortTranslationEnableGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    IN  CPSS_DIRECTION_ENT          direction,
    OUT GT_BOOL                     *enablePtr
)
{
    GT_U32      regAddr;
    GT_U32      hwValue;
    GT_U32      startWord; /* the table word at which the field starts */
    GT_U32      startBit;  /* the word's bit at which the field starts */
    GT_STATUS   rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (GT_TRUE != PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.vlanTranslationSupported)
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    switch (direction)
    {
    case CPSS_DIRECTION_INGRESS_E:
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, port);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat and above */
            startWord = 0;
            startBit  = 31;
        }
        else
        {
            startWord = 0;
            startBit  = 15;
        }

        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                            (GT_U32)port,
                                            startWord,
                                            startBit,
                                            1, /* 1 bit */
                                            &hwValue);
        break;

    case CPSS_DIRECTION_EGRESS_E:
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,port);

        /* get the start bit 0..63 (in the bmp of registers) */
        startBit = (localPort == CPSS_CPU_PORT_NUM_CNS) ?
                 PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum) :
                 localPort;

        /* get address of Header Alteration Vlan Translation Enable register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                haVlanTransEnReg[OFFSET_TO_WORD_MAC(startBit)];

        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, OFFSET_TO_BIT_MAC(startBit), 1, &hwValue);
        break;
    default:
        return GT_BAD_PARAM;
    }

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanTranslationEntryWrite
*
* DESCRIPTION:
*       Write an entry into Vlan Translation Table Ingress or Egress
*       This mapping avoids collisions between a VLAN-ID
*       used for a logical interface and a VLAN-ID used by the local network.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - VLAN id, used as index in the Translation Table, 0..4095.
*       direction       - ingress or egress
*       transVlanId     - Translated Vlan ID, use as value in the
*                         Translation Table, 0..4095.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_OUT_OF_RANGE          - when transVlanId is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTranslationEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_U16                       vlanId,
    IN CPSS_DIRECTION_ENT           direction,
    IN GT_U16                       transVlanId
)
{
    PRV_CPSS_DXCH_TABLE_ENT     tableType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(transVlanId);

    if (GT_TRUE != PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.vlanTranslationSupported)
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    switch (direction)
    {
        case CPSS_DIRECTION_INGRESS_E:
            tableType = PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E;
            break;

        case CPSS_DIRECTION_EGRESS_E:
            tableType = PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E;
            break;

        default:
            return GT_BAD_PARAM;
    }


    return prvCpssDxChWriteTableEntryField(devNum,
                                           tableType,
                                           (GT_U32)vlanId,
                                           0,   /* word */
                                           0,   /* start bit */
                                           12,
                                           transVlanId);
}

/*******************************************************************************
* cpssDxChBrgVlanTranslationEntryRead
*
* DESCRIPTION:
*       Read an entry from Vlan Translation Table, Ingress or Egress.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - VLAN id used as index in the Translation Table, 0..4095.
*       direction       - ingress or egress
*
* OUTPUTS:
*       transVlanIdPtr  - (pointer to) Translated Vlan ID, used as value in the
*                         Translation Table.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTranslationEntryRead
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      vlanId,
    IN CPSS_DIRECTION_ENT           direction,
    OUT GT_U16                      *transVlanIdPtr
)
{
    PRV_CPSS_DXCH_TABLE_ENT     tableType;  /* table type               */
    GT_U32                      hwValue;    /* value to read from hw    */
    GT_STATUS                   rc;         /* return code              */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);
    CPSS_NULL_PTR_CHECK_MAC(transVlanIdPtr);

    if (GT_TRUE != PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.vlanTranslationSupported)
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    switch (direction)
    {
        case CPSS_DIRECTION_INGRESS_E:
            tableType = PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E;
            break;

        case CPSS_DIRECTION_EGRESS_E:
            tableType = PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    rc = prvCpssDxChReadTableEntryField(devNum,
                                        tableType,
                                        (GT_U32)vlanId,
                                        0,   /* word */
                                        0,   /* start bit */
                                        12,
                                        &hwValue);

    *transVlanIdPtr = (GT_U16)hwValue;

    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanValidCheckEnableSet
*
* DESCRIPTION:
*       Enable/disable check of "Valid" field in the VLAN entry
*       When check enable and VLAN entry is not valid then packets are dropped.
*       When check disable and VLAN entry is not valid then packets are
*       not dropped and processed like in case of valid VLAN.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - physical device number
*       enable    - GT_TRUE  - Enable check of "Valid" field in the VLAN entry
*                   GT_FALSE - Disable check of "Valid" field in the VLAN entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanValidCheckEnableSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.bridgeGlobalConfigRegArray[0];
    value = BOOL2BIT_MAC(enable);

    /* Bridge Global Configuration Register0, field <VLAN Valid Check Enable> */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 4, 1, value);

}

/*******************************************************************************
* cpssDxChBrgVlanValidCheckEnableGet
*
* DESCRIPTION:
*       Get check status  of "Valid" field in the VLAN entry
*       When check enable and VLAN entry is not valid then packets are dropped.
*       When check disable and VLAN entry is not valid then packets are
*       not dropped and processed like in case of valid VLAN.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       enablePtr - pointer to check status  of "Valid" field in the VLAN entry.
*                 - GT_TRUE  - Enable check of "Valid" field in the VLAN entry
*                   GT_FALSE - Disable check of "Valid" field in the VLAN entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - failed to read from hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanValidCheckEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_U32    regAddr;     /* register address */
    GT_U32    value;       /* value to read from register */
    GT_STATUS rc;          /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.bridgeGlobalConfigRegArray[0];

    /* Bridge Global Configuration Register0, field <VLAN Valid Check Enable> */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 4, 1, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanTpidEntrySet
*
* DESCRIPTION:
*       Function sets TPID (Tag Protocol ID) table entry.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*       entryIndex  - entry index for TPID table (0-7)
*       etherType   - Tag Protocol ID value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, entryIndex, direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTpidEntrySet
(
    IN  GT_U8               devNum,
    IN  CPSS_DIRECTION_ENT  direction,
    IN  GT_U32              entryIndex,
    IN  GT_U16              etherType
)
{
    GT_U32    regAddr;  /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    /* table index validity check */
    if (entryIndex > 7)
    {
        return GT_BAD_PARAM;
    }

    /* choose the appropriate table */
    switch (direction)
    {
        case CPSS_DIRECTION_INGRESS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.bridgeIngressVlanEtherTypeTable[entryIndex / 2];
            break;

        case CPSS_DIRECTION_EGRESS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    haRegs.bridgeEgressVlanEtherTypeTable[entryIndex / 2];
            break;

        default:
            return GT_BAD_PARAM;
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 16 * (entryIndex % 2), 16, etherType);
}

/*******************************************************************************
* cpssDxChBrgVlanTpidEntryGet
*
* DESCRIPTION:
*       Function gets TPID (Tag Protocol ID) table entry.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/ egress direction
*       entryIndex  - entry index for TPID table (0-7)
*
* OUTPUTS:
*       etherTypePtr - (pointer to) Tag Protocol ID value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, entryIndex, direction
*       GT_BAD_PTR               - etherTypePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTpidEntryGet
(
    IN  GT_U8               devNum,
    IN  CPSS_DIRECTION_ENT  direction,
    IN  GT_U32              entryIndex,
    OUT GT_U16              *etherTypePtr
)
{
    GT_U32    regAddr;  /* register address */
    GT_U32    regValue; /* register value */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(etherTypePtr);

    /* table index validity check */
    if (entryIndex > 7)
    {
        return GT_BAD_PARAM;
    }

    /* choose the appropriate table */
    switch (direction)
    {
        case CPSS_DIRECTION_INGRESS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.bridgeIngressVlanEtherTypeTable[entryIndex / 2];
            break;

        case CPSS_DIRECTION_EGRESS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    haRegs.bridgeEgressVlanEtherTypeTable[entryIndex / 2];
            break;

        default:
            return GT_BAD_PARAM;
    }

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 16 * (entryIndex % 2), 16, &regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *etherTypePtr = (GT_U16) regValue;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanPortIngressTpidSet
*
* DESCRIPTION:
*       Function sets bitmap of TPID (Tag Protocol ID) table indexes per
*       ingress port.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (CPU port is supported)
*       ethMode    - TAG0/TAG1 selector
*       tpidBmp    - bitmap represent 8 entries at the TPID Table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - tpidBmp initialized with more then 8 bits
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note that for the ingress direction multiple TPID may be use to
*       identify Tag0 and Tag1. Each bit at the bitmap represent one of
*       the 8 entries at the TPID Table.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortIngressTpidSet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN  GT_U32               tpidBmp
)
{
    GT_U32    regAddr;  /* register address */
    GT_U32    vlanOffset;
    GT_U8   localPort;/* local port - support multi-port-groups device */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    if (tpidBmp >= BIT_8)
    {
        return GT_OUT_OF_RANGE;
    }

    switch (ethMode)
    {
        case CPSS_VLAN_ETHERTYPE0_E:
            vlanOffset = 0 + 16 * (portNum % 2);
            break;

        case CPSS_VLAN_ETHERTYPE1_E:
            vlanOffset = 8 + 16 * (portNum % 2);
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            bridgeRegs.bridgeIngressVlanSelect[localPort / 2];

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, vlanOffset, 8, tpidBmp);
}

/*******************************************************************************
* cpssDxChBrgVlanPortIngressTpidGet
*
* DESCRIPTION:
*       Function gets bitmap of TPID (Tag Protocol ID) table indexes  per
*       ingress port.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (CPU port is supported)
*       ethMode    - TAG0/TAG1 selector
*
* OUTPUTS:
*       tpidBmpPtr - (pointer to) bitmap represent 8 entries at the TPID Table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_BAD_PTR               - tpidBmpPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note that for the ingress direction multiple TPID may be use to
*       identify Tag0 and Tag1. Each bit at the bitmap represent one of
*       the 8 entries at the TPID Table.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortIngressTpidGet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    OUT GT_U32               *tpidBmpPtr
)
{
    GT_U32  regAddr;  /* register address */
    GT_U32  vlanOffset;
    GT_U8   localPort;/* local port - support multi-port-groups device */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(tpidBmpPtr);

    switch (ethMode)
    {
        case CPSS_VLAN_ETHERTYPE0_E:
            vlanOffset = 0 + 16 * (portNum % 2);
            break;

        case CPSS_VLAN_ETHERTYPE1_E:
            vlanOffset = 8 + 16 * (portNum % 2);
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            bridgeRegs.bridgeIngressVlanSelect[localPort / 2];

    return prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, vlanOffset, 8, tpidBmpPtr);
}

/*******************************************************************************
* cpssDxChBrgVlanPortEgressTpidSet
*
* DESCRIPTION:
*       Function sets index of TPID (Tag protocol ID) table per egress port.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number
*       portNum        - port number (CPU port is supported)
*       ethMode        - TAG0/TAG1 selector
*       tpidEntryIndex - TPID table entry index (0-7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - illegal tpidEntryIndex (not 0-7)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortEgressTpidSet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN  GT_U32               tpidEntryIndex
)
{
    GT_U32    regAddr;  /* register address */
    GT_U32    vlanOffset;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    if (tpidEntryIndex >= 8)
    {
        return GT_OUT_OF_RANGE;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if (localPort == CPSS_CPU_PORT_NUM_CNS)
    {
        localPort = (GT_U8)PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum);
    }

    vlanOffset = 3 * (localPort % 8);

    switch (ethMode)
    {
        case CPSS_VLAN_ETHERTYPE0_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    haRegs.bridgeEgressPortTag0TpidSelect[localPort / 8];
            break;

        case CPSS_VLAN_ETHERTYPE1_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        haRegs.bridgeEgressPortTag1TpidSelect[localPort / 8];
            break;

        default:
            return GT_BAD_PARAM;
    }

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, vlanOffset, 3, tpidEntryIndex);
}

/*******************************************************************************
* cpssDxChBrgVlanPortEgressTpidGet
*
* DESCRIPTION:
*       Function gets index of TPID (Tag Protocol ID) table per egress port.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (CPU port is supported)
*       ethMode    - TAG0/TAG1 selector
*
* OUTPUTS:
*       tpidEntryIndexPtr - (pointer to) TPID table entry index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_BAD_PTR               - tpidEntryIndexPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortEgressTpidGet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    OUT GT_U32               *tpidEntryIndexPtr
)
{
    GT_U32    regAddr;  /* register address */
    GT_U32    vlanOffset;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(tpidEntryIndexPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if (localPort == CPSS_CPU_PORT_NUM_CNS)
    {
        localPort = (GT_U8)PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum);
    }

    vlanOffset = 3 * (localPort % 8);

    switch (ethMode)
    {
        case CPSS_VLAN_ETHERTYPE0_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    haRegs.bridgeEgressPortTag0TpidSelect[localPort / 8];
            break;

        case CPSS_VLAN_ETHERTYPE1_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    haRegs.bridgeEgressPortTag1TpidSelect[localPort / 8];
            break;

        default:
            return GT_BAD_PARAM;
    }

    return prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, vlanOffset, 3, tpidEntryIndexPtr);
}

/*******************************************************************************
* cpssDxChBrgVlanRangeSet
*
* DESCRIPTION:
*       Function configures the valid VLAN Range.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       vidRange  - VID range for VLAN filtering (0 - 4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - illegal vidRange
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanRangeSet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vidRange
)
{
    GT_U32    regAddr;  /* register address */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vidRange);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlan0;

    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 12, vidRange);
    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanRangeGet
*
* DESCRIPTION:
*       Function gets the valid VLAN Range.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       vidRangePtr - (pointer to) VID range for VLAN filtering
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - vidRangePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanRangeGet
(
    IN  GT_U8   devNum,
    OUT GT_U16  *vidRangePtr
)
{
    GT_U32  regAddr;  /* register address */
    GT_U32  regValue; /* register value */
    GT_STATUS rc;     /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(vidRangePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlan0;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 12, &regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *vidRangePtr = (GT_U16) regValue;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanPortIsolationCmdSet
*
* DESCRIPTION:
*       Function sets port isolation command per egress VLAN.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN id
*       cmd     - port isolation command for given VID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or cmd
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortIsolationCmdSet
(
    IN  GT_U8                                      devNum,
    IN  GT_U16                                     vlanId,
    IN  CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT  cmd
)
{
    GT_U32  value;  /* HW value for L2/L3 Port Isolation command */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    PRV_CPSS_DXCH_CONVERT_PORT_ISOLATION_MODE_TO_HW_VAL_MAC(value, cmd);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* write 1 bit for L2 Port Isolation command */
        return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_VLAN_E,
                                           (GT_U32)vlanId,/* vid */
                                           3,
                                           21,
                                           2,
                                           value);
    }
    else
    {
        /* write 1 bit for L2 Port Isolation command */
        return prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                               (GT_U32)vlanId,
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                               263,
                                               2,
                                               value);
    }
}

/*******************************************************************************
* cpssDxChBrgVlanLocalSwitchingEnableSet
*
* DESCRIPTION:
*       Function sets local switching of Multicast, known and unknown Unicast,
*       and Broadcast traffic per VLAN.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN id
*       trafficType - local switching traffic type
*       enable      - Enable/disable of local switching
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or trafficType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. To enable local switching of Multicast, unknown Unicast, and
*       Broadcast traffic, both this field in the VLAN entry and the
*       egress port configuration must be enabled for Multicast local switching
*       in function cpssDxChBrgPortEgressMcastLocalEnable.
*
*       2. To enable  local switching of known Unicast traffic, both this
*       field in the VLAN entry and the ingress port configuration must
*       be enabled for Unicast local switching in function
*       cpssDxChBrgGenUcLocalSwitchingEnable.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanLocalSwitchingEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanId,
    IN  CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT  trafficType,
    IN  GT_BOOL enable
)
{
    GT_U32  wordOffset;  /* word offset */
    GT_U32  fieldOffset; /* field offset */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        switch (trafficType)
        {
            case CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_KNOWN_UC_E:
                wordOffset = 5;
                fieldOffset = 17;
                break;

            case CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_FLOODED_E:
                wordOffset = 3;
                fieldOffset = 20;
                break;

            default:
                return GT_BAD_PARAM;
        }

            /* write 1 bit local switching command */
        return prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_TABLE_VLAN_E,
                                               (GT_U32)vlanId,/* vid */
                                               wordOffset,
                                               fieldOffset,
                                               1,
                                               BOOL2BIT_MAC(enable));
    }
    else
    {
        switch (trafficType)
        {
            case CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_KNOWN_UC_E:
                /* write to ingress table */
                return prvCpssDxChWriteTableEntryField(devNum,
                                                       PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                                       (GT_U32)vlanId,
                                                       PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                                       128,
                                                       1,
                                                       BOOL2BIT_MAC(enable));

            case CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_FLOODED_E:
                /* write to egress table */
                return prvCpssDxChWriteTableEntryField(devNum,
                                                       PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                                       (GT_U32)vlanId,
                                                       PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                                       262,
                                                       1,
                                                       BOOL2BIT_MAC(enable));

            default:
                return GT_BAD_PARAM;
        }
    }
}

/*******************************************************************************
* cpssDxChBrgVlanFloodVidxModeSet
*
* DESCRIPTION:
*       Function sets Flood VIDX and Flood VIDX Mode per VLAN.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       vlanId        - VLAN id
*       floodVidx     - VIDX value (12bit) - VIDX value applied to Broadcast,
*                       Unregistered Multicast and unknown Unicast packets,
*                       depending on the VLAN entry field <Flood VIDX Mode>.
*                       xCat2 range is 0..1023, 4095
*       floodVidxMode - Flood VIDX Mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or floodVidxMode
*       GT_OUT_OF_RANGE          - illegal floodVidx
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanFloodVidxModeSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U16                                  vlanId,
    IN  GT_U16                                  floodVidx,
    IN  CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT  floodVidxMode
)
{
    GT_U32  hwValue;
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        /* xCat2 support constant 4095 (0xFFF) and values in range 0..1023 */
        if ((floodVidx != 0xFFF) &&
            (floodVidx >= BIT_10))
        {
            return GT_OUT_OF_RANGE;
        }
    }
    else
    {
        if (floodVidx >= BIT_12)
        {
            return GT_OUT_OF_RANGE;
        }
    }

    PRV_CPSS_DXCH_CONVERT_FLOOD_VIDX_MODE_TO_HW_VAL_MAC(hwValue, floodVidxMode);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_VLAN_E,
                                             (GT_U32)vlanId,/* vid */
                                             3,
                                             8,
                                             12,
                                             (GT_U32)floodVidx);
        if (rc != GT_OK)
        {
            return rc;
        }

            /* write 1 bit floodVidxMode */
        return prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_TABLE_VLAN_E,
                                               (GT_U32)vlanId,/* vid */
                                               5,
                                               18,
                                               1,
                                               hwValue);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                             (GT_U32)vlanId,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             104,
                                             12,
                                             (GT_U32)floodVidx);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* write 1 bit floodVidxMode */
        return prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                               (GT_U32)vlanId,
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                               129,
                                               1,
                                               hwValue);
    }
}

/*******************************************************************************
* prvCpssBrgVlanHwEntryBuildIngress
*
* DESCRIPTION:
*       Builds Ingress vlan entry to buffer.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum              - device number
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*       vlanInfoPtr         - (pointer to) VLAN specific information
*
* OUTPUTS:
*       hwDataArr[VLAN_INGRESS_ENTRY_WORDS_NUM_CNS]
*                           - pointer to hw VLAN entry.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*                         or vlanInfoPtr->stgId
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssBrgVlanHwEntryBuildIngress
(
    IN GT_U8                        devNum,
    IN CPSS_PORTS_BMP_STC           *portsMembersPtr,
    IN CPSS_DXCH_BRG_VLAN_INFO_STC  *vlanInfoPtr,
    OUT GT_U32                      hwDataArr[VLAN_INGRESS_ENTRY_WORDS_NUM_CNS]
)
{
    GT_U32      value;          /* value in HW format */

    devNum = devNum; /* avoid compiler warning */
    
    /* reset all words in hwDataArr */
    cpssOsMemSet((char *) &hwDataArr[0], 0, VLAN_INGRESS_ENTRY_WORDS_NUM_CNS * sizeof(GT_U32));

    /* bit 0 - valid bit */
    value = 1;
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,0,1,value);

    /* bit 1 - NewSrcAddrIsNotSecurityBreach - revert the bit */
    value = 1 - (BOOL2BIT_MAC(vlanInfoPtr->unkSrcAddrSecBreach));
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,1,1,value);

    /* bit 2..65 port members */
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr, 2,32,portsMembersPtr->ports[0]);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,34,32,portsMembersPtr->ports[1]);

    /* bits 66..68 - UnregisteredNonIPMulticastCmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unregNonIpMcastCmd))
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,66,3,value);

    /* bits 69..71 - UnregisteredIpv4MulticastCmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unregIpv4McastCmd))
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,69,3,value);


    /* bits 72..74 - UnregisteredIpv6MulticastCmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unregIpv6McastCmd))
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,72,3,value);

    /* bits 75..77 - UnknownUnicastCmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unkUcastCmd))
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,75,3,value);

    /* bit 78 - Ipv4IgmpToCpuEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv4IgmpToCpuEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,78,1,value);

    /* bit 81 - Ipv6IpmBridgingMode */
    value = BOOL2BIT_MAC(vlanInfoPtr->mirrToRxAnalyzerEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,81,1,value);


    /* bit 82 - Ipv6IcmpToCpuEn*/
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv6IcmpToCpuEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,82,1,value);

    /* check ipCtrlToCpuEn for ipv4 */
    switch(vlanInfoPtr->ipCtrlToCpuEn)
    {
        case CPSS_DXCH_BRG_IP_CTRL_NONE_E     :
        case CPSS_DXCH_BRG_IP_CTRL_IPV6_E     :
            value = 0;
            break;
        case CPSS_DXCH_BRG_IP_CTRL_IPV4_E     :
        case CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E:
            value = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* bit 83 - Ipv4ControlToCpuEn*/
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,83,1,value);

    /* bit 84 - Ipv4IpmBridgingEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv4IpmBrgEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,84,1,value);

    /* bit 85 - Ipv6IpmBridgingEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv6IpmBrgEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,85,1,value);

    /* IPv4 IPM Bridging mode */
    if(GT_TRUE == vlanInfoPtr->ipv4IpmBrgEn)
    {
        /* bit 79 - Ipv4IpmBridgingMode */
        PRV_DXCH_BRG_VLAN_IPM_MODE_CONVERT_MAC(vlanInfoPtr->ipv4IpmBrgMode, value);
        U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,79,1,value);
    }

    /* IPv6 IPM Bridging mode */
    if(GT_TRUE == vlanInfoPtr->ipv6IpmBrgEn)
    {
        /* bit 80 - Ipv6IpmBridgingMode */
        PRV_DXCH_BRG_VLAN_IPM_MODE_CONVERT_MAC(vlanInfoPtr->ipv6IpmBrgMode, value);
        U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,80,1,value);
    }

    /* bits 86..88 - UnregisteredIpv4BcCmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                            vlanInfoPtr->unregIpv4BcastCmd);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,86,3,value);

    /* bits 89..91 - UnregisteredIpv4BcCmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                            vlanInfoPtr->unregNonIpv4BcastCmd);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,89,3,value);

    /* bit 92 - Ipv4UnicastRouteEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv4UcastRouteEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,92,1,value);

    /* bit 93 - Ipv4MulticastRouteEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv4McastRouteEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,93,3,value);

    /* bit 94 - Ipv6UnicastRouteEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv6UcastRouteEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,94,1,value);

    /* bit 95 - Ipv6MulticastRouteEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv6McastRouteEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,95,1,value);

    switch(vlanInfoPtr->ipv6SiteIdMode)
    {
        case CPSS_IP_SITE_ID_INTERNAL_E:
            value = 0;
            break;
        case CPSS_IP_SITE_ID_EXTERNAL_E:
            value = 1;
            break;
        default: return GT_BAD_PARAM;
    }
    /* bit 96 - Ipv6SiteId */
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,96,1,value);

    /* bit 97 - AutoLearnDis */
    value = BOOL2BIT_MAC(vlanInfoPtr->autoLearnDisable);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,97,1,value);

    /* bit 98 - NaMsgToCpuEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->naMsgToCpuEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,98,1,value);

    value = vlanInfoPtr->mruIdx;
    /* check MRU index */
    PRV_CPSS_DXCH_BRG_MRU_INDEX_CHECK_MAC(value);
    /* bits 99..101 MRU Index */
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,99,3,value);

    /* bit 102 - BcUdpTrapMirrorEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->bcastUdpTrapMirrEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,102,1,value);

    /* check ipCtrlToCpuEn for ipv6 */
    switch(vlanInfoPtr->ipCtrlToCpuEn)
    {
        case CPSS_DXCH_BRG_IP_CTRL_NONE_E     :
        case CPSS_DXCH_BRG_IP_CTRL_IPV4_E     :
            value = 0;
            break;
        case CPSS_DXCH_BRG_IP_CTRL_IPV6_E     :
        case CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E:
            value = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* bit 103 - Ipv6ControlToCpuEn */
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,103,1,value);

    if(vlanInfoPtr->floodVidx >= BIT_12)
    {
        return GT_OUT_OF_RANGE;
    }
    value = vlanInfoPtr->floodVidx;
    /* bit 104..115 - Vidx */
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,104,12,value);

    if(vlanInfoPtr->vrfId >= BIT_12 )
    {
        return GT_OUT_OF_RANGE;
    }
    /* bit 116..127 - VrfId */
    value = vlanInfoPtr->vrfId;
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,116,12,value);

    /* bit 128 - UcLocalEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ucastLocalSwitchingEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,128,1,value);

    /* bit 129 - FloodVidxMode */
    PRV_CPSS_DXCH_CONVERT_FLOOD_VIDX_MODE_TO_HW_VAL_MAC(value, vlanInfoPtr->floodVidxMode);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,129,1,value);

    if(vlanInfoPtr->stgId >= BIT_8)
    {
        return GT_OUT_OF_RANGE;
    }
    value = vlanInfoPtr->stgId;
    /* bit 130..137 - SpanStateGroupIndex */
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,130,8,value);

    return GT_OK;
}

/*******************************************************************************
* prvCpssBrgVlanHwEntryBuildEgress
*
* DESCRIPTION:
*       Builds Ingress vlan entry to buffer.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum              - device number
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan -
*                             the relevant parameter for DxCh3 and beyond and
*                             not relevant for TR101 supported devices.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       portsTaggingCmdPtr  - (pointer to) ports tagging commands in the vlan -
*                             the relevant parameter for xCat and above
*                             with TR101 feature support.
*
* OUTPUTS:
*       hwDataArr[VLAN_EGRESS_ENTRY_WORDS_NUM_CNS]
*                           - pointer to hw VLAN entry.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*                         or vlanInfoPtr->stgId
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssBrgVlanHwEntryBuildEgress
(
    IN GT_U8                        devNum,
    IN CPSS_PORTS_BMP_STC           *portsMembersPtr,
    IN CPSS_PORTS_BMP_STC           *portsTaggingPtr,
    IN CPSS_DXCH_BRG_VLAN_INFO_STC  *vlanInfoPtr,
    IN CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr,
    OUT GT_U32                      hwDataArr[VLAN_EGRESS_ENTRY_WORDS_NUM_CNS]
)
{
    GT_U32      value;          /* value in HW format */
    GT_U32      ii;             /* iterator */
    GT_U32      tagCmdMaxPorts = 63;  /* 63 ports with tagCmd -- NOT 64 !!! */
    CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT currTagCmd;
    GT_U32      startBit;/* start bit in entry */
    GT_BOOL     tr101Supported;   /* TR101 Feature Support */

    /* reset all words in hwDataArr */
    cpssOsMemSet((char *) &hwDataArr[0], 0, VLAN_EGRESS_ENTRY_WORDS_NUM_CNS * sizeof(GT_U32));

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    /* bit 0 - valid bit */
    value = 1;
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,0,1,value);

    /* bit 1..64 port members */
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr, 1,32,portsMembersPtr->ports[0]);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,33,32,portsMembersPtr->ports[1]);

    startBit = 65;
    /* tagCmd info for all ports */
    for (ii = 0; ii < tagCmdMaxPorts; ii++ , startBit += 3)
    {
        if (tr101Supported == GT_FALSE)
        {
            currTagCmd = CPSS_PORTS_BMP_IS_PORT_SET_MAC(portsTaggingPtr,ii) ?
                CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E :
                CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
        }
        else
        {
            currTagCmd = portsTaggingCmdPtr->portsCmd[ii];
        }

        PRV_CPSS_DXCH_CONVERT_VLAN_TAG_MODE_TO_HW_VAL_MAC(value,currTagCmd);

        /* 65..253 - tagCmd of ports */
        U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,startBit, 3 ,value);
    }

    value = vlanInfoPtr->stgId;
    /* bit 254..261 - SpanStateGroupIndex */
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,254,8,value);

    /* bit 262 - LocalSwithingEn*/
    value = BOOL2BIT_MAC(vlanInfoPtr->mcastLocalSwitchingEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,262,1,value);

    /* bits 263..264 - PortIsolationVlanCmd */
    PRV_CPSS_DXCH_CONVERT_PORT_ISOLATION_MODE_TO_HW_VAL_MAC(value, vlanInfoPtr->portIsolationMode);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,263,2,value);

    return GT_OK;
}


/*******************************************************************************
* vlanEntrySplitTableWrite
*
* DESCRIPTION:
*       Builds and writes vlan entry to HW - for split table (ingress,egress)
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum              - device number
*       vlanId              - VLAN Id
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan -
*                             The parameter is relevant for DxCh1, DxCh2
*                             and DxCh3 devices.
*                             The parameter is relevant for xCat and
*                             above devices without TR101 feature support
*                             The parameter is not relevant and ignored for
*                             xCat and above devices with TR101 feature
*                             support.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       portsTaggingCmdPtr  - (pointer to) ports tagging commands in the vlan -
*                             The parameter is relevant only for xCat and
*                             above with TR101 feature support.
*                             The parameter is not relevant and ignored for
*                             other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_OUT_OF_RANGE          - length of portsMembersPtr, portsTaggingPtr
*                                  or vlanInfoPtr->stgId is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS vlanEntrySplitTableWrite
(
    IN  GT_U8                                 devNum,
    IN  GT_U16                                vlanId,
    IN  CPSS_PORTS_BMP_STC                    *portsMembersPtr,
    IN  CPSS_PORTS_BMP_STC                    *portsTaggingPtr,
    IN  CPSS_DXCH_BRG_VLAN_INFO_STC           *vlanInfoPtr,
    IN  CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr
)
{
    GT_STATUS   rc;
    /* ingress HW format */
    GT_U32      ingressHwData[VLAN_INGRESS_ENTRY_WORDS_NUM_CNS];
    /* egress HW format */
    GT_U32      egressHwData[VLAN_EGRESS_ENTRY_WORDS_NUM_CNS];

    /* build ingress VLAN entry */
    rc = prvCpssBrgVlanHwEntryBuildIngress(devNum,
                                    portsMembersPtr,
                                    vlanInfoPtr,
                                    &ingressHwData[0]);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* build egress VLAN entry */
    rc = prvCpssBrgVlanHwEntryBuildEgress(devNum,
                                    portsMembersPtr,
                                    portsTaggingPtr,
                                    vlanInfoPtr,
                                    portsTaggingCmdPtr,
                                    &egressHwData[0]);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* write ingress VLAN entry to the ingress VLAN Table */
    rc = prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                    (GT_U32)vlanId,/* vid */
                                    &ingressHwData[0]);
    if (rc != GT_OK)
    {
        return rc;
    }


    /* write egress VLAN entry to the egress VLAN Table */
    rc = prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                    (GT_U32)vlanId,/* vid */
                                    &egressHwData[0]);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssBrgVlanHwEntryParseIngress
*
* DESCRIPTION:
*       parse vlan entry from buffer. - ingress vlan
*
*       NOTE: for multi-port-groups device , this function called per port group , to get the
*             per port group info
*
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum - device number
*       hwDataArr - pointer to hw VLAN entry.
*
* OUTPUTS:
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan -
*                             the relevant parameter for DxCh3 and beyond and
*                             not relevant for TR101 supported devices.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       isValidPtr          - (pointer to) VLAN entry status
*                           GT_TRUE = Vlan is valid
*                           GT_FALSE = Vlan is not Valid
*       portsTaggingCmdPtr  - (pointer to) ports tagging commands in the vlan -
*                             the relevant parameter for xCat and above
*                             with TR101 feature support.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssBrgVlanHwEntryParseIngress
(
    IN GT_U8                        devNum,
    IN GT_U32                       hwDataArr[VLAN_INGRESS_ENTRY_WORDS_NUM_CNS],
    OUT CPSS_PORTS_BMP_STC          *portsMembersPtr,
    OUT CPSS_PORTS_BMP_STC          *portsTaggingPtr,
    OUT CPSS_DXCH_BRG_VLAN_INFO_STC *vlanInfoPtr,
    OUT GT_BOOL                     *isValidPtr,
    OUT CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr
)
{
    GT_U32      value;

    *isValidPtr = BIT2BOOL_MAC(hwDataArr[0] & 0x1);

    devNum = devNum; /* avoid compiler warning */
    portsTaggingPtr    = portsTaggingPtr;   /* avoid compiler warning */
    portsTaggingCmdPtr = portsTaggingCmdPtr;/* avoid compiler warning */

    /* bit 0 - valid bit */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,0,1,value);
    *isValidPtr = BIT2BOOL_MAC(value);

    /* bit 1 - NewSrcAddrIsNotSecurityBreach - revert the bit */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,1,1,value);
    vlanInfoPtr->unkSrcAddrSecBreach = BIT2BOOL_MAC(value - 1);

    /* bit 2..65 port members */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr, 2,32,portsMembersPtr->ports[0]);
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,34,32,portsMembersPtr->ports[1]);

    /* bits 66..68 - UnregisteredNonIPMulticastCmd */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,66,3,value);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregNonIpMcastCmd,
                                              value)

    /* bits 69..71 - UnregisteredIpv4MulticastCmd */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,69,3,value);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregIpv4McastCmd,
                                              value)


    /* bits 72..74 - UnregisteredIpv6MulticastCmd */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,72,3,value);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregIpv6McastCmd,
                                              value)

    /* bits 75..77 - UnknownUnicastCmd */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,75,3,value);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unkUcastCmd,
                                              value)

    /* bit 78 - Ipv4IgmpToCpuEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,78,1,value);
    vlanInfoPtr->ipv4IgmpToCpuEn = BIT2BOOL_MAC(value);

    /* bit 81 - Ipv6IpmBridgingMode */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,81,1,value);
    vlanInfoPtr->mirrToRxAnalyzerEn = BIT2BOOL_MAC(value);


    /* bit 82 - Ipv6IcmpToCpuEn*/
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,82,1,value);
    vlanInfoPtr->ipv6IcmpToCpuEn = BIT2BOOL_MAC(value);

    /* parse the Ipv4ControlToCpuEn,
       (the Ipv6ControlToCpuEn will be read later , and the ipCtrlToCpuEn will
       be changed accordingly */

    /* bit 83 - Ipv4ControlToCpuEn*/
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,83,1,value);
    vlanInfoPtr->ipCtrlToCpuEn = (value == 0x1) ?
                                    CPSS_DXCH_BRG_IP_CTRL_IPV4_E :
                                    CPSS_DXCH_BRG_IP_CTRL_NONE_E;

    /* bit 84 - Ipv4IpmBridgingEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,84,1,value);
    vlanInfoPtr->ipv4IpmBrgEn = BIT2BOOL_MAC(value);

    /* bit 85 - Ipv6IpmBridgingEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,85,1,value);
    vlanInfoPtr->ipv6IpmBrgEn = BIT2BOOL_MAC(value);

    /* IPv4 IPM Bridging mode */
    if(GT_TRUE == vlanInfoPtr->ipv4IpmBrgEn)
    {
        /* bit 79 - Ipv4IpmBridgingMode */
        U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,79,1,value);
        vlanInfoPtr->ipv4IpmBrgMode = (value == 0x0) ? CPSS_BRG_IPM_SGV_E :
                                                     CPSS_BRG_IPM_GV_E;
    }

    /* IPv6 IPM Bridging mode */
    if(GT_TRUE == vlanInfoPtr->ipv6IpmBrgEn)
    {
        /* bit 80 - Ipv6IpmBridgingMode */
        U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,80,1,value);
        vlanInfoPtr->ipv6IpmBrgMode = (value == 0x0) ? CPSS_BRG_IPM_SGV_E :
                                                     CPSS_BRG_IPM_GV_E;
    }

    /* bits 86..88 - UnregisteredIpv4BcCmd */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,86,3,value);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregIpv4BcastCmd,
                                              value)

    /* bits 89..91 - UnregisteredIpv4BcCmd */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,89,3,value);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregNonIpv4BcastCmd,
                                              value)

    /* bit 92 - Ipv4UnicastRouteEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,92,1,value);
    vlanInfoPtr->ipv4UcastRouteEn = BIT2BOOL_MAC(value);

    /* bit 93 - Ipv4MulticastRouteEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,93,3,value);
    vlanInfoPtr->ipv4McastRouteEn = BIT2BOOL_MAC(value);

    /* bit 94 - Ipv6UnicastRouteEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,94,1,value);
    vlanInfoPtr->ipv6UcastRouteEn = BIT2BOOL_MAC(value);

    /* bit 95 - Ipv6MulticastRouteEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,95,1,value);
    vlanInfoPtr->ipv6McastRouteEn = BIT2BOOL_MAC(value);

    /* bit 96 - Ipv6SiteId */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,96,1,value);
    vlanInfoPtr->ipv6SiteIdMode = (value == 0x0) ?
        CPSS_IP_SITE_ID_INTERNAL_E :
        CPSS_IP_SITE_ID_EXTERNAL_E;

    /* bit 97 - AutoLearnDis */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,97,1,value);
    vlanInfoPtr->autoLearnDisable = BIT2BOOL_MAC(value);

    /* bit 98 - NaMsgToCpuEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,98,1,value);
    vlanInfoPtr->naMsgToCpuEn = BIT2BOOL_MAC(value);

    /* bits 99..101 MRU Index */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,99,3,value);
    vlanInfoPtr->mruIdx = value;

    /* bit 102 - BcUdpTrapMirrorEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,102,1,value);
    vlanInfoPtr->bcastUdpTrapMirrEn = BIT2BOOL_MAC(value);

    /* bit 103 - Ipv6ControlToCpuEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,103,1,value);

    if(vlanInfoPtr->ipCtrlToCpuEn == CPSS_DXCH_BRG_IP_CTRL_NONE_E)
    {
        vlanInfoPtr->ipCtrlToCpuEn = (value == 0x1) ?
                                    CPSS_DXCH_BRG_IP_CTRL_IPV6_E :
                                    CPSS_DXCH_BRG_IP_CTRL_NONE_E;
    }
    else /*CPSS_DXCH_BRG_IP_CTRL_IPV4_E*/
    {
        vlanInfoPtr->ipCtrlToCpuEn = (value == 0x1) ?
                                    CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E :
                                    CPSS_DXCH_BRG_IP_CTRL_IPV4_E;
    }

    /* bit 104..115 - Vidx */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,104,12,value);
    vlanInfoPtr->floodVidx = (GT_U16)value;

    /* bit 116..127 - VrfId */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,116,12,value);
    vlanInfoPtr->vrfId = value;

    /* bit 128 - UcLocalEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,128,1,value);
    vlanInfoPtr->ucastLocalSwitchingEn = BIT2BOOL_MAC(value);

    /* bit 129 - FloodVidxMode */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,129,1,value);
    vlanInfoPtr->floodVidxMode = (value == 0) ? CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E
                                              : CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E;

    /* bit 130..137 - SpanStateGroupIndex */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,130,8,value);
    vlanInfoPtr->stgId = value;

    return GT_OK;
}

/*******************************************************************************
* prvCpssBrgVlanHwEntryParseEgress
*
* DESCRIPTION:
*       parse vlan entry from buffer. -- egress vlan
*
*       NOTE: for multi-port-groups device , this function called per port group , to get the
*             per port group info
*
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum - device number
*       hwDataArr - pointer to hw VLAN entry.
*       isValidPtr          - (pointer to) INGRESS VLAN entry status
*       portsMembersPtr     - (pointer to) bmp of ports members in INGRESS vlan
*       vlanInfoPtr         - (pointer to) VLAN specific information -
*                              field of : PortIsolationVlanCmd is taken from stgId
*
* OUTPUTS:
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan -
*                             the relevant parameter for DxCh3 and beyond and
*                             not relevant for TR101 supported devices.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       isValidPtr          - (pointer to) egress VLAN entry status
*                           GT_TRUE = Vlan is valid
*                           GT_FALSE = Vlan is not Valid
*       portsTaggingCmdPtr  - (pointer to) ports tagging commands in the vlan -
*                             the relevant parameter for xCat and above
*                             with TR101 feature support.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_STATE    - the ingress and egress vlan hold 'same fields' but with
*                         different values : valid,members,stgId
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssBrgVlanHwEntryParseEgress
(
    IN GT_U8                        devNum,
    IN GT_U32                       hwDataArr[VLAN_EGRESS_ENTRY_WORDS_NUM_CNS],
    IN CPSS_PORTS_BMP_STC           *portsMembersPtr,
    OUT CPSS_PORTS_BMP_STC          *portsTaggingPtr,
    INOUT CPSS_DXCH_BRG_VLAN_INFO_STC *vlanInfoPtr,
    IN GT_BOOL                      *isValidPtr,
    OUT CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr
)
{
    GT_U32      value;          /* value in HW format */
    GT_U32      ii;             /* iterator */
    GT_U32      tagCmdMaxPorts = 63;  /* 63 ports with tagCmd -- NOT 64 !!! */
    GT_U32      startBit;/* start bit in entry */
    GT_BOOL     tr101Supported;   /* TR101 Feature Support */

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    /* bit 0 - valid bit */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,0,1,value);
    if(value != (GT_U32)(BOOL2BIT_MAC(*isValidPtr)))
    {
        /* the ingress vlan and the egress vlan not synchronized ?! */
        /* we have not API to allow this situation */
        return GT_BAD_STATE;
    }

    /* bit 1..64 port members */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr, 1,32,value);
    if(portsMembersPtr->ports[0] != value)
    {
        /* the ingress vlan and the egress vlan not synchronized ?! */
        /* we have not API to allow this situation */
        return GT_BAD_STATE;
    }

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,33,32,value);
    if(portsMembersPtr->ports[1] != value)
    {
        /* the ingress vlan and the egress vlan not synchronized ?! */
        /* we have not API to allow this situation */
        return GT_BAD_STATE;
    }

    startBit = 65;
    /* tagCmd info for all ports */
    for (ii = 0; ii < tagCmdMaxPorts; ii++ , startBit += 3)
    {
        /* 65..253 - tagCmd of ports */
        U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,startBit, 3 ,value);

        if (tr101Supported == GT_FALSE)
        {
            if(value != 0)
            {
                CPSS_PORTS_BMP_PORT_SET_MAC(portsTaggingPtr,ii);
            }
        }
        else
        {
            PRV_CPSS_DXCH_CONVERT_HW_VAL_TO_VLAN_TAG_MODE_MAC(value,
                    portsTaggingCmdPtr->portsCmd[ii]);
        }
    }

    /* bit 254..261 - SpanStateGroupIndex */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,254,8,value);
    if(value != vlanInfoPtr->stgId)
    {
        /* the ingress vlan and the egress vlan not synchronized ?! */
        /* we have not API to allow this situation */
        return GT_BAD_STATE;
    }

    /* bit 262 - LocalSwithingEn*/
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,262,1,value);
    vlanInfoPtr->mcastLocalSwitchingEn = BIT2BOOL_MAC (value);

    /* bits 263..264 - PortIsolationVlanCmd */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,263,2,value);
    switch (value)
    {
        case 0:
            vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E;
            break;
        case 1:
            vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_CMD_E;
            break;
        case 2:
            vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L3_CMD_E;
            break;
        default:
            vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_L3_CMD_E;
    }

    return GT_OK;
}

/*******************************************************************************
* vlanEntrySplitTableRead
*
* DESCRIPTION:
*       Read vlan entry. - for split table (ingress,egress)
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum              - device number
*       vlanId              - VLAN Id
*
* OUTPUTS:
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan -
*                             The parameter is relevant for DxCh1, DxCh2
*                             and DxCh3 devices.
*                             The parameter is relevant for xCat and
*                             above devices without TR101 feature support
*                             The parameter is not relevant and ignored for
*                             xCat and above devices with TR101 feature
*                             support.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       isValidPtr          - (pointer to) VLAN entry status
*                                 GT_TRUE = Vlan is valid
*                                 GT_FALSE = Vlan is not Valid
*       portsTaggingCmdPtr  - (pointer to) ports tagging commands in the vlan -
*                             The parameter is relevant only for xCat and
*                             above with TR101 feature support.
*                             The parameter is not relevant and ignored for
*                             other devices.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT               - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS vlanEntrySplitTableRead
(
    IN  GT_U8                                devNum,
    IN  GT_U16                               vlanId,
    OUT CPSS_PORTS_BMP_STC                   *portsMembersPtr,
    OUT CPSS_PORTS_BMP_STC                   *portsTaggingPtr,
    OUT CPSS_DXCH_BRG_VLAN_INFO_STC          *vlanInfoPtr,
    OUT GT_BOOL                              *isValidPtr,
    OUT CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC *portsTaggingCmdPtr
)
{
    GT_STATUS   rc;
    /* ingress HW format */
    GT_U32      ingressHwData[VLAN_INGRESS_ENTRY_WORDS_NUM_CNS];
    /* egress HW format */
    GT_U32      egressHwData[VLAN_EGRESS_ENTRY_WORDS_NUM_CNS];
    GT_BOOL     tr101Supported;   /* TR101 Feature Support */

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    cpssOsMemSet(vlanInfoPtr,0,sizeof(*vlanInfoPtr));

    portsMembersPtr->ports[0] = 0;
    portsMembersPtr->ports[1] = 0;

    /* get TR101 feature support */
    if (tr101Supported != GT_FALSE)
    {
        cpssOsMemSet(portsTaggingCmdPtr,0,sizeof(*portsTaggingCmdPtr));
    }
    else
    {
        portsTaggingPtr->ports[0] = 0;
        portsTaggingPtr->ports[1] = 0;
    }

    /* read entry from HW ingress table */
    rc = prvCpssDxChReadTableEntry(devNum,
                                   PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                   (GT_U32)vlanId,/* vid */
                                   &ingressHwData[0]);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* convert HW format to SW format */
    rc = prvCpssBrgVlanHwEntryParseIngress(devNum,
                                    &ingressHwData[0],
                                    portsMembersPtr,
                                    portsTaggingPtr,
                                    vlanInfoPtr,
                                    isValidPtr,
                                    portsTaggingCmdPtr);

    if (rc != GT_OK)
    {
        return rc;
    }

    /* read entry from HW ingress table */
    rc = prvCpssDxChReadTableEntry(devNum,
                                   PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                   (GT_U32)vlanId,/* vid */
                                   &egressHwData[0]);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* convert HW format to SW format */
    rc = prvCpssBrgVlanHwEntryParseEgress(devNum,
                                    &egressHwData[0],
                                    portsMembersPtr,
                                    portsTaggingPtr,
                                    vlanInfoPtr,
                                    isValidPtr,
                                    portsTaggingCmdPtr);

    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet
*
* DESCRIPTION:
*       Set Tag1 removal mode from the egress port Tag State if Tag1 VID
*       is assigned a value of 0.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       mode          - Vlan Tag1 Removal mode when Tag1 VID=0
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet
(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT     mode
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value   */
    GT_U32  regValueB1; /* register value for Lion revision 3 and above */
    GT_U32  regOffset;  /* offset in register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);


    switch (mode)
    {
        case CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E:
            regValue = regValueB1 = 0;
            break;
        case CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E:
            regValue = 1;
            regValueB1 = 2;
            break;
        case CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E:
            regValue = regValueB1 = 3;
            break;
        default:
            return GT_BAD_PARAM;
    }

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        if( PRV_CPSS_PP_MAC(devNum)->revision > 2 )
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                txqVer1.dq.global.debug.metalFix;
            regOffset = 22;

            regValue = regValueB1;
        }
        else
        {
            return GT_NOT_APPLICABLE_DEVICE;
        }
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueMiscCtrl;
        regOffset = 6;
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, regOffset, 2, regValue);
}

/*******************************************************************************
* cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet
*
* DESCRIPTION:
*       Get Tag1 removal mode from the egress port Tag State when Tag1 VID
*       is assigned a value of 0.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       modePtr       - (pointer to) Vlan Tag1 Removal mode when Tag1 VID=0
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - modePtr is NULL pointer
*       GT_BAD_STATE             - on bad state of register
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet
(
    IN  GT_U8                                               devNum,
    OUT CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT     *modePtr
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  regOffset;  /* offset in register */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        if( PRV_CPSS_PP_MAC(devNum)->revision > 2 )
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                txqVer1.dq.global.debug.metalFix;
            regOffset = 22;
        }
        else
        {
            return GT_NOT_APPLICABLE_DEVICE;
        }
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueMiscCtrl;
        regOffset = 6;
    }

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, regOffset, 2, &regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (regValue)
    {
        case 0:
            *modePtr = CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E;
            break;
        case 1:
        case 2: 
            if(((CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily) &&
                (1 == regValue)) ||
               ((CPSS_PP_FAMILY_DXCH_LION_E != PRV_CPSS_PP_MAC(devNum)->devFamily) &&
                (2 == regValue )))
            {
                return GT_BAD_STATE;
            }
            *modePtr = CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E;
            break;
        case 3:
            *modePtr = CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E;
            break;
        default:
            /* no chance getting here */
            return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanForceNewDsaToCpuEnableSet
*
* DESCRIPTION:
*       Enable / Disable preserving the original VLAN tag
*       and VLAN EtherType (or tags),
*       and including the new VLAN Id assignment in the TO_CPU DSA tag
*       for tagged packets that are sent to the CPU.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum        - device number.
*       enable        - GT_TRUE - Double Tag to CPU: Packets sent to the
*                                 CPU whose Vlan Id was modified by the device,
*                                 with a DSA tag that contains the newly
*                                 assigned Vlan Id and the original Vlan tag
*                                 (or tags) that is preserved after the DSA tag.
*                     - GT_FALSE - If the original packet contained a Vlan tag,
*                                  it is removed and the DSA tag contains
*                                  the newly assigned Vlan Id.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanForceNewDsaToCpuEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    data = (enable == GT_TRUE) ? 1 : 0;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;

    /* Enable / Disable preserving the original VLAN tag  */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 17, 1, data);

}

/*******************************************************************************
* cpssDxChBrgVlanForceNewDsaToCpuEnableGet
*
* DESCRIPTION:
*       Get Force New DSA To Cpu mode for enabling / disabling
*       preserving the original VLAN tag and VLAN EtherType (or tags),
*       and including the new VLAN Id assignment in the TO_CPU DSA tag
*       for tagged packets that are sent to the CPU.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum        - device number.
* OUTPUTS:
*       enablePtr     - pointer to Force New DSA To Cpu mode.
*                     - GT_TRUE - Double Tag to CPU: Packets sent to the
*                                 CPU whose Vlan Id was modified by the device,
*                                 with a DSA tag that contains the newly
*                                 assigned Vlan Id and the original Vlan tag
*                                 (or tags) that is preserved after the DSA tag.
*                     - GT_FALSE - If the original packet contained a Vlan tag,
*                                  it is removed and the DSA tag contains
*                                  the newly assigned Vlan Id.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - modePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanForceNewDsaToCpuEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32    regAddr;     /* register address */
    GT_U32    data;        /* reg sub field data */
    GT_STATUS rc;          /* return status                */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;

    /* Get Force New DSA To Cpu mode for enabling / disabling
       preserving the original VLAN tag */
    rc =  prvCpssDrvHwPpGetRegField(devNum, regAddr, 17, 1, &data);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (data == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgVlanKeepVlan1EnableSet
*
* DESCRIPTION:
*       Enable/Disable keeping VLAN1 in the packet, for packets received with VLAN1,
*       even-though the tag-state of this egress-port is configured in the
*       VLAN-table to "untagged" or "VLAN0".
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       up              - VLAN tag 0 User Priority
*                         [0..7].
*       enable          - GT_TRUE: If the packet is received with VLAN1 and
*                                  VLAN Tag state is "VLAN0" or "untagged"
*                                  then VLAN1 is not removed from the packet.
*                         GT_FALSE: Tag state assigned by VLAN is preserved.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or up
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanKeepVlan1EnableSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U8       up,
    IN  GT_BOOL     enable
)
{
    GT_U32    regAddr;     /* register address                              */
    GT_U32    data;        /* reg sub field data                            */
    GT_U32    regOffset;   /* register offset                               */
    GT_U32    fieldOffset; /* field offset inside register                  */
    GT_U32    portGroupId; /* the port group Id - support
                              multi-port-groups device                      */
    GT_U8     localPort;  /* local port - support multi-port-groups device  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);
    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(up);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regOffset = (localPort / 4);
    fieldOffset = ((localPort % 4) * 8) + up;
    data = BOOL2BIT_MAC(enable);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.keepVlan1Reg[regOffset];

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                              fieldOffset, 1, data);
}

/*******************************************************************************
* cpssDxChBrgVlanKeepVlan1EnableGet
*
* DESCRIPTION:
*       Get status of keeping VLAN1 in the packet, for packets received with VLAN1.
*       (Enabled/Disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       up              - VLAN tag 0 User Priority
*                         [0..7].
*
* OUTPUTS:
*       enablePtr       - GT_TRUE: If the packet is received with VLAN1 and
*                                  VLAN Tag state is "VLAN0" or "untagged"
*                                  then VLAN1 is not removed from the packet.
*                         GT_FALSE: Tag state assigned by VLAN is preserved.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or up
*       GT_BAD_PTR               - modePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanKeepVlan1EnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U8       up,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32    regAddr;     /* register address                              */
    GT_U32    data;        /* reg sub field data                            */
    GT_U32    regOffset;   /* register offset                               */
    GT_U32    fieldOffset; /* field offset inside register                  */
    GT_U32    portGroupId; /* the port group Id - support
                              multi-port-groups device                      */
    GT_U8     localPort;   /* local port - support multi-port-groups device */
    GT_STATUS rc;          /* return status                                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);
    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(up);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regOffset = (localPort / 4);
    fieldOffset = ((localPort % 4) * 8) + up;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.keepVlan1Reg[regOffset];

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                            fieldOffset, 1, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(data);

    return GT_OK;
}


#ifndef __AX_PLATFORM__
int vlanread(unsigned char dev,unsigned short vid)
{
	int ret =0 ;
	CPSS_PORTS_BMP_STC          portsMembersPtr;
    CPSS_PORTS_BMP_STC          portsTaggingPtr;
    CPSS_DXCH_BRG_VLAN_INFO_STC vlanInfoPtr;
    GT_BOOL                     isValidPtr = GT_FALSE;
	int i; 

	memset(&portsMembersPtr,0,sizeof(CPSS_PORTS_BMP_STC));
	memset(&portsTaggingPtr,0,sizeof(CPSS_PORTS_BMP_STC));
	memset(&vlanInfoPtr,0,sizeof(CPSS_DXCH_BRG_VLAN_INFO_STC));

    #if 0   
	if(0 != (ret = cpssDxChBrgVlanEntryRead( \
		dev,vid,&portsMembersPtr,&portsTaggingPtr,&vlanInfoPtr,&isValidPtr)))
		return ret;
    #endif
    /* 
        *luoxun -- 
        *   cpssDxChBrgVlanEntryRead() is different from cpss1.3,
        *   the last argument 'NULL' is added just for compiling.
        */
    if(0 != (ret = cpssDxChBrgVlanEntryRead( \
		dev,vid,&portsMembersPtr,&portsTaggingPtr,&vlanInfoPtr,&isValidPtr, NULL)))
		return ret;

	osPrintf("====================================\n");
	osPrintf("isValid\t : %s\n", isValidPtr ? "True" : "False");
	osPrintf("untag ports bmp\t : ");
	for(i = 0; i < 8; i++) {
		osPrintf("%02x,",*(((unsigned char *)portsMembersPtr.ports) + i));
	}
	osPrintf("\ntag ports bmp\t : ");
	for(i = 0; i < 8; i++) {
		osPrintf("%02x,",*(((unsigned char *)portsTaggingPtr.ports) + i));
	}	
	osPrintf("\n");

	
	osPrintf("unkSrcAddrSecBreach\t : %s \n",vlanInfoPtr.unkSrcAddrSecBreach ? "True" : "False");
	osPrintf("autoLearnDisable\t : %s\n",vlanInfoPtr.autoLearnDisable ? "True" : "False");
	
	osPrintf("====================================\n");
	
	return ret;	  /* code optimize: Missing return statement. zhangdi@autelan.com 2012-01-18 */
}
#endif

