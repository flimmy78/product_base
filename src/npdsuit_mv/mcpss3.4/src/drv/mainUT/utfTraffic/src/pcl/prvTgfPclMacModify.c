/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfPclMacModify.c
*
* DESCRIPTION:
*       PCL MAC address modification specific PCL features testing
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/pcl/cpssPcl.h>
#include <gtOs/gtOsTimer.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>

#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfPclGen.h>
#include <common/tgfIpGen.h>
#include <pcl/prvTgfPclMacModify.h>

#define PRV_TGF_DUMP_PACKTS_CNS 0

/* port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_CNS     1

/* port number to receive traffic from */
#define PRV_TGF_RECEIVE_PORT_IDX_CNS  2

/* amount of sent packets */
#define PRV_TGF_PACKET_NUM_CNS 5

/* index of MacDa in ARP table */
#define PRV_TGF_ARP_MAC_DA_INDEX_CNS 3

/* value of MacDa in ARP table */
#define PRV_TGF_ARP_MAC_DA_VALUE_CNS {0x00, 0x12, 0x23, 0x45, 0x67, 0x89}

/* value of Base MacSa */
#define PRV_TGF_MAC_SA_BASE_CNS {0x00, 0xA1, 0xB2, 0xC3, 0xD4, 0x00}

/* value of MacSa LSB*/
#define PRV_TGF_MAC_SA_LSB_CNS 0xE5

/* PCL rule index */
static GT_U32   prvTgfPclRuleIndex = 1;

/********************************************************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part =
{
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x55},                /* daMac */
    {0x00, 0x66, 0x77, 0x88, 0x99, 0x11}                 /* saMac */
};

/* ethertype part of ETH_OTHER packet */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketEtherOtherTypePart = {0x2222};

/* Data of packet (long version) */
static GT_U8 prvTgfPacketPayloadLongDataArr[] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f
};

/* Payload part (long version) */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketLongPayloadPart =
{
    sizeof(prvTgfPacketPayloadLongDataArr), /* dataLength */
    prvTgfPacketPayloadLongDataArr          /* dataPtr */
};


/* PARTS of packet */
static TGF_PACKET_PART_STC prvTgfEthernetPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketEtherOtherTypePart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketLongPayloadPart}
};

/* PACKET1 to send */
static TGF_PACKET_STC prvTgfEthernetPacketInfo = {
    (TGF_L2_HEADER_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS
     + sizeof(prvTgfPacketPayloadLongDataArr)),            /* totalLen */
    (sizeof(prvTgfEthernetPacketPartArray)
     / sizeof(TGF_PACKET_PART_STC)),                       /* numOfParts */
    prvTgfEthernetPacketPartArray                          /* partsArray */
};

