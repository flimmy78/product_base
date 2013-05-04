/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfFdbAuNaMessageExtFormat.c
*
* DESCRIPTION:
*       Bridge FDB AU NA Message with new format UT.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>
#include <cpss/generic/trunk/cpssGenTrunkTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>
#include <utf/private/prvUtfExtras.h>

#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfBridgeGen.h>
#include <common/tgfTrunkGen.h>
#include <bridge/prvTgfBrgGen.h>
#include <bridge/prvTgfFdbAuNaMessageExtFormat.h>
#include <appDemo/userExit/userEventHandler.h>


/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/
/* default VLAN0 Id */
#define PRV_TGF_VLANID_2_CNS  2

/* VLAN 5 Id */
#define PRV_TGF_VLANID_5_CNS  5

/* VLAN 7 Id */
#define PRV_TGF_VLANID_7_CNS  7

/* User Priority value0 */
#define PRV_TGF_UP_4_CNS  4

/* User Priority value0 */
#define PRV_TGF_UP_7_CNS  7

/* port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_1_CNS         1

/* port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_2_CNS         2

/* trunk number to send traffic to */
#define PRV_TGF_SEND_TRUNK_ID_127_CNS       127

/* not existed device number */
#define PRV_TGF_DEV_NUM_8_CNS               8

/* number of iterations in the test */
#define PRV_AU_NA_ITERAT_NUM_CNS 4

extern GT_STATUS   appDemoAllowProcessingOfAuqMessages
(
    GT_BOOL     enable
);

/* default number of packets to send */
static GT_U32  prvTgfBurstCount   = 1;

/******************************* Test packet **********************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part = {
    {0x00, 0x00, 0x00, 0x00, 0x11, 0xAA},               /* dstMac */
    {0x00, 0x00, 0x00, 0x00, 0x11, 0xBB}                /* srcMac */
};

/* Unicast unknown da mac */
static TGF_MAC_ADDR     prvTgfUcastUnknownMac = 
    {0x00, 0x00, 0x00, 0x00, 0x11, 0xAA};

/* Unicast da mac */
static TGF_MAC_ADDR     prvTgfUcastKnownMac = 
    {0x00, 0x00, 0x00, 0x44, 0x55, 0x44};

/* First VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTag0Part = {
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,                    /* etherType */
    PRV_TGF_UP_4_CNS, 0, PRV_TGF_VLANID_2_CNS           /* pri, cfi, VlanId */
};


/* First VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTag1Part = {
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,                    /* etherType */
    0, 0, PRV_TGF_VLANID_5_CNS                          /* pri, cfi, VlanId */
};

/* DATA of packet */
static GT_U8 prvTgfPayloadDataArr[] = {
    0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};
/* PAYLOAD part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketPayloadPart = {
    sizeof(prvTgfPayloadDataArr),                       /* dataLength */
    prvTgfPayloadDataArr                                /* dataPtr */
};

/************************ Double tagged packet ********************************/
/* PARTS of double tagged packet */
static TGF_PACKET_PART_STC prvTgfPacketDoubleTagPartArray[] =
{
    {TGF_PACKET_PART_L2_E,       &prvTgfPacketL2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E, &prvTgfPacketVlanTag0Part},
    {TGF_PACKET_PART_VLAN_TAG_E, &prvTgfPacketVlanTag1Part},
    {TGF_PACKET_PART_PAYLOAD_E,  &prvTgfPacketPayloadPart}
};

/* LENGTH of double tagged packet */
#define PRV_TGF_PACKET_DOUBLE_TAG_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + 2 * TGF_VLAN_TAG_SIZE_CNS + sizeof(prvTgfPayloadDataArr)

/* LENGTH of double tagged packet with CRC */
#define PRV_TGF_PACKET_DOUBLE_TAG_CRC_LEN_CNS  PRV_TGF_PACKET_DOUBLE_TAG_LEN_CNS + TGF_CRC_LEN_CNS

