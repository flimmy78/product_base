/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfCommon.c
*
* DESCRIPTION:
*       Common helper API for enhanced UTs
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <utf/private/prvUtfHelpers.h>
#include <utf/private/prvUtfExtras.h>

#include <utf/utfMain.h>
#include <trafficEngine/tgfTrafficEngine.h>
#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>
#include <common/tgfMirror.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <common/tgfCommon.h>

/* include the AppDemo to get info about the infrastructure */
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#include <common/tgfBridgeGen.h>
#include <common/tgfPortGen.h>
#include <common/tgfVntGen.h>

/******************************************************************************\
 *              Default device\port numbers initialization                    *
\******************************************************************************/
/* ports array */
GT_U8   prvTgfPortsArray[PRV_TGF_MAX_PORTS_NUM_CNS] =
    {0, 8, 18, 23, /* 4 ports of 'default' tests */
     0, 0,  0,  0, 0, 0};

/* devices array */
GT_U8   prvTgfDevsArray[PRV_TGF_MAX_PORTS_NUM_CNS]  =
    {PRV_TGF_DEV_0_CNS, PRV_TGF_DEV_0_CNS,  PRV_TGF_DEV_0_CNS,  PRV_TGF_DEV_0_CNS,
     PRV_TGF_DEV_0_CNS, PRV_TGF_DEV_0_CNS,  PRV_TGF_DEV_0_CNS,  PRV_TGF_DEV_0_CNS,
     PRV_TGF_DEV_0_CNS, PRV_TGF_DEV_0_CNS    };

/* number of ports/devices in arrays of :prvTgfPortsArray,prvTgfDevsArray */
GT_U8   prvTgfPortsNum = PRV_TGF_DEFAULT_NUM_PORTS_IN_TEST_CNS;

/* port array size per device type */
#define PRV_TGF_DEV_TYPE_COUNT_CNS PRV_TGF_DEVICE_TYPE_COUNT_E

/* illegal value for port numbers in port's array */
#define PRV_TGF_INVALID_PORT_NUM_CNS CPSS_MAX_PORTS_NUM_CNS

/* default ports array per device type */
GT_U8   prvTgfDefPortsArray[PRV_TGF_DEV_TYPE_COUNT_CNS][PRV_TGF_MAX_PORTS_NUM_CNS] =
        {{0,  8, 18, 23, 0, 0, 0, 0, 0, 0},  /* 28 port devices */
         {0, 18, 36, 58, 0, 0, 0, 0, 0, 0}}; /* 60 port devices */

/* number of used ports in default ports array per device type */
GT_U8   prvTgfDefPortsArraySize[PRV_TGF_DEV_TYPE_COUNT_CNS] = {4, 4};

/* list of ports in devices that must be added to all vlans and MC (vidx) groups */
PRV_TGF_MEMBER_FORCE_INFO_STC  prvTgfDevPortForceArray[PRV_TGF_FORCE_MEMBERS_COUNT_CNS];
/* number of ports/devices in arrays of :prvTgfDevPortForceArray[] */
/* by default there are no members that need to forced to all vlans */
GT_U32    prvTgfDevPortForceNum = 0;

static GT_BOOL  useLastTpid = GT_FALSE;

static GT_U32   debugSleepTime = 0;

/* Tx ports maximal amount for continuous mode.
  The limitation is 7 Analyzer interfaces in source based mode. */
#define PRV_TGF_TX_PORTS_MAX_NUM_CNS 7
/*
 * typedef: struct PRV_TGF_TX_DEV_PORT_STC
 *
 * Description:
 *      continuous mode DB entry.
 *
 * Fields:
 *      devNum    - device number of Tx port
 *      portNum   - port number of Tx port
 *      unitsType - Tx units type
 *      units     - Tx units value
 *      analyzerInf - analyzer interface to restore after stop traffic
 *                  
 * Comment:
 *      
 */
typedef struct{
        GT_U8                           devNum;
        GT_U8                           portNum;
        PRV_TGF_TX_CONT_MODE_UNIT_ENT   unitsType;
        GT_U32                          units;
        PRV_TGF_MIRROR_ANALYZER_INTERFACE_STC analyzerInf;
}PRV_TGF_TX_DEV_PORT_STC;

/* mark DB entry as free */
#define  PRV_TGF_TX_MODE_FREE_DB_ENTRY(_index) U32_SET_FIELD_MAC(txDevPortContUsedBmp,(_index), 1, 0)

/* mark DB entry as allocated */
#define  PRV_TGF_TX_MODE_ALLOC_DB_ENTRY(_index) U32_SET_FIELD_MAC(txDevPortContUsedBmp,(_index), 1, 1)

/* is DB not empty */
#define PRV_TRG_TX_MODE_DB_NOT_EMPTY (txDevPortContUsedBmp)

/* the DB */
static PRV_TGF_TX_DEV_PORT_STC txDevPortArr[PRV_TGF_TX_PORTS_MAX_NUM_CNS];
static GT_U32 txDevPortContUsedBmp = 0;

/* store mirroring mode and descriptors limits */
static PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT mirrModeStore;
static GT_U32 mirrRxDescStore;
static GT_U32 mirrTxDescStore;

/* number of packets to get WS traffic. */
GT_U32 prvTgfWsBurst = 30;

/* debug flag to check rate by VNT Time Stamp */
GT_U32 prvTgfCommonVntTimeStampsDebug = 0;

static GT_STATUS prvTgfTxContModeEntryGet
(
    IN GT_U8                            devNum,
    IN GT_U8                            portNum,
    OUT PRV_TGF_TX_DEV_PORT_STC         **modePtr,
    OUT GT_U32                          *idxPtr,
    OUT GT_U32                          *freeIdxPtr
);

/******************************************************************************\
 *                             Define section                                 *
\******************************************************************************/

/* total number of counters */
#define PRV_TGF_MAX_COUNTER_NUM_CNS    35

/* max counter field's name length */
#define PRV_TGF_MAX_FIELD_NAME_LEN_CNS 25

/* packet for transmitting */
static TGF_PACKET_STC   *prvTgfPacketInfoPtr = NULL;

/* array of VFDs */
static TGF_VFD_INFO_STC *prvTgfVfdArray      = NULL;

/* number of frames */
static GT_U32            prvTgfBurstCount    = 0;

/* number of VFDs */
static GT_U32            prvTgfNumVfd        = 0;

/* sleep time in milliseconds  */
#define hugeBurstSleepTime  300

#define RESET_VALUE_CNS 0

/* default packets count for huge burst */
#define PRV_TGF_DEF_BURST_TRACE_CNS 16

/* allow to do 'sleep' after X packets sent.
when = 0 , meaning NO SLEEP needed during the burst of 'burstCount'*/
static GT_U32            prvTgfBurstSleepAfterXCount = RESET_VALUE_CNS;
/* the sleep time (in milliseconds) after X packets sent , see parameter
prvTgfBurstSleepAfterXCount */
static GT_U32            prvTgfBurstSleepTime = RESET_VALUE_CNS;

/* packets count for huge burst */
static GT_U32            prvTgfBurstTraceCount = PRV_TGF_DEF_BURST_TRACE_CNS;


/* when SA macs burst is large we need to allow the appDemo to learn the SA
   (due to controlled learning) , so we set flag to state the
   'Need sleep after X count'

   flag is 'Auto cleared'

   SET by function prvTgfCommonIncrementalSaMacSend(...) for SA learning.
   checked by sendMultiDestinationTraffic(...)
*/
static GT_BOOL prvTgfUseSleepAfterXCount = GT_FALSE;

/******************************* common payload *******************************/

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

/******************************* multi destination packet **********************************/

/*a.Send BC (FF:FF:FF:FF:FF:FF) with 100 incremental SA.*/
static TGF_PACKET_L2_STC multiDestination_prvTgfPacketL2Part_BC =
{
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff},                /* daMac */
    {0x00, 0x01, 0x11, 0x11, 0x11, 0x11}                 /* saMac */
};

/*b.Send unregistered MC (01:02:03:04:05:06) with 100 incremental SA.*/
static TGF_PACKET_L2_STC multiDestination_prvTgfPacketL2Part_MC =
{
    {0x01, 0x02, 0x03, 0x04, 0x05, 0x06},                /* daMac */
    {0x00, 0x01, 0x11, 0x11, 0x11, 0x11}                 /* saMac */
};

/*c.Send unknown unicast (00:09:99:99:99:99) with 100 incremental DA and static SA */
/*d.Send unknown unicast (00:09:99:99:99:99) with 100 static DA and incremental SA*/
static TGF_PACKET_L2_STC multiDestination_prvTgfPacketL2Part_unk_UC =
{
    {0x00, 0x09, 0x99, 0x99, 0x99, 0x99},                /* daMac */
    {0x00, 0x01, 0x11, 0x11, 0x11, 0x11}                 /* saMac */
};

static  TGF_PACKET_L2_STC*  multiDestination_prvTgfPacketL2Array[] =
{
    &multiDestination_prvTgfPacketL2Part_BC,
    &multiDestination_prvTgfPacketL2Part_MC,
    &multiDestination_prvTgfPacketL2Part_unk_UC,
    &multiDestination_prvTgfPacketL2Part_unk_UC
};

/* PARTS of packet */
static TGF_PACKET_PART_STC multiDestination_prvTgfPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &multiDestination_prvTgfPacketL2Part_BC},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketLongPayloadPart}
};

/* Length of packet */
#define MULTI_DESTINATION_PRV_TGF_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + sizeof(prvTgfPacketPayloadLongDataArr)

/* packet to send */
static TGF_PACKET_STC multiDestination_prvTgfPacketInfo =
{
    MULTI_DESTINATION_PRV_TGF_PACKET_LEN_CNS,                 /* totalLen */
    (sizeof(multiDestination_prvTgfPacketPartArray)
        / sizeof(multiDestination_prvTgfPacketPartArray[0])), /* numOfParts */
    multiDestination_prvTgfPacketPartArray                    /* partsArray */
};

static TGF_VFD_INFO_STC  multiDestination_prvTgfPacketL2Part_SA_incemental_vfdInfo=
{
    TGF_VFD_MODE_INCREMENT_E,
    1,
    0,
    6,  /* offset in the packet to override */
    {0},/* saMac */
    6,
    NULL,
    0,
    0
};

static TGF_VFD_INFO_STC  multiDestination_prvTgfPacketL2Part_DA_incemental_vfdInfo=
{
    TGF_VFD_MODE_INCREMENT_E,
    1,
    0,
    0,  /* offset in the packet to override */
    {0},/* daMac */
    6,
    NULL,
    0,
    0
};

static TGF_VFD_INFO_STC  multiDestination_prvTgfPacketL2Part_SA_static_vfdInfo=
{
    TGF_VFD_MODE_STATIC_E,
    1,
    0,
    6,  /* offset in the packet to override */
    {0},/* daMac */
    6,
    NULL,
    0,
    0,
};

/******************************************************************************\
 *                     Private function's implementation                      *
\******************************************************************************/

