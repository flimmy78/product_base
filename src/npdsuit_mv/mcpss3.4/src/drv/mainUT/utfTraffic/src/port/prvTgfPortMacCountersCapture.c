/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfPortMacCountersCapture.c
*
* DESCRIPTION:
*       Specific Port MAC Counters features testing
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/pcl/cpssPcl.h>
#include <cpss/generic/cpssCommonDefs.h>


#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>

#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <port/prvTgfPortMacCountersCapture.h>

#include <common/tgfCommon.h>
#include <common/tgfPortGen.h>
#include <common/tgfBridgeGen.h>

/* default VLAN Id */
#define PRV_TGF_VLANID_CNS            2

/* ether type used for "vlan tag" */
#define TGF_ETHERTYPE_8100_VLAN_TAG_CNS 0x8100

/* port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_0_CNS     0

/* port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_2_CNS     2

/* default number of packets to send */
static GT_U32   prvTgfBurstCount   = 1;

/* FDB MAC entry */
static PRV_TGF_BRG_MAC_ENTRY_STC prvTgfMacEntry;


/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part = {
    {0x00, 0x00, 0x00, 0x00, 0x11, 0x11},               /* dstMac */
    {0x00, 0x00, 0x00, 0x00, 0x22, 0x22}                /* srcMac */
};

/* First VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTag0Part = {
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,                    /* etherType */
    0, 0, PRV_TGF_VLANID_CNS                            /* pri, cfi, VlanId */
};

/* DATA of packet */
static GT_U8 prvTgfPayloadDataArr[] = {
    0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x55};

/* DATA of packet with size 100 */
static GT_U8 prvTgfPayloadData80BytesArr[80];

/* DATA of packet with size 200 */
static GT_U8 prvTgfPayloadData200BytesArr[200];

/* DATA of packet with size 300 */
static GT_U8 prvTgfPayloadData300BytesArr[300];

/* DATA of packet with size 500 */
static GT_U8 prvTgfPayloadData500BytesArr[500];

/* DATA of packet with size 1024 */
static GT_U8 prvTgfPayloadData1024BytesArr[_1KB];

/* PAYLOAD part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketPayloadPart = {
    sizeof(prvTgfPayloadDataArr),                       /* dataLength */
    prvTgfPayloadDataArr                                /* dataPtr */
};

/* PARTS of packet */
static TGF_PACKET_PART_STC prvTgfPacketPartArray[] = {
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTag0Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};


/* LENGTH of packet */
#define PRV_TGF_PACKET_LEN_CNS \
    (TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + sizeof(prvTgfPayloadDataArr))

/* LENGTH of packet with CRC */
#define PRV_TGF_PACKET_CRC_LEN_CNS  PRV_TGF_PACKET_LEN_CNS + TGF_CRC_LEN_CNS


/* PACKET to send */
static TGF_PACKET_STC prvTgfPacketInfo = {
    PRV_TGF_PACKET_LEN_CNS,                                      /* totalLen */
    sizeof(prvTgfPacketPartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketPartArray                                        /* partsArray */
};

/******************************************************************************\
 *                            Private test functions                          *
\******************************************************************************/
/*******************************************************************************
* prvTgfPolicerVlanFdbConfigurationSet
*
* DESCRIPTION:
*       Set FDB configuration.
*
*       MAC                 VID     VIDX    DA_CMD
*       01:02:03:04:05:06   2       1       CPSS_MAC_TABLE_FRWRD_E
*
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
static GT_VOID prvTgfPortMacCountersFdbConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS                   rc;                 /* return code */

    /* Add 5 MAC Entries */

    /* clear entry */
    cpssOsMemSet(&prvTgfMacEntry, 0, sizeof(prvTgfMacEntry));

    prvTgfMacEntry.key.entryType                = PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E;
    prvTgfMacEntry.key.key.macVlan.vlanId       = PRV_TGF_VLANID_CNS;
    prvTgfMacEntry.dstInterface.type            = CPSS_INTERFACE_VIDX_E;
    prvTgfMacEntry.dstInterface.vidx            = 1;
    prvTgfMacEntry.isStatic                     = GT_TRUE;
    prvTgfMacEntry.daCommand                    = PRV_TGF_PACKET_CMD_FORWARD_E;
    prvTgfMacEntry.saCommand                    = PRV_TGF_PACKET_CMD_FORWARD_E;
    prvTgfMacEntry.daRoute                      = GT_FALSE;
    prvTgfMacEntry.mirrorToRxAnalyzerPortEn     = GT_FALSE;
    prvTgfMacEntry.userDefined                  = 0;
    prvTgfMacEntry.daQosIndex                   = 0;
    prvTgfMacEntry.saQosIndex                   = 0;
    prvTgfMacEntry.daSecurityLevel              = 0;
    prvTgfMacEntry.saSecurityLevel              = 0;
    prvTgfMacEntry.appSpecificCpuCode           = GT_FALSE;
    prvTgfMacEntry.pwId                         = 0;
    prvTgfMacEntry.spUnknown                    = GT_FALSE;
    prvTgfMacEntry.sourceId                     = 1;

    prvTgfMacEntry.key.key.macVlan.macAddr.arEther[0] = 0x01;
    prvTgfMacEntry.key.key.macVlan.macAddr.arEther[1] = 0x02;
    prvTgfMacEntry.key.key.macVlan.macAddr.arEther[2] = 0x03;
    prvTgfMacEntry.key.key.macVlan.macAddr.arEther[3] = 0x04;
    prvTgfMacEntry.key.key.macVlan.macAddr.arEther[4] = 0x05;
    prvTgfMacEntry.key.key.macVlan.macAddr.arEther[5] = 0x06;

    /* 00:00:00:00:00:10 on port 0 */
    rc = prvTgfBrgFdbMacEntrySet(&prvTgfMacEntry);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in prvTgfBrgFdbMacEntrySet %d", GT_TRUE);
}

/*******************************************************************************
* prvTgfPortMacCountersFdbEntryDelete
*
* DESCRIPTION:
*       Delete FDB entry.
*

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
static GT_VOID prvTgfPortMacCountersFdbEntryDelete
(
    GT_VOID
)
{
    GT_STATUS   rc;

    rc = prvTgfBrgFdbMacEntryDelete(&(prvTgfMacEntry.key));
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in prvTgfBrgFdbMacEntrySet %d", GT_TRUE);
}

/*******************************************************************************
* prvTgfPortMacCountersVlanConfigurationSet
*
* DESCRIPTION:
*       Set Default Vlan Port MAC Counters configuration.
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
GT_VOID prvTgfPortMacCountersVlanConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;

    /* create a vlan on ports (0,0), (0,8), (0,18), (0,23) */
    rc = prvTgfBrgDefVlanEntryWrite(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d", prvTgfDevNum);
}