/* Double tagged PACKET to send */
static TGF_PACKET_STC prvTgfPacketInfoDoubleTag =
{
    PRV_TGF_PACKET_DOUBLE_TAG_LEN_CNS,                                    /* totalLen */
    sizeof(prvTgfPacketDoubleTagPartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketDoubleTagPartArray                                        /* partsArray */
};

/************************ Single tagged packet ********************************/

/* PARTS of single tagged packet */
static TGF_PACKET_PART_STC prvTgfPacketSingleTagPartArray[] =
{
    {TGF_PACKET_PART_L2_E,       &prvTgfPacketL2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E, &prvTgfPacketVlanTag0Part},
    {TGF_PACKET_PART_PAYLOAD_E,  &prvTgfPacketPayloadPart}
};

/* LENGTH of single tagged packet */
#define PRV_TGF_PACKET_SINGLE_TAG_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + sizeof(prvTgfPayloadDataArr)

/* LENGTH of single tagged packet with CRC */
#define PRV_TGF_PACKET_SINGLE_TAG_CRC_LEN_CNS  PRV_TGF_PACKET_SINGLE_TAG_LEN_CNS + TGF_CRC_LEN_CNS

/* Single tagged PACKET to send */
static TGF_PACKET_STC prvTgfPacketInfoSingleTag =
{
    PRV_TGF_PACKET_SINGLE_TAG_LEN_CNS,                                    /* totalLen */
    sizeof(prvTgfPacketSingleTagPartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketSingleTagPartArray                                        /* partsArray */
};

/******************************************************************************\
 *                            Private test functions                          *
\******************************************************************************/
/*******************************************************************************
* prvTgfFdbAuNaMessageExtFormatCfg
*
* DESCRIPTION:
*       Set VLAN entry.
*
* INPUTS:
*       isPktDoubleTagged   - GT_TRUE: packet is double tagged
*                           - GT_FALSE: packet is single tagged
*       up0                 - User priority of the Tag0
*       vlan1               - vlan Id of the Tag1
*       isDaMacNew          - GT_TRUE: DA MAC is unknown unicast
*                             GT_FALSE: DA MAC is known unicast
*       srcId               - source Id of the address
*       dstInterfacePtr     - The associated information with the entry
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
static GT_VOID prvTgfFdbAuNaMessageExtFormatCfg
(
    IN GT_BOOL                  isPktDoubleTagged,
    IN TGF_PRI                  up0,
    IN GT_U16                   vid1,
    IN GT_BOOL                  isDaMacNew,
    IN GT_U32                   srcId,
    IN CPSS_INTERFACE_INFO_STC  *dstInterfacePtr
)
{
    GT_STATUS                   rc;
    PRV_TGF_BRG_MAC_ENTRY_STC   prvTgfMacEntry;

    if (isPktDoubleTagged == GT_TRUE) 
    {
        /* Create Vlan with vid1 */
        rc = prvTgfBrgDefVlanEntryWrite(vid1);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryWrite: %d, %d",
                                     prvTgfDevNum, vid1);

        /* Enable vid1 assignment */
        rc = prvTgfBrgFdbNaMsgVid1EnableSet(prvTgfDevNum, GT_TRUE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbNaMsgVid1EnableSet: %d",
                                     prvTgfDevNum);

        /* Set Tag1 VLAN ID of the sent packet */
        prvTgfPacketVlanTag1Part.vid = vid1;
    }

    /* Set User Priority of Tag0 to up0 */
    prvTgfPacketVlanTag0Part.pri = up0;;

    if (isDaMacNew) 
    {
        /* Set destination addres to unknown unicast */
        cpssOsMemCpy(prvTgfPacketL2Part.saMac, prvTgfUcastUnknownMac,
                         sizeof(prvTgfPacketL2Part.daMac));
    }
    else
    {
        /* Set destination addres to known unicast */
        cpssOsMemCpy(prvTgfPacketL2Part.saMac, prvTgfUcastKnownMac,
                         sizeof(prvTgfPacketL2Part.daMac));

        /* Add static MAC Entry */
    
        /* clear entry */
        cpssOsMemSet(&prvTgfMacEntry, 0, sizeof(prvTgfMacEntry));
    
        prvTgfMacEntry.key.entryType                = PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E;
        prvTgfMacEntry.key.key.macVlan.vlanId       = PRV_TGF_VLANID_2_CNS;
        prvTgfMacEntry.dstInterface.type            = dstInterfacePtr->type;
        if (prvTgfMacEntry.dstInterface.type == CPSS_INTERFACE_PORT_E) 
        {
            /* Dev/Port */
            prvTgfMacEntry.dstInterface.devPort.devNum  = dstInterfacePtr->devPort.devNum;
            prvTgfMacEntry.dstInterface.devPort.portNum = dstInterfacePtr->devPort.portNum;
        }
        else
        {
            /* Trunk */
            prvTgfMacEntry.dstInterface.trunkId = dstInterfacePtr->trunkId;
        }

        prvTgfMacEntry.sourceId                     = srcId;
        prvTgfMacEntry.isStatic                     = GT_FALSE;
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

        cpssOsMemCpy(prvTgfMacEntry.key.key.macVlan.macAddr.arEther, 
                     prvTgfUcastKnownMac, 
                     sizeof(TGF_MAC_ADDR));
    
        rc = prvTgfBrgFdbMacEntrySet(&prvTgfMacEntry);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbMacEntrySet");
    }

    /* Disable learning and allow accumulation of AU messages */
    rc = appDemoAllowProcessingOfAuqMessages(GT_FALSE);
    UTF_VERIFY_EQUAL0_STRING_MAC(
        GT_OK, rc, "appDemoAllowProcessingOfAuqMessages");
}

