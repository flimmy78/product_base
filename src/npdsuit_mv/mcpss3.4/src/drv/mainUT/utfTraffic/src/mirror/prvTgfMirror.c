/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfMirror.c
*
* DESCRIPTION:
*       CPSS Mirror
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>

#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfBridgeGen.h>
#include <common/tgfPclGen.h>
#include <common/tgfMirror.h>
#include <mirror/prvTgfMirror.h>


/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/
/* default VLAN0 Id */
#define PRV_TGF_VLANID_CNS  5

/* ether type used for "vlan tag" */
#define TGF_ETHERTYPE_8100_VLAN_TAG_CNS 0x8100

/* port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_CNS         1

/* another port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_ANOTHER_CNS 0

/* port number to receive traffic */
#define PRV_TGF_RCV1_PORT_IDX_CNS         2

/* port number to receive traffic */
#define PRV_TGF_RCV_PORT_IDX_CNS          3

/* Egress Source mode test */

/* Tx Mirrored ports */
#define PRV_TGF_TX_MIRR_PORT1_IDX_CNS      2
#define PRV_TGF_TX_MIRR_PORT2_IDX_CNS      3

/* Send port */
#define PRV_TGF_TX_MIRR_SEND_IDX_CNS       1

/* save ports list */
static GT_U8   prvTgfPortsArraySave[PRV_TGF_MAX_PORTS_NUM_CNS];
static GT_U8   prvTgfPortsNumSave;

/* default number of packets to send */
static GT_U32  prvTgfBurstCount   = 1;

/******************************* Test packet **********************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part =
{
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},               /* dstMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x02}                /* srcMac */
};

/* L2 part of UC packet 1 */
static TGF_PACKET_L2_STC prvTgfPacketL2PartUC1 =
{
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x55},               /* dstMac */
    {0x00, 0x00, 0x00, 0x00, 0x22, 0x22}                /* srcMac */
};

/* L2 part of UC packet 2 */
static TGF_PACKET_L2_STC prvTgfPacketL2PartUC2 =
{
    {0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0xee},               /* dstMac */
    {0x00, 0x00, 0x00, 0x00, 0x33, 0x33}                /* srcMac */
};

/* L2 part of UC packet 1 for learning */
static TGF_PACKET_L2_STC prvTgfPacketL2PartUC1Learn =
{
    {0x00, 0x00, 0x00, 0x00, 0x33, 0x33},               /* dstMac */
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x55}                /* srcMac */
};

/* L2 part of UC packet 2 for learning */
static TGF_PACKET_L2_STC prvTgfPacketL2PartUC2Learn =
{
    {0x00, 0x00, 0x00, 0x00, 0x44, 0x55},               /* dstMac */
    {0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0xee}                /* srcMac */
};

/* First VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTag0Part =
{
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,                    /* etherType */
    0, 0, PRV_TGF_VLANID_CNS                            /* pri, cfi, VlanId */
};


/* DATA of packet */
static GT_U8 prvTgfPayloadDataArr[] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x02, 0x81, 0x00, 0x05, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* PAYLOAD part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketPayloadPart =
{
    sizeof(prvTgfPayloadDataArr),                       /* dataLength */
    prvTgfPayloadDataArr                                /* dataPtr */
};

/* PARTS of packet */
static TGF_PACKET_PART_STC prvTgfPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,       &prvTgfPacketL2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E, &prvTgfPacketVlanTag0Part},
    {TGF_PACKET_PART_PAYLOAD_E,  &prvTgfPacketPayloadPart}
};


static TGF_PACKET_PART_STC prvTgfPacketPartArrayUC1[] =
{
    {TGF_PACKET_PART_L2_E,       &prvTgfPacketL2PartUC1},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E, &prvTgfPacketVlanTag0Part},
    {TGF_PACKET_PART_PAYLOAD_E,  &prvTgfPacketPayloadPart}
};

static TGF_PACKET_PART_STC prvTgfPacketPartArrayUC2[] =
{
    {TGF_PACKET_PART_L2_E,       &prvTgfPacketL2PartUC2},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E, &prvTgfPacketVlanTag0Part},
    {TGF_PACKET_PART_PAYLOAD_E,  &prvTgfPacketPayloadPart}
};


static TGF_PACKET_PART_STC prvTgfPacketPartArrayUC1Learn[] =
{
    {TGF_PACKET_PART_L2_E,       &prvTgfPacketL2PartUC1Learn},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E, &prvTgfPacketVlanTag0Part},
    {TGF_PACKET_PART_PAYLOAD_E,  &prvTgfPacketPayloadPart}
};

static TGF_PACKET_PART_STC prvTgfPacketPartArrayUC2Learn[] =
{
    {TGF_PACKET_PART_L2_E,       &prvTgfPacketL2PartUC2Learn},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E, &prvTgfPacketVlanTag0Part},
    {TGF_PACKET_PART_PAYLOAD_E,  &prvTgfPacketPayloadPart}
};

/* LENGTH of packet */
#define PRV_TGF_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + sizeof(prvTgfPayloadDataArr)

/* LENGTH of packet with CRC */
#define PRV_TGF_PACKET_CRC_LEN_CNS  PRV_TGF_PACKET_LEN_CNS + TGF_CRC_LEN_CNS

/* PACKET to send */
static TGF_PACKET_STC prvTgfPacketInfo =
{
    PRV_TGF_PACKET_LEN_CNS,                                      /* totalLen */
    sizeof(prvTgfPacketPartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketPartArray                                        /* partsArray */
};


/* PACKET to send */
static TGF_PACKET_STC prvTgfPacketInfoUC1 =
{
    PRV_TGF_PACKET_LEN_CNS,                                      /* totalLen */
    sizeof(prvTgfPacketPartArrayUC1) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketPartArrayUC1                                        /* partsArray */
};


static TGF_PACKET_STC prvTgfPacketInfoUC2 =
{
    PRV_TGF_PACKET_LEN_CNS,                                      /* totalLen */
    sizeof(prvTgfPacketPartArrayUC2) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketPartArrayUC2                                        /* partsArray */
};

static TGF_PACKET_STC prvTgfPacketInfoUC1Learn =
{
    PRV_TGF_PACKET_LEN_CNS,                                      /* totalLen */
    sizeof(prvTgfPacketPartArrayUC1Learn) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketPartArrayUC1Learn                                        /* partsArray */
};


static TGF_PACKET_STC prvTgfPacketInfoUC2Learn =
{
    PRV_TGF_PACKET_LEN_CNS,                                      /* totalLen */
    sizeof(prvTgfPacketPartArrayUC2Learn) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketPartArrayUC2Learn                                        /* partsArray */
};


/******************************************************************************\
 *                           Private declarations                             *
\******************************************************************************/

/* default enable/disable mirroring */
static GT_BOOL   prvTgfEnableMirror = GT_FALSE;

/* default analyzer destination interface index */
static GT_U32    prvTgfIndex        = 0;


/******************************************************************************\
 *                            Private test functions                          *
\******************************************************************************/

/*******************************************************************************
* prvTgfMirrorTestPacketSend
*
* DESCRIPTION:
*       Function sends packet, performs trace and check expected results.
*
* INPUTS:
*       portNum       - port number to send traffic from
*       packetInfoPtr - PACKET to send
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
static GT_VOID prvTgfMirrorTestPacketSend
(
    IN GT_U8           portNum,
    IN TGF_PACKET_STC *packetInfoPtr
)
{
    GT_STATUS       rc = GT_OK;

    /* use different 'etherType for vlan tagging' instead of 0x8100 */
    /* this will make our packet that has '0x8100' to be recognized as 'untagged'
      by the 'traffic generator' and also the CPSS will not override the ethertype
      of tag0 with the DSA tag */
    tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_NON_VALID_TAG_CNS);

    /* setup Packet */
    rc = prvTgfSetTxSetupEth(prvTgfDevNum, packetInfoPtr, prvTgfBurstCount, 0, NULL);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "ERROR of SetTxSetup: %d, %d",
                                 prvTgfDevNum, prvTgfBurstCount, 0, NULL);

    /* send Packet from port portNum */
    rc = prvTgfStartTransmitingEth(prvTgfDevNum, portNum);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, %d",
                                 prvTgfDevNum, portNum);

    /* restore default ethertype */
    tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_8100_VLAN_TAG_CNS);
}

