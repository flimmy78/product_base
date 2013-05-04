 /*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIp.c
*
* DESCRIPTION:
*       The CPSS DXCH Ip HW structures APIs
*
* FILE REVISION NUMBER:
*       $Revision: 1.2 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/ip/cpssDxChIp.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#include <cpss/dxCh/dxChxGen/private/routerTunnelTermTcam/prvCpssDxChRouterTunnelTermTcam.h>
#include <cpss/dxCh/dxChxGen/ip/private/prvCpssDxChIp.h>

/* Static function declarations */
static GT_STATUS prvCpssDxChIpHwFormat2MllStruct
(
    IN   GT_U32                 *hwData,
    OUT  CPSS_DXCH_IP_MLL_STC   *mllPairEntryPtr
);
static GT_STATUS prvCpssDxChIpMllStruct2HwFormat
(
    IN  CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_FORM_ENT   mllPairWriteForm,
    IN  CPSS_DXCH_IP_MLL_PAIR_STC                   *mllPairEntryPtr,
    OUT GT_U32                                      *hwData
);


/* number of LTT enrties */
#define PRV_CPSS_DXCH_ROUTER_TCAM_ENRTY_NUM_MAX_CNS    5119


/* number of ARP ADDRESS enrties */
#define PRV_CPSS_DXCH_ARP_ADDR_INDEX_NUM_MAX_CNS    1023

/* number of ARP ADDRESS enrties */
#define PRV_CPSS_DXCH2_ARP_ADDR_INDEX_NUM_MAX_CNS    4095

/* check CH LTT table Index */
#define PRV_CPSS_DXCH_ROUTER_TCAM_INDEX_CHECK_MAC(_lttTTIndex)     \
        if((_lttTTIndex) > PRV_CPSS_DXCH_ROUTER_TCAM_ENRTY_NUM_MAX_CNS)   \
        return GT_OUT_OF_RANGE;


/* Maximal number of QoS Profiles  for a device */
#define PRV_CPSS_DXCH_ARP_ADDR_INDEX_MAX_MAC(_devNum)                           \
    (((PRV_CPSS_PP_MAC(_devNum)->devFamily) != CPSS_PP_FAMILY_CHEETAH_E) ?   \
     PRV_CPSS_DXCH2_ARP_ADDR_INDEX_NUM_MAX_CNS : PRV_CPSS_DXCH_ARP_ADDR_INDEX_NUM_MAX_CNS)

/* check CH ARP table Index */
#define PRV_CPSS_DXCH_ARP_ADDR_INDEX_CHECK_MAC(_devNum,_arpAddrIndex)     \
        if((_arpAddrIndex) > (GT_U32)(PRV_CPSS_DXCH_ARP_ADDR_INDEX_MAX_MAC(_devNum)))   \
        return GT_OUT_OF_RANGE;

#define CPSS_DXCH_SIZE_OF_MLL_NODE_IN_BYTE  (16)
#define CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD  (CPSS_DXCH_SIZE_OF_MLL_NODE_IN_BYTE >> 2)

#ifndef __AX_PLATFORM__
typedef struct llmInfo {
	unsigned char tblIndex; /* multicast address last byte as table index */
	CPSS_IP_PROTOCOL_STACK_ENT protocolStack; /* supported protocol stack */
	CPSS_NET_RX_CPU_CODE_ENT cpuCode; /* CPU code for this mirrored packets */
}Llm_Info;

Llm_Info LinkLocalMulticastMirrorIndex[] =
{
	{  1, CPSS_IP_PROTOCOL_IPV4V6_E, CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E},		/*All Systems on this Subnet  224.0.0.1*/
	{  2, CPSS_IP_PROTOCOL_IPV4V6_E, CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E},		/*All Routers on this Subnet  224.0.0.2*/
	{  4, CPSS_IP_PROTOCOL_IPV4V6_E, CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E},		/*DVMRP Routers  224.0.0.4*/	
	{  5, CPSS_IP_PROTOCOL_IPV4V6_E, CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E},	/*OSPF All Routers  224.0.0.5*/
	{  6, CPSS_IP_PROTOCOL_IPV4V6_E, CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E},	/*OSPF Designated Routers  224.0.0.6*/
	{  9, CPSS_IP_PROTOCOL_IPV4V6_E, CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E},	/*RIPv2 Routers  224.0.0.9*/
	{ 10, CPSS_IP_PROTOCOL_IPV4V6_E, CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E},		/*EIGRP Routers  224.0.0.10*/
	{ 13, CPSS_IP_PROTOCOL_IPV4V6_E, CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E},	/*All PIM Routers  224.0.0.13*/
	{ 18, CPSS_IP_PROTOCOL_IPV4V6_E, CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E},	/*VRRP  224.0.0.18*/
};

typedef struct mutiInfo{
	unsigned int actType;
	unsigned char ieeeResProtocol;
	CPSS_NET_RX_CPU_CODE_ENT ieeeCpuCode;
}mutiInfo;

mutiInfo LinkLocalMulticastToCpuIndex[] =
{
	{ CPSS_PACKET_CMD_TRAP_TO_CPU_E, 0x15, CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E},
	{ CPSS_PACKET_CMD_TRAP_TO_CPU_E, 0x14, CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E}, 
};
#endif


/*******************************************************************************
* prvCpssDxCh2Ipv4PrefixSet
*
* DESCRIPTION:
*    Sets an ipv4 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to set
*       routerTtiTcamColumn       - the TCAM column to set
*       prefixPtr                 - prefix to set
*       maskPtr                   - mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on fail.
*       GT_BAD_PARAM    - on devNum not active
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh2Ipv4PrefixSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  GT_U32                      routerTtiTcamColumn,
    IN  CPSS_DXCH_IPV4_PREFIX_STC   *prefixPtr,
    IN  CPSS_DXCH_IPV4_PREFIX_STC   *maskPtr
)
{
    GT_U32  keyArr[6];    /* TCAM key in hw format  */
    GT_U32  maskArr[6];   /* TCAM mask in hw format */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    GT_U32  mcGroupIndex;
    GT_U32  mcGroupIndexMask;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(prefixPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.routerAndTunnelTermTcam)
        return GT_BAD_PARAM;
    if (routerTtiTcamColumn >= 5)
        return GT_BAD_PARAM;
    if (prefixPtr->isMcSource == GT_TRUE)
    {
        if (prefixPtr->mcGroupIndexRow >= fineTuningPtr->tableSize.routerAndTunnelTermTcam)
            return GT_BAD_PARAM;
        if (prefixPtr->mcGroupIndexColumn >= 5)
            return GT_BAD_PARAM;
        /* in Ch2 mc source can reside only on second and third columns */
        if ((routerTtiTcamColumn != 1) && (routerTtiTcamColumn != 2))
            return GT_BAD_PARAM;
    }

    if (prefixPtr->isMcSource == GT_TRUE)
        mcGroupIndex = (prefixPtr->mcGroupIndexColumn * fineTuningPtr->tableSize.routerAndTunnelTermTcam) + prefixPtr->mcGroupIndexRow;
    else
        mcGroupIndex = 0;

    if (maskPtr->isMcSource == GT_TRUE)
        mcGroupIndexMask = (maskPtr->mcGroupIndexColumn * fineTuningPtr->tableSize.routerAndTunnelTermTcam) + maskPtr->mcGroupIndexRow;
    else
        mcGroupIndexMask = 0;

    /***********************************/
    /* convert to TCAM hardware format */
    /***********************************/

    /* clear hw data */
    cpssOsMemSet(keyArr, 0, sizeof(GT_U32) * 6);
    cpssOsMemSet(maskArr, 0, sizeof(GT_U32) * 6);

    maskArr[2] = 0xFFFF;
    maskArr[3] = 0xFFFF0000;

    /* set the ip address & mask*/
    switch (routerTtiTcamColumn)
    {
        case 0:
        keyArr[0] = (prefixPtr->ipAddr.arIP[3]) |
                    (prefixPtr->ipAddr.arIP[2] << 8) |
                    (prefixPtr->ipAddr.arIP[1] << 16)|
                    (prefixPtr->ipAddr.arIP[0] << 24);

        maskArr[0] = (maskPtr->ipAddr.arIP[3]) |
                     (maskPtr->ipAddr.arIP[2] << 8) |
                     (maskPtr->ipAddr.arIP[1] << 16)|
                     (maskPtr->ipAddr.arIP[0] << 24);
        break;

    case 1:
        keyArr[1] = (prefixPtr->ipAddr.arIP[3]) |
                    (prefixPtr->ipAddr.arIP[2] << 8) |
                    (prefixPtr->ipAddr.arIP[1] << 16)|
                    (prefixPtr->ipAddr.arIP[0] << 24);

        keyArr[2] = (mcGroupIndex & 0x3FFF);

        maskArr[1] = (maskPtr->ipAddr.arIP[3]) |
                     (maskPtr->ipAddr.arIP[2] << 8) |
                     (maskPtr->ipAddr.arIP[1] << 16)|
                     (maskPtr->ipAddr.arIP[0] << 24);

        maskArr[2] = (mcGroupIndexMask & 0x3FFF);

        break;

    case 2:
        keyArr[2] = (prefixPtr->ipAddr.arIP[3] << 16) |
                    (prefixPtr->ipAddr.arIP[2] << 24);

        keyArr[3] = (prefixPtr->ipAddr.arIP[1]) |
                    (prefixPtr->ipAddr.arIP[0] << 8) |
                       (mcGroupIndex & 0x3FFF) << 16;

        maskArr[2] |= (maskPtr->ipAddr.arIP[3] << 16)|
                      (maskPtr->ipAddr.arIP[2] << 24);

        maskArr[3] |= (maskPtr->ipAddr.arIP[1]) |
                      (maskPtr->ipAddr.arIP[0] << 8) |
                      (mcGroupIndexMask & 0x3FFF) << 16;

        break;

    case 3:
        keyArr[4] = (prefixPtr->ipAddr.arIP[3]) |
                    (prefixPtr->ipAddr.arIP[2] << 8) |
                    (prefixPtr->ipAddr.arIP[1] << 16)|
                    (prefixPtr->ipAddr.arIP[0] << 24);

        maskArr[4] = (maskPtr->ipAddr.arIP[3]) |
                     (maskPtr->ipAddr.arIP[2] << 8) |
                     (maskPtr->ipAddr.arIP[1] << 16)|
                     (maskPtr->ipAddr.arIP[0] << 24);
        break;

    case 4:
        keyArr[5] = (prefixPtr->ipAddr.arIP[3]) |
                    (prefixPtr->ipAddr.arIP[2] << 8) |
                    (prefixPtr->ipAddr.arIP[1] << 16)|
                    (prefixPtr->ipAddr.arIP[0] << 24);

        maskArr[5] = (maskPtr->ipAddr.arIP[3]) |
                     (maskPtr->ipAddr.arIP[2] << 8) |
                     (maskPtr->ipAddr.arIP[1] << 16)|
                     (maskPtr->ipAddr.arIP[0] << 24);
        break;
    default:
        return(GT_FAIL);
    }

    /*****************************/
    /* write the TCAM key & mask */
    /*****************************/

    return  prvCpssDxChRouterTunnelTermTcamKeyMaskWriteEntry(devNum,
                                                             CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                             routerTtiTcamRow,routerTtiTcamColumn,
                                                             keyArr,maskArr);
}

/*******************************************************************************
* prvCpssDxCh3Ipv4PrefixSet
*
* DESCRIPTION:
*    Sets an ipv4 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to set
*       routerTtiTcamColumn       - the TCAM column to set
*       prefixPtr                 - prefix to set
*       maskPtr                   - mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on fail.
*       GT_BAD_PARAM    - on devNum not active
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3Ipv4PrefixSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  GT_U32                      routerTtiTcamColumn,
    IN  CPSS_DXCH_IPV4_PREFIX_STC   *prefixPtr,
    IN  CPSS_DXCH_IPV4_PREFIX_STC   *maskPtr
)
{
    GT_U32  keyArr[6];      /* TCAM key in hw format  */
    GT_U32  maskArr[6];     /* TCAM mask in hw format */
    GT_U32  wordOffset;     /* word offset in hw format to set */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(prefixPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.routerAndTunnelTermTcam)
        return GT_BAD_PARAM;
    if (routerTtiTcamColumn >= 4)
        return GT_BAD_PARAM;
    if (prefixPtr->isMcSource == GT_TRUE)
    {
        if (prefixPtr->mcGroupIndexRow >= fineTuningPtr->tableSize.routerAndTunnelTermTcam)
            return GT_BAD_PARAM;
        if (prefixPtr->mcGroupIndexColumn >= 4)
            return GT_BAD_PARAM;
    }
    else
    {
        if (prefixPtr->vrId >= 4096)
            return GT_BAD_PARAM;
    }

    /***********************************/
    /* convert to TCAM hardware format */
    /***********************************/

    /* clear hw data */
    cpssOsMemSet(keyArr, 0, sizeof(GT_U32) * 6);
    cpssOsMemSet(maskArr, 0, sizeof(GT_U32) * 6);

    /* set the ip address & mask*/
    switch (routerTtiTcamColumn)
    {
    case 0:
    case 2:
        wordOffset = (routerTtiTcamColumn == 0) ? 0 : 3;

        keyArr[wordOffset] = (prefixPtr->ipAddr.arIP[3]) |
                             (prefixPtr->ipAddr.arIP[2] << 8) |
                             (prefixPtr->ipAddr.arIP[1] << 16)|
                             (prefixPtr->ipAddr.arIP[0] << 24);

        maskArr[wordOffset] = (maskPtr->ipAddr.arIP[3]) |
                              (maskPtr->ipAddr.arIP[2] << 8) |
                              (maskPtr->ipAddr.arIP[1] << 16)|
                              (maskPtr->ipAddr.arIP[0] << 24);

        if (prefixPtr->isMcSource == GT_FALSE)
        {
            keyArr[wordOffset + 1] = (prefixPtr->vrId & 0xfff) |
                                     (0 << 15);     /* indicates not Mc source */

            maskArr[wordOffset + 1] = (maskPtr->vrId & 0xfff) |
                                      (1 << 15);    /* Mc source bit can't be masked */
        }
        else
        {
            keyArr[wordOffset + 1] = (prefixPtr->mcGroupIndexRow & 0x1fff) |
                                     ((prefixPtr->mcGroupIndexColumn & 0x3) << 13) |
                                     (1 << 15);     /* indicates Mc source */

            maskArr[wordOffset + 1] = (maskPtr->mcGroupIndexRow & 0x1fff) |
                                      ((maskPtr->mcGroupIndexColumn & 0x3) << 13) |
                                      (1 << 15);    /* Mc source bit can't be masked */
        }
        break;

    case 1:
    case 3:
        wordOffset = (routerTtiTcamColumn == 1) ? 1 : 4;

        keyArr[wordOffset] = (prefixPtr->ipAddr.arIP[3] << 16) |
                             (prefixPtr->ipAddr.arIP[2] << 24);

        keyArr[wordOffset + 1] = (prefixPtr->ipAddr.arIP[1]) |
                                 (prefixPtr->ipAddr.arIP[0] << 8);

        maskArr[wordOffset] = (maskPtr->ipAddr.arIP[3] << 16) |
                              (maskPtr->ipAddr.arIP[2] << 24);

        maskArr[wordOffset + 1] = (maskPtr->ipAddr.arIP[1]) |
                                  (maskPtr->ipAddr.arIP[0] << 8);

        if (prefixPtr->isMcSource == GT_FALSE)
        {
            keyArr[wordOffset + 1] |= ((prefixPtr->vrId & 0xfff) << 16) |
                                      (0 << 31);     /* indicates not Mc source */

            maskArr[wordOffset + 1] |= ((maskPtr->vrId & 0xfff) << 16) |
                                       (1 << 31);    /* Mc source bit can't be masked */
        }
        else
        {
            keyArr[wordOffset + 1] |= ((prefixPtr->mcGroupIndexRow & 0x1fff) << 16) |
                                      ((prefixPtr->mcGroupIndexColumn & 0x3) << 29) |
                                      (1 << 31);     /* indicates Mc source */

            maskArr[wordOffset + 1] |= ((maskPtr->mcGroupIndexRow & 0x1fff) << 16) |
                                       ((maskPtr->mcGroupIndexColumn & 0x3) << 29) |
                                       (1 << 31);    /* Mc source bit can't be masked */
        }
        break;

    default:
        return(GT_FAIL);
    }

    /*****************************/
    /* write the TCAM key & mask */
    /*****************************/

    return  prvCpssDxChRouterTunnelTermTcamKeyMaskWriteEntry(devNum,
                                                             CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                             routerTtiTcamRow,routerTtiTcamColumn,
                                                             keyArr,maskArr);
}


/*******************************************************************************
* cpssDxChIpv4PrefixSet
*
* DESCRIPTION:
*    Sets an ipv4 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to set
*       routerTtiTcamColumn       - the TCAM column to set
*       prefixPtr                 - prefix to set
*       maskPtr                   - mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on fail.
*       GT_BAD_PARAM    - on devNum not active
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   1.  IP prefix entries reside in router TCAM. The router TCAM is organized
*       in rows, where each row contains 4 column entries for Ch3 and 5 column
*       entries for Ch2. Each line can hold:
*       - 1 TTI (tunnel termination interface) entry
*       - 1 ipv6 address
*       - 4 (ch3)/ 5 (ch2) ipv4 addresses
*       Note that if a line is used for ipv6 address or TTI entry then
*       it can't be used for ipv4 addresses and if an entry in a line is used
*       for ipv4 address, then the other line entries must hold ipv4 addresses
*       as well.
*   2.  The match for prefixes is done using a first match , thus if two prefixes
*       can match a packet (they have the same prefix , but diffrent prefix
*       length) and we want LPM to work on them we have to make sure to put
*       the longest prefix before the short one.
*       Search order is by columns, first column, then second and so on.
*       All and all default UC prefix 0.0.0.0/mask 0 must reside at the last
*       possible ipv4 prefix, and default MC prefix 224.0.0.0/ mask 240.0.0.0
*       must reside at the index before it.
*   3.  In Ch2 MC source addresses can reside only on the second and third
*       columns (on other PP, there is no such restriction).
*   4.  In Ch2 there is no support for multiple virtual routers. Therefore in
*       Ch2 vrId is ignored.
*
*******************************************************************************/
GT_STATUS cpssDxChIpv4PrefixSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  GT_U32                      routerTtiTcamColumn,
    IN  CPSS_DXCH_IPV4_PREFIX_STC   *prefixPtr,
    IN  CPSS_DXCH_IPV4_PREFIX_STC   *maskPtr
)
{
    /* make sure the device is Ch2 and above */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH2_E:
        return prvCpssDxCh2Ipv4PrefixSet(devNum,
                                         routerTtiTcamRow,routerTtiTcamColumn,
                                         prefixPtr,maskPtr);
    default:
        return prvCpssDxCh3Ipv4PrefixSet(devNum,
                                         routerTtiTcamRow,routerTtiTcamColumn,
                                         prefixPtr,maskPtr);
    }
}