/*******************************************************************************
* prvTgfFdbAuNaMessageExtFormatPacketSend
*
* DESCRIPTION:
*       Function sends packet, performs trace and check expected results.
*
* INPUTS:
*       isPktDoubleTagged   - GT_TRUE: packet is double tagged
*                             GT_FALSE: packet is single tagged
*       portNum             - port number to send packet from
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
static GT_VOID prvTgfFdbAuNaMessageExtFormatPacketSend
(
    IN GT_BOOL  isPktDoubleTagged,
    IN GT_U8    portNum
)
{
    GT_STATUS       rc = GT_OK;
    GT_U8           portIter;                            

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    if (isPktDoubleTagged == GT_TRUE) 
    {
        /* setup Packet with double tag */
        rc = prvTgfSetTxSetupEth(prvTgfDevNum, &prvTgfPacketInfoDoubleTag, prvTgfBurstCount, 0, NULL);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "ERROR of SetTxSetup: %d, %d",
                                     prvTgfDevNum, prvTgfBurstCount, 0, NULL);
    }
    else
    {

        /* setup Packet with single tag */
        rc = prvTgfSetTxSetupEth(prvTgfDevNum, &prvTgfPacketInfoSingleTag, prvTgfBurstCount, 0, NULL);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "ERROR of SetTxSetup: %d, %d",
                                     prvTgfDevNum, prvTgfBurstCount, 0, NULL);
    }

    /* clear table */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");


    /* send Packet */
    rc = prvTgfStartTransmitingEth(prvTgfDevNum, portNum);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, %d",
                                 prvTgfDevNum, portNum);

}