/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/

/*******************************************************************************
* prvMirrorAnalyzerInterfaceSet
*
* DESCRIPTION:
*       This function sets analyzer interface.
*
*
* INPUTS:
*      devNum            - device number.
*      index             - index of analyzer interface. (0 - 6)
*      analyzerDevNum    - analyzer device number.
*      analyzerPortNum   - analyzer port number.
*
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
static GT_VOID prvMirrorAnalyzerInterfaceSet
(
    IN GT_U32    index,
    IN GT_U8     analyzerDevNum,
    IN GT_U8     analyzerPort
)
{
    PRV_TGF_MIRROR_ANALYZER_INTERFACE_STC  interface;
    GT_STATUS    rc = GT_OK;

    interface.interface.type = CPSS_INTERFACE_PORT_E;
    interface.interface.devPort.devNum = analyzerDevNum;
    interface.interface.devPort.portNum = analyzerPort;

    /* set analyzer interface */
    rc = prvTgfMirrorAnalyzerInterfaceSet(index, &interface);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorAnalyzerInterfaceSet: %d", index);
}

/*******************************************************************************
* prvTgfIngressMirrorHopByHopConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set analyzer interface for index 4.
*            Device number 0, port number 18, port interface.
*         -        Set analyzer interface for index 0.
*            Device number 0, port number 23, port interface.
*         -        Set analyzer interface index to 4 and enable Rx mirroring.
*         - Enable Rx mirroring on port 8.
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
GT_VOID prvTgfIngressMirrorHopByHopConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;


    /* set VLAN entry */
    rc = prvTgfBrgDefVlanEntryWrite(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d",
                                 PRV_TGF_VLANID_CNS);

    /* Set analyzer interface for index 4.
        Device number 0, port number 18, port interface. */
    prvMirrorAnalyzerInterfaceSet(4, prvTgfDevsArray[2], prvTgfPortsArray[2]);

    /* Set analyzer interface for index 0.
     Device number 0, port number 23, port interface. */
    prvMirrorAnalyzerInterfaceSet(0, prvTgfDevsArray[3], prvTgfPortsArray[3]);

    /* save default analyzer interface */
    rc = prvTgfMirrorRxGlobalAnalyzerInterfaceIndexGet(prvTgfDevNum, &prvTgfEnableMirror, &prvTgfIndex);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxGlobalAnalyzerInterfaceIndexGet: %d", prvTgfDevNum);

    /* Set analyzer interface index to 4 and enable Rx mirroring */
    rc = prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet(GT_TRUE, 4);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet: %d %d", GT_TRUE, 4);

    /* Enable Rx mirroring on port 8. */
    rc = prvTgfMirrorRxPortSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_TRUE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxPortSet: %d %d",
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_TRUE);
}

/*******************************************************************************
* prvTgfIngressMirrorHopByHopTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 23.
*                2 Packets are captured on port 18.
*       Disable Rx mirroring.
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                Packet is captured on ports 0, 18, 23.
*       Set analyzer interface index to 0 and enable  Rx mirroring.
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 18.
*                2 Packets are captured on port 23.
*       Disable Rx mirroring on port 8.
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                Packet is captured on ports 0, 18, 23.
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
GT_VOID prvTgfIngressMirrorHopByHopTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS    rc = GT_OK;
    GT_U32       portIter = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC portCntrs;


    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {

            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that port 18 received 2 packets */
        if (PRV_TGF_RCV1_PORT_IDX_CNS == portIter)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount * 2, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* Disable Rx mirroring */
    rc = prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet(GT_FALSE, 0);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet: %d", GT_FALSE);

    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {

            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that all ports except of send port received only 1 packet.*/
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* Set analyzer interface index to 0 and enable Rx mirroring */
    rc = prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet(GT_TRUE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet: %d %d", GT_TRUE, 0);

    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that port 23 received 2 packets */
        if (PRV_TGF_RCV_PORT_IDX_CNS == portIter)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount * 2, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* Disable Rx mirroring on port 8. */
    rc = prvTgfMirrorRxPortSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxPortSet: %d %d",
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE);

    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that all ports except of send port received only 1 packet.*/
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }
}

/*******************************************************************************
* prvTgfIngressMirrorHopByHopConfigurationRestore
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
GT_VOID prvTgfIngressMirrorHopByHopConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS rc;

    /* restore Rx mirroring */
    rc = prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet(prvTgfEnableMirror, prvTgfIndex);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet: %d", prvTgfEnableMirror);

    /* Disable Rx mirroring on port 8. */
    rc = prvTgfMirrorRxPortSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxPortSet: %d %d",
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE);

    /* invalidate VLAN entry */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d",
                                 PRV_TGF_VLANID_CNS);
}

/*******************************************************************************
* prvTgfIngressMirrorSourceBasedConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*              - Set source-based forwarding mode.
*              - Set analyzer interface for index 6.
*           Device number 0, port number 18, port interface.
*              - Set analyzer interface for index 0.
*           Device number 0, port number 23, port interface.
*              - Enable Rx mirroring on port 8 and set analyzer
*           interface index for the port to 6.
*         - Enable Rx mirroring on port 0 and set analyzer interface
*           index for the port to 0.
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
GT_VOID prvTgfIngressMirrorSourceBasedConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;


    /* set VLAN entry */
    rc = prvTgfBrgDefVlanEntryWrite(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d",
                                 PRV_TGF_VLANID_CNS);

    /*  Set source-based forwarding mode. */
    rc = prvTgfMirrorToAnalyzerForwardingModeSet(PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorToAnalyzerForwardingModeSet: %d",
                                 PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E);

    /* Set analyzer interface for index 6.
        Device number 0, port number 18, port interface. */
    prvMirrorAnalyzerInterfaceSet(6, prvTgfDevsArray[2], prvTgfPortsArray[2]);

    /* Set analyzer interface for index 0.
     Device number 0, port number 23, port interface. */
    prvMirrorAnalyzerInterfaceSet(0, prvTgfDevsArray[3], prvTgfPortsArray[3]);

    /* Enable Rx mirroring on port 8 and set analyzer interface index for the port to 6. */
    rc = prvTgfMirrorRxPortSet(prvTgfPortsArray[1], GT_TRUE, 6);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxPortSet: %d %d %d",
                                 prvTgfPortsArray[1], GT_TRUE, 6);

    /* Enable Rx mirroring on port 0 and set analyzer interface index for the port to 0. */
    rc = prvTgfMirrorRxPortSet(prvTgfPortsArray[0], GT_TRUE, 0);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxPortSet: %d %d %d",
                                 prvTgfPortsArray[0], GT_TRUE, 0);
}