/*******************************************************************************
* prvCpssDxCh2Ipv4PrefixGet
*
* DESCRIPTION:
*    Gets an ipv4 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to get from
*       routerTtiTcamColumn       - the TCAM column to get from
*
* OUTPUTS:
*       validPtr              - whether the entry is valid (if the entry isn't valid
*                               all the following fields will not get values)
*       prefixPtr             - prefix to set
*       maskPtr               - mask of the prefix
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh2Ipv4PrefixGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  GT_U32                      routerTtiTcamColumn,
    OUT GT_BOOL                     *validPtr,
    OUT CPSS_DXCH_IPV4_PREFIX_STC   *prefixPtr,
    OUT CPSS_DXCH_IPV4_PREFIX_STC   *maskPtr
)
{
    GT_U32  keyArr[6];          /* TCAM key in hw format            */
    GT_U32  maskArr[6];         /* TCAM mask in hw format           */
    GT_U32  actionArr[6];       /* TCAM action (rule) in hw format  */
    GT_U32  validArr[5];        /* TCAM line valid bits             */
    GT_U32  compareModeArr[5];  /* TCAM line compare mode           */
    GT_U32  retVal = GT_OK;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.routerAndTunnelTermTcam)
        return GT_BAD_PARAM;
    if (routerTtiTcamColumn >= 5)
        return GT_BAD_PARAM;
    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    CPSS_NULL_PTR_CHECK_MAC(prefixPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

     /* clear data */
    cpssOsMemSet(keyArr, 0, sizeof(GT_U32) * 6);
    cpssOsMemSet(maskArr, 0, sizeof(GT_U32) * 6);
    cpssOsMemSet(prefixPtr, 0, sizeof(CPSS_DXCH_IPV4_PREFIX_STC));
    cpssOsMemSet(maskPtr, 0, sizeof(CPSS_DXCH_IPV4_PREFIX_STC));

    /* read hw data */
    retVal = prvCpssDxChRouterTunnelTermTcamGetLine(devNum,
                                                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    routerTtiTcamRow,
                                                    &validArr[0],
                                                    &compareModeArr[0],
                                                    &keyArr[0],&maskArr[0],
                                                    &actionArr[0]);
    if (retVal != GT_OK)
        return retVal;

    /* convert from HW format to ip address & mask */
    /***********************************************/

    switch (routerTtiTcamColumn)
    {
    case 0:
        prefixPtr->ipAddr.arIP[3] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 0,8);
        prefixPtr->ipAddr.arIP[2] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 8,8);
        prefixPtr->ipAddr.arIP[1] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 16,8);
        prefixPtr->ipAddr.arIP[0] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 24,8);

        maskPtr->ipAddr.arIP[3]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 0,8);
        maskPtr->ipAddr.arIP[2]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 8,8);
        maskPtr->ipAddr.arIP[1]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 16,8);
        maskPtr->ipAddr.arIP[0]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 24,8);

        break;

    case 1:
        prefixPtr->ipAddr.arIP[3] = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 0,8);
        prefixPtr->ipAddr.arIP[2] = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 8,8);
        prefixPtr->ipAddr.arIP[1] = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 16,8);
        prefixPtr->ipAddr.arIP[0] = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 24,8);

        prefixPtr->mcGroupIndexRow    = U32_GET_FIELD_MAC(keyArr[2], 0,10);
        prefixPtr->mcGroupIndexColumn = U32_GET_FIELD_MAC(keyArr[2], 10,4);

        maskPtr->ipAddr.arIP[3]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 0,8);
        maskPtr->ipAddr.arIP[2]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 8,8);
        maskPtr->ipAddr.arIP[1]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 16,8);
        maskPtr->ipAddr.arIP[0]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 24,8);

        maskPtr->mcGroupIndexRow    = U32_GET_FIELD_MAC(maskArr[2], 0,10);
        maskPtr->mcGroupIndexColumn = U32_GET_FIELD_MAC(maskArr[2], 10,4);

        break;

    case 2:
        prefixPtr->ipAddr.arIP[3] = (GT_U8)U32_GET_FIELD_MAC(keyArr[2], 16,8);
        prefixPtr->ipAddr.arIP[2] = (GT_U8)U32_GET_FIELD_MAC(keyArr[2], 24,8);

        prefixPtr->ipAddr.arIP[1] = (GT_U8)U32_GET_FIELD_MAC(keyArr[3], 0,8);
        prefixPtr->ipAddr.arIP[0] = (GT_U8)U32_GET_FIELD_MAC(keyArr[3], 8,8);

        prefixPtr->mcGroupIndexRow    = U32_GET_FIELD_MAC(keyArr[3], 16,10);
        prefixPtr->mcGroupIndexColumn = U32_GET_FIELD_MAC(keyArr[3], 26,4);

        maskPtr->ipAddr.arIP[3]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[2], 16,8);
        maskPtr->ipAddr.arIP[2]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[2], 24,8);

        maskPtr->ipAddr.arIP[1]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[3], 0,8);
        maskPtr->ipAddr.arIP[0]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[3], 8,8);

        maskPtr->mcGroupIndexRow    = U32_GET_FIELD_MAC(maskArr[3], 16,10);
        maskPtr->mcGroupIndexColumn = U32_GET_FIELD_MAC(maskArr[3], 26,4);

        break;

    case 3:
        prefixPtr->ipAddr.arIP[3] = (GT_U8)U32_GET_FIELD_MAC(keyArr[4], 0,8);
        prefixPtr->ipAddr.arIP[2] = (GT_U8)U32_GET_FIELD_MAC(keyArr[4], 8,8);
        prefixPtr->ipAddr.arIP[1] = (GT_U8)U32_GET_FIELD_MAC(keyArr[4], 16,8);
        prefixPtr->ipAddr.arIP[0] = (GT_U8)U32_GET_FIELD_MAC(keyArr[4], 24,8);

        maskPtr->ipAddr.arIP[3]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[4], 0,8);
        maskPtr->ipAddr.arIP[2]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[4], 8,8);
        maskPtr->ipAddr.arIP[1]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[4], 16,8);
        maskPtr->ipAddr.arIP[0]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[4], 24,8);

        break;

    case 4:
        prefixPtr->ipAddr.arIP[3] = (GT_U8)U32_GET_FIELD_MAC(keyArr[5], 0,8);
        prefixPtr->ipAddr.arIP[2] = (GT_U8)U32_GET_FIELD_MAC(keyArr[5], 8,8);
        prefixPtr->ipAddr.arIP[1] = (GT_U8)U32_GET_FIELD_MAC(keyArr[5], 16,8);
        prefixPtr->ipAddr.arIP[0] = (GT_U8)U32_GET_FIELD_MAC(keyArr[5], 24,8);

        maskPtr->ipAddr.arIP[3]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[5], 0,8);
        maskPtr->ipAddr.arIP[2]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[5], 8,8);
        maskPtr->ipAddr.arIP[1]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[5], 16,8);
        maskPtr->ipAddr.arIP[0]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[5], 24,8);

        break;
    default:
        return(GT_FAIL);
    }

    if ((prefixPtr->mcGroupIndexColumn != 0) || (prefixPtr->mcGroupIndexRow != 0))
        maskPtr->isMcSource = prefixPtr->isMcSource = GT_TRUE;
    else
        maskPtr->isMcSource = prefixPtr->isMcSource = GT_FALSE;

    /* entry holds valid IPv4 prefix if the followings applies:
         - the entry is valid
         - the compare mode of the entry is single compare mode */
    if ((validArr[routerTtiTcamColumn] == 1) && (compareModeArr[routerTtiTcamColumn] == 0))
        *validPtr = GT_TRUE;
    else
        *validPtr = GT_FALSE;

    return retVal;
}

/*******************************************************************************
* prvCpssDxCh3Ipv4PrefixGet
*
* DESCRIPTION:
*    Gets an ipv4 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to get from
*       routerTtiTcamColumn       - the TCAM column to get from
*
* OUTPUTS:
*       validPtr              - whether the entry is valid (if the entry isn't valid
*                               all the following fields will not get values)
*       prefixPtr             - prefix to set
*       maskPtr               - mask of the prefix
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3Ipv4PrefixGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  GT_U32                      routerTtiTcamColumn,
    OUT GT_BOOL                     *validPtr,
    OUT CPSS_DXCH_IPV4_PREFIX_STC   *prefixPtr,
    OUT CPSS_DXCH_IPV4_PREFIX_STC   *maskPtr
)
{
    GT_U32  keyArr[6];          /* TCAM key in hw format            */
    GT_U32  maskArr[6];         /* TCAM mask in hw format           */
    GT_U32  actionArr[6];       /* TCAM action (rule) in hw format  */
    GT_U32  validArr[5];        /* TCAM line valid bits             */
    GT_U32  compareModeArr[5];  /* TCAM line compare mode           */
    GT_U32  wordOffset;         /* word offset in hw format to set  */
    GT_U32  retVal = GT_OK;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT2_E);


    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.routerAndTunnelTermTcam)
        return GT_BAD_PARAM;
    if (routerTtiTcamColumn >= 4)
        return GT_BAD_PARAM;
    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    CPSS_NULL_PTR_CHECK_MAC(prefixPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

     /* clear data */
    cpssOsMemSet(keyArr, 0, sizeof(GT_U32) * 6);
    cpssOsMemSet(maskArr, 0, sizeof(GT_U32) * 6);
    cpssOsMemSet(prefixPtr, 0, sizeof(CPSS_DXCH_IPV4_PREFIX_STC));
    cpssOsMemSet(maskPtr, 0, sizeof(CPSS_DXCH_IPV4_PREFIX_STC));

    /* read hw data */

    retVal = prvCpssDxChRouterTunnelTermTcamGetLine(devNum,
                                                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    routerTtiTcamRow,
                                                    &validArr[0],
                                                    &compareModeArr[0],
                                                    &keyArr[0],&maskArr[0],
                                                    &actionArr[0]);
    if (retVal != GT_OK)
        return retVal;

    /* convert from HW format to ip address & mask */
    /***********************************************/

    switch (routerTtiTcamColumn)
    {
    case 0:
    case 2:
        wordOffset = (routerTtiTcamColumn == 0) ? 0 : 3;

        prefixPtr->ipAddr.arIP[3] = (GT_U8)U32_GET_FIELD_MAC(keyArr[wordOffset], 0,8);
        prefixPtr->ipAddr.arIP[2] = (GT_U8)U32_GET_FIELD_MAC(keyArr[wordOffset], 8,8);
        prefixPtr->ipAddr.arIP[1] = (GT_U8)U32_GET_FIELD_MAC(keyArr[wordOffset], 16,8);
        prefixPtr->ipAddr.arIP[0] = (GT_U8)U32_GET_FIELD_MAC(keyArr[wordOffset], 24,8);

        maskPtr->ipAddr.arIP[3]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[wordOffset], 0,8);
        maskPtr->ipAddr.arIP[2]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[wordOffset], 8,8);
        maskPtr->ipAddr.arIP[1]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[wordOffset], 16,8);
        maskPtr->ipAddr.arIP[0]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[wordOffset], 24,8);

        prefixPtr->isMcSource = maskPtr->isMcSource =
            (((keyArr[wordOffset + 1] >> 15) & 0x1) == 0) ? GT_FALSE : GT_TRUE;

        if (prefixPtr->isMcSource == GT_FALSE)
        {
            prefixPtr->vrId = (keyArr[wordOffset + 1] & 0xfff);
            maskPtr->vrId =   (maskArr[wordOffset + 1] & 0xfff);
        }
        else
        {
            prefixPtr->mcGroupIndexRow =    ((keyArr[wordOffset + 1] & 0x1fff));
            prefixPtr->mcGroupIndexColumn = ((keyArr[wordOffset + 1] >> 13) & 0x3);
            maskPtr->mcGroupIndexRow =      ((maskArr[wordOffset + 1] & 0x1fff));
            maskPtr->mcGroupIndexColumn =   ((maskArr[wordOffset + 1] >> 13) & 0x3);
        }

        break;

    case 1:
    case 3:
        wordOffset = (routerTtiTcamColumn == 1) ? 1 : 4;

        prefixPtr->ipAddr.arIP[3] = (GT_U8)U32_GET_FIELD_MAC(keyArr[wordOffset], 16,8);
        prefixPtr->ipAddr.arIP[2] = (GT_U8)U32_GET_FIELD_MAC(keyArr[wordOffset], 24,8);

        prefixPtr->ipAddr.arIP[1] = (GT_U8)U32_GET_FIELD_MAC(keyArr[wordOffset + 1], 0,8);
        prefixPtr->ipAddr.arIP[0] = (GT_U8)U32_GET_FIELD_MAC(keyArr[wordOffset + 1], 8,8);

        maskPtr->ipAddr.arIP[3]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[wordOffset], 16,8);
        maskPtr->ipAddr.arIP[2]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[wordOffset], 24,8);

        maskPtr->ipAddr.arIP[1]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[wordOffset + 1], 0,8);
        maskPtr->ipAddr.arIP[0]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[wordOffset + 1], 8,8);

        prefixPtr->isMcSource = maskPtr->isMcSource =
            (((keyArr[wordOffset + 1] >> 31) & 0x1) == 0) ? GT_FALSE : GT_TRUE;

        if (prefixPtr->isMcSource == GT_FALSE)
        {
            prefixPtr->vrId = ((keyArr[wordOffset + 1] >> 16) & 0xfff);
            maskPtr->vrId =   ((maskArr[wordOffset + 1] >> 16) & 0xfff);
        }
        else
        {
            prefixPtr->mcGroupIndexRow =    ((keyArr[wordOffset + 1] >> 16) & 0x1fff);
            prefixPtr->mcGroupIndexColumn = ((keyArr[wordOffset + 1] >> 29) & 0x3);
            maskPtr->mcGroupIndexRow =      ((maskArr[wordOffset + 1] >> 16) & 0x1fff);
            maskPtr->mcGroupIndexColumn =   ((maskArr[wordOffset + 1] >> 29) & 0x3);
        }

        break;

    default:
        return(GT_FAIL);
    }

    /* entry holds valid IPv4 prefix if the followings applies:
         - the entry is valid
         - the compare mode of the entry is single compare mode */
    if ((validArr[routerTtiTcamColumn] == 1) && (compareModeArr[routerTtiTcamColumn] == 0))
        *validPtr = GT_TRUE;
    else
        *validPtr = GT_FALSE;

    return retVal;
}

/*******************************************************************************
* cpssDxChIpv4PrefixGet
*
* DESCRIPTION:
*    Gets an ipv4 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to get from
*       routerTtiTcamColumn       - the TCAM column to get from
*
* OUTPUTS:
*       validPtr              - whether the entry is valid (if the entry isn't valid
*                               all the following fields will not get values)
*       prefixPtr             - prefix to set
*       maskPtr               - mask of the prefix
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*******************************************************************************/
GT_STATUS cpssDxChIpv4PrefixGet
(
    IN  GT_U8       devNum,
    IN  GT_U32      routerTtiTcamRow,
    IN  GT_U32      routerTtiTcamColumn,
    OUT GT_BOOL     *validPtr,
    OUT CPSS_DXCH_IPV4_PREFIX_STC   *prefixPtr,
    OUT CPSS_DXCH_IPV4_PREFIX_STC   *maskPtr
)
{
    /* make sure the device is Ch2 and above */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH2_E:
        return prvCpssDxCh2Ipv4PrefixGet(devNum,
                                         routerTtiTcamRow,routerTtiTcamColumn,
                                         validPtr,prefixPtr,maskPtr);
    default:
        return prvCpssDxCh3Ipv4PrefixGet(devNum,
                                         routerTtiTcamRow,routerTtiTcamColumn,
                                         validPtr,prefixPtr,maskPtr);
    }
}

/*******************************************************************************
* cpssDxChIpv4PrefixInvalidate
*
* DESCRIPTION:
*    Invalidates an ipv4 UC or MC prefix in the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to invalidate
*       routerTtiTcamColumn       - the TCAM column to invalidate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpv4PrefixInvalidate
(
    IN  GT_U8       devNum,
    IN  GT_U32      routerTtiTcamRow,
    IN  GT_U32      routerTtiTcamColumn
)
{
    GT_U32  retVal = GT_OK;         /* function return code */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.routerAndTunnelTermTcam)
        return GT_BAD_PARAM;
    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH2_E:
        if (routerTtiTcamColumn >= 5)
            return GT_BAD_PARAM;
        break;
    default:
        if (routerTtiTcamColumn >= 4)
            return GT_BAD_PARAM;
        break;
    }

    retVal = prvCpssDxChRouterTunnelTermTcamInvalidateEntry(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,routerTtiTcamRow,routerTtiTcamColumn);
    return retVal;
}