/*******************************************************************************
* prvTgfCountersPrint
*
* DESCRIPTION:
*       Private function to print port's counters
*
* INPUTS:
*       portCountersPtr - (pointer to) port's counters to print
*       portNum         - port number
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
static GT_VOID prvTgfCountersPrint
(
    IN const CPSS_PORT_MAC_COUNTER_SET_STC   *portCountersPtr,
    IN       GT_U8                            portNum
)
{
    static const GT_U8 cntFieldNames[PRV_TGF_MAX_COUNTER_NUM_CNS][PRV_TGF_MAX_FIELD_NAME_LEN_CNS] =
                            {"goodOctetsRcv", "badOctetsRcv", "macTransmitErr",
                             "goodPktsRcv", "badPktsRcv", "brdcPktsRcv", "mcPktsRcv",
                             "pkts64Octets", "pkts65to127Octets", "pkts128to255Octets",
                             "pkts256to511Octets", "pkts512to1023Octets", "pkts1024tomaxOoctets",
                             "goodOctetsSent", "goodPktsSent", "excessiveCollisions",
                             "mcPktsSent", "brdcPktsSent", "unrecogMacCntrRcv",
                             "fcSent", "goodFcRcv", "dropEvents", "undersizePkts",
                             "fragmentsPkts", "oversizePkts", "jabberPkts", "macRcvError",
                             "badCrc", "collisions", "lateCollisions", "badFcRcv", "ucPktsRcv",
                             "ucPktsSent", "multiplePktsSent", "deferredPktsSent"};
    GT_U32   cntHi32Bits  = 0;
    GT_U32   cntLow32Bits = 0;
    GT_U32   iter         = 0;
    GT_U32  *portCntPtr   = NULL;
    GT_BOOL  isNonZeroCnt = GT_FALSE;


    portCntPtr = (GT_U32*) portCountersPtr;

    /* print counters */
    PRV_UTF_LOG1_MAC("Port [%d] counters:\n", portNum);

    for (iter = 0; iter < PRV_TGF_MAX_COUNTER_NUM_CNS; iter++)
    {
        /* get Hi and Low 32 bits of the port counter */
        cntHi32Bits  = *(portCntPtr + 1);
        cntLow32Bits = *(portCntPtr++);

        /* print only non zero counters */
        if ((cntHi32Bits != 0) || (cntLow32Bits != 0))
        {
            /* find non zero counter */
            isNonZeroCnt = GT_TRUE;

            PRV_UTF_LOG1_MAC(" %s = ", cntFieldNames[iter]);
            if (cntHi32Bits != 0)
            {
                PRV_UTF_LOG1_MAC("%d", cntHi32Bits);
            }
            PRV_UTF_LOG1_MAC("%d\n", cntLow32Bits);
        }

        portCntPtr++;
    }

    /* check if all counters are zero */
    if (GT_FALSE == isNonZeroCnt)
    {
        PRV_UTF_LOG0_MAC(" All counters are 0\n");
    }
    PRV_UTF_LOG0_MAC("\n");
}


/******************************************************************************\
 *                              API implementation                            *
\******************************************************************************/

/*******************************************************************************
* prvTgfResetCountersEth
*
* DESCRIPTION:
*       Reset port counters and set this port in linkup mode
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfResetCountersEth
(
    IN GT_U8                devNum,
    IN GT_U8                portNum
)
{
    CPSS_INTERFACE_INFO_STC portInterface;
    GT_STATUS rc = GT_OK;

    /* map input params */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = devNum;
    portInterface.devPort.portNum = portNum;

    /* reset counters */
    rc = tgfTrafficGeneratorPortCountersEthReset(&portInterface);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: tgfTrafficGeneratorPortCountersEthReset FAILED, rc = [%d]", rc);

        return rc;
    }

    /* set port into linkup mode */
    rc = tgfTrafficGeneratorPortForceLinkUpEnableSet(&portInterface, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: tgfTrafficGeneratorPortForceLinkUpEnableSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
}

/*******************************************************************************
* prvTgfSetTxSetupEth
*
* DESCRIPTION:
*       Setting transmit parameters
*
* INPUTS:
*       devNum        - the CPU device number
*       packetInfoPtr - (pointer to) the packet info
*       burstCount    - number of frames
*       numVfd        - number of VFDs
*       vfdArray      - (pointer to) array of VFDs
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfSetTxSetupEth
(
    IN GT_U8                devNum,
    IN TGF_PACKET_STC      *packetInfoPtr,
    IN GT_U32               burstCount,
    IN GT_U32               numVfd,
    IN TGF_VFD_INFO_STC     vfdArray[]
)
{
    GT_STATUS rc = GT_OK;

    /* enable tracing */
    rc = tgfTrafficTracePacketByteSet(GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: tgfTrafficTracePacketByteSet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* set the CPU device to send traffic */
    rc = tgfTrafficGeneratorCpuDeviceSet(devNum);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: tgfTrafficGeneratorCpuDeviceSet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* register CB */
    rc = tgfTrafficTableNetRxPacketCbRegister(tgfTrafficTableRxPcktReceiveCb);
    if ((GT_OK != rc) && (GT_ALREADY_EXIST != rc))
    {
        PRV_UTF_LOG1_MAC("[TGF]: tgfTrafficTableNetRxPacketCbRegister FAILED, rc = [%d]", rc);

        return rc;
    }

    /* bind parameters into local variables */
    prvTgfPacketInfoPtr = packetInfoPtr;
    prvTgfBurstCount    = burstCount;
    prvTgfNumVfd        = numVfd;
    prvTgfVfdArray      = vfdArray;

    /* default for option that can be set by function prvTgfSetTxSetup2Eth(...) */
    prvTgfBurstSleepAfterXCount = RESET_VALUE_CNS;
    prvTgfBurstSleepTime        = RESET_VALUE_CNS;


    return (GT_ALREADY_EXIST == rc) ? GT_OK : rc;
}

/*******************************************************************************
* prvTgfSetTxSetup2Eth
*
* DESCRIPTION:
*       Setting transmit 2 parameters .
*       NOTE:
*           This function should be called after prvTgfSetTxSetupEth(..) ,
*           when need to set the parameters of this function.
*
* INPUTS:
*       sleepAfterXCount - do 'sleep' after X packets sent
*                          when = 0 , meaning NO SLEEP needed during the burst
*                          of 'burstCount'
*       sleepTime        - sleep time (in milliseconds) after X packets sent , see
*                          parameter sleepAfterXCount
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfSetTxSetup2Eth
(
    IN GT_U32               sleepAfterXCount,
    IN GT_U32               sleepTime
)
{

    prvTgfBurstSleepAfterXCount = sleepAfterXCount;
    prvTgfBurstSleepTime        = sleepTime;

    return GT_OK;
}

/*******************************************************************************
* prvTgfBurstTraceSet
*
* DESCRIPTION:
*       Set max number of packets in burst count that will be printed
*
* INPUTS:
*       packetCount - disable logging when burst is lager then packetCount
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success.
*
* COMMENTS:
*       To restore default setting call function with packetCount == 0
*
*******************************************************************************/
GT_STATUS prvTgfBurstTraceSet
(
    IN GT_U32       packetCount
)
{
    prvTgfBurstTraceCount = (0 == packetCount) ? PRV_TGF_DEF_BURST_TRACE_CNS : packetCount;

    return GT_OK;
}

/*******************************************************************************
* prvTgfStartTransmitingEth
*
* DESCRIPTION:
*       Transmit packet
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfStartTransmitingEth
(
    IN GT_U8                devNum,
    IN GT_U8                portNum
)
{
    CPSS_INTERFACE_INFO_STC  portInterface;
    GT_STATUS                rc         = GT_OK;
    GT_U32                      freeIndex;
    GT_U32                      index;
    PRV_TGF_TX_DEV_PORT_STC     *modePtr;
    GT_STATUS                   modeRc;
    GT_U32                      burstCount = prvTgfBurstCount;

    /* get DB entry  */
    modeRc = prvTgfTxContModeEntryGet(devNum, portNum, &modePtr, &index, &freeIndex);

    /* clear portInterface */
    cpssOsMemSet(&portInterface, 0, sizeof(portInterface));

    /* fill portInterface struct */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = devNum;
    portInterface.devPort.portNum = portNum;

    /* set port in loopback mode */
    rc = tgfTrafficGeneratorPortLoopbackModeEnableSet(&portInterface, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: tgfTrafficGeneratorPortLoopbackModeEnableSet FAILED, rc = [%d]", rc);

        return rc;
    }

    if (modeRc == GT_OK)
    {
        /* continuous mode is chosen */
        rc = prvTgfMirrorRxPortSet(portNum, GT_TRUE, index);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfMirrorRxPortSet FAILED, rc = [%d]", rc);
            return rc;
        }

        /* set burst count to provide WS traffic */
        burstCount = prvTgfWsBurst;
    }

    /* transmit packet */
    rc = tgfTrafficGeneratorPortTxEthTransmit(&portInterface,
                                              prvTgfPacketInfoPtr,
                                              burstCount,
                                              prvTgfNumVfd,
                                              prvTgfVfdArray,
                                              prvTgfBurstSleepAfterXCount,
                                              prvTgfBurstSleepTime,
                                              prvTgfBurstTraceCount);

    /* do not disable loopback for continuous */
    if (modeRc != GT_OK)
    {
        /* disable loopback mode on port */
        (void) tgfTrafficGeneratorPortLoopbackModeEnableSet(&portInterface, GT_FALSE);
    }

    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: tgfTrafficGeneratorPortTxEthTransmit FAILED, rc = [%d]", rc);
    }

    return rc;
}
/*******************************************************************************
* prvTgfStopTransmitingEth
*
* DESCRIPTION:
*       Stop transmit packets
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfStopTransmitingEth
(
    IN GT_U8                devNum,
    IN GT_U8                portNum
)
{
    CPSS_INTERFACE_INFO_STC  portInterface;
    GT_STATUS                rc         = GT_OK;
    GT_U32                      freeIndex;
    GT_U32                      index;
    PRV_TGF_TX_DEV_PORT_STC     *modePtr;
    GT_STATUS                   modeRc;

    /* get DB entry  */
    modeRc = prvTgfTxContModeEntryGet(devNum, portNum, &modePtr, &index, &freeIndex);

    if (modeRc != GT_OK)
    {
        /* port in burst mode. Nothing to do. */
        return GT_OK;
    }


    /* clear portInterface */
    cpssOsMemSet(&portInterface, 0, sizeof(portInterface));

    /* fill portInterface struct */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = devNum;
    portInterface.devPort.portNum = portNum;

    /* disable loopback mode on port */
    (void) tgfTrafficGeneratorPortLoopbackModeEnableSet(&portInterface, GT_FALSE);

    /* disable mirroring */
    rc = prvTgfMirrorRxPortSet(portNum, GT_FALSE, index);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfMirrorRxPortSet FAILED, rc = [%d]", rc);
        return rc;
    }

    return GT_OK;
}
/*******************************************************************************
* prvTgfTxContModeEntryGet
*
* DESCRIPTION:
*       Get entry from transmit continuous mode DB
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       modePtr    - (pointer to) mode entry
*       idxPtr     - (pointer to) entry index
*       freeIdxPtr - (pointer to) free entry index
*
* RETURNS:
*       GT_OK        - on success.
*       GT_NO_SUCH   - on entry not exist.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfTxContModeEntryGet
(
    IN GT_U8                            devNum,
    IN GT_U8                            portNum,
    OUT PRV_TGF_TX_DEV_PORT_STC         **modePtr,
    OUT GT_U32                          *idxPtr,
    OUT GT_U32                          *freeIdxPtr
)
{
    GT_U32 freeIndex = PRV_TGF_TX_PORTS_MAX_NUM_CNS; /* index of first free entry */
    GT_U32 ii; /* iterator */
    GT_BOOL found = GT_FALSE; /* is entry found */
    GT_U32 foundIdx = 0; /* index of found entry */

    /* check bitmap */
    if (txDevPortContUsedBmp == 0)
    {
        *freeIdxPtr = 0;
        *modePtr = &(txDevPortArr[0]);
        return GT_NO_SUCH;
    }     

    /* find index in the DB */
    for (ii = 0; ii < PRV_TGF_TX_PORTS_MAX_NUM_CNS; ii++)
    {
        if (txDevPortContUsedBmp & (1 << ii))
        {
            /* check devNum and portNum */
            if ((txDevPortArr[ii].devNum == devNum) &&
                (txDevPortArr[ii].portNum == portNum))
            {
                /* entry found */
                found = GT_TRUE;
                foundIdx = ii;
            }
        }
        else
        {
            freeIndex = ii;
        }
    }

   *freeIdxPtr = freeIndex;
   if (freeIndex != PRV_TGF_TX_PORTS_MAX_NUM_CNS)
   {
       *modePtr = &(txDevPortArr[freeIndex]);
   }

   if (found == GT_FALSE)
       return GT_NO_SUCH;

   *modePtr = &(txDevPortArr[foundIdx]);
   *idxPtr = foundIdx;

   return GT_OK;
}