/*******************************************************************************
* prvTgfPortMacCountersConfigurationSet
*
* DESCRIPTION:
*       Set configuration.
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
GT_VOID prvTgfPortMacCountersConfigurationSet
(
    GT_VOID
)
{
    GT_U32          portsCount = prvTgfPortsNum;
    GT_U32          portIter   = 0;
    GT_STATUS       rc = GT_OK;

    /* Disable Clear on Read */
    for (portIter = 0; portIter < portsCount; portIter++)
    {

        rc = prvTgfPortMacCountersClearOnReadSet(prvTgfPortsArray[portIter], GT_TRUE);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersClearOnReadSet FAILED, rc = [%d]", rc);
        }

        /* clear the counters */
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        rc = prvTgfPortMacCountersClearOnReadSet(prvTgfPortsArray[portIter], GT_FALSE);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersClearOnReadSet FAILED, rc = [%d]", rc);
        }
    }
}

/*******************************************************************************
* prvTgfPortMacCountersRestore
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
GT_VOID prvTgfPortMacCountersRestore
(
    GT_VOID
)
{
    GT_U32          portsCount = prvTgfPortsNum;
    GT_U32          portIter   = 0;
    GT_STATUS       rc;

    /* ----- Enable both Rx and Tx Histogram counters ----- */
    rc = prvTgfPortMacCountersRxHistogramEnable(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_0_CNS], GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersRxHistogramEnable FAILED, rc = [%d]", rc);
    }

    rc = prvTgfPortMacCountersRxHistogramEnable(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_2_CNS], GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersRxHistogramEnable FAILED, rc = [%d]", rc);
    }

    /* Enable Clear on Read */
    for (portIter = 0; portIter < portsCount; portIter++)
    {
        rc = prvTgfPortMacCountersClearOnReadSet(prvTgfPortsArray[portIter], GT_TRUE);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersClearOnReadSet FAILED, rc = [%d]", rc);
        }

        /* Clear MAC counters */
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }
}

/*******************************************************************************
* prvTgfPortMacCounterPacketSend
*
* DESCRIPTION:
*       Function sends packet, performs trace and check expected results.
*
* INPUTS:
*       portNum       - port number
*       packetInfoPtr - (pointer to) the packet info
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
static GT_VOID prvTgfPortMacCounterPacketSend
(
    IN  GT_U8                        portNum,
    IN  TGF_PACKET_STC               *packetInfoPtr
)
{
    GT_STATUS       rc         = GT_OK;
    GT_U32          portsCount = prvTgfPortsNum;
    GT_U32          portIter   = 0;


    /* Send packet Vlan Tagged */
    tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_8100_VLAN_TAG_CNS);

    /* reset counters */
    for (portIter = 0; portIter < portsCount; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* setup Packet */
    rc = prvTgfSetTxSetupEth(prvTgfDevNum, packetInfoPtr, prvTgfBurstCount, 0, NULL);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfSetTxSetupEth: %d, %d",
                                 prvTgfDevNum, prvTgfBurstCount, 0, NULL);

    /* clear table */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");


    /* send Packet from port portNum */
    rc = prvTgfStartTransmitingEth(prvTgfDevNum, portNum);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, %d",
                                 prvTgfDevNum, portNum);
}

/*******************************************************************************
* prvTgPortMacCounterCaptureBroadcastTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send from port 0packets:
*               macDa = ff:ff:ff:ff:ff:ff,
*               macSa = 00:00:00:00:22:22,
*           Success Criteria:
*               MAC MIB Captured counters:
*               1 Broadcast Packet sent from port 0.
*               1 Broadcast Packets received on ports 8, 18 and 23.
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
GT_VOID prvTgPortMacCounterCaptureBroadcastTrafficGenerate
(
    void
)
{
    GT_STATUS                       rc = GT_OK;
    GT_U32                          portIter = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    CPSS_PORT_MAC_COUNTER_SET_STC   portMacCntrsCaptured;


    /* enable packet trace */
    rc = tgfTrafficTracePacketByteSet(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in tgfTrafficTracePacketByteSet %d", GT_TRUE);


    /* -------------------------------------------------------------------------
     * 1. Sending broadcast
     */
    PRV_UTF_LOG0_MAC("======= Sending broadcast =======\n");

    /* Set DA MAC Address to be ff:ff:ff:ff:ff:ff */
    prvTgfPacketL2Part.daMac[0] = 0xff;
    prvTgfPacketL2Part.daMac[1] = 0xff;
    prvTgfPacketL2Part.daMac[2] = 0xff;
    prvTgfPacketL2Part.daMac[3] = 0xff;
    prvTgfPacketL2Part.daMac[4] = 0xff;
    prvTgfPacketL2Part.daMac[5] = 0xff;

    /* send packets */
    prvTgfPortMacCounterPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_0_CNS], &prvTgfPacketInfo);
    cpssOsTimerWkAfter(10);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfReadPortCountersEth FAILED, rc = [%d]", rc);
        }

        /* Check Port  */
        rc = prvTgfPortMacCountersCaptureOnPortGet(prvTgfPortsArray[portIter], &portMacCntrsCaptured);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersCaptureOnPortGet FAILED, rc = [%d]", rc);
        }

        if (PRV_TGF_SEND_PORT_IDX_0_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.brdcPktsRcv.l[0],
                                         "get another captured brdcPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.ucPktsRcv.l[0],
                                         "get another captured ucPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.mcPktsRcv.l[0],
                                         "get another captured mcPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC((PRV_TGF_PACKET_CRC_LEN_CNS * prvTgfBurstCount), portMacCntrsCaptured.goodOctetsRcv.l[0],
                                         "get another captured goodOctetsRcv counter than expected");
            continue;
        }

             /* check Tx counters */
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                     "get another goodPktsSent counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.brdcPktsSent.l[0],
                                     "get another captured brdcPktsSent counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.ucPktsSent.l[0],
                                     "get another captured ucPktsSent counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.mcPktsSent.l[0],
                                     "get another captured mcPktsSent counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(((PRV_TGF_PACKET_CRC_LEN_CNS - TGF_VLAN_TAG_SIZE_CNS) * prvTgfBurstCount),
                                     portMacCntrsCaptured.goodOctetsSent.l[0],
                                     "get another captured goodOctetsSent counter than expected");
    }
}