/*******************************************************************************
* prvTgfIngressMirrorSourceBasedTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 23.
*                2 Packets are captured on port 18.
*           Send to device's port 0 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 18.
*                2 Packets are captured on port 23.
*       Disable Rx mirroring on port 8.
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                Packet is captured on ports 0, 18, 23.
*       Disable Rx mirroring on port 0.
*           Send to device's port 0 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                Packet is captured on ports 8, 18, 23.
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
GT_VOID prvTgfIngressMirrorSourceBasedTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS    rc = GT_OK;
    GT_U32       portIter = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC portCntrs;


    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that port 18 received 2 packets */
        if (PRV_TGF_RCV1_PORT_IDX_CNS == portIter)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount * 2, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_ANOTHER_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_ANOTHER_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that port 23 received 2 packets */
        if (PRV_TGF_RCV_PORT_IDX_CNS == portIter)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount * 2, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
    }

    /* Disable Rx mirroring on port 8 */
    rc = prvTgfMirrorRxPortSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "cpssDxChMirrorRxPortSet: %d %d",
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE);

    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that all ports except of send port received only 1 packet.*/
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* Disable Rx mirroring on port 0 */
    rc = prvTgfMirrorRxPortSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_ANOTHER_CNS], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "cpssDxChMirrorRxPortSet: %d %d",
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_ANOTHER_CNS], GT_FALSE);

    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_ANOTHER_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_ANOTHER_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that all ports except of send port received only 1 packet.*/
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }
}

/*******************************************************************************
* prvTgfIngressMirrorSourceBasedConfigurationRestore
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
GT_VOID prvTgfIngressMirrorSourceBasedConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS rc;

    /*  Set hop-by-hop (default) forwarding mode. */
    rc = prvTgfMirrorToAnalyzerForwardingModeSet(PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorToAnalyzerForwardingModeSet: %d",
                                 PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E);

    /* Disable Rx mirroring on port 8 */
    rc = prvTgfMirrorRxPortSet(prvTgfPortsArray[1], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxPortSet: %d %d",
                                 prvTgfPortsArray[1], GT_FALSE);

    /* Disable Rx mirroring on port 0 */
    rc = prvTgfMirrorRxPortSet(prvTgfPortsArray[0], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxPortSet: %d %d",
                                 prvTgfPortsArray[0], GT_FALSE);

    /* invalidate VLAN entry */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d",
                                 PRV_TGF_VLANID_CNS);
}

/*******************************************************************************
* prvTgfIngressMirrorHighestIndexSelectionConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*              - Set source-based forwarding mode.
*              - Set analyzer interface for index 3.
*           Device number 0, port number 18, port interface.
*              - Set analyzer interface for index 1.
*           Device number 0, port number 23, port interface.
*              - Enable Rx mirroring on port 8 and set analyzer
*           interface index for the port to 1.
*         - Set FDB entry with MAC address 00:00:00:00:00:01
*           and mirrorToRxAnalyzerPortEn = GT_TRUE.
*         -        Set analyzer interface index to 3 and enable Rx mirroring.
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
GT_VOID prvTgfIngressMirrorHighestIndexSelectionConfigurationSet
(
    GT_VOID
)
{
    PRV_TGF_BRG_MAC_ENTRY_STC prvTgfMacEntry;       /* FDB MAC entry */
    GT_STATUS               rc = GT_OK;


    /* set VLAN entry */
    rc = prvTgfBrgDefVlanEntryWrite(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d",
                                 PRV_TGF_VLANID_CNS);

    /* clear entry */
    cpssOsMemSet(&prvTgfMacEntry, 0, sizeof(prvTgfMacEntry));
    /* we need to make the entry static otherwise the AppDemo will get message
       about station movement and will replace the entry.
       in multi-port group FDB this will cause the removal of the entry from 3
       port groups and will be left only to single port group.
       */
    prvTgfMacEntry.isStatic = GT_TRUE;
    /* Set FDB entry with MAC address 00:00:00:00:00:02
       and mirrorToRxAnalyzerPortEn = GT_TRUE. */
    prvTgfMacEntry.mirrorToRxAnalyzerPortEn     = GT_TRUE;
    prvTgfMacEntry.key.key.macVlan.vlanId = PRV_TGF_VLANID_CNS;
    cpssOsMemCpy(prvTgfMacEntry.key.key.macVlan.macAddr.arEther, prvTgfPacketL2Part.saMac, sizeof(TGF_MAC_ADDR));
    rc = prvTgfBrgFdbMacEntrySet(&prvTgfMacEntry);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbMacEntrySet");

    /*  Set source-based forwarding mode. */
    rc = prvTgfMirrorToAnalyzerForwardingModeSet(PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorToAnalyzerForwardingModeSet: %d",
                                 PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E);

    /* Set analyzer interface for index 3.
        Device number 0, port number 18, port interface. */
    prvMirrorAnalyzerInterfaceSet(3, prvTgfDevsArray[2], prvTgfPortsArray[2]);

    /* Set analyzer interface for index 1.
     Device number 0, port number 23, port interface. */
    prvMirrorAnalyzerInterfaceSet(1, prvTgfDevsArray[3], prvTgfPortsArray[3]);

    /* Enable Rx mirroring on port 8 and set analyzer interface index for the port to 1. */
    rc = prvTgfMirrorRxPortSet(prvTgfPortsArray[1], GT_TRUE, 1);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxPortSet: %d %d %d",
                                 prvTgfPortsArray[1], GT_TRUE, 1);

    /* save default analyzer interface */
    rc = prvTgfMirrorRxGlobalAnalyzerInterfaceIndexGet(prvTgfDevNum, &prvTgfEnableMirror, &prvTgfIndex);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxGlobalAnalyzerInterfaceIndexGet: %d", prvTgfDevNum);

    /* Set analyzer interface index to 3 and enable Rx mirroring */
    rc = prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet(GT_TRUE, 3);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet: %d %d", GT_TRUE, 3);
}