/*******************************************************************************
* prvCpssDxCh2Ipv6PrefixSet
*
* DESCRIPTION:
*    Sets an ipv6 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to set
*       prefixPtr                 - prefix to set
*       maskPtr                   - mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh2Ipv6PrefixSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  CPSS_DXCH_IPV6_PREFIX_STC   *prefixPtr,
    IN  CPSS_DXCH_IPV6_PREFIX_STC   *maskPtr
)
{
    GT_U32  keyArr[6];   /* TCAM key in hw format  */
    GT_U32  maskArr[6];  /* TCAM mask in hw format */
    GT_U32  retVal;      /* function return code   */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH2_E)
        return GT_BAD_PARAM;
    CPSS_NULL_PTR_CHECK_MAC(prefixPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.routerAndTunnelTermTcam)
        return GT_BAD_PARAM;
    if (prefixPtr->isMcSource == GT_TRUE)
    {
        if (prefixPtr->mcGroupIndexRow >= fineTuningPtr->tableSize.routerAndTunnelTermTcam)
            return GT_BAD_PARAM;
    }

    /***********************************/
    /* convert to TCAM hardware format */
    /***********************************/

    /* clear hw data */
     cpssOsMemSet(keyArr, 0, sizeof(GT_U32) * 6);
     cpssOsMemSet(maskArr, 0, sizeof(GT_U32) * 6);

    /* set the ip address */
    /**********************/

    keyArr[0] = (prefixPtr->ipAddr.arIP[15]) |
                (prefixPtr->ipAddr.arIP[14] << 8) |
                (prefixPtr->ipAddr.arIP[13] << 16)|
                (prefixPtr->ipAddr.arIP[12] << 24);

    keyArr[1] = (prefixPtr->ipAddr.arIP[11]) |
                (prefixPtr->ipAddr.arIP[10] << 8) |
                (prefixPtr->ipAddr.arIP[9]  << 16)|
                (prefixPtr->ipAddr.arIP[8]  << 24);

    if (prefixPtr->isMcSource == GT_TRUE)
    {
        keyArr[2] = (prefixPtr->mcGroupIndexRow & 0x3FFF);
    }

    keyArr[3] = 0;

    keyArr[4] = (prefixPtr->ipAddr.arIP[7]) |
                (prefixPtr->ipAddr.arIP[6] << 8) |
                (prefixPtr->ipAddr.arIP[5] << 16)|
                (prefixPtr->ipAddr.arIP[4] << 24);

    keyArr[5] = (prefixPtr->ipAddr.arIP[3]) |
                (prefixPtr->ipAddr.arIP[2] << 8) |
                (prefixPtr->ipAddr.arIP[1] << 16)|
                (prefixPtr->ipAddr.arIP[0] << 24);

    /* set the mask */
    /****************/

    maskArr[0] = (maskPtr->ipAddr.arIP[15]) |
                 (maskPtr->ipAddr.arIP[14] << 8) |
                 (maskPtr->ipAddr.arIP[13] << 16)|
                 (maskPtr->ipAddr.arIP[12] << 24);

    maskArr[1] = (maskPtr->ipAddr.arIP[11]) |
                 (maskPtr->ipAddr.arIP[10] << 8) |
                 (maskPtr->ipAddr.arIP[9]  << 16)|
                 (maskPtr->ipAddr.arIP[8]  << 24);

    if (prefixPtr->isMcSource == GT_TRUE)
    {
        maskArr[2] = (maskPtr->mcGroupIndexRow & 0x3FFF) |
                    (0xffffc000);
    }

    maskArr[3] = 0xFFFFFFFF;

    maskArr[4] = (maskPtr->ipAddr.arIP[7]) |
                 (maskPtr->ipAddr.arIP[6] << 8) |
                 (maskPtr->ipAddr.arIP[5] << 16)|
                 (maskPtr->ipAddr.arIP[4] << 24);

    maskArr[5] = (maskPtr->ipAddr.arIP[3]) |
                 (maskPtr->ipAddr.arIP[2] << 8) |
                 (maskPtr->ipAddr.arIP[1] << 16)|
                 (maskPtr->ipAddr.arIP[0] << 24);


    /*****************************/
    /* write the TCAM key & mask */
    /*****************************/

    retVal =  prvCpssDxChRouterTunnelTermTcamKeyMaskWriteLine(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                              routerTtiTcamRow,keyArr,maskArr);

    if (retVal != GT_OK)
        return retVal;

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxCh3Ipv6PrefixSet
*
* DESCRIPTION:
*    Sets an ipv6 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to set
*       prefixPtr                 - prefix to set
*       maskPtr                   - mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3Ipv6PrefixSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  CPSS_DXCH_IPV6_PREFIX_STC   *prefixPtr,
    IN  CPSS_DXCH_IPV6_PREFIX_STC   *maskPtr
)
{
    GT_U32  keyArr[6];   /* TCAM key in hw format  */
    GT_U32  maskArr[6];  /* TCAM mask in hw format */
    GT_U32  retVal;      /* function return code   */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(prefixPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.routerAndTunnelTermTcam)
        return GT_BAD_PARAM;
    if (prefixPtr->isMcSource == GT_TRUE)
    {
        if (prefixPtr->mcGroupIndexRow >= fineTuningPtr->tableSize.routerAndTunnelTermTcam)
            return GT_BAD_PARAM;
    }
    else
    {
        if (prefixPtr->vrId >= 4096)
            return GT_BAD_PARAM;
    }

    /***********************************/
    /* convert to TCAM hardware format */
    /***********************************/

    /* clear hw data */
     cpssOsMemSet(keyArr, 0, sizeof(GT_U32) * 6);
     cpssOsMemSet(maskArr, 0, sizeof(GT_U32) * 6);

    /* set the ip address key */
    /**************************/

    keyArr[0] = (prefixPtr->ipAddr.arIP[15]) |
                (prefixPtr->ipAddr.arIP[14] << 8) |
                (prefixPtr->ipAddr.arIP[13] << 16)|
                (prefixPtr->ipAddr.arIP[12] << 24);

    keyArr[1] = (prefixPtr->ipAddr.arIP[11]) |
                (prefixPtr->ipAddr.arIP[10] << 8) |
                (prefixPtr->ipAddr.arIP[9]  << 16)|
                (prefixPtr->ipAddr.arIP[8]  << 24);

    keyArr[2] = (prefixPtr->ipAddr.arIP[7]) |
                (prefixPtr->ipAddr.arIP[6] << 8) |
                (prefixPtr->ipAddr.arIP[5] << 16)|
                (prefixPtr->ipAddr.arIP[4] << 24);

    keyArr[3] = (prefixPtr->ipAddr.arIP[3]) |
                (prefixPtr->ipAddr.arIP[2] << 8) |
                (prefixPtr->ipAddr.arIP[1] << 16)|
                (prefixPtr->ipAddr.arIP[0] << 24);

    /* set the ip address mask */
    /***************************/

    maskArr[0] = (maskPtr->ipAddr.arIP[15]) |
                 (maskPtr->ipAddr.arIP[14] << 8) |
                 (maskPtr->ipAddr.arIP[13] << 16)|
                 (maskPtr->ipAddr.arIP[12] << 24);

    maskArr[1] = (maskPtr->ipAddr.arIP[11]) |
                 (maskPtr->ipAddr.arIP[10] << 8) |
                 (maskPtr->ipAddr.arIP[9]  << 16)|
                 (maskPtr->ipAddr.arIP[8]  << 24);

    maskArr[2] = (maskPtr->ipAddr.arIP[7]) |
                 (maskPtr->ipAddr.arIP[6] << 8) |
                 (maskPtr->ipAddr.arIP[5] << 16)|
                 (maskPtr->ipAddr.arIP[4] << 24);

    maskArr[3] = (maskPtr->ipAddr.arIP[3]) |
                 (maskPtr->ipAddr.arIP[2] << 8) |
                 (maskPtr->ipAddr.arIP[1] << 16)|
                 (maskPtr->ipAddr.arIP[0] << 24);

    /* set the vr Id or MC Group Index */
    /***********************************/

    if (prefixPtr->isMcSource == GT_FALSE)
    {
        keyArr[4] = (prefixPtr->vrId & 0xfff) |
                    (0 << 15); /* indicates not Mc Source */

        maskArr[4] = (maskPtr->vrId & 0xfff) |
                      0xffff8000;
    }
    else
    {
        keyArr[4] = (prefixPtr->mcGroupIndexRow & 0x7fff) |
                    (1 << 15); /* indicate Mc source */

        maskArr[4] = (maskPtr->mcGroupIndexRow & 0x7fff) |
                     0xffffffff;
    }

    keyArr[5] = 0;

    maskArr[5] = 0xffffffff;


    /*****************************/
    /* write the TCAM key & mask */
    /*****************************/
    retVal =  prvCpssDxChRouterTunnelTermTcamKeyMaskWriteLine(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                              routerTtiTcamRow,keyArr,maskArr);
    if (retVal != GT_OK)
        return retVal;

    return GT_OK;
}


/*******************************************************************************
* cpssDxChIpv6PrefixSet
*
* DESCRIPTION:
*    Sets an ipv6 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to set
*       prefixPtr                 - prefix to set
*       maskPtr                   - mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpv6PrefixSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  CPSS_DXCH_IPV6_PREFIX_STC   *prefixPtr,
    IN  CPSS_DXCH_IPV6_PREFIX_STC   *maskPtr
)
{
    /* make sure the device is Ch2 and above */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH2_E:
        return prvCpssDxCh2Ipv6PrefixSet(devNum,routerTtiTcamRow,
                                         prefixPtr,maskPtr);
    default:
        return prvCpssDxCh3Ipv6PrefixSet(devNum,routerTtiTcamRow,
                                         prefixPtr,maskPtr);
    }
}


/*******************************************************************************
* prvCpssDxCh2Ipv6PrefixGet
*
* DESCRIPTION:
*    Gets an ipv6 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to get from
*
* OUTPUTS:
*       validPtr              - whether the entry is valid (if the entry isn't valid
*                               all the following fields will not get values)
*       prefixPtr             - prefix to set
*       maskPtr               - mask of the prefix
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv6PrefixSet
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh2Ipv6PrefixGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    OUT GT_BOOL                     *validPtr,
    OUT CPSS_DXCH_IPV6_PREFIX_STC   *prefixPtr,
    OUT CPSS_DXCH_IPV6_PREFIX_STC   *maskPtr
)
{
    GT_U32  keyArr[6];          /* TCAM key in hw format            */
    GT_U32  maskArr[6];         /* TCAM mask in hw format           */
    GT_U32  actionArr[6];       /* TCAM action (rule) in hw format  */
    GT_U32  validArr[5];        /* TCAM line valid bits             */
    GT_U32  compareModeArr[5];  /* TCAM line compare mode           */
    GT_U32  retVal = GT_OK;     /* function return code             */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    GT_U32  i;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH2_E)
        return GT_BAD_PARAM;

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.routerAndTunnelTermTcam)
        return GT_BAD_PARAM;
    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    CPSS_NULL_PTR_CHECK_MAC(prefixPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

     /* clear hw data */
    cpssOsMemSet(keyArr, 0, sizeof(GT_U32) * 6);
    cpssOsMemSet(maskArr, 0, sizeof(GT_U32) * 6);
    /* clear output data */
    *validPtr = GT_FALSE;
    cpssOsMemSet(prefixPtr, 0, sizeof(CPSS_DXCH_IPV6_PREFIX_STC));
    cpssOsMemSet(maskPtr, 0, sizeof(CPSS_DXCH_IPV6_PREFIX_STC));

    retVal = prvCpssDxChRouterTunnelTermTcamGetLine(devNum,
                                                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    routerTtiTcamRow,
                                                    &validArr[0],
                                                    &compareModeArr[0],
                                                    &keyArr[0],
                                                    &maskArr[0],
                                                    &actionArr[0]);
    if (retVal != GT_OK)
        return retVal;

    /* convert from HW format to ip address */
    /****************************************/

    prefixPtr->ipAddr.arIP[15] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 0,8);
    prefixPtr->ipAddr.arIP[14] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 8,8);
    prefixPtr->ipAddr.arIP[13] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 16,8);
    prefixPtr->ipAddr.arIP[12] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 24,8);

    prefixPtr->ipAddr.arIP[11] = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 0,8);
    prefixPtr->ipAddr.arIP[10] = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 8,8);
    prefixPtr->ipAddr.arIP[9]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 16,8);
    prefixPtr->ipAddr.arIP[8]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 24,8);

    prefixPtr->mcGroupIndexRow = U32_GET_FIELD_MAC(keyArr[2], 0,14);

    /* keyArr[3] = 0; */

    prefixPtr->ipAddr.arIP[7]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[4], 0,8);
    prefixPtr->ipAddr.arIP[6]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[4], 8,8);
    prefixPtr->ipAddr.arIP[5]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[4], 16,8);
    prefixPtr->ipAddr.arIP[4]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[4], 24,8);

    prefixPtr->ipAddr.arIP[3]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[5], 0,8);
    prefixPtr->ipAddr.arIP[2]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[5], 8,8);
    prefixPtr->ipAddr.arIP[1]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[5], 16,8);
    prefixPtr->ipAddr.arIP[0]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[5], 24,8);

    /* convert from HW format to mask */
    /**********************************/

    maskPtr->ipAddr.arIP[15]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 0,8);
    maskPtr->ipAddr.arIP[14]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 8,8);
    maskPtr->ipAddr.arIP[13]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 16,8);
    maskPtr->ipAddr.arIP[12]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 24,8);

    maskPtr->ipAddr.arIP[11]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 0,8);
    maskPtr->ipAddr.arIP[10]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 8,8);
    maskPtr->ipAddr.arIP[9]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 16,8);
    maskPtr->ipAddr.arIP[8]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 24,8);

    maskPtr->mcGroupIndexRow   = U32_GET_FIELD_MAC(maskArr[2], 0,14);

    /* maskArr[3] = 0xFFFFFFFF; */

    maskPtr->ipAddr.arIP[7]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[4], 0,8);
    maskPtr->ipAddr.arIP[6]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[4], 8,8);
    maskPtr->ipAddr.arIP[5]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[4], 16,8);
    maskPtr->ipAddr.arIP[4]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[4], 24,8);

    maskPtr->ipAddr.arIP[3]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[5], 0,8);
    maskPtr->ipAddr.arIP[2]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[5], 8,8);
    maskPtr->ipAddr.arIP[1]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[5], 16,8);
    maskPtr->ipAddr.arIP[0]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[5], 24,8);

    if (prefixPtr->mcGroupIndexRow != 0)
        prefixPtr->isMcSource = maskPtr->isMcSource = GT_TRUE;
    else
        prefixPtr->isMcSource = maskPtr->isMcSource = GT_FALSE;

    /* line holds valid IPv6 prefix if the following applies:
         - all entries are valid
         - the compare mode or all entries is row compare
         - keyArr[3] bit 31 must be 0 (to indicate IPv6 entry and not TT entry) */
    *validPtr = GT_TRUE;
    for (i = 0 ; i < 5 ; i++)
    {
        /* if entry is not valid or is single compare mode, whole line is not valid */
        if ((validArr[i] == 0) || (compareModeArr[i] == 0))
        {
            *validPtr = GT_FALSE;
            break;
        }
    }
    /* if whole line is valid, verify it is indeed IPv6 entry and not TT entry */
    if ((*validPtr == GT_TRUE) && (((keyArr[3] >> 31) & 0x1) != 0))
    {
        *validPtr = GT_FALSE;
    }

    return retVal;
}


/*******************************************************************************
* prvCpssDxCh3Ipv6PrefixGet
*
* DESCRIPTION:
*    Gets an ipv6 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to get from
*
* OUTPUTS:
*       validPtr              - whether the entry is valid (if the entry isn't valid
*                               all the following fields will not get values)
*       prefixPtr             - prefix to set
*       maskPtr               - mask of the prefix
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv6PrefixSet
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3Ipv6PrefixGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    OUT GT_BOOL                     *validPtr,
    OUT CPSS_DXCH_IPV6_PREFIX_STC   *prefixPtr,
    OUT CPSS_DXCH_IPV6_PREFIX_STC   *maskPtr
)
{
    GT_U32  keyArr[6];          /* TCAM key in hw format            */
    GT_U32  maskArr[6];         /* TCAM mask in hw format           */
    GT_U32  actionArr[6];       /* TCAM action (rule) in hw format  */
    GT_U32  validArr[5];        /* TCAM line valid bits             */
    GT_U32  compareModeArr[5];  /* TCAM line compare mode           */
    GT_U32  retVal = GT_OK;     /* function return code             */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    GT_U32  i;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT2_E);


    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.routerAndTunnelTermTcam)
        return GT_BAD_PARAM;
    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    CPSS_NULL_PTR_CHECK_MAC(prefixPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

     /* clear hw data */
    cpssOsMemSet(keyArr, 0, sizeof(GT_U32) * 6);
    cpssOsMemSet(maskArr, 0, sizeof(GT_U32) * 6);
    /* clear output data */
    *validPtr = GT_FALSE;
    cpssOsMemSet(prefixPtr, 0, sizeof(CPSS_DXCH_IPV6_PREFIX_STC));
    cpssOsMemSet(maskPtr, 0, sizeof(CPSS_DXCH_IPV6_PREFIX_STC));

    retVal = prvCpssDxChRouterTunnelTermTcamGetLine(devNum,
                                                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    routerTtiTcamRow,
                                                    &validArr[0],
                                                    &compareModeArr[0],
                                                    &keyArr[0],
                                                    &maskArr[0],
                                                    &actionArr[0]);
    if (retVal != GT_OK)
        return retVal;

    /* convert from HW format to ip address */
    /****************************************/

    prefixPtr->ipAddr.arIP[15] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 0,8);
    prefixPtr->ipAddr.arIP[14] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 8,8);
    prefixPtr->ipAddr.arIP[13] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 16,8);
    prefixPtr->ipAddr.arIP[12] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 24,8);

    prefixPtr->ipAddr.arIP[11] = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 0,8);
    prefixPtr->ipAddr.arIP[10] = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 8,8);
    prefixPtr->ipAddr.arIP[9]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 16,8);
    prefixPtr->ipAddr.arIP[8]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 24,8);

    prefixPtr->ipAddr.arIP[7]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[2], 0,8);
    prefixPtr->ipAddr.arIP[6]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[2], 8,8);
    prefixPtr->ipAddr.arIP[5]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[2], 16,8);
    prefixPtr->ipAddr.arIP[4]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[2], 24,8);

    prefixPtr->ipAddr.arIP[3]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[3], 0,8);
    prefixPtr->ipAddr.arIP[2]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[3], 8,8);
    prefixPtr->ipAddr.arIP[1]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[3], 16,8);
    prefixPtr->ipAddr.arIP[0]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[3], 24,8);

    /* convert from HW format to mask */
    /**********************************/

    maskPtr->ipAddr.arIP[15]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 0,8);
    maskPtr->ipAddr.arIP[14]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 8,8);
    maskPtr->ipAddr.arIP[13]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 16,8);
    maskPtr->ipAddr.arIP[12]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 24,8);

    maskPtr->ipAddr.arIP[11]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 0,8);
    maskPtr->ipAddr.arIP[10]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 8,8);
    maskPtr->ipAddr.arIP[9]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 16,8);
    maskPtr->ipAddr.arIP[8]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 24,8);

    maskPtr->ipAddr.arIP[7]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[2], 0,8);
    maskPtr->ipAddr.arIP[6]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[2], 8,8);
    maskPtr->ipAddr.arIP[5]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[2], 16,8);
    maskPtr->ipAddr.arIP[4]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[2], 24,8);

    maskPtr->ipAddr.arIP[3]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[3], 0,8);
    maskPtr->ipAddr.arIP[2]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[3], 8,8);
    maskPtr->ipAddr.arIP[1]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[3], 16,8);
    maskPtr->ipAddr.arIP[0]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[3], 24,8);

    /* get the vr Id or MC Group Index */
    /***********************************/

    prefixPtr->isMcSource = maskPtr->isMcSource =
        (((keyArr[4] >> 15) & 0x1) == 1) ? GT_TRUE : GT_FALSE;

    if (prefixPtr->isMcSource == GT_FALSE)
    {
        prefixPtr->vrId = (keyArr[4] & 0xfff);
        maskPtr->vrId   = (maskArr[4] & 0xfff);
    }
    else
    {
        prefixPtr->mcGroupIndexRow = (keyArr[4] & 0x7fff);
        maskPtr->mcGroupIndexRow   = (maskArr[4] & 0x7fff);
    }

    /* line holds valid IPv6 prefix if the following applies:
         - all entries are valid
         - the compare mode or all entries is row compare
         - keyArr[5] bit 31 must be 0 (to indicate IPv6 entry and not TT entry) */
    *validPtr = GT_TRUE;
    for (i = 0 ; i < 4 ; i++)
    {
        /* if entry is not valid or is single compare mode, whole line is not valid */
        if ((validArr[i] == 0) || (compareModeArr[i] == 0))
        {
            *validPtr = GT_FALSE;
            break;
        }
    }
    /* if whole line is valid, verify it is indeed IPv6 entry and not TTI entry */
    if ((*validPtr == GT_TRUE) && (((keyArr[5] >> 31) & 0x1) != 0))
    {
        *validPtr = GT_FALSE;
    }

    return retVal;
}