/*******************************************************************************
* prvTgPortMacCounterCaptureUnicastTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send from port 0packets:
*               macDa = 00:00:00:00:11:11,
*               macSa = 00:00:00:00:22:22,
*           Success Criteria:
*               MAC MIB Captured counters:
*               1 Unicast Packet sent from port 18.
*               1 Unicast Packets received on ports 0, 8 and 23.
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
GT_VOID prvTgPortMacCounterCaptureUnicastTrafficGenerate
(
    void
)
{
    GT_STATUS                       rc = GT_OK;
    GT_U32                          portIter = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    CPSS_PORT_MAC_COUNTER_SET_STC   portMacCntrsCaptured;


    /* enable packet trace */
    rc = tgfTrafficTracePacketByteSet(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in tgfTrafficTracePacketByteSet %d", GT_TRUE);


    /* -------------------------------------------------------------------------
     * 1. Sending unicast
     */
    PRV_UTF_LOG0_MAC("======= Sending unicast =======\n");

    /* Set DA MAC Address to be 00:00:00:00:11:11 */
    prvTgfPacketL2Part.daMac[0] = 0x00;
    prvTgfPacketL2Part.daMac[1] = 0x00;
    prvTgfPacketL2Part.daMac[2] = 0x00;
    prvTgfPacketL2Part.daMac[3] = 0x00;
    prvTgfPacketL2Part.daMac[4] = 0x11;
    prvTgfPacketL2Part.daMac[5] = 0x11;

    /* send packets */
    prvTgfPortMacCounterPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_2_CNS], &prvTgfPacketInfo);
    cpssOsTimerWkAfter(10);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfReadPortCountersEth FAILED, rc = [%d]", rc);
        }

        /* Check Port  */
        rc = prvTgfPortMacCountersCaptureOnPortGet(prvTgfPortsArray[portIter], &portMacCntrsCaptured);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersCaptureOnPortGet FAILED, rc = [%d]", rc);
        }

        if (PRV_TGF_SEND_PORT_IDX_2_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.ucPktsRcv.l[0],
                                         "get another captured ucPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.brdcPktsRcv.l[0],
                                         "get another captured brdcPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.mcPktsRcv.l[0],
                                         "get another captured mcPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC((PRV_TGF_PACKET_CRC_LEN_CNS * prvTgfBurstCount), portMacCntrsCaptured.goodOctetsRcv.l[0],
                                         "get another captured goodOctetsRcv counter than expected");
            continue;
        }

             /* check Tx counters */
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                     "get another goodPktsSent counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.ucPktsSent.l[0],
                                     "get another captured ucPktsSent counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.brdcPktsSent.l[0],
                                     "get another captured brdcPktsSent counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.mcPktsSent.l[0],
                                     "get another captured mcPktsSent counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(((PRV_TGF_PACKET_CRC_LEN_CNS - TGF_VLAN_TAG_SIZE_CNS) * prvTgfBurstCount),
                                     portMacCntrsCaptured.goodOctetsSent.l[0],
                                     "get another captured goodOctetsSent counter than expected");
    }
}

/*******************************************************************************
* prvTgPortMacCounterCaptureMulticastTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send from port 0packets:
*               macDa = 01:02:03:04:05:06,
*               macSa = 00:00:00:00:22:22,
*           Success Criteria:
*               MAC MIB Captured counters:
*               1 Multicast Packet sent from port 0.
*               1 Multicast Packets received on ports 8, 18 and 23.
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
GT_VOID prvTgPortMacCounterCaptureMulticastTrafficGenerate
(
    void
)
{
    GT_STATUS                       rc = GT_OK;
    GT_U32                          portIter = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    CPSS_PORT_MAC_COUNTER_SET_STC   portMacCntrsCaptured;
    GT_U16                          vidx;
    CPSS_PORTS_BMP_STC              portBitmap;


    /* enable packet trace */
    rc = tgfTrafficTracePacketByteSet(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in tgfTrafficTracePacketByteSet %d", GT_TRUE);

    vidx = 1;

    /* clear entry */
    cpssOsMemSet(&portBitmap, 0, sizeof(portBitmap));

    /* ports 0 and 18 */
    portBitmap.ports[prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_0_CNS] >> 5] |=
        1 << (prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_0_CNS] & 0x1f);
    portBitmap.ports[prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_2_CNS] >> 5] |=
        1 << (prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_2_CNS] & 0x1f);

    /* Create MC group */
    rc = prvTgfBrgMcEntryWrite(prvTgfDevNum, vidx, &portBitmap);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in prvTgfBrgMcEntryWrite %d", GT_TRUE);

    /* configure FDB entry */
    prvTgfPortMacCountersFdbConfigurationSet();

    /* -------------------------------------------------------------------------
     * 1. Sending multicast
     */
    PRV_UTF_LOG0_MAC("======= Sending multicast =======\n");

    /* Set DA MAC Address to be 01:02:03:04:05:06 */
    prvTgfPacketL2Part.daMac[0] = 0x01;
    prvTgfPacketL2Part.daMac[1] = 0x02;
    prvTgfPacketL2Part.daMac[2] = 0x03;
    prvTgfPacketL2Part.daMac[3] = 0x04;
    prvTgfPacketL2Part.daMac[4] = 0x05;
    prvTgfPacketL2Part.daMac[5] = 0x06;

    /* send packets */
    prvTgfPortMacCounterPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_0_CNS], &prvTgfPacketInfo);
    cpssOsTimerWkAfter(10);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfReadPortCountersEth FAILED, rc = [%d]", rc);
        }

        /* Check Port  */
        rc = prvTgfPortMacCountersCaptureOnPortGet(prvTgfPortsArray[portIter], &portMacCntrsCaptured);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersCaptureOnPortGet FAILED, rc = [%d]", rc);
        }

        if (PRV_TGF_SEND_PORT_IDX_0_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.mcPktsRcv.l[0],
                                         "get another captured mcPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.brdcPktsRcv.l[0],
                                         "get another captured brdcPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.ucPktsRcv.l[0],
                                         "get another captured ucPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC((PRV_TGF_PACKET_CRC_LEN_CNS * prvTgfBurstCount), portMacCntrsCaptured.goodOctetsRcv.l[0],
                                         "get another captured goodOctetsRcv counter than expected");
            continue;
        }

        if (PRV_TGF_SEND_PORT_IDX_2_CNS == portIter)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                     "get another goodPktsSent counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.mcPktsSent.l[0],
                                     "get another captured mcPktsSent counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.brdcPktsSent.l[0],
                                         "get another captured brdcPktsSent counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.ucPktsSent.l[0],
                                         "get another captured ucPktsSent counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(((PRV_TGF_PACKET_CRC_LEN_CNS - TGF_VLAN_TAG_SIZE_CNS) * prvTgfBurstCount),
                                         portMacCntrsCaptured.goodOctetsSent.l[0],
                                         "get another captured goodOctetsSent counter than expected");
        }
    }

    /* Invalidate MC group */
    portBitmap.ports[0] = 0x0;
    portBitmap.ports[1] = 0x0;

    rc = prvTgfBrgMcEntryWrite(prvTgfDevNum, vidx, &portBitmap);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in prvTgfBrgMcEntryWrite %d", GT_TRUE);

    /* Delete FDB entry */
    prvTgfPortMacCountersFdbEntryDelete();
}

