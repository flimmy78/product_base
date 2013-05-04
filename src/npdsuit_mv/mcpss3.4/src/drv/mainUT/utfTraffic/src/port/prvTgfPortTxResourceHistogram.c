/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfPortTxResourceHistogram.c
*
* DESCRIPTION:
*       Resource Histigram Counters features testing
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

#include <common/tgfCommon.h>
#include <common/tgfPortGen.h>
#include <common/tgfBridgeGen.h>

/* default VLAN Id */
#define PRV_TGF_VLANID_CNS            2

/* ether type used for "vlan tag" */
#define TGF_ETHERTYPE_8100_VLAN_TAG_CNS 0x8100

/* port number to receive traffic from */
#define PRV_TGF_RECIEVE_PORT_IDX_0_CNS  0

/* port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_2_CNS     2

/* default number of packets to send */
static GT_U32   prvTgfBurstCount   = 100;

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x11},               /* dstMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x22}                /* srcMac */
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
    0x00, 0x00, 0x55, 0x55};

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

/* FDB MAC entry */
static PRV_TGF_BRG_MAC_ENTRY_STC prvTgfMacEntry;

/*******************************************************************************
* prvTgfPortTxResourceHistogramFdbConfigurationSet
*
* DESCRIPTION:
*       Set FDB configuration.
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
static GT_VOID prvTgfPortTxResourceHistogramFdbConfigurationSet
(
    GT_VOID    
)
{
    GT_STATUS                   rc;                 /* return code */

    /* clear entry */
    cpssOsMemSet(&prvTgfMacEntry, 0, sizeof(prvTgfMacEntry)); 

    prvTgfMacEntry.key.entryType                = PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E;
    prvTgfMacEntry.key.key.macVlan.vlanId       = PRV_TGF_VLANID_CNS;
    prvTgfMacEntry.dstInterface.type            = CPSS_INTERFACE_PORT_E;
    prvTgfMacEntry.dstInterface.devPort.devNum  = prvTgfDevNum;
    prvTgfMacEntry.dstInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_RECIEVE_PORT_IDX_0_CNS];
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

    prvTgfMacEntry.key.key.macVlan.macAddr.arEther[0] = 0x0;
    prvTgfMacEntry.key.key.macVlan.macAddr.arEther[1] = 0x0;
    prvTgfMacEntry.key.key.macVlan.macAddr.arEther[2] = 0x0;
    prvTgfMacEntry.key.key.macVlan.macAddr.arEther[3] = 0x0;
    prvTgfMacEntry.key.key.macVlan.macAddr.arEther[4] = 0x0;
    prvTgfMacEntry.key.key.macVlan.macAddr.arEther[5] = 0x11;

    /* 00:00:00:00:00:11 on port 0 */
    rc = prvTgfBrgFdbMacEntrySet(&prvTgfMacEntry);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in prvTgfBrgFdbMacEntrySet %d", GT_TRUE);
}

/*******************************************************************************
* prvTgfPortTxResourceHistogramFdbEntryDelete
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
static GT_VOID prvTgfPortTxResourceHistogramFdbEntryDelete
(
    GT_VOID    
)
{
    GT_STATUS   rc;

    rc = prvTgfBrgFdbMacEntryDelete(&(prvTgfMacEntry.key));
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in prvTgfBrgFdbMacEntrySet %d", GT_TRUE);
}

/*******************************************************************************
* prvTgfPortTxResourceHistogramVlanConfigurationSet
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
GT_VOID prvTgfPortTxResourceHistogramVlanConfigurationSet
(
    GT_VOID    
)
{
    GT_STATUS rc = GT_OK;

    /* create a vlan */
    rc = prvTgfBrgDefVlanEntryWrite(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d, %d", 
                                 prvTgfDevNum, PRV_TGF_VLANID_CNS);

    prvTgfPortTxResourceHistogramFdbConfigurationSet();
}

/*******************************************************************************
* prvTgfPortTxResourceHistogramConfigurationSet
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
GT_VOID prvTgfPortTxResourceHistogramConfigurationSet
(
    GT_VOID
)
{
    GT_U32      i, cntrValue;
    GT_STATUS   rc = GT_OK;

    rc = prvTgfPortFcHolSysModeSet(prvTgfDevNum, PRV_TGF_PORT_PORT_HOL_E);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPortFcHolSysModeSet: %d, %d",
                                 prvTgfDevNum, PRV_TGF_PORT_PORT_HOL_E);
    
    for (i = 0; i < 4; i++)
    {
        /* set threshold */
        rc = prvTgfPortTxResourceHistogramThresholdSet(prvTgfDevNum, i, i*10);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfPortTxResourceHistogramThresholdSet: %d, %d, %d",
                                     prvTgfDevNum, i, i*10);

        /* clear counters */
        rc = prvTgfPortTxResourceHistogramCounterGet(prvTgfDevNum, i, &cntrValue);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPortTxResourceHistogramCounterGet: %d, %d",
                                     prvTgfDevNum, i);
    }
}