/*******************************************************************************
* prvTgfIngressMirrorHighestIndexSelectionTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 23.
*                2 Packets are captured on port 18.
*           Send to device's port 0 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:01,
*           Success Criteria:
*                1 packet is captured on ports 0, 18.
*                2 Packets are captured on port 23.
*         - Set analyzer interface index to 0 and enable Rx mirroring.
*           Send to device's port 0 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 18.
*                2 Packets are captured on port 23.
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
GT_VOID prvTgfIngressMirrorHighestIndexSelectionTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS    rc = GT_OK;
    GT_U32       portIter = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC portCntrs;
    TGF_MAC_ADDR    saMac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
    GT_U32                          expectedCount;/* expected count*/
    PRV_TGF_MIRROR_ANALYZER_INTERFACE_STC   mirror0Interface;/* mirror interface in index 0 */

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {

            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        if(prvTgfNumOfPortGroups && usePortGroupsBmp == GT_FALSE && prvTgfDevPortForceNum)
        {
            /* the test added ring ports , and since the packet did flooding in
            the port groups , each port group will have FDB match on SA lookup
            that will result mirror to analyzer */
            expectedCount = prvTgfBurstCount * (prvTgfNumOfPortGroups - 1);
        }
        else
        {
            expectedCount = 0;
        }

        /* check Tx counters */
        /* Verify that port 18 received 2 packets - FDB match - max(1, 3). */
        if (PRV_TGF_RCV1_PORT_IDX_CNS == portIter)
        {
            expectedCount += (prvTgfBurstCount * 2);
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(expectedCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            expectedCount = prvTgfBurstCount;
            UTF_VERIFY_EQUAL0_STRING_MAC(expectedCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* Set analyzer interface index to 0 and enable Rx mirroring */
    rc = prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet(GT_TRUE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet: %d %d", GT_TRUE, 0);

    /* get analyzer interface in index 0 */
    rc = prvTgfMirrorAnalyzerInterfaceGet(prvTgfDevNum, 0, &mirror0Interface);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorAnalyzerInterfaceGet: %d", prvTgfDevNum);

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    cpssOsMemCpy(prvTgfPacketL2Part.saMac, saMac, sizeof(TGF_MAC_ADDR));

    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        expectedCount = 0;

        if(prvTgfNumOfPortGroups && usePortGroupsBmp == GT_FALSE && prvTgfDevPortForceNum &&
            (mirror0Interface.interface.type == CPSS_INTERFACE_PORT_E &&
             prvTgfDevsArray[portIter] == mirror0Interface.interface.devPort.devNum &&
             prvTgfPortsArray[portIter] == mirror0Interface.interface.devPort.portNum))
        {
            /* this port is the analyzer port of index 0 */
            /* this port will get extra copies due to mirror from the FDB on SA match */

            /* the test added ring ports , and since the packet did flooding in
            the port groups , each port group will have FDB match on SA lookup
            that will result mirror to analyzer */
            expectedCount += prvTgfBurstCount * (prvTgfNumOfPortGroups - 1);
        }

        /* check Tx counters */
        /* Verify that port 23 received 2 packets  - (FDB match - max(0, 1).)*/
        if (PRV_TGF_RCV_PORT_IDX_CNS == portIter)
        {
            expectedCount += (prvTgfBurstCount * 2);
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(expectedCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            expectedCount += prvTgfBurstCount;
            UTF_VERIFY_EQUAL0_STRING_MAC(expectedCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }
}

/*******************************************************************************
* prvTgfIngressMirrorHighestIndexSelectionConfigurationRestore
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
GT_VOID prvTgfIngressMirrorHighestIndexSelectionConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS rc;


    /* Disable Rx mirroring on port 8 */
    rc = prvTgfMirrorRxPortSet(prvTgfPortsArray[1], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxPortSet: %d %d",
                                 prvTgfPortsArray[1], GT_FALSE);

    /* restore Rx mirroring */
    rc = prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet(prvTgfEnableMirror, prvTgfIndex);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet: %d", prvTgfEnableMirror);

    /*  Set hop-by-hop forwarding mode. */
    rc = prvTgfMirrorToAnalyzerForwardingModeSet(PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorToAnalyzerForwardingModeSet: %d",
                                 PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E);

    /* invalidate VLAN entry */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d",
                                 PRV_TGF_VLANID_CNS);

    /* flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d", GT_TRUE);
}

/*******************************************************************************
* prvTgfEgressMirrorHopByHopConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set analyzer interface for index 2.
*           Device number 0, port number 18, port interface.
*         -        Set analyzer interface for index 0.
*           Device number 0, port number 23, port interface.
*              - Set analyzer interface index to 2 and enable Tx mirroring.
*              - Enable Tx mirroring on port 0.
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
GT_VOID prvTgfEgressMirrorHopByHopConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS               rc = GT_OK;


    /* set VLAN entry */
    rc = prvTgfBrgDefVlanEntryWrite(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d",
                                 PRV_TGF_VLANID_CNS);

    /* Set analyzer interface for index 2.
       Device number 0, port number 18, port interface.  */
    prvMirrorAnalyzerInterfaceSet(2, prvTgfDevsArray[2], prvTgfPortsArray[2]);

    /* Set analyzer interface for index 0.
       Device number 0, port number 23, port interface. */
    prvMirrorAnalyzerInterfaceSet(0, prvTgfDevsArray[3], prvTgfPortsArray[3]);

    /* Set analyzer interface index to 2 and enable Tx mirroring. */
    rc = prvTgfMirrorTxGlobalAnalyzerInterfaceIndexSet(GT_TRUE, 2);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxGlobalAnalyzerInterfaceIndexSet: %d %d", GT_TRUE, 2);

    /* Enable Tx mirroring on port 0. */
    rc = prvTgfMirrorTxPortSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_ANOTHER_CNS], GT_TRUE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxPortSet: %d %d",
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_ANOTHER_CNS], GT_TRUE);
}

/*******************************************************************************
* prvTgfEgressMirrorHopByHopTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 23.
*                2 Packets are captured on port 18.
*       Disable Tx mirroring.
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                Packet is captured on ports 0, 18, 23.
*       Set analyzer interface index to 0 and enable  Tx mirroring.
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 18.
*                2 Packets are captured on port 23.
*       Disable Tx mirroring on port 0.
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                Packet is captured on ports 0, 18, 23.
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
GT_VOID prvTgfEgressMirrorHopByHopTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS    rc = GT_OK;
    GT_U32       portIter = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC portCntrs;


    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that port 18 received 2 packets */
        if (PRV_TGF_RCV1_PORT_IDX_CNS == portIter)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount * 2, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }

    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* Disable Tx mirroring */
    rc = prvTgfMirrorTxGlobalAnalyzerInterfaceIndexSet(GT_FALSE, 0);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxGlobalAnalyzerInterfaceIndexSet: %d", GT_FALSE);

    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that all ports except of send port received only 1 packet.*/
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* Set analyzer interface index to 0 and enable Tx mirroring */
    rc = prvTgfMirrorTxGlobalAnalyzerInterfaceIndexSet(GT_TRUE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxGlobalAnalyzerInterfaceIndexSet: %d %d", GT_TRUE, 0);
    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that port 23 received 2 packets */
        if (PRV_TGF_RCV_PORT_IDX_CNS == portIter)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount * 2, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
    }
    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }
    /* Disable Tx mirroring on port 0. */
    rc = prvTgfMirrorTxPortSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_ANOTHER_CNS], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxPortSet: %d %d",
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_ANOTHER_CNS], GT_FALSE);

    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that all ports except of send port received only 1 packet.*/
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }
}

/*******************************************************************************
* prvTgfEgressMirrorHopByHopConfigurationRestore
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
GT_VOID prvTgfEgressMirrorHopByHopConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS rc;

    /* Disable Tx mirroring. */
    rc = prvTgfMirrorTxGlobalAnalyzerInterfaceIndexSet(GT_FALSE, 0);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxGlobalAnalyzerInterfaceIndexSet: %d", GT_FALSE);

    /* Disable Tx mirroring on port 0. */
    rc = prvTgfMirrorTxPortSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_ANOTHER_CNS], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxPortSet: %d %d",
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_ANOTHER_CNS], GT_FALSE);

    /* invalidate VLAN entry */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d",
                                 PRV_TGF_VLANID_CNS);
}