/*******************************************************************************
* prvTgPortMacCounterCaptureOversizeTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send from port 0 packets:
*               macDa = 00:00:00:00:11:11,
*               macSa = 00:00:00:00:22:22,
*           Success Criteria:
*               MAC MIB Captured counters:
*               1 Unicast Packet sent from port 0.
*               1 Oversize Packets received on ports 8, 18 and 23.
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
GT_VOID prvTgPortMacCounterCaptureOversizeTrafficGenerate
(
    void
)
{
    GT_STATUS                       rc = GT_OK;
    GT_U32                          portIter = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    CPSS_PORT_MAC_COUNTER_SET_STC   portMacCntrsCaptured;
    GT_U32                          defaultMru;
    GT_U32                          mruSize;

    rc = prvTgfPortMruGet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_0_CNS], &defaultMru);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in prvTgfPortMruGet %d", GT_TRUE);

    /* Set Rx Port MRU to 100 bytes */
    mruSize = 100;
    rc = prvTgfPortMruSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_0_CNS], mruSize);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in prvTgfPortMruSet %d", GT_TRUE);

    /* enable packet trace */
    rc = tgfTrafficTracePacketByteSet(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in tgfTrafficTracePacketByteSet %d", GT_TRUE);


    /* -------------------------------------------------------------------------
     * 1. Sending unicast oversize
     *    packet size must be bigger then configured MRU .
     */
    PRV_UTF_LOG0_MAC("======= Sending unicast oversize packet =======\n");

    /* Set PayLoad length to 200 bytes */
    cpssOsMemSet(prvTgfPayloadData200BytesArr, 0x0, sizeof(prvTgfPayloadData200BytesArr));
    prvTgfPacketPayloadPart.dataLength = sizeof(prvTgfPayloadData200BytesArr);
    prvTgfPacketPayloadPart.dataPtr = prvTgfPayloadData200BytesArr;
    prvTgfPacketInfo.totalLen =
            TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + prvTgfPacketPayloadPart.dataLength;

    /* Set DA MAC Address to be 00:00:00:00:11:11 */
    prvTgfPacketL2Part.daMac[0] = 0x00;
    prvTgfPacketL2Part.daMac[1] = 0x00;
    prvTgfPacketL2Part.daMac[2] = 0x00;
    prvTgfPacketL2Part.daMac[3] = 0x00;
    prvTgfPacketL2Part.daMac[4] = 0x11;
    prvTgfPacketL2Part.daMac[5] = 0x11;

    /* send packets */
    prvTgfPortMacCounterPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_0_CNS], &prvTgfPacketInfo);
    cpssOsTimerWkAfter(10);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfReadPortCountersEth FAILED, rc = [%d]", rc);
        }

        /* Check Port  */
        rc = prvTgfPortMacCountersCaptureOnPortGet(prvTgfPortsArray[portIter], &portMacCntrsCaptured);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersCaptureOnPortGet FAILED, rc = [%d]", rc);
        }

        if (PRV_TGF_SEND_PORT_IDX_0_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.oversizePkts.l[0],
                                         "get another oversizePkts counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(mruSize, portMacCntrsCaptured.badOctetsRcv.l[0],
                                         "get another badOctetsRcv counter than expected");
            continue;
        }

             /* check Tx counters */
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.goodPktsSent.l[0],
                                     "get another goodPktsSent counter than expected");
    }

    /* set back the packet payload part and size */
    prvTgfPacketPayloadPart.dataLength = sizeof(prvTgfPayloadDataArr);
    prvTgfPacketPayloadPart.dataPtr = prvTgfPayloadDataArr;
    prvTgfPacketInfo.totalLen =
            TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + prvTgfPacketPayloadPart.dataLength;


    /* set back default MRU */
    rc = prvTgfPortMruSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_0_CNS], defaultMru);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in prvTgfPortMruSet %d", GT_TRUE);
}


