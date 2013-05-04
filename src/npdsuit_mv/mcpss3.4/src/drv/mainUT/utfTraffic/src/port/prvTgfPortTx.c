/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfPortTx.c
*
* DESCRIPTION:
*       Port Tx Queue features testing
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/cpssCommonDefs.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>
#include <utf/private/prvUtfExtras.h>

#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfPortGen.h>
#include <common/tgfBridgeGen.h>
#include <common/tgfCosGen.h>

/* default VLAN Id */
#define PRV_TGF_VLANID_CNS            223

/* ether type used for "vlan tag" */
#define TGF_ETHERTYPE_8100_VLAN_TAG_CNS 0x8100

/* egress port index */
#define PRV_TGF_EGR_PORT_IDX_0_CNS  0

/* number of ingress ports */
#define PRV_TGF_INGR_PORTS_NUM_CNS  3

/* acceptable difference between calculated rate and counted one */
GT_U32 prvTgfWsDiffInPercent = 2;

/* debug mode */
GT_U32 prvTgfPortTxSpDebug = 0;

/* open log */
GT_U32 prvTgfPortTxOpenLog = 0;
#define PORT_TX_PRV_UTF_LOG0_MAC \
    if(prvTgfPortTxOpenLog) PRV_UTF_LOG0_MAC

#define PORT_TX_PRV_UTF_LOG1_MAC \
    if(prvTgfPortTxOpenLog) PRV_UTF_LOG1_MAC

#define PORT_TX_PRV_UTF_LOG2_MAC \
    if(prvTgfPortTxOpenLog) PRV_UTF_LOG2_MAC

/* array of TC for test.
   values must be in ascent order  */
static GT_U32 tcArr[PRV_TGF_INGR_PORTS_NUM_CNS] = {2, 3, 4};

/* array of QoS Profile for test */
static GT_U8 qosArr[PRV_TGF_INGR_PORTS_NUM_CNS] = {8, 23, 120};

/* array of Ingress Ports indexes */
static GT_U32 inPortsIdxArr[PRV_TGF_INGR_PORTS_NUM_CNS] = {1, 2, 3};

/* array to store arbiter mode */
static CPSS_PORT_TX_Q_ARB_GROUP_ENT savePortTxArpArr[PRV_TGF_INGR_PORTS_NUM_CNS];

/* scheduler profile */
static GT_U32 prvPortTxProfile;

/* tail drop profile */
static CPSS_PORT_TX_DROP_PROFILE_SET_ENT prvPortTailDropProfile;

/* default number of packets to send */
static GT_U32   prvTgfBurstCount   = 1;

/* L2 part of packet 1 */
static TGF_PACKET_L2_STC prvTgfPacket1L2Part = {
    {0x00, 0xAA, 0xBB, 0xCC, 0xDD, 0x11},               /* dstMac */
    {0x00, 0x88, 0x99, 0x33, 0x44, 0x11}                /* srcMac */
};

/* L2 part of packet 2 */
static TGF_PACKET_L2_STC prvTgfPacket2L2Part = {
    {0x00, 0xAA, 0xBB, 0xCC, 0xDD, 0x11},               /* dstMac */
    {0x00, 0x11, 0x22, 0x55, 0x66, 0x22}                /* srcMac */
};

/* L2 part of packet 3 */
static TGF_PACKET_L2_STC prvTgfPacket3L2Part = {
    {0x00, 0xAA, 0xBB, 0xCC, 0xDD, 0x11},               /* dstMac */
    {0x00, 0xAA, 0x22, 0xCC, 0x66, 0x33}                /* srcMac */
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

/* PAYLOAD part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketPayloadPart = {
    sizeof(prvTgfPayloadDataArr),                       /* dataLength */
    prvTgfPayloadDataArr                                /* dataPtr */
};