/*******************************************************************************
* prvTgfEgressMirrorSourceBasedConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*              - Set source-based forwarding mode.
*         -        Set analyzer interface for index 5.
*           Device number 0, port number 17, port interface.
*         - Set analyzer interface for index 0.
*           Device number 0, port number 0, port interface.
*         -        Enable Tx mirroring on port 23 and
*           set analyzer interface index for the port to 5.
*         -        Enable Tx mirroring on port 18 and
*           set analyzer interface index for the port to 0.
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
GT_VOID prvTgfEgressMirrorSourceBasedConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS               rc = GT_OK;

    /* test uses 5 ports. Store 5th ports */
    prvTgfPortsNumSave = prvTgfPortsNum;
    prvTgfPortsArraySave[4] = prvTgfPortsArray[4];
    if(prvTgfNumOfPortGroups && usePortGroupsBmp == GT_FALSE && prvTgfDevPortForceNum)
    {
        prvTgfPortsArray[4] = 40;/* use port 40 and not 17 because 17 uses as ring port */
    }
    else
    {
        prvTgfPortsArray[4] = 17;
    }
    prvTgfPortsNum = 5;

    /* set VLAN entry */
    rc = prvTgfBrgDefVlanEntryWrite(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d",
                                 PRV_TGF_VLANID_CNS);

    /*  Set source-based forwarding mode. */
    rc = prvTgfMirrorToAnalyzerForwardingModeSet(PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorToAnalyzerForwardingModeSet: %d",
                                 PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E);

    /* Set analyzer interface for index 5.
        Device number 0, port number 17, port interface. */
    prvMirrorAnalyzerInterfaceSet(5, prvTgfDevsArray[4], prvTgfPortsArray[4]);

    /* Set analyzer interface for index 0.
     Device number 0, port number 0, port interface. */
    prvMirrorAnalyzerInterfaceSet(0, prvTgfDevsArray[0], prvTgfPortsArray[0]);

    /* Enable Tx mirroring on port 23 and
       set analyzer interface index for the port to 5. */
    rc = prvTgfMirrorTxPortSet(prvTgfPortsArray[PRV_TGF_TX_MIRR_PORT2_IDX_CNS], GT_TRUE, 5);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxPortSet: %d %d %d",
                                 prvTgfPortsArray[PRV_TGF_TX_MIRR_PORT2_IDX_CNS], GT_TRUE, 5);

    /* Enable Tx mirroring on port 18 and
       set analyzer interface index for the port to 0. */
    rc = prvTgfMirrorTxPortSet(prvTgfPortsArray[PRV_TGF_TX_MIRR_PORT1_IDX_CNS], GT_TRUE, 0);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxPortSet: %d %d %d",
                                 prvTgfPortsArray[PRV_TGF_TX_MIRR_PORT1_IDX_CNS], GT_TRUE, 0);

    /* remove analyzer ports from VLAN */
    rc = prvTgfBrgVlanMemberRemove(prvTgfDevNum, PRV_TGF_VLANID_CNS, prvTgfPortsArray[0]);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanMemberRemove: %d%d",
                                 PRV_TGF_VLANID_CNS, prvTgfPortsArray[0]);


    rc = prvTgfBrgVlanMemberRemove(prvTgfDevNum, PRV_TGF_VLANID_CNS, prvTgfPortsArray[4]);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanMemberRemove: %d%d",
                                 PRV_TGF_VLANID_CNS, prvTgfPortsArray[4]);

}

/*******************************************************************************
* prvTgfEgressMirrorSourceBasedTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Learn MACs UC1 and UC2 on Tx mirrored ports 18 and 23
*           Send to device's port 8 two packets:
*               macDa = UC1,macDa = UC2,
*           Success Criteria:
*                1 packet is captured o n ports Tx mirrored 23.
*                1 Packets are captured on port Tx mirrored 18.
*                1 Packets are captured on analyzer port 0.
*                1 Packets are captured on analyzer port 17.
*       Disable Tx mirroring on port 18.
*           Send to device's port 8 two packets:
*               macDa = UC1,macDa = UC2,
*           Success Criteria:
*                1 packet is captured o n ports Tx mirrored 23.
*                1 Packets are captured on port Tx mirrored 18.
*                0 Packets are captured on analyzer port 0.
*                1 Packets are captured on analyzer port 17.
*       Disable Tx mirroring on port 23.
*           Send to device's port 8 two packets:
*               macDa = UC1,macDa = UC2,
*           Success Criteria:
*                1 packet is captured o n ports Tx mirrored 23.
*                1 Packets are captured on port Tx mirrored 18.
*                0 Packets are captured on analyzer port 0.
*                0 Packets are captured on analyzer port 17.
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
GT_VOID prvTgfEgressMirrorSourceBasedTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS    rc = GT_OK;
    GT_U32       portIter = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC portCntrs;

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* learn MAC addresses on Tx mirrored ports */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_TX_MIRR_PORT1_IDX_CNS], &prvTgfPacketInfoUC1Learn);
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_TX_MIRR_PORT2_IDX_CNS], &prvTgfPacketInfoUC2Learn);

    /* wait to learn packets */
    cpssOsTimerWkAfter(1);

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* send 2 UC packets */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_TX_MIRR_SEND_IDX_CNS], &prvTgfPacketInfoUC1);
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_TX_MIRR_SEND_IDX_CNS], &prvTgfPacketInfoUC2);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_TX_MIRR_SEND_IDX_CNS == portIter)
        {

            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount*2, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that analyzer ports 17, 0 received 1 packet.
           And other ports received 1 packet as VLAN members. */
        UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                     "get another goodPktsSent counter than expected");
    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* Disable Tx mirroring on port 18 */
    rc = prvTgfMirrorTxPortSet(prvTgfPortsArray[PRV_TGF_TX_MIRR_PORT1_IDX_CNS], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxPortSet: %d %d",
                                 prvTgfPortsArray[PRV_TGF_TX_MIRR_PORT1_IDX_CNS], GT_FALSE);
    /* send 2 UC packets */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_TX_MIRR_SEND_IDX_CNS], &prvTgfPacketInfoUC1);
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_TX_MIRR_SEND_IDX_CNS], &prvTgfPacketInfoUC2);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_TX_MIRR_SEND_IDX_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount*2, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that port 0 do not receive packets because it's not
           VLAN member and mirroring to it is disabled */
        if (portIter == 0)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
    }

    /* Disable Tx mirroring on port 23 */
    rc = prvTgfMirrorTxPortSet(prvTgfPortsArray[PRV_TGF_TX_MIRR_PORT2_IDX_CNS], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxPortSet: %d %d",
                                 prvTgfPortsArray[PRV_TGF_TX_MIRR_PORT2_IDX_CNS], GT_FALSE);

    /* send 2 UC packets */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_TX_MIRR_SEND_IDX_CNS], &prvTgfPacketInfoUC1);
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_TX_MIRR_SEND_IDX_CNS], &prvTgfPacketInfoUC2);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_TX_MIRR_SEND_IDX_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount*2, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that port 0 and 17 do not receive packets because they are not
           VLAN members and mirroring to them is disabled */
        if ((portIter == 0) || (portIter == 4))
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }
}

/*******************************************************************************
* prvTgfEgressMirrorSourceBasedConfigurationRestore
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
GT_VOID prvTgfEgressMirrorSourceBasedConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS rc;

    /*  Set hop-by-hop (default) forwarding mode. */
    rc = prvTgfMirrorToAnalyzerForwardingModeSet(PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorToAnalyzerForwardingModeSet: %d",
                                 PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E);

    /* Disable Tx mirroring on port 18. */
    rc = prvTgfMirrorTxPortSet(prvTgfPortsArray[PRV_TGF_TX_MIRR_PORT1_IDX_CNS], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxPortSet: %d %d",
                                 prvTgfPortsArray[PRV_TGF_TX_MIRR_PORT1_IDX_CNS], GT_FALSE);

    /* Disable Tx mirroring on port 23. */
    rc = prvTgfMirrorTxPortSet(prvTgfPortsArray[PRV_TGF_TX_MIRR_PORT2_IDX_CNS], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxPortSet: %d %d",
                                 prvTgfPortsArray[PRV_TGF_TX_MIRR_PORT2_IDX_CNS], GT_FALSE);

    /* flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d, %d", prvTgfDevNum, GT_TRUE);

    /* invalidate VLAN entry */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d",
                                 PRV_TGF_VLANID_CNS);

    /* test uses 5 ports. Restore 5th ports */
    prvTgfPortsNum = prvTgfPortsNumSave;
    prvTgfPortsArray[4] = prvTgfPortsArraySave[4];
}