/*******************************************************************************
* cpssDxChIpv6PrefixGet
*
* DESCRIPTION:
*    Gets an ipv6 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to get from
*
* OUTPUTS:
*       validPtr              - whether the entry is valid (if the entry isn't valid
*                               all the following fields will not get values)
*       prefixPtr             - prefix to set
*       maskPtr               - mask of the prefix
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv6PrefixSet
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpv6PrefixGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    OUT GT_BOOL                     *validPtr,
    OUT CPSS_DXCH_IPV6_PREFIX_STC   *prefixPtr,
    OUT CPSS_DXCH_IPV6_PREFIX_STC   *maskPtr
)
{
    /* make sure the device is Ch2 and above */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH2_E:
        return prvCpssDxCh2Ipv6PrefixGet(devNum,routerTtiTcamRow,
                                         validPtr,prefixPtr,maskPtr);
    default:
        return prvCpssDxCh3Ipv6PrefixGet(devNum,routerTtiTcamRow,
                                         validPtr,prefixPtr,maskPtr);
    }
}


/*******************************************************************************
* cpssDxChIpv6PrefixInvalidate
*
* DESCRIPTION:
*    Invalidates an ipv6 UC or MC prefix in the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to invalidate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note that invalidating ipv6 address (that takes whole TCAM line) will
*       result invalidating all columns in that line.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpv6PrefixInvalidate
(
    IN  GT_U8           devNum,
    IN  GT_U32          routerTtiTcamRow
)
{
    GT_U32  retVal = GT_OK;         /* function return code */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.routerAndTunnelTermTcam)
        return GT_BAD_PARAM;

    retVal = prvCpssDxChRouterTunnelTermTcamInvalidateLine(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,routerTtiTcamRow);

    return retVal;
}


/*******************************************************************************
* cpssDxChIpLttWrite
*
* DESCRIPTION:
*    Writes a LookUp Translation Table Entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum        - the device number.
*       lttTtiRow     - the entry's row index in LTT table (equivalent to
*                       the router tcam entry it is attached to)
*       lttTtiColumn   - the entry's column index in LTT table (equivalent to
*                       the router tcam entry it is attached to)
*       lttEntryPtr   - the lookup translation table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - if succeeded
*       GT_BAD_PARAM    - on devNum not active or
*                         invalid ipv6MCGroupScopeLevel value.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Lookup translation table is shared with the Tunnel Termination (TT)
*       Table.
*       If the relevant index in the Router Tcam is in line used for tunnel
*       termination key then the indexes in the LTT/TT table will function as
*       Tunnel Termination Action, and you shouldn't write LTT entries in these
*       indexes. It works Vice-verse.
*       For Ipv6 Entries the relevant index is the line prefix (same index sent
*       as parameter to the ipv6 prefix set API). The other LTT/TT entries
*       related to that TCAM line are left unused.
*
*       FEr#2018 - if the PCL action uses redirection to Router Lookup Translation Table (LTT)
*                 (policy based routing) or if the TTI action uses redirection to LTT
*                 (TTI base routing), then the LTT index written in the PCL/TTI action is
*                  restricted only to column 0 of the LTT row.
*       NOTE: - Since LTT entries can be used also for Router TCAM routing, this API is not
*               affected directly by this errata. However when the LTT should be pointed from
*               PCL action or LTT action, it should be set at column 0.
*             - The API affected by this errata are PCL rule set, PCL action update,
*               TTI rule set and TTI action update.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLttWrite
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          lttTtiRow,
    IN  GT_U32                          lttTtiColumn,
    IN  CPSS_DXCH_IP_LTT_ENTRY_STC      *lttEntryPtr
)
{
    GT_U32 lltEntryHwFormat = 0;
    GT_U32 lltEntryHwFormatLength=0;
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    /* ltt size is the same as router TCAM size */
    if (lttTtiRow >= fineTuningPtr->tableSize.routerAndTunnelTermTcam)
        return GT_BAD_PARAM;
    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH2_E:
        if (lttTtiColumn >= 5)
            return GT_BAD_PARAM;
        break;
    default:
        if (lttTtiColumn >= 4)
            return GT_BAD_PARAM;
        break;
    }
    CPSS_NULL_PTR_CHECK_MAC(lttEntryPtr);

    /* check ltt parameters */
    switch (lttEntryPtr->routeType)
    {
    case CPSS_DXCH_IP_QOS_ROUTE_ENTRY_GROUP_E:
    case CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E:
        break;
    default:
        return GT_BAD_PARAM;
    }
    if (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        if (lttEntryPtr->numOfPaths > CPSS_DXCH_IP_MAX_ECMP_QOS_GROUP)
            return GT_BAD_PARAM;
    }
    else
    {
        if(lttEntryPtr->routeType == CPSS_DXCH_IP_QOS_ROUTE_ENTRY_GROUP_E)
        {
            if (lttEntryPtr->numOfPaths > CPSS_DXCH_IP_MAX_LION_QOS_GROUP)
                return GT_BAD_PARAM;
        }
        else
        {
            if (lttEntryPtr->numOfPaths > CPSS_DXCH_IP_MAX_LION_ECMP_GROUP)
                return GT_BAD_PARAM;
        }

    }
    if (lttEntryPtr->routeEntryBaseIndex >= fineTuningPtr->tableSize.routerNextHop)
        return GT_BAD_PARAM;

    switch(lttEntryPtr->ipv6MCGroupScopeLevel)
    {
    case CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E:
    case CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E:
    case CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E:
    case CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E:
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* build the 22 bits of the llt entry */
    if(lttEntryPtr->routeType == CPSS_DXCH_IP_QOS_ROUTE_ENTRY_GROUP_E)
        lltEntryHwFormat = 0x1;

    if (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        lltEntryHwFormat |= (lttEntryPtr->numOfPaths & 0x7) << 1;
        lltEntryHwFormat |= (BOOL2BIT_MAC(lttEntryPtr->ucRPFCheckEnable)) << 4;
        lltEntryHwFormat |= (lttEntryPtr->ipv6MCGroupScopeLevel & 0x3) << 5;
        lltEntryHwFormat |= (BOOL2BIT_MAC(lttEntryPtr->sipSaCheckMismatchEnable)) << 7;
        lltEntryHwFormat |= lttEntryPtr->routeEntryBaseIndex << 8;
        lltEntryHwFormatLength   = 22;
    }
    else
    {
        lltEntryHwFormat |= (lttEntryPtr->numOfPaths & 0x3F) << 1;
        lltEntryHwFormat |= (BOOL2BIT_MAC(lttEntryPtr->ucRPFCheckEnable)) << 7;
        lltEntryHwFormat |= (lttEntryPtr->ipv6MCGroupScopeLevel & 0x3) << 8;
        lltEntryHwFormat |= (BOOL2BIT_MAC(lttEntryPtr->sipSaCheckMismatchEnable)) << 10;
        lltEntryHwFormat |= lttEntryPtr->routeEntryBaseIndex << 11;
        lltEntryHwFormatLength   = 25;
    }

    /* now set the 22 bits in the hw according the location of the entry in the line */
    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        /* for Cheeatah2 devices */
    case CPSS_PP_FAMILY_CHEETAH2_E:
        switch(lttTtiColumn)
        {
        case 0:
            retVal = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                     lttTtiRow,0,0,22,lltEntryHwFormat);
            break;

        case 1:
            retVal = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                     lttTtiRow,0,22,10,lltEntryHwFormat);
            if (retVal != GT_OK)
                break;
            retVal = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                     lttTtiRow,1,0,12,lltEntryHwFormat >> 10);
            break;

        case 2:
            retVal = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                     lttTtiRow,1,12,20,lltEntryHwFormat);
            if (retVal != GT_OK)
                break;
            retVal = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                     lttTtiRow,2,0,2,lltEntryHwFormat >> 20);
            break;

        case 3:
            retVal = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                     lttTtiRow,2,2,22,lltEntryHwFormat);
            break;

        case 4:
            retVal = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                     lttTtiRow,2,24,8,lltEntryHwFormat);
            if (retVal != GT_OK)
                break;
            retVal = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                     lttTtiRow,3,0,14,lltEntryHwFormat >> 8);
            break;
        }
        break;

        /* for cheetah3 devices */
    default:
        retVal = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH3_LTT_TT_ACTION_E,
                                                 lttTtiRow,lttTtiColumn,0,
                                                 lltEntryHwFormatLength,
                                                 lltEntryHwFormat);
        break;
    }

    return (retVal);
}


/*******************************************************************************
* cpssDxChIpLttRead
*
* DESCRIPTION:
*    Reads a LookUp Translation Table Entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number.
*       lttTtiRow       - the entry's row index in LTT table (equivalent to
*                         the router tcam entry it is attached to)
*       lttTtiColumn    - the entry's column index in LTT table (equivalent to
*                         the router tcam entry it is attached to)
*
* OUTPUTS:
*       lttEntryPtr     - the lookup translation table entry
*
* RETURNS:
*       GT_OK           - if succeeded
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpLttWrite
*
*******************************************************************************/
GT_STATUS cpssDxChIpLttRead
(
    IN  GT_U8                              devNum,
    IN  GT_U32                             lttTtiRow,
    IN  GT_U32                             lttTtiColumn,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC         *lttEntryPtr
)
{
    GT_U32      data = 0, data2 = 0;
    GT_STATUS   retVal = GT_OK;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    GT_U32      ipv6MCGroupScopeLevelOffset=0;
    GT_U32      lltEntryHwFormatLength=0;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    /* ltt size is the same as router TCAM size */
    if (lttTtiRow >= fineTuningPtr->tableSize.routerAndTunnelTermTcam)
        return GT_BAD_PARAM;
    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH2_E:
        if (lttTtiColumn >= 5)
            return GT_BAD_PARAM;
        break;
    default:
        if (lttTtiColumn >= 4)
            return GT_BAD_PARAM;
        break;
    }
    CPSS_NULL_PTR_CHECK_MAC(lttEntryPtr);

    /* now get the 22 bits in the hw according the location of the entry in the line */
    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        /* for cheetah2 devices */
    case CPSS_PP_FAMILY_CHEETAH2_E:
        switch(lttTtiColumn)
        {
        case 0:
            retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                    lttTtiRow,0,0,22,&data);
            break;

        case 1:
            retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                    lttTtiRow,0,22,10,&data);
            if (retVal != GT_OK)
                break;
            retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                    lttTtiRow,1,0,12,&data2);
            data |= (data2 << 10);
            break;

        case 2:
            retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                    lttTtiRow,1,12,20,&data);
            if (retVal != GT_OK)
                break;
            retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                    lttTtiRow,2,0,2,&data2);
            data |= (data2 << 20);
            break;

        case 3:
            retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                    lttTtiRow,2,2,22,&data);
            break;

        case 4:
            retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                    lttTtiRow,2,24,8,&data);
            if (retVal != GT_OK)
                break;
            retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                    lttTtiRow,3,0,14,&data2);
            data |= (data2 << 8);
            break;
        }
        break;

        /* for cheetah3 devices */
    default:
        if (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
        {
            lltEntryHwFormatLength = 22;
        }
        else
        {
            lltEntryHwFormatLength = 25;
        }

        retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH3_LTT_TT_ACTION_E,
                                                lttTtiRow,lttTtiColumn,0,lltEntryHwFormatLength,&data);
        break;
    }

    if(data & 0x1)
        lttEntryPtr->routeType = CPSS_DXCH_IP_QOS_ROUTE_ENTRY_GROUP_E;
    else
        lttEntryPtr->routeType = CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E;

    if (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        lttEntryPtr->numOfPaths               = U32_GET_FIELD_MAC(data, 1,3);
        lttEntryPtr->ucRPFCheckEnable         = BOOL2BIT_MAC(U32_GET_FIELD_MAC(data, 4,1));
        lttEntryPtr->sipSaCheckMismatchEnable = BOOL2BIT_MAC(U32_GET_FIELD_MAC(data, 7,1));
        lttEntryPtr->routeEntryBaseIndex      = U32_GET_FIELD_MAC(data, 8,13);
        ipv6MCGroupScopeLevelOffset           = 5;
    }
    else
    {
        lttEntryPtr->numOfPaths               = U32_GET_FIELD_MAC(data, 1,6);
        lttEntryPtr->ucRPFCheckEnable         = BOOL2BIT_MAC(U32_GET_FIELD_MAC(data, 7,1));
        lttEntryPtr->sipSaCheckMismatchEnable = BOOL2BIT_MAC(U32_GET_FIELD_MAC(data, 10,1));
        lttEntryPtr->routeEntryBaseIndex      = U32_GET_FIELD_MAC(data,11,13);
        ipv6MCGroupScopeLevelOffset           = 8;

    }

    switch(U32_GET_FIELD_MAC(data,ipv6MCGroupScopeLevelOffset,2))
    {
    case 0:
        lttEntryPtr->ipv6MCGroupScopeLevel = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        break;
    case 1:
        lttEntryPtr->ipv6MCGroupScopeLevel = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        break;
    case 2:
        lttEntryPtr->ipv6MCGroupScopeLevel = CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        break;
    case 3:
        lttEntryPtr->ipv6MCGroupScopeLevel = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
        break;
    default:
        break;
    }

    return (retVal);
}