/*******************************************************************************
* prvTgfTrafficGenerateWithCapture
*
* DESCRIPTION:
*       None
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfTrafficGenerateWithCapture
(
    GT_VOID
)
{
    GT_STATUS               rc = GT_OK;
    CPSS_INTERFACE_INFO_STC portInterface;

    /* clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    PRV_UTF_VERIFY_GT_OK(rc, "tgfTrafficTableRxPcktTblClear");

    /* setup receive portInterface for capturing */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum =
        prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS];

    /* enable capture on receive port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(
            &portInterface, TGF_CAPTURE_MODE_MIRRORING_E, GT_TRUE);
    PRV_UTF_VERIFY_GT_OK(rc, "tgftrafficgeneratorporttxethcaptureset");


    /* setup transmit params */
    rc = prvTgfSetTxSetupEth(
        prvTgfDevNum, &prvTgfEthernetPacketInfo,
        PRV_TGF_PACKET_NUM_CNS /*burstCount*/, 0 /*numVfd*/, NULL);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfSetTxSetupEth");

    /* start transmitting */
    rc = prvTgfStartTransmitingEth(
        prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfStartTransmitingEth");

    osTimerWkAfter(2000);

    /* enable capture on receive port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(
            &portInterface, TGF_CAPTURE_MODE_MIRRORING_E, GT_FALSE);
    PRV_UTF_VERIFY_GT_OK(rc, "tgftrafficgeneratorporttxethcaptureset");

    return rc;
}

/*******************************************************************************
* prvTgfTrafficCheckCapturedByField
*
* DESCRIPTION:
*       None
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfTrafficCheckCapturedByField
(
    IN GT_U8                devNum,
    IN GT_U8                portNum,
    IN GT_U32               packetAmount,
    IN GT_U32               fieldOffset,
    IN GT_U32               fieldLength,
    IN GT_U8                fieldArr[]
)
{
    GT_STATUS       rc, rc1;
    GT_U8           dev      = 0;
    GT_U8           queue    = 0;
    GT_BOOL         getFirst = GT_TRUE;
    GT_U32          buffLen  = TGF_RX_BUFFER_MAX_SIZE_CNS;
    GT_U32          packetActualLength = 0;
    TGF_NET_DSA_STC rxParam;
    static GT_U8    packetBuff[TGF_RX_BUFFER_MAX_SIZE_CNS] = {0};

    rc1  = GT_OK;

    for (; (packetAmount > 0); packetAmount--)
    {
        /* get entry from rxNetworkIf table */
        rc = tgfTrafficGeneratorRxInCpuGet(TGF_PACKET_TYPE_CAPTURE_E,
                                           getFirst, GT_TRUE, packetBuff,
                                           &buffLen, &packetActualLength,
                                           &dev, &queue, &rxParam);
        PRV_UTF_VERIFY_GT_OK(rc, "tgfTrafficGeneratorRxInCpuGet");

        getFirst = GT_FALSE;

#if PRV_TGF_DUMP_PACKTS_CNS
        {
            int i;
            for (i = 0; (i < 24); i++)
            {
                if ((i % 6) == 0)
                {
                    cpssOsPrintf("  ");
                }

                cpssOsPrintf("%02X", packetBuff[i]);
            }
            cpssOsPrintf("\n");
        }
#endif /*PRV_TGF_DUMP_PACKTS_CNS*/


        /* trace packet */

        /* check the DSA tag */
        if (GT_FALSE == rxParam.dsaCmdIsToCpu || GT_TRUE == rxParam.srcIsTrunk ||
           rxParam.devNum != devNum || rxParam.portNum != portNum)
        {
            /* go to the next packet */
            continue;
        }

        if (packetActualLength < (fieldOffset + fieldLength))
        {
            rc1 = GT_FAIL;
        }

        if (0 != cpssOsMemCmp(fieldArr, &(packetBuff[fieldOffset]), fieldLength))
        {
            rc1 = GT_FAIL;
        }

    }
    return rc1;
}

/*******************************************************************************
* prvTgfConfigurationRestore
*
* DESCRIPTION:
*       None
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS rc, rc1 = GT_OK;

    /* disable ingress policy on ports */
    rc = prvTgfPclPortIngressPolicyEnable(
        prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE);
    PRV_UTF_VERIFY_RC1(rc, "prvTgfPclPortIngressPolicyEnable");

    /* invalidate PCL rules */
    rc = prvTgfPclRuleValidStatusSet(
            CPSS_PCL_RULE_SIZE_STD_E, prvTgfPclRuleIndex, GT_FALSE);
    PRV_UTF_VERIFY_RC1(rc, "prvTgfPclRuleValidStatusSet");

    /* enables ingress policy */
    rc = prvTgfPclPortIngressPolicyEnable(
        prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfPclPortIngressPolicyEnable FAILED, rc = [%d]", rc);

        return rc;
    }

    /* clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    PRV_UTF_VERIFY_RC1(rc, "tgfTrafficTableRxPcktTblClear");

    return rc1;
};

/*******************************************************************************
* prvTgfTrafficPclRuleAndHA_ModifiMacDa
*
* DESCRIPTION:
*       None
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfTrafficPclRuleAndHA_ModifiMacDa
(
    INOUT PRV_TGF_PCL_RULE_FORMAT_UNT *maskPtr,
    INOUT PRV_TGF_PCL_RULE_FORMAT_UNT *pattPtr,
    INOUT PRV_TGF_PCL_ACTION_STC      *actionPtr
)
{
    GT_STATUS      rc = GT_OK;
    GT_ETHERADDR   arpMacAddr = {PRV_TGF_ARP_MAC_DA_VALUE_CNS};

    /* mask for MAC address */
    cpssOsMemSet(maskPtr, 0, sizeof(*maskPtr));

    /* difine mask, pattern and action */
    cpssOsMemSet(pattPtr, 0, sizeof(*pattPtr));

    /* action redirect */
    cpssOsMemSet(actionPtr, 0, sizeof(*actionPtr));
    actionPtr->pktCmd                                           = CPSS_PACKET_CMD_FORWARD_E;
    actionPtr->bypassIngressPipe                                = GT_TRUE;
    actionPtr->redirect.redirectCmd                             = PRV_TGF_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
    actionPtr->redirect.data.outIf.outInterface.type            = CPSS_INTERFACE_PORT_E;
    actionPtr->redirect.data.outIf.outInterface.devPort.devNum  = prvTgfDevNum;
    actionPtr->redirect.data.outIf.outInterface.devPort.portNum =
        prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS];

    actionPtr->redirect.data.outIf.outlifPointer.arpPtr =
        PRV_TGF_ARP_MAC_DA_INDEX_CNS;
    actionPtr->redirect.data.outIf.modifyMacDa = GT_TRUE;

    rc = prvTgfIpRouterArpAddrWrite(
        PRV_TGF_ARP_MAC_DA_INDEX_CNS, &arpMacAddr);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfIpRouterArpAddrWrite");

    PRV_UTF_LOG0_MAC(" TEST -------\n");


    return rc;
};