/*******************************************************************************
* prvTgfFdbAuNaMessageExtFormatCheckAuMessage
*
* DESCRIPTION:
*       Get MAC entry.
*
* INPUTS:
*       isPktDoubleTagged   - GT_TRUE: packet is double tagged
*                           - GT_FALSE: packet is single tagged
*       up0                 - User priority of the Tag0
*       vlan1               - vlan Id of the Tag1
*       isDaMacNew          - GT_TRUE: DA MAC is unknown unicast
*                             GT_FALSE: DA MAC is known unicast
*       oldSrcId            - old source Id of the address
*       oldDstInterfacePtr  - The aold ssociated information with the entry
*       oldAssociatedDevNum - The old device number the entry is associated with
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
static GT_VOID prvTgfFdbAuNaMessageExtFormatCheckAuMessage
(
    IN GT_BOOL                  isPktDoubleTagged,
    IN GT_U32                   up0,
    IN GT_U16                   vid1,
    IN GT_BOOL                  isDaMacNew,
    IN GT_U32                   oldSrcId,
    IN CPSS_INTERFACE_INFO_STC  *oldDstInterfacePtr,
    IN GT_U8                    oldAssociatedDevNum
)
{
    GT_STATUS                       rc = GT_OK;
    GT_U32                          actNumOfAu;
    CPSS_MAC_UPDATE_MSG_EXT_STC     *auMessagesPtr;
    GT_U32                          numOfAu = 1; /* each time only one message 
                                                    is expected */
    auMessagesPtr  =
        (CPSS_MAC_UPDATE_MSG_EXT_STC*)
         cpssOsMalloc(sizeof(CPSS_MAC_UPDATE_MSG_EXT_STC) * numOfAu);

    cpssOsMemSet(auMessagesPtr, 0, (sizeof(CPSS_MAC_UPDATE_MSG_EXT_STC) * numOfAu));

    actNumOfAu = numOfAu;

    /* get AU messages */
    rc = prvTgfBrgFdbAuMsgBlockGet(prvTgfDevNum, &actNumOfAu, auMessagesPtr);
    if (rc != GT_OK) 
    {
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_NO_MORE, rc, "prvTgfBrgFdbAuMsgBlockGet: %d", prvTgfDevNum);
    }
    if (rc != GT_NO_MORE) 
    {
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbAuMsgBlockGet: %d", prvTgfDevNum);
    }

    UTF_VERIFY_EQUAL2_STRING_MAC(numOfAu, actNumOfAu, "The number of received messages is wrong: %d, %d",
                                 actNumOfAu, numOfAu);

    if (actNumOfAu == numOfAu) 
    {
        /* Check the received message */

        /* check up0 */
        UTF_VERIFY_EQUAL2_STRING_MAC(auMessagesPtr->up0, up0, "The up0 of the AU message is not as expected: %d, %d",
                                     up0, oldSrcId);

        if (isPktDoubleTagged == GT_TRUE) 
        {
            /* check vid1 */
            UTF_VERIFY_EQUAL2_STRING_MAC(auMessagesPtr->vid1, vid1, "The vid1 of the AU message is not as expected: %d, %d",
                                         up0, oldSrcId);
        }

        if (isDaMacNew == GT_TRUE) 
        {
            /* check that isMoved == GT_FALSE */
            UTF_VERIFY_EQUAL2_STRING_MAC(auMessagesPtr->isMoved, GT_FALSE, "The 'isMoved' of the AU message is not as expected: %d, %d",
                                         up0, (!isDaMacNew));

        }
        else
        {
            /* check that isMoved == GT_TRUE */
            UTF_VERIFY_EQUAL2_STRING_MAC(auMessagesPtr->isMoved, GT_TRUE, "The 'isMoved' of the AU message is not as expected: %d, %d",
                                         up0, (!isDaMacNew));
            /* check oldSrcId */
            UTF_VERIFY_EQUAL2_STRING_MAC(auMessagesPtr->oldSrcId, oldSrcId, "The 'oldSrcId' of the AU message is not as expected: %d, %d",
                                         up0, oldSrcId);
            /* check old associated devNum */
            UTF_VERIFY_EQUAL2_STRING_MAC(auMessagesPtr->oldAssociatedDevNum, 
                                         oldAssociatedDevNum, "The 'oldAssociatedDevNum' of the AU message is not as expected: %d, %d",
                                         up0, oldSrcId);

            if (oldDstInterfacePtr->type == CPSS_INTERFACE_PORT_E) 
            {
                /* check old devNum and portNum */
                UTF_VERIFY_EQUAL2_STRING_MAC(auMessagesPtr->oldDstInterface.devPort.devNum, 
                                             oldDstInterfacePtr->devPort.devNum, "The 'devNum' of the AU message is not as expected: %d, %d",
                                             up0, oldSrcId);

                UTF_VERIFY_EQUAL2_STRING_MAC(auMessagesPtr->oldDstInterface.devPort.portNum, 
                                             oldDstInterfacePtr->devPort.portNum, "The 'portNum' of the AU message is not as expected: %d, %d",
                                             up0, oldSrcId);
            }
            else
            {
                /* check old trunk */
                UTF_VERIFY_EQUAL2_STRING_MAC(auMessagesPtr->oldDstInterface.trunkId, 
                                             oldDstInterfacePtr->trunkId, "The 'trunkId' of the AU message is not as expected: %d, %d",
                                             up0, oldSrcId);
            }
        }
    }

    cpssOsFree(auMessagesPtr);
}


