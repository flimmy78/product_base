/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfCscd.c
*
* DESCRIPTION:
*       CPSS Cascading
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>

#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfBridgeGen.h>
#include <common/tgfCscdGen.h>
#include <cscd/prvTgfCscd.h>


/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/
/* default VLAN0 Id */
#define PRV_TGF_VLANID_CNS  5

/* port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_CNS   1

/* index of local egress port */
#define LOCAL_EGRESS_PORT_INDEX_CNS 2

/* default number of packets to send */
static GT_U32  prvTgfBurstCount   = 1;

/******************************* Test packet **********************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part =
{
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x56},               /* dstMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x02}                /* srcMac */
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

/* cascaded link number */
#define CSCD_LINK_IDX         3

/* cascaded link number */
#define CSCD_LINK_IDX_ANOTHER 0

/* target dev/port array size */
#define PRV_TGF_CSCD_TRG_ARRAY_SIZE_CNS 3

/* target device number array*/
static GT_U8 targetDevNumArr[PRV_TGF_CSCD_TRG_ARRAY_SIZE_CNS] = {0, 13, 31};

/* target device number array*/
static GT_U8 targetPortNumArr[PRV_TGF_CSCD_TRG_ARRAY_SIZE_CNS] = {2, 26, 56};

/* FDB MAC entry */
static PRV_TGF_BRG_MAC_ENTRY_STC prvTgfMacEntry;      


/*************************** Restore config ***********************************/

/* parameters that is needed to be restored */
static struct
{
    PRV_TGF_DEV_MAP_LOOKUP_MODE_ENT     lookupMode;
    CPSS_CSCD_LINK_TYPE_STC             cascadeLinkArr[PRV_TGF_CSCD_TRG_ARRAY_SIZE_CNS];
    PRV_TGF_CSCD_TRUNK_LINK_HASH_ENT    srcPortTrunkHashEnArr[PRV_TGF_CSCD_TRG_ARRAY_SIZE_CNS];
} prvTgfRestoreCfg;

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
static GT_VOID prvTgfCscdTestPacketSend
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
* prvTgfCscdSingleTargetDestinationConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set FDB entry with device number 0, port 1,  MAC 00:00:00:00: 00:56. 
*         -	Set lookup mode to Port for accessing the Device Map table. 
*         -	Set the cascade map table 
*            - Target device 0, target port 1, link type port, link number 23. 
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
GT_VOID prvTgfCscdSingleTargetDestinationConfigurationSet
(
    GT_VOID
)
{
    CPSS_CSCD_LINK_TYPE_STC   cascadeLink;          /* cascade link */
    GT_STATUS                 rc = GT_OK;

    
    /* set VLAN entry */
    rc = prvTgfBrgDefVlanEntryWrite(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d",
                                 PRV_TGF_VLANID_CNS);

    /* clear entry */
    cpssOsMemSet(&prvTgfMacEntry, 0, sizeof(prvTgfMacEntry)); 

    /* Set FDB entry with device number 31, port 27,  MAC 00:00:00:00: 00:56 */
    prvTgfMacEntry.key.key.macVlan.vlanId = PRV_TGF_VLANID_CNS;
    prvTgfMacEntry.isStatic = GT_TRUE;
    prvTgfMacEntry.dstInterface.type = CPSS_INTERFACE_PORT_E;
    prvTgfMacEntry.dstInterface.devPort.devNum = targetDevNumArr[0];
    prvTgfMacEntry.dstInterface.devPort.portNum = targetPortNumArr[0];
    cpssOsMemCpy(prvTgfMacEntry.key.key.macVlan.macAddr.arEther, prvTgfPacketL2Part.daMac, sizeof(TGF_MAC_ADDR));
    rc = prvTgfBrgFdbMacEntrySet(&prvTgfMacEntry); 
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbMacEntrySet");

    /* save the current lookup mode */
    rc = prvTgfCscdDevMapLookupModeGet(prvTgfDevNum, &(prvTgfRestoreCfg.lookupMode));
    if (rc == GT_NOT_APPLICABLE_DEVICE)
    {
        /* such device support only TRG_DEV mode */
        prvTgfRestoreCfg.lookupMode = PRV_TGF_DEV_MAP_LOOKUP_MODE_TRG_DEV_E;
    }
    else
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCscdDevMapLookupModeGet: %d", prvTgfDevNum);

    /* Set lookup mode for accessing the Device Map table. */
    rc = prvTgfCscdDevMapLookupModeSet(PRV_TGF_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCscdDevMapLookupModeSet: %d",
                                 PRV_TGF_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E);

    /* save the current cascade map table */
    rc = prvTgfCscdDevMapTableGet(prvTgfDevNum, targetDevNumArr[0], targetPortNumArr[0],
                                  &(prvTgfRestoreCfg.cascadeLinkArr[0]), &(prvTgfRestoreCfg.srcPortTrunkHashEnArr[0]));
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfCscdDevMapTableGet: %d,%d,%d",
                                 prvTgfDevNum, targetDevNumArr[0], targetPortNumArr[0]);

    /* Set the cascade map table */
    cascadeLink.linkType = CPSS_CSCD_LINK_TYPE_PORT_E;
    cascadeLink.linkNum = prvTgfPortsArray[CSCD_LINK_IDX];

    rc = prvTgfCscdDevMapTableSet(prvTgfDevNum, targetDevNumArr[0], targetPortNumArr[0], &cascadeLink, 0);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfCscdDevMapLookupModeSet: %d%d%d%d",
                                 targetDevNumArr[0], targetPortNumArr[0], CPSS_CSCD_LINK_TYPE_PORT_E, prvTgfPortsArray[CSCD_LINK_IDX]);
}