/* PARTS of packet 1 */
static TGF_PACKET_PART_STC prvTgfPacket1PartArray[] = {
    {TGF_PACKET_PART_L2_E,        &prvTgfPacket1L2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTag0Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/* PARTS of packet 2 */
static TGF_PACKET_PART_STC prvTgfPacket2PartArray[] = {
    {TGF_PACKET_PART_L2_E,        &prvTgfPacket2L2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTag0Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/* PARTS of packet 3 */
static TGF_PACKET_PART_STC prvTgfPacket3PartArray[] = {
    {TGF_PACKET_PART_L2_E,        &prvTgfPacket3L2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTag0Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketPayloadPart}
};

/* LENGTH of packet */
#define PRV_TGF_PACKET_LEN_CNS \
    (TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + sizeof(prvTgfPayloadDataArr))

/* LENGTH of packet with CRC */
#define PRV_TGF_PACKET_CRC_LEN_CNS  PRV_TGF_PACKET_LEN_CNS + TGF_CRC_LEN_CNS
    

/* PACKET 1 to send */
static TGF_PACKET_STC prvTgfPacket1Info = {
    PRV_TGF_PACKET_LEN_CNS,                                      /* totalLen */
    sizeof(prvTgfPacket1PartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacket1PartArray                                        /* partsArray */
};


/* PACKET 2 to send */
static TGF_PACKET_STC prvTgfPacket2Info = {
    PRV_TGF_PACKET_LEN_CNS,                                      /* totalLen */
    sizeof(prvTgfPacket2PartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacket2PartArray                                        /* partsArray */
};

/* PACKET 3 to send */
static TGF_PACKET_STC prvTgfPacket3Info = {
    PRV_TGF_PACKET_LEN_CNS,                                      /* totalLen */
    sizeof(prvTgfPacket3PartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacket3PartArray                                        /* partsArray */
};

/* FDB MAC entry */
static PRV_TGF_BRG_MAC_ENTRY_STC prvTgfMacEntry;

/* variables to save configuration */
static GT_U16 savePortMaxBuffLimit;
static GT_U16 savePortMaxDescrLimit;
static GT_BOOL saveEnableSharing;


/*******************************************************************************
* prvTgfPortWsRateCheck
*
* DESCRIPTION:
*       Check rate by WS rate of port
*
* INPUTS:
*       portIdx    - index of egress port
*       rate       - rate to check in packets per second
*       packetSize - packet size in bytes including CRC
*
* OUTPUTS:
*       none
*
* RETURNS:
*       0 - input rate is WS
*       other - difference between input rate and WS one
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 prvTgfPortWsRateCheck
(
    IN  GT_U32 portIdx,
    IN  GT_U32 rate,
    IN  GT_U32 packetSize
)
{
    GT_U32      packetRate; /* packet rate in packets per second */
    GT_U32      percent;     /* difference between ports WS rate 
                               and input one in percent */

    /* get port's WS rate */
    packetRate = prvTgfCommonPortWsRateGet(portIdx,packetSize);

    /* calculate difference between ports WS rate and input one */
    percent = prvTgfCommonDiffInPercentCalc(packetRate, rate);

    if (percent > prvTgfWsDiffInPercent)
    {
        /* big difference */
        return percent;
    }
    else
    {
        return 0;
    } 
}

/*******************************************************************************
* prvTgfPortTxSpSchedulerStopSend
*
* DESCRIPTION:
*       Function stops sends packets
*
* INPUTS:
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
GT_VOID prvTgfPortTxSpSchedulerStopSend
(
    IN  GT_VOID
)
{
    GT_STATUS       rc         = GT_OK;
    GT_U32          i;
    GT_U8           portNum;

    for (i = 0; i < PRV_TGF_INGR_PORTS_NUM_CNS; i++)
    {
        portNum = prvTgfPortsArray[inPortsIdxArr[i]];

        /* stop send Packet from port portNum */
        rc = prvTgfStopTransmitingEth(prvTgfDevNum, portNum);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StopTransmitting: %d, %d",
                                     prvTgfDevNum, portNum);

        /* configure default mode */
        rc = prvTgfTxModeSetupEth(prvTgfDevNum, portNum, 
                                  PRV_TGF_TX_MODE_SINGLE_BURST_E,
                                  PRV_TGF_TX_CONT_MODE_UNIT_WS_E,
                                  0);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfTxModeSetupEth: %d, %d",
                                     prvTgfDevNum, portNum);

    }

    /* wait some time to guaranty that traffic stopped */
    cpssOsTimerWkAfter(10);
}

/*******************************************************************************
* prvTgfPortTxSpSchedulerFdbConfigurationSet
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
static GT_VOID prvTgfPortTxSpSchedulerFdbConfigurationSet
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
    prvTgfMacEntry.dstInterface.devPort.portNum = prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_0_CNS];
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

    cpssOsMemCpy(prvTgfMacEntry.key.key.macVlan.macAddr.arEther, prvTgfPacket1L2Part.daMac, 6);

    /* DA on port PRV_TGF_EGR_PORT_IDX_0_CNS */
    rc = prvTgfBrgFdbMacEntrySet(&prvTgfMacEntry);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in prvTgfBrgFdbMacEntrySet %d", GT_TRUE);
}

/*******************************************************************************
* prvTgfPortTxSpSchedulerBrgConfigurationSet
*
* DESCRIPTION:
*       Set Bridge configuration.
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
GT_VOID prvTgfPortTxSpSchedulerBrgConfigurationSet
(
    GT_VOID    
)
{
    GT_STATUS rc;

    /* create a VLAN, all ports are tagged because packets are send tagged too. */
    rc = prvTgfBrgDefVlanEntryWriteWithTaggingCmd(PRV_TGF_VLANID_CNS, 
                                                  PRV_TGF_BRG_VLAN_PORT_TAG0_CMD_E);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWriteWithTaggingCmd: %d, %d", 
                                 prvTgfDevNum, PRV_TGF_VLANID_CNS);

    prvTgfPortTxSpSchedulerFdbConfigurationSet();
}