/*******************************************************************************
* prvTgPortMacCounterCaptureHistogramTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send from port 0packets:
*               macDa = 01:02:03:04:05:06,
*               macSa = 00:00:00:00:22:22,
*           Success Criteria:
*               Histogram Captured counters:
*               Check, that when the feature is disabled, the counters are not updated.
*               Check, that when the feature is enabled, the counters are updated.
*               Check for different packet sizes:
*                   packetSize = 64.
*                   64<packetSize<128
*                   127<packetSize<256
*                   255<packetSize<512
*                   511<packetSize<1024
*                   1023<packetSize
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
GT_VOID prvTgPortMacCounterCaptureHistogramTrafficGenerate
(
    void
)
{
    GT_STATUS                       rc = GT_OK;
    GT_U32                          portIter = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    CPSS_PORT_MAC_COUNTER_SET_STC   portMacCntrsCaptured;
    GT_U32                          burstCount;


    /* enable packet trace */
    rc = tgfTrafficTracePacketByteSet(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in tgfTrafficTracePacketByteSet %d", GT_TRUE);


    /* ----- Disable both Rx and Tx Histogram counters ----- */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfPortMacCountersRxHistogramEnable(prvTgfPortsArray[portIter], GT_FALSE);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersRxHistogramEnable FAILED, rc = [%d]", rc);
        }

        rc = prvTgfPortMacCountersTxHistogramEnable(prvTgfPortsArray[portIter], GT_FALSE);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersTxHistogramEnable FAILED, rc = [%d]", rc);
        }
    }


    /* -------------------------------------------------------------------------
     * 1. Sending unicast packet, Rx/Tx Histogram disabled
     *    Packet size is 64 bytes
     */
    PRV_UTF_LOG0_MAC("======= Sending unicast packet (packetSize = 64) =======\n");
    PRV_UTF_LOG0_MAC("======= Rx/Tx Histogram disabled =======\n");

    /* Set DA MAC Address to be 00:00:00:00:11:11 */
    prvTgfPacketL2Part.daMac[0] = 0x00;
    prvTgfPacketL2Part.daMac[1] = 0x00;
    prvTgfPacketL2Part.daMac[2] = 0x00;
    prvTgfPacketL2Part.daMac[3] = 0x00;
    prvTgfPacketL2Part.daMac[4] = 0x11;
    prvTgfPacketL2Part.daMac[5] = 0x11;

    /* send packets */
    prvTgfPortMacCounterPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_0_CNS], &prvTgfPacketInfo);
    cpssOsTimerWkAfter(10);


    burstCount = prvTgfBurstCount;

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfReadPortCountersEth FAILED, rc = [%d]", rc);
        }

        /* Check Port  */
        rc = prvTgfPortMacCountersCaptureOnPortGet(prvTgfPortsArray[portIter], &portMacCntrsCaptured);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersCaptureOnPortGet FAILED, rc = [%d]", rc);
        }

        /* Check that Hystogram counters counted nothing */
        if (PRV_TGF_SEND_PORT_IDX_0_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts64Octets.l[0],
                                         "get another captured pkts64Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts65to127Octets.l[0],
                                         "get another captured pkts65to127Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts128to255Octets.l[0],
                                         "get another captured pkts128to255Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts256to511Octets.l[0],
                                         "get another captured pkts256to511Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts512to1023Octets.l[0],
                                         "get another captured pkts512to1023Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts1024tomaxOoctets.l[0],
                                         "get another captured pkts1024tomaxOoctets counter than expected");
            continue;
        }

        /* check Tx counters */
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                     "get another goodPktsSent counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts64Octets.l[0],
                                     "get another captured pkts64Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts65to127Octets.l[0],
                                     "get another captured pkts65to127Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts128to255Octets.l[0],
                                     "get another captured pkts128to255Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts256to511Octets.l[0],
                                     "get another captured pkts256to511Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts512to1023Octets.l[0],
                                     "get another captured pkts512to1023Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts1024tomaxOoctets.l[0],
                                     "get another captured pkts1024tomaxOoctets counter than expected");
    }

    /* Pay attantion: as the counters are not cleared on read,
       their values are accumulated */


    /* ------------- Enable Rx Histogram --------------- */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfPortMacCountersRxHistogramEnable(prvTgfPortsArray[portIter], GT_TRUE);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersRxHistogramEnable FAILED, rc = [%d]", rc);
        }
    }

    /* -------------------------------------------------------------------------
     * 2. Sending unicast packet, Rx Histogram enabled and Tx Histogram disabled
     *    Packet size is 64 bytes
     */
    PRV_UTF_LOG0_MAC("======= Sending unicast packet (packetSize = 64) =======\n");
    PRV_UTF_LOG0_MAC("==== Rx Histogram enabled and Tx Histogram disabled ====\n");

    /* send packets */
    prvTgfPortMacCounterPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_0_CNS], &prvTgfPacketInfo);
    cpssOsTimerWkAfter(10);


    burstCount ++;

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfReadPortCountersEth FAILED, rc = [%d]", rc);
        }

        /* Check Port  */
        rc = prvTgfPortMacCountersCaptureOnPortGet(prvTgfPortsArray[portIter], &portMacCntrsCaptured);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersCaptureOnPortGet FAILED, rc = [%d]", rc);
        }

        /* Check that Rx Hystogram counters counted  1 packet and
           Tx Hystogram counters counted nothing */
        if (PRV_TGF_SEND_PORT_IDX_0_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(burstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts64Octets.l[0],
                                         "get another captured pkts64Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts65to127Octets.l[0],
                                         "get another captured pkts65to127Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts128to255Octets.l[0],
                                         "get another captured pkts128to255Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts256to511Octets.l[0],
                                         "get another captured pkts256to511Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts512to1023Octets.l[0],
                                         "get another captured pkts512to1023Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts1024tomaxOoctets.l[0],
                                         "get another captured pkts1024tomaxOoctets counter than expected");
            continue;
        }

             /* check Tx counters */
        UTF_VERIFY_EQUAL0_STRING_MAC(burstCount, portCntrs.goodPktsSent.l[0],
                                     "get another goodPktsSent counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts64Octets.l[0],
                                     "get another captured pkts64Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts65to127Octets.l[0],
                                     "get another captured pkts65to127Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts128to255Octets.l[0],
                                     "get another captured pkts128to255Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts256to511Octets.l[0],
                                     "get another captured pkts256to511Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts512to1023Octets.l[0],
                                     "get another captured pkts512to1023Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts1024tomaxOoctets.l[0],
                                     "get another captured pkts1024tomaxOoctets counter than expected");
    }


    /* ---------- Enable Tx Histogram ---------- */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfPortMacCountersTxHistogramEnable(prvTgfPortsArray[portIter], GT_TRUE);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersRxHistogramEnable FAILED, rc = [%d]", rc);
        }
    }

    /* -------------------------------------------------------------------------
     * 3. Sending unicast packet, Rx/Tx Histogram enabled
     *    Packet size is 64 bytes
     */
    PRV_UTF_LOG0_MAC("======= Sending unicast packet (packetSize = 64) =======\n");
    PRV_UTF_LOG0_MAC("======= Rx/Tx Histogram enabled  ========\n");

    /* send packets */
    prvTgfPortMacCounterPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_0_CNS], &prvTgfPacketInfo);
    cpssOsTimerWkAfter(10);

    burstCount ++;

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfReadPortCountersEth FAILED, rc = [%d]", rc);
        }

        /* Check Port  */
        rc = prvTgfPortMacCountersCaptureOnPortGet(prvTgfPortsArray[portIter], &portMacCntrsCaptured);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersCaptureOnPortGet FAILED, rc = [%d]", rc);
        }

        /* Check that Rx and Tx Hystogram counters counted  1 packet */
        if (PRV_TGF_SEND_PORT_IDX_0_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(burstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts64Octets.l[0],
                                         "get another captured pkts64Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(burstCount, portMacCntrsCaptured.pkts65to127Octets.l[0],
                                         "get another captured pkts65to127Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts128to255Octets.l[0],
                                         "get another captured pkts128to255Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts256to511Octets.l[0],
                                         "get another captured pkts256to511Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts512to1023Octets.l[0],
                                         "get another captured pkts512to1023Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts1024tomaxOoctets.l[0],
                                         "get another captured pkts1024tomaxOoctets counter than expected");
            continue;
        }

        /* check Tx counters */
        UTF_VERIFY_EQUAL0_STRING_MAC(burstCount, portCntrs.goodPktsSent.l[0],
                                     "get another goodPktsSent counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts64Octets.l[0],
                                     "get another captured pkts64Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts65to127Octets.l[0],
                                     "get another captured pkts65to127Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts128to255Octets.l[0],
                                     "get another captured pkts128to255Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts256to511Octets.l[0],
                                     "get another captured pkts256to511Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts512to1023Octets.l[0],
                                     "get another captured pkts512to1023Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts1024tomaxOoctets.l[0],
                                     "get another captured pkts1024tomaxOoctets counter than expected");
    }


    /* -------------------------------------------------------------------------
     * 4. Sending unicast packets with  64<packetSize<128
     */
    PRV_UTF_LOG0_MAC("======= Sending unicast packet 64<packetSize<128 =======\n");

    /* Set PayLoad length to 80 bytes */
    cpssOsMemSet(prvTgfPayloadData80BytesArr, 0x0, sizeof(prvTgfPayloadData80BytesArr));
    prvTgfPacketPayloadPart.dataLength = sizeof(prvTgfPayloadData80BytesArr);
    prvTgfPacketPayloadPart.dataPtr = prvTgfPayloadData80BytesArr;
    prvTgfPacketInfo.totalLen =
            TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + prvTgfPacketPayloadPart.dataLength;

    /* send packets */
    prvTgfPortMacCounterPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_0_CNS], &prvTgfPacketInfo);
    cpssOsTimerWkAfter(10);

    burstCount ++;

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfReadPortCountersEth FAILED, rc = [%d]", rc);
        }

        /* Check Port  */
        rc = prvTgfPortMacCountersCaptureOnPortGet(prvTgfPortsArray[portIter], &portMacCntrsCaptured);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersCaptureOnPortGet FAILED, rc = [%d]", rc);
        }

        if (PRV_TGF_SEND_PORT_IDX_0_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(burstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC((prvTgfBurstCount * 2) + 3, portMacCntrsCaptured.pkts65to127Octets.l[0],
                                         "get another captured pkts65to127Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts64Octets.l[0],
                                         "get another captured pkts64Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts128to255Octets.l[0],
                                         "get another captured pkts128to255Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts256to511Octets.l[0],
                                         "get another captured pkts256to511Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts512to1023Octets.l[0],
                                         "get another captured pkts512to1023Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts1024tomaxOoctets.l[0],
                                         "get another captured pkts1024tomaxOoctets counter than expected");
            continue;
        }

             /* check Tx counters */
        UTF_VERIFY_EQUAL0_STRING_MAC(burstCount, portCntrs.goodPktsSent.l[0],
                                     "get another goodPktsSent counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts65to127Octets.l[0],
                                     "get another captured pkts65to127Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts64Octets.l[0],
                                     "get another captured pkts64Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts128to255Octets.l[0],
                                     "get another captured pkts128to255Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts256to511Octets.l[0],
                                     "get another captured pkts256to511Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts512to1023Octets.l[0],
                                     "get another captured pkts512to1023Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts1024tomaxOoctets.l[0],
                                     "get another captured pkts1024tomaxOoctets counter than expected");
    }

    /* -------------------------------------------------------------------------
     * 5. Sending unicast packets with  127<packetSize<256
     */
    PRV_UTF_LOG0_MAC("======= Sending unicast packet 127<packetSize<256 =======\n");

    /* Set PayLoad length to 200 bytes */
    cpssOsMemSet(prvTgfPayloadData200BytesArr, 0x0, sizeof(prvTgfPayloadData200BytesArr));
    prvTgfPacketPayloadPart.dataLength = sizeof(prvTgfPayloadData200BytesArr);
    prvTgfPacketPayloadPart.dataPtr = prvTgfPayloadData200BytesArr;
    prvTgfPacketInfo.totalLen =
            TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + prvTgfPacketPayloadPart.dataLength;

    /* send packets */
    prvTgfPortMacCounterPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_0_CNS], &prvTgfPacketInfo);
    cpssOsTimerWkAfter(10);

    burstCount ++;

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfReadPortCountersEth FAILED, rc = [%d]", rc);
        }

        /* Check Port  */
        rc = prvTgfPortMacCountersCaptureOnPortGet(prvTgfPortsArray[portIter], &portMacCntrsCaptured);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersCaptureOnPortGet FAILED, rc = [%d]", rc);
        }

        if (PRV_TGF_SEND_PORT_IDX_0_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(burstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC((prvTgfBurstCount * 2), portMacCntrsCaptured.pkts128to255Octets.l[0],
                                         "get another captured pkts128to255Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts64Octets.l[0],
                                         "get another captured pkts64Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC((prvTgfBurstCount * 2) + 3, portMacCntrsCaptured.pkts65to127Octets.l[0],
                                         "get another captured pkts65to127Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts256to511Octets.l[0],
                                         "get another captured pkts256to511Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts512to1023Octets.l[0],
                                         "get another captured pkts512to1023Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts1024tomaxOoctets.l[0],
                                         "get another captured pkts1024tomaxOoctets counter than expected");
            continue;
        }

             /* check Tx counters */
        UTF_VERIFY_EQUAL0_STRING_MAC(burstCount, portCntrs.goodPktsSent.l[0],
                                     "get another goodPktsSent counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts128to255Octets.l[0],
                                     "get another captured pkts128to255Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts64Octets.l[0],
                                     "get another captured pkts64Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts65to127Octets.l[0],
                                     "get another captured pkts65to127Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts256to511Octets.l[0],
                                     "get another captured pkts256to511Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts512to1023Octets.l[0],
                                     "get another captured pkts512to1023Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts1024tomaxOoctets.l[0],
                                     "get another captured pkts1024tomaxOoctets counter than expected");
    }

    /* -------------------------------------------------------------------------
     * 6. Sending unicast packets with  255<packetSize<512
     */
    PRV_UTF_LOG0_MAC("======= Sending unicast packet 255<packetSize<512 =======\n");

    /* Set PayLoad length to 300 bytes */
    cpssOsMemSet(prvTgfPayloadData300BytesArr, 0x0, sizeof(prvTgfPayloadData300BytesArr));
    prvTgfPacketPayloadPart.dataLength = sizeof(prvTgfPayloadData300BytesArr);
    prvTgfPacketPayloadPart.dataPtr = prvTgfPayloadData300BytesArr;
    prvTgfPacketInfo.totalLen =
            TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + prvTgfPacketPayloadPart.dataLength;

    /* send packets */
    prvTgfPortMacCounterPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_0_CNS], &prvTgfPacketInfo);
    cpssOsTimerWkAfter(10);

    burstCount++;

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfReadPortCountersEth FAILED, rc = [%d]", rc);
        }

        /* Check Port  */
        rc = prvTgfPortMacCountersCaptureOnPortGet(prvTgfPortsArray[portIter], &portMacCntrsCaptured);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersCaptureOnPortGet FAILED, rc = [%d]", rc);
        }

        if (PRV_TGF_SEND_PORT_IDX_0_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(burstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC((prvTgfBurstCount * 2), portMacCntrsCaptured.pkts256to511Octets.l[0],
                                         "get another captured pkts256to511Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts64Octets.l[0],
                                         "get another captured pkts64Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC((prvTgfBurstCount * 2) + 3, portMacCntrsCaptured.pkts65to127Octets.l[0],
                                         "get another captured pkts65to127Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC((prvTgfBurstCount * 2), portMacCntrsCaptured.pkts128to255Octets.l[0],
                                         "get another captured pkts128to255Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts512to1023Octets.l[0],
                                         "get another captured pkts512to1023Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts1024tomaxOoctets.l[0],
                                         "get another captured pkts1024tomaxOoctets counter than expected");
            continue;
        }

             /* check Tx counters */
        UTF_VERIFY_EQUAL0_STRING_MAC(burstCount, portCntrs.goodPktsSent.l[0],
                                     "get another goodPktsSent counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts256to511Octets.l[0],
                                     "get another captured pkts256to511Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts64Octets.l[0],
                                     "get another captured pkts64Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts65to127Octets.l[0],
                                     "get another captured pkts65to127Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts128to255Octets.l[0],
                                     "get another captured pkts128to255Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts512to1023Octets.l[0],
                                     "get another captured pkts512to1023Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts1024tomaxOoctets.l[0],
                                     "get another captured pkts1024tomaxOoctets counter than expected");
    }

    /* -------------------------------------------------------------------------
     * 7. Sending unicast packets with  511<packetSize<1024
     */
    PRV_UTF_LOG0_MAC("======= Sending unicast packet 511<packetSize<1024 =======\n");

    /* Set PayLoad length to 300 bytes */
    cpssOsMemSet(prvTgfPayloadData500BytesArr, 0x0, sizeof(prvTgfPayloadData500BytesArr));
    prvTgfPacketPayloadPart.dataLength = sizeof(prvTgfPayloadData500BytesArr);
    prvTgfPacketPayloadPart.dataPtr = prvTgfPayloadData500BytesArr;
    prvTgfPacketInfo.totalLen =
            TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + prvTgfPacketPayloadPart.dataLength;

    /* send packets */
    prvTgfPortMacCounterPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_0_CNS], &prvTgfPacketInfo);
    cpssOsTimerWkAfter(10);

    burstCount++;

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfReadPortCountersEth FAILED, rc = [%d]", rc);
        }

        /* Check Port  */
        rc = prvTgfPortMacCountersCaptureOnPortGet(prvTgfPortsArray[portIter], &portMacCntrsCaptured);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersCaptureOnPortGet FAILED, rc = [%d]", rc);
        }

        if (PRV_TGF_SEND_PORT_IDX_0_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(burstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC((prvTgfBurstCount * 2),
                                         portMacCntrsCaptured.pkts512to1023Octets.l[0],
                                         "get another captured pkts512to1023Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts64Octets.l[0],
                                         "get another captured pkts64Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC((prvTgfBurstCount * 2) + 3, portMacCntrsCaptured.pkts65to127Octets.l[0],
                                         "get another captured pkts65to127Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC((prvTgfBurstCount * 2), portMacCntrsCaptured.pkts128to255Octets.l[0],
                                         "get another captured pkts128to255Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC((prvTgfBurstCount * 2), portMacCntrsCaptured.pkts256to511Octets.l[0],
                                         "get another captured pkts256to511Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts1024tomaxOoctets.l[0],
                                         "get another captured pkts1024tomaxOoctets counter than expected");
            continue;
        }

             /* check Tx counters */
        UTF_VERIFY_EQUAL0_STRING_MAC(burstCount, portCntrs.goodPktsSent.l[0],
                                     "get another goodPktsSent counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts512to1023Octets.l[0],
                                     "get another captured pkts512to1023Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts64Octets.l[0],
                                     "get another captured pkts64Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts65to127Octets.l[0],
                                     "get another captured pkts65to127Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts128to255Octets.l[0],
                                     "get another captured pkts128to255Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts256to511Octets.l[0],
                                     "get another captured pkts256to511Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts1024tomaxOoctets.l[0],
                                     "get another captured pkts1024tomaxOoctets counter than expected");
    }


    /* -------------------------------------------------------------------------
     * 8. Sending unicast packets with  1023<packetSize
     */
    PRV_UTF_LOG0_MAC("======= Sending unicast packet 1023<packetSize  =======\n");

    /* Set PayLoad length to 300 bytes */
    cpssOsMemSet(prvTgfPayloadData1024BytesArr, 0x0, sizeof(prvTgfPayloadData1024BytesArr));
    prvTgfPacketPayloadPart.dataLength = sizeof(prvTgfPayloadData1024BytesArr);
    prvTgfPacketPayloadPart.dataPtr = prvTgfPayloadData1024BytesArr;
    prvTgfPacketInfo.totalLen =
            TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + prvTgfPacketPayloadPart.dataLength;

    /* send packets */
    prvTgfPortMacCounterPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_0_CNS], &prvTgfPacketInfo);
    cpssOsTimerWkAfter(10);

    burstCount++;

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfReadPortCountersEth FAILED, rc = [%d]", rc);
        }

        /* Check Port  */
        rc = prvTgfPortMacCountersCaptureOnPortGet(prvTgfPortsArray[portIter], &portMacCntrsCaptured);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersCaptureOnPortGet FAILED, rc = [%d]", rc);
        }

        if (PRV_TGF_SEND_PORT_IDX_0_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(burstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC((prvTgfBurstCount * 2),
                                         portMacCntrsCaptured.pkts1024tomaxOoctets.l[0],
                                         "get another captured pkts1024tomaxOoctets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portMacCntrsCaptured.pkts64Octets.l[0],
                                         "get another captured pkts64Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC((prvTgfBurstCount * 2) + 3, portMacCntrsCaptured.pkts65to127Octets.l[0],
                                         "get another captured pkts65to127Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC((prvTgfBurstCount * 2), portMacCntrsCaptured.pkts128to255Octets.l[0],
                                         "get another captured pkts128to255Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC((prvTgfBurstCount * 2), portMacCntrsCaptured.pkts256to511Octets.l[0],
                                         "get another captured pkts256to511Octets counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC((prvTgfBurstCount * 2), portMacCntrsCaptured.pkts512to1023Octets.l[0],
                                         "get another captured pkts512to1023Octets counter than expected");
            continue;
        }

             /* check Tx counters */
        UTF_VERIFY_EQUAL0_STRING_MAC(burstCount, portCntrs.goodPktsSent.l[0],
                                     "get another goodPktsSent counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts1024tomaxOoctets.l[0],
                                     "get another captured pkts1024tomaxOoctets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts64Octets.l[0],
                                     "get another captured pkts64Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts65to127Octets.l[0],
                                     "get another captured pkts65to127Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts128to255Octets.l[0],
                                     "get another captured pkts128to255Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts256to511Octets.l[0],
                                     "get another captured pkts256to511Octets counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portMacCntrsCaptured.pkts512to1023Octets.l[0],
                                     "get another captured pkts512to1023Octets counter than expected");


        /* return the packet data and size to previous one */
        prvTgfPacketPayloadPart.dataLength = sizeof(prvTgfPayloadDataArr);
        prvTgfPacketPayloadPart.dataPtr = prvTgfPayloadDataArr;
        prvTgfPacketInfo.totalLen =
                TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + prvTgfPacketPayloadPart.dataLength;
    }
}


