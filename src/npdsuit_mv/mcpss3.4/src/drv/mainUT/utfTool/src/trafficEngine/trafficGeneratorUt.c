/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* trafficGeneratorUt.c
*
* DESCRIPTION:
*   - emulate traffic generator capabilities
*    reset counters
*    read counters
*    send traffic
*    capture received traffic
*    set trigger on received traffic
*    ...
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <trafficEngine/trafficGeneratorUt.h>
#include <trafficEngine/trafficTableUt.h>
#include <utf/private/prvUtfExtras.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsTimer.h>
#include <gtUtil/gtBmPool.h>

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/port/cpssDxChPortStat.h>
    #include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
    #include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
    #include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
    #include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#endif /*CHX_FAMILY*/

/* copy field from source structure to destination structure
   dstStc can be pointer or not the 'field' should hold
   '->' or '.'

   --> the 'swap' allow to do copy of dst to src !
*/
#define UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dstStcPtr,srcStcPtr,field) \
    if(swap == 0)                                \
        (dstStcPtr)->field = (srcStcPtr)->field;   \
    else                                         \
        (srcStcPtr)->field = (dstStcPtr)->field

/* the device number that is connected to the CPU via it we transmit traffic
   to the 'loopback' port */
static GT_U8   cpuDevNum        = 0;
static GT_BOOL cpuDeviceExMx    = GT_TRUE;
static GT_BOOL cpuDeviceExMxPm  = GT_TRUE;
static GT_BOOL cpuDeviceDxCh    = GT_TRUE;

#ifdef EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIfTypes.h>
    #define UT_TRAFFIC_SPECIFIC_DEVICE_FORMAT_SIZE_CNS  sizeof(CPSS_EXMXPM_NET_RX_PARAMS_STC)
#endif /*EXMXPM_FAMILY*/

#ifdef EX_FAMILY
    #include <cpss/exMx/exMxGen/networkIf/cpssExMxGenNetIfTypes.h>
    #ifndef UT_TRAFFIC_SPECIFIC_DEVICE_FORMAT_SIZE_CNS
        #define UT_TRAFFIC_SPECIFIC_DEVICE_FORMAT_SIZE_CNS  sizeof(CPSS_EXMX_NET_RX_PARAMS_STC)
    #endif /*UT_TRAFFIC_SPECIFIC_DEVICE_FORMAT_SIZE_CNS*/
#endif /*EX_FAMILY*/

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
    #ifndef UT_TRAFFIC_SPECIFIC_DEVICE_FORMAT_SIZE_CNS
        #define UT_TRAFFIC_SPECIFIC_DEVICE_FORMAT_SIZE_CNS  sizeof(CPSS_DXCH_NET_RX_PARAMS_STC)
    #endif /*UT_TRAFFIC_SPECIFIC_DEVICE_FORMAT_SIZE_CNS*/
#endif /*CHX_FAMILY*/

#ifndef UT_TRAFFIC_SPECIFIC_DEVICE_FORMAT_SIZE_CNS
    #define UT_TRAFFIC_SPECIFIC_DEVICE_FORMAT_SIZE_CNS  1
#endif /*UT_TRAFFIC_SPECIFIC_DEVICE_FORMAT_SIZE_CNS*/

/*number of buffer at the pool*/
#define   MAX_NUM_OF_BUFFERS_AT_POOL    4

static GT_POOL_ID utTxBuffersPoolId=0;

/* define a cpu code that represent the packets in CPU table, that emulate the
  'Captured'  packets that egress port(s) */
#define   UT_TRAFFIC_CAPTURE_CPU_CODE_CNS   CPSS_NET_LAST_USER_DEFINED_E

/* reset parameter according to it's size */
#define RESET_PARAM_MAC(param)              osMemSet(&(param),0,sizeof(param))

#ifdef CHX_FAMILY
/* set PCL id for the capture */
static GT_U16   utTrafficCapturePclId = (BIT_7 - 1);/* use 7 bits to support ExMx */
/* PCL id that will represent the  capture */
#define   UT_TRAFFIC_CAPTURE_PCL_ID_CNS (utTrafficCapturePclId)

/* set index for the PCL to use for capture*/
static GT_U32   utTrafficCapturePclRuleIndex = 255;

/* rule index that will represent the capture */
#define UT_TRAFFIC_CAPTURE_PCL_RULE_INDEX_CNS (utTrafficCapturePclRuleIndex)

#endif /* CHX_FAMILY */
/*******************************************************************************
* utTrafficGeneratorCpuDeviceSet
*
* DESCRIPTION:
*       set the CPU device though it we send traffic to the 'Loop back port'
*
* INPUTS:
*       cpuDevice - the CPU device number
*       cpuDevExMx - the device to send the traffic from CPU is 'ExMx'
*       cpuDevExMxPm - the device to send the traffic from CPU is 'ExMxPm'
*       cpuDevDxCh - the device to send the traffic from CPU is 'DxCh'
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*******************************************************************************/
GT_STATUS utTrafficGeneratorCpuDeviceSet
(
    IN GT_U8    cpuDevice,
    IN GT_BOOL  cpuDevExMx,
    IN GT_BOOL  cpuDevExMxPm,
    IN GT_BOOL  cpuDevDxCh
)
{
    cpuDevNum        = cpuDevice;
    cpuDeviceExMx    = cpuDevExMx;
    cpuDeviceExMxPm  = cpuDevExMxPm;
    cpuDeviceDxCh    = cpuDevDxCh;

    return GT_OK;
}