/*******************************************************************************
* prvTgfTxModeSetupEth
*
* DESCRIPTION:
*       Transmit mode configuration
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS prvTgfTxModeSetupEth
(
    IN GT_U8                            devNum,
    IN GT_U8                            portNum,
    IN PRV_TGF_TX_MODE_ENT              mode,
    IN PRV_TGF_TX_CONT_MODE_UNIT_ENT    unitsType,
    IN GT_U32                           units
)
{
    GT_U32                      freeIndex;
    GT_U32                      index;
    PRV_TGF_TX_DEV_PORT_STC     *modePtr;
    PRV_TGF_MIRROR_ANALYZER_INTERFACE_STC   analyzerInf;
    GT_STATUS                   rc;

    /* get DB entry  */
    rc = prvTgfTxContModeEntryGet(devNum, portNum, &modePtr, &index, &freeIndex);

    switch (mode)
    {
        case PRV_TGF_TX_MODE_SINGLE_BURST_E:
            if (rc != GT_OK)
            {
                /* port already in burst mode */
                return GT_OK;
            }

            /* mark entry as free*/
            PRV_TGF_TX_MODE_FREE_DB_ENTRY(index);

            if (!PRV_TRG_TX_MODE_DB_NOT_EMPTY)
            {
                /* restore mirroring mode */
                rc = prvTgfMirrorToAnalyzerForwardingModeSet(mirrModeStore);
                if (rc != GT_OK)
                {
                    PRV_UTF_LOG1_MAC("[TGF]: prvTgfMirrorToAnalyzerForwardingModeSet FAILED, rc = [%d]", rc);
                    return rc;
                }

                /* restore Rx descriptors limits */
                rc = prvTgfPortTxSniffedPcktDescrLimitSet(devNum, 
                                                          mirrRxDescStore, 
                                                          mirrTxDescStore);
                if (rc != GT_OK)
                {
                    PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortTxSniffedPcktDescrLimitSet FAILED, rc = [%d]", rc);
                    return rc;
                }

                /* restore Rx buffers limits */
                rc = prvTgfPortTxSniffedBuffersLimitSet(devNum, 
                                                        mirrRxDescStore, 
                                                        mirrTxDescStore);
                if ((rc != GT_OK) && (rc != GT_NOT_APPLICABLE_DEVICE))
                {
                    PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortTxSniffedBuffersLimitSet FAILED, rc = [%d]", rc);
                    return rc;
                }

            }

            /* restore analyzer */
            rc = prvTgfMirrorAnalyzerInterfaceSet(index,&(modePtr->analyzerInf));
            if (rc != GT_OK)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfMirrorAnalyzerInterfaceSet FAILED, rc = [%d]", rc);
                return rc;
            }

            break;
        case PRV_TGF_TX_MODE_CONTINUOUS_E:
            if (rc == GT_OK)
            {
                /* port is already in the mode.
                  update DB entry. */
                modePtr->units = units;
                modePtr->unitsType = unitsType;
                return GT_OK;
            }

            if (freeIndex == PRV_TGF_TX_PORTS_MAX_NUM_CNS)
            {
                /* there is no place for new port */
                return GT_FULL;
            }

            if (!PRV_TRG_TX_MODE_DB_NOT_EMPTY)
            {
                /* this is first port in continuous mode.
                   configure mirroring.                    */
                /* store mode */
                rc = prvTgfMirrorToAnalyzerForwardingModeGet(devNum, &mirrModeStore);
                if (rc != GT_OK)
                {
                    PRV_UTF_LOG1_MAC("[TGF]: prvTgfMirrorToAnalyzerForwardingModeGet FAILED, rc = [%d]", rc);
                    return rc;
                }

                /* set source based mode */
                rc = prvTgfMirrorToAnalyzerForwardingModeSet(PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E);
                if (rc != GT_OK)
                {
                    PRV_UTF_LOG1_MAC("[TGF]: prvTgfMirrorToAnalyzerForwardingModeSet FAILED, rc = [%d]", rc);
                    return rc;
                }

                /* store Tx and Rx descriptors limits */
                rc = prvTgfPortTxSniffedPcktDescrLimitGet(devNum, &mirrRxDescStore,&mirrTxDescStore);
                if (rc != GT_OK)
                {
                    PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortTxSniffedPcktDescrLimitGet FAILED, rc = [%d]", rc);
                    return rc;
                }

                /* set Rx descriptors limits to be WS burst * number of mirrored ports */
                rc = prvTgfPortTxSniffedPcktDescrLimitSet(devNum, 
                                                          prvTgfWsBurst * PRV_TGF_TX_PORTS_MAX_NUM_CNS, 
                                                          mirrTxDescStore);
                if (rc != GT_OK)
                {
                    PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortTxSniffedPcktDescrLimitSet FAILED, rc = [%d]", rc);
                    return rc;
                }

                /* set Rx buffers limits to be WS burst * number of mirrored ports */
                rc = prvTgfPortTxSniffedBuffersLimitSet(devNum, 
                                                        prvTgfWsBurst * PRV_TGF_TX_PORTS_MAX_NUM_CNS, 
                                                        mirrTxDescStore);
                if ((rc != GT_OK) && (rc != GT_NOT_APPLICABLE_DEVICE))
                {
                    PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortTxSniffedBuffersLimitSet FAILED, rc = [%d]", rc);
                    return rc;
                }
            }

            /* mark entry as allocated */
            PRV_TGF_TX_MODE_ALLOC_DB_ENTRY(freeIndex);
            modePtr->devNum = devNum;
            modePtr->portNum = portNum;
            modePtr->units = units;
            modePtr->unitsType = unitsType;

            /* store analyzer interface */
            rc = prvTgfMirrorAnalyzerInterfaceGet(devNum, freeIndex, &(modePtr->analyzerInf));
            if (rc != GT_OK)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfMirrorAnalyzerInterfaceGet FAILED, rc = [%d]", rc);
                return rc;
            }

            analyzerInf.interface.type = CPSS_INTERFACE_PORT_E;
            analyzerInf.interface.devPort.devNum = devNum;
            analyzerInf.interface.devPort.portNum = portNum;

            /* set analyzer to be Tx port */
            rc = prvTgfMirrorAnalyzerInterfaceSet(freeIndex,&analyzerInf);
            if (rc != GT_OK)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfMirrorAnalyzerInterfaceSet FAILED, rc = [%d]", rc);
                return rc;
            }
            break;
        default: return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfReadPortCountersEth
*
* DESCRIPTION:
*       Read port's counters
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enablePrint - enable\disable counters print
*
* OUTPUTS:
*       portCountersPtr - (pointer to) received counters values
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_FAIL      - if test doesn't pass comparing.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfReadPortCountersEth
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          portNum,
    IN  GT_BOOL                        enablePrint,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC *portCountersPtr
)
{
    CPSS_INTERFACE_INFO_STC         portInterface;
    GT_STATUS                       rc = GT_OK;


    /* clear portInterface */
    cpssOsMemSet(&portInterface, 0, sizeof(portInterface));

    /* fill portInterface struct */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = devNum;
    portInterface.devPort.portNum = portNum;

    /* read counters */
    rc = tgfTrafficGeneratorPortCountersEthRead(&portInterface, portCountersPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: tgfTrafficGeneratorPortCountersEthRead FAILED, rc = [%d]", rc);

        return rc;
    }

    /* print counters */
    if (GT_TRUE == enablePrint)
    {
        prvTgfCountersPrint(portCountersPtr, portNum);
    }

    return rc;
}

/*******************************************************************************
* prvTgfPacketSizeGet
*
* DESCRIPTION:
*       Calculate packet size
*
* INPUTS:
*       partsArray - packet part's array
*       partsCount - number of parts
*
* OUTPUTS:
*       packetSize - (pointer to) packet size
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_FAIL      - if test doesn't pass comparing.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPacketSizeGet
(
    IN  TGF_PACKET_PART_STC  partsArray[],
    IN  GT_U32               partsCount,
    OUT GT_U32              *packetSizePtr
)
{
    GT_U32      packetIter = 0;
    GT_U32      packetSize = 0;


    CPSS_NULL_PTR_CHECK_MAC(partsArray);
    CPSS_NULL_PTR_CHECK_MAC(packetSizePtr);

    /* calc packet size */
    packetSize = 0;
    for (packetIter = 0; packetIter < partsCount; packetIter++)
    {
        switch (partsArray[packetIter].type)
        {
            case TGF_PACKET_PART_L2_E:
                packetSize += TGF_L2_HEADER_SIZE_CNS;
                break;

            case TGF_PACKET_PART_VLAN_TAG_E:
                packetSize += TGF_VLAN_TAG_SIZE_CNS;
                break;

            case TGF_PACKET_PART_ETHERTYPE_E:
                packetSize += TGF_ETHERTYPE_SIZE_CNS;
                break;

            case TGF_PACKET_PART_MPLS_E:
                packetSize += TGF_MPLS_HEADER_SIZE_CNS;
                break;

            case TGF_PACKET_PART_IPV4_E:
                packetSize += TGF_IPV4_HEADER_SIZE_CNS;
                break;

            case TGF_PACKET_PART_IPV6_E:
                packetSize += TGF_IPV6_HEADER_SIZE_CNS;
                break;

            case TGF_PACKET_PART_TCP_E:
                packetSize += TGF_TCP_HEADER_SIZE_CNS;
                break;

            case TGF_PACKET_PART_UDP_E:
                packetSize += TGF_UDP_HEADER_SIZE_CNS;
                break;

            case TGF_PACKET_PART_ARP_E:
                packetSize += TGF_ARP_HEADER_SIZE_CNS;
                break;

            case TGF_PACKET_PART_ICMP_E:
                packetSize += TGF_ICMP_HEADER_SIZE_CNS;
                break;

            case TGF_PACKET_PART_WILDCARD_E:
                packetSize += ((TGF_PACKET_PAYLOAD_STC*) partsArray[packetIter].partPtr)->dataLength;
                break;

            case TGF_PACKET_PART_PAYLOAD_E:
                packetSize += ((TGF_PACKET_WILDCARD_STC*) partsArray[packetIter].partPtr)->numOfBytes;
                break;

            default:
                return GT_BAD_PARAM;
        }
    }

    *packetSizePtr = packetSize;

    return GT_OK;
}