/*******************************************************************************
* cpssDxChIpUcRouteEntriesWrite
*
* DESCRIPTION:
*    Writes an array of uc route entries to hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                - the device number
*       baseRouteEntryIndex   - the index from which to write the array
*       routeEntriesArray     - the uc route entries array
*       numOfRouteEntries     - the number route entries in the array (= the
*                               number of route entries to write)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In case uRPF ECMP/QOS is globally enabled, then for ECMP/QOS block
*       with x route entries, additional route entry should be included after
*       the block with the uRPF route entry information.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpUcRouteEntriesWrite
(
    IN GT_U8                           devNum,
    IN GT_U32                          baseRouteEntryIndex,
    IN CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *routeEntriesArray,
    IN GT_U32                          numOfRouteEntries
)
{
    GT_STATUS retVal = GT_OK;
    GT_U32 i;
    GT_U32 hwData[4];
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(routeEntriesArray);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (baseRouteEntryIndex >= fineTuningPtr->tableSize.routerNextHop)
        return GT_OUT_OF_RANGE;
    if((baseRouteEntryIndex + numOfRouteEntries) > fineTuningPtr->tableSize.routerNextHop)
        return GT_OUT_OF_RANGE;

    retVal = prvCpssDxChIpUcRouteEntriesCheck(devNum,routeEntriesArray,numOfRouteEntries);
    if(retVal != GT_OK)
        return retVal;

    for (i = 0; i < numOfRouteEntries ; i++)
    {
        /* Convert route entry to HW format */
        /* currently only regular route entry is supported */
        retVal = prvCpssDxChIpConvertUcEntry2HwFormat(&routeEntriesArray[i],hwData);
        if(retVal != GT_OK)
            return retVal;

        /* Write to hw table */
        retVal =
            prvCpssDxChWriteTableEntry(devNum,
                                       PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E,
                                       baseRouteEntryIndex + i,
                                       hwData);
        if(retVal != GT_OK)
            return retVal;
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpUcRouteEntriesRead
*
* DESCRIPTION:
*    Reads an array of uc route entries from the hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                - the device number
*       baseRouteEntryIndex   - the index from which to start reading
*       routeEntriesArray     - the uc route entries array, for each element
*                               set the uc route entry type to determine how
*                               entry data will be interpreted
*       numOfRouteEntries     - the number route entries in the array (= the
*                               number of route entries to read)
*
*
* OUTPUTS:
*       routeEntriesArray     - the uc route entries array read
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on devNum not active.
*       GT_BAD_PTR     - one of the parameters is NULL pointer
*       GT_BAD_STATE   - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpUcRouteEntriesWrite
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpUcRouteEntriesRead
(
    IN    GT_U8                             devNum,
    IN    GT_U32                            baseRouteEntryIndex,
    INOUT CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC   *routeEntriesArray,
    IN    GT_U32                            numOfRouteEntries
)
{
    GT_STATUS retVal = GT_OK;
    GT_U32 i;
    GT_U32 hwData[4];
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(routeEntriesArray);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (baseRouteEntryIndex >= fineTuningPtr->tableSize.routerNextHop)
        return GT_OUT_OF_RANGE;
    if((baseRouteEntryIndex + numOfRouteEntries) > fineTuningPtr->tableSize.routerNextHop)
        return GT_OUT_OF_RANGE;

    /* Convert route entry to HW format */
    for (i = 0; i < numOfRouteEntries ; i++)
    {
        retVal =
            prvCpssDxChReadTableEntry(devNum,
                                      PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E,
                                      baseRouteEntryIndex + i,
                                      hwData);

        if(retVal != GT_OK)
            return retVal;

        retVal =
            prvCpssDxChIpConvertHwFormat2UcEntry(hwData,&routeEntriesArray[i]);

        if(retVal != GT_OK)
            return retVal;

    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChIpMcRouteEntriesWrite
*
* DESCRIPTION:
*    Writes a MC route entry to hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       routeEntryIndex - the Index in the Route entries table
*       routeEntryPtr   - the MC route entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpMcRouteEntriesWrite
(
    IN GT_U8                           devNum,
    IN GT_U32                          routeEntryIndex,
    IN CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC *routeEntryPtr
)
{
    GT_U32 hwData[32];
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(routeEntryPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routeEntryIndex >= fineTuningPtr->tableSize.routerNextHop)
        return GT_OUT_OF_RANGE;

    retVal = prvCpssDxChIpMcRouteEntryCheck(routeEntryPtr);
    if(retVal != GT_OK)
        return retVal;

    /* Convert route entry to HW format */
    retVal = prvCpssDxChIpConvertMcEntry2HwFormat(routeEntryPtr,hwData);
    if(retVal != GT_OK)
        return retVal;

    /* Write to hw table */
    retVal = prvCpssDxChWriteTableEntry(devNum,
                                        PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E,
                                        routeEntryIndex,
                                        hwData);

    return retVal;
}

/*******************************************************************************
* cpssDxChIpMcRouteEntriesRead
*
* DESCRIPTION:
*    Reads a MC route entry from the hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       routeEntryIndex - the route entry index.
*
* OUTPUTS:
*       routeEntryPtr   - the MC route entry read
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_STATE    - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpMcRouteEntriesRead
(
    IN GT_U8                            devNum,
    IN GT_U32                           routeEntryIndex,
    OUT CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC *routeEntryPtr
)
{
    GT_U32 hwData[32];
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(routeEntryPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routeEntryIndex >= fineTuningPtr->tableSize.routerNextHop)
        return GT_OUT_OF_RANGE;

    /* Write to hw table */
    retVal = prvCpssDxChReadTableEntry(devNum,
                                       PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E,
                                       routeEntryIndex,
                                       hwData);
    if(retVal != GT_OK)
        return retVal;

    /* Convert route entry to HW format */
    retVal = prvCpssDxChIpConvertHwFormat2McEntry(routeEntryPtr,hwData);

    return retVal;
}

/*******************************************************************************
* cpssDxChIpRouterNextHopTableAgeBitsEntryWrite
*
* DESCRIPTION:
*    set router next hop table age bits entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                              - the device number
*       routerNextHopTableAgeBitsEntryIndex - the router next hop table age bits
*                                             entry index. each entry is 32 Age
*                                             bits.
*       routerNextHopTableAgeBitsEntry      - a 32 Age Bit map for route entries
*                                             <32*routeEntriesIndexesArrayPtr>..
*                                             <32*routeEntriesIndexesArrayPtr+31>
*                                             bits.
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterNextHopTableAgeBitsEntryWrite
(
    IN  GT_U8   devNum,
    IN  GT_U32  routerNextHopTableAgeBitsEntryIndex,
    IN  GT_U32  routerNextHopTableAgeBitsEntry
)
{
    GT_U32      regAddr;        /* register address */
    GT_STATUS   retVal = GT_OK;
    GT_U32      hwStep;         /* hw index step */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    /* the number of age bits is same as the number of route */
    /* entries, and each age bits entry holds 32 age bits    */
    if (routerNextHopTableAgeBitsEntryIndex >= fineTuningPtr->tableSize.routerNextHop / 32)
        return GT_OUT_OF_RANGE;

    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH2_E:
        hwStep = 0x10;
        break;
    default:
        hwStep = 0x04;
        break;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.ucMcRouteEntriesAgeBitsBase +
              routerNextHopTableAgeBitsEntryIndex * hwStep;

    retVal = prvCpssDrvHwPpWriteRegister(devNum, regAddr, routerNextHopTableAgeBitsEntry);

    return retVal;
}

/*******************************************************************************
* cpssDxChIpRouteEntryActiveBitRead
*
* DESCRIPTION:
*     read router next hop table age bits entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                              - the device number
*       routerNextHopTableAgeBitsEntryIndex - the router next hop table age bits
*                                             entry index. each entry is 32 Age
*                                             bits.
* OUTPUTS:
*       routerNextHopTableAgeBitsEntryPtr   - a 32 Age Bit map for route entries
*                                             <32*routeEntriesIndexesArrayPtr>..
*                                             <32*routeEntriesIndexesArrayPtr+31>
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterNextHopTableAgeBitsEntryRead
(
    IN  GT_U8   devNum,
    IN  GT_U32  routerNextHopTableAgeBitsEntryIndex,
    OUT GT_U32  *routerNextHopTableAgeBitsEntryPtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_STATUS   retVal = GT_OK;
    GT_U32      hwStep;         /* hw index step */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    GT_U32      portGroupId;    /* port group Id */
    GT_U32      tempRouterNextHopTableAgeBitsEntry=0;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(routerNextHopTableAgeBitsEntryPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    /* the number of age bits is same as the number of route */
    /* entries, and each age bits entry holds 32 age bits    */
    if (routerNextHopTableAgeBitsEntryIndex >= fineTuningPtr->tableSize.routerNextHop / 32)
        return GT_OUT_OF_RANGE;

    *routerNextHopTableAgeBitsEntryPtr=0;

    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH2_E:
        hwStep = 0x10;
        break;
    default:
        hwStep = 0x04;
        break;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.ucMcRouteEntriesAgeBitsBase +
              routerNextHopTableAgeBitsEntryIndex * hwStep;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        retVal = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr,
                                                     &tempRouterNextHopTableAgeBitsEntry);

        if (retVal != GT_OK)
            return retVal;

        (*routerNextHopTableAgeBitsEntryPtr) |= tempRouterNextHopTableAgeBitsEntry;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return retVal;
}