/*******************************************************************************
* utTrafficGeneratorPortLoopbackModeEnableSet
*
* DESCRIPTION:
*       set port in loopback mode . enable/disable.
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       enable           - enable / disable (loopback/no loopback)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong interface
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*******************************************************************************/
GT_STATUS utTrafficGeneratorPortLoopbackModeEnableSet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN GT_BOOL                  enable
)
{

    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);
    UT_TRAFFIC_GENERATOR_PARAM_NUT_USED(enable);

    if(portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

#ifdef EXMXPM_FAMILY
#endif /*EXMXPM_FAMILY*/

#ifdef EX_FAMILY
#endif /*EX_FAMILY*/

#ifdef CHX_FAMILY
    /* ignore return value */
    (void)cpssDxChPortInternalLoopbackEnableSet(
            portInterfacePtr->devPort.devNum,
            portInterfacePtr->devPort.portNum,
            enable);
#endif /*CHX_FAMILY*/
    return GT_OK;
}

/*******************************************************************************
* utTrafficGeneratorPortForceLinkUpEnableSet
*
* DESCRIPTION:
*       set port in 'Force link up' mode . enable/disable.
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       enable           - enable / disable (force/not force)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong interface
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*******************************************************************************/
GT_STATUS utTrafficGeneratorPortForceLinkUpEnableSet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN GT_BOOL                  enable
)
{
    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);
    UT_TRAFFIC_GENERATOR_PARAM_NUT_USED(enable);

    if(portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

#ifdef EXMXPM_FAMILY
#endif /*EXMXPM_FAMILY*/

#ifdef EX_FAMILY
#endif /*EX_FAMILY*/

#ifdef CHX_FAMILY
    /* ignore return value */
    (void) cpssDxChPortForceLinkPassEnableSet(
            portInterfacePtr->devPort.devNum,
            portInterfacePtr->devPort.portNum,
            enable);
#endif /*CHX_FAMILY*/
    return GT_OK;
}

/*******************************************************************************
* utTrafficGeneratorPortCountersEthReset
*
* DESCRIPTION:
*       Reset the traffic generator counters on the port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong interface
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*******************************************************************************/
GT_STATUS utTrafficGeneratorPortCountersEthReset
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr
)
{
    CPSS_PORT_MAC_COUNTER_SET_STC   dummyCounters;

    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);

    if(portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

    dummyCounters.goodOctetsRcv.c[0] = 0;/* this line to avoid compiler warning on variable defined without use */

    /* function read the counters of the port ,
       assuming that the 'read' will clear the counters ... */


#ifdef EXMXPM_FAMILY
#endif /*EXMXPM_FAMILY*/

#ifdef EX_FAMILY
#endif /*EX_FAMILY*/

#ifdef CHX_FAMILY
    /* ignore return value */
    (void)cpssDxChPortMacCountersOnPortGet(
            portInterfacePtr->devPort.devNum,
            portInterfacePtr->devPort.portNum,
            &dummyCounters);
#endif /*CHX_FAMILY*/


    return GT_OK;
}

/*******************************************************************************
* utTrafficGeneratorPortCountersEthRead
*
* DESCRIPTION:
*       Read the traffic generator counters on the port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*
* OUTPUTS:
*       countersPtr      - (pointer to) the counters of port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong interface
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*******************************************************************************/
GT_STATUS utTrafficGeneratorPortCountersEthRead
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC  *countersPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);
    CPSS_NULL_PTR_CHECK_MAC(countersPtr);

    if(portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

#ifdef EXMXPM_FAMILY
#endif /*EXMXPM_FAMILY*/

#ifdef EX_FAMILY
#endif /*EX_FAMILY*/

#ifdef CHX_FAMILY
    /* ignore return value */
    (void)cpssDxChPortMacCountersOnPortGet(
            portInterfacePtr->devPort.devNum,
            portInterfacePtr->devPort.portNum,
            countersPtr);
#endif /*CHX_FAMILY*/

    /* the devices support goodPktsRcv or uc,mc,bc
       so I do combination ! */
    if(countersPtr->goodPktsRcv.l[0] == 0)
    {
        countersPtr->goodPktsRcv.l[0] = countersPtr->ucPktsRcv.l[0] + countersPtr->mcPktsRcv.l[0] + countersPtr->brdcPktsRcv.l[0];
        countersPtr->goodPktsRcv.l[1] = countersPtr->ucPktsRcv.l[1] + countersPtr->mcPktsRcv.l[1] + countersPtr->brdcPktsRcv.l[1];
    }

    /* the devices support goodPktsSend or uc,mc,bc
       so I do combination ! */
    if(countersPtr->goodPktsSent.l[0] == 0)
    {
        countersPtr->goodPktsSent.l[0] = countersPtr->ucPktsSent.l[0] + countersPtr->mcPktsSent.l[0] + countersPtr->brdcPktsSent.l[0];
        countersPtr->goodPktsSent.l[1] = countersPtr->ucPktsSent.l[1] + countersPtr->mcPktsSent.l[1] + countersPtr->brdcPktsSent.l[1];
    }

    return GT_OK;
}