/*******************************************************************************
* prvTgfTrafficPclRuleAndHA_ModifiMacSa
*
* DESCRIPTION:
*       None
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfTrafficPclRuleAndHA_ModifiMacSa
(
    INOUT PRV_TGF_PCL_RULE_FORMAT_UNT *maskPtr,
    INOUT PRV_TGF_PCL_RULE_FORMAT_UNT *pattPtr,
    INOUT PRV_TGF_PCL_ACTION_STC      *actionPtr
)
{
    GT_STATUS      rc = GT_OK;
    GT_ETHERADDR   macAddr = {PRV_TGF_MAC_SA_BASE_CNS};
    GT_U8          egrPort =
        prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS];

    /* mask for MAC address */
    cpssOsMemSet(maskPtr, 0, sizeof(*maskPtr));

    /* difine mask, pattern and action */
    cpssOsMemSet(pattPtr, 0, sizeof(*pattPtr));

    /* action redirect */
    cpssOsMemSet(actionPtr, 0, sizeof(*actionPtr));
    actionPtr->pktCmd                                           = CPSS_PACKET_CMD_FORWARD_E;
    actionPtr->bypassIngressPipe                                = GT_TRUE;
    actionPtr->redirect.redirectCmd                             = PRV_TGF_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
    actionPtr->redirect.data.outIf.outInterface.type            = CPSS_INTERFACE_PORT_E;
    actionPtr->redirect.data.outIf.outInterface.devPort.devNum  = prvTgfDevNum;
    actionPtr->redirect.data.outIf.outInterface.devPort.portNum = egrPort;

    actionPtr->redirect.data.outIf.modifyMacSa = GT_TRUE;

    rc = prvTgfIpRouterMacSaBaseSet(
        prvTgfDevNum, &macAddr);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfIpRouterMacSaBaseSet");

    rc = prvTgfIpRouterMacSaModifyEnable(
        prvTgfDevNum, egrPort, GT_TRUE);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfIpRouterMacSaModifyEnable");

    rc = prvTgfIpPortRouterMacSaLsbModeSet(
        prvTgfDevNum, egrPort, CPSS_SA_LSB_PER_PORT_E);
    PRV_UTF_VERIFY_GT_OK(rc, "cpssDxChIpPortRouterMacSaLsbModeSet");

    rc = prvTgfIpRouterPortMacSaLsbSet(
        prvTgfDevNum, egrPort, PRV_TGF_MAC_SA_LSB_CNS);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfIpRouterPortMacSaLsbSet");

    return rc;
};