/*******************************************************************************
* prvTgfFdbAuNaMessageExtFormatTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic and check results
*
* INPUTS:
*       isPktDoubleTagged   - GT_TRUE: packet is double tagged
*                           - GT_FALSE: packet is single tagged
*       up0                 - User priority of the Tag0
*       vlan1               - vlan Id of the Tag1
*       isDaMacNew          - GT_TRUE: DA MAC is unknown unicast
*                             GT_FALSE: DA MAC is known unicast
*       oldSrcId            - old source Id of the address
*       oldDstInterfacePtr  - The aold ssociated information with the entry
*       oldAssociatedDevNum - The old device number the entry is associated with
*       ingressPort         - The port number to send packet from
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
static GT_VOID prvTgfFdbAuNaMessageExtFormatTrafficGenerate
(
    IN GT_BOOL                  isPktDoubleTagged,
    IN TGF_PRI                  up0,
    IN GT_U16                   vid1,
    IN GT_BOOL                  isDaMacNew,
    IN GT_U32                   oldSrcId,
    IN CPSS_INTERFACE_INFO_STC  *oldDstInterfacePtr,
    IN GT_U8                    oldAssociatedDevNum,
    IN GT_U8                    ingressPort
)
{
    GT_STATUS                       rc;             /* returned status */

    /* Configure AU NA Message */
    prvTgfFdbAuNaMessageExtFormatCfg(isPktDoubleTagged,
                                     up0,
                                     vid1,
                                     isDaMacNew,
                                     oldSrcId,
                                     oldDstInterfacePtr);

    /* Send packet */
    prvTgfFdbAuNaMessageExtFormatPacketSend(isPktDoubleTagged, ingressPort);

    /* give time for device/simulation for packet processing */
    cpssOsTimerWkAfter(10);

    /* check AU message */
    prvTgfFdbAuNaMessageExtFormatCheckAuMessage(isPktDoubleTagged,
                                                up0,
                                                vid1,
                                                isDaMacNew,
                                                oldSrcId,
                                                oldDstInterfacePtr,
                                                oldAssociatedDevNum);


    if (isPktDoubleTagged == GT_TRUE) 
    {
        /* invalidate vlan entry (and reset vlan entry) */
        rc = prvTgfBrgDefVlanEntryInvalidate(vid1);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d, %d",
                                     prvTgfDevNum, vid1);

        /* Restore vid assignment to be from Tag0*/
        rc = prvTgfBrgFdbNaMsgVid1EnableSet(prvTgfDevNum, GT_FALSE);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbNaMsgVid1EnableSet: %d",
                                     prvTgfDevNum);
    }

    /* Enable learning */
    rc = appDemoAllowProcessingOfAuqMessages(GT_TRUE);
    UTF_VERIFY_EQUAL0_STRING_MAC(
        GT_OK, rc, "appDemoAllowProcessingOfAuqMessages");
}

