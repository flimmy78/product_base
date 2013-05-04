/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfBasicDynamicLearning.c
*
* DESCRIPTION:
*       Basic Dynamic Learning
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>

#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfBridgeGen.h>
#include <bridge/prvTgfBasicDynamicLearning.h>


/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/

/* default VLAN Id */
#define PRV_TGF_VLANID_CNS             2

/* default number of packets to send */
static GT_U32       prvTgfBurstCount = 1;

/* array of source MACs for the tests */
static TGF_MAC_ADDR prvTgfSaMacArr[] = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
                                        {0x00, 0x00, 0x00, 0x00, 0x00, 0x02},
                                        {0x00, 0x00, 0x00, 0x00, 0x00, 0x03},
                                        {0x00, 0x00, 0x00, 0x00, 0x00, 0x04}};
/* array of destination MACs for the tests */
static TGF_MAC_ADDR prvTgfDaMacArr[][PRV_TGF_PORTS_NUM_CNS - 1] = {
                                        {{0x00, 0x00, 0x00, 0x00, 0x00, 0x02},
                                         {0x00, 0x00, 0x00, 0x00, 0x00, 0x03},
                                         {0x00, 0x00, 0x00, 0x00, 0x00, 0x04}
                                        },
                                        {{0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
                                         {0x00, 0x00, 0x00, 0x00, 0x00, 0x03},
                                         {0x00, 0x00, 0x00, 0x00, 0x00, 0x04}
                                        },
                                        {{0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
                                         {0x00, 0x00, 0x00, 0x00, 0x00, 0x02},
                                         {0x00, 0x00, 0x00, 0x00, 0x00, 0x04}
                                        },
                                        {{0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
                                         {0x00, 0x00, 0x00, 0x00, 0x00, 0x02},
                                         {0x00, 0x00, 0x00, 0x00, 0x00, 0x03}
                                        }
                                       };

/******************************* Test packet **********************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part = {
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},               /* dstMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x33}                /* srcMac */
};
/* VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTagPart = {
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
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x55
};
/* PAYLOAD part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketPayloadPart = {
    sizeof(prvTgfPayloadDataArr),                       /* dataLength */
    prvTgfPayloadDataArr                                /* dataPtr */
};

/* PARTS of packet */
static TGF_PACKET_PART_STC prvTgfPacketPartArray[] = {
    {TGF_PACKET_PART_L2_E,       &prvTgfPacketL2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E, &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_PAYLOAD_E,  &prvTgfPacketPayloadPart}
};

/* LENGTH of packet */
#define PRV_TGF_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + sizeof(prvTgfPayloadDataArr)

/* LENGTH of packet with CRC */
#define PRV_TGF_PACKET_CRC_LEN_CNS  PRV_TGF_PACKET_LEN_CNS + TGF_CRC_LEN_CNS

/* PACKET to send */
static TGF_PACKET_STC prvTgfPacketInfo = {
    PRV_TGF_PACKET_LEN_CNS,                                      /* totalLen */
    sizeof(prvTgfPacketPartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketPartArray                                        /* partsArray */
};

/******************************************************************************/


/******************************************************************************\
 *                            Private variables                               *
\******************************************************************************/

/* stored default L2 part */
static TGF_PACKET_L2_STC prvTgfDefPacketL2Part;

/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfBrgBdlConfigurationSet
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
GT_VOID prvTgfBrgBdlConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;
    
    /* save L2 packet's part */
    cpssOsMemCpy(&prvTgfDefPacketL2Part, &prvTgfPacketL2Part, sizeof(prvTgfDefPacketL2Part));

    /* set default vlan entry */
    rc = prvTgfBrgDefVlanEntryWrite(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d", prvTgfDevNum);
}

/*******************************************************************************
* prvTgfBrgBdlTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic
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
GT_VOID prvTgfBrgBdlTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS       rc           = GT_OK;
    GT_U32          portsCount   = prvTgfPortsNum;
    GT_U32          portIter     = 0;
    GT_U32          ucPortIter   = 0;
    GT_U32          ucPortToIter = 0;
    GT_U32          fdbIter      = 0;
    GT_U32          fdbCount     = sizeof(prvTgfSaMacArr) / sizeof(TGF_MAC_ADDR);

    CPSS_PORT_MAC_COUNTER_SET_STC portCntrs;
    PRV_TGF_BRG_MAC_ENTRY_STC     macEntry;
    PRV_TGF_MAC_ENTRY_KEY_STC     macEntryKey;

    /* -------------------------------------------------------------------------
     * 1. brodcast phase - send brodcast packet from each port to setup FDB table
     */
    PRV_UTF_LOG0_MAC("======= Sending brodcast packets to setup FDB table =======\n");

    /* reset counters */
    for (portIter = 0; portIter < portsCount; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* setup Packet */
    rc = prvTgfSetTxSetupEth(prvTgfDevNum, &prvTgfPacketInfo, prvTgfBurstCount, 0, NULL);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of SetTxSetup: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[portIter]);

    /* send Packet from each port */
    for (portIter = 0; portIter < portsCount; portIter++)
    {
        /* set source MAC address in Packet */
        cpssOsMemCpy(prvTgfPacketL2Part.saMac, prvTgfSaMacArr[portIter],
                     sizeof(prvTgfPacketL2Part.saMac));

        /* send Packet */
        rc = prvTgfStartTransmitingEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, &d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* read and check counters */
    for (portIter = 0; portIter < portsCount; portIter++)
    {
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        /* check Tx counters */
        UTF_VERIFY_EQUAL0_STRING_MAC(((PRV_TGF_PACKET_CRC_LEN_CNS - TGF_VLAN_TAG_SIZE_CNS) * (portsCount - 1) + PRV_TGF_PACKET_CRC_LEN_CNS) * prvTgfBurstCount,
                                     portCntrs.goodOctetsSent.l[0],
                                     "get another goodOctetsSent counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(portsCount * prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                     "get another goodPktsSent counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(portsCount * prvTgfBurstCount, portCntrs.brdcPktsSent.l[0],
                                     "get another brdcPktsSent counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.ucPktsSent.l[0],
                                     "get another ucPktsSent counter than expected");

        /* check Rx counters */
        UTF_VERIFY_EQUAL0_STRING_MAC((PRV_TGF_PACKET_CRC_LEN_CNS) * prvTgfBurstCount,
                                     portCntrs.goodOctetsRcv.l[0],
                                     "get another goodOctetsRcv counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                     "get another goodPktsRcv counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.brdcPktsRcv.l[0],
                                     "get another brdcPktsRcv counter than expected");
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.ucPktsRcv.l[0],
                                     "get another ucPktsRcv counter than expected");
    }


    /* -------------------------------------------------------------------------
     * 2. Check FDB entries phase
     */
    macEntryKey.entryType = PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E;
    macEntryKey.key.macVlan.vlanId = PRV_TGF_VLANID_CNS;
    macEntryKey.key.macVlan.fId = PRV_TGF_VLANID_CNS;
    for (fdbIter = 0; fdbIter < fdbCount; fdbIter++)
    {
        cpssOsMemCpy(macEntryKey.key.macVlan.macAddr.arEther, prvTgfSaMacArr[fdbIter], sizeof(TGF_MAC_ADDR));
        cpssOsMemSet(&macEntry, 0, sizeof(macEntry));

        /* get FDB entry */
        rc = prvTgfBrgFdbMacEntryGet(&macEntryKey, &macEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbMacEntryGet: %d", prvTgfDevNum);

        /* compare entryType */
        rc = (macEntry.key.entryType == PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E) ? GT_OK : GT_FAIL;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "vlanId: %d", macEntry.key.entryType);

        /* compare MAC address */
        rc = cpssOsMemCmp(macEntry.key.key.macVlan.macAddr.arEther,
                          macEntryKey.key.macVlan.macAddr.arEther,
                          sizeof(TGF_MAC_ADDR)) == 0 ? GT_OK : GT_FAIL;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "MAC address[0]: %d",
                                     macEntry.key.key.macVlan.macAddr.arEther[0]);

        /* compare vlanId */
        rc = (macEntry.key.key.macVlan.vlanId == macEntryKey.key.macVlan.vlanId) ? GT_OK : GT_FAIL;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "vlanId: %d", macEntryKey.key.macVlan.vlanId);

        /* compare dstInterface.type */
        rc = (macEntry.dstInterface.type == CPSS_INTERFACE_PORT_E) ? GT_OK : GT_FAIL;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "dstInterface.type: %d", macEntry.dstInterface.type);

        /* compare dstInterface */
        if (macEntry.dstInterface.type == CPSS_INTERFACE_PORT_E) {
            rc = (macEntry.dstInterface.devPort.devNum == prvTgfDevNum) ? GT_OK : GT_FAIL;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "dstInterface.devPort.devNum: %d",
                                         macEntry.dstInterface.devPort.devNum);

            rc = (macEntry.dstInterface.devPort.portNum == prvTgfPortsArray[fdbIter]) ? GT_OK : GT_FAIL;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "dstInterface.devPort.portNum: %d",
                                         macEntry.dstInterface.devPort.portNum);
        }
    }


    /* -------------------------------------------------------------------------
     * 3. unicast phase - send unicast packets from each port to all another ports
     */
    PRV_UTF_LOG0_MAC("======= Sending unicast packets to check FDB table =======\n");

    /* send Packet from each port */
    for (ucPortIter = 0; ucPortIter < portsCount; ucPortIter++)
    {
        /* reset counters */
        for (portIter = 0; portIter < portsCount; portIter++)
        {
            rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                         prvTgfDevNum, prvTgfPortsArray[portIter]);
        }

        PRV_UTF_LOG1_MAC("------- Sending packets from port %d -------\n", prvTgfPortsArray[ucPortIter]);
        for (ucPortToIter = 0; ucPortToIter < portsCount - 1; ucPortToIter++)
        {
            /* set destination MAC address in Packet */
            cpssOsMemCpy(prvTgfPacketL2Part.daMac, prvTgfDaMacArr[ucPortIter][ucPortToIter],
                         sizeof(prvTgfPacketL2Part.daMac));

            /* set source MAC address in Packet */
            cpssOsMemCpy(prvTgfPacketL2Part.saMac, prvTgfSaMacArr[ucPortIter],
                         sizeof(prvTgfPacketL2Part.saMac));

            /* send Packet */
            rc = prvTgfStartTransmitingEth(prvTgfDevNum, prvTgfPortsArray[ucPortIter]);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, &d\n",
                                         prvTgfDevNum, prvTgfPortsArray[ucPortIter]);
        }

        /* read and check counters */
        for (portIter = 0; portIter < portsCount; portIter++)
        {
            /* read counters */
            rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d\n",
                                         prvTgfDevNum, prvTgfPortsArray[portIter]);

            /* for sending port */
            if (ucPortIter == portIter)
            {
                /* check Rx counters - CPU receives 3 packets from this port to forwarding */
                UTF_VERIFY_EQUAL0_STRING_MAC((PRV_TGF_PACKET_CRC_LEN_CNS) * (portsCount - 1) * prvTgfBurstCount,
                                             portCntrs.goodOctetsRcv.l[0],
                                             "get another goodOctetsRcv counter than expected");
                UTF_VERIFY_EQUAL0_STRING_MAC((portsCount - 1) * prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                             "get another goodPktsRcv counter than expected");
                UTF_VERIFY_EQUAL0_STRING_MAC((portsCount - 1) * prvTgfBurstCount, portCntrs.ucPktsRcv.l[0],
                                             "get another ucPktsRcv counter than expected");

                /* check Tx counters - CPU transfers these packet back because LoopBack is ON in simulation */
                UTF_VERIFY_EQUAL0_STRING_MAC((PRV_TGF_PACKET_CRC_LEN_CNS) * (portsCount - 1) * prvTgfBurstCount,
                                             portCntrs.goodOctetsSent.l[0],
                                             "get another goodOctetsSent counter than expected");
                UTF_VERIFY_EQUAL0_STRING_MAC((portsCount - 1) * prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                             "get another goodPktsSent counter than expected");
                UTF_VERIFY_EQUAL0_STRING_MAC((portsCount - 1) * prvTgfBurstCount, portCntrs.ucPktsSent.l[0],
                                             "get another ucPktsSent counter than expected");
            }

            /* for other ports */
            else
            {
                /* check Rx counters - CPU doesn't receive any packets from these ports */
                UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.goodOctetsRcv.l[0],
                                             "get another goodOctetsRcv counter than expected");
                UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.goodPktsRcv.l[0],
                                             "get another goodPktsRcv counter than expected");
                UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.ucPktsRcv.l[0],
                                             "get another ucPktsRcv counter than expected");

                /* check Tx counters - CPU transfers packets to these ports according to FDB record */
                UTF_VERIFY_EQUAL0_STRING_MAC((PRV_TGF_PACKET_CRC_LEN_CNS - TGF_VLAN_TAG_SIZE_CNS) * prvTgfBurstCount,
                                             portCntrs.goodOctetsSent.l[0],
                                             "get another goodOctetsSent counter than expected");
                UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                             "get another goodPktsSent counter than expected");
                UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.ucPktsSent.l[0],
                                             "get another ucPktsSent counter than expected");
            }

            /* check zero counters for unused traffic type */
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.brdcPktsSent.l[0],
                                         "get another brdcPktsSent counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.mcPktsSent.l[0],
                                         "get another mcPktsSent counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.brdcPktsRcv.l[0],
                                         "get another brdcPktsRcv counter than expected");
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.mcPktsRcv.l[0],
                                         "get another mcPktsRcv counter than expected");
        }
    }
}

/*******************************************************************************
* prvTgfBrgBdlConfigurationRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfBrgBdlConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;


    /* restore L2 packet's part */
    cpssOsMemCpy(&prvTgfPacketL2Part, &prvTgfDefPacketL2Part, sizeof(prvTgfPacketL2Part));

    /* flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d, %d", prvTgfDevNum, GT_TRUE);

    /* invalidate vlan entry (and reset vlan entry) */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d, %d",
                                 prvTgfDevNum, PRV_TGF_VLANID_CNS);
}