/*******************************************************************************
* prvTgfPclRedirectMacDaModify
*
* DESCRIPTION:
*       Set test configuration
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfPclRedirectMacDaModify
(
    GT_VOID
)
{
    GT_STATUS                   rc = GT_OK;
    PRV_TGF_PCL_RULE_FORMAT_UNT mask;
    PRV_TGF_PCL_RULE_FORMAT_UNT pattern;
    PRV_TGF_PCL_ACTION_STC      action;
    GT_ETHERADDR   macAddr = {PRV_TGF_ARP_MAC_DA_VALUE_CNS};

    rc = prvTgfTrafficPclRuleAndHA_ModifiMacDa(
        &mask, &pattern, &action);
    UTF_VERIFY_EQUAL0_STRING_MAC(
        GT_OK, rc, "prvTgfTrafficPclRuleAndHA_ModifiMacDa");

    /* init PCL Engine for send port */
    rc = prvTgfPclDefPortInit(
        prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
        CPSS_PCL_DIRECTION_INGRESS_E,
        CPSS_PCL_LOOKUP_0_E,
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E /*nonIpKey*/,
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E /*ipv4Key*/,
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E /*ipv6Key*/);
    UTF_VERIFY_EQUAL1_STRING_MAC(
        GT_OK, rc, "prvTgfPclDefPortInit: %d", prvTgfDevNum);

    rc = prvTgfPclRuleSet(
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
        prvTgfPclRuleIndex, &mask, &pattern, &action);
    UTF_VERIFY_EQUAL2_STRING_MAC(
        GT_OK, rc, "prvTgfPclRuleSet: %d, %d",
        prvTgfDevNum, prvTgfPclRuleIndex);

    rc = prvTgfTrafficGenerateWithCapture();
    UTF_VERIFY_EQUAL0_STRING_MAC(
        GT_OK, rc, "prvTgfTrafficGenerate");

    rc = prvTgfTrafficCheckCapturedByField(
        prvTgfDevNum,
        prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS],
        PRV_TGF_PACKET_NUM_CNS,
        0 /*fieldOffset*/,
        6 /*fieldLength*/,
        &(macAddr.arEther[0]));
    UTF_VERIFY_EQUAL0_STRING_MAC(
        GT_OK, rc, "prvTgfTrafficCheckCapturedByField");

    prvTgfConfigurationRestore();
}

/*******************************************************************************
* prvTgfPclRedirectMacSaModify
*
* DESCRIPTION:
*       Set test configuration
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfPclRedirectMacSaModify
(
    GT_VOID
)
{
    GT_STATUS                   rc = GT_OK;
    PRV_TGF_PCL_RULE_FORMAT_UNT mask;
    PRV_TGF_PCL_RULE_FORMAT_UNT pattern;
    PRV_TGF_PCL_ACTION_STC      action;
    GT_ETHERADDR   macAddr = {PRV_TGF_MAC_SA_BASE_CNS};

    macAddr.arEther[5] = PRV_TGF_MAC_SA_LSB_CNS;

    rc = prvTgfTrafficPclRuleAndHA_ModifiMacSa(
        &mask, &pattern, &action);
    UTF_VERIFY_EQUAL0_STRING_MAC(
        GT_OK, rc, "prvTgfTrafficPclRuleAndHA_ModifiMacSa");

    /* init PCL Engine for send port */
    rc = prvTgfPclDefPortInit(
        prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
        CPSS_PCL_DIRECTION_INGRESS_E,
        CPSS_PCL_LOOKUP_0_E,
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E /*nonIpKey*/,
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E /*ipv4Key*/,
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E /*ipv6Key*/);
    UTF_VERIFY_EQUAL1_STRING_MAC(
        GT_OK, rc, "prvTgfPclDefPortInit: %d", prvTgfDevNum);

    rc = prvTgfPclRuleSet(
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
        prvTgfPclRuleIndex, &mask, &pattern, &action);
    UTF_VERIFY_EQUAL2_STRING_MAC(
        GT_OK, rc, "prvTgfPclRuleSet: %d, %d",
        prvTgfDevNum, prvTgfPclRuleIndex);

    rc = prvTgfTrafficGenerateWithCapture();
    UTF_VERIFY_EQUAL0_STRING_MAC(
        GT_OK, rc, "prvTgfTrafficGenerate");

    rc = prvTgfTrafficCheckCapturedByField(
        prvTgfDevNum,
        prvTgfPortsArray[PRV_TGF_RECEIVE_PORT_IDX_CNS],
        PRV_TGF_PACKET_NUM_CNS,
        6 /*fieldOffset*/,
        6 /*fieldLength*/,
        &(macAddr.arEther[0]));
    UTF_VERIFY_EQUAL0_STRING_MAC(
        GT_OK, rc, "prvTgfTrafficCheckCapturedByField");

    prvTgfConfigurationRestore();
}