#ifdef CHX_FAMILY
/*******************************************************************************
* utTestNetIfDsaInfoConvertDxCh
*
* DESCRIPTION:
*       convert the Dsa info
*       convert from UT to DXCH info , or from DXCH to UT (see parameter 'convert')
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dsaParamsPtr - (pointer to) UT formant Dsa information
*
* OUTPUTS:
*       dxChVlanParamPtr - (pointer to) DXCH Dsa information
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS utTestNetIfDsaInfoConvertDxCh(
    IN UT_CONVERT_DIRECTION_ENT convert,
    INOUT  UT_TRAFFIC_GENERATOR_NET_DSA_PARAMS_STC   *dsaParamsPtr,
    INOUT CPSS_DXCH_NET_DSA_PARAMS_STC     *dxChDsaParamsPtr
)
{
    GT_U32  swap = (convert == UT_CONVERT_DIRECTION_CPSS_TO_UT_E) ? 1 : 0;

    UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,commonParams.dsaTagType     );
    UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,commonParams.vpt            );
    UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,commonParams.cfiBit         );
    UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,commonParams.vid            );
    UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,commonParams.dropOnSource   );
    UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,commonParams.packetIsLooped );

    UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaType                     );

    switch(dsaParamsPtr->dsaType)
    {
        case UT_TRAFFIC_GENERATOR_NET_DSA_CMD_TO_CPU_E:
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.toCpu.isEgressPipe        );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.toCpu.isTagged            );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.toCpu.devNum              );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.toCpu.srcIsTrunk          );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.toCpu.interface.srcTrunkId);
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.toCpu.interface.portNum   );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.toCpu.cpuCode             );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.toCpu.wasTruncated        );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.toCpu.originByteCount     );
            break;
        case UT_TRAFFIC_GENERATOR_NET_DSA_CMD_FROM_CPU_E:
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.fromCpu.dstInterface        );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.fromCpu.tc                  );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.fromCpu.dp                  );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.fromCpu.egrFilterEn         );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.fromCpu.cascadeControl      );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.fromCpu.egrFilterRegistered );

            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.fromCpu.srcId               );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.fromCpu.srcDev              );


            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.fromCpu.extDestInfo.multiDest.excludeInterface  );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.fromCpu.extDestInfo.multiDest.mirrorToAllCPUs   );

            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged          );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.fromCpu.extDestInfo.devPort.mailBoxToNeighborCPU );
            break;
        case UT_TRAFFIC_GENERATOR_NET_DSA_CMD_FORWARD_E:
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.forward.srcIsTagged        );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.forward.srcDev             );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.forward.srcIsTrunk         );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.forward.source.trunkId     );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.forward.source.portNum     );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.forward.srcId              );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.forward.egrFilterRegistered);
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.forward.wasRouted          );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.forward.qosProfileIndex    );
            UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,dxChDsaParamsPtr,dsaParamsPtr,dsaInfo.forward.dstInterface       );
            break;

        default:
            return GT_BAD_PARAM;

    };

    return GT_OK;
}
#endif /*CHX_FAMILY*/
/*******************************************************************************
* utTrafficGeneratorNetIfSdmaSyncTxPacketSend
*
*       This function receives packet buffers & parameters from Applications .
*       Prepares them for the transmit operation, and enqueues the prepared
*       descriptors to the PP's tx queues.
*       After transmit end all transmitted packets descriptors are freed.
*        -- SDMA relate.
*       function activates Tx SDMA , function wait for PP to finish processing
*       the buffers.
*
* APPLICABLE DEVICES: ALL PCI/SDMA Devices
*
* INPUTS:
*       devNum       - Device number.
*       pcktParamsPtr- The internal packet params to be set into the packet
*                      descriptors.
*       buffList     - The packet data buffers list.
*       buffLenList  - A list of the buffers len in buffList.
*       numOfBufs    - Length of buffList.
*
* OUTPUTS:
*       firstDescPtr - (pointer to) the Tx desc. list control structure
*
* RETURNS:
*       GT_OK on success, or
*       GT_NO_RESOURCE if there is not enough desc. for enqueuing the packet
*       GT_HW_ERROR - when  after transmission the last descriptor own bit
*                     wasn't changed for long time.
*       GT_FAIL otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*           support both :
*           ===== regular DSA tag  ====
*           ===== extended DSA tag ====
*
*******************************************************************************/
static GT_STATUS netIfSdmaSyncTxPacketSend
(
    IN GT_U8                                    devNum,
    IN UT_TRAFFIC_GENERATOR_NET_TX_PARAMS_STC   *pcktParamsPtr,
    IN GT_U8                                    *buffList[],
    IN GT_U32                                   buffLenList[],
    IN GT_U32                                   numOfBufs
)
{
    UT_TRAFFIC_GENERATOR_PARAM_NUT_USED(devNum);
    UT_TRAFFIC_GENERATOR_PARAM_NUT_USED(pcktParamsPtr);
    UT_TRAFFIC_GENERATOR_PARAM_NUT_USED(buffList);
    UT_TRAFFIC_GENERATOR_PARAM_NUT_USED(buffLenList);
    UT_TRAFFIC_GENERATOR_PARAM_NUT_USED(numOfBufs);

#ifdef CHX_FAMILY
    if(cpuDeviceDxCh == GT_TRUE)
    {
        CPSS_DXCH_NET_TX_PARAMS_STC              dxChPcktParams={0};
        GT_U32  swap = 0;

        UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,&dxChPcktParams,pcktParamsPtr,packetIsTagged                       );
        UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,&dxChPcktParams,pcktParamsPtr,cookie                               );
        UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,&dxChPcktParams,pcktParamsPtr,sdmaInfo.recalcCrc                   );
        UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,&dxChPcktParams,pcktParamsPtr,sdmaInfo.txQueue                     );
        UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,&dxChPcktParams,pcktParamsPtr,sdmaInfo.evReqHndl                   );
        UT_TRAFFIC_GENERATOR_FIELD_COPY_MAC(swap,&dxChPcktParams,pcktParamsPtr,sdmaInfo.invokeTxBufferQueueEvent    );

        utTestNetIfDsaInfoConvertDxCh(UT_CONVERT_DIRECTION_UT_TO_CPSS_E,&pcktParamsPtr->dsaParam,&dxChPcktParams.dsaParam);

        return cpssDxChNetIfSdmaSyncTxPacketSend(devNum,&dxChPcktParams,buffList,buffLenList,numOfBufs);
    }