/*******************************************************************************
* prvTgfPortTxSpSchedulerConfigurationSet
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
GT_VOID prvTgfPortTxSpSchedulerConfigurationSet
(
    GT_VOID
)
{
    GT_U32                      i;         /* iterator */
    GT_STATUS                   rc;        /* return code */
    GT_U8                       portNum;   /* port number */
    CPSS_QOS_ENTRY_STC          portQosCfg; /* port QoS configuration */
    PRV_TGF_COS_PROFILE_STC     qosProfile; /* QoS Profile */

    /* enable tail drop by set HOL mode */
    rc = prvTgfPortFcHolSysModeSet(prvTgfDevNum, PRV_TGF_PORT_PORT_HOL_E);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPortFcHolSysModeSet: %d, %d",
                                 prvTgfDevNum, PRV_TGF_PORT_PORT_HOL_E);

    if(GT_FALSE == prvUtfDeviceTestSupport(prvTgfDevNum, UTF_LION_E))
    {
        /* disable global resource sharing */
        rc = prvTgfPortTxSharingGlobalResourceEnableSet(prvTgfDevNum, GT_FALSE);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfPortTxSharingGlobalResourceEnableSet");
    }

    /* get tail drop profile */
    rc = prvTgfPortTxBindPortToDpGet(prvTgfDevNum,
                                     prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_0_CNS],
                                     &prvPortTailDropProfile);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPortTxBindPortToDpGet: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_0_CNS]);

    /* save profile settings */
    rc = prvTgfPortTxTailDropProfileGet(prvTgfDevNum, prvPortTailDropProfile,
                                        &saveEnableSharing,
                                        &savePortMaxBuffLimit, 
                                        &savePortMaxDescrLimit);
    UTF_VERIFY_EQUAL2_STRING_MAC(rc, GT_OK, "prvTgfPortTxTailDropProfileGet: %d, %d",
                                 prvTgfDevNum, prvPortTailDropProfile);

    /* configure port's limits to be TC limit * 8.
      where TC limit are 25 descriptors.  */
    rc = prvTgfPortTxTailDropProfileSet(prvTgfDevNum, prvPortTailDropProfile,
                                        GT_FALSE,
                                        200, 200);
    UTF_VERIFY_EQUAL5_STRING_MAC(rc, GT_OK, "prvTgfPortTxTailDropProfileSet: %d, %d, %d, %d, %d",
                                 prvTgfDevNum, prvPortTailDropProfile, GT_FALSE,
                                 200, 200);

    /* get scheduler profile */
    rc = prvTgfPortTxSchedulerProfileIndexGet(prvTgfDevNum,
                                              prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_0_CNS],
                                              &prvPortTxProfile);

    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPortTxSchedulerProfileIndexGet: %d, %d",
                                 prvTgfDevNum, prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_0_CNS]);

    for (i = 0; i < PRV_TGF_INGR_PORTS_NUM_CNS; i++)
    {
        /* store arbiter configuration */
        rc = prvTgfPortTxSchedulerArbGroupGet(prvTgfDevNum, prvPortTxProfile, 
                                              tcArr[i], &(savePortTxArpArr[i]));
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPortTxSchedulerArbGroupGet: %d, %d",
                                     prvTgfDevNum, i);

        /* set SP arbiter configuration */
        rc = prvTgfPortTxSchedulerArbGroupSet(prvTgfDevNum, prvPortTxProfile, 
                                              tcArr[i], CPSS_PORT_TX_SP_ARB_GROUP_E);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPortTxSchedulerArbGroupSet: %d, %d",
                                     prvTgfDevNum, i);

        portNum = prvTgfPortsArray[inPortsIdxArr[i]];

        /* set NO QoS trust mode to use QoS profile of port */
        rc = prvTgfCosPortQosTrustModeSet(portNum, CPSS_QOS_PORT_NO_TRUST_E);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfCosPortQosTrustModeSet: %d, %d",
                                     prvTgfDevNum, portNum);

        /* Configure Port QoS attributes */
        portQosCfg.assignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        portQosCfg.enableModifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
        portQosCfg.enableModifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
        portQosCfg.qosProfileId     = qosArr[i];
        rc = prvTgfCosPortQosConfigSet(portNum,
                                       &portQosCfg);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfCosPortQosConfigSet: %d", prvTgfDevNum);

        /* Set packets TC */
        qosProfile.dropPrecedence = 0;
        qosProfile.trafficClass   = tcArr[i];
        qosProfile.dscp           = 0;
        qosProfile.exp            = 0;
        qosProfile.userPriority   = 0;

        rc = prvTgfCosProfileEntrySet(qosArr[i], &qosProfile);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfCosProfileEntrySet");
    }

    prvTgfPortTxSpSchedulerBrgConfigurationSet();
}