/*******************************************************************************
* prvTgfIngressMirrorForwardingModeChangeConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set analyzer interface for index 3.
*            Device number 0, port number 18, port interface.
*         -        Set analyzer interface for index 0.
*            Device number 0, port number 23, port interface.
*         -        Set analyzer interface index to 3 and enable Rx mirroring.
*         - Enable Rx mirroring on port 8.
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
GT_VOID prvTgfIngressMirrorForwardingModeChangeConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS               rc = GT_OK;


    /* set VLAN entry */
    rc = prvTgfBrgDefVlanEntryWrite(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d",
                                 PRV_TGF_VLANID_CNS);

    /* Set analyzer interface for index 3.
        Device number 0, port number 18, port interface. */
    prvMirrorAnalyzerInterfaceSet(3, prvTgfDevsArray[2], prvTgfPortsArray[2]);

    /* Set analyzer interface for index 4.
     Device number 0, port number 23, port interface. */
    prvMirrorAnalyzerInterfaceSet(4, prvTgfDevsArray[3], prvTgfPortsArray[3]);

    /* save default analyzer interface */
    rc = prvTgfMirrorRxGlobalAnalyzerInterfaceIndexGet(prvTgfDevNum, &prvTgfEnableMirror, &prvTgfIndex);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxGlobalAnalyzerInterfaceIndexGet: %d", prvTgfDevNum);

    /* Set analyzer interface index to 3 and enable Rx mirroring */
    rc = prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet(GT_TRUE, 3);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet: %d %d", GT_TRUE, 3);

    /* Enable Rx mirroring on port 8. */
    rc = prvTgfMirrorRxPortSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_TRUE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxPortSet: %d %d",
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_TRUE);
}

/*******************************************************************************
* prvTgfIngressMirrorForwardingModeChangeTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 23.
*                2 Packets are captured on port 18.
*       Disable Rx mirroring on port 8.
*       Set source-based forwarding mode.
*       Enable Rx mirroring on port 8 and set
*       analyzer interface index for the port to 0.
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                Packet is captured on ports 0, 18.
*                2 Packets are captured on port 23.
*       Disable Rx mirroring on port 8.
*       Set hop-byhop forwarding mode.
*       Enable Rx mirroring on port 8.
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 23.
*                2 Packets are captured on port 18.

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
GT_VOID prvTgfIngressMirrorForwardingModeChangeTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS    rc = GT_OK;
    GT_U32       portIter = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC portCntrs;


    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that port 18 received 2 packets */
        if (PRV_TGF_RCV1_PORT_IDX_CNS == portIter)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount * 2, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* Disable Rx mirroring on port 8. */
    rc = prvTgfMirrorRxPortSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxPortSet: %d %d",
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE);

    /*  Set source-based forwarding mode. */
    rc = prvTgfMirrorToAnalyzerForwardingModeSet(PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorToAnalyzerForwardingModeSet: %d",
                                 PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E);

    /* Enable Rx mirroring on port 8 and set analyzer interface index for the port to 4. */
    rc = prvTgfMirrorRxPortSet(prvTgfPortsArray[1], GT_TRUE, 4);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxPortSet: %d %d %d",
                                 prvTgfPortsArray[1], GT_TRUE, 4);

    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that port 23 received 2 packets */
        if (PRV_TGF_RCV_PORT_IDX_CNS == portIter)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount * 2, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* Disable Rx mirroring on port 8. */
    rc = prvTgfMirrorRxPortSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxPortSet: %d %d",
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE);

    /*  Set hop-by-hop forwarding mode. */
    rc = prvTgfMirrorToAnalyzerForwardingModeSet(PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorToAnalyzerForwardingModeSet: %d",
                                 PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E);

    /* Enable Rx mirroring on port 8 */
    rc = prvTgfMirrorRxPortSet(prvTgfPortsArray[1], GT_TRUE, 0);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxPortSet: %d %d %d",
                                 prvTgfPortsArray[1], GT_TRUE, 0);

    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that port 18 received 2 packets */
        if (PRV_TGF_RCV1_PORT_IDX_CNS == portIter)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount * 2, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }
}

/*******************************************************************************
* prvTgfIngressMirrorForwardingModeChangeConfigurationRestore
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
GT_VOID prvTgfIngressMirrorForwardingModeChangeConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS rc;

    /* restore Rx mirroring */
    rc = prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet(prvTgfEnableMirror, prvTgfIndex);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet: %d", prvTgfEnableMirror);

    /* Disable Rx mirroring on port 8. */
    rc = prvTgfMirrorRxPortSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxPortSet: %d %d",
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE);

    /* invalidate VLAN entry */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d",
                                 PRV_TGF_VLANID_CNS);

}

/*******************************************************************************
* prvTgfEgressMirrorForwardingModeChangeConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set analyzer interface for index 3.
*            Device number 0, port number 18, port interface.
*         -        Set analyzer interface for index 0.
*            Device number 0, port number 23, port interface.
*         -        Set analyzer interface index to 3 and enable Tx mirroring.
*         - Enable Tx mirroring on port 0.
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
GT_VOID prvTgfEgressMirrorForwardingModeChangeConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS               rc = GT_OK;


    /* set VLAN entry */
    rc = prvTgfBrgDefVlanEntryWrite(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d",
                                 PRV_TGF_VLANID_CNS);

    /* Set analyzer interface for index 3.
        Device number 0, port number 18, port interface. */
    prvMirrorAnalyzerInterfaceSet(3, prvTgfDevsArray[2], prvTgfPortsArray[2]);

    /* Set analyzer interface for index 0.
     Device number 0, port number 23, port interface. */
    prvMirrorAnalyzerInterfaceSet(0, prvTgfDevsArray[3], prvTgfPortsArray[3]);

    /* Set analyzer interface index to 3 and enable Tx mirroring */
    rc = prvTgfMirrorTxGlobalAnalyzerInterfaceIndexSet(GT_TRUE, 3);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxGlobalAnalyzerInterfaceIndexSet: %d %d", GT_TRUE, 3);

    /* Enable Tx mirroring on port 0. */
    rc = prvTgfMirrorTxPortSet(prvTgfPortsArray[0], GT_TRUE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxPortSet: %d %d",
                                 prvTgfPortsArray[0], GT_TRUE);
}

/*******************************************************************************
* prvTgfEgressMirrorForwardingModeChangeTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 23.
*                2 Packets are captured on port 18.
*       Disable Tx mirroring on port 0.
*       Set source-based forwarding mode.
*       Enable Tx mirroring on port 0 and set
*       analyzer interface index for the port to 0.
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                Packet is captured on ports 0, 18.
*                2 Packets are captured on port 23.
*       Disable Tx mirroring on port 0.
*       Set hop-byhop forwarding mode.
*       Enable Tx mirroring on port 0.
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 23.
*                2 Packets are captured on port 18.

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
GT_VOID prvTgfEgressMirrorForwardingModeChangeTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS    rc = GT_OK;
    GT_U32       portIter = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC portCntrs;


    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that port 18 received 2 packets */
        if (PRV_TGF_RCV1_PORT_IDX_CNS == portIter)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount * 2, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* Disable Tx mirroring on port 0. */
    rc = prvTgfMirrorTxPortSet(prvTgfPortsArray[0], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxPortSet: %d %d", prvTgfPortsArray[0], GT_FALSE);

    /*  Set source-based forwarding mode. */
    rc = prvTgfMirrorToAnalyzerForwardingModeSet(PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorToAnalyzerForwardingModeSet: %d",
                                 PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E);

    /* Enable Tx mirroring on port 0 and set analyzer interface index for the port to 0. */
    rc = prvTgfMirrorTxPortSet(prvTgfPortsArray[0], GT_TRUE, 0);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxPortSet: %d %d %d",
                                 prvTgfPortsArray[0], GT_TRUE, 0);

    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that port 23 received 2 packets */
        if (PRV_TGF_RCV_PORT_IDX_CNS == portIter)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount * 2, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }

    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* Disable Tx mirroring on port 0. */
    rc = prvTgfMirrorTxPortSet(prvTgfPortsArray[0], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxPortSet: %d %d", prvTgfPortsArray[0], GT_FALSE);

    /*  Set hop-by-hop forwarding mode. */
    rc = prvTgfMirrorToAnalyzerForwardingModeSet(PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorToAnalyzerForwardingModeSet: %d",
                                 PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E);

    /* Enable Tx mirroring on port 0 */
    rc = prvTgfMirrorTxPortSet(prvTgfPortsArray[0], GT_TRUE, 0);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxPortSet: %d %d %d",
                                 prvTgfPortsArray[0], GT_TRUE, 0);

    /* send packet */
    prvTgfMirrorTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that port 18 received 2 packets */
        if (PRV_TGF_RCV1_PORT_IDX_CNS == portIter)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount * 2, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }
}

