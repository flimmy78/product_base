/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfBrgRateLimitMcastReg.c
*
* DESCRIPTION:
*       Bridge Generic Port Rate Limit Multicast Registered packets UT.
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
#include <bridge/prvTgfBrgGen.h>
#include <bridge/prvTgfBrgRateLimitMcastReg.h>

/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/
/* default VLAN0 Id */
#define PRV_TGF_VLANID_CNS  2

/* VDIX to send traffic to */
#define PRV_TGF_SEND_VIDX_CNS   0xF

/* default number of packets to send */
static GT_U32  prvTgfBurstCount   = 2;

/* the number of vidx group members (two first indexes) */
static GT_U8  prvTgfMcMembersNum = 2;

/* port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_CNS   prvTgfMcMembersNum


/******************************* Test packet **********************************/

/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part = {
    {0x01, 0x22, 0x33, 0x44, 0x55, 0x66},               /* dstMac */
    {0x00, 0x00, 0x00, 0x00, 0x22, 0x22}                /* srcMac */
};

/* Multicast unregistered da mac */
static TGF_MAC_ADDR     prvTgfMcastUnregDaMac = 
    {0x01, 0x77, 0x88, 0x99, 0xAA, 0xBB};

/* Multicast registered da mac */
static TGF_MAC_ADDR     prvTgfMcastRegDaMac = 
    {0x01, 0x22, 0x33, 0x44, 0x55, 0x66};

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
    0x00, 0x00, 0x00, 0x00
};

/* PAYLOAD part */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketPayloadPart = {
    sizeof(prvTgfPayloadDataArr),                       /* dataLength */
    prvTgfPayloadDataArr                                /* dataPtr */
};

/* PARTS of packet */
static TGF_PACKET_PART_STC prvTgfPacketPartArray[] = {
    {TGF_PACKET_PART_L2_E,       &prvTgfPacketL2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E, &prvTgfPacketVlanTag0Part},
    {TGF_PACKET_PART_PAYLOAD_E,  &prvTgfPacketPayloadPart}
};

/* LENGTH of packet */
#define PRV_TGF_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + sizeof(prvTgfPayloadDataArr)

/* LENGTH of packet with CRC */
#define PRV_TGF_PACKET_CRC_LEN_CNS  (PRV_TGF_PACKET_LEN_CNS + TGF_CRC_LEN_CNS)

/* PACKET to send */
static TGF_PACKET_STC prvTgfPacketInfo = {
    PRV_TGF_PACKET_LEN_CNS,                                      /* totalLen */
    sizeof(prvTgfPacketPartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketPartArray                                        /* partsArray */
};

/* stored default drop mode */
static PRV_TGF_BRG_DROP_CNTR_MODE_ENT prvTgfDefDropMode = PRV_TGF_BRG_DROP_CNTR_COUNT_ALL_E;

/* stored default rate limit value */
static GT_U32  prvTgfDefRateLimit = 0xFFFF;

/* stored default rate limit parameters */
static PRV_TGF_BRG_GEN_RATE_LIMIT_STC   prvTgfDefRateLimitCfg;

/******************************************************************************\
 *                            Private test functions                          *
\******************************************************************************/
/*******************************************************************************
* prvTgfBrgRateLimitMcastRegCfg
*
* DESCRIPTION:
*       Set VLAN entry.
*
* INPUTS:
*       enableMcastReg    - GT_TRUE: enable rate limit for multicast registered packets
*       enableMcastUnReg  - GT_TRUE: enable rate limit for multicast unregistered packets
*       rateLimit         - rate limit specified in 64 bytes/packets
*                           depends on ratelimit mode, range 0 - 65535
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfBrgRateLimitMcastRegCfg
(
    IN GT_BOOL                              enableMcastReg,
    IN GT_BOOL                              enableMcastUnReg,
    IN GT_U32                               rateLimit
)
{
    GT_STATUS   rc;
    PRV_TGF_BRG_GEN_RATE_LIMIT_PORT_STC     portGfg;
    PRV_TGF_BRG_GEN_RATE_LIMIT_STC          globalRateLimitCfg;


    /* create MC Group with the needed ports */
    rc = prvTgfBrgVidxEntrySet(PRV_TGF_SEND_VIDX_CNS, prvTgfPortsArray,
                               NULL, prvTgfMcMembersNum);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVidxEntrySet: %d, %d",
                                 prvTgfDevNum, PRV_TGF_SEND_VIDX_CNS);

    /* create a new MAC Entry into FDB Table with VIDX interface */
    rc = prvTgfBrgDefFdbMacEntryOnVidxSet(prvTgfMcastRegDaMac, PRV_TGF_VLANID_CNS,
                                          PRV_TGF_SEND_VIDX_CNS, GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefFdbMacEntryOnVidxSet: %d", prvTgfDevNum);

    /* Store default global rate limit configuration */
    rc = prvTgfBrgGenRateLimitGlobalCfgGet(prvTgfDevNum, &prvTgfDefRateLimitCfg);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgGenRateLimitGlobalCfgGet: %d", prvTgfDevNum);


    globalRateLimitCfg.dropMode = CPSS_DROP_MODE_HARD_E;
    globalRateLimitCfg.rMode = CPSS_RATE_LIMIT_PCKT_BASED_E;
    globalRateLimitCfg.win1000Mbps = (0x3F * 256);
    globalRateLimitCfg.win100Mbps = (0x1FF * 256);
    globalRateLimitCfg.win10Gbps = ((0xFFF * 256) / 10);
    globalRateLimitCfg.win10Mbps = (0xFFF * 256);
    /* unused parameter */
    globalRateLimitCfg.rateLimitModeL1 = 0;
    
    /* configure rate limit global parameters */
    rc = prvTgfBrgGenRateLimitGlobalCfgSet(prvTgfDevNum, &globalRateLimitCfg);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgGenRateLimitGlobalCfgSet: %d", prvTgfDevNum);


    portGfg.enableMc = enableMcastUnReg;
    portGfg.enableMcReg = enableMcastReg;
    portGfg.rateLimit = rateLimit;
    /* default values */
    portGfg.enableBc = GT_FALSE;
    portGfg.enableTcpSyn = GT_FALSE;
    portGfg.enableUcKnown = GT_FALSE;
    portGfg.enableUcUnk = GT_FALSE;

    /* configure rate limit for specific ingress port */
    rc = prvTgfBrgGenPortRateLimitSet(prvTgfDevNum, 
                                      prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], 
                                      &portGfg);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgGenRateLimitSet: %d", prvTgfDevNum);
}