#endif /*CHX_FAMILY*/

    return GT_NOT_IMPLEMENTED;
}

/*******************************************************************************
* utTrafficGeneratorNetIfSdmaSyncTxPacketSend
*
* DESCRIPTION:
*       Transmit the traffic to the port (the packet will ingress to the PP
*       after 'loopback' and will act as it was received from a traffic
*       generator connected to the port)
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       bufferPtr    - (pointer to) the buffer to send
*       bufferLength  - buffer length (include CRC bytes)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong interface
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*******************************************************************************/
static GT_STATUS utTrafficGeneratorNetIfSdmaSyncTxPacketSend
(
    IN CPSS_INTERFACE_INFO_STC          *portInterfacePtr,
    IN GT_U8                            *bufferPtr,
    IN GT_U32                           bufferLength
)
{
    GT_U8                                    devNum;
    UT_TRAFFIC_GENERATOR_NET_TX_PARAMS_STC   pcktParams;
    GT_U8                                    *buffList[1];
    GT_U32                                   buffLenList[1];
    GT_U32                                   numOfBufs = 1;
    UT_TRAFFIC_GENERATOR_NET_DSA_FROM_CPU_STC *fromCpuPtr;
    GT_U32  vpt;
    GT_U32  cfiBit;
    GT_U32  vid;

    buffList[0] = bufferPtr;
    buffLenList[0] = bufferLength;

    devNum = cpuDevNum;

    if((bufferPtr[12] == (GT_U8)(UT_ETHERTYPE_8100_TAG_CNS >> 8)) &&
       (bufferPtr[13] == (GT_U8)(UT_ETHERTYPE_8100_TAG_CNS & 0xff)))
    {
        pcktParams.packetIsTagged = GT_TRUE;
        vpt     = (GT_U8)(bufferPtr[14] >> 5);
        cfiBit  = (GT_U8)((bufferPtr[14] >> 4) & 1);
        vid     = ((bufferPtr[14] & 0xf) << 8) | bufferPtr[15];
    }
    else
    {
        pcktParams.packetIsTagged = GT_FALSE;
        vpt     = 0;
        cfiBit  = 0;
        vid     = 4094;
    }

    pcktParams.cookie = NULL;

    pcktParams.sdmaInfo.recalcCrc = GT_TRUE;
    pcktParams.sdmaInfo.txQueue = 7;
    pcktParams.sdmaInfo.evReqHndl = 0;
    pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_TRUE;/*??*/

    pcktParams.dsaParam.commonParams.dsaTagType = UT_TRAFFIC_GENERATOR_NET_DSA_TYPE_EXTENDED_E;
    pcktParams.dsaParam.commonParams.vpt = (GT_U8)vpt;
    pcktParams.dsaParam.commonParams.cfiBit = (GT_U8)cfiBit;
    pcktParams.dsaParam.commonParams.vid = (GT_U16)vid;
    pcktParams.dsaParam.commonParams.dropOnSource = GT_FALSE;
    pcktParams.dsaParam.commonParams.packetIsLooped = GT_FALSE;

    pcktParams.dsaParam.dsaType = UT_TRAFFIC_GENERATOR_NET_DSA_CMD_FROM_CPU_E;
    fromCpuPtr = &pcktParams.dsaParam.dsaInfo.fromCpu;

    fromCpuPtr->dstInterface = *portInterfacePtr;
    fromCpuPtr->tc = 7;
    fromCpuPtr->dp = CPSS_DP_GREEN_E;
    fromCpuPtr->egrFilterEn = GT_FALSE;
    fromCpuPtr->cascadeControl = GT_TRUE;
    fromCpuPtr->egrFilterRegistered = GT_FALSE;
    fromCpuPtr->srcId = 0;
    fromCpuPtr->srcDev = devNum;
    fromCpuPtr->extDestInfo.devPort.dstIsTagged = pcktParams.packetIsTagged;
    fromCpuPtr->extDestInfo.devPort.mailBoxToNeighborCPU = GT_FALSE;


    return netIfSdmaSyncTxPacketSend(devNum,&pcktParams,buffList,buffLenList,numOfBufs);

}