/*******************************************************************************
* prvTgfEgressMirrorForwardingModeChangeConfigurationRestore
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
GT_VOID prvTgfEgressMirrorForwardingModeChangeConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS rc;

    /* Disable Tx mirroring */
    rc = prvTgfMirrorTxGlobalAnalyzerInterfaceIndexSet(GT_FALSE, 3);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxGlobalAnalyzerInterfaceIndexSet: %d", GT_FALSE);

    /* Disable Tx mirroring on port 8. */
    rc = prvTgfMirrorTxPortSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxPortSet: %d %d",
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE);

    /* Disable Tx mirroring on port 0. */
    rc = prvTgfMirrorTxPortSet(prvTgfPortsArray[0], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxPortSet: %d %d",
                                 prvTgfPortsArray[0], GT_FALSE);

    /* invalidate VLAN entry */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d",
                                 PRV_TGF_VLANID_CNS);
}

/*******************************************************************************
* prvTgfIngressMirrorVlanTagRemovalConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Create VLAN 5 with all ports.
*         - Remove port 18 from VLAN 5.
*              - Set analyzer interface for index 3.
*           Device number 0, port number 18, port interface.
*         - Enable Rx mirroring on port 8.
*         -        Set analyzer interface index to 3 and enable Rx mirroring.
*         - Enable VLAN tag removal of mirrored traffic.
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
GT_VOID prvTgfIngressMirrorVlanTagRemovalConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS               rc = GT_OK;


    /* set VLAN entry */
    rc = prvTgfBrgDefVlanEntryWrite(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d",
                                 PRV_TGF_VLANID_CNS);

    /* remove port 18 from VLAN 5 */
    rc = prvTgfBrgVlanMemberRemove(prvTgfDevNum, PRV_TGF_VLANID_CNS, prvTgfPortsArray[2]);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d%d",
                                 PRV_TGF_VLANID_CNS, prvTgfPortsArray[2]);

    /* enable PCL ingress Policy */
    rc = prvTgfPclIngressPolicyEnable(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclIngressPolicyEnable: %d", GT_TRUE);

    /* Enable Rx mirroring on port 8. */
    rc = prvTgfMirrorRxPortSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_TRUE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxPortSet: %d %d",
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_TRUE);

    /* Set analyzer interface for index 3.
        Device number 0, port number 18, port interface. */
    prvMirrorAnalyzerInterfaceSet(3, prvTgfDevsArray[2], prvTgfPortsArray[2]);

    /* Set analyzer interface index to 3 and enable Rx mirroring */
    rc = prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet(GT_TRUE, 3);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet: %d %d", GT_TRUE, 3);

    /*  Enable VLAN tag removal of mirrored traffic */
    rc = prvTgfMirrorAnalyzerVlanTagRemoveEnableSet(prvTgfPortsArray[2], GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfcpssDxChMirrorAnalyzerVlanTagRemoveEnableSet: %d%d", prvTgfPortsArray[2], GT_TRUE);

}

/*******************************************************************************
* prvTgfMirrorVlanTagRemovalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*           Success Criteria:
*               Untagged/Tagged packet is captured on port 18.
*
* INPUTS:
*       vfd - VFD info
*       ingressMirrorEnable - GT_TRUE - Generate traffic for Vlan Tag Removal
*                                       in Ingress mirroring
*                           - GT_FALSE - Generate traffic for Vlan Tag Removal
*                                       in Egress mirroring
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
static GT_VOID prvTgfMirrorVlanTagRemovalTrafficGenerate
(
    TGF_VFD_INFO_STC     vfd,
    GT_BOOL              ingressMirrorEnable
)
{
    GT_STATUS    rc = GT_OK;
    GT_U32       portIter = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC portCntrs;
    CPSS_INTERFACE_INFO_STC       portInterface;
    GT_U8                trigPacketBuff[TGF_RX_BUFFER_MAX_SIZE_CNS] = {0};
    GT_U32               buffLen = TGF_RX_BUFFER_MAX_SIZE_CNS;
    GT_U32               packetActualLength = 0;
    GT_U8                devNum;
    GT_U8                queue;
    TGF_NET_DSA_STC      rxParam;
    GT_BOOL              triggered;
    GT_U32               vfdNum = 1;
    TGF_CAPTURE_MODE_ENT mode;

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }



    /* enable packet trace */
    rc = tgfTrafficTracePacketByteSet(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in tgfTrafficTracePacketByteSet %d", GT_TRUE);

    /* clear table */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* enable capture */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = prvTgfPortsArray[2];


    if(ingressMirrorEnable == GT_TRUE)
    {
        mode = TGF_CAPTURE_MODE_PCL_E;
    }
    else
    {
        mode = TGF_CAPTURE_MODE_MIRRORING_E;
    }
    /* Start capturing Tx packets */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, mode, GT_TRUE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthCaptureSet: %d, %d, %d",
                                 portInterface.devPort.devNum, portInterface.devPort.portNum, GT_TRUE);

    /* setup Packet */
    rc = prvTgfSetTxSetupEth(prvTgfDevNum, &prvTgfPacketInfo, 1, 0, NULL);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "ERROR of SetTxSetup: %d, %d",
                                 prvTgfDevNum, 1, 0, NULL);

    /* send Packet from port 8 */
    rc = prvTgfStartTransmitingEth(prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);
    cpssOsTimerWkAfter(1);

    /* Stop capturing Tx packets */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, mode, GT_FALSE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorPortTxEthCaptureSet: %d, %d, %d",
                                 portInterface.devPort.devNum, portInterface.devPort.portNum, GT_FALSE);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }

        /* check Tx counters */
        /* Verify that port 18 received 1 packet */
        if (PRV_TGF_RCV1_PORT_IDX_CNS == portIter)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsRcv counter than expected");

            /* set max allowed buff len */
            buffLen = TGF_RX_BUFFER_MAX_SIZE_CNS;

            /* enable packet trace */
            rc = tgfTrafficTracePacketByteSet(GT_TRUE);
            UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in tgfTrafficTracePacketByteSet %d", GT_TRUE);



            /* Get entry from captured packet's table */
            rc = tgfTrafficGeneratorRxInCpuGet(TGF_PACKET_TYPE_CAPTURE_E,
                                               GT_TRUE, GT_TRUE, trigPacketBuff,
                                               &buffLen, &packetActualLength,
                                               &devNum, &queue,
                                               &rxParam);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK , rc , "Error in tgfTrafficGeneratorRxInCpuGet %d", TGF_PACKET_TYPE_CAPTURE_E);


            /* check triggers */
            rc = tgfTrafficGeneratorTxEthTriggerCheck(trigPacketBuff,
                                                      buffLen,
                                                      1,
                                                      &vfd,
                                                      &triggered);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "Error in tgfTrafficGeneratorTxEthTriggerCheck %d, %d",
                                         buffLen, vfdNum);

            UTF_VERIFY_EQUAL0_STRING_MAC(GT_TRUE, triggered,
                                   "Packet Tag remove is done");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
    }

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }


}