/*******************************************************************************
* prvTgfBrgGenRateLimitMcastRegPacketSend
*
* DESCRIPTION:
*       Function sends packet, performs trace and check expected results.
*
* INPUTS:
*       portNum - port number
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
static GT_VOID prvTgfBrgGenRateLimitMcastRegPacketSend
(
    IN GT_U8 portNum
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

    /* setup Packet */
    rc = prvTgfSetTxSetupEth(prvTgfDevNum, &prvTgfPacketInfo, prvTgfBurstCount, 0, NULL);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "ERROR of SetTxSetup: %d, %d",
                                 prvTgfDevNum, prvTgfBurstCount, 0, NULL);

    /* clear table */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");


    /* send Packet from port 0 */
    rc = prvTgfStartTransmitingEth(prvTgfDevNum, portNum);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, %d",
                                 prvTgfDevNum, portNum);
}


/*******************************************************************************
* prvTgfBrgGenRateLimitMcastRegTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic and check results
*
* INPUTS:
* INPUTS:
*       port              - port number (including CPU port)
*       enableMcastReg    - GT_TRUE: enable rate limit for multicast registered packets
*       enableMcastUnReg  - GT_TRUE: enable rate limit for multicast unregistered packets
*       rateLimit         - rate limit specified in 64 bytes/packets
*                           depends on ratelimit mode, range 0 - 65535
*       sendMcastRegPkts  - GT_TRUE: muticast registered packets should be sent 
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
static GT_VOID prvTgfBrgGenRateLimitMcastRegTrafficGenerate
(
    IN GT_BOOL                              enableMcastReg,
    IN GT_BOOL                              enableMcastUnReg,
    IN GT_U32                               rateLimit,
    IN GT_BOOL                              sendMcastRegPkts
)
{
    GT_STATUS rc;                               /* returned status */
    GT_U8  portIter;                            /* loop port iterator */
    CPSS_PORT_MAC_COUNTER_SET_STC portCntrs;    /* port counters */
    GT_U32  expectedPktsNum;                    /* expected packets number */
    GT_U32  expectedDropPkts;                   /* expected dropped packets */
    GT_U32  dropPkts;                           /* dropped packets */
    PRV_TGF_BRG_GEN_RATE_LIMIT_PORT_STC  portGfg;   /* port rate limit configurations */

    /* Configure Rate Limit and global rate limit parameters */
    prvTgfBrgRateLimitMcastRegCfg(enableMcastReg,
                                  enableMcastUnReg,
                                  rateLimit);

   /* configure drop counter to count only packets dropped due to rate limit  */
    rc = prvTgfBrgCntDropCntrModeSet(PRV_TGF_BRG_DROP_CNTR_RATE_LIMIT_E);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgCntDropCntrModeSet: %d, %d",
                                 prvTgfDevNum, prvTgfDefDropMode);

    /* clear drop counters */
    rc = prvTgfBrgCntDropCntrSet(0);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgCntDropCntrSet: %d", prvTgfDevNum);


    if (sendMcastRegPkts == GT_TRUE) 
    {
        /* Set destination addres to multicast registered */
        cpssOsMemCpy(prvTgfPacketL2Part.daMac, prvTgfMcastRegDaMac,
                         sizeof(prvTgfPacketL2Part.daMac));
    }
    else
    {
        /* Set destination addres to multicast unregistered */
        cpssOsMemCpy(prvTgfPacketL2Part.daMac, prvTgfMcastUnregDaMac,
                         sizeof(prvTgfPacketL2Part.daMac));
    }

    /* send L2 packet */
    prvTgfBrgGenRateLimitMcastRegPacketSend(prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS]);

    if (rateLimit == 0) 
    {
        /* one packet will egress */
        rateLimit = 1;
    }

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

        if (PRV_TGF_SEND_PORT_IDX_CNS == portIter)
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(prvTgfBurstCount, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
        }

        else if (portIter < prvTgfMcMembersNum) 
        {
            if ((enableMcastReg == GT_TRUE) && (sendMcastRegPkts == GT_TRUE))
            {
                expectedPktsNum = rateLimit;
            }
            else
            {
                expectedPktsNum = prvTgfBurstCount;
            }

            /* check that packets received on Tx ports are as expected */
            UTF_VERIFY_EQUAL0_STRING_MAC(expectedPktsNum, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }

        else
        {
            if (sendMcastRegPkts == GT_TRUE) 
            {
                expectedPktsNum = 0;
            }
            else
            {
                expectedPktsNum = prvTgfBurstCount;
            }

            /* check Tx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(expectedPktsNum, portCntrs.goodPktsSent.l[0],
                                         "get another goodPktsSent counter than expected");
        }
    }

    if ((enableMcastReg == GT_TRUE) && (sendMcastRegPkts == GT_TRUE))
    {
        expectedDropPkts = prvTgfBurstCount - rateLimit;
    }
    else
    {
        expectedDropPkts = 0;
    }

    /* get drop counter */
    rc = prvTgfBrgCntDropCntrGet(&dropPkts);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgCntDropCntrGet: %d", prvTgfDevNum);

    /* check that dropped packets number is as expected */
    UTF_VERIFY_EQUAL0_STRING_MAC(expectedDropPkts, dropPkts,
                                 "get another drop counter than expected");

    /* Restore default configurations */

    /* Restore Drop counter mode */
    rc = prvTgfBrgCntDropCntrModeSet(prvTgfDefDropMode);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgCntDropCntrModeSet: %d, %d",
                                 prvTgfDevNum, prvTgfDefDropMode);
    /* clear drop counters */
    rc = prvTgfBrgCntDropCntrSet(0);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgCntDropCntrSet: %d", prvTgfDevNum);

    /* Restore default global rate limit configuration */
    rc = prvTgfBrgGenRateLimitGlobalCfgSet(prvTgfDevNum, &prvTgfDefRateLimitCfg);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgGenRateLimitGlobalCfgSet: %d", prvTgfDevNum);

    /* Restore default port rate limit configurations */
    portGfg.enableMc = GT_FALSE;
    portGfg.enableMcReg = GT_FALSE;
    portGfg.rateLimit = prvTgfDefRateLimit;
    portGfg.enableBc = GT_FALSE;
    portGfg.enableTcpSyn = GT_FALSE;
    portGfg.enableUcKnown = GT_FALSE;
    portGfg.enableUcUnk = GT_FALSE;

    /* configure rate limit for ingress port2 */
    rc = prvTgfBrgGenPortRateLimitSet(prvTgfDevNum, 
                                      prvTgfPortsArray[PRV_TGF_SEND_PORT_IDX_CNS], 
                                      &portGfg);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgGenRateLimitSet: %d", prvTgfDevNum);
}