/*******************************************************************************
* utTrafficGeneratorPoolInit
*
* DESCRIPTION:
*       Init the pool for sending packets
*
* INPUTS:
*       None
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong interface
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*******************************************************************************/
static GT_STATUS utTrafficGeneratorPoolInit
(
    void
)
{
    GT_STATUS   rc;
    static GT_BOOL  poolCreated = GT_FALSE;

    if(poolCreated == GT_FALSE)
    {
        /* create pool of buffers from Cache */
        rc = gtPoolCreateDmaPool(
                      UT_TRAFFIC_TX_BUFFER_MAX_SIZE_CNS,
                      GT_4_BYTE_ALIGNMENT,
                      MAX_NUM_OF_BUFFERS_AT_POOL,
                      GT_TRUE,
                      &utTxBuffersPoolId);
        RC_CHECK(rc);

        poolCreated = GT_TRUE;
    }

    return GT_OK;
}



/*******************************************************************************
* utTrafficGeneratorPortTxEthTransmit
*
* DESCRIPTION:
*       Transmit the traffic to the port (the packet will ingress to the PP
*       after 'loopback' and will act as it was received from a traffic
*       generator connected to the port)
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       packetInfoPtr    - (pointer to) the packet info
*       burstCount  - number of frames to send in the burst (non-zero value)
*       numVfd      - number of VFDs (override of the background in specific offsets)
*                     can be 0
*       vfdArray   - pointer to array of VFDs (may be NULL if numVfd = 0)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong interface
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*******************************************************************************/
GT_STATUS utTrafficGeneratorPortTxEthTransmit
(
    IN CPSS_INTERFACE_INFO_STC          *portInterfacePtr,
    IN UT_TRAFFIC_ENGINE_PACKET_STC     *packetInfoPtr,
    IN GT_U32                           burstCount,
    IN GT_U32                           numVfd,
    IN UT_TRAFFIC_ENGINE_VFD_STC        vfdArray[]
)
{
    GT_STATUS   rc;
    GT_U32      ii,jj;
    GT_U8   *bufferPtr=NULL;/* buffer for current packet to send */

    /* do validity check on the parameters */
    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr);
    if(numVfd)
    {
        CPSS_NULL_PTR_CHECK_MAC(vfdArray);
    }

    if(burstCount == 0)
    {
        return GT_BAD_PARAM;
    }

    /* init the pool */
    utTrafficGeneratorPoolInit();

    if(portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

    if((packetInfoPtr->totalLen + UT_CRC_LEN_CNS) > UT_TRAFFIC_TX_BUFFER_MAX_SIZE_CNS)
    {
        return GT_BAD_PARAM;
    }

    /* get buffer from the pool */
    bufferPtr = gtPoolGetBuf(utTxBuffersPoolId);
    if(bufferPtr == NULL)
    {
        /* out of buffers !? */
        return GT_OUT_OF_CPU_MEM;
    }

    /* build the basic frame info */
    rc = utTrafficEnginePacketBuild(packetInfoPtr,bufferPtr);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

    for (ii = 0 ; ii < burstCount ; ii++)
    {
        for(jj = 0 ; jj < numVfd; jj++)
        {
            vfdArray[jj].modeExtraInfo = ii;
            /* over ride the buffer with VFD info */
            rc = utTrafficEnginePacketVfdApply(&vfdArray[jj],bufferPtr,packetInfoPtr->totalLen);
            if(rc != GT_OK)
            {
                goto exit_cleanly_lbl;
            }
        }

        /*this buffer now ready for the checksums , and the CRC */


        /* add the ability to trace the packet content*/
        rc = utTrafficTracePacket(bufferPtr,
                             packetInfoPtr->totalLen + UT_CRC_LEN_CNS);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }

        /* send the buffer to the device,port */
        rc = utTrafficGeneratorNetIfSdmaSyncTxPacketSend(portInterfacePtr,
                    bufferPtr,
                    packetInfoPtr->totalLen + UT_CRC_LEN_CNS);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }

    }

    /* let the packets be sent */
    {
        GT_U32  timeLoop = 1;

        while(timeLoop--)/* allow debugging by set this value = 1000 */
        {
            osTimerWkAfter(2000);
        }
    }

exit_cleanly_lbl:
    if(bufferPtr)
    {
        /* release the buffer back to the pool */
        gtPoolFreeBuf(utTxBuffersPoolId,bufferPtr);
    }

    RC_CHECK(rc);

    return GT_OK;
}


/*******************************************************************************
* utTrafficGeneratorRxParamsConvert
*
* DESCRIPTION:
*       Get entry from rxNetworkIf table
*
* INPUTS:
*       specificDeviceFormatPtr - (pointer to)the specific device Rx info format
*                             format of DXCH / EXMX /DXSAL ...
* OUTPUTS:
*       rxParamsPtr - (pointer to) the specific device Rx info format.
*
* RETURNS:
*       GT_OK - on success
*       GT_OUT_OF_RANGE - index is out of range (entry not valid)
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS utTrafficGeneratorRxParamsConvert
(
    IN  void*                                   specificDeviceFormatPtr,
    OUT UT_TRAFFIC_GENERATOR_NET_RX_PARAMS_STC  *rxParamsPtr
)
{
    UT_TRAFFIC_GENERATOR_PARAM_NUT_USED(specificDeviceFormatPtr);
    UT_TRAFFIC_GENERATOR_PARAM_NUT_USED(rxParamsPtr);

#ifdef CHX_FAMILY
    if(cpuDeviceDxCh == GT_TRUE)
    {
        CPSS_DXCH_NET_RX_PARAMS_STC *dxChPcktParamsPtr= specificDeviceFormatPtr;

        return utTestNetIfDsaInfoConvertDxCh(UT_CONVERT_DIRECTION_CPSS_TO_UT_E,
                &rxParamsPtr->dsaParam,&dxChPcktParamsPtr->dsaParam);
    }
#endif /*CHX_FAMILY*/

    return GT_NOT_IMPLEMENTED;
}