/*******************************************************************************
* prvTgfPortTxSpSchedulerRestore
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
GT_VOID prvTgfPortTxSpSchedulerRestore
(
    GT_VOID
)
{
    GT_U32                      i;         /* iterator */
    CPSS_QOS_ENTRY_STC          cosEntry;  /* QoS entry */
    PRV_TGF_COS_PROFILE_STC     qosProfile; /* QoS profile */
    GT_STATUS                   rc;         /* return code */
    GT_U8                       portNum;    /* port number */

    /* stop send packets */
    prvTgfPortTxSpSchedulerStopSend();

    /* restore FC mode */
    rc = prvTgfPortFcHolSysModeSet(prvTgfDevNum, PRV_TGF_PORT_PORT_FC_E);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPortFcHolSysModeSet: %d, %d",
                                 prvTgfDevNum, PRV_TGF_PORT_PORT_FC_E);

    if(GT_FALSE == prvUtfDeviceTestSupport(prvTgfDevNum, UTF_LION_E))
    {
        /* enable global resource sharing */
        rc = prvTgfPortTxSharingGlobalResourceEnableSet(prvTgfDevNum, GT_TRUE);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfPortTxSharingGlobalResourceEnableSet");
    }


    /* restore profile settings */
    rc = prvTgfPortTxTailDropProfileSet(prvTgfDevNum, prvPortTailDropProfile,
                                        saveEnableSharing,
                                        savePortMaxBuffLimit, savePortMaxDescrLimit);
    UTF_VERIFY_EQUAL5_STRING_MAC(rc, GT_OK, "prvTgfPortTxTailDropProfileSet: %d, %d, %d, %d, %d",
                                 prvTgfDevNum, prvPortTailDropProfile, GT_FALSE,
                                 savePortMaxBuffLimit, savePortMaxDescrLimit);
    
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d, %d", 
                                 prvTgfDevNum, PRV_TGF_VLANID_CNS);

    /* flush FDB include static entries */
    rc = prvTgfBrgFdbFlush(GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgFdbFlush: %d, %d", prvTgfDevNum, GT_TRUE);
   
    for (i = 0; i < PRV_TGF_INGR_PORTS_NUM_CNS; i++)
    {
        /* restore arbiter configuration */
        rc = prvTgfPortTxSchedulerArbGroupSet(prvTgfDevNum, prvPortTxProfile, tcArr[i], savePortTxArpArr[i]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfPortTxSchedulerArbGroupSet: %d, %d",
                                     prvTgfDevNum, i);

        portNum = prvTgfPortsArray[inPortsIdxArr[i]];

        /* restore QoS configuration */
        rc = prvTgfCosPortQosTrustModeSet(portNum, CPSS_QOS_PORT_TRUST_L2_L3_E);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfCosPortQosTrustModeSet: %d, %d",
                                     prvTgfDevNum, portNum);

        /* map send port to Qos Profile0 */
        cosEntry.qosProfileId     = 0;
        cosEntry.enableModifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
        cosEntry.enableModifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
        cosEntry.assignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

        rc = prvTgfCosPortQosConfigSet(portNum, &cosEntry);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfCosPortQosConfigSet");

        /* Reset packets TC */
        qosProfile.dropPrecedence = 0;
        qosProfile.trafficClass   = 0;
        qosProfile.dscp           = 0;
        qosProfile.exp            = 0;
        qosProfile.userPriority   = 0;

        rc = prvTgfCosProfileEntrySet(qosArr[i], &qosProfile);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfCosProfileEntrySet");
    }
}