/*******************************************************************************
* prvTgfIngressMirrorVlanTagRemovalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*           Success Criteria:
*               Untagged packet is captured on port 18.
*           Disable VLAN tag removal of mirrored traffic.
*           Send to device's port 8 packet:
*           Success Criteria:
*               Tagged packet is captured on port 18.
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
GT_VOID prvTgfIngressMirrorVlanTagRemovalTrafficGenerate
(
    GT_VOID
)
{
    TGF_VFD_INFO_STC     vfd;
    GT_STATUS    rc = GT_OK;


    /* set VFD array of expected fields value in received packet*/
    vfd.cycleCount = 2;
    vfd.mode = TGF_VFD_MODE_STATIC_E;
    vfd.modeExtraInfo = 0;
    vfd.offset = 12;
    vfd.patternPtr[0] = 0x00;
    vfd.patternPtr[1] = 0x00;

    prvTgfMirrorVlanTagRemovalTrafficGenerate(vfd, GT_TRUE);

    cpssOsTimerWkAfter(10);

    /*  Disable VLAN tag removal of mirrored traffic */
    rc = prvTgfMirrorAnalyzerVlanTagRemoveEnableSet(prvTgfPortsArray[2], GT_FALSE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfcpssDxChMirrorAnalyzerVlanTagRemoveEnableSet: %d%d", prvTgfPortsArray[2], GT_FALSE);

    /* set VFD array of expected fields value in received packet*/
    vfd.cycleCount = 4;
    vfd.patternPtr[0] = 0x81;
    vfd.patternPtr[1] = 0x00;
    vfd.patternPtr[2] = 0x00;
    vfd.patternPtr[3] = 0x05;

    prvTgfMirrorVlanTagRemovalTrafficGenerate(vfd, GT_TRUE);

}


/*******************************************************************************
* prvTgfIngressMirrorVlanTagRemovalConfigurationRestore
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
GT_VOID prvTgfIngressMirrorVlanTagRemovalConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS rc;

    /* Disable Rx mirroring */
    rc = prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet(GT_FALSE, 3);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet: %d", GT_FALSE);

    /* Disable Rx mirroring on port 8. */
    rc = prvTgfMirrorRxPortSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorRxPortSet: %d %d",
                                 prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], GT_FALSE);

    /* disable PCL ingress Policy */
    rc = prvTgfPclIngressPolicyEnable(GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclIngressPolicyEnable: %d", GT_FALSE);

    /* invalidate VLAN entry */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d",
                                 PRV_TGF_VLANID_CNS);


    /* clear table */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");
}

/*******************************************************************************
* prvTgfEgressMirrorVlanTagRemovalConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Create VLAN 5 with all ports.
*         - Remove port 18 from VLAN 5.
*              - Set analyzer interface for index 3.
*           Device number 0, port number 18, port interface.
*         - Enable Rx mirroring on port 8.
*         -        Set analyzer interface index to 3 and enable Rx mirroring.
*         - Enable VLAN tag removal of mirrored traffic.
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
GT_VOID prvTgfEgressMirrorVlanTagRemovalConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS               rc = GT_OK;


    /* set VLAN entry */
    rc = prvTgfBrgDefVlanEntryWithPortsSet(PRV_TGF_VLANID_CNS,
                                           prvTgfPortsArray, NULL,
                                           prvTgfPortsArray, prvTgfPortsNum);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWithPortsSet: %d",
                                 prvTgfDevNum);

    /* remove port 18 from VLAN 5 */
    rc = prvTgfBrgVlanMemberRemove(prvTgfDevNum, PRV_TGF_VLANID_CNS, prvTgfPortsArray[2]);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d%d",
                                 PRV_TGF_VLANID_CNS, prvTgfPortsArray[2]);


    /* Enable Tx mirroring on port 23. */
    rc = prvTgfMirrorTxPortSet(prvTgfPortsArray[3], GT_TRUE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxPortSet: %d %d",
                                 prvTgfPortsArray[3], GT_TRUE);

    /* configure Tx mirroring port to be tagged one */
    rc = prvTgfBrgVlanMemberAdd(prvTgfDevNum, PRV_TGF_VLANID_CNS,
                                prvTgfPortsArray[3], GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanMemberAdd: %d %d",
                                 prvTgfPortsArray[3], GT_TRUE);

    /* Set analyzer interface for index 3.
        Device number 0, port number 18, port interface. */
    prvMirrorAnalyzerInterfaceSet(3, prvTgfDevsArray[2], prvTgfPortsArray[2]);

    /* Set analyzer interface index to 3 and enable Tx mirroring */
    rc = prvTgfMirrorTxGlobalAnalyzerInterfaceIndexSet(GT_TRUE, 3);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxGlobalAnalyzerInterfaceIndexSet: %d %d", GT_TRUE, 3);

    /*  Enable VLAN tag removal of mirrored traffic */
    rc = prvTgfMirrorAnalyzerVlanTagRemoveEnableSet(prvTgfPortsArray[2], GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfcpssDxChMirrorAnalyzerVlanTagRemoveEnableSet: %d%d", prvTgfPortsArray[2], GT_TRUE);

}

/*******************************************************************************
* prvTgfIngressMirrorVlanTagRemovalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*           Success Criteria:
*               Untagged packet is captured on port 18.
*           Disable VLAN tag removal of mirrored traffic.
*           Send to device's port 8 packet:
*           Success Criteria:
*               Tagged packet is captured on port 18.
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
GT_VOID prvTgfEgressMirrorVlanTagRemovalTrafficGenerate
(
    GT_VOID
)
{
    TGF_VFD_INFO_STC     vfd;
    GT_STATUS    rc = GT_OK;


    /* set VFD array of expected fields value in received packet*/
    vfd.cycleCount = 2;
    vfd.mode = TGF_VFD_MODE_STATIC_E;
    vfd.modeExtraInfo = 0;
    vfd.offset = 12;
    vfd.patternPtr[0] = 0x00;
    vfd.patternPtr[1] = 0x00;

    prvTgfMirrorVlanTagRemovalTrafficGenerate(vfd, GT_FALSE);

    cpssOsTimerWkAfter(10);

    /*  Disable VLAN tag removal of mirrored traffic */
    rc = prvTgfMirrorAnalyzerVlanTagRemoveEnableSet(prvTgfPortsArray[2], GT_FALSE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfcpssDxChMirrorAnalyzerVlanTagRemoveEnableSet: %d%d", prvTgfPortsArray[2], GT_FALSE);

    /* set VFD array of expected fields value in received packet*/
    vfd.cycleCount = 4;
    vfd.patternPtr[0] = 0x81;
    vfd.patternPtr[1] = 0x00;
    vfd.patternPtr[2] = 0x00;
    vfd.patternPtr[3] = 0x05;

    prvTgfMirrorVlanTagRemovalTrafficGenerate(vfd, GT_FALSE);
}


/*******************************************************************************
* prvTgfEgressMirrorVlanTagRemovalConfigurationRestore
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
GT_VOID prvTgfEgressMirrorVlanTagRemovalConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS rc;

    /* Disable Tx mirroring */
    rc = prvTgfMirrorTxGlobalAnalyzerInterfaceIndexSet(GT_FALSE, 3);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxGlobalAnalyzerInterfaceIndexSet: %d", GT_FALSE);

    /* Disable Tx mirroring on port 23. */
    rc = prvTgfMirrorTxPortSet(prvTgfPortsArray[3], GT_FALSE, 0);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfMirrorTxPortSet: %d %d",
                                 prvTgfPortsArray[3], GT_FALSE);

    /* invalidate VLAN entry */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d",
                                 PRV_TGF_VLANID_CNS);

    /* clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

}