/*******************************************************************************
* prvTgfPortCapturedPacketPrint
*
* DESCRIPTION:
*       Print captured packet on specific port
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPortCapturedPacketPrint
(
    IN GT_U8                devNum,
    IN GT_U8                portNum
)
{
    GT_STATUS       rc, rc1  = GT_OK;
    GT_U8           dev      = 0;
    GT_U8           queue    = 0;
    GT_BOOL         getFirst = GT_TRUE;
    GT_U32          buffLen  = TGF_RX_BUFFER_MAX_SIZE_CNS;
    GT_U32          packetActualLength = 0;
    TGF_NET_DSA_STC rxParam = {0};
    GT_U8           packetBuff[TGF_RX_BUFFER_MAX_SIZE_CNS] = {0};


    /* disable packet trace */
    rc = tgfTrafficTracePacketByteSet(GT_FALSE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: tgfTrafficTracePacketByteSet FAILED, rc = [%d]", rc);
    }

    while(1)
    {
        /* get entry from rxNetworkIf table */
        rc = tgfTrafficGeneratorRxInCpuGet(TGF_PACKET_TYPE_CAPTURE_E,
                                           getFirst, GT_TRUE, packetBuff,
                                           &buffLen, &packetActualLength,
                                           &dev, &queue, &rxParam);
        if (GT_OK != rc)
        {
            break;
        }

        /* get the next entry */
        getFirst = GT_FALSE;

        /* check the DSA tag */
        if(GT_FALSE == rxParam.dsaCmdIsToCpu || GT_TRUE == rxParam.srcIsTrunk ||
           rxParam.devNum != devNum || rxParam.portNum != portNum)
        {
            /* go to the next packet */
            continue;
        }

        /* enable packet trace */
        rc = tgfTrafficTracePacketByteSet(GT_TRUE);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: tgfTrafficTracePacketByteSet FAILED, rc = [%d]", rc);
        }

        /* trace packet */
        rc = tgfTrafficTracePacket(packetBuff, packetActualLength, GT_TRUE);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: tgfTrafficTracePacket FAILED, rc = [%d]", rc);

            break;
        }

        /* disable packet trace */
        rc = tgfTrafficTracePacketByteSet(GT_FALSE);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: tgfTrafficTracePacketByteSet FAILED, rc = [%d]", rc);
        }
    }

    /* enable packet trace */
    rc1 = tgfTrafficTracePacketByteSet(GT_TRUE);
    if (GT_OK != rc1)
    {
        PRV_UTF_LOG1_MAC("[TGF]: tgfTrafficTracePacketByteSet FAILED, rc = [%d]", rc1);
    }

    return ((GT_NO_MORE == rc || GT_NOT_FOUND == rc) && (GT_OK == rc1)) ? GT_OK : rc;
}

/*******************************************************************************
* prvTgfEthCountersCheck
*
* DESCRIPTION:
*       Read and check port's counters
*
* INPUTS:
*       devNum         - device number
*       portNum        - port number
*       packetsCountRx - Expected number of Rx packets
*       packetsCountTx - Expected number of Tx packets
*       packetSize     - Size of packets
*       burstCount     - Number of packets with the same size
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfEthCountersCheck
(
    IN GT_U8  devNum,
    IN GT_U8  portNum,
    IN GT_U8  packetsCountRx,
    IN GT_U8  packetsCountTx,
    IN GT_U32 packetSize,
    IN GT_U32 burstCount
)
{
    CPSS_PORT_MAC_COUNTER_SET_STC portCntrs;
    CPSS_PORT_MAC_COUNTER_SET_STC expectedCntrs;
    GT_BOOL                       isCountersOk;
    GT_STATUS                     rc = GT_OK;

    /* read counters */
    rc = prvTgfReadPortCountersEth(devNum, portNum, GT_TRUE, &portCntrs);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfReadPortCountersEth");

    /* calculate expected numder of pakets */
    cpssOsMemSet(&expectedCntrs, 0, sizeof(expectedCntrs));
    expectedCntrs.goodPktsSent.l[0]   = packetsCountTx * burstCount;
    expectedCntrs.goodPktsRcv.l[0]    = packetsCountRx * burstCount;
    expectedCntrs.goodOctetsSent.l[0] = (packetSize + TGF_CRC_LEN_CNS) * packetsCountTx * burstCount;
    expectedCntrs.goodOctetsRcv.l[0]  = (packetSize + TGF_CRC_LEN_CNS) * packetsCountRx * burstCount;

    /* check Tx and Rx counters */
    isCountersOk =
        portCntrs.goodPktsSent.l[0]   == expectedCntrs.goodPktsSent.l[0] &&
        portCntrs.goodPktsRcv.l[0]    == expectedCntrs.goodPktsRcv.l[0] &&
        portCntrs.goodOctetsSent.l[0] == expectedCntrs.goodOctetsSent.l[0] &&
        portCntrs.goodOctetsRcv.l[0]  == expectedCntrs.goodOctetsRcv.l[0];

    /* print expected values if bug */
    if (GT_TRUE != isCountersOk) {
        PRV_UTF_LOG0_MAC("Expected values:\n");
        PRV_UTF_LOG1_MAC(" goodPktsSent = %d\n", expectedCntrs.goodPktsSent.l[0]);
        PRV_UTF_LOG1_MAC(" goodPktsRcv = %d\n", expectedCntrs.goodPktsRcv.l[0]);
        PRV_UTF_LOG1_MAC(" goodOctetsSent = %d\n", expectedCntrs.goodOctetsSent.l[0]);
        PRV_UTF_LOG1_MAC(" goodOctetsRcv = %d\n", expectedCntrs.goodOctetsRcv.l[0]);
    }

    return GT_TRUE != isCountersOk ? GT_FAIL : rc;
}

/*******************************************************************************
* prvTgfEthCountersReset
*
* DESCRIPTION:
*       Reset all Ethernet port's counters
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong interface
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfEthCountersReset
(
    IN GT_U8 devNum
)
{
    GT_U32    portIter;
    GT_STATUS rc, rc1 = GT_OK;

    for (portIter = 0; portIter < PRV_TGF_PORTS_NUM_CNS; portIter++)
    {
        /* reset ethernet counters */
        rc = prvTgfResetCountersEth(devNum, prvTgfPortsArray[portIter]);
        PRV_UTF_VERIFY_RC1(rc, "prvTgfResetCountersEth");
    }

    /* Clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    PRV_UTF_VERIFY_RC1(rc, "tgfTrafficTableRxPcktTblClear");

    return rc1;
}

/*******************************************************************************
* sendMultiDestinationTraffic
*
* DESCRIPTION:
*       Function sends packets.
*
* INPUTS:
*       devNum       - the device to send traffic from.
*       portNum      - the port to send traffic from.
*       burstNum     - number of packets to send in the burst
*       trace        - enable\disable packet tracing
*       packetInfoPtr - (pointer to) the packet info
*       vfdNum        - VFD number
*       vfdArray      - VFD array to override packet info
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
static GT_VOID sendMultiDestinationTraffic
(
    IN  GT_U8               devNum,
    IN  GT_U8               portNum,
    IN  GT_U32              burstNum,
    IN  GT_BOOL             trace,
    IN TGF_PACKET_STC       *packetInfoPtr,
    IN GT_U8                vfdNum,
    IN TGF_VFD_INFO_STC     vfdArray[]
)
{
    GT_STATUS   rc;

    /* setup Packet */
    rc = prvTgfSetTxSetupEth(devNum, packetInfoPtr, burstNum, vfdNum, vfdArray);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, rc, "ERROR of SetTxSetup: devNum[%d],portNum[%d],vfdNum[%d]",
                                 devNum, burstNum, vfdNum, 0);


    if(prvTgfUseSleepAfterXCount == GT_TRUE)
    {
        /* we do 'Controlled learning' , so we need to allow AppDemo to process
           the AU messages , so do 'sleep' after x count of frames that so SA
           learning */

        if(debugSleepTime)
        {
            /* so every 1K/2 of packets sent , we do sleep */
            prvTgfSetTxSetup2Eth(_1K/2 , debugSleepTime);
        }
        else
        {
            /* so every 1K/2 of packets sent , we do sleep of 1/2 seconds */
            prvTgfSetTxSetup2Eth(_1K/2 , hugeBurstSleepTime);
        }

        /* 'Auto clear' the flag */
        prvTgfUseSleepAfterXCount = GT_FALSE;
    }

    /* set tracing */
    rc = tgfTrafficTracePacketByteSet(trace);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "ERROR of tgfTrafficTracePacketByteSet: devNum[%d]", devNum);

    /* send Packet from requested port  */
    rc = prvTgfStartTransmitingEth(devNum, portNum);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: devNum[%d],portNum[%d]",
                                 devNum, portNum);

    /* enable tracing */
    rc = tgfTrafficTracePacketByteSet(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "ERROR of tgfTrafficTracePacketByteSet: devNum[%d]", devNum);
}