/******************************************************************************\
 *                            Test implementation                             *
\******************************************************************************/
/*******************************************************************************
* prvTgfBrgGenRateLimitMcastRegConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Create VLAN 2 on all ports (0,8,18,23)
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
GT_VOID prvTgfBrgGenRateLimitMcastRegConfigurationSet
(
    GT_VOID
)
{
    GT_STATUS   rc;

    /* 1. Create new vlan on ports (0,0), (0,8), (0,18), (0,23) */
    rc = prvTgfBrgDefVlanEntryWrite(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryWrite: %d, %d",
                                 prvTgfDevNum, PRV_TGF_VLANID_CNS);
}

/*******************************************************************************
* prvTgfBrgGenRateLimitMcastRegEnableRegTrafficGenerate
*
* DESCRIPTION:
*           Enable Rate Limit on port port2 for multicast registered packets.
*           Create vidx 0xF with port members port0 and port1.
*           Configure MAC 01:22:33:44:55:66 on vidx 0xF 
*           Set window size to maximum and drop command to hard
*           Set rate limit to 1.
*       Generate traffic:
*           Send from port2 single tagged packet:
*               macDa = 01:22:33:44:55:66
*               macSa = 00:00:00:00:22:22
*           Success Criteria:
*               One packet is received on port0 and port1 and one packet is 
*               dropped.
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
GT_VOID prvTgfBrgGenRateLimitMcastRegEnableRegTrafficGenerate
(
    void
)
{
    prvTgfBrgGenRateLimitMcastRegTrafficGenerate(GT_TRUE, /* enable rate limit for 
                                                             multicast registered */
                                                 GT_FALSE, /* disable rate limit for 
                                                              multicast unregistered */
                                                 0,         /* rate limit */
                                                 GT_TRUE   /* send multicast registered */
                                                 );
}