/*******************************************************************************
* prvTgfPortTxSpSchedulerPacketSend
*
* DESCRIPTION:
*       Function sends packet.
*
* INPUTS:
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
static GT_VOID prvTgfPortTxSpSchedulerPacketSend
(
    IN  GT_VOID
)
{
    GT_STATUS       rc;                         /* return code*/
    GT_U32          portsCount = prvTgfPortsNum; /* number of used ports */
    GT_U32          portIter;                    /* iterator */
    TGF_PACKET_STC      *packetInfoPtr;          /* pointer to packet info */
    GT_U32          i;                           /* iterator */
    GT_U8           portNum;                     /* port number */
    TGF_PACKET_STC  *packetInfoPtrArr[] = {&prvTgfPacket1Info, &prvTgfPacket2Info, &prvTgfPacket3Info};
                                                 /* array of packet info */
   
    /* Send packet Vlan Tagged */
    tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_8100_VLAN_TAG_CNS);

    /* reset counters */
    for (portIter = 0; portIter < portsCount; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    for (i = 0; i < PRV_TGF_INGR_PORTS_NUM_CNS; i++)
    {
        packetInfoPtr = packetInfoPtrArr[i];
        portNum = prvTgfPortsArray[inPortsIdxArr[i]];

        /* setup Packet */
        rc = prvTgfSetTxSetupEth(prvTgfDevNum, packetInfoPtr, prvTgfBurstCount, 0, NULL);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "prvTgfSetTxSetupEth: %d, %d",
                                     prvTgfDevNum, prvTgfBurstCount, 0, NULL);

        /* configure WS mode */
        rc = prvTgfTxModeSetupEth(prvTgfDevNum, portNum, 
                                  PRV_TGF_TX_MODE_CONTINUOUS_E,
                                  PRV_TGF_TX_CONT_MODE_UNIT_WS_E,
                                  0);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfTxModeSetupEth: %d, %d",
                                     prvTgfDevNum, portNum);

        /* send Packet from port portNum */
        rc = prvTgfStartTransmitingEth(prvTgfDevNum, portNum);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, %d",
                                     prvTgfDevNum, portNum);
    }

}