/*******************************************************************************
* prvTgfCommonMultiDestinationTrafficTypeSend
*
*
* DESCRIPTION:
*   like prvTgfCommonMultiDestinationTrafficSend it :
*   Send multi-destination traffic , meaning from requested port:
*       Send BC (FF:FF:FF:FF:FF:FF) with burstNum incremental SA.
*       Send unregistered MC (01:02:03:04:05:06) with burstNum incremental SA.
*       Send unknown unicast (00:09:99:99:99:99) with burstNum incremental DA and static SA
*       Send unknown unicast (00:09:99:99:99:99) with burstNum static DA and incremental SA
*
*   BUT the packet itself (beside the SA,DA) is given by the caller.

*
* INPUTS:
*       devNum       - the device to send traffic from.
*       portNum      - the port to send traffic from.
*       burstNum     - number of packets to send in the burst
*       trace        - enable\disable packet tracing
*       packetPtr    - pointer to the packet
*       type         - multi-destination type of sending
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void prvTgfCommonMultiDestinationTrafficTypeSend
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U32  burstNum,
    IN  GT_BOOL trace,
    IN  TGF_PACKET_STC *packetPtr,
    IN  PRV_TGF_MULTI_DESTINATION_TYPE_ENT  type
)
{
    TGF_VFD_INFO_STC        vfdInfo;
    TGF_PACKET_PART_STC     *l2PartPtr;
    TGF_PACKET_L2_STC       *origL2PartPtr;/* pointer to original L2 part */

    /* use different 'etherType for vlan tagging' instead of 0x8100 */
    /* this will make our packet that has '0x8100' to be recognized as 'untagged'
      by the 'traffic generator' and also the CPSS will not override the ethertype
      of tag0 with the DSA tag */
    tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_NON_VALID_TAG_CNS);

    utfPrintKeepAlive();

    l2PartPtr = &packetPtr->partsArray[0];
    /* save pointer to original L2 part */
    origL2PartPtr = l2PartPtr->partPtr;

    if(type == PRV_TGF_MULTI_DESTINATION_TYPE_ALL_E ||
       type == PRV_TGF_MULTI_DESTINATION_TYPE_BC_E )
    {
        /*a.        Send BC (FF:FF:FF:FF:FF:FF) with burstNum incremental SA.*/
        l2PartPtr->partPtr = multiDestination_prvTgfPacketL2Array[0];
        vfdInfo = multiDestination_prvTgfPacketL2Part_SA_incemental_vfdInfo;
        cpssOsMemCpy(vfdInfo.patternPtr, multiDestination_prvTgfPacketL2Part_BC.saMac, vfdInfo.cycleCount);

        /* send packets */
        sendMultiDestinationTraffic(devNum , portNum , burstNum , trace, packetPtr, 1, &vfdInfo);
    }


    if(type == PRV_TGF_MULTI_DESTINATION_TYPE_ALL_E ||
       type == PRV_TGF_MULTI_DESTINATION_TYPE_MC_E )
    {
        /*b.        Send unregistered MC (01:02:03:04:05:06) with burstNum incremental SA. */
        l2PartPtr->partPtr = multiDestination_prvTgfPacketL2Array[1];
        vfdInfo = multiDestination_prvTgfPacketL2Part_SA_incemental_vfdInfo;
        cpssOsMemCpy(vfdInfo.patternPtr, multiDestination_prvTgfPacketL2Part_MC.saMac, vfdInfo.cycleCount);

        /* send packets */
        sendMultiDestinationTraffic(devNum , portNum , burstNum , trace, packetPtr, 1, &vfdInfo);
    }

    if(type == PRV_TGF_MULTI_DESTINATION_TYPE_ALL_E ||
       type == PRV_TGF_MULTI_DESTINATION_TYPE_UNKNOWN_UC_DA_INCREMENT_E )
    {
        /*c.        Send unknown unicast (00:09:99:99:99:99) with burstNum incremental DA and static SA */
        l2PartPtr->partPtr = multiDestination_prvTgfPacketL2Array[2];
        vfdInfo = multiDestination_prvTgfPacketL2Part_DA_incemental_vfdInfo;
        cpssOsMemCpy(vfdInfo.patternPtr, multiDestination_prvTgfPacketL2Part_unk_UC.daMac, vfdInfo.cycleCount);

        /* send packets */
        sendMultiDestinationTraffic(devNum , portNum , burstNum , trace, packetPtr, 1, &vfdInfo);
    }

    if(type == PRV_TGF_MULTI_DESTINATION_TYPE_ALL_E ||
       type == PRV_TGF_MULTI_DESTINATION_TYPE_UNKNOWN_UC_SA_INCREMENT_E )
    {
        /*d.        Send unknown unicast (00:09:99:99:99:99) with burstNum static DA and incremental SA  */
        l2PartPtr->partPtr = multiDestination_prvTgfPacketL2Array[3];
        vfdInfo = multiDestination_prvTgfPacketL2Part_SA_incemental_vfdInfo;
        cpssOsMemCpy(vfdInfo.patternPtr, multiDestination_prvTgfPacketL2Part_unk_UC.saMac, vfdInfo.cycleCount);

        /* send packets */
        sendMultiDestinationTraffic(devNum , portNum , burstNum , trace, packetPtr, 1, &vfdInfo);
    }

    /* restore pointer to original L2 part */
    l2PartPtr->partPtr = origL2PartPtr;
    /* restore default ethertype */
    tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_8100_VLAN_TAG_CNS);
}

/*******************************************************************************
* prvTgfCommonMultiDestinationTrafficCounterCheck
*
* DESCRIPTION:
*   check that traffic counter match the needed value
*
* INPUTS:
*       devNum - device number
*       portNum - port number
*       expectedCounterValue - expected counter value (counter of goodPktsSent.l[0])
*
* OUTPUTS:
*       devNum - device number
*       portNum - port number
*       expectedCounterValue - expected counter value (counter of goodPktsSent.l[0])
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void prvTgfCommonMultiDestinationTrafficCounterCheck
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U32   expectedCounterValue
)
{
    GT_STATUS rc;
    CPSS_PORT_MAC_COUNTER_SET_STC  counters;

    rc = prvTgfReadPortCountersEth(devNum, portNum, GT_TRUE, &counters);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: devNum[%d],portNum[%d]",
                                 devNum, portNum);

    UTF_VERIFY_EQUAL2_STRING_MAC(expectedCounterValue,
                                 counters.goodPktsSent.l[0],
                                 "expected counter [%d] but got [%d]",
                                 expectedCounterValue,counters.goodPktsSent.l[0]);
    return ;
}

/*******************************************************************************
* prvTgfCommonIncrementalSaMacSend
*
* DESCRIPTION:
*   Send packets with incremental SA mac address (that start from specified mac
*   address)
*   -- this used for 'mac incremental learning'
*
* INPUTS:
*       devNum       - the device to send traffic from.
*       portNum      - the port to send traffic from.
*       saMacPtr     - (pointer to) the base mac address (of source mac)
*       burstNum     - number of packets to send in the burst
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
void prvTgfCommonIncrementalSaMacSend
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  TGF_MAC_ADDR    *saMacPtr,
    IN  GT_U32  burstNum
)
{
    TGF_VFD_INFO_STC        vfdInfo;

    utfPrintKeepAlive();

    /* Send unknown unicast (00:09:99:99:99:99) with burstNum static DA and incremental SA  */
    multiDestination_prvTgfPacketPartArray[0].partPtr = multiDestination_prvTgfPacketL2Array[3];
    vfdInfo = multiDestination_prvTgfPacketL2Part_SA_incemental_vfdInfo;
    cpssOsMemCpy(vfdInfo.patternPtr, *saMacPtr, vfdInfo.cycleCount);

    /* when burst is large we need to allow the appDemo to learn the SA
       (due to controlled learning) , so we set flag to state the
       'Need sleep after X count'

       flag is 'Auto cleared'
    */
    prvTgfUseSleepAfterXCount = GT_TRUE;
    /* send packets */
    sendMultiDestinationTraffic(devNum , portNum , burstNum ,GT_TRUE, &multiDestination_prvTgfPacketInfo, 1, &vfdInfo);
}

/*******************************************************************************
* prvTgfCommonMultiDestinationTrafficSend
*
* DESCRIPTION:
*   Send packets with incremental SA mac address .
*   this start from mac address of:    [00:00:port:port:port:port]
*   For example :
*   for port 0x22     --> [00:00:22:22:22:22]
*   and for port 0x6  --> [00:00:06:06:06:06]
*
*   -- this used for 'mac incremental learning'
*
* INPUTS:
*       devNum       - the device to send traffic from.
*       portNum      - the port to send traffic from.
*       burstNum     - number of packets to send in the burst
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
void prvTgfCommonIncrementalSaMacByPortNumSend
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U32  burstNum
)
{
    TGF_MAC_ADDR    saMac;

    /* build the source mac address */
    saMac[0] = 0;
    saMac[1] = 0;
    saMac[2] = portNum;
    saMac[3] = portNum;
    saMac[4] = portNum;
    saMac[5] = portNum;

    /* call to send by staring from specific mac address */
    prvTgfCommonIncrementalSaMacSend(devNum,portNum,&saMac,burstNum);
}

/*******************************************************************************
* prvTgfCommonIncrementalDaMacSend
*
* DESCRIPTION:
*   Send packets with incremental DA mac address (that start from specified mac
*   address) and static SA mac address
*   -- this used for checking the already learned mac addresses
*
* INPUTS:
*       devNum       - the device to send traffic from.
*       portNum      - the port to send traffic from.
*       saMacPtr     - (pointer to) the SA mac address
*       daMacPtr     - (pointer to) the DA base mac address
*       burstNum     - number of packets to send in the burst
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
void prvTgfCommonIncrementalDaMacSend
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  TGF_MAC_ADDR    *saMacPtr,
    IN  TGF_MAC_ADDR    *daMacPtr,
    IN  GT_U32  burstNum
)
{
    TGF_VFD_INFO_STC        vfdInfo[2];

    utfPrintKeepAlive();

    /*  Send unicast with incremental DA and static SA */
    multiDestination_prvTgfPacketPartArray[0].partPtr = multiDestination_prvTgfPacketL2Array[2];
    vfdInfo[0] = multiDestination_prvTgfPacketL2Part_DA_incemental_vfdInfo;
    cpssOsMemCpy(vfdInfo[0].patternPtr, *daMacPtr, vfdInfo[0].cycleCount);
    vfdInfo[1] = multiDestination_prvTgfPacketL2Part_SA_static_vfdInfo;
    cpssOsMemCpy(vfdInfo[1].patternPtr, *saMacPtr, vfdInfo[1].cycleCount);

    /* when burst is large we need to allow the simulation to process Tx frames */
    prvTgfUseSleepAfterXCount = GT_TRUE;

    /* send packets */
    sendMultiDestinationTraffic(devNum , portNum , burstNum , GT_TRUE, &multiDestination_prvTgfPacketInfo, 2, vfdInfo);
}

/*******************************************************************************
* prvTgfCommonIncrementalDaMacByPortNumSend
*
* DESCRIPTION:
*   Send packets with incremental DA mac address (according to dstPortNum)
*   and static SA mac address (according to portNum)
*
*   DA start from mac address of:    [00:00:dstPortNum:dstPortNum:dstPortNum:dstPortNum]
*   For example :
*   for port 0x22     --> [00:00:22:22:22:22]
*   and for port 0x6  --> [00:00:06:06:06:06]
*
*   SA start from mac address of:    [00:00:portNum:portNum:portNum:portNum]
*   For example :
*   for port 0x33     --> [00:00:33:33:33:33]
*   and for port 0x8  --> [00:00:08:08:08:08]
*
*   -- this used for checking the already learned mac addresses
*
* INPUTS:
*       devNum       - the device to send traffic from.
*       portNum      - the port to send traffic from.
*                      this port number also used for SA mac address
*       dstPortNum   - the alleged port number to receive the traffic
*                      this port number used for base DA mac address
*       daMacPtr     - (pointer to) the DA base mac address
*       burstNum     - number of packets to send in the burst
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
void prvTgfCommonIncrementalDaMacByPortNumSend
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U8   dstPortNum,
    IN  GT_U32  burstNum
)
{
    TGF_MAC_ADDR    saMac;
    TGF_MAC_ADDR    daMac;

    /* build the source mac address */
    saMac[0] = 0;
    saMac[1] = 0;
    saMac[2] = portNum;
    saMac[3] = portNum;
    saMac[4] = portNum;
    saMac[5] = portNum;

    /* build the source mac address */
    daMac[0] = 0;
    daMac[1] = 0;
    daMac[2] = dstPortNum;
    daMac[3] = dstPortNum;
    daMac[4] = dstPortNum;
    daMac[5] = dstPortNum;

    /* call to send by staring from specific mac addresses */
    prvTgfCommonIncrementalDaMacSend(devNum,portNum,&saMac,&daMac,burstNum);
}