/*******************************************************************************
* cpssDxChIpMLLPairWrite
*
* DESCRIPTION:
*    Write a Mc Link List (MLL) pair entry to hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       mllPairWriteForm  - the way to write the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*       mllPairEntryPtr   - the Mc lisk list pair entry
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active or invalid mllPairWriteForm.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       MLL entries are two words long.
*       In Lion and above devices the whole entry is written when last word is
*       set.
*       In PP prior to lion The atomic hw write operation is done on one word.
*       Writing order of MLL parameters could be significant as some parameters
*       in word[1],[3] depend on parameters in word[0],[2].
*       Erroneous handling of mllRPFFailCommand/ nextHopTunnelPointer may
*       result with bad traffic. nextPointer may result with PP infinite loop.
*       cpssDxChIpMLLPairWrite handles the nextPointer parameter in the
*       following way:
*       if (last == 1) first set word[0] or word[2].
*       if (last == 0) first set word[3].
*
*******************************************************************************/
GT_STATUS cpssDxChIpMLLPairWrite
(
    IN GT_U8                                        devNum,
    IN GT_U32                                       mllPairEntryIndex,
    IN CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_FORM_ENT    mllPairWriteForm,
    IN CPSS_DXCH_IP_MLL_PAIR_STC                    *mllPairEntryPtr
)
{
    GT_U32    hwAddr;
    GT_U32    hwData[CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD];
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    GT_BOOL   isWriteOnLastWord = GT_FALSE;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(mllPairEntryPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if(mllPairEntryIndex >= fineTuningPtr->tableSize.mllPairs)
        return GT_BAD_PARAM;

    /*in here we need to add all devices where entry is written on last word*/
    if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        isWriteOnLastWord = GT_TRUE;
    }

    if((mllPairWriteForm == CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_FIRST_MLL_ONLY_E) ||
       (mllPairWriteForm == CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_WHOLE_E))
    {
        switch(mllPairEntryPtr->firstMllNode.mllRPFFailCommand)
        {
            case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            case CPSS_PACKET_CMD_DROP_HARD_E:
            case CPSS_PACKET_CMD_DROP_SOFT_E:
            case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            case CPSS_PACKET_CMD_BRIDGE_E:
                break;
            default:
                return GT_BAD_PARAM;
        }
    }

    if((mllPairWriteForm == CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_SECOND_MLL_NEXT_POINTER_ONLY_E) ||
       (mllPairWriteForm == CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_WHOLE_E))
    {
        switch(mllPairEntryPtr->secondMllNode.mllRPFFailCommand)
        {
            case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            case CPSS_PACKET_CMD_DROP_HARD_E:
            case CPSS_PACKET_CMD_DROP_SOFT_E:
            case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            case CPSS_PACKET_CMD_BRIDGE_E:
                break;
            default:
                return GT_BAD_PARAM;
        }
    }
    retVal = prvCpssDxChIpMllStruct2HwFormat(mllPairWriteForm,mllPairEntryPtr,hwData);
    if(retVal != GT_OK)
        return retVal;

    hwAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.mllTableBase +
             mllPairEntryIndex * 0x10;

    if (isWriteOnLastWord)
    {
        switch(mllPairWriteForm)
        {
        case CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_FIRST_MLL_ONLY_E:
            /*we need to modify only the first mll and we read the last word
              so it will not change*/
            retVal = prvCpssDrvHwPpReadRam(devNum,
                                           hwAddr + 12,
                                           CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                           &hwData[3]);

            retVal = prvCpssDrvHwPpWriteRam(devNum,
                                            hwAddr,
                                            CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/2,
                                            &hwData[0]);

            retVal = prvCpssDrvHwPpWriteRam(devNum,
                                            hwAddr + 12,
                                            CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                            &hwData[3]);
            break;

        case CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_SECOND_MLL_NEXT_POINTER_ONLY_E:
            /*we are writing both words and overwriting the last word*/
            retVal = prvCpssDrvHwPpWriteRam(devNum,
                                            hwAddr + 8,
                                            CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/2,
                                            &hwData[2]);
            break;

        case CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_WHOLE_E:
            retVal = prvCpssDrvHwPpWriteRam(devNum,
                                            hwAddr,
                                            CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD,
                                            &hwData[0]);
            break;
        default:
            return GT_BAD_PARAM;
        }
    }
    else
    {
        switch(mllPairWriteForm)
        {
        case CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_FIRST_MLL_ONLY_E:
            if(mllPairEntryPtr->firstMllNode.last)
            {
                retVal = prvCpssDrvHwPpWriteRam(devNum,
                                                hwAddr,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/2,
                                                &hwData[0]);
            }
            else
            {
                retVal = prvCpssDrvHwPpWriteRam(devNum,
                                                hwAddr + 4,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                                &hwData[1]);
                retVal = prvCpssDrvHwPpWriteRam(devNum,
                                                hwAddr,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                                &hwData[0]);
            }
            break;

        case CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_SECOND_MLL_NEXT_POINTER_ONLY_E:
            if(mllPairEntryPtr->secondMllNode.last)
            {
                retVal = prvCpssDrvHwPpWriteRam(devNum,
                                                hwAddr + 8,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/2,
                                                &hwData[2]);
            }
            else
            {
                retVal = prvCpssDrvHwPpWriteRam(devNum,
                                                hwAddr + 12,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                                &hwData[3]);
                retVal = prvCpssDrvHwPpWriteRam(devNum,
                                                hwAddr + 8,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                                &hwData[2]);
            }
            break;

        case CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_WHOLE_E:
            if(mllPairEntryPtr->firstMllNode.last)
            {
                retVal = prvCpssDrvHwPpWriteRam(devNum,
                                                hwAddr,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD,
                                                &hwData[0]);
            }
            else if(mllPairEntryPtr->secondMllNode.last)
            {
                retVal = prvCpssDrvHwPpWriteRam(devNum,
                                                hwAddr + 8,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/2,
                                                &hwData[2]);
                retVal = prvCpssDrvHwPpWriteRam(devNum,
                                                hwAddr,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/2,
                                                &hwData[0]);
            }
            else
            {
                retVal = prvCpssDrvHwPpWriteRam(devNum,
                                                hwAddr + 12,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                                &hwData[3]);
                retVal = prvCpssDrvHwPpWriteRam(devNum,
                                                hwAddr + 8,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                                &hwData[2]);
                retVal = prvCpssDrvHwPpWriteRam(devNum,
                                                hwAddr + 4,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                                &hwData[1]);
                retVal = prvCpssDrvHwPpWriteRam(devNum,
                                                hwAddr,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                                &hwData[0]);
            }
            break;
        default:
            return GT_BAD_PARAM;
        }
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpMLLPairRead
*
* DESCRIPTION:
*    Read a Mc Link List (MLL) pair entry from the hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       mllPairReadForm   - the way to read the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*
*
* OUTPUTS:
*       mllPairEntryPtr   - the Mc lisk list pair entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid parameter.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpMLLPairRead
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      mllPairEntryIndex,
    IN CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_FORM_ENT    mllPairReadForm,
    OUT CPSS_DXCH_IP_MLL_PAIR_STC                   *mllPairEntryPtr
)
{
    GT_U32    hwAddr;
    GT_U32    hwData[CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD];
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(mllPairEntryPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if(mllPairEntryIndex >= fineTuningPtr->tableSize.mllPairs)
        return GT_BAD_PARAM;

    hwAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.mllTableBase +
             mllPairEntryIndex * 0x10;

    retVal = prvCpssDrvHwPpReadRam(devNum, hwAddr,
                                    CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD, hwData);
    if(retVal != GT_OK)
        return retVal;

    /* read generic data */
    switch(mllPairReadForm)
    {
        case CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_FIRST_MLL_ONLY_E:
            retVal = prvCpssDxChIpHwFormat2MllStruct(&hwData[0], &mllPairEntryPtr->firstMllNode);
            if(retVal != GT_OK)
                return retVal;
            break;

        case CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_SECOND_MLL_NEXT_POINTER_ONLY_E:
            retVal = prvCpssDxChIpHwFormat2MllStruct(&hwData[2], &mllPairEntryPtr->secondMllNode);
            if(retVal != GT_OK)
                return retVal;
            break;

        case CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_WHOLE_E:
            retVal = prvCpssDxChIpHwFormat2MllStruct(&hwData[0], &mllPairEntryPtr->firstMllNode);
            if(retVal != GT_OK)
                return retVal;

            retVal = prvCpssDxChIpHwFormat2MllStruct(&hwData[2], &mllPairEntryPtr->secondMllNode);
            if(retVal != GT_OK)
                return retVal;
            break;

        default:
            return GT_BAD_PARAM;
    }

    if ((mllPairReadForm == CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_SECOND_MLL_NEXT_POINTER_ONLY_E) ||
        (mllPairReadForm == CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_WHOLE_E))
    {
        /* Next MLL Pointer */
        mllPairEntryPtr->nextPointer = (GT_U16)U32_GET_FIELD_MAC(hwData[3], 20, 12);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpMLLLastBitWrite
*
* DESCRIPTION:
*    Write a Mc Link List (MLL) Last bit to the hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       lastBit           - the Last bit
*       mllEntryPart      - to which part of the mll Pair to write the Last bit
*                           for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active or invalid mllEntryPart.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpMLLLastBitWrite
(
    IN GT_U8                                            devNum,
    IN GT_U32                                           mllPairEntryIndex,
    IN GT_BOOL                                          lastBit,
    IN CPSS_DXCH_IP_MLL_PAIR_PART_LAST_BIT_WRITE_ENT    mllEntryPart
)
{
    GT_U32      hwAddr;
    GT_U32      hwData[CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD];
    GT_STATUS   retVal = GT_OK;
    GT_U32      hwWord;
    GT_U32      offsetInWord;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if(mllPairEntryIndex >= fineTuningPtr->tableSize.mllPairs)
        return GT_BAD_PARAM;

    hwAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.mllTableBase +
             mllPairEntryIndex * 0x10;

    /* here we need to add all devices where entry write is triggered upon writing last word */
    if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        /* read the table entry from hardware */
        retVal = prvCpssDrvHwPpReadRam(devNum,hwAddr,CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD,hwData);
        if (retVal != GT_OK)
            return retVal;

        switch(mllEntryPart)
        {
        case CPSS_DXCH_IP_MLL_PAIR_WRITE_FIRST_LAST_BIT_E:
            hwWord = 0;
            offsetInWord = 0;
            break;
        case CPSS_DXCH_IP_MLL_PAIR_WRITE_SECOND_LAST_BIT_E:
            hwWord = 2;
            offsetInWord = 0;
            break;
        default:
            return GT_BAD_PARAM;
        }

        hwData[hwWord] &= (~(1 << offsetInWord));
        hwData[hwWord] |= (BOOL2BIT_MAC(lastBit) << offsetInWord);

        /* write the updated entry to the hardware */
        retVal = prvCpssDrvHwPpWriteRam(devNum,hwAddr,CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD,hwData);
        if (retVal != GT_OK)
            return retVal;
    }
    else
    {
        switch(mllEntryPart)
        {
        case CPSS_DXCH_IP_MLL_PAIR_WRITE_FIRST_LAST_BIT_E:
            retVal = prvCpssDrvHwPpSetRegField(devNum,hwAddr,0,1,BOOL2BIT_MAC(lastBit));
            break;
        case CPSS_DXCH_IP_MLL_PAIR_WRITE_SECOND_LAST_BIT_E:
            retVal = prvCpssDrvHwPpSetRegField(devNum,hwAddr+8,0,1,BOOL2BIT_MAC(lastBit));
            break;
        default:
            return GT_BAD_PARAM;
        }
    }

    return retVal;
}


/*******************************************************************************
* cpssDxChIpRouterArpAddrWrite
*
* DESCRIPTION:
*    write a ARP MAC address to the router ARP / Tunnel start Table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - the device number
*       routerArpIndex  - The Arp Address index (to be inserted later
*                         in the UC Route entry Arp nextHopARPPointer
*                         field)
*       arpMacAddrPtr   - the ARP MAC address to write
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on devNum not active.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       pay attention that the router ARP table is shard with a tunnel start
*       table, each tunnel start entry takes 4 ARP Address.
*       that is Tunnel start <n> takes ARP addresses <4n>,<4n+1>,<4n+2>,
*       <4n+3>
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterArpAddrWrite
(
    IN GT_U8                         devNum,
    IN GT_U32                        routerArpIndex,
    IN GT_ETHERADDR                  *arpMacAddrPtr
)
{
    GT_U32  hwData[2];
    GT_U32  entryIdx,wordIdx;
    GT_U32  retVal = GT_OK;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(arpMacAddrPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerArpIndex >= fineTuningPtr->tableSize.routerArp)
        return GT_OUT_OF_RANGE;

    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH_E:

        hwData[0] =  arpMacAddrPtr->arEther[5] |
                     (arpMacAddrPtr->arEther[4] << 8) |
                     (arpMacAddrPtr->arEther[3] << 16)|
                     (arpMacAddrPtr->arEther[2] << 24);
        hwData[1] =  arpMacAddrPtr->arEther[1] |
                     (arpMacAddrPtr->arEther[0] << 8);

        retVal = prvCpssDxChWriteTableEntry(devNum,
                                            PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E,
                                            routerArpIndex,hwData);

        break;

    default:
        /* convert the ARP MAC address to hw format */
        /* and write to hw table                    */

        entryIdx = (routerArpIndex / 4);
        wordIdx = ((routerArpIndex % 4) * 6) /4;

        if ((routerArpIndex % 2) == 0)
        {
            hwData[0] =  arpMacAddrPtr->arEther[5] |
                         (arpMacAddrPtr->arEther[4] << 8) |
                         (arpMacAddrPtr->arEther[3] << 16)|
                         (arpMacAddrPtr->arEther[2] << 24);
            hwData[1] =  arpMacAddrPtr->arEther[1] |
                         (arpMacAddrPtr->arEther[0] << 8); /* only 16 Msbs are relevant */

            retVal = prvCpssDxChWriteTableEntryField(devNum,
                                                     PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
                                                     entryIdx,
                                                     wordIdx,
                                                     0,32,
                                                     hwData[0]);
            if (retVal != GT_OK)
            {
                return retVal;
            }

            retVal = prvCpssDxChWriteTableEntryField(devNum,
                                                     PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
                                                     entryIdx,
                                                     wordIdx + 1,
                                                     0,16,
                                                     hwData[1]);
        }
        else
        {
            hwData[0] =  arpMacAddrPtr->arEther[5] |
                         (arpMacAddrPtr->arEther[4] << 8); /* only 16 Lsbs are relevant */
            hwData[1] =  arpMacAddrPtr->arEther[3] |
                         (arpMacAddrPtr->arEther[2] << 8) |
                         (arpMacAddrPtr->arEther[1] << 16)|
                         (arpMacAddrPtr->arEther[0] << 24);


            retVal = prvCpssDxChWriteTableEntryField(devNum,
                                                     PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
                                                     entryIdx,
                                                     wordIdx,
                                                     16,16,
                                                     hwData[0]);
            if (retVal != GT_OK)
            {
                return retVal;
            }

            retVal = prvCpssDxChWriteTableEntryField(devNum,
                                                     PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
                                                     entryIdx,
                                                     wordIdx + 1,
                                                     0,32,
                                                     hwData[1]);
        }

        break;
    }

    return retVal;
}


/*******************************************************************************
* cpssDxChIpRouterArpAddrRead
*
* DESCRIPTION:
*    read a ARP MAC address from the router ARP / Tunnel start Table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - the device number
*       routerArpIndex  - The Arp Address index (to be inserted later in the
*                         UC Route entry Arp nextHopARPPointer field)
*
*
* OUTPUTS:
*       arpMacAddrPtr   - the ARP MAC address
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on devNum not active.
*       GT_OUT_OF_RANGE          - on routerArpIndex bigger then 4095 or
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       see cpssDxChIpRouterArpAddrWrite
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterArpAddrRead
(
    IN  GT_U8                        devNum,
    IN  GT_U32                       routerArpIndex,
    OUT GT_ETHERADDR                 *arpMacAddrPtr
)
{
    GT_U32  hwData[2];
    GT_U32  entryIdx,wordIdx;
    GT_U32  retVal = GT_OK;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(arpMacAddrPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerArpIndex >= fineTuningPtr->tableSize.routerArp)
        return GT_OUT_OF_RANGE;

    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH_E:

        retVal = prvCpssDxChReadTableEntry(devNum,
                                           PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E,
                                           routerArpIndex,hwData);

        arpMacAddrPtr->arEther[5] = (GT_U8)U32_GET_FIELD_MAC(hwData[0],0,8);
        arpMacAddrPtr->arEther[4] = (GT_U8)U32_GET_FIELD_MAC(hwData[0],8,8);
        arpMacAddrPtr->arEther[3] = (GT_U8)U32_GET_FIELD_MAC(hwData[0],16,8);
        arpMacAddrPtr->arEther[2] = (GT_U8)U32_GET_FIELD_MAC(hwData[0],24,8);

        arpMacAddrPtr->arEther[1] = (GT_U8)U32_GET_FIELD_MAC(hwData[1],0,8);
        arpMacAddrPtr->arEther[0] = (GT_U8)U32_GET_FIELD_MAC(hwData[1],8,8);

        break;

    default:
        /* Read from hw table and               */
        /* convert hw format to ARP MAC address */

        entryIdx = (routerArpIndex / 4);
        wordIdx = ((routerArpIndex % 4) * 6) /4;

        if ((routerArpIndex % 2) == 0)
        {
            retVal = prvCpssDxChReadTableEntryField(devNum,
                                                    PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
                                                    entryIdx,
                                                    wordIdx,
                                                    0,32,
                                                    &hwData[0]);
            if (retVal != GT_OK)
            {
                return retVal;
            }

            retVal = prvCpssDxChReadTableEntryField(devNum,
                                                    PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
                                                    entryIdx,
                                                    wordIdx + 1,
                                                    0,16,
                                                    &hwData[1]);
            if (retVal != GT_OK)
            {
                return retVal;
            }

            arpMacAddrPtr->arEther[5] = (GT_U8)U32_GET_FIELD_MAC(hwData[0],0,8);
            arpMacAddrPtr->arEther[4] = (GT_U8)U32_GET_FIELD_MAC(hwData[0],8,8);
            arpMacAddrPtr->arEther[3] = (GT_U8)U32_GET_FIELD_MAC(hwData[0],16,8);
            arpMacAddrPtr->arEther[2] = (GT_U8)U32_GET_FIELD_MAC(hwData[0],24,8);

            /* only 16 Msbs are relevant */
            arpMacAddrPtr->arEther[1] = (GT_U8)U32_GET_FIELD_MAC(hwData[1],0,8);
            arpMacAddrPtr->arEther[0] = (GT_U8)U32_GET_FIELD_MAC(hwData[1],8,8);
        }
        else
        {
            retVal = prvCpssDxChReadTableEntryField(devNum,
                                                    PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
                                                    entryIdx,
                                                    wordIdx,
                                                    16,16,
                                                    &hwData[0]);
            if (retVal != GT_OK)
            {
                return retVal;
            }

            retVal = prvCpssDxChReadTableEntryField(devNum,
                                                    PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
                                                    entryIdx,
                                                    wordIdx + 1,
                                                    0,32,
                                                    &hwData[1]);
            if (retVal != GT_OK)
            {
                return retVal;
            }

            /* only 16 Lsbs are relevant */
            arpMacAddrPtr->arEther[5] = (GT_U8)U32_GET_FIELD_MAC(hwData[0],0,8);
            arpMacAddrPtr->arEther[4] = (GT_U8)U32_GET_FIELD_MAC(hwData[0],8,8);

            arpMacAddrPtr->arEther[3] = (GT_U8)U32_GET_FIELD_MAC(hwData[1],0,8);
            arpMacAddrPtr->arEther[2] = (GT_U8)U32_GET_FIELD_MAC(hwData[1],8,8);
            arpMacAddrPtr->arEther[1] = (GT_U8)U32_GET_FIELD_MAC(hwData[1],16,8);
            arpMacAddrPtr->arEther[0] = (GT_U8)U32_GET_FIELD_MAC(hwData[1],24,8);
        }

        break;
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChIpConvertUcEntry2HwFormat
*
* DESCRIPTION:
*       This function converts a given ip uc entry to the HW format.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       ipUcRouteEntryPtr - The entry to be converted.
*
* OUTPUTS:
*       hwDataPtr  - The entry in the HW format representation.
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpConvertUcEntry2HwFormat
(
    IN  CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *ipUcRouteEntryPtr,
    OUT GT_U32 *hwDataPtr
)
{
    GT_U8         packetCmd;        /* packet command */
    GT_U8         cpuCodeIdx;       /* cpu code index */
    GT_U8         modifyDscp;       /* modify packet Dscp command */
    GT_U8         modifyUp;         /* modify packet user priority field */
    GT_U8         targDev;          /* target device */
    GT_U8         targPort;         /* target port   */
    GT_U8         counterSet;

    CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC  *routeEntry = NULL;

    /* clear hw data */
    cpssOsMemSet(hwDataPtr, 0, sizeof(GT_U32) * 4);

    switch (ipUcRouteEntryPtr->type)
    {
    case CPSS_DXCH_IP_UC_ROUTE_ENTRY_E:

        routeEntry = &ipUcRouteEntryPtr->entry.regularEntry;   /* to make the code more readable */

    switch(routeEntry->cmd)
    {
        case CPSS_PACKET_CMD_ROUTE_E:
            packetCmd= 0;
            break;
        case CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E:
            packetCmd= 1;
            break;
        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            packetCmd= 2;
            break;
        case CPSS_PACKET_CMD_DROP_HARD_E:
            packetCmd= 3;
            break;
        case CPSS_PACKET_CMD_DROP_SOFT_E:
            packetCmd= 4;
            break;
        default:
            return GT_BAD_PARAM;
    }

    switch(routeEntry->cpuCodeIdx)
    {
        case CPSS_DXCH_IP_CPU_CODE_IDX_0_E:
            cpuCodeIdx= 0;
            break;
        case CPSS_DXCH_IP_CPU_CODE_IDX_1_E:
            cpuCodeIdx= 1;
            break;
        case CPSS_DXCH_IP_CPU_CODE_IDX_2_E:
            cpuCodeIdx= 2;
            break;
        case CPSS_DXCH_IP_CPU_CODE_IDX_3_E:
            cpuCodeIdx= 3;
            break;
        default:
            return GT_BAD_PARAM;
    }

    switch(routeEntry->modifyDscp)
    {
        case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            modifyDscp= 0;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            modifyDscp= 2;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
            modifyDscp= 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    switch(routeEntry->modifyUp)
    {
        case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            modifyUp= 0;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            modifyUp= 2;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
            modifyUp= 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    hwDataPtr[0] = ((packetCmd & 0x7)                                              |
                 ((routeEntry->ttlHopLimDecOptionsExtChkByPass & 0x1) << 3) |
                 ((cpuCodeIdx & 0x3) << 4)                                      |
                 ((routeEntry->ingressMirror & 0x1) << 6)                   |
                 ((routeEntry->qosPrecedence & 0x1) << 7)                   |
                 ((routeEntry->qosProfileMarkingEnable & 0x1) << 8)         |
                 ((routeEntry->qosProfileIndex & 0x7F) << 9)                |
                 ((modifyDscp & 0x3) << 16)                                     |
                 ((modifyUp & 0x3) << 18)                                       |
                 ((routeEntry->nextHopVlanId & 0xFFF) << 20));

    /* hwDataPtr[1] = 0; */

    if ((routeEntry->cmd == CPSS_PACKET_CMD_ROUTE_E) ||
        (routeEntry->cmd == CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E))
    {
        /* outlif */
        if (routeEntry->nextHopInterface.type == CPSS_INTERFACE_VIDX_E)
        {
            hwDataPtr[1] |= 0x1; /* use VIDX */
            hwDataPtr[1] |= (routeEntry->nextHopInterface.vidx & 0xFFF) << 1;
        }
        else if (routeEntry->nextHopInterface.type == CPSS_INTERFACE_TRUNK_E)
        {
            /* TRUNK */
            hwDataPtr[1] |= (1 << 1) | /* target is trunk */
                ((routeEntry->nextHopInterface.trunkId & 0x7F) << 6);
        }
        else
        {
            /* PORT */
            targDev  = routeEntry->nextHopInterface.devPort.devNum;
            targPort = routeEntry->nextHopInterface.devPort.portNum;
            hwDataPtr[1] |= ((targPort & 0x3F) << 2) | ((targDev & 0x1F) << 8);
        }

        if (routeEntry->isTunnelStart == GT_FALSE)  /* Link Layer */
        {
            /* arp mac Da Pointer */
            hwDataPtr[1] |= ((routeEntry->nextHopARPPointer & 0x1FFF) << 19);
        }
        else /* tunnel start */
        {
            hwDataPtr[1] |=
                (((routeEntry->isTunnelStart & 0x1) << 17) |

                 (0x1 << 31) |
                 ((routeEntry->nextHopTunnelPointer & 0xFFF) << 19));
        }
    }

    hwDataPtr[1] |= ((routeEntry->ttlHopLimitDecEnable & 0x1) << 18);


    counterSet = (GT_U8)((routeEntry->countSet == CPSS_IP_CNT_NO_SET_E) ?
                                        7 : routeEntry->countSet);
    hwDataPtr[2] = ((routeEntry->siteId & 0x1)                               |
                 ((routeEntry->scopeCheckingEnable & 0x1) << 1)           |
                 ((routeEntry->ICMPRedirectEnable & 0x1) << 2)            |
                 ((counterSet & 0x7) << 3)                       |
                 ((routeEntry->mtuProfileIndex & 0x7) << 6)               |
                 ((routeEntry->trapMirrorArpBcEnable & 0x1) << 9)         |
                 ((routeEntry->appSpecificCpuCodeEnable & 0x1) << 10)     |
                 ((routeEntry->dipAccessLevel & 0x7) << 11)               |
                 ((routeEntry->sipAccessLevel & 0x7) << 14)               |
                 ((routeEntry->unicastPacketSipFilterEnable & 0x1) << 17) |
                 (((routeEntry->nextHopARPPointer >> 13) & 0x1) << 19));


    /* hwDataPtr[3] = 0; */

        break;

    case CPSS_DXCH_IP_UC_ECMP_RPF_E:

        hwDataPtr[0] = (ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[0]) |
                    (ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[1] << 12) |
                    ((ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[2] & 0xff) << 24);

        hwDataPtr[1] = ((ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[2] & 0xf00) >> 8) |
                    (ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[3] << 4) |
                    (ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[4] << 16) |
                    ((ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[5] & 0xf) << 28);

        hwDataPtr[2] = ((ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[5] & 0xff0) >> 4) |
                    (ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[6] << 8) |
                    (ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[7] << 20);

        /* hwDataPtr[3] = 0; */

        break;

    default:
        return GT_FAIL;
    }


    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChIpConvertHwFormat2UcEntry
*
* DESCRIPTION:
*       This function converts a given ip uc entry to the HW format.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       hwDataPtr          - The entry in the HW format representation.
*       ipUcRouteEntryPtr  - Set the route entry type to determine how hw data will be
*                         interpreted
*
* OUTPUTS:
*       ipUcRouteEntryPtr - The entry to be converted.
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpConvertHwFormat2UcEntry
(
    IN  GT_U32 *hwDataPtr,
    INOUT CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *ipUcRouteEntryPtr
)
{
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC     *regularEntry = NULL;

    /* clear output data */
    cpssOsMemSet(&ipUcRouteEntryPtr->entry, 0, sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_UNT));

    switch (ipUcRouteEntryPtr->type)
    {
    case CPSS_DXCH_IP_UC_ROUTE_ENTRY_E:

        regularEntry = &ipUcRouteEntryPtr->entry.regularEntry; /* to make the code more readable... */

    switch(U32_GET_FIELD_MAC(hwDataPtr[0],0,3))
    {
        case 0:
            regularEntry->cmd = CPSS_PACKET_CMD_ROUTE_E;
            break;
        case 1:
            regularEntry->cmd = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;
            break;
        case 2:
            regularEntry->cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            break;
        case 3:
            regularEntry->cmd = CPSS_PACKET_CMD_DROP_HARD_E;
            break;
        case 4:
            regularEntry->cmd = CPSS_PACKET_CMD_DROP_SOFT_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    switch(U32_GET_FIELD_MAC(hwDataPtr[0],4,2))
    {
        case 0:
            regularEntry->cpuCodeIdx = CPSS_DXCH_IP_CPU_CODE_IDX_0_E;
            break;
        case 1:
            regularEntry->cpuCodeIdx = CPSS_DXCH_IP_CPU_CODE_IDX_1_E;
            break;
        case 2:
            regularEntry->cpuCodeIdx = CPSS_DXCH_IP_CPU_CODE_IDX_2_E;
            break;
        case 3:
            regularEntry->cpuCodeIdx = CPSS_DXCH_IP_CPU_CODE_IDX_3_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    switch(U32_GET_FIELD_MAC(hwDataPtr[0],16,2))
    {
        case 0:
            regularEntry->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
            break;
        case 1:
            regularEntry->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            break;
        case 2:
            regularEntry->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            break;
        case 3:
        default:
            return GT_BAD_STATE;
    }

    switch(U32_GET_FIELD_MAC(hwDataPtr[0],18,2))
    {
        case 0:
            regularEntry->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
            break;
        case 1:
            regularEntry->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            break;
        case 2:
            regularEntry->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            break;
        case 3:
        default:
            return GT_BAD_STATE;
    }

    regularEntry->ttlHopLimDecOptionsExtChkByPass = U32_GET_FIELD_MAC(hwDataPtr[0],3,1);
    regularEntry->ingressMirror                   = U32_GET_FIELD_MAC(hwDataPtr[0],6,1);
    regularEntry->qosPrecedence                   = U32_GET_FIELD_MAC(hwDataPtr[0],7,1);
    regularEntry->qosProfileMarkingEnable         = U32_GET_FIELD_MAC(hwDataPtr[0],8,1);
    regularEntry->qosProfileIndex                 = U32_GET_FIELD_MAC(hwDataPtr[0],9,7);
    regularEntry->nextHopVlanId           = (GT_U16)U32_GET_FIELD_MAC(hwDataPtr[0],20,12);

    if ((regularEntry->cmd == CPSS_PACKET_CMD_ROUTE_E  ) ||
        (regularEntry->cmd == CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E ))
    {
        if (U32_GET_FIELD_MAC(hwDataPtr[1],0,1) == 1)
        {
            regularEntry->nextHopInterface.type    = CPSS_INTERFACE_VIDX_E;
            regularEntry->nextHopInterface.vidx    =
                (GT_U16)U32_GET_FIELD_MAC(hwDataPtr[1],1,13);
        }
        else if(U32_GET_FIELD_MAC(hwDataPtr[1],1,1) == 1)
        {
            regularEntry->nextHopInterface.type    = CPSS_INTERFACE_TRUNK_E;
            regularEntry->nextHopInterface.trunkId =
                (GT_U16)U32_GET_FIELD_MAC(hwDataPtr[1],6,7);
        }
        else
        {
            regularEntry->nextHopInterface.type    = CPSS_INTERFACE_PORT_E;
            regularEntry->nextHopInterface.devPort.devNum =
                (GT_U8)U32_GET_FIELD_MAC(hwDataPtr[1],8,5);
            regularEntry->nextHopInterface.devPort.portNum =
                (GT_U8)U32_GET_FIELD_MAC(hwDataPtr[1],2,6);
        }

        if(U32_GET_FIELD_MAC(hwDataPtr[1],17,1) == 0)
        {
            regularEntry->isTunnelStart = GT_FALSE;
                regularEntry->nextHopARPPointer =
                    U32_GET_FIELD_MAC(hwDataPtr[1],19,13) | (U32_GET_FIELD_MAC(hwDataPtr[2],19,1) << 13);
        }
        else
        {
            regularEntry->isTunnelStart = GT_TRUE;
            regularEntry->nextHopTunnelPointer = U32_GET_FIELD_MAC(hwDataPtr[1],19,12);
        }
    }

    regularEntry->ttlHopLimitDecEnable = U32_GET_FIELD_MAC(hwDataPtr[1],18,1);

    regularEntry->siteId                       = U32_GET_FIELD_MAC(hwDataPtr[2],0,1);
    regularEntry->scopeCheckingEnable          = U32_GET_FIELD_MAC(hwDataPtr[2],1,1);
    regularEntry->ICMPRedirectEnable           = U32_GET_FIELD_MAC(hwDataPtr[2],2,1);
    regularEntry->countSet                     = U32_GET_FIELD_MAC(hwDataPtr[2],3,3);
    if (regularEntry->countSet == 7)
        regularEntry->countSet = CPSS_IP_CNT_NO_SET_E;

    regularEntry->mtuProfileIndex              = U32_GET_FIELD_MAC(hwDataPtr[2],6,3);
    regularEntry->trapMirrorArpBcEnable        = U32_GET_FIELD_MAC(hwDataPtr[2],9,1);
    regularEntry->appSpecificCpuCodeEnable     = U32_GET_FIELD_MAC(hwDataPtr[2],10,1);
    regularEntry->dipAccessLevel               = U32_GET_FIELD_MAC(hwDataPtr[2],11,3);
    regularEntry->sipAccessLevel               = U32_GET_FIELD_MAC(hwDataPtr[2],14,3);
    regularEntry->unicastPacketSipFilterEnable = U32_GET_FIELD_MAC(hwDataPtr[2],17,1);
        break;

    case CPSS_DXCH_IP_UC_ECMP_RPF_E:

        ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[0] =
            (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[0],0,12));
        ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[1] =
            (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[0],12,12));
        ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[2] =
            (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[0],24,8) | (U32_GET_FIELD_MAC(hwDataPtr[1],0,4) << 8));
        ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[3] =
            (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[1],4,12));
        ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[4] =
            (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[1],16,12));
        ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[5] =
            (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[1],28,4) | (U32_GET_FIELD_MAC(hwDataPtr[2],0,8) << 4));
        ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[6] =
            (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[2],8,12));
        ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[7] =
            (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[2],20,12));

        break;

    default:
        return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChIpConvertMcEntry2HwFormat