/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/
/*******************************************************************************
* prvTgfFdbAuNaMessageExtFormatConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Create VLAN 2 on all ports (0,8,18,23)
*
* INPUTS:
*       vlanId  - VLAN ID
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
GT_VOID prvTgfFdbAuNaMessageExtFormatConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS   rc;

    /* 1. Create new vlan on ports (0,0), (0,8), (0,18), (0,23) */
    rc = prvTgfBrgDefVlanEntryWrite(PRV_TGF_VLANID_2_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryWrite: %d, %d",
                                 prvTgfDevNum, PRV_TGF_VLANID_2_CNS);
}

/*******************************************************************************
* prvTgfFdbAuNaMessageExtFormatWithTag1DueToNewAddrTrafficGenerate
*
* DESCRIPTION:
*           Disable learning.
*           Send double tagged packet with unknown unicast.
*       Generate traffic:
*           Send from port1 double tagged packet:
*           Success Criteria:
*               The AU NA message is updated correctly including the extended 
*               parameters: vid1, up0 and isMoved.
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
GT_VOID prvTgfFdbAuNaMessageExtFormatWithTag1DueToNewAddrTrafficGenerate
(
    void
)
{
    GT_BOOL     isPktDoubleTagged = GT_TRUE;
    TGF_PRI     up0;
    GT_U16      vid1;
    GT_BOOL     isDaMacNew = GT_TRUE;
    GT_U32      srcId;
    CPSS_INTERFACE_INFO_STC  dstInterface;
    GT_U8       associatedDevNum = 0;
    GT_U8       ingressPort;
    TGF_PRI     up0Arr[PRV_AU_NA_ITERAT_NUM_CNS]   = { 4,    7,  5,    0};
    GT_U16      vid1Arr[PRV_AU_NA_ITERAT_NUM_CNS]  = { 5, 4094,  3, 2789};
    GT_U32      srcIdArr[PRV_AU_NA_ITERAT_NUM_CNS] = { 9,   19, 31,    0};
    GT_U32      ii;

    /* clear entry */
    cpssOsMemSet(&dstInterface, 0, sizeof(dstInterface));

    for (ii = 0; ii < PRV_AU_NA_ITERAT_NUM_CNS; ii++)
    {
        up0 = up0Arr[ii];
        vid1 = vid1Arr[ii];
        srcId = srcIdArr[ii];
        ingressPort = prvTgfPortsArray[ii];
        prvTgfFdbAuNaMessageExtFormatTrafficGenerate(isPktDoubleTagged,
                                                     up0,
                                                     vid1,
                                                     isDaMacNew,
                                                     srcId,
                                                     &dstInterface,
                                                     associatedDevNum,
                                                     ingressPort);
    }
}


/*******************************************************************************
* prvTgfFdbAuNaMessageExtFormatDueToMovedPortTrafficGenerate
*
* DESCRIPTION:
*           Learn MAC on FDB on port2.
*           Disable learning.
*           Send single tagged packet with known unicast, but from different 
*           source (port).
*       Generate traffic:
*           Send from port1 single tagged packet:
*           Success Criteria:
*               The AU NA message is updated correctly including the extended 
*               parameters: up0, isMoved oldSrcId, oldDstInterface and
*               oldAssociatedDevNum.
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
GT_VOID prvTgfFdbAuNaMessageExtFormatDueToMovedPortTrafficGenerate
(
    void
)
{
    GT_BOOL     isPktDoubleTagged = GT_FALSE;
    TGF_PRI     up0;
    GT_BOOL     isDaMacNew = GT_FALSE;
    GT_U32      srcId;
    CPSS_INTERFACE_INFO_STC  dstInterface;
    GT_U8       associatedDevNum;
    GT_U8       ingressPort; 
    TGF_PRI     up0Arr[PRV_AU_NA_ITERAT_NUM_CNS]   = {    3,    6,    4,    1};
    GT_U32      srcIdArr[PRV_AU_NA_ITERAT_NUM_CNS] = {   11,   23,   30,    1};
    GT_U32      ii;

    /* clear entry */
    cpssOsMemSet(&dstInterface, 0, sizeof(dstInterface));

    dstInterface.type = CPSS_INTERFACE_PORT_E;
    dstInterface.devPort.devNum = prvTgfDevNum;
    associatedDevNum = dstInterface.devPort.devNum;

    for (ii = 0; ii < PRV_AU_NA_ITERAT_NUM_CNS; ii++)
    {
        up0 = up0Arr[ii];
        srcId = srcIdArr[ii];
        dstInterface.devPort.portNum = prvTgfPortsArray[ii];
        ingressPort = prvTgfPortsArray[(ii + 1) % 4];
        prvTgfFdbAuNaMessageExtFormatTrafficGenerate(isPktDoubleTagged,
                                                 up0,
                                                 0,
                                                 isDaMacNew,
                                                 srcId,
                                                 &dstInterface,
                                                 associatedDevNum,
                                                 ingressPort);
    }
}