/*******************************************************************************
* prvTgfPortTxSpSchedulerTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic and test results.
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
GT_VOID prvTgfPortTxSpSchedulerTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS    rc;      /* return code */
    GT_U32       txRate;  /* Tx rate */
    GT_U32       tmp;     /* variable */
    GT_U32       ii, kk;  /* iterators */
    GT_U32       rateOk;  /* is rate OK */
    GT_U8        portNum; /* port number */
    GT_U32       expectRate[PRV_TGF_INGR_PORTS_NUM_CNS]; /* array of expected rates */
    GT_U32       actualRate[PRV_TGF_INGR_PORTS_NUM_CNS]; /* array of actual rates */
    GT_U32       expectDropRate[PRV_TGF_INGR_PORTS_NUM_CNS]; /* array of expected drop rates */
    GT_U32       actualDropRate[PRV_TGF_INGR_PORTS_NUM_CNS]; /* array of actual drop rates */

    /* send packets */
    prvTgfPortTxSpSchedulerPacketSend();

    /* wait to get stable traffic */
    cpssOsTimerWkAfter(10);

    /* check that egress port get WS traffic */
    prvTgfCommonPortTxRateGet(PRV_TGF_EGR_PORT_IDX_0_CNS, 1000, &txRate);
    tmp = prvTgfPortWsRateCheck(PRV_TGF_EGR_PORT_IDX_0_CNS, txRate, PRV_TGF_PACKET_CRC_LEN_CNS); 
    UTF_VERIFY_EQUAL2_STRING_MAC(0, tmp, "wrong egress rate in WS percent %d, port %d",
                                     tmp, prvTgfPortsArray[PRV_TGF_EGR_PORT_IDX_0_CNS]);

    PORT_TX_PRV_UTF_LOG1_MAC("[TxSpScheduler]: port rate %d\n", txRate);

    /* index of ingress ports is priority 
       maximal index take 100% of traffic */
    for (ii = 0; ii < (PRV_TGF_INGR_PORTS_NUM_CNS -1); ii++)
    {
        expectRate[ii] = 0;
        expectDropRate[ii] = txRate;
    }

    /* highest priority */
    expectRate[ii] = txRate;
    expectDropRate[ii] = 0;

    for (ii = 0; ii < PRV_TGF_INGR_PORTS_NUM_CNS; ii++)
    {
        PORT_TX_PRV_UTF_LOG1_MAC("[TxSpScheduler]: iteration %d\n", ii);

        /* get rates for TCs */
        prvTgfCommonPortTxTwoUcTcRateGet(PRV_TGF_EGR_PORT_IDX_0_CNS, tcArr[0], tcArr[1],
                                 1000, &actualRate[0], &actualRate[1],
                                 &actualDropRate[0], &actualDropRate[1]);
        prvTgfCommonPortTxTwoUcTcRateGet(PRV_TGF_EGR_PORT_IDX_0_CNS, tcArr[1], tcArr[2],
                                 1000, &actualRate[1], &actualRate[2],
                                 &actualDropRate[1], &actualDropRate[2]);

        /* compare rates */
        for (kk = 0; kk < PRV_TGF_INGR_PORTS_NUM_CNS; kk++)
        {
            PORT_TX_PRV_UTF_LOG2_MAC("[TxSpScheduler]: compare rate %d drop %d\n", actualRate[kk], actualDropRate[kk]);
            if (expectRate[kk])
            {
                /* the TC counter rate is not exact. 
                  get percentage of difference and compare with permitted accuracy.*/
                tmp = prvTgfCommonDiffInPercentCalc(expectRate[kk],actualRate[kk]); 
                rateOk = (tmp <= prvTgfWsDiffInPercent)? 1 : 0;
                UTF_VERIFY_EQUAL4_STRING_MAC(1, rateOk," wrong rate, iter %d idx %d expected %d actual %d\n", 
                                             ii, kk, expectRate[kk],actualRate[kk]);
            }
            else
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(0,actualRate[kk]," wrong zero rate, idx %d\n", kk);
            }

            if (expectDropRate[kk])
            {
                /* the TC counter rate is not exact. 
                  get percentage of difference and compare with permitted accuracy.*/
                tmp = prvTgfCommonDiffInPercentCalc(expectDropRate[kk],actualDropRate[kk]); 
                rateOk = (tmp <= prvTgfWsDiffInPercent)? 1 : 0;
                UTF_VERIFY_EQUAL4_STRING_MAC(1, rateOk," wrong drop rate, iter %d idx %d expected %d actual %d\n", 
                                             ii, kk, expectDropRate[kk],actualDropRate[kk]);
            }
            else
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(0,actualDropRate[kk]," wrong zero drop rate, iter %d idx %d\n", ii, kk);
            }
        }

        /* disable highest priority traffic */
        tmp = PRV_TGF_INGR_PORTS_NUM_CNS - 1 - ii;
        portNum = prvTgfPortsArray[inPortsIdxArr[tmp]];

        if (prvTgfPortTxSpDebug == 0)
        {
            /* stop send Packet from port portNum */
            rc = prvTgfStopTransmitingEth(prvTgfDevNum, portNum);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StopTransmitting: %d, %d",
                                         prvTgfDevNum, portNum);

            /* wait to stop traffic */
             cpssOsTimerWkAfter(10);
        }
        /* correct expected results */
        if (tmp)
        {
            /* change highest priotity traffic */
            expectRate[tmp-1] = txRate;
            expectRate[tmp] = 0;
            expectDropRate[tmp-1] = 0;
            expectDropRate[tmp] = 0;
        }
    }
}