/*******************************************************************************
* prvTgfBrgGenRateLimitMcastRegDisableRegTrafficGenerate
*
* DESCRIPTION:
*           Disable Rate Limit on port port2 for multicast registered packets.
*           Create vidx 0xF with port members port0 and port1.
*           Configure MAC 01:22:33:44:55:66 on vidx 0xF 
*           Set window size to maximum and drop command to hard
*           Set rate limit to 1.
*       Generate traffic:
*           Send from port2 single tagged packet:
*               macDa = 01:22:33:44:55:66
*               macSa = 00:00:00:00:22:22
*           Success Criteria:
*               Two packets are received on port0 and port1 and no packet is 
*               dropped.
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
GT_VOID prvTgfBrgGenRateLimitMcastRegDisableRegTrafficGenerate
(
    void
)
{
    prvTgfBrgGenRateLimitMcastRegTrafficGenerate(GT_FALSE, /* disable rate limit for 
                                                              multicast registered */
                                                 GT_FALSE, /* disable rate limit for 
                                                              multicast unregistered */
                                                 0,         /* rate limit */
                                                 GT_TRUE   /* send multicast registered */
                                                 );
}

/*******************************************************************************
* prvTgfBrgGenRateLimitMcastRegEnableUnregTrafficGenerate
*
* DESCRIPTION:
*           Enable Rate Limit on port port2 for multicast registered packets.
*           Create vidx 0xF with port members port0 and port1.
*           Configure MAC 01:22:33:44:55:66 on vidx 0xF 
*           Set window size to maximum and drop command to hard
*           Set rate limit to 1.
*       Generate traffic:
*           Send from port2 single tagged packet:
*               macDa = 01:77:88:99:AA:BB
*               macSa = 00:00:00:00:22:22
*           Success Criteria:
*               Two packets are received on port0 and port1 and no packet is 
*               dropped.
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
GT_VOID prvTgfBrgGenRateLimitMcastRegEnableUnregTrafficGenerate
(
    void
)
{
    prvTgfBrgGenRateLimitMcastRegTrafficGenerate(GT_TRUE, /* enable rate limit for 
                                                             multicast registered */
                                                 GT_FALSE, /* disable rate limit for 
                                                              multicast unregistered */
                                                 0,          /* rate limit */
                                                 GT_FALSE   /* send multicast 
                                                               registered traffic */
                                                 );
}


/*******************************************************************************
* prvTgfBrgGenRateLimitMcastRegDisableUnregEnableRegTrafficGenerate
*
* DESCRIPTION:
*           Disable Rate Limit on port port2 for multicast registered packets.
*           Enable Rate Limit on port port2 for multicast unregistered packets.
*           Create vidx 0xF with port members port0 and port1.
*           Configure MAC 01:22:33:44:55:66 on vidx 0xF 
*           Set window size to maximum and drop command to hard
*           Set rate limit to 1.
*       Generate traffic:
*           Send from port2 single tagged packet:
*               macDa = 01:22:33:44:55:66
*               macSa = 00:00:00:00:22:22
*           Success Criteria:
*               Both packets are received on port0 and port1 and no packet is 
*               dropped.
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
GT_VOID prvTgfBrgGenRateLimitMcastRegDisableUnregEnableRegTrafficGenerate
(
    void
)
{
    prvTgfBrgGenRateLimitMcastRegTrafficGenerate(GT_FALSE, /* disable rate limit for 
                                                              multicast registered */
                                                 GT_TRUE, /* enable rate limit for 
                                                             multicast unregistered */
                                                 0,         /* rate limit */
                                                 GT_TRUE   /* send multicast registered */
                                                 );
}

/*******************************************************************************
* prvTgfBrgGenRateLimitMcastRegConfigurationRestore
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
GT_VOID prvTgfBrgGenRateLimitMcastRegConfigurationRestore
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
    rc = prvTgfBrgDefVlanEntryInvalidate(PRV_TGF_VLANID_CNS);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d, %d",
                                 prvTgfDevNum, PRV_TGF_VLANID_CNS);
}