/*******************************************************************************
* utTrafficGeneratorRxInCpuGet
*
* DESCRIPTION:
*       Get entry from rxNetworkIf table
*
* INPUTS:
*       packetType - the packet type that we want to get. regular/'captured'
*                   for 'captured' see API of:
*                   utTrafficGeneratorPortTxEthCaptureSet
*
*       getFirst - get first/next entry
*       packetBufLenPtr - the length of the user space for the packet
*
* OUTPUTS:
*       packetBufPtr    - packet's buffer (pre allocated by the user)
*       packetBufLenPtr - the length of the copied packet to gtBuf
*       packetLenPtr    - the Rx packet original length
*       devNumPtr       - packet's device number
*       queuePtr       - The Rx queue in which the packet was received.
*       rxParamsPtr - (pointer to) the specific device Rx info format.
*
* RETURNS:
*       GT_OK - on success
*       GT_NO_MORE - no more entries
*       GT_BAD_PTR - on NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS utTrafficGeneratorRxInCpuGet
(
    IN  UT_TRAFFIC_GENERATOR_PACKET_TYPE_ENT packetType,
    IN  GT_BOOL         getFirst,
    OUT GT_U8           *packetBufPtr,
    INOUT GT_U32        *packetBufLenPtr,
    OUT GT_U32          *packetLenPtr,
    OUT GT_U8           *devNumPtr,
    OUT GT_U8           *queuePtr,
    OUT UT_TRAFFIC_GENERATOR_NET_RX_PARAMS_STC           *rxParamsPtr
)
{
    GT_STATUS rc;
    static GT_U32   currentIndex = 0;
    UT_TRAFFIC_GENERATOR_PACKET_TYPE_ENT currentPacketType;/* current packet type */
    char   specificDeviceFormat[UT_TRAFFIC_SPECIFIC_DEVICE_FORMAT_SIZE_CNS];/* the specific device Rx info format*/
    GT_BOOL         newGetFirst = getFirst;/* a local get first parameter  */

    CPSS_NULL_PTR_CHECK_MAC(packetBufPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetBufLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(devNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(queuePtr);
    CPSS_NULL_PTR_CHECK_MAC(rxParamsPtr);

start_lbl:
    currentPacketType = UT_TRAFFIC_GENERATOR_PACKET_TYPE_REGULAR_E;

    if(newGetFirst == GT_TRUE)
    {
        currentIndex = 0;
    }
    else
    {
        currentIndex++;
    }

    newGetFirst = GT_FALSE; /* set parameter as 'next' . to avoid 'Endless loop' .
                               due to the 'goto start_lbl' */

    rc = utTrafficRxPkGet(currentIndex,packetBufPtr,packetBufLenPtr,
                          packetLenPtr,devNumPtr,queuePtr,specificDeviceFormat);
    if(rc == GT_OUT_OF_RANGE)
    {
        /* table is done */
        return GT_NO_MORE;
    }
    else if(rc == GT_NOT_FOUND)
    {
        /* current index has not valid entry , but table is not done yet */
        goto start_lbl;
    }
    RC_CHECK(rc);

    /* convert specificDeviceFormatPtr to rxParamsPtr */
    rc = utTrafficGeneratorRxParamsConvert(specificDeviceFormat,rxParamsPtr);
    RC_CHECK(rc);

    if(rxParamsPtr->dsaParam.dsaType == UT_TRAFFIC_GENERATOR_NET_DSA_CMD_TO_CPU_E)
    {
        if(rxParamsPtr->dsaParam.dsaInfo.toCpu.cpuCode == UT_TRAFFIC_CAPTURE_CPU_CODE_CNS)
        {
            /* 'Captured' packet */
            currentPacketType = UT_TRAFFIC_GENERATOR_PACKET_TYPE_CAPTURE_E;
        }
    }

    if(currentPacketType != packetType)
    {
        /* not matching packet type , so try to get next packet */
        goto start_lbl;
    }

    /* add the ability to trace the packet content */
    rc = utTrafficTracePacket(packetBufPtr,(*packetLenPtr));
    RC_CHECK(rc);

    /* add ability to trace the packet parts fields */
    CPSS_TBD_BOOKMARK

    /* add ability to trace the Rx specific device parameters */
    CPSS_TBD_BOOKMARK

    return GT_OK;
}

/*******************************************************************************
* utTrafficGeneratorPortTxEthTriggerCountersGet
*
* DESCRIPTION:
*       Get number of triggers on port (traffic that Tx egress this port in PP
*       that match the triggers (VFD parameters))
*
*       NOTE: The 'captured' (emulation of capture) must be set on this port
*       see APIs :
*           utTrafficGeneratorPortTxEthCaptureSet
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       numVfd      - number of VFDs (define of the background in specific offsets)
*       vfdArray    - pointer to array of VFDs (not NULL)
* OUTPUTS:
*       numTriggersPtr - (pointer to) number of triggers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong interface
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*******************************************************************************/
GT_STATUS utTrafficGeneratorPortTxEthTriggerCountersGet
(
    IN CPSS_INTERFACE_INFO_STC          *portInterfacePtr,
    IN GT_U32                           numVfd,
    IN UT_TRAFFIC_ENGINE_VFD_STC        vfdArray[],
    OUT GT_U32                          *numTriggersPtr
)
{
    GT_STATUS   rc;
    GT_U32 packetActualLength;
    GT_U8   packetBuff[UT_TRAFFIC_RX_BUFFER_MAX_SIZE_CNS];
    GT_U32 buffLen = UT_TRAFFIC_RX_BUFFER_MAX_SIZE_CNS;
    GT_U8  devNum;
    GT_U8  queue;
    UT_TRAFFIC_GENERATOR_NET_RX_PARAMS_STC rxParam;
    GT_BOOL getFirst = GT_TRUE;
    GT_U32  ii;
    GT_BOOL match;

    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);
    CPSS_NULL_PTR_CHECK_MAC(vfdArray);
    CPSS_NULL_PTR_CHECK_MAC(numTriggersPtr);
    if(numVfd == 0)
    {
        return GT_BAD_PARAM;
    }

    if(portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

    (*numTriggersPtr) = 0;

    while(1)
    {
        rc = utTrafficGeneratorRxInCpuGet(
                    UT_TRAFFIC_GENERATOR_PACKET_TYPE_CAPTURE_E,  /* 'Captured' packets */
                    getFirst, packetBuff,
                    &buffLen, &packetActualLength,
                    &devNum,&queue,
                    &rxParam);
        if(rc != GT_OK)
        {
            break;
        }

        getFirst = GT_FALSE; /* now we get the next */

        /* check that the DSA is 'TO_CPU' */
        /* check that the packet came from the needed port,device (not trunk) */
        if(rxParam.dsaParam.dsaType != UT_TRAFFIC_GENERATOR_NET_DSA_CMD_TO_CPU_E ||
           rxParam.dsaParam.dsaInfo.toCpu.srcIsTrunk != 0 ||
           rxParam.dsaParam.dsaInfo.toCpu.devNum != portInterfacePtr->devPort.devNum ||
           rxParam.dsaParam.dsaInfo.toCpu.interface.portNum != portInterfacePtr->devPort.portNum)
        {
            /* the packet NOT came from where I expected it to be */
            continue;
        }

        /* check the VFD parameters */
        for(ii = 0 ; ii< numVfd ;ii++)
        {
            rc = utTrafficEnginePacketVfdCheck(&vfdArray[ii],packetBuff,buffLen,&match);
            RC_CHECK(rc);

            if(match != GT_TRUE)
            {
                break;
            }
        }

        if( ii == numVfd)
        {
            /* we have a match */
            (*numTriggersPtr)++;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* utTrafficGeneratorTxEthTriggerCheck
*
* DESCRIPTION:
*       check if the buffer triggered by the trigger parameters
*
*       NOTE: The 'captured' (emulation of capture) must be set on this port
*       see APIs :
*           utTrafficGeneratorPortTxEthCaptureSet
*           utTrafficGeneratorRxInCpuGet
*
* INPUTS:
*       bufferPtr   - pointer to the buffer
*       bufferLength - length of the buffer
*       numVfd      - number of VFDs (define of the background in specific offsets)
*       vfdArray    - pointer to array of VFDs (not NULL)
* OUTPUTS:
*       triggeredPtr - (pointer to) triggered / not triggered
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong interface
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*******************************************************************************/
GT_STATUS utTrafficGeneratorTxEthTriggerCheck
(
    IN GT_U8                            *bufferPtr,
    IN GT_U32                           bufferLength,
    IN GT_U32                           numVfd,
    IN UT_TRAFFIC_ENGINE_VFD_STC        vfdArray[],
    OUT GT_BOOL                         *triggeredPtr
)
{
    GT_STATUS   rc;
    GT_U32      ii;
    GT_BOOL     match;

    CPSS_NULL_PTR_CHECK_MAC(vfdArray);
    CPSS_NULL_PTR_CHECK_MAC(triggeredPtr);
    if(numVfd == 0)
    {
        return GT_BAD_PARAM;
    }

    *triggeredPtr = GT_FALSE;

    /* check the VFD parameters */
    for(ii = 0 ; ii< numVfd ;ii++)
    {
        rc = utTrafficEnginePacketVfdCheck(&vfdArray[ii],bufferPtr,bufferLength,&match);
        RC_CHECK(rc);

        if(match != GT_TRUE)
        {
            break;
        }
    }

    if( ii == numVfd)
    {
        *triggeredPtr = GT_TRUE;
    }

    return GT_OK;
}

#ifdef CHX_FAMILY

/*******************************************************************************
* captureSetDxCh
*
* DESCRIPTION:
*       Start/Stop capture the traffic that egress the port
*
*       NOTE:
*           the Start 'Capture' is emulated by setting next for the port:
*           1. ingress PCL rule for ALL traffic from this port to TRAP to the CPU
*              we will assign it with 'Application specific cpu code' to
*              distinguish from other traffic go to the CPU
*
*           the Stop 'Capture' is emulated by setting next for the port:
*           2. remove the ingress PCL rule that function
*              utTrafficGeneratorPortTxEthCaptureStart added
*
* INPUTS:
*       devNum - device number
*       portNum - port number
*       start - start/stop. capture on this port.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong interface
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*******************************************************************************/
static GT_STATUS captureSetDxCh
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  start
)
{
    GT_STATUS   rc;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat;
    GT_U32                             ruleIndex;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      mask;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      pattern;
    CPSS_DXCH_PCL_ACTION_STC           action;
    static GT_U32   first = 1;
    CPSS_INTERFACE_INFO_STC            portInterface;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC       lookupCfg;

    if(start == GT_TRUE)
    {
        if(first)
        {
            RESET_PARAM_MAC(mask);
            RESET_PARAM_MAC(pattern);
            RESET_PARAM_MAC(action);

            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
            ruleIndex = UT_TRAFFIC_CAPTURE_PCL_RULE_INDEX_CNS;

            /* need to match Only the PCL id */
            mask.ruleStdNotIp.common.pclId = 0xFFFF;/*exact match on this field */
            pattern.ruleStdNotIp.common.pclId = UT_TRAFFIC_CAPTURE_PCL_ID_CNS;

            /* trap to CPU */
            action.pktCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            /* set specific CPU code for those 'captured' packets */
            action.mirror.cpuCode = UT_TRAFFIC_CAPTURE_CPU_CODE_CNS;

            /* add the rule to the PCL */
            rc = cpssDxChPclRuleSet(
                devNum,ruleFormat,ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask,&pattern,&action);
            RC_CHECK(rc);

            if(rc == GT_OK)
            {
                first = 0;
            }
        }
    }

    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.portNum = portNum;
    portInterface.devPort.devNum = devNum;

    RESET_PARAM_MAC(lookupCfg);

    if(start == GT_TRUE)
    {
        lookupCfg.enableLookup = GT_TRUE;
        lookupCfg.pclId = UT_TRAFFIC_CAPTURE_PCL_ID_CNS;
        lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        lookupCfg.groupKeyTypes.ipv4Key  = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
        lookupCfg.groupKeyTypes.ipv6Key  = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
    }
    else
    {
        lookupCfg.enableLookup = GT_TRUE;
    }

    rc = prvUtfHwFromSwDeviceNumberGet(portInterface.devPort.devNum,
                                       &(portInterface.devPort.devNum));
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = cpssDxChPclCfgTblSet(devNum,&portInterface,
        CPSS_PCL_DIRECTION_INGRESS_E,
        CPSS_PCL_LOOKUP_0_E,
        &lookupCfg);
    RC_CHECK(rc);


    rc = cpssDxChPclPortIngressPolicyEnable(devNum,portNum,start);
    RC_CHECK(rc);

    return GT_OK;
}
#endif /*CHX_FAMILY*/

/*******************************************************************************
* captureSet
*
* DESCRIPTION:
*       Start/Stop capture the traffic that egress the port
*
*       NOTE:
*           the Start 'Capture' is emulated by setting next for the port:
*           1. ingress PCL rule for ALL traffic from this port to TRAP to the CPU
*              we will assign it with 'Application specific cpu code' to
*              distinguish from other traffic go to the CPU
*
*           the Stop 'Capture' is emulated by setting next for the port:
*           2. remove the ingress PCL rule that function
*              utTrafficGeneratorPortTxEthCaptureStart added
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       start - start/stop. capture on this port.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong interface
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*******************************************************************************/
static GT_STATUS captureSet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN GT_BOOL                  start
)
{
    UT_TRAFFIC_GENERATOR_PARAM_NUT_USED(portInterfacePtr);
    UT_TRAFFIC_GENERATOR_PARAM_NUT_USED(start);

#ifdef EXMXPM_FAMILY
#endif /*EXMXPM_FAMILY*/

#ifdef EX_FAMILY
#endif /*EX_FAMILY*/

#ifdef CHX_FAMILY
    /* ignore return value */
    (void)captureSetDxCh(
            portInterfacePtr->devPort.devNum,
            portInterfacePtr->devPort.portNum,
            start);
#endif /*CHX_FAMILY*/

    return GT_OK;
}

/*******************************************************************************
* utTrafficGeneratorPortTxEthCaptureSet
*
* DESCRIPTION:
*       Start/Stop capture the traffic that egress the port
*
*       NOTE:
*           the Start 'Capture' is emulated by setting next for the port:
*           1. loopback on the port (so all traffic that egress the port will
*              ingress the same way)
*           2. ingress PCL rule for ALL traffic from this port to TRAP to the CPU
*              we will assign it with 'Application specific cpu code' to
*              distinguish from other traffic go to the CPU
*
*           the Stop 'Capture' is emulated by setting next for the port:
*           1. disable loopback on the port
*           2. remove the ingress PCL rule that function
*              utTrafficGeneratorPortTxEthCaptureStart added
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       start - start/stop. capture on this port.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong interface
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*******************************************************************************/
GT_STATUS utTrafficGeneratorPortTxEthCaptureSet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN GT_BOOL                  start
)
{
    GT_STATUS rc;

    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);

    if(portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

    rc = utTrafficGeneratorPortLoopbackModeEnableSet(portInterfacePtr,start);
    RC_CHECK(rc);

    rc = captureSet(portInterfacePtr,start);
    RC_CHECK(rc);

    return GT_OK;
}