/*******************************************************************************
* prvTgfCscdSingleTargetDestinationTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = 00:00:00:00:00:56,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                No packets are captured.
*           Set FDB entry with device number 13, port 27,  MAC 00:00:00:00: 00:56. 
*          	Set the cascade map table 
*            - Target device 13, target port 27, link type port, link number 23. 
*           Send to device's port 8 packet:
*               macDa = 00:00:00:00:00:56,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 23.
*           Set FDB entry with device number 31, port 58,  MAC 00:00:00:00: 00:56. 
*          	Set the cascade map table 
*            - Target device 31, target port 58, link type port, link number 0. 
*           Send to device's port 8 packet:
*               macDa = 00:00:00:00:00:56,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0.
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
GT_VOID prvTgfCscdSingleTargetDestinationTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS    rc = GT_OK;
    GT_U32       portIter = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC portCntrs;
    CPSS_CSCD_LINK_TYPE_STC   cascadeLink;          /* cascade link */

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* send packet */
    prvTgfCscdTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

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
        /* No port  received packet */
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
    }

    prvTgfMacEntry.dstInterface.devPort.devNum = targetDevNumArr[1];
    prvTgfMacEntry.dstInterface.devPort.portNum = targetPortNumArr[1];
    rc = prvTgfBrgFdbMacEntrySet(&prvTgfMacEntry); 
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbMacEntrySet");

    /* save the current cascade map table */
    rc = prvTgfCscdDevMapTableGet(prvTgfDevNum, targetDevNumArr[1], targetPortNumArr[1],
                                  &(prvTgfRestoreCfg.cascadeLinkArr[1]), &(prvTgfRestoreCfg.srcPortTrunkHashEnArr[1]));
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfCscdDevMapTableGet: %d,%d,%d",
                                 prvTgfDevNum, targetDevNumArr[1], targetPortNumArr[1]);

    /* Set the cascade map table */
    cascadeLink.linkType = CPSS_CSCD_LINK_TYPE_PORT_E;
    cascadeLink.linkNum = prvTgfPortsArray[CSCD_LINK_IDX];

    rc = prvTgfCscdDevMapTableSet(prvTgfDevNum, targetDevNumArr[1], targetPortNumArr[1], &cascadeLink, 0);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfCscdDevMapLookupModeSet: %d%d%d%d",
                                 targetDevNumArr[1], targetPortNumArr[1], CPSS_CSCD_LINK_TYPE_PORT_E, prvTgfPortsArray[CSCD_LINK_IDX]);


    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /* send packet */
    prvTgfCscdTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

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
        /* Verify that port 23 received 1 packet */
        if (portIter == CSCD_LINK_IDX)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.goodPktsSent.l[0],
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


    /* Set lookup mode for accessing the Device Map table. */
    rc = prvTgfCscdDevMapLookupModeSet(PRV_TGF_DEV_MAP_LOOKUP_MODE_TRG_DEV_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCscdDevMapLookupModeSet: %d",
                                 PRV_TGF_DEV_MAP_LOOKUP_MODE_TRG_DEV_E);


    prvTgfMacEntry.dstInterface.devPort.devNum = targetDevNumArr[2];
    prvTgfMacEntry.dstInterface.devPort.portNum = targetPortNumArr[2];
    rc = prvTgfBrgFdbMacEntrySet(&prvTgfMacEntry); 
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbMacEntrySet");

    /* save the current cascade map table */
    rc = prvTgfCscdDevMapTableGet(prvTgfDevNum, targetDevNumArr[2], targetPortNumArr[2],
                                  &(prvTgfRestoreCfg.cascadeLinkArr[2]), &(prvTgfRestoreCfg.srcPortTrunkHashEnArr[2]));
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfCscdDevMapTableGet: %d,%d,%d",
                                 prvTgfDevNum, targetDevNumArr[2], targetPortNumArr[2]);

    /* Set the cascade map table */
    cascadeLink.linkType = CPSS_CSCD_LINK_TYPE_PORT_E;
    cascadeLink.linkNum = prvTgfPortsArray[CSCD_LINK_IDX_ANOTHER];

    rc = prvTgfCscdDevMapTableSet(prvTgfDevNum, targetDevNumArr[2], targetPortNumArr[2], &cascadeLink, 0);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfCscdDevMapLookupModeSet: %d%d%d%d",
                                 targetDevNumArr[2], targetPortNumArr[2], 
                                 CPSS_CSCD_LINK_TYPE_PORT_E, prvTgfPortsArray[CSCD_LINK_IDX_ANOTHER]);

    /* send packet */
    prvTgfCscdTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

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
        /* Verify that port 0 received 1 packet */
        if (portIter == CSCD_LINK_IDX_ANOTHER)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.goodPktsSent.l[0],
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
* prvTgfCscdSingleTargetDestinationConfigurationRestore
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
GT_VOID prvTgfCscdSingleTargetDestinationConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;


    /* restore cascade map table */
    rc = prvTgfCscdDevMapTableSet(prvTgfDevNum, targetDevNumArr[2], targetPortNumArr[2],
                                  &(prvTgfRestoreCfg.cascadeLinkArr[2]), prvTgfRestoreCfg.srcPortTrunkHashEnArr[2]);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfCscdDevMapTableSet: %d,%d,%d",
                                 prvTgfDevNum, targetDevNumArr[2], targetPortNumArr[2]);

    /* restore cascade map table */
    rc = prvTgfCscdDevMapTableSet(prvTgfDevNum, targetDevNumArr[1], targetPortNumArr[1],
                                  &(prvTgfRestoreCfg.cascadeLinkArr[1]), prvTgfRestoreCfg.srcPortTrunkHashEnArr[1]);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfCscdDevMapTableSet: %d,%d,%d",
                                 prvTgfDevNum, targetDevNumArr[1], targetPortNumArr[1]);

    /* restore cascade map table */
    rc = prvTgfCscdDevMapTableSet(prvTgfDevNum, targetDevNumArr[0], targetPortNumArr[0],
                                  &(prvTgfRestoreCfg.cascadeLinkArr[0]), prvTgfRestoreCfg.srcPortTrunkHashEnArr[0]);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfCscdDevMapTableSet: %d,%d,%d",
                                 prvTgfDevNum, targetDevNumArr[0], targetPortNumArr[0]);

    /* Restore lookup mode for accessing the Device Map table. */
    rc = prvTgfCscdDevMapLookupModeSet(prvTgfRestoreCfg.lookupMode);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCscdDevMapLookupModeSet: %d",
                                 prvTgfRestoreCfg.lookupMode);

    /* invalidate VLAN entry */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d",
                                 PRV_TGF_VLANID_CNS);

    /* flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d", GT_TRUE);

}

/*******************************************************************************
* prvTgfCscdSingleTargetDestinationLocalDeviceConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set FDB entry with device number 0, port 18,  MAC 00:00:00:00: 00:56. 
*         -	Set lookup mode to Port for accessing the Device Map table. 
*         -	Set the cascade map table 
*            - Target device 0, target port 0, link type port, link number 23. 
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
GT_VOID prvTgfCscdSingleTargetDestinationLocalDeviceConfigurationSet
(
    GT_VOID
)
{
    PRV_TGF_BRG_MAC_ENTRY_STC prvTgfMacEntry;       /* FDB MAC entry */
    CPSS_CSCD_LINK_TYPE_STC   cascadeLink;          /* cascade link */
    GT_STATUS                 rc = GT_OK;

    
    /* set VLAN entry */
    rc = prvTgfBrgDefVlanEntryWrite(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d",
                                 PRV_TGF_VLANID_CNS);

    /* clear entry */
    cpssOsMemSet(&prvTgfMacEntry, 0, sizeof(prvTgfMacEntry)); 

    /* Set FDB entry with device number 0, port 18,  MAC 00:00:00:00: 00:56 */
    prvTgfMacEntry.key.key.macVlan.vlanId = PRV_TGF_VLANID_CNS;
    prvTgfMacEntry.isStatic = GT_TRUE;
    prvTgfMacEntry.dstInterface.type = CPSS_INTERFACE_PORT_E;
    prvTgfMacEntry.dstInterface.devNum = 0;
    prvTgfMacEntry.dstInterface.devPort.devNum = 0;
    prvTgfMacEntry.dstInterface.devPort.portNum = prvTgfPortsArray[LOCAL_EGRESS_PORT_INDEX_CNS];
    cpssOsMemCpy(prvTgfMacEntry.key.key.macVlan.macAddr.arEther, prvTgfPacketL2Part.daMac, sizeof(TGF_MAC_ADDR));
    rc = prvTgfBrgFdbMacEntrySet(&prvTgfMacEntry); 
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbMacEntrySet");

    /* save the current lookup mode */
    rc = prvTgfCscdDevMapLookupModeGet(prvTgfDevNum, &(prvTgfRestoreCfg.lookupMode));
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCscdDevMapLookupModeGet: %d",
                                 prvTgfDevNum);

    /* Set lookup mode for accessing the Device Map table. */
    rc = prvTgfCscdDevMapLookupModeSet(PRV_TGF_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCscdDevMapLookupModeSet: %d", PRV_TGF_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E);

    /* save the current cascade map table */
    rc = prvTgfCscdDevMapTableGet(prvTgfDevNum, targetDevNumArr[0], prvTgfPortsArray[LOCAL_EGRESS_PORT_INDEX_CNS],
                                  &(prvTgfRestoreCfg.cascadeLinkArr[0]), &(prvTgfRestoreCfg.srcPortTrunkHashEnArr[0]));
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfCscdDevMapTableGet: %d,%d,%d",
                                 prvTgfDevNum, targetDevNumArr[0], prvTgfPortsArray[LOCAL_EGRESS_PORT_INDEX_CNS]);

    /* Set the cascade map table */
    cascadeLink.linkType = CPSS_CSCD_LINK_TYPE_PORT_E;
    cascadeLink.linkNum = prvTgfPortsArray[CSCD_LINK_IDX];

    rc = prvTgfCscdDevMapTableSet(prvTgfDevNum, targetDevNumArr[0], prvTgfPortsArray[LOCAL_EGRESS_PORT_INDEX_CNS],
                                  &cascadeLink, PRV_TGF_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfCscdDevMapLookupModeSet: %d%d%d%d",
                                 0, prvTgfPortsArray[LOCAL_EGRESS_PORT_INDEX_CNS], CPSS_CSCD_LINK_TYPE_PORT_E, prvTgfPortsArray[CSCD_LINK_IDX]);
}