GT_VOID prvTgfPortTxTcRateDump
(
    IN  GT_U32 portIdx,
    IN  GT_U32 tc1,
    IN  GT_U32 tc2,
    IN  GT_U32 timeOut
)
{
    GT_U32 outRate1;
    GT_U32 outRate2;
    GT_U32 dropRate1;
    GT_U32 dropRate2;
    GT_U32 portRate;

    /* get rates */
    prvTgfCommonPortTxTwoUcTcRateGet( portIdx, tc1, tc2, timeOut,
                           &outRate1, &outRate2, &dropRate1, &dropRate2);

    prvTgfCommonPortTxRateGet(portIdx, timeOut,&portRate);

    /* print rates */
    cpssOsPrintf("Rate Dump: port %d  TC1=%d  TC2=%d  timeout=%d\n",
                 prvTgfPortsArray[portIdx], tc1, tc2, timeOut);
    cpssOsPrintf("OUT  Rate: %10d %10d %10d\n", 
                 outRate1, outRate2, outRate1 + outRate2);
    cpssOsPrintf("Drop Rate: %10d %10d %10d\n", 
                 dropRate1, dropRate2, dropRate1 + dropRate2);

    cpssOsPrintf("Port Rate: %10d\n", portRate);


}

GT_VOID prvTgfPortsRxRateDump
(
    IN  GT_U32 timeOut
)
{
    GT_U32 portIdx;
    GT_U32 rxRate;

    cpssOsPrintf("Rx Rate Dump: timeout=%d\n", timeOut);
    for (portIdx = 0; portIdx < 4; portIdx++)
    {
        prvTgfCommonPortRxRateGet(portIdx, timeOut, &rxRate);
        cpssOsPrintf(" port %3d  Rx rate=%10d\n",
                     prvTgfPortsArray[portIdx], rxRate);
    }
}

GT_VOID prvTgfPortsRatesDump
(
    IN  GT_U32 timeOut
)
{
    GT_U32 portIdx;
    GT_U32 rxRate;
    GT_U32 txRate;

    cpssOsPrintf("Rx and Tx Rates Dump: timeout=%d\n", timeOut);
    for (portIdx = 0; portIdx < 4; portIdx++)
    {
        prvTgfCommonPortRxRateGet(portIdx, timeOut, &rxRate);
        prvTgfCommonPortTxRateGet(portIdx, timeOut, &txRate);
        cpssOsPrintf(" port %3d Rx rate=%10d Tx rate=%10d\n",
                     prvTgfPortsArray[portIdx], rxRate, txRate);
    }
}