/*******************************************************************************
* prvTgfCommonDebugSleepTime
*
* DESCRIPTION:
*   set debug time (milliseconds) for sleep after every burst of 512 packets
*
* INPUTS:
*       time       - time in milliseconds
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
GT_STATUS prvTgfCommonDebugSleepTime
(
    IN  GT_U32   time
)
{
    debugSleepTime = time;
    return GT_OK;
}


/*******************************************************************************
* prvTgfCommonMultiDestinationTrafficSend
*
* DESCRIPTION:
*   Send multi-destination traffic , meaning from requested port:
*       Send BC (FF:FF:FF:FF:FF:FF) with burstNum incremental SA.
*       Send unregistered MC (01:02:03:04:05:06) with burstNum incremental SA.
*       Send unknown unicast (00:09:99:99:99:99) with burstNum incremental DA and static SA
*       Send unknown unicast (00:09:99:99:99:99) with burstNum static DA and incremental SA
*
*
* INPUTS:
*       devNum       - the device to send traffic from.
*       portNum      - the port to send traffic from.
*       burstNum     - number of packets to send in the burst
*       trace        - enable\disable packet tracing
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
void prvTgfCommonMultiDestinationTrafficSend
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U32  burstNum,
    IN  GT_BOOL trace
)
{
    prvTgfCommonMultiDestinationTrafficTypeSend(
            devNum,portNum,burstNum,trace,&multiDestination_prvTgfPacketInfo,
            PRV_TGF_MULTI_DESTINATION_TYPE_ALL_E);
}

/*******************************************************************************
* prvTgfTransmitPacketsWithCapture
*
* DESCRIPTION:
*       None
*
* INPUTS:
*       inDevNum      - ingress device number
*       inPortNum     - ingress port number
*       packetInfoPtr - (pointer to) the packet info
*       burstCount    - number of frames
*       numVfd        - number of VFDs
*       vfdArray      - (pointer to) array of VFDs
*       outDevNum     - egress device number
*       outPortNum    - egress port number
*       captureMode   - packet capture mode
*       captureOnTime - time the capture will be enabe (in milliseconds)
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
GT_STATUS prvTgfTransmitPacketsWithCapture
(
    IN GT_U8                inDevNum,
    IN GT_U8                inPortNum,
    IN TGF_PACKET_STC       *packetInfoPtr,
    IN GT_U32               burstCount,
    IN GT_U32               numVfd,
    IN TGF_VFD_INFO_STC     vfdArray[],
    IN GT_U8                outDevNum,
    IN GT_U8                outPortNum,
    IN TGF_CAPTURE_MODE_ENT captureMode,
    IN GT_U32               captureOnTime
)
{
    GT_STATUS               rc = GT_OK;
    CPSS_INTERFACE_INFO_STC portInterface;

    /* clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    PRV_UTF_VERIFY_GT_OK(rc, "tgfTrafficTableRxPcktTblClear");

    /* setup receive portInterface for capturing */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = outDevNum;
    portInterface.devPort.portNum = outPortNum;

    /* enable capture on receive port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(
            &portInterface, captureMode, GT_TRUE);
    PRV_UTF_VERIFY_GT_OK(rc, "tgftrafficgeneratorporttxethcaptureset");


    /* setup transmit params */
    rc = prvTgfSetTxSetupEth(
        inDevNum, packetInfoPtr,
        burstCount, numVfd, vfdArray);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfSetTxSetupEth");

    /* start transmitting */
    rc = prvTgfStartTransmitingEth(inDevNum, inPortNum);
    PRV_UTF_VERIFY_GT_OK(rc, "prvTgfStartTransmitingEth");

    cpssOsTimerWkAfter(captureOnTime);

    /* enable capture on receive port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(
            &portInterface, captureMode, GT_FALSE);
    PRV_UTF_VERIFY_GT_OK(rc, "tgftrafficgeneratorporttxethcaptureset");

    return rc;
}

/*******************************************************************************
* prvTgfDefPortsArraySet
*
* DESCRIPTION:
*       Set predefined number of ports and port's array
*
* INPUTS:
*       portsArray - array of ports
*       arraySize  - number of ports
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_SIZE  - wrong array size
*       GT_BAD_PTR   - wrong pointer
*
* COMMENTS:
*       Call this function each time when needs to override default ports array
*       by specific values
*
*******************************************************************************/
GT_STATUS prvTgfDefPortsArraySet
(
    IN GT_U8    portsArray[],
    IN GT_U8    arraySize
)
{
    GT_U32              portIter = 0;
    CPSS_PORTS_BMP_STC  portBmp  = {{0, 0}};


    /* check array */
    CPSS_NULL_PTR_CHECK_MAC(portsArray);

    /* check array size */
    if (arraySize > PRV_TGF_MAX_PORTS_NUM_CNS)
    {
        return GT_BAD_SIZE;
    }

    /* check ports in array */
    for (portIter = 0; portIter < arraySize; portIter++)
    {
        /* check that ports is valid */
        PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(prvTgfDevNum, portsArray[portIter]);

        /* check that ports is unique */
        if ((portBmp.ports[portsArray[portIter] >> 5]) & (1 << (portsArray[portIter] % 32)))
        {
            return GT_BAD_PARAM;
        }
        else
        {
            portBmp.ports[portsArray[portIter] >> 5] |= 1 << (portsArray[portIter] % 32);
        }
    }

    /* set number of ports */
    prvTgfPortsNum = arraySize;

    /* set ports array */
    cpssOsMemCpy(prvTgfPortsArray, portsArray, sizeof(portsArray[0]) * arraySize);

    /* set not used ports with illegal values */
    cpssOsMemSet(prvTgfPortsArray + prvTgfPortsNum, PRV_TGF_INVALID_PORT_NUM_CNS,
                 sizeof(portsArray[0]) * (PRV_TGF_MAX_PORTS_NUM_CNS - prvTgfPortsNum));

    return GT_OK;
}

/*******************************************************************************
* prvTgfPortsArrayByDevTypeSet
*
* DESCRIPTION:
*       Override default ports from prvTgfPortsArray by new ports from
*       prvTgfDefPortsArray depending on device type
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
GT_VOID prvTgfPortsArrayByDevTypeSet
(
    GT_VOID
)
{
    PRV_TGF_DEVICE_TYPE_ENT devTypeIdx = PRV_TGF_28_PORT_DEVICE_TYPE_E;

    /* calc device type index in port array */
    devTypeIdx = PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(prvTgfDevNum) ? PRV_TGF_60_PORT_DEVICE_TYPE_E :
                                                                          PRV_TGF_28_PORT_DEVICE_TYPE_E;

    /* update default number of ports */
    prvTgfPortsNum = prvTgfDefPortsArraySize[devTypeIdx];

    /* update default port array */
    cpssOsMemCpy(prvTgfPortsArray, prvTgfDefPortsArray[devTypeIdx], sizeof(prvTgfPortsArray));

    /* set not used ports with illegal values */
    cpssOsMemSet(prvTgfPortsArray + prvTgfPortsNum, PRV_TGF_INVALID_PORT_NUM_CNS,
                 sizeof(prvTgfPortsArray[0]) * (PRV_TGF_MAX_PORTS_NUM_CNS - prvTgfPortsNum));

    return;
}

/*******************************************************************************
* prvTgfPortsArrayPrint
*
* DESCRIPTION:
*       Print number of used ports and port's array prvTgfPortsArray
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
GT_VOID prvTgfPortsArrayPrint
(
    GT_VOID
)
{
    GT_U32  portIter = 0;


    /* print out number of ports */
    PRV_UTF_LOG1_MAC("Number of ports = %d\n", prvTgfPortsNum);

    /* print out port's array */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        PRV_UTF_LOG2_MAC("  prvTgfPortsArray[%d] = %d\n",
                         portIter, prvTgfPortsArray[portIter]);
    }

    return;
}

/*******************************************************************************
* prvTgfCommonMemberForceInit
*
* DESCRIPTION:
*       init the need to force members into vlan/mc groups.
*       to support cascade ports and ring ports.
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
*       called for debug from terminal , so function is not static
*******************************************************************************/
GT_STATUS prvTgfCommonMemberForceInit(
    IN PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT tagCmd
)
{
    GT_U32  ii;
    PRV_TGF_MEMBER_FORCE_INFO_STC   member;
    GT_U8   devNum;
    GT_STATUS   rc;

    devNum = prvTgfDevNum;

    member.forceToVlan = GT_TRUE;
    member.forceToVidx = GT_TRUE;
    member.vlanInfo.tagged = GT_TRUE;
    member.vlanInfo.tagCmd = tagCmd;/* settings for 'cascade ports' (and ring ports) */

    if(PRV_CPSS_IS_DEV_EXISTS_MAC(devNum) == 0)
    {
        return GT_NOT_INITIALIZED;
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        /* lion-A,B */
        member.forceToVidx = GT_FALSE;/* must not add to vidx */
    }

    if(appDemoPpConfigList[devNum].multiPortGroupRingPortsBmp.ports[0] ||
       appDemoPpConfigList[devNum].multiPortGroupRingPortsBmp.ports[1] )
    {
        /* state that the tests may look in 4 port groups for FDB entry */
        prvTgfNumOfPortGroups = 4;
    }

    for(ii = 0 ; ii < CPSS_MAX_PORTS_NUM_CNS; ii++)
    {
        if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&appDemoPpConfigList[devNum].multiPortGroupRingPortsBmp,ii))
        {
            continue;
        }

        member.member.devNum = devNum;
        member.member.portNum = (GT_U8)ii;
        rc = prvTgfCommonMemberForceInfoSet(&member);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfCommonInit
*
* DESCRIPTION:
*       Initialize TFG configuration
*
* INPUTS:
*       firstDevNum - device number of first device.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       Put all necessary initializations into this function
*
*******************************************************************************/
GT_STATUS prvTgfCommonInit
(
    GT_U8 firstDevNum
)
{
    GT_U8 i;

    /* set not used ports with illegal values */
    cpssOsMemSet(prvTgfPortsArray + prvTgfPortsNum, PRV_TGF_INVALID_PORT_NUM_CNS,
                 sizeof(prvTgfPortsArray[0]) * (PRV_TGF_MAX_PORTS_NUM_CNS - prvTgfPortsNum));

    prvTgfDevNum = firstDevNum;
    for( i = 0 ; i < PRV_TGF_MAX_PORTS_NUM_CNS ; i++ )
    {
        prvTgfDevsArray[i] = firstDevNum;
    }

    /* set port configuration according to device family */
    prvTgfPortsArrayByDevTypeSet();

    return prvTgfCommonMemberForceInit(
        PRV_TGF_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E);
}

/*******************************************************************************
* prvTgfCommonMemberForceInfoSet
*
* DESCRIPTION:
*       add member info to DB of 'members to force to vlan/mc groups'
*       in case that the member already exists {dev,port} the other parameters
*       are 'updated' according to last setting.
*
* INPUTS:
*       memberInfoPtr - (pointer to) member info
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FULL - the DB is full and no more members can be added -->
*                 need to make DB larger.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvTgfCommonMemberForceInfoSet(
    IN PRV_TGF_MEMBER_FORCE_INFO_STC    *memberInfoPtr
)
{
    GT_U32  ii;
    PRV_TGF_MEMBER_FORCE_INFO_STC *currMemPtr;

    /* look for this member in the DB */
    currMemPtr = &prvTgfDevPortForceArray[0];
    for(ii = 0 ; ii < prvTgfDevPortForceNum; ii++,currMemPtr++)
    {
        if(memberInfoPtr->member.devNum == currMemPtr->member.devNum &&
           memberInfoPtr->member.portNum == currMemPtr->member.portNum )
        {
            break;
        }
    }

    if(ii == prvTgfDevPortForceNum)
    {
        /* new member */

        if(prvTgfDevPortForceNum >= PRV_TGF_FORCE_MEMBERS_COUNT_CNS)
        {
            /* the table already full , can't add new member */
            return GT_FULL;
        }

        prvTgfDevPortForceNum++;
    }

    /* copy the member into the DB */
    *currMemPtr = *memberInfoPtr;


    return GT_OK;
}

/*******************************************************************************
* prvTgfCommonIsDeviceForce
*
* DESCRIPTION:
*       check if the device has ports that used with force configuration
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE  - the device force configuration
*       GT_FALSE - the device NOT force configuration
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL prvTgfCommonIsDeviceForce(
    IN GT_U8    devNum
)
{
    GT_U32  ii;
    PRV_TGF_MEMBER_FORCE_INFO_STC *currMemPtr;

    /* look for this member in the DB */
    currMemPtr = &prvTgfDevPortForceArray[0];
    for(ii = 0 ; ii < prvTgfDevPortForceNum; ii++,currMemPtr++)
    {
        if(devNum == currMemPtr->member.devNum)
        {
            return GT_TRUE;
        }
    }

    return GT_FALSE;
}

/*******************************************************************************
* prvTgfCommonIsUseLastTpid
*
* DESCRIPTION:
*       state that test use last TPIDs for ingress or not
*
* INPUTS:
*       enable
*           GT_TRUE  - the device  use last TPIDs for ingress
*           GT_FALSE - the device  NOT use last TPIDs for ingress
*
* OUTPUTS:
*       None
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
void prvTgfCommonUseLastTpid(
    IN GT_BOOL  enable
)
{
    useLastTpid = enable;
}

/*******************************************************************************
* prvTgfCommonIsUseLastTpid
*
* DESCRIPTION:
*       check if the test use last TPIDs for ingress
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE  - the device  use last TPIDs for ingress
*       GT_FALSE - the device  NOT use last TPIDs for ingress
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL prvTgfCommonIsUseLastTpid(
    void
)
{
    return useLastTpid;
}

/*******************************************************************************
* prvTgfGetRate
*
* DESCRIPTION:
*       Get rate in event per second.
*
* INPUTS:
*       countEnd   - events count in the end of period
*       countStart - events count in the start of period
*       timeOutMilliSec - timeout in milli seconds
*
* OUTPUTS:
*       None
*
* RETURNS:
*       rate in events per seconds
*
* COMMENTS:
*
*******************************************************************************/
static GT_U32  prvTgfGetRate(IN GT_U32 countEnd, IN GT_U32 countStart, IN GT_U32 timeOutMilliSec)
{
    GT_U32 delta; /* difference between end and start */
    GT_U32 rate;  /* result */

    delta = countEnd - countStart;
    if(delta < (0xFFFFFFFF / 1000))
    {
        rate = (1000 * delta) / timeOutMilliSec;
    }
    else if (delta < (0xFFFFFFFF / 100))
    {
        rate = ((100 * delta) / timeOutMilliSec) * 10;
    }
    else if (delta < (0xFFFFFFFF / 10))
    {
        rate = ((10 * delta) / timeOutMilliSec) * 100;
    }
    else
    {
        rate = 1000 * (delta / timeOutMilliSec);
    }
    return rate;
}

/*******************************************************************************
* prvTgfCommonVntTimeStampsRateGet
*
* DESCRIPTION:
*       Get rate of events by VNT Time Stamps.
*
* INPUTS:
*       dev                 - device number
*       startTimeStamp      - start counting time stamp
*       endTimeStamp        - end counting time stamp
*       eventCount          - events count between end and start time stamps
*
* OUTPUTS:
*       none
*
* RETURNS:
*       events rate in events per second
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 prvTgfCommonVntTimeStampsRateGet
(
    IN  GT_U8          dev,
    IN  GT_U32         startTimeStamp,
    IN  GT_U32         endTimeStamp,
    IN  GT_U32         eventCount
)
{
    GT_STATUS rc;           /* return code*/
    GT_U32    coreClock;    /* device core clock in HZ */
    double    diff;         /* difference between end and start time stamps */
    double    rate;         /* rate of events */
    GT_U32    rateOut;      /* integer value of rate */

    /* get core closk in HZ */
    rc = prvUtfDeviceCoreClockGet(dev,&coreClock);
    if (rc != GT_OK)
    {
        PRV_UTF_LOG1_MAC("prvUtfDeviceCoreClockGet: %d", dev);
        return 0xFFFFFFFF;
    }

    /* calculate difference between end and start */
    if (startTimeStamp > endTimeStamp)
    {        
       /* take care of wraparound of end time */
       diff = 0xFFFFFFFFU;
       diff = diff + 1 + endTimeStamp - startTimeStamp; 
    }
    else
       diff = endTimeStamp - startTimeStamp; 

   /* time stamp is in device core clocks.
     event rate is number of events divided by time between them.
     the time is core clock period multiplied on number of core clocks.
     rate = eventCount / time, where time = diff * (1/coreClock) */
   rate = eventCount;
   rate = (rate * coreClock) / diff;

   /* convert float point value to unsigned integer */
   rateOut = (GT_U32)rate;

   return rateOut;
}

/*******************************************************************************
* prvTgfCommonPortTxTwoUcTcRateGet
*
* DESCRIPTION:
*       Get rate of two unicast flows by egress counters.
*
* INPUTS:
*       portIdx - index of egress port
*       tc1     - traffic class of first unicast flow
*       tc2     - traffic class of second unicast flow
*       timeOut - timeout in milli seconds to calculate rate
*
* OUTPUTS:
*       outRate1Ptr - (pointer to) Tx rate of first flow
*       outRate1Ptr - (pointer to) Tx rate of second flow
*       dropRate1Ptr - (pointer to) drop rate of first flow
*       dropRate1Ptr - (pointer to) drop rate of second flow
*
* RETURNS:
*       none
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfCommonPortTxTwoUcTcRateGet
(
    IN  GT_U32 portIdx,
    IN  GT_U32 tc1,
    IN  GT_U32 tc2,
    IN  GT_U32 timeOut,
    OUT GT_U32 *outRate1Ptr,
    OUT GT_U32 *outRate2Ptr,
    OUT GT_U32 *dropRate1Ptr,
    OUT GT_U32 *dropRate2Ptr
)
{
    CPSS_PORT_EGRESS_CNTR_STC   egressCntrsArr[2]; /* egress counters */
    GT_U8                       cntrSetId;         /* iterator */
    GT_U8                       tc;                /* traffic class */
    GT_STATUS                   rc, rc1;           /* return codes */
    GT_U32      startNano, endNano;   /* start and end time seconds part */     
    GT_U32      startSec, endSec;     /* start and end time nanoseconds part */ 
    GT_U32      startMilli, endMilli; /* start and end time in milliseconds */  
    GT_U32      startTimeStamp;       /* debug - VNT time stamp start */     
    GT_U32      endTimeStamp;         /* debug - VNT time stamp start */     
    GT_U32      rate;                 /* debug - VNT time stamp based rate */

    /* set mode of Egress Port Counters.
       enable filter by TC and Port. */
    for (cntrSetId = 0; (cntrSetId < 2); cntrSetId++)
    {
        tc = (GT_U8)((cntrSetId == 0) ? tc1 : tc2);
        rc = prvTgfPortEgressCntrModeSet(cntrSetId,
                                         CPSS_EGRESS_CNT_PORT_E | CPSS_EGRESS_CNT_TC_E,
                                         prvTgfPortsArray[portIdx],
                                         0, tc, 0);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPortEgressCntrModeSet: %d", tc);
    }

    /* reset counters */
    rc  = prvTgfPortEgressCntrsGet(0, &egressCntrsArr[0]);
    rc1 = prvTgfPortEgressCntrsGet(1, &egressCntrsArr[0]);

    /* save start time */
    cpssOsTimeRT(&startSec,&startNano);

    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfPortEgressCntrsGet\n");
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc1, "prvTgfPortEgressCntrsGet\n");

    rc = prvTgfVntLastReadTimeStampGet(prvTgfDevNum, &startTimeStamp);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfVntLastReadTimeStampGet: %d", prvTgfPortsArray[portIdx]);

    /* sleep */
    cpssOsTimerWkAfter(timeOut);

    /* get counters */
    rc = prvTgfPortEgressCntrsGet(0, &egressCntrsArr[0]);
    rc1 = prvTgfPortEgressCntrsGet(1, &egressCntrsArr[1]);

    /* get end time */
    cpssOsTimeRT(&endSec,&endNano);

    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "prvTgfPortEgressCntrsGet\n");
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc1, "prvTgfPortEgressCntrsGet\n");

    rc = prvTgfVntLastReadTimeStampGet(prvTgfDevNum, &endTimeStamp);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfVntLastReadTimeStampGet: %d", prvTgfPortsArray[portIdx]);

    startMilli = startSec * 1000 + startNano / 1000000;
    endMilli = endSec * 1000 + endNano / 1000000;
    /* calculate exact timeout */
    timeOut = endMilli - startMilli;

    *outRate1Ptr = prvTgfGetRate(egressCntrsArr[0].outUcFrames, 0, timeOut); 
    *outRate2Ptr = prvTgfGetRate(egressCntrsArr[1].outUcFrames, 0, timeOut); 
    *dropRate1Ptr = prvTgfGetRate(egressCntrsArr[0].txqFilterDisc, 0, timeOut); 
    *dropRate2Ptr = prvTgfGetRate(egressCntrsArr[1].txqFilterDisc, 0, timeOut); 

    if (prvTgfCommonVntTimeStampsDebug)
    {
        rate = prvTgfCommonVntTimeStampsRateGet(prvTgfDevNum, startTimeStamp, endTimeStamp,egressCntrsArr[0].outUcFrames);
        cpssOsPrintf("---rate egressCntrsArr[0].outUcFrames %d\n", rate);
        rate = prvTgfCommonVntTimeStampsRateGet(prvTgfDevNum, startTimeStamp, endTimeStamp,egressCntrsArr[1].outUcFrames);
        cpssOsPrintf("---rate egressCntrsArr[1].outUcFrames %d\n", rate);    
        rate = prvTgfCommonVntTimeStampsRateGet(prvTgfDevNum, startTimeStamp, endTimeStamp,egressCntrsArr[0].txqFilterDisc);
        cpssOsPrintf("---rate egressCntrsArr[0].txqFilterDisc %d\n", rate);
        rate = prvTgfCommonVntTimeStampsRateGet(prvTgfDevNum, startTimeStamp, endTimeStamp,egressCntrsArr[1].txqFilterDisc);
        cpssOsPrintf("---rate egressCntrsArr[1].txqFilterDisc %d\n", rate);
    }

    /* check other counters. they must be 0 */
    UTF_VERIFY_EQUAL0_STRING_MAC(0, egressCntrsArr[0].brgEgrFilterDisc, "TC1 brgEgrFilterDisc\n");
    UTF_VERIFY_EQUAL0_STRING_MAC(0, egressCntrsArr[0].egrFrwDropFrames, "TC1 egrFrwDropFrames\n");
    UTF_VERIFY_EQUAL0_STRING_MAC(0, egressCntrsArr[0].outBcFrames, "TC1 outBcFrames\n");
    UTF_VERIFY_EQUAL0_STRING_MAC(0, egressCntrsArr[0].outCtrlFrames, "TC1 outCtrlFrames\n");
    UTF_VERIFY_EQUAL0_STRING_MAC(0, egressCntrsArr[0].outMcFrames, "TC1 outMcFrames\n");

    UTF_VERIFY_EQUAL0_STRING_MAC(0, egressCntrsArr[1].brgEgrFilterDisc, "TC2 brgEgrFilterDisc\n");
    UTF_VERIFY_EQUAL0_STRING_MAC(0, egressCntrsArr[1].egrFrwDropFrames, "TC2 egrFrwDropFrames\n");
    UTF_VERIFY_EQUAL0_STRING_MAC(0, egressCntrsArr[1].outBcFrames, "TC2 outBcFrames\n");
    UTF_VERIFY_EQUAL0_STRING_MAC(0, egressCntrsArr[1].outCtrlFrames, "TC2 outCtrlFrames\n");
    UTF_VERIFY_EQUAL0_STRING_MAC(0, egressCntrsArr[1].outMcFrames, "TC2 outMcFrames\n");

    /* restore default of Egress Port Counters */
    rc = prvTgfPortEgressCntrModeSet(0, 0, 0, 0, 0, 0);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPortEgressCntrModeSet: %d", 0);

    rc = prvTgfPortEgressCntrModeSet(1, 0, 0, 0, 0, 0);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPortEgressCntrModeSet: %d", 0);

}