/*******************************************************************************
* prvTgfCscdSingleTargetDestinationLocalDeviceTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = 00:00:00:00:00:56,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 18.
*           Enable the local target port for device map lookup for local device.
*           Send to device's port 8 packet:
*               macDa = 00:00:00:00:00:56,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 23.
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
GT_VOID prvTgfCscdSingleTargetDestinationLocalDeviceTrafficGenerate
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
    prvTgfCscdTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

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
        /* Verify that port 18 received 1 packet */
        if (portIter == LOCAL_EGRESS_PORT_INDEX_CNS)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.goodPktsSent.l[0],
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

    /* Enable the local target port for device map lookup for local device. */                                          
    rc = prvTgfCscdPortLocalDevMapLookupEnableSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], 
                                                  CPSS_DIRECTION_INGRESS_E,
                                                  GT_TRUE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfCscdPortLocalDevMapLookupEnableSet: %d%d%d",
                                                  prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                                  CPSS_DIRECTION_INGRESS_E,
                                                  GT_TRUE);

    rc = prvTgfCscdPortLocalDevMapLookupEnableSet(prvTgfPortsArray[LOCAL_EGRESS_PORT_INDEX_CNS], 
                                                  CPSS_DIRECTION_EGRESS_E,
                                                  GT_TRUE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfCscdPortLocalDevMapLookupEnableSet: %d%d%d",
                                                  prvTgfPortsArray[LOCAL_EGRESS_PORT_INDEX_CNS],
                                                  CPSS_DIRECTION_EGRESS_E,
                                                  GT_TRUE);

    /* send packet */
    prvTgfCscdTestPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], &prvTgfPacketInfo);

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
        /* Verify that port 23 received 1 packet */
        if (portIter == CSCD_LINK_IDX)
        {
            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
        else
        {
            UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.goodPktsSent.l[0],
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
* prvTgfCscdSingleTargetDestinationLocalDeviceConfigurationRestore
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
GT_VOID prvTgfCscdSingleTargetDestinationLocalDeviceConfigurationRestore
(
    GT_VOID
)
{
    GT_STATUS rc = GT_OK;

    
    /* Disable the local target port for device map lookup for local device. */                                          
    rc = prvTgfCscdPortLocalDevMapLookupEnableSet(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], 
                                                  CPSS_DIRECTION_INGRESS_E,
                                                  GT_FALSE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfCscdPortLocalDevMapLookupEnableSet: %d%d%d",
                                                  prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS],
                                                  CPSS_DIRECTION_INGRESS_E,
                                                  GT_FALSE);

    rc = prvTgfCscdPortLocalDevMapLookupEnableSet(prvTgfPortsArray[LOCAL_EGRESS_PORT_INDEX_CNS], 
                                                  CPSS_DIRECTION_EGRESS_E,
                                                  GT_FALSE);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfCscdPortLocalDevMapLookupEnableSet: %d%d%d",
                                                  prvTgfPortsArray[LOCAL_EGRESS_PORT_INDEX_CNS],
                                                  CPSS_DIRECTION_EGRESS_E,
                                                  GT_FALSE);

    /* Restore the cascade map table */
    rc = prvTgfCscdDevMapTableSet(prvTgfDevNum, targetDevNumArr[0], prvTgfPortsArray[LOCAL_EGRESS_PORT_INDEX_CNS],
                                  &(prvTgfRestoreCfg.cascadeLinkArr[0]), prvTgfRestoreCfg.srcPortTrunkHashEnArr[0]);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfCscdDevMapTableSet: %d,%d,%d",
                                 prvTgfDevNum, targetDevNumArr[0], prvTgfPortsArray[LOCAL_EGRESS_PORT_INDEX_CNS]);

    /* Restore lookup mode for accessing the Device Map table. */
    rc = prvTgfCscdDevMapLookupModeSet(prvTgfRestoreCfg.lookupMode);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCscdDevMapLookupModeSet: %d",
                                 prvTgfRestoreCfg.lookupMode);

    /* invalidate VLAN entry */
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d",
                                 PRV_TGF_VLANID_CNS);

    /* flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d", GT_TRUE);

}