/*******************************************************************************
* prvTgfFdbAuNaMessageExtFormatDueToMovedTrunkTrafficGenerate
*
* DESCRIPTION:
*           Learn MAC on FDB on trunk.
*           Disable learning.
*           Send single tagged packet with known unicast, but from different
*           source (port).
*       Generate traffic:
*           Send from port1 single tagged packet:
*           Success Criteria:
*               The AU NA message is updated correctly including the extended 
*               parameters: up0, isMoved oldSrcId, oldDstInterface and
*               oldAssociatedDevNum.
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
GT_VOID prvTgfFdbAuNaMessageExtFormatDueToMovedTrunkTrafficGenerate
(
    void
)
{
    GT_BOOL     isPktDoubleTagged = GT_FALSE;
    TGF_PRI     up0;
    GT_BOOL     isDaMacNew = GT_FALSE;
    GT_U32      srcId;
    CPSS_INTERFACE_INFO_STC  dstInterface;
    GT_U8       associatedDevNum;
    GT_U8       ingressPort;
    TGF_PRI     up0Arr[PRV_AU_NA_ITERAT_NUM_CNS]      = {  0,   2,  7,  3};
    GT_U32      srcIdArr[PRV_AU_NA_ITERAT_NUM_CNS]    = {  3,  28, 11,  7};
    GT_TRUNK_ID trunkIdArr[PRV_AU_NA_ITERAT_NUM_CNS]  = { 23, 127,  1, 58};
    GT_U32      ii;

    /* clear entry */
    cpssOsMemSet(&dstInterface, 0, sizeof(dstInterface));
    dstInterface.type = CPSS_INTERFACE_TRUNK_E;

    /* associated devNum is set by CPSS to be HW devNum */
    associatedDevNum = 0;


    for (ii = 0; ii < PRV_AU_NA_ITERAT_NUM_CNS; ii++)
    {
        up0 = up0Arr[ii];
        srcId = srcIdArr[ii];
        dstInterface.trunkId = trunkIdArr[ii];
        ingressPort = prvTgfPortsArray[ii];

        prvTgfFdbAuNaMessageExtFormatTrafficGenerate(isPktDoubleTagged,
                                                 up0,
                                                 0,
                                                 isDaMacNew,
                                                 srcId,
                                                 &dstInterface,
                                                 associatedDevNum,
                                                 ingressPort);
    }
}


/*******************************************************************************
* prvTgfFdbAuNaMessageExtFormatConfigurationRestore
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
GT_VOID prvTgfFdbAuNaMessageExtFormatConfigurationRestore
(
    void
)
{
    GT_STATUS   rc = GT_OK;

    /****************/
    /* Tables Reset */
    /****************/

    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d, %d", prvTgfDevNum, GT_TRUE);

    /* invalidate vlan entry (and reset vlan entry) */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_2_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d, %d",
                                 prvTgfDevNum, PRV_TGF_VLANID_2_CNS);

}