/*******************************************************************************
* prvTgfCommonPortCntrRateGet
*
* DESCRIPTION:
*       Get rate of specific port MAC MIB counter.
*
* INPUTS:
*       portIdx - index of egress port
*       cntrName - name of counter
*       timeOut - timeout in milli seconds to calculate rate
*
* OUTPUTS:
*       outRatePtr - (pointer to) rate of port's counter
*
* RETURNS:
*       none
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfCommonPortCntrRateGet
(
    IN  GT_U32                      portIdx,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    IN  GT_U32                      timeOut,
    OUT GT_U32                      *outRatePtr
)
{
    GT_U64      cntrValue;            /* value of counter */
    GT_U32      startSec, endSec;     /* start and end time seconds part */
    GT_U32      startNano, endNano;   /* start and end time nanoseconds part */
    GT_U32      startMilli, endMilli; /* start and end time in milliseconds */
    GT_STATUS   rc;                   /* return code */
    GT_U32      startTimeStamp;       /* debug - VNT time stamp start */
    GT_U32      endTimeStamp;         /* debug - VNT time stamp start */
    GT_U32      rate;                 /* debug - VNT time stamp based rate */

    /* reset counter by read it */
    rc =  prvTgfPortMacCounterGet(prvTgfDevNum, prvTgfPortsArray[portIdx], 
                                  cntrName, &cntrValue);
    /* store start time */
    cpssOsTimeRT(&startSec,&startNano);

    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPortMacCounterGet: %d", prvTgfPortsArray[portIdx]);

    rc = prvTgfVntLastReadTimeStampGet(prvTgfDevNum, &startTimeStamp);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfVntLastReadTimeStampGet: %d", prvTgfPortsArray[portIdx]);

    /* sleep */
    cpssOsTimerWkAfter(timeOut);

    /* get counter */
    rc =  prvTgfPortMacCounterGet(prvTgfDevNum, prvTgfPortsArray[portIdx], 
                                  cntrName, &cntrValue);
    /* get end time */
    cpssOsTimeRT(&endSec,&endNano);

    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPortMacCounterGet: %d", prvTgfPortsArray[portIdx]);

    rc = prvTgfVntLastReadTimeStampGet(prvTgfDevNum, &endTimeStamp);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfVntLastReadTimeStampGet: %d", prvTgfPortsArray[portIdx]);


    startMilli = startSec * 1000 + startNano / 1000000;
    endMilli = endSec * 1000 + endNano / 1000000;

    timeOut = (endMilli - startMilli);

    *outRatePtr = prvTgfGetRate(cntrValue.l[0], 0, timeOut); 

    if (prvTgfCommonVntTimeStampsDebug)
    {
        rate = prvTgfCommonVntTimeStampsRateGet(prvTgfDevNum, startTimeStamp, endTimeStamp,cntrValue.l[0]);
        cpssOsPrintf("---rate %d\n", rate);
    }
}


/*******************************************************************************
* prvTgfCommonPortTxRateGet
*
* DESCRIPTION:
*       Get Tx rate of port by MAC MIB counters.
*
* INPUTS:
*       portIdx - index of egress port
*       timeOut - timeout in milli seconds to calculate rate
*
* OUTPUTS:
*       outRatePtr - (pointer to) Tx rate of port
*
* RETURNS:
*       none
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfCommonPortTxRateGet
(
    IN  GT_U32 portIdx,
    IN  GT_U32 timeOut,
    OUT GT_U32 *outRatePtr
)
{
    prvTgfCommonPortCntrRateGet(portIdx, CPSS_GOOD_UC_PKTS_SENT_E, 
                                timeOut,outRatePtr);
}

/*******************************************************************************
* prvTgfCommonPortRxRateGet
*
* DESCRIPTION:
*       Get Rx rate of port by MAC MIB counters.
*
* INPUTS:
*       portIdx - index of ingress port
*       timeOut - timeout in milli seconds to calculate rate
*
* OUTPUTS:
*       outRatePtr - (pointer to) Rx rate of port
*
* RETURNS:
*       none
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfCommonPortRxRateGet
(
    IN  GT_U32 portIdx,
    IN  GT_U32 timeOut,
    OUT GT_U32 *outRatePtr
)
{
    prvTgfCommonPortCntrRateGet(portIdx, CPSS_GOOD_UC_PKTS_RCV_E, 
                                timeOut,outRatePtr);
}

/*******************************************************************************
* prvTgfCommonDiffInPercentCalc
*
* DESCRIPTION:
*       Calculate difference between input values in percent
*
* INPUTS:
*       value1    - value1 to compare
*       value2    - value2 to compare
*
* OUTPUTS:
*       none
*
* RETURNS:
*       difference between input values in percent
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 prvTgfCommonDiffInPercentCalc
(
    IN  GT_U32 value1,
    IN  GT_U32 value2
)
{
    GT_U32      delta ;      /* difference between ports WS rate and input one*/
    GT_U32      percent;     /* difference between ports WS rate 
                               and input one in percent */

    if (value1 > value2)
    {
        delta = value1 - value2; 
    }
    else
    {    
        delta = value2 - value1;
    }

    /* calculate difference in percent of port's rate */
    percent = (delta * 100) / value1;
    return percent;
}

/*******************************************************************************
* prvTgfCommonPortWsRateGet
*
* DESCRIPTION:
*       Get wire speed rate of port in packets per second
*
* INPUTS:
*       portIdx    - index of egress port
*       packetSize - packet size in bytes including CRC
*
* OUTPUTS:
*       none
*
* RETURNS:
*       wire speed rate of port in packets per second.
*       0xFFFFFFFF - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 prvTgfCommonPortWsRateGet
(
    IN  GT_U32 portIdx,
    IN  GT_U32 packetSize
)
{
    GT_STATUS   rc;
    CPSS_PORT_SPEED_ENT portSpeed; /* port speed in enum values */
    GT_U32      speed;        /* speed in kilobits per second */
    double      packetRate;   /* packet rate in packets per second */
    GT_U32      l1PacketSize; /* packet size in media in bits */
    GT_U32      rateOut;      /* integer value of rate */

    /* get start value - reset counter */
    rc =  prvTgfPortSpeedGet(prvTgfDevNum, prvTgfPortsArray[portIdx],&portSpeed);
    if (rc != GT_OK)
    {
        PRV_UTF_LOG1_MAC("prvTgfPortSpeedGet: %d", prvTgfPortsArray[portIdx]);
        return 0xFFFFFFFF;
    }
    
    switch(portSpeed)
    {
        case CPSS_PORT_SPEED_10_E:    speed =     10000; break;
        case CPSS_PORT_SPEED_100_E:   speed =    100000; break;
        case CPSS_PORT_SPEED_1000_E:  speed =   1000000; break;
        case CPSS_PORT_SPEED_10000_E: speed =  10000000; break;
        case CPSS_PORT_SPEED_12000_E: speed =  12000000; break;
        case CPSS_PORT_SPEED_2500_E:  speed =   2500000; break;
        case CPSS_PORT_SPEED_5000_E:  speed =   5000000; break;
        case CPSS_PORT_SPEED_13600_E: speed =  13600000; break;
        case CPSS_PORT_SPEED_20000_E: speed =  20000000; break;
        case CPSS_PORT_SPEED_40000_E: speed =  40000000; break;
        case CPSS_PORT_SPEED_16000_E: speed =  16000000; break;
        default: 
            PRV_UTF_LOG1_MAC("wrong speed: %d", portSpeed);return 0xFFFFFFFF;
    }

    /* add 20 bytes IPG and preamble to get L1 byte size */
    l1PacketSize = (packetSize + 20) * 8;

    packetRate = speed;
    packetRate = (packetRate * 1000) / l1PacketSize;

    /* convert floating point to integer */
    rateOut = (GT_U32) packetRate;

    return rateOut;
}