*
* DESCRIPTION:
*       This function converts a given ip mc entry to the HW format.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       ipMcRouteEntryPtr - The entry to be converted.
*
* OUTPUTS:
*       hwDataPtr  - The entry in the HW format representation.
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpConvertMcEntry2HwFormat
(
    IN  CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC *ipMcRouteEntryPtr,
    OUT GT_U32 *hwDataPtr
)
{
    GT_U8         packetCmd;        /* packet command   */
    GT_U8         cpuCodeIdx;       /* cpu code index   */
    GT_U8         rpfFailCmd = 0;       /* RPF Fail command */
    GT_U8         counterSet;       /* counter Set */
    GT_U8         modifyDscp;       /* modify DSCP*/
    GT_U8         modifyUp;         /* modify UP */

    switch(ipMcRouteEntryPtr->cmd)
    {
        case CPSS_PACKET_CMD_ROUTE_E:
            packetCmd = 0;
            break;
        case CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E:
            packetCmd = 1;
            break;
        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            packetCmd = 2;
            break;
        case CPSS_PACKET_CMD_DROP_HARD_E:
            packetCmd = 3;
            break;
        case CPSS_PACKET_CMD_DROP_SOFT_E:
            packetCmd = 4;
            break;
        case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            packetCmd = 5;
            break;
        case CPSS_PACKET_CMD_BRIDGE_E:
            packetCmd = 6;
            break;
        default:
            return GT_BAD_PARAM;
    }

    switch(ipMcRouteEntryPtr->cpuCodeIdx)
    {
         case CPSS_DXCH_IP_CPU_CODE_IDX_0_E:
             cpuCodeIdx = 0;
             break;
         case CPSS_DXCH_IP_CPU_CODE_IDX_1_E:
             cpuCodeIdx = 1;
             break;
         case CPSS_DXCH_IP_CPU_CODE_IDX_2_E:
             cpuCodeIdx = 2;
             break;
         case CPSS_DXCH_IP_CPU_CODE_IDX_3_E:
             cpuCodeIdx = 3;
             break;
        default:
            return GT_BAD_PARAM;
    }

    switch(ipMcRouteEntryPtr->modifyDscp)
    {
        case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            modifyDscp = 0;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            modifyDscp = 2;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
            modifyDscp = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    switch(ipMcRouteEntryPtr->modifyUp)
    {
        case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            modifyUp = 0;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            modifyUp = 2;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
            modifyUp = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    hwDataPtr[0] = ((packetCmd & 0x7)                                            |
                ((ipMcRouteEntryPtr->ttlHopLimDecOptionsExtChkByPass & 0x1) << 3)|
                ((cpuCodeIdx & 0x3) << 4)                                     |
                ((ipMcRouteEntryPtr->ingressMirror & 0x1) << 6)                  |
                ((ipMcRouteEntryPtr->qosPrecedence & 0x1) << 7)                  |
                ((ipMcRouteEntryPtr->qosProfileMarkingEnable & 0x1) << 8)        |
                ((ipMcRouteEntryPtr->qosProfileIndex & 0x7F) << 9)               |
                ((modifyDscp & 0x3) << 16)                    |
                ((modifyUp & 0x3) << 18)                      |
                ((ipMcRouteEntryPtr->multicastRPFVlan & 0xFFF) << 20));

    if(ipMcRouteEntryPtr->multicastRPFCheckEnable == GT_TRUE)
    {
        switch(ipMcRouteEntryPtr->RPFFailCommand)
        {
        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            rpfFailCmd = 2;
            break;
        case CPSS_PACKET_CMD_DROP_HARD_E:
            rpfFailCmd = 3;
            break;
        case CPSS_PACKET_CMD_DROP_SOFT_E:
            rpfFailCmd = 4;
            break;
        case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            rpfFailCmd = 5;
            break;
        case CPSS_PACKET_CMD_BRIDGE_E:
            rpfFailCmd = 6;
            break;
        default:
            return GT_BAD_PARAM;
        }
    }
    hwDataPtr[1] = ((ipMcRouteEntryPtr->externalMLLPointer & 0x1FFF)                |
                ((rpfFailCmd & 0x7) << 14)                                    |
                ((ipMcRouteEntryPtr->multicastRPFFailCommandMode & 0x1) << 17)   |
                ((ipMcRouteEntryPtr->ttlHopLimitDecEnable & 0x1) << 18)          |
                ((ipMcRouteEntryPtr->internalMLLPointer & 0x1FFF) << 19));


   counterSet = (GT_U8)((ipMcRouteEntryPtr->countSet == CPSS_IP_CNT_NO_SET_E) ?
                                                7 : ipMcRouteEntryPtr->countSet);
   hwDataPtr[2] = ((ipMcRouteEntryPtr->siteId & 0x1)                                |
                ((ipMcRouteEntryPtr->scopeCheckingEnable & 0x1) << 1)            |
                ((counterSet & 0x7) << 3)                                     |
                ((ipMcRouteEntryPtr->mtuProfileIndex & 0x7) << 6)                |
                ((ipMcRouteEntryPtr->appSpecificCpuCodeEnable & 0x1) << 10)      |
                ((ipMcRouteEntryPtr->multicastRPFCheckEnable & 0x1) << 18));


   hwDataPtr[3] = 0;

   return GT_OK;
}

/*******************************************************************************
* prvCpssDxChIpConvertHwFormat2McEntry
*
* DESCRIPTION:
*       This function converts a given ip mc entry to the HW format.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       hwDataPtr    - The mc entry in the HW format to be converted..
*
* OUTPUTS:
*       ipMcRouteEntryPtr - The mc entry data.
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpConvertHwFormat2McEntry
(
    OUT  CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC *ipMcRouteEntryPtr,
    IN   GT_U32 *hwDataPtr
)
{
    switch(U32_GET_FIELD_MAC(hwDataPtr[0],0,3))
    {
        case 0:
            ipMcRouteEntryPtr->cmd = CPSS_PACKET_CMD_ROUTE_E;
            break;
        case 1:
            ipMcRouteEntryPtr->cmd = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;
            break;
        case 2:
            ipMcRouteEntryPtr->cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            break;
        case 3:
            ipMcRouteEntryPtr->cmd = CPSS_PACKET_CMD_DROP_HARD_E;
            break;
        case 4:
            ipMcRouteEntryPtr->cmd = CPSS_PACKET_CMD_DROP_SOFT_E;
            break;
        case 5:
            ipMcRouteEntryPtr->cmd = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
            break;
        case 6:
            ipMcRouteEntryPtr->cmd = CPSS_PACKET_CMD_BRIDGE_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    switch(U32_GET_FIELD_MAC(hwDataPtr[0],4,2))
    {
         case 0:
             ipMcRouteEntryPtr->cpuCodeIdx = CPSS_DXCH_IP_CPU_CODE_IDX_0_E;
             break;
         case 1:
             ipMcRouteEntryPtr->cpuCodeIdx = CPSS_DXCH_IP_CPU_CODE_IDX_1_E;
             break;
         case 2:
             ipMcRouteEntryPtr->cpuCodeIdx = CPSS_DXCH_IP_CPU_CODE_IDX_2_E;
             break;
         case 3:
             ipMcRouteEntryPtr->cpuCodeIdx = CPSS_DXCH_IP_CPU_CODE_IDX_3_E;
             break;
        default:
            return GT_BAD_STATE;
    }

    ipMcRouteEntryPtr->ttlHopLimDecOptionsExtChkByPass = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwDataPtr[0],3,1));
    ipMcRouteEntryPtr->ingressMirror                   = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwDataPtr[0],6,1));

     switch(U32_GET_FIELD_MAC(hwDataPtr[0],7,1))
     {
     case 0:
         ipMcRouteEntryPtr->qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
         break;
     case 1:
         ipMcRouteEntryPtr->qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
         break;
     default:
         return GT_BAD_STATE;
     }

    ipMcRouteEntryPtr->qosProfileMarkingEnable         = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwDataPtr[0],8,1));
    ipMcRouteEntryPtr->qosProfileIndex                 = U32_GET_FIELD_MAC(hwDataPtr[0],9,7);
    ipMcRouteEntryPtr->multicastRPFVlan        = (GT_U16)U32_GET_FIELD_MAC(hwDataPtr[0],20,12);

    switch(U32_GET_FIELD_MAC(hwDataPtr[0],16,2))
    {
        case 0:
            ipMcRouteEntryPtr->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
            break;
        case 1:
            ipMcRouteEntryPtr->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            break;
        case 2:
            ipMcRouteEntryPtr->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            break;
        case 3:
        default:
            return GT_BAD_STATE;
    }

    switch(U32_GET_FIELD_MAC(hwDataPtr[0],18,2))
    {
        case 0:
            ipMcRouteEntryPtr->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
            break;
        case 1:
            ipMcRouteEntryPtr->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            break;
        case 2:
            ipMcRouteEntryPtr->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            break;
        case 3:
        default:
            return GT_BAD_STATE;
    }
    ipMcRouteEntryPtr->multicastRPFCheckEnable  = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwDataPtr[2],18,1));

    if(ipMcRouteEntryPtr->multicastRPFCheckEnable == GT_TRUE)
    {

        switch(U32_GET_FIELD_MAC(hwDataPtr[1],14,3))
        {
        case 2:
            ipMcRouteEntryPtr->RPFFailCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            break;
        case 3:
            ipMcRouteEntryPtr->RPFFailCommand = CPSS_PACKET_CMD_DROP_HARD_E;
            break;
        case 4:
            ipMcRouteEntryPtr->RPFFailCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
            break;
        case 5:
            ipMcRouteEntryPtr->RPFFailCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
            break;
        case 6:
            ipMcRouteEntryPtr->RPFFailCommand = CPSS_PACKET_CMD_BRIDGE_E;
            break;
        default:
            return GT_BAD_STATE;
        }
    }
    else
    {
      ipMcRouteEntryPtr->RPFFailCommand = 0;
    }

    ipMcRouteEntryPtr->externalMLLPointer              = U32_GET_FIELD_MAC(hwDataPtr[1],0,13);

    switch(U32_GET_FIELD_MAC(hwDataPtr[1],17,1))
    {
    case 0:
        ipMcRouteEntryPtr->multicastRPFFailCommandMode = CPSS_DXCH_IP_MULTICAST_ROUTE_ENTRY_RPF_FAIL_COMMAND_MODE_E;
        break;
    case 1:
        ipMcRouteEntryPtr->multicastRPFFailCommandMode = CPSS_DXCH_IP_MULTICAST_MLL_RPF_FAIL_COMMAND_MODE_E;
        break;
    default:
        return GT_BAD_STATE;
    }

    ipMcRouteEntryPtr->ttlHopLimitDecEnable            = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwDataPtr[1],18,1));
    ipMcRouteEntryPtr->internalMLLPointer              = U32_GET_FIELD_MAC(hwDataPtr[1],19,13);

    switch(U32_GET_FIELD_MAC(hwDataPtr[2],0,1))
    {
    case 0:
        ipMcRouteEntryPtr->siteId = CPSS_IP_SITE_ID_INTERNAL_E;
        break;
    case 1:
        ipMcRouteEntryPtr->siteId = CPSS_IP_SITE_ID_EXTERNAL_E;
        break;
    default:
        return GT_BAD_STATE;
    }

    ipMcRouteEntryPtr->scopeCheckingEnable             = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwDataPtr[2],1,1));
    switch(U32_GET_FIELD_MAC(hwDataPtr[2],3,3))
    {
    case 0:
        ipMcRouteEntryPtr->countSet = CPSS_IP_CNT_SET0_E;
        break;
    case 1:
        ipMcRouteEntryPtr->countSet = CPSS_IP_CNT_SET1_E;
        break;
    case 2:
        ipMcRouteEntryPtr->countSet = CPSS_IP_CNT_SET2_E;
        break;
    case 3:
        ipMcRouteEntryPtr->countSet = CPSS_IP_CNT_SET3_E;
        break;
    case 4:
        ipMcRouteEntryPtr->countSet = CPSS_IP_CNT_NO_SET_E;
        break;
    case 7:
        ipMcRouteEntryPtr->countSet = CPSS_IP_CNT_NO_SET_E;
        break;
    default:
        return GT_BAD_STATE;
    }

    ipMcRouteEntryPtr->mtuProfileIndex                 = U32_GET_FIELD_MAC(hwDataPtr[2],6,3);
    ipMcRouteEntryPtr->appSpecificCpuCodeEnable        = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwDataPtr[2],10,1));


    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChIpUcRouteEntriesCheck
*
* DESCRIPTION:
*       Check validity of the route entry parametrers, in all entries
*       of routeEntriesArray.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum               - the device number
*       routeEntriesArray    - the uc route entries array
*       numOfRouteEntries    - the number route entries in the array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on all valid parameters.
*       GT_BAD_PARAM    - on invalid parameter.
*       GT_OUT_OF_RANGE - on out of range parameter.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpUcRouteEntriesCheck
(
    IN GT_U8                           devNum,
    IN CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *routeEntriesArray,
    IN GT_U32                          numOfRouteEntries
)
{
    GT_U32 i,j;
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC  *routeEntry = NULL;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(routeEntriesArray);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    for (i = 0; i<numOfRouteEntries; i++)
    {
        switch (routeEntriesArray[i].type)
        {
        case CPSS_DXCH_IP_UC_ROUTE_ENTRY_E:

            routeEntry = &routeEntriesArray[i].entry.regularEntry; /* to make the code more readable... */

            switch(routeEntry->cmd)
            {
            case CPSS_PACKET_CMD_ROUTE_E:
            case CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E:
            case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            case CPSS_PACKET_CMD_DROP_HARD_E:
            case CPSS_PACKET_CMD_DROP_SOFT_E:
                break;
            default:
                return GT_BAD_PARAM;
            }

            switch(routeEntry->cpuCodeIdx)
            {
            case CPSS_DXCH_IP_CPU_CODE_IDX_0_E:
            case CPSS_DXCH_IP_CPU_CODE_IDX_1_E:
            case CPSS_DXCH_IP_CPU_CODE_IDX_2_E:
            case CPSS_DXCH_IP_CPU_CODE_IDX_3_E:
                break;
            default:
                return GT_BAD_PARAM;
            }

            switch(routeEntry->qosPrecedence)
            {
            case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E:
            case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E:
                break;
            default:
                return GT_BAD_PARAM;
            }

            switch(routeEntry->modifyUp)
            {
            case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
                break;
            default:
                return GT_BAD_PARAM;
            }

            switch(routeEntry->modifyDscp)
            {
            case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
                break;
            default:
                return GT_BAD_PARAM;
            }

            switch(routeEntry->countSet)
            {
            case CPSS_IP_CNT_SET0_E:
            case CPSS_IP_CNT_SET1_E:
            case CPSS_IP_CNT_SET2_E:
            case CPSS_IP_CNT_SET3_E:
            case CPSS_IP_CNT_NO_SET_E:
                break;
            default:
                return GT_BAD_PARAM;
            }

            switch(routeEntry->siteId)
            {
            case CPSS_IP_SITE_ID_INTERNAL_E:
            case CPSS_IP_SITE_ID_EXTERNAL_E:
                break;
            default:
                return GT_BAD_PARAM;
            }

            if(routeEntry->nextHopVlanId >= BIT_12)
                return GT_OUT_OF_RANGE;

            switch(routeEntry->nextHopInterface.type)
            {
            case CPSS_INTERFACE_PORT_E:
                if(routeEntry->nextHopInterface.devPort.devNum >= BIT_5)
                    return GT_OUT_OF_RANGE;
                if(routeEntry->nextHopInterface.devPort.portNum >= BIT_6)
                    return GT_OUT_OF_RANGE;
                break;
            case CPSS_INTERFACE_TRUNK_E:
            case CPSS_INTERFACE_VIDX_E:
            case CPSS_INTERFACE_VID_E:
                break;
            default:
                return GT_BAD_PARAM;
            }

            if(routeEntry->mtuProfileIndex > 7)
                return GT_OUT_OF_RANGE;

            /* Tunnel pointer and ARP pointer reside on the same HW bits,
               the meaning of those bits is determine by those bits          */
            if(routeEntry->isTunnelStart == GT_TRUE)
            {
                if (routeEntry->nextHopTunnelPointer >= fineTuningPtr->tableSize.tunnelStart)
                    return GT_BAD_PARAM;
            }
            else/* ARP */
            {
                /* ARP pointer is relevant only if packet */
                /* command is route or route and mirror   */
                if ((routeEntry->cmd == CPSS_PACKET_CMD_ROUTE_E) ||
                    (routeEntry->cmd == CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E))
                {
                    if (routeEntry->nextHopARPPointer >= fineTuningPtr->tableSize.routerArp)
                        return GT_BAD_PARAM;
                }
            }

            break;

        case CPSS_DXCH_IP_UC_ECMP_RPF_E:

            for (j = 0 ; j < 8 ; j++)
                {
                if (routeEntriesArray[i].entry.ecmpRpfCheck.vlanArray[j] > 4095)
                    return GT_OUT_OF_RANGE;
            }
            break;

        default:
            return GT_BAD_PARAM;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChIpMcRouteEntryCheck
*
* DESCRIPTION:
*       Check validity of the route entry parametrers, in all entries
*       of routeEntriesArray.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       routeEntryPtr    - the Mc route entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on all valid parameters.
*       GT_BAD_PARAM    - on invalid parameter.
*       GT_OUT_OF_RANGE - on out of range parameter.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpMcRouteEntryCheck
(
    IN CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC *routeEntryPtr
)
{
    switch(routeEntryPtr->cmd)
    {
    case CPSS_PACKET_CMD_ROUTE_E:
    case CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E:
    case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
    case CPSS_PACKET_CMD_DROP_HARD_E:
    case CPSS_PACKET_CMD_DROP_SOFT_E:
    case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
    case CPSS_PACKET_CMD_BRIDGE_E:
        break;
    default:
        return GT_BAD_PARAM;
    }

    switch(routeEntryPtr->cpuCodeIdx)
    {
    case CPSS_DXCH_IP_CPU_CODE_IDX_0_E:
    case CPSS_DXCH_IP_CPU_CODE_IDX_1_E:
    case CPSS_DXCH_IP_CPU_CODE_IDX_2_E:
    case CPSS_DXCH_IP_CPU_CODE_IDX_3_E:
        break;
    default:
        return GT_BAD_PARAM;
    }

    switch(routeEntryPtr->qosPrecedence)
    {
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E:
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E:
        break;
    default:
        return GT_BAD_PARAM;
    }

    switch(routeEntryPtr->modifyUp)
    {
    case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
    case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
    case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
        break;
    default:
        return GT_BAD_PARAM;
    }

    switch(routeEntryPtr->modifyDscp)
    {
    case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
    case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
    case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
        break;
    default:
        return GT_BAD_PARAM;
    }

    switch(routeEntryPtr->countSet)
    {
    case CPSS_IP_CNT_SET0_E:
    case CPSS_IP_CNT_SET1_E:
    case CPSS_IP_CNT_SET2_E:
    case CPSS_IP_CNT_SET3_E:
    case CPSS_IP_CNT_NO_SET_E:
        break;
    default:
        return GT_BAD_PARAM;
    }

    switch(routeEntryPtr->siteId)
    {
    case CPSS_IP_SITE_ID_INTERNAL_E:
    case CPSS_IP_SITE_ID_EXTERNAL_E:
        break;
    default:
        return GT_BAD_PARAM;
    }

    if(routeEntryPtr->multicastRPFCheckEnable)
    {
        switch(routeEntryPtr->multicastRPFFailCommandMode)
        {
        case CPSS_DXCH_IP_MULTICAST_ROUTE_ENTRY_RPF_FAIL_COMMAND_MODE_E:

            switch(routeEntryPtr->RPFFailCommand)
            {
            case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            case CPSS_PACKET_CMD_DROP_SOFT_E:
            case CPSS_PACKET_CMD_DROP_HARD_E:
            case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            case CPSS_PACKET_CMD_BRIDGE_E:
                break;
            default:
                return GT_BAD_PARAM;
            }
        case CPSS_DXCH_IP_MULTICAST_MLL_RPF_FAIL_COMMAND_MODE_E:
            break;
        default:
            return GT_BAD_PARAM;
        }
    }

    if((routeEntryPtr->multicastRPFCheckEnable == GT_TRUE) &&
       (routeEntryPtr->multicastRPFVlan >= BIT_12))
    {
        return GT_OUT_OF_RANGE;
    }

    if(routeEntryPtr->mtuProfileIndex > 7)
        return GT_OUT_OF_RANGE;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChIpMllStruct2HwFormat
*
* DESCRIPTION:
*       This function converts a given ip Mll struct to the hardware format.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       mllPairWriteForm  - the way to write the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*       mllPairEntryPtr   - (points to ) the entry to be converted.
*
* OUTPUTS:
*       hwDataPtr            - The entry in the HW format representation.
*
* RETURNS:
*       GT_OK             - on all valid parameters.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChIpMllStruct2HwFormat
(
    IN  CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_FORM_ENT   mllPairWriteForm,
    IN  CPSS_DXCH_IP_MLL_PAIR_STC                   *mllPairEntryPtr,
    OUT GT_U32                                      *hwDataPtr
)
{
    GT_U32 rpfFailCommand;

    /* clear hw data */
    cpssOsMemSet(hwDataPtr, 0, sizeof(CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD));

    if((mllPairWriteForm == CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_FIRST_MLL_ONLY_E) ||
       (mllPairWriteForm == CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_WHOLE_E))
    {
        switch(mllPairEntryPtr->firstMllNode.mllRPFFailCommand)
        {
            case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
                rpfFailCommand = 2;
                break;
            case CPSS_PACKET_CMD_DROP_HARD_E:
                rpfFailCommand = 3;
                break;
            case CPSS_PACKET_CMD_DROP_SOFT_E:
                rpfFailCommand = 4;
                break;
            case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
                rpfFailCommand = 5;
                break;
            case CPSS_PACKET_CMD_BRIDGE_E:
                rpfFailCommand = 6;
                break;
            default:
                return GT_BAD_PARAM;
        }

        hwDataPtr[0] = (BOOL2BIT_MAC(mllPairEntryPtr->firstMllNode.last)                |
                     (BOOL2BIT_MAC(mllPairEntryPtr->firstMllNode.isTunnelStart) << 1)|
                     ((mllPairEntryPtr->firstMllNode.nextHopVlanId & 0xFFF) << 3)|
                     ((rpfFailCommand & 0x7) << 29));

        switch(mllPairEntryPtr->firstMllNode.nextHopInterface.type)
        {
            case CPSS_INTERFACE_PORT_E:
                hwDataPtr[0] |= ((0x0 << 15) | (0x0 << 16)                                                      |
                              ((mllPairEntryPtr->firstMllNode.nextHopInterface.devPort.portNum & 0x3F) << 17)|
                              ((mllPairEntryPtr->firstMllNode.nextHopInterface.devPort.devNum & 0x1F) << 23));
                break;
            case CPSS_INTERFACE_TRUNK_E:
                hwDataPtr[0] |= ((0x0 << 15) | (0x1 << 16) |
                              ((mllPairEntryPtr->firstMllNode.nextHopInterface.trunkId & 0x7F) << 21));
                break;
            case CPSS_INTERFACE_VIDX_E:
                hwDataPtr[0] |= ((0x1 << 15) |
                             ((mllPairEntryPtr->firstMllNode.nextHopInterface.vidx & 0xFFF) << 16));

                break;
            default:
                return GT_BAD_PARAM;

        }

        hwDataPtr[1] = ((mllPairEntryPtr->firstMllNode.nextHopTunnelPointer & 0x3FF)        |
                     (0x1 << 10)                                                         |
                     ((mllPairEntryPtr->firstMllNode.ttlHopLimitThreshold & 0xFF) << 11) |
                     (BOOL2BIT_MAC(mllPairEntryPtr->firstMllNode.excludeSrcVlan) << 19));
    }

    if((mllPairWriteForm == CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_SECOND_MLL_NEXT_POINTER_ONLY_E) ||
       (mllPairWriteForm == CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_WHOLE_E))
    {
        switch(mllPairEntryPtr->secondMllNode.mllRPFFailCommand)
        {
            case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
                rpfFailCommand = 2;
                break;
            case CPSS_PACKET_CMD_DROP_HARD_E:
                rpfFailCommand = 3;
                break;
            case CPSS_PACKET_CMD_DROP_SOFT_E:
                rpfFailCommand = 4;
                break;
            case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
                rpfFailCommand = 5;
                break;
            case CPSS_PACKET_CMD_BRIDGE_E:
                rpfFailCommand = 6;
                break;
            default:
                return GT_BAD_PARAM;
        }

        hwDataPtr[2] = (BOOL2BIT_MAC(mllPairEntryPtr->secondMllNode.last)                 |
                     (BOOL2BIT_MAC(mllPairEntryPtr->secondMllNode.isTunnelStart) << 1) |
                     ((mllPairEntryPtr->secondMllNode.nextHopVlanId & 0xFFF) << 3) |
                     ((rpfFailCommand & 0x7) << 29));

        switch(mllPairEntryPtr->secondMllNode.nextHopInterface.type)
        {
            case CPSS_INTERFACE_PORT_E:
                hwDataPtr[2] |= ((0x0 << 15) | (0x0 << 16)                                                       |
                              ((mllPairEntryPtr->secondMllNode.nextHopInterface.devPort.portNum & 0x3F) << 17)|
                              ((mllPairEntryPtr->secondMllNode.nextHopInterface.devPort.devNum & 0x1F) << 23));
                break;
            case CPSS_INTERFACE_TRUNK_E:
                hwDataPtr[2] |= ((0x0 << 15) | (0x1 << 16) |
                              ((mllPairEntryPtr->secondMllNode.nextHopInterface.trunkId & 0x7F) << 21));
                break;
            case CPSS_INTERFACE_VIDX_E:
                hwDataPtr[2] |= ((0x1 << 15) |
                             ((mllPairEntryPtr->secondMllNode.nextHopInterface.vidx & 0xFFF) << 16));

                break;
            default:
                return GT_BAD_PARAM;

        }

        hwDataPtr[3] = ((mllPairEntryPtr->secondMllNode.nextHopTunnelPointer & 0x3FF)        |
                     (0x1 << 10)                                                          |
                     ((mllPairEntryPtr->secondMllNode.ttlHopLimitThreshold & 0xFF) << 11) |
                     (BOOL2BIT_MAC(mllPairEntryPtr->secondMllNode.excludeSrcVlan) << 19)      |
                     ((mllPairEntryPtr->nextPointer & 0xFFF) << 20));
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChIpHwFormat2MllStruct
*
* DESCRIPTION:
*       This function converts a given ip Mll struct to the hardware format.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       hwDataPtr     - The entry in the HW format representation.
*
* OUTPUTS:
*       mllEntryPtr   - The entry to be converted.
*
* RETURNS:
*       GT_OK           - on all valid parameters.
*       GT_BAD_STATE    - on invalid hardware value read
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChIpHwFormat2MllStruct
(
    IN   GT_U32                 *hwDataPtr,
    OUT  CPSS_DXCH_IP_MLL_STC   *mllEntryPtr
)
{
    GT_U32 useVidx0;
    GT_U32 targetIsTrunk;

    /* clear output data */
    cpssOsMemSet(mllEntryPtr, 0, sizeof(CPSS_DXCH_IP_MLL_STC));

    mllEntryPtr->last          = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwDataPtr[0],0,1));
    mllEntryPtr->isTunnelStart = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwDataPtr[0],1,1));
    mllEntryPtr->nextHopVlanId = (GT_U16)U32_GET_FIELD_MAC(hwDataPtr[0],3,12);

    useVidx0      = U32_GET_FIELD_MAC(hwDataPtr[0],15,1);
    targetIsTrunk = U32_GET_FIELD_MAC(hwDataPtr[0],16,1);

    if(useVidx0 == 1)
    {
        mllEntryPtr->nextHopInterface.type = CPSS_INTERFACE_VIDX_E;
        mllEntryPtr->nextHopInterface.vidx =
            (GT_U16)U32_GET_FIELD_MAC(hwDataPtr[0],16,12);
    }
    else if(targetIsTrunk == 1)
    {
        mllEntryPtr->nextHopInterface.type = CPSS_INTERFACE_TRUNK_E;
        mllEntryPtr->nextHopInterface.trunkId =
            (GT_U16)U32_GET_FIELD_MAC(hwDataPtr[0],21,7);
    }
    else
    {
        mllEntryPtr->nextHopInterface.type = CPSS_INTERFACE_PORT_E;
        mllEntryPtr->nextHopInterface.devPort.portNum =
            (GT_U8)U32_GET_FIELD_MAC(hwDataPtr[0],17,6);
        mllEntryPtr->nextHopInterface.devPort.devNum  =
            (GT_U8)U32_GET_FIELD_MAC(hwDataPtr[0],23,5);
    }

    switch(U32_GET_FIELD_MAC(hwDataPtr[0],29,3))
    {
        case 2:
            mllEntryPtr->mllRPFFailCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            break;
        case 3:
            mllEntryPtr->mllRPFFailCommand = CPSS_PACKET_CMD_DROP_HARD_E;
            break;
        case 4:
            mllEntryPtr->mllRPFFailCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
            break;
        case 5:
            mllEntryPtr->mllRPFFailCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
            break;
        case 6:
            mllEntryPtr->mllRPFFailCommand = CPSS_PACKET_CMD_BRIDGE_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    mllEntryPtr->nextHopTunnelPointer = U32_GET_FIELD_MAC(hwDataPtr[1],0,10);
    mllEntryPtr->ttlHopLimitThreshold = (GT_U16)U32_GET_FIELD_MAC(hwDataPtr[1],11,8);
    mllEntryPtr->excludeSrcVlan       = U32_GET_FIELD_MAC(hwDataPtr[1],19,1);

    return GT_OK;
}


#ifndef __AX_PLATFORM__
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
GT_STATUS cpssDxChIPv4LinkLocalMirrorEnable
(
    IN GT_U8               dev,
    IN GT_BOOL			  enable
)
{

	GT_STATUS rc = GT_OK;
	GT_U32 i = 0, arraySize = sizeof(LinkLocalMulticastMirrorIndex)/sizeof(Llm_Info);
	CPSS_IP_PROTOCOL_STACK_ENT protocolStack = LinkLocalMulticastMirrorIndex[i].protocolStack;

	rc = cpssDxChBrgGenIPv4LinkLocalMirrorEnable(dev,enable);
	if(rc != GT_OK)
		return rc;

	for(i = 0; i < arraySize; i++) {
		/* dual-protocol stack */
		if(CPSS_IP_PROTOCOL_IPV4V6_E == protocolStack) {
			rc  = cpssDxChBrgGenIpLinkLocalProtMirrorToCpuEnable(	\
							dev, CPSS_IP_PROTOCOL_IPV4_E, LinkLocalMulticastMirrorIndex[i].tblIndex,enable);
			if(rc != GT_OK)
				return rc;
			rc  = cpssDxChBrgGenIpLinkLocalProtMirrorToCpuEnable(	\
							dev, CPSS_IP_PROTOCOL_IPV6_E, LinkLocalMulticastMirrorIndex[i].tblIndex,enable);
			if(rc != GT_OK)
				return rc;
			
			rc = cpssDxChNetIfCpuCodeIpLinkLocalProtSet(	\
							dev, CPSS_IP_PROTOCOL_IPV4_E, LinkLocalMulticastMirrorIndex[i].tblIndex,	\
							LinkLocalMulticastMirrorIndex[i].cpuCode);
			if(rc != GT_OK)
				return rc;

			rc = cpssDxChNetIfCpuCodeIpLinkLocalProtSet(	\
							dev, CPSS_IP_PROTOCOL_IPV6_E, LinkLocalMulticastMirrorIndex[i].tblIndex,	\
							LinkLocalMulticastMirrorIndex[i].cpuCode);
			if(rc != GT_OK)
				return rc;
		}
		else {/* uni-protocol stack */
			rc  = cpssDxChBrgGenIpLinkLocalProtMirrorToCpuEnable(	\
							dev, protocolStack, LinkLocalMulticastMirrorIndex[i].tblIndex,enable);
			if(rc != GT_OK)
				return rc;
			
			rc = cpssDxChNetIfCpuCodeIpLinkLocalProtSet(	\
							dev, protocolStack,LinkLocalMulticastMirrorIndex[i].tblIndex,	\
							LinkLocalMulticastMirrorIndex[i].cpuCode);
			if(rc != GT_OK)
				return rc;
			}
		}

	return rc;
}

/*******************************************************************************
* cpssDxChNetIfMultiTrapToCpu
*
* DESCRIPTION:
*        set net intf multicast packet trap to cpu.
*		  
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
GT_STATUS cpssDxChNetIfSetMultiTrapToCpu
(
	IN GT_U8			   devNum,
	IN GT_BOOL			  enable
)
{
	
	GT_STATUS rc 	= GT_OK;
	GT_U32 i = 0, arraySize = sizeof(LinkLocalMulticastToCpuIndex)/sizeof(mutiInfo);

	for(i=0;i<arraySize;i++) {
		rc = cpssDxChBrgGenIeeeReservedMcastProtCmdSet(devNum,0,LinkLocalMulticastToCpuIndex[i].ieeeResProtocol,LinkLocalMulticastToCpuIndex[i].actType);
		if(GT_OK != rc) {
			return rc;
		}
		rc = cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet(devNum,LinkLocalMulticastToCpuIndex[i].ieeeResProtocol,LinkLocalMulticastToCpuIndex[i].ieeeCpuCode);
		if(GT_OK != rc) {
			return rc;
		}
	}
	rc = cpssDxChBrgGenIeeeReservedMcastTrapEnable(devNum,enable);
	if(GT_OK != rc) {
		return rc;
	}
	return rc;
}
#endif