/*******************************************************************************
* prvTgPortMacCounterClearOnReadTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send from port 0 packets:
*               macDa = 00:00:00:00:11:11,
*               macSa = 00:00:00:00:22:22,
*           Success Criteria:
*               MAC MIB counters:
*               Check that when the feature enabled, counters are cleared on read.
*               Check that when the feature disabled, counters are not cleared on read.
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
GT_VOID prvTgPortMacCounterClearOnReadTrafficGenerate
(
    void
)
{
    GT_STATUS                       rc = GT_OK;
    GT_U32                          portIter = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    GT_U32                          burstCount;


    /* enable packet trace */
    rc = tgfTrafficTracePacketByteSet(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in tgfTrafficTracePacketByteSet %d", GT_TRUE);

    /* -------------------------------------------------------------------------
     * 1. Sending unicast
     */
    PRV_UTF_LOG0_MAC("======= Sending unicast =======\n");

    /* Set DA MAC Address to be 00:00:00:00:11:11 */
    prvTgfPacketL2Part.daMac[0] = 0x00;
    prvTgfPacketL2Part.daMac[1] = 0x00;
    prvTgfPacketL2Part.daMac[2] = 0x00;
    prvTgfPacketL2Part.daMac[3] = 0x00;
    prvTgfPacketL2Part.daMac[4] = 0x11;
    prvTgfPacketL2Part.daMac[5] = 0x11;

    /* send packets */
    prvTgfPortMacCounterPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_2_CNS], &prvTgfPacketInfo);
    cpssOsTimerWkAfter(10);

    burstCount = prvTgfBurstCount;

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* Disable Clear on Read */
        rc = prvTgfPortMacCountersClearOnReadSet(prvTgfPortsArray[portIter], GT_FALSE);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersClearOnReadSet FAILED, rc = [%d]", rc);
        }

        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfReadPortCountersEth FAILED, rc = [%d]", rc);
        }

        if (PRV_TGF_SEND_PORT_IDX_2_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.ucPktsRcv.l[0],
                                         "get another captured ucPktsRcv counter than expected");
        }
        else
        {
             /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.ucPktsSent.l[0],
                                         "get another captured ucPktsSent counter than expected");
        }


        /* read the counters again, check that they were not cleared */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfReadPortCountersEth FAILED, rc = [%d]", rc);
        }

        if (PRV_TGF_SEND_PORT_IDX_2_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.ucPktsRcv.l[0],
                                         "get another captured ucPktsRcv counter than expected");
        }
        else
        {
             /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.ucPktsSent.l[0],
                                         "get another captured ucPktsSent counter than expected");
        }
    }

    /* send packets */
    prvTgfPortMacCounterPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_2_CNS], &prvTgfPacketInfo);
    cpssOsTimerWkAfter(10);

    burstCount ++;

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* Enable Clear on Read */
        rc = prvTgfPortMacCountersClearOnReadSet(prvTgfPortsArray[portIter], GT_TRUE);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortMacCountersClearOnReadSet FAILED, rc = [%d]", rc);
        }

        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfReadPortCountersEth FAILED, rc = [%d]", rc);
        }

        if (PRV_TGF_SEND_PORT_IDX_2_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(burstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(burstCount, portCntrs.ucPktsRcv.l[0],
                                         "get another captured ucPktsRcv counter than expected");
        }
        else
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(burstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(burstCount, portCntrs.ucPktsSent.l[0],
                                         "get another captured ucPktsSent counter than expected");
        }

        /* read the counters again, check that they were cleared */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfReadPortCountersEth FAILED, rc = [%d]", rc);
        }

        if (PRV_TGF_SEND_PORT_IDX_2_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.ucPktsRcv.l[0],
                                         "get another captured ucPktsRcv counter than expected");
        }
        else
        {
             /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.ucPktsSent.l[0],
                                         "get another captured ucPktsSent counter than expected");
        }
    }
}