/*******************************************************************************
* prvTgfPortTxResourceHistogramRestore
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
GT_VOID prvTgfPortTxResourceHistogramRestore
(
    GT_VOID
)
{
    GT_U32      i, cntrValue;
    GT_STATUS   rc = GT_OK;

    rc = prvTgfPortFcHolSysModeSet(prvTgfDevNum, PRV_TGF_PORT_PORT_FC_E);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPortFcHolSysModeSet: %d, %d",
                                 prvTgfDevNum, PRV_TGF_PORT_PORT_FC_E);

    prvTgfPortTxResourceHistogramFdbEntryDelete();
    
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d, %d", 
                                 prvTgfDevNum, PRV_TGF_VLANID_CNS);

    for (i = 0; i < 4; i++)
    {
        /* reset threshold */
        rc = prvTgfPortTxResourceHistogramThresholdSet(prvTgfDevNum, i, 0);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfPortTxResourceHistogramThresholdSet: %d, %d, %d",
                                     prvTgfDevNum, i, 0);

        /* clear counters */
        rc = prvTgfPortTxResourceHistogramCounterGet(prvTgfDevNum, i, &cntrValue);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPortTxResourceHistogramCounterGet: %d, %d",
                                     prvTgfDevNum, i);
    }
}

/*******************************************************************************
* prvTgfPortTxResourceHistogramPacketSend
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
static GT_VOID prvTgfPortTxResourceHistogramPacketSend
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
* prvTgPortTxResourceHistogramTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic.
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
GT_VOID prvTgfPortTxResourceHistogramTrafficGenerate
(
    void
)
{
    GT_STATUS                       rc = GT_OK;
    GT_U32                          portIter = 0, i, cntrValue;
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    GT_BOOL                         saveEnableSharing;
    GT_U16                          savePortMaxBuffLimit, savePortMaxDescrLimit;
    PRV_TGF_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS saveTailDropProfileTc0Params;
    PRV_TGF_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS saveTailDropProfileTc7Params;
    PRV_TGF_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS setTailDropProfileParams;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT   savePort0ProfileSet, savePort18ProfileSet;

    /* save & set profile settings */
    rc = prvTgfPortTxTailDropProfileGet(prvTgfDevNum, CPSS_PORT_TX_DROP_PROFILE_1_E,
                                        &saveEnableSharing,
                                        &savePortMaxBuffLimit, 
                                        &savePortMaxDescrLimit);
    UTF_VERIFY_EQUAL2_STRING_MAC(rc, GT_OK, "prvTgfPortTxTailDropProfileGet: %d, %d",
                                 prvTgfDevNum, CPSS_PORT_TX_DROP_PROFILE_1_E);

    rc = prvTgfPortTxTailDropProfileSet(prvTgfDevNum, CPSS_PORT_TX_DROP_PROFILE_1_E,
                                        GT_FALSE,
                                        1024, 1024);
    UTF_VERIFY_EQUAL5_STRING_MAC(rc, GT_OK, "prvTgfPortTxTailDropProfileSet: %d, %d, %d, %d, %d",
                                 prvTgfDevNum, CPSS_PORT_TX_DROP_PROFILE_1_E, GT_FALSE,
                                 1024, 1024);

    rc = prvTgfPortTx4TcTailDropProfileGet(prvTgfDevNum, CPSS_PORT_TX_DROP_PROFILE_1_E,
                                           0, &saveTailDropProfileTc0Params);
    UTF_VERIFY_EQUAL3_STRING_MAC(rc, GT_OK, "prvTgfPortTx4TcTailDropProfileGet: %d, %d, %d",
                                 prvTgfDevNum, CPSS_PORT_TX_DROP_PROFILE_1_E, 0);

    rc = prvTgfPortTx4TcTailDropProfileGet(prvTgfDevNum, CPSS_PORT_TX_DROP_PROFILE_1_E,
                                           7, &saveTailDropProfileTc7Params);
    UTF_VERIFY_EQUAL3_STRING_MAC(rc, GT_OK, "prvTgfPortTx4TcTailDropProfileGet: %d, %d, %d",
                                 prvTgfDevNum, CPSS_PORT_TX_DROP_PROFILE_1_E, 7);

    setTailDropProfileParams.dp0MaxBuffNum  = 50;
    setTailDropProfileParams.dp0MaxDescrNum = 50;
    setTailDropProfileParams.dp1MaxBuffNum  = 40;
    setTailDropProfileParams.dp1MaxDescrNum = 40;
    setTailDropProfileParams.dp2MaxBuffNum  = 30;
    setTailDropProfileParams.dp2MaxDescrNum = 30;
    setTailDropProfileParams.tcMaxBuffNum   = 100;
    setTailDropProfileParams.tcMaxDescrNum  = 100;

    rc = prvTgfPortTx4TcTailDropProfileSet(prvTgfDevNum, CPSS_PORT_TX_DROP_PROFILE_1_E,
                                           0, &setTailDropProfileParams);
    UTF_VERIFY_EQUAL3_STRING_MAC(rc, GT_OK, "prvTgfPortTx4TcTailDropProfileSet: %d, %d, %d",
                                 prvTgfDevNum, CPSS_PORT_TX_DROP_PROFILE_1_E, 0);

    rc = prvTgfPortTx4TcTailDropProfileSet(prvTgfDevNum, CPSS_PORT_TX_DROP_PROFILE_1_E,
                                           7, &setTailDropProfileParams);
    UTF_VERIFY_EQUAL3_STRING_MAC(rc, GT_OK, "prvTgfPortTx4TcTailDropProfileSet: %d, %d, %d",
                                 prvTgfDevNum, CPSS_PORT_TX_DROP_PROFILE_1_E, 7);

    rc = prvTgfPortTxBindPortToDpGet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECIEVE_PORT_IDX_0_CNS],
                                     &savePort0ProfileSet);
    UTF_VERIFY_EQUAL2_STRING_MAC(rc, GT_OK, "prvTgfPortTxBindPortToDpGet: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECIEVE_PORT_IDX_0_CNS]);

    rc = prvTgfPortTxBindPortToDpGet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_2_CNS],
                                     &savePort18ProfileSet);
    UTF_VERIFY_EQUAL2_STRING_MAC(rc, GT_OK, "prvTgfPortTxBindPortToDpGet: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECIEVE_PORT_IDX_0_CNS]);

    rc = prvTgfPortTxBindPortToDpSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECIEVE_PORT_IDX_0_CNS],
                                     CPSS_PORT_TX_DROP_PROFILE_1_E);
    UTF_VERIFY_EQUAL3_STRING_MAC(rc, GT_OK, "prvTgfPortTxBindPortToDpSet: %d, %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECIEVE_PORT_IDX_0_CNS],
                                 CPSS_PORT_TX_DROP_PROFILE_1_E);

    /* enable packet trace */
    rc = tgfTrafficTracePacketByteSet(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in tgfTrafficTracePacketByteSet %d", GT_TRUE);

    rc = prvTgfPortTxQueueTxEnableSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECIEVE_PORT_IDX_0_CNS], 0, GT_FALSE);
    UTF_VERIFY_EQUAL4_STRING_MAC(rc, GT_OK, "prvTgfPortTxQueueTxEnableSet: %d, %d, %d, *d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECIEVE_PORT_IDX_0_CNS], 0, GT_FALSE);

    /* Sending unicast */
    PRV_UTF_LOG0_MAC("======= Sending unicast =======\n");

    /* Set DA MAC Address to be 00:00:00:00:00:11 */
    prvTgfPacketL2Part.daMac[0] = 0x00;
    prvTgfPacketL2Part.daMac[1] = 0x00;
    prvTgfPacketL2Part.daMac[2] = 0x00;
    prvTgfPacketL2Part.daMac[3] = 0x00;
    prvTgfPacketL2Part.daMac[4] = 0x00;
    prvTgfPacketL2Part.daMac[5] = 0x11;

    /* send 100 packets */
    prvTgfPortTxResourceHistogramPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_2_CNS], &prvTgfPacketInfo);
    cpssOsTimerWkAfter(10);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_2_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
    }

    for (i = 0; i < 4; i++)
    {
        /* get counters */
        rc = prvTgfPortTxResourceHistogramCounterGet(prvTgfDevNum, i, &cntrValue);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPortTxResourceHistogramCounterGet: %d, %d",
                                     prvTgfDevNum, i);
    }

    rc = prvTgfPortTxQueueTxEnableSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECIEVE_PORT_IDX_0_CNS], 0, GT_TRUE);
    UTF_VERIFY_EQUAL4_STRING_MAC(rc, GT_OK, "prvTgfPortTxQueueTxEnableSet: %d, %d, %d, *d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECIEVE_PORT_IDX_0_CNS], 0, GT_TRUE);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        /* check Tx counters */
        if (PRV_TGF_RECIEVE_PORT_IDX_0_CNS == portIter)
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(50, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
    }
    
    rc = prvTgfPortTxBindPortToDpSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_2_CNS],
                                     CPSS_PORT_TX_DROP_PROFILE_1_E);
    UTF_VERIFY_EQUAL3_STRING_MAC(rc, GT_OK, "prvTgfPortTxBindPortToDpSet: %d, %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_2_CNS],
                                 CPSS_PORT_TX_DROP_PROFILE_1_E);

    rc = prvTgfPortTxQueueTxEnableSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_2_CNS], 7, GT_FALSE);
    UTF_VERIFY_EQUAL4_STRING_MAC(rc, GT_OK, "prvTgfPortTxQueueTxEnableSet: %d, %d, %d, *d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECIEVE_PORT_IDX_0_CNS], 0, GT_FALSE);

    
    /* send packets */
    prvTgfPortTxResourceHistogramPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_2_CNS], &prvTgfPacketInfo);
    cpssOsTimerWkAfter(10);
 
    for (i = 0; i < 4; i++)
    {
        /* get counters */
        rc = prvTgfPortTxResourceHistogramCounterGet(prvTgfDevNum, i, &cntrValue);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPortTxResourceHistogramCounterGet: %d, %d",
                                     prvTgfDevNum, i);
 
        UTF_VERIFY_EQUAL1_STRING_MAC(49-i*10, 
                                     cntrValue, "got other value then expected for counter %d",
                                     i);
    }
  
    rc = prvTgfPortTxQueueTxEnableSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_2_CNS], 7, GT_TRUE);
    UTF_VERIFY_EQUAL4_STRING_MAC(rc, GT_OK, "prvTgfPortTxQueueTxEnableSet: %d, %d, %d, *d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECIEVE_PORT_IDX_0_CNS], 0, GT_TRUE);

    /* restore profile settings */
    rc = prvTgfPortTxTailDropProfileSet(prvTgfDevNum, CPSS_PORT_TX_DROP_PROFILE_1_E,
                                        saveEnableSharing,
                                        savePortMaxBuffLimit, 
                                        savePortMaxDescrLimit);
    UTF_VERIFY_EQUAL5_STRING_MAC(rc, GT_OK, "prvTgfPortTxTailDropProfileSet: %d, %d, %d, %d, %d",
                                 prvTgfDevNum, CPSS_PORT_TX_DROP_PROFILE_1_E, 
                                 saveEnableSharing,
                                 savePortMaxBuffLimit, 
                                 savePortMaxDescrLimit);

    rc = prvTgfPortTx4TcTailDropProfileSet(prvTgfDevNum, CPSS_PORT_TX_DROP_PROFILE_1_E,
                                           0, &saveTailDropProfileTc0Params);
    UTF_VERIFY_EQUAL3_STRING_MAC(rc, GT_OK, "prvTgfPortTx4TcTailDropProfileSet: %d, %d, %d",
                                 prvTgfDevNum, CPSS_PORT_TX_DROP_PROFILE_1_E, 0);

    rc = prvTgfPortTx4TcTailDropProfileSet(prvTgfDevNum, CPSS_PORT_TX_DROP_PROFILE_1_E,
                                           7, &saveTailDropProfileTc7Params);
    UTF_VERIFY_EQUAL3_STRING_MAC(rc, GT_OK, "prvTgfPortTx4TcTailDropProfileSet: %d, %d, %d",
                                 prvTgfDevNum, CPSS_PORT_TX_DROP_PROFILE_1_E, 7);

    rc = prvTgfPortTxBindPortToDpSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECIEVE_PORT_IDX_0_CNS],
                                     savePort0ProfileSet);
    UTF_VERIFY_EQUAL3_STRING_MAC(rc, GT_OK, "prvTgfPortTxBindPortToDpSet: %d, %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_RECIEVE_PORT_IDX_0_CNS],
                                 savePort0ProfileSet);

    rc = prvTgfPortTxBindPortToDpSet(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_2_CNS],
                                     savePort18ProfileSet);
    UTF_VERIFY_EQUAL3_STRING_MAC(rc, GT_OK, "prvTgfPortTxBindPortToDpSet: %d, %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_2_CNS],
                                 savePort18ProfileSet);
}
