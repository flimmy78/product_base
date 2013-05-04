/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfTrafficGenerator.c
*
* DESCRIPTION:
*     emulate traffic generator capabilities
*     reset counters
*     read counters
*     send traffic
*     capture received traffic
*     set trigger on received traffic
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>
#include <utf/private/prvUtfExtras.h>
#include <gtOs/gtOsTimer.h>
#include <gtUtil/gtBmPool.h>
#include <utf/utfMain.h>

#include <common/tgfCommon.h>
#include <common/tgfBridgeGen.h>



#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/port/cpssDxChPortStat.h>
    #include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
    #include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
    #include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
    #include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
    #include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>
    #include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
    #include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortStat.h>
    #include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortCtrl.h>
    #include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIf.h>
    #include <cpss/exMxPm/exMxPmGen/pcl/cpssExMxPmPcl.h>
#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    #include <cpss/exMx/exMxGen/port/cpssExMxPortStat.h>
    #include <cpss/exMx/exMxGen/port/cpssExMxPortCtrl.h>
    #include <cpss/exMx/exMxGen/networkIf/cpssExMxNetIf.h>
    #include <cpss/exMx/exMxGen/pcl/cpssExMxPcl.h>
#endif /* EX_FAMILY */


/******************************************************************************\
 *                              Define section                                *
\******************************************************************************/
static GT_U32   miiCounter = 10;
/* the device number that is connected to the CPU via it we transmit traffic
   to the 'loopback' port */
static GT_U8   prvTgfCpuDevNum = 0;

/* default number of time loops to send packet */
#define TGF_DEFAULT_TIME_LOOP_NUM_CNS   5

/* number of time loops to send packet */
static GT_U32   prvTgfTimeLoop = TGF_DEFAULT_TIME_LOOP_NUM_CNS;

/* default number of loops to get captured packet */
#define TGF_DEFAULT_CAPTURE_LOOP_NUM_CNS 5

/* default sleep time in single loop step to get captured packet */
#define TGF_DEFAULT_CAPTURE_SLEEP_TIME_CNS 10

/* number of time loops to get captured packet */
static GT_U32   prvTgfCaptureTimeLoop = TGF_DEFAULT_CAPTURE_LOOP_NUM_CNS;

#ifdef CHX_FAMILY
    #ifndef TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS
        #define TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS  sizeof(CPSS_DXCH_NET_RX_PARAMS_STC)
    #endif /* TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS */
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    #ifndef TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS
        #define TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS  sizeof(CPSS_EXMXPM_NET_RX_PARAMS_STC)
    #endif /* TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS */
#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    #ifndef TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS
        #define TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS  sizeof(CPSS_EXMX_NET_RX_PARAMS_STC)
    #endif /* TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS */
#endif /* EX_FAMILY */

#ifndef TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS
    #define TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS  1
#endif /* TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS */

/*number of buffer at the pool*/
#define   TGF_MAX_NUM_OF_BUFFERS_AT_POOL    4

static GT_POOL_ID prvTgfTxBuffersPoolId = 0;

/* set PCL id for the capture */
static GT_U16   prvTgfTrafficCapturePclId = (BIT_7 - 1);/* use 7 bits to support ExMx */

/* PCL id that will represent the  capture */
#define   TGF_CAPTURE_PCL_ID_CNS (prvTgfTrafficCapturePclId)

/* set index for the PCL to use for capture*/
static GT_U32   prvTgfTrafficCapturePclRuleIndex = 255;

/* rule index that will represent the capture */
#define TGF_PCL_RULE_INDEX_CNS (prvTgfTrafficCapturePclRuleIndex)

/* use this etherType to recognize 'Tagged packes' that send
  from the traffic generator to the PP */
static GT_U16 etherTypeForVlanTag = TGF_ETHERTYPE_8100_VLAN_TAG_CNS;

#ifdef CHX_FAMILY
    /* PVID per port struct typedef */
    typedef struct
    {
        GT_U16 pvidArr[PRV_CPSS_MAX_PP_PORTS_NUM_CNS];
        GT_U32 refCounter;
    } PRV_TGF_DXCH_PVID_PER_PORT_STC;

    /* global PVID DB per device, per port */
    static PRV_TGF_DXCH_PVID_PER_PORT_STC *prvTgfDxChGlobalPvidDatabaseArr[PRV_CPSS_MAX_PP_DEVICES_CNS] = { NULL };
#endif /* CHX_FAMILY */

/******************************************************************************\
 *                              API implementation                            *
\******************************************************************************/
static GT_STATUS autoChecksumField
(
    IN TGF_AUTO_CHECKSUM_INFO_STC *checksumInfoPtr,
    IN TGF_AUTO_CHECKSUM_EXTRA_INFO_STC *checksumExtraInfoPtr
);

/*******************************************************************************
* tgfTrafficGeneratorCpuDeviceSet
*
* DESCRIPTION:
*       Set the CPU device though it we send traffic to the 'Loop back port'
*
* INPUTS:
*       cpuDevice - the CPU device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorCpuDeviceSet
(
    IN GT_U8    cpuDevice
)
{
    if (!PRV_CPSS_IS_DEV_EXISTS_MAC(cpuDevice))
    {
        return GT_BAD_PARAM;
    }

    prvTgfCpuDevNum = cpuDevice;

    /* avoid compiler warnings */
    TGF_PARAM_NOT_USED(prvTgfTrafficCapturePclId);
    TGF_PARAM_NOT_USED(prvTgfTrafficCapturePclRuleIndex);

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficGeneratorPortLoopbackModeEnableSet
*
* DESCRIPTION:
*       Set port in 'loopback' mode
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       enable           - enable / disable (loopback/no loopback)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortLoopbackModeEnableSet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN GT_BOOL                   enable
)
{
    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);
    TGF_PARAM_NOT_USED(enable);

    if(portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    {
        GT_STATUS   rc;
        GT_U32      tpidBmp = (enable == GT_TRUE) ? 0x3f : 0xff;

        /* Set ingress TPID select for TAG0 --> set to use only 6 TPID,
           to not recognize TPIDs that used only for egress between cascade/ring ports.
           this setting needed when :
           1. we capture traffic to the CPU.
           2. the port send traffic 'in loopback' and at the ingress it should
                not be recognize ethertypes that relate to 'egress'
        */


        if(GT_TRUE == prvTgfCommonIsDeviceForce(portInterfacePtr->devPort.devNum))
        {
            rc = prvTgfBrgVlanPortIngressTpidSet(portInterfacePtr->devPort.devNum,
                                                 portInterfacePtr->devPort.portNum,
                                                 CPSS_VLAN_ETHERTYPE0_E,
                                                 tpidBmp);
            if(rc != GT_OK)
            {
                return rc;
            }

            rc = prvTgfBrgVlanPortIngressTpidSet(portInterfacePtr->devPort.devNum,
                                                 portInterfacePtr->devPort.portNum,
                                                 CPSS_VLAN_ETHERTYPE1_E,
                                                 tpidBmp);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    return cpssDxChPortInternalLoopbackEnableSet(portInterfacePtr->devPort.devNum,
                                                 portInterfacePtr->devPort.portNum,
                                                 enable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    return cpssExMxPmPortInternalLoopbackEnableSet(portInterfacePtr->devPort.devNum,
                                                   portInterfacePtr->devPort.portNum,
                                                   enable);
#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    return cpssExMxPortInternalLoopbackEnableSet(portInterfacePtr->devPort.devNum,
                                                 portInterfacePtr->devPort.portNum,
                                                 enable);
#endif /* EX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY) */
}

/*******************************************************************************
* tgfTrafficGeneratorPortForceLinkUpEnableSet
*
* DESCRIPTION:
*       Set port in 'force linkup' mode
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       enable           - enable / disable (force/not force)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortForceLinkUpEnableSet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN GT_BOOL                   enable
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc;
    GT_BOOL linkUp;
    CPSS_PORT_SPEED_ENT  forceSpeed;
    GT_U8    devNum;
    GT_U8    portNum;
    CPSS_PORT_INTERFACE_MODE_ENT ifMode;
#endif

    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);
    TGF_PARAM_NOT_USED(enable);

    if(portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    devNum = portInterfacePtr->devPort.devNum;
    portNum = portInterfacePtr->devPort.portNum;
    if(enable == GT_TRUE)
    {
        /* set MAC in NORMAL state (not in RESET) */
        rc = cpssDxChPortMacResetStateSet(devNum, portNum, GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* get link status */
        rc = cpssDxChPortLinkStatusGet(devNum, portNum, &linkUp);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(linkUp == GT_FALSE)
        {
            /* link is down. Set port speed to be maximal
               according to interface mode.
               there is no 'other side' that the port can be 'auto negotiation'
               with. The speed and duplex should be forced too. */
            if(enable == GT_TRUE)
            {
                /* get interface mode */
                rc = cpssDxChPortInterfaceModeGet(devNum, portNum, &ifMode);
                if(rc != GT_OK)
                {
                    return rc;
                }

                /* do not change speed for other then in following switch case
                  interfaces.  port is already configured for proper speed. */
                forceSpeed = CPSS_PORT_SPEED_NA_E;

                switch (ifMode)
                {
                    case CPSS_PORT_INTERFACE_MODE_REDUCED_GMII_E:
                    case CPSS_PORT_INTERFACE_MODE_MII_E:
                    case CPSS_PORT_INTERFACE_MODE_SGMII_E:
                    case CPSS_PORT_INTERFACE_MODE_GMII_E:
                    case CPSS_PORT_INTERFACE_MODE_MII_PHY_E:
                    case CPSS_PORT_INTERFACE_MODE_100BASE_FX_E:
                    case CPSS_PORT_INTERFACE_MODE_QSGMII_E:
                        /* force speed to be common acceptable 100M */
                        forceSpeed = CPSS_PORT_SPEED_100_E;
                        break;
                    case CPSS_PORT_INTERFACE_MODE_XGMII_E:
                        /* NOTE: this section was added to support board of xcat-FE , with 4 XG ports.
                            the trunk tests uses port 27 , and without next configuration the
                            BM (buffer management) in the device was stuck */
                        rc = cpssDxChPortInterfaceModeSet(devNum, portNum,ifMode);
                        if(rc != GT_OK)
                        {
                            cpssOsPrintf("FAIL : cpssDxChPortInterfaceModeSet port[%d]\n",
                                portNum);
                            return rc;
                        }
                        /* set speed to be 10G */
                        rc = cpssDxChPortSpeedSet(devNum, portNum, CPSS_PORT_SPEED_10000_E);
                        if(rc != GT_OK)
                        {
                            cpssOsPrintf("FAIL : cpssDxChPortSpeedSet port[%d]\n",
                                portNum);
                            return rc;
                        }
                        /* SERDES, XPCS, MAC configuration */
                        rc = cpssDxChPortSerdesPowerStatusSet(devNum, portNum,
                                                              CPSS_PORT_DIRECTION_BOTH_E,
                                                              0xF, GT_TRUE);
                        if(rc != GT_OK)
                        {
                            cpssOsPrintf("FAIL : cpssDxChPortSerdesPowerStatusSet port[%d]\n",
                                portNum);
                            return rc;
                        }
                        break;
                    case CPSS_PORT_INTERFACE_MODE_NA_E:
                        /* set interface for Lion odd ports */
                        if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
                        {
                            if (portNum & 1)
                            {
                                /* set port to be local XGMII */
                                rc = cpssDxChPortInterfaceModeSet(devNum, portNum,
                                                                  CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E);
                                if(rc != GT_OK)
                                {
                                    cpssOsPrintf("FAIL : cpssDxChPortInterfaceModeSet -Local XGMII port[%d]\n",
                                        portNum);
                                    return rc;
                                }

                                /* set speed to be 10G */
                                rc = cpssDxChPortSpeedSet(devNum, portNum, CPSS_PORT_SPEED_10000_E);
                                if(rc != GT_OK)
                                {
                                    cpssOsPrintf("FAIL : cpssDxChPortSpeedSet -Local XGMII port[%d]\n",
                                        portNum);
                                    return rc;
                                }

                                /* SERDES, XPCS, MAC configuration */
                                rc = cpssDxChPortSerdesPowerStatusSet(devNum, portNum,
                                                                      CPSS_PORT_DIRECTION_BOTH_E,
                                                                      0xF, GT_TRUE);
                                if(rc != GT_OK)
                                {
                                    cpssOsPrintf("FAIL : cpssDxChPortSerdesPowerStatusSet -Local XGMII port[%d]\n",
                                        portNum);
                                    return rc;
                                }
                            }
                        }
                        break;
                    default:
                        break;
                }

                if (forceSpeed != CPSS_PORT_SPEED_NA_E)
                {
                    /* set the port to not be 'auto negotiation' */
                    rc = cpssDxChPortSpeedAutoNegEnableSet(devNum, portNum, GT_FALSE);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintf("FAIL : cpssDxChPortSpeedAutoNegEnableSet - port[%d]\n",
                            portNum);
                        return rc;
                    }

                    /* set port speed */
                    rc = cpssDxChPortSpeedSet(devNum, portNum, forceSpeed);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintf("FAIL : cpssDxChPortSpeedSet - port[%d]\n",
                            portNum);
                        return rc;
                    }


                    /* set the port to not be 'auto negotiation' */
                    rc = cpssDxChPortDuplexAutoNegEnableSet(devNum, portNum, GT_FALSE);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintf("FAIL : cpssDxChPortDuplexAutoNegEnableSet - port[%d]\n",
                            portNum);
                        return rc;
                    }

                    /* set port to be full duplex */
                    rc = cpssDxChPortDuplexModeSet(devNum, portNum, CPSS_PORT_FULL_DUPLEX_E);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintf("FAIL : cpssDxChPortDuplexModeSet - port[%d]\n",
                            portNum);
                        return rc;
                    }
                }
            }
        }
    }


    return cpssDxChPortForceLinkPassEnableSet(portInterfacePtr->devPort.devNum,
                                              portInterfacePtr->devPort.portNum,
                                              enable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    return cpssExMxPmPortForceLinkUpSet(portInterfacePtr->devPort.devNum,
                                        portInterfacePtr->devPort.portNum,
                                        enable);
#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    return cpssExMxPortForceLinkPassEnable(portInterfacePtr->devPort.devNum,
                                           portInterfacePtr->devPort.portNum,
                                           enable);
#endif /* EX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY) */
}

/*******************************************************************************
* tgfTrafficGeneratorPortCountersEthReset
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
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortCountersEthReset
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

    /* avoid compiler warning */
    dummyCounters.goodOctetsRcv.c[0] = 0;

#ifdef CHX_FAMILY
    return cpssDxChPortMacCountersOnPortGet(portInterfacePtr->devPort.devNum,
                                            portInterfacePtr->devPort.portNum,
                                            &dummyCounters);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    return  cpssExMxPmPortCapturedMacCountersGet(portInterfacePtr->devPort.devNum,
                                                 portInterfacePtr->devPort.portNum,
                                                 &dummyCounters);
#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    return cpssExMxPortMacCountersOnPortGet(portInterfacePtr->devPort.devNum,
                                            portInterfacePtr->devPort.portNum,
                                            &dummyCounters);
#endif /* EX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY) */
}

/*******************************************************************************
* tgfTrafficGeneratorPortCountersEthRead
*
* DESCRIPTION:
*       Read the traffic generator counters on the port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*
* OUTPUTS:
*       countersPtr - (pointer to) the counters of port
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortCountersEthRead
(
    IN  CPSS_INTERFACE_INFO_STC        *portInterfacePtr,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC  *countersPtr
)
{
    GT_STATUS   rc = GT_OK;

    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);
    CPSS_NULL_PTR_CHECK_MAC(countersPtr);

    if(portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    rc = cpssDxChPortMacCountersOnPortGet(portInterfacePtr->devPort.devNum,
                                          portInterfacePtr->devPort.portNum,
                                          countersPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    rc = cpssExMxPmPortCapturedMacCountersGet(portInterfacePtr->devPort.devNum,
                                              portInterfacePtr->devPort.portNum,
                                              countersPtr);
#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    rc = cpssExMxPortMacCountersOnPortGet(portInterfacePtr->devPort.devNum,
                                          portInterfacePtr->devPort.portNum,
                                          countersPtr);
#endif /* EX_FAMILY */

    if(countersPtr->goodPktsRcv.l[0] == 0)
    {
        countersPtr->goodPktsRcv.l[0] = countersPtr->ucPktsRcv.l[0] + countersPtr->mcPktsRcv.l[0] + countersPtr->brdcPktsRcv.l[0];
        countersPtr->goodPktsRcv.l[1] = countersPtr->ucPktsRcv.l[1] + countersPtr->mcPktsRcv.l[1] + countersPtr->brdcPktsRcv.l[1];
    }

    if(countersPtr->goodPktsSent.l[0] == 0)
    {
        countersPtr->goodPktsSent.l[0] = countersPtr->ucPktsSent.l[0] + countersPtr->mcPktsSent.l[0] + countersPtr->brdcPktsSent.l[0];
        countersPtr->goodPktsSent.l[1] = countersPtr->ucPktsSent.l[1] + countersPtr->mcPktsSent.l[1] + countersPtr->brdcPktsSent.l[1];
    }

    return rc;
}

/*******************************************************************************
* prvTgfTrafficGeneratorNetIfSdmaSyncTxPacketSend
*
* DESCRIPTION:
*       Transmit the traffic to the port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       bufferPtr        - (pointer to) the buffer to send
*       bufferLength     - buffer length (include CRC bytes)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The packet will ingress to the PP after 'loopback' and will act as
*       it was received from a traffic generator connected to the port
*
*******************************************************************************/
static GT_STATUS prvTgfTrafficGeneratorNetIfSdmaSyncTxPacketSend
(
    IN CPSS_INTERFACE_INFO_STC          *portInterfacePtr,
    IN GT_U8                            *bufferPtr,
    IN GT_U32                            bufferLength
)
{
    GT_U8   devNum    = 0;
    GT_U32  numOfBufs = 0;
    GT_U32  vpt       = 0;
    GT_U32  cfiBit    = 0;
    GT_U32  vid       = 0;
    GT_BOOL packetIsTagged = GT_FALSE;

#ifdef CHX_FAMILY
    GT_STATUS   rc;
    CPSS_DXCH_NET_TX_PARAMS_STC       dxChPcktParams = {0};
    CPSS_DXCH_NET_DSA_FROM_CPU_STC    *fromCpuPtr    = NULL;
#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_NET_TX_PARAMS_STC     exMxPmPcktParams = {0};
    CPSS_EXMXPM_NET_DSA_FROM_CPU_STC  *fromCpuPtr      = NULL;
    GT_U8                             dummyCookie      = 0;
#endif /* EXMXPM_FAMILY */
#ifdef EX_FAMILY
    CPSS_EXMX_NET_TX_PARAMS_STC       exMxPcktParams = {0};
#endif /* EX_FAMILY */

    GT_U8   *buffList[1];
    GT_U32  buffLenList[1];


    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    buffList[0]    = bufferPtr;
    buffLenList[0] = bufferLength;

    numOfBufs = 1;
    devNum    = prvTgfCpuDevNum;

    if ((bufferPtr[12] == (GT_U8)(etherTypeForVlanTag >> 8)) &&
        (bufferPtr[13] == (GT_U8)(etherTypeForVlanTag & 0xFF)))
    {
        packetIsTagged = GT_TRUE;
        vpt    = (GT_U8) (bufferPtr[14] >> 5);
        cfiBit = (GT_U8) ((bufferPtr[14] >> 4) & 1);
        vid    = ((bufferPtr[14] & 0xF) << 8) | bufferPtr[15];
    }
    else
    {
        packetIsTagged = GT_FALSE;
        vpt    = 0;
        cfiBit = 0;
        vid    = 4094;
    }

#ifdef CHX_FAMILY
    dxChPcktParams.cookie = NULL;
    dxChPcktParams.packetIsTagged = packetIsTagged;

    dxChPcktParams.sdmaInfo.recalcCrc = GT_TRUE;
    dxChPcktParams.sdmaInfo.txQueue   = 7;
    dxChPcktParams.sdmaInfo.evReqHndl = 0;
    dxChPcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_TRUE;

    dxChPcktParams.dsaParam.commonParams.dsaTagType     = CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E;
    dxChPcktParams.dsaParam.commonParams.vpt            = (GT_U8) vpt;
    dxChPcktParams.dsaParam.commonParams.cfiBit         = (GT_U8) cfiBit;
    dxChPcktParams.dsaParam.commonParams.vid            = (GT_U16) vid;
    dxChPcktParams.dsaParam.commonParams.dropOnSource   = GT_FALSE;
    dxChPcktParams.dsaParam.commonParams.packetIsLooped = GT_FALSE;

    dxChPcktParams.dsaParam.dsaType = CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E;

    fromCpuPtr = &dxChPcktParams.dsaParam.dsaInfo.fromCpu;

    fromCpuPtr->dstInterface        = *portInterfacePtr;
    if( CPSS_INTERFACE_PORT_E == portInterfacePtr->type )
    {
        rc = prvUtfHwFromSwDeviceNumberGet(portInterfacePtr->devPort.devNum,
                                           &(fromCpuPtr->dstInterface.devPort.devNum));
        if(GT_OK != rc)
            return rc;
    }
    fromCpuPtr->tc                  = 7;
    fromCpuPtr->dp                  = CPSS_DP_GREEN_E;
    fromCpuPtr->egrFilterEn         = GT_FALSE;
    fromCpuPtr->cascadeControl      = GT_TRUE;
    fromCpuPtr->egrFilterRegistered = GT_FALSE;
    fromCpuPtr->srcId               = 0;
    rc = prvUtfHwFromSwDeviceNumberGet(devNum, &(fromCpuPtr->srcDev));
    if(GT_OK != rc)
        return rc;
    fromCpuPtr->extDestInfo.devPort.dstIsTagged = dxChPcktParams.packetIsTagged;
    fromCpuPtr->extDestInfo.devPort.mailBoxToNeighborCPU = GT_FALSE;

    return cpssDxChNetIfSdmaSyncTxPacketSend(devNum, &dxChPcktParams, buffList, buffLenList, numOfBufs);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    exMxPmPcktParams.cookie = (GT_PTR) &dummyCookie;
    exMxPmPcktParams.packetIsTagged = packetIsTagged;

    exMxPmPcktParams.sdmaInfo.recalcCrc = GT_TRUE;
    exMxPmPcktParams.sdmaInfo.txQueue   = 7;
    exMxPmPcktParams.sdmaInfo.evReqHndl = 0;
    exMxPmPcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_TRUE;

    exMxPmPcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;
    exMxPmPcktParams.dsaParam.commonParams.vpt        = (GT_U8) vpt;
    exMxPmPcktParams.dsaParam.commonParams.cfiBit     = (GT_U8) cfiBit;
    exMxPmPcktParams.dsaParam.commonParams.vid        = (GT_U16) vid;

    exMxPmPcktParams.dsaParam.dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E;
    fromCpuPtr = &exMxPmPcktParams.dsaParam.dsaInfo.fromCpu;

    fromCpuPtr->dstInterface        = *portInterfacePtr;
    fromCpuPtr->tc                  = 7;
    fromCpuPtr->dp                  = CPSS_DP_GREEN_E;
    fromCpuPtr->egrFilterEn         = GT_FALSE;
    fromCpuPtr->egrFilterRegistered = GT_FALSE;
    fromCpuPtr->srcId               = 0;
    fromCpuPtr->isDit               = GT_FALSE;

    fromCpuPtr->dstIsTagged = exMxPmPcktParams.packetIsTagged;
    fromCpuPtr->extDestInfo.devPort.mailBoxToNeighborCPU = GT_FALSE;

    return cpssExMxPmNetIfSdmaTxSyncPacketSend(devNum, &exMxPmPcktParams, buffList, buffLenList, numOfBufs);
#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    exMxPcktParams.txParams.userPrioTag = 0;
    exMxPcktParams.txParams.tagged      = packetIsTagged;
    exMxPcktParams.txParams.packetEncap = CPSS_EXMX_NET_CONTROL_PCKT_E;
    exMxPcktParams.txParams.recalcCrc   = GT_TRUE;
    exMxPcktParams.txParams.vid         = (GT_U16) vid;
    exMxPcktParams.txParams.macDaType   = CPSS_EXMX_NET_UNICAST_MAC_E;
    exMxPcktParams.txParams.txQueue     = 7;
    exMxPcktParams.txParams.evReqHndl   = 0;
    exMxPcktParams.txParams.cookie      = NULL;
    exMxPcktParams.txParams.dropPrecedence = CPSS_DP_GREEN_E;

    exMxPcktParams.useVidx = GT_FALSE;

    exMxPcktParams.dest.devPort.tgtDev  = portInterfacePtr->devPort.devNum;
    exMxPcktParams.dest.devPort.tgtPort = portInterfacePtr->devPort.portNum;

    exMxPcktParams.addDsa = GT_FALSE;

    exMxPcktParams.invokeTxBufferQueueEvent = GT_TRUE;

    return cpssExMxNetIfSyncTxPacketSend(devNum, &exMxPcktParams, buffList, buffLenList, numOfBufs);
#endif /* EX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY) */
}

/*******************************************************************************
* prvTgfTrafficGeneratorPoolInit
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
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfTrafficGeneratorPoolInit
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;
    static GT_BOOL  poolCreated = GT_FALSE;

    if(GT_FALSE == poolCreated)
    {
        /* create pool of buffers from cache */
        rc = gtPoolCreateDmaPool(TGF_TX_BUFFER_MAX_SIZE_CNS,
                                 GT_4_BYTE_ALIGNMENT,
                                 TGF_MAX_NUM_OF_BUFFERS_AT_POOL,
                                 GT_TRUE,
                                 &prvTgfTxBuffersPoolId);
        if (GT_OK != rc)
        {
            return rc;
        }

        poolCreated = GT_TRUE;
    }

    return rc;
}

/*******************************************************************************
* tgfTrafficGeneratorPortTxEthTransmit
*
* DESCRIPTION:
*       Transmit the traffic to the port
*       NOTE:
*           1. allow to transmit with chunks of burst:
*              after x sent frames --> do sleep of y millisecond
*              see parameters
*
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       packetInfoPtr    - (pointer to) the packet info
*       burstCount       - number of frames (non-zero value)
*       numVfd           - number of VFDs
*       vfdArray         - pointer to array of VFDs (may be NULL if numVfd = 0)
*       sleepAfterXCount - do 'sleep' after X packets sent
*                          when = 0 , meaning NO SLEEP needed during the burst
*                          of 'burstCount'
*       sleepTime        - sleep time (in milliseconds) after X packets sent , see
*                          parameter sleepAfterXCount
*       traceBurstCount  - number of packets in burst count that will be printed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The packet will ingress to the PP after 'loopback' and will act as
*       it was received from a traffic generator connected to the port
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortTxEthTransmit
(
    IN CPSS_INTERFACE_INFO_STC         *portInterfacePtr,
    IN TGF_PACKET_STC                  *packetInfoPtr,
    IN GT_U32                           burstCount,
    IN GT_U32                           numVfd,
    IN TGF_VFD_INFO_STC                 vfdArray[],
    IN GT_U32                           sleepAfterXCount,
    IN GT_U32                           sleepTime,
    IN GT_U32                           traceBurstCount
)
{
    GT_STATUS   rc, rc1    = GT_OK;
    GT_U32      i, j;
    GT_U32      loops;
    GT_U8       *bufferPtr = NULL;
    GT_U32      xCounter   = sleepAfterXCount;/* counter of already sent packets ,
                            for sleep when xCounter = 0 ,
                            relevant only when sleepAfterXCount != 0*/
    GT_BOOL     hugeSend = GT_FALSE;/* is the burst 'huge'*/
    GT_U32      maxRetry = 5;  /* max retry number on error */
    GT_BOOL     doProgressIndication = GT_FALSE;
    GT_BOOL     avoidKeepAlive = GT_FALSE;/* avoid keep alive print */
    TGF_AUTO_CHECKSUM_INFO_STC checksumInfoArr[TGF_AUTO_CHECKSUM_FIELD_LAST_E];/* auto checksum info */
    TGF_AUTO_CHECKSUM_EXTRA_INFO_STC extraChecksumInfo;/* extra checksum info */
    GT_BOOL     useMii;/* do we use MII or SDMA */

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

#ifdef EXMXPM_FAMILY
    useMii = GT_FALSE;
#elif defined CHX_FAMILY
    useMii =((PRV_CPSS_PP_MAC(prvTgfCpuDevNum)->cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_MII_E) ? GT_TRUE:
             (PRV_CPSS_PP_MAC(prvTgfCpuDevNum)->cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_NONE_E) ? GT_FALSE:
             PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(prvTgfCpuDevNum) ? GT_TRUE : GT_FALSE);
#else
    useMii = GT_FALSE;
#endif

    if(useMii == GT_TRUE)
    {
#ifdef _WIN32
        if(burstCount >= 16 && sleepAfterXCount == 0)
        {
            sleepAfterXCount = miiCounter;
            sleepTime = 10;
            avoidKeepAlive = GT_TRUE;
        }
#endif /*_WIN32*/

        xCounter = sleepAfterXCount;
    }


    cpssOsMemSet(checksumInfoArr,0,sizeof(checksumInfoArr));

    /* init the pool */
    rc = prvTgfTrafficGeneratorPoolInit();
    if(rc != GT_OK)
    {
        return rc;
    }

    if(portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

    if((packetInfoPtr->totalLen + TGF_CRC_LEN_CNS) > TGF_TX_BUFFER_MAX_SIZE_CNS)
    {
        return GT_BAD_PARAM;
    }

    /* get buffer from the pool */
    bufferPtr = gtPoolGetBuf(prvTgfTxBuffersPoolId);
    if(NULL == bufferPtr)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* build the basic frame info */
    rc = tgfTrafficEnginePacketBuild(packetInfoPtr, bufferPtr , checksumInfoArr,&extraChecksumInfo);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

    hugeSend = (burstCount > traceBurstCount) ? GT_TRUE : GT_FALSE;

    if(hugeSend == GT_TRUE)
    {
        /* we can't allow printings for huge number of frames */
        rc = tgfTrafficTracePacketByteSet(GT_FALSE);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }
    }


    if(hugeSend == GT_TRUE)
    {
#ifdef _WIN32
        if(burstCount > 1000)
        {
            cpssOsPrintf("utf Transmit: Huge burst[%d] to dev [%d] port[%d] \n",
                burstCount,
                portInterfacePtr->devPort.devNum,
                portInterfacePtr->devPort.portNum);
        }
#endif /*_WIN32*/
        doProgressIndication = GT_TRUE;
    }

    for (i = 0; i < burstCount; i++)
    {
        for(j = 0; j < numVfd; j++)
        {
            vfdArray[j].modeExtraInfo = i;

            /* over ride the buffer with VFD info */
            rc = tgfTrafficEnginePacketVfdApply(&vfdArray[j], bufferPtr, packetInfoPtr->totalLen);
            if(rc != GT_OK)
            {
                goto exit_cleanly_lbl;
            }
        }

        /* apply auto checksum fields after applying the VFD*/
        rc = autoChecksumField(checksumInfoArr,&extraChecksumInfo);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }

        /* add the ability to trace the packet content */
        rc = tgfTrafficTracePacket(bufferPtr,
                                   packetInfoPtr->totalLen,
                                   GT_FALSE);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }

        if(sleepAfterXCount && ((--xCounter) == 0))
        {
            /* allow the 'sleep' after X packets sent , to let AppDemo task to
               process AUQ */
            if(avoidKeepAlive == GT_FALSE)
            {
                utfPrintKeepAlive();
            }

            cpssOsTimerWkAfter(sleepTime);
            xCounter   = sleepAfterXCount;
        }

        if(doProgressIndication == GT_TRUE)
        {
            if(4999 == (i % 5000))
            {
#ifdef _WIN32
                cpssOsPrintf("sent [%d]",i+1);
#endif /*_WIN32*/
            }
            else if(1999 == (i % 2000))
            {
                utfPrintKeepAlive();
            }
        }
retry_send_lbl:
        /* send the buffer to the device,port */
        rc = prvTgfTrafficGeneratorNetIfSdmaSyncTxPacketSend(portInterfacePtr,
                                                             bufferPtr,
                                                             packetInfoPtr->totalLen + TGF_CRC_LEN_CNS);
        if(rc != GT_OK)
        {
            if(hugeSend == GT_TRUE)
            {
                cpssOsTimerWkAfter(sleepTime);
                /* retry to send again ! */
                if(--maxRetry)
                {
                    goto retry_send_lbl;
                }
            }

            goto exit_cleanly_lbl;
        }

        maxRetry = 5;/* restore the 5 retries per frame */
    }

    if(sleepAfterXCount)
    {
        /* allow appDemo process the last section of the burst */
        cpssOsTimerWkAfter(sleepTime);
    }


#ifdef _WIN32
    if(hugeSend == GT_TRUE && burstCount > 1000)
    {
        cpssOsPrintf(" : Huge burst Ended \n");
    }
#endif /*_WIN32*/

    /* let the packets be sent */
#ifdef  EMBEDDED
    /* run on hardware */
    loops = prvTgfTimeLoop;
#else
    /* run on simulation */
    loops = prvTgfTimeLoop;
#endif
    while(loops--)/* allow debugging by set this value = 1000 */
    {
        osTimerWkAfter(10);
    }

exit_cleanly_lbl:
    if(hugeSend == GT_TRUE)
    {
        /* restore value */
        rc1 = tgfTrafficTracePacketByteSet(GT_TRUE);
    }

    if(bufferPtr)
    {
        /* release the buffer back to the pool */
        rc1 = gtPoolFreeBuf(prvTgfTxBuffersPoolId, bufferPtr);
    }

    rc = rc1 != GT_OK ? rc1 : rc;

    return rc;
}

/*******************************************************************************
* tgfTrafficGeneratorRxInCpuGet
*
* DESCRIPTION:
*       Get entry from rxNetworkIf table
*
* INPUTS:
*       packetType      - the packet type to get
*       getFirst        - get first/next entry
*       trace           - enable\disable packet tracing
*       packetBufLenPtr - the length of the user space for the packet
*
* OUTPUTS:
*       packetBufPtr    - packet's buffer (pre allocated by the user)
*       packetBufLenPtr - length of the copied packet to gtBuf
*       packetLenPtr    - Rx packet original length
*       devNumPtr       - packet's device number
*       queuePtr        - Rx queue in which the packet was received.
*       rxParamsPtr     - specific device Rx info format.
*
* RETURNS:
*       GT_OK      - on success
*       GT_NO_MORE - on more entries
*       GT_BAD_PTR - on NULL pointer
*
* COMMENTS:
*       For 'captured' see API tgfTrafficGeneratorPortTxEthCaptureSet
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorRxInCpuGet
(
    IN    TGF_PACKET_TYPE_ENT   packetType,
    IN    GT_BOOL               getFirst,
    IN    GT_BOOL               trace,
    OUT   GT_U8                *packetBufPtr,
    INOUT GT_U32               *packetBufLenPtr,
    OUT   GT_U32               *packetLenPtr,
    OUT   GT_U8                *devNumPtr,
    OUT   GT_U8                *queuePtr,
    OUT   TGF_NET_DSA_STC      *rxParamsPtr
)
{
    TGF_PACKET_TYPE_ENT currPacketType = TGF_PACKET_TYPE_REGULAR_E;
    static GT_U32       currentIndex   = 0;
    GT_STATUS           rc             = GT_OK;
    GT_BOOL             newGetFirst    = getFirst;
    GT_8                specificDeviceFormat[TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS] = {0};
#ifdef CHX_FAMILY
    CPSS_DXCH_NET_RX_PARAMS_STC   *dxChPcktParamsPtr   = NULL;
#endif
#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_NET_RX_PARAMS_STC *exMxPmPcktParamsPtr = NULL;
#endif
#ifdef EX_FAMILY
    CPSS_EXMX_NET_RX_PARAMS_STC   *exMxPcktParamsPtr   = NULL;
#endif

    CPSS_NULL_PTR_CHECK_MAC(packetBufPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetBufLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(devNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(queuePtr);
    CPSS_NULL_PTR_CHECK_MAC(rxParamsPtr);

start_lbl:
    currPacketType = TGF_PACKET_TYPE_REGULAR_E;

    if(newGetFirst == GT_TRUE)
    {
        currentIndex = 0;
    }
    else
    {
        currentIndex++;
    }

    newGetFirst = GT_FALSE;

    rc = tgfTrafficTableRxPcktGet(currentIndex, packetBufPtr, packetBufLenPtr, packetLenPtr,
                                  devNumPtr, queuePtr, specificDeviceFormat);
    if(GT_OUT_OF_RANGE == rc)
    {
        /* table is done */
        return GT_NO_MORE;
    }
    else if(rc == GT_NOT_FOUND)
    {
        /* current index has not valid entry, but table is not done yet */
        goto start_lbl;
    }
    else if(rc != GT_OK)
    {
        return rc;
    }

#ifdef CHX_FAMILY
    dxChPcktParamsPtr = (CPSS_DXCH_NET_RX_PARAMS_STC*) specificDeviceFormat;

    if(CPSS_DXCH_NET_DSA_CMD_TO_CPU_E == dxChPcktParamsPtr->dsaParam.dsaType)
    {
        rxParamsPtr->dsaCmdIsToCpu = GT_TRUE;
        rxParamsPtr->cpuCode       = dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.cpuCode;
        rxParamsPtr->srcIsTrunk    = dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.srcIsTrunk;
        rc = prvUtfSwFromHwDeviceNumberGet(dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.devNum,
                                           &(rxParamsPtr->devNum));
        if(GT_OK != rc)
            return rc;
        rxParamsPtr->portNum       = dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.interface.portNum;
        rxParamsPtr->originByteCount = dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.originByteCount;

        if((dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.cpuCode == CPSS_NET_LAST_USER_DEFINED_E)||
           (dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.cpuCode == CPSS_NET_INGRESS_MIRRORED_TO_ANLYZER_E))
        {
            /* 'Captured' packet */
            currPacketType = TGF_PACKET_TYPE_CAPTURE_E;
        }
    }
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    exMxPmPcktParamsPtr = (CPSS_EXMXPM_NET_RX_PARAMS_STC*) specificDeviceFormat;

    if(CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E == exMxPmPcktParamsPtr->dsaParam.dsaCmd)
    {
        rxParamsPtr->dsaCmdIsToCpu = GT_TRUE;
        rxParamsPtr->cpuCode       = exMxPmPcktParamsPtr->dsaParam.dsaInfo.toCpu.cpuCode;
        rxParamsPtr->srcIsTrunk    = !(exMxPmPcktParamsPtr->dsaParam.dsaInfo.toCpu.isEgressPipe);
        rxParamsPtr->devNum        = exMxPmPcktParamsPtr->dsaParam.dsaInfo.toCpu.devNum;
        rxParamsPtr->portNum       = exMxPmPcktParamsPtr->dsaParam.dsaInfo.toCpu.portNum;

        if(exMxPmPcktParamsPtr->dsaParam.dsaInfo.toCpu.cpuCode == CPSS_NET_LAST_USER_DEFINED_E)
        {
            /* 'Captured' packet */
            currPacketType = TGF_PACKET_TYPE_CAPTURE_E;
        }
    }
#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    exMxPcktParamsPtr = (CPSS_EXMX_NET_RX_PARAMS_STC*) specificDeviceFormat;

    rxParamsPtr->dsaCmdIsToCpu = GT_TRUE;
    rxParamsPtr->cpuCode       = exMxPcktParamsPtr->cpuCode;
    rxParamsPtr->srcIsTrunk    = GT_FALSE;
    rxParamsPtr->devNum        = exMxPcktParamsPtr->srcDevNum;
    rxParamsPtr->portNum       = exMxPcktParamsPtr->srcPortNum;

    if(exMxPcktParamsPtr->cpuCode == CPSS_NET_LAST_USER_DEFINED_E)
    {
        /* 'Captured' packet */
        currPacketType = TGF_PACKET_TYPE_CAPTURE_E;
    }
#endif /* EX_FAMILY */

    if(currPacketType != packetType)
    {
        /* not matching packet type, so try to get next packet */
        goto start_lbl;
    }

    /* add the ability to trace the packet content */
    if (GT_TRUE == trace)
    {
        rc = tgfTrafficTracePacket(packetBufPtr, (*packetLenPtr), GT_TRUE);
        if (GT_OK != rc)
        {
            return rc;
        }
    }
    
    /* add ability to trace the packet parts fields */
    CPSS_TBD_BOOKMARK

    /* add ability to trace the Rx specific device parameters */
    CPSS_TBD_BOOKMARK

    return rc;
}

/*******************************************************************************
* tgfTrafficGeneratorPortTxEthTriggerCountersGet
*
* DESCRIPTION:
*       Get number of triggers on port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       numVfd           - number of VFDs
*       vfdArray         - pointer to array of VFDs
*
* OUTPUTS:
*       numTriggersBmpPtr - (pointer to) bitmap of triggers
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The 'captured' (emulation of capture) must be set on this port
*       see tgfTrafficGeneratorPortTxEthCaptureSet
*
*       Triggers bitmap has the following structure:
*           N - number of captured packets on this port
*           K - number of VFDs (equal to numVfd)
*           |      Packet0     |      Packet1     |     |       PacketN    |
*           +----+----+---+----+----+----+---+----+ ... +----+----+---+----+
*           |Bit0|Bit1|...|BitK|Bit0|Bit1|...|BitK|     |Bit0|Bit1|...|BitK|
*           +----+----+---+----+----+----+---+----+     +----+----+---+----+
*       Bit[i] for Packet[j] set to 1 means that VFD[i] is matched for captured
*       packet number j.
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortTxEthTriggerCountersGet
(
    IN  CPSS_INTERFACE_INFO_STC         *portInterfacePtr,
    IN  GT_U32                           numVfd,
    IN  TGF_VFD_INFO_STC                 vfdArray[],
    OUT GT_U32                          *numTriggersBmpPtr
)
{
    GT_U8           devNum   = 0;
    GT_U8           queue    = 0;
    GT_BOOL         getFirst = GT_TRUE;
    GT_U32          i        = 0;
    GT_BOOL         match    = GT_FALSE;
    GT_STATUS       rc, rc1  = GT_OK;
    GT_U32          buffLen  = TGF_RX_BUFFER_MAX_SIZE_CNS;
    GT_U32          packetActualLength = 0;
    GT_U32          packetCount = 0;
    GT_U8           packetBuff[TGF_RX_BUFFER_MAX_SIZE_CNS] = {0};
    TGF_NET_DSA_STC rxParam = {0};


    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);
    CPSS_NULL_PTR_CHECK_MAC(vfdArray);
    CPSS_NULL_PTR_CHECK_MAC(numTriggersBmpPtr);

    if (numVfd == 0)
    {
        return GT_BAD_PARAM;
    }

    if (portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

    (*numTriggersBmpPtr) = 0;

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
                                           &devNum, &queue, &rxParam);
        if (rc != GT_OK)
        {
            break;
        }

        getFirst = GT_FALSE; /* now we get the next */

        /* check the DSA tag */
        if (GT_FALSE == rxParam.dsaCmdIsToCpu || GT_TRUE == rxParam.srcIsTrunk ||
           rxParam.devNum != portInterfacePtr->devPort.devNum ||
           rxParam.portNum != portInterfacePtr->devPort.portNum)
        {
            /* the packet NOT came from where I expected it to be */
            continue;
        }

        /* calculate number of captured packets */
        packetCount++;

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

        /* check the VFD parameters */
        for (i = 0; i < numVfd ;i++)
        {
            rc = tgfTrafficEnginePacketVfdCheck(&vfdArray[i], packetBuff, buffLen, &match);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* set appropriate bit to 1 if VFD is matched */
            *numTriggersBmpPtr |= ((GT_U32) match) << (i + (packetCount - 1) * numVfd);
        }
    }

    /* enable packet trace */
    rc1 = tgfTrafficTracePacketByteSet(GT_TRUE);
    if (GT_OK != rc1)
    {
        PRV_UTF_LOG1_MAC("[TGF]: tgfTrafficTracePacketByteSet FAILED, rc = [%d]", rc1);
    }

    return (GT_OK == rc1) ? rc : rc1;
}

/*******************************************************************************
* tgfTrafficGeneratorTxEthTriggerCheck
*
* DESCRIPTION:
*       Check if the buffer triggered by the trigger parameters
*
* INPUTS:
*       bufferPtr    - (pointer to) the buffer
*       bufferLength - length of the buffer
*       numVfd       - number of VFDs
*       vfdArray     - array of VFDs
* OUTPUTS:
*       triggeredPtr - (pointer to) triggered / not triggered
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The 'captured' (emulation of capture) must be set on this port see
*       tgfTrafficGeneratorPortTxEthCaptureSet, tgfTrafficGeneratorRxInCpuGet
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorTxEthTriggerCheck
(
    IN  GT_U8                           *bufferPtr,
    IN  GT_U32                           bufferLength,
    IN  GT_U32                           numVfd,
    IN  TGF_VFD_INFO_STC                 vfdArray[],
    OUT GT_BOOL                         *triggeredPtr
)
{
    GT_BOOL     match = GT_FALSE;
    GT_U32      i     = 0;
    GT_STATUS   rc    = GT_OK;

    CPSS_NULL_PTR_CHECK_MAC(vfdArray);
    CPSS_NULL_PTR_CHECK_MAC(triggeredPtr);

    if(numVfd == 0)
    {
        return GT_BAD_PARAM;
    }

    *triggeredPtr = GT_FALSE;

    /* check the VFD parameters */
    for(i = 0; i < numVfd ;i++)
    {
        rc = tgfTrafficEnginePacketVfdCheck(&vfdArray[i], bufferPtr, bufferLength, &match);
        if (rc != GT_OK)
        {
            return rc;
        }

        if(match != GT_TRUE)
        {
            break;
        }
    }

    if(i == numVfd)
    {
        *triggeredPtr = GT_TRUE;
    }

    return rc;
}

#ifdef CHX_FAMILY
/*******************************************************************************
* prvTgfDxChCaptureSet
*
* DESCRIPTION:
*       Start/Stop capture the traffic that egress the port
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       start   - start/stop capture
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The Start 'Capture' is emulated by setting next for the port:
*           1. ingress PCL rule for ALL traffic from this port to TRAP to the CPU
*              we will assign it with 'Application specific cpu code' to
*              distinguish from other traffic go to the CPU
*
*       The Stop 'Capture' is emulated by setting next for the port:
*           1. remove the ingress PCL rule that function
*              tgfTrafficGeneratorPortTxEthCaptureStart added
*
*******************************************************************************/
static GT_STATUS prvTgfDxChPclCaptureSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  start
)
{
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    GT_U32                             ruleIndex  = 0;
    GT_STATUS                          rc         = GT_OK;
    static GT_U32                      first      = 1;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      mask;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      pattern;
    CPSS_DXCH_PCL_ACTION_STC           action;
    CPSS_INTERFACE_INFO_STC            portInterface;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC       lookupCfg;

    if(GT_TRUE == start)
    {
        if(first)
        {
            TGF_RESET_PARAM_MAC(mask);
            TGF_RESET_PARAM_MAC(pattern);
            TGF_RESET_PARAM_MAC(action);

            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
            ruleIndex  = TGF_PCL_RULE_INDEX_CNS;

            /* need to match Only the PCL id */
            mask.ruleStdNotIp.common.pclId    = 0xFFFF;/*exact match on this field */
            pattern.ruleStdNotIp.common.pclId = TGF_CAPTURE_PCL_ID_CNS;

            /* trap to CPU */
            action.pktCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

            /* set specific CPU code for those 'captured' packets */
            action.mirror.cpuCode = CPSS_NET_LAST_USER_DEFINED_E;

            /* add the rule to the PCL */
            rc = cpssDxChPclRuleSet(
                devNum, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);
            if(rc != GT_OK)
            {
                return rc;
            }

            rc = cpssDxChPclPortLookupCfgTabAccessModeSet(
                    devNum, portNum,
                    CPSS_PCL_DIRECTION_INGRESS_E,
                    CPSS_PCL_LOOKUP_0_E,
                    0,
                    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E);
            if(rc != GT_OK)
            {
                return rc;
            }

            first = 0;
        }
    }
    else
    {
        /* invalidate PCL rule */
        rc = cpssDxChPclRuleValidStatusSet(devNum, CPSS_PCL_RULE_SIZE_STD_E,
                                           TGF_PCL_RULE_INDEX_CNS, GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }

        first = 1;
    }

    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.portNum = portNum;
    portInterface.devPort.devNum  = devNum;

    TGF_RESET_PARAM_MAC(lookupCfg);

    lookupCfg.pclId        = TGF_CAPTURE_PCL_ID_CNS;
    lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    lookupCfg.groupKeyTypes.ipv4Key  = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
    lookupCfg.groupKeyTypes.ipv6Key  = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
    /* start - enable lookup, stop - disable lookup */
    lookupCfg.enableLookup = start;

    rc = prvUtfHwFromSwDeviceNumberGet(portInterface.devPort.devNum,
                                       &(portInterface.devPort.devNum));
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = cpssDxChPclCfgTblSet(devNum, &portInterface,
                              CPSS_PCL_DIRECTION_INGRESS_E,
                              CPSS_PCL_LOOKUP_0_E,
                              &lookupCfg);
    if (GT_OK != rc)
    {
        return rc;
    }


    rc = cpssDxChPclPortIngressPolicyEnable(devNum, portNum, start);

    return rc;
}

/*******************************************************************************
* prvTgfDxChMirroringCaptureSet
*
* DESCRIPTION:
*       Start/Stop capture the traffic that egress the port.
*       Temporary function for DxChXcat A1 that uses Mirroring for packet
*       capture, while PCL development in progress.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       start   - start/stop capture
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The Start 'Capture' is emulated by setting next for the port:
*           1. Enable mirroring for this port.
*           2. Drop all packets by forcing PVID = 0.
*           3. Set CPU port as analyzer port.
*
*       The Stop 'Capture' is emulated by setting next for the port:
*           1. Disable mirroring for this port.
*           2. Disable Force PVID.
*
*******************************************************************************/
static GT_STATUS prvTgfDxChMirroringCaptureSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  start
)
{
    GT_STATUS       rc = GT_OK, rc1;
    GT_U16          hwValue;
    CPSS_PP_FAMILY_TYPE_ENT  devFamily;
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC analyzerInf;
    GT_U8           hwDevNum;

    /*******************************/
    /* Configure Mirroring to CPU  */
    /*******************************/

    if(GT_TRUE == start)
    {
        /* Allocate memory only when capturing is enabled for the first port */
        if (prvTgfDxChGlobalPvidDatabaseArr[devNum] == NULL)
        {
            /* Allocate memory */
            prvTgfDxChGlobalPvidDatabaseArr[devNum] = (PRV_TGF_DXCH_PVID_PER_PORT_STC*)
                                   cpssOsMalloc(sizeof(PRV_TGF_DXCH_PVID_PER_PORT_STC));

            /* Set default value for the reference */
            prvTgfDxChGlobalPvidDatabaseArr[devNum]->refCounter = 0;
            prvTgfDxChGlobalPvidDatabaseArr[devNum]->pvidArr[portNum] = 0;
        }

        /* set force PVID */
        if (prvTgfDxChGlobalPvidDatabaseArr[devNum] != NULL)
        {
            /* add reference to a port */
            prvTgfDxChGlobalPvidDatabaseArr[devNum]->refCounter++;

            /* get current PVID */
            rc = cpssDxChBrgVlanPortVidGet(devNum,
                                           portNum,
                                           &hwValue);
            if (GT_OK != rc)
            {
                return rc;
            }

            /* store current PVID value */
            prvTgfDxChGlobalPvidDatabaseArr[devNum]->pvidArr[portNum] = hwValue;

            /* set PVID = 0 for the port */
            rc = cpssDxChBrgVlanPortVidSet(devNum,
                                           portNum,
                                           0);
            if (GT_OK != rc)
            {
                return rc;
            }

            /* set force PVID for all packets in the port */
            rc = cpssDxChBrgVlanForcePvidEnable(devNum,
                                                portNum,
                                                GT_TRUE);
            if (GT_OK != rc)
            {
                return rc;
            }
        }

        rc = prvUtfDeviceFamilyGet(devNum, &devFamily);
        if (GT_OK != rc)
        {
            return rc;
        }

        /* call device specific API */
        if (devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            rc = prvUtfHwFromSwDeviceNumberGet(devNum, &hwDevNum);
            if(GT_OK != rc)
                return rc;
            rc = cpssDxChMirrorRxAnalyzerPortSet(devNum,
                                                 63,
                                                 hwDevNum);
        }
        else
        {
            analyzerInf.interface.type = CPSS_INTERFACE_PORT_E;
            rc = prvUtfHwFromSwDeviceNumberGet(devNum,
                                               &(analyzerInf.interface.devPort.devNum));
            if(GT_OK != rc)
                return rc;
            analyzerInf.interface.devPort.portNum = 63;

            /* use interface 0 for Rx analyzer by default */
            rc = cpssDxChMirrorAnalyzerInterfaceSet(devNum, 0, &analyzerInf);
        }

        /* enable mirroring */
        rc = cpssDxChMirrorRxPortSet(devNum,
                                     portNum,
                                     GT_TRUE,
                                     0);
        if (GT_OK != rc)
        {
            return rc;
        }
    }
    else
    {
        /* disable mirroring */
        rc1 = cpssDxChMirrorRxPortSet(devNum,
                                     portNum,
                                     GT_FALSE,
                                     0);
        if (GT_OK != rc1)
        {
            rc = rc1;
        }

        /* restore PVID */
        if (prvTgfDxChGlobalPvidDatabaseArr[devNum] != NULL)
        {
            /* disable force PVID */
            rc1 = cpssDxChBrgVlanForcePvidEnable(devNum,
                                                portNum,
                                                GT_FALSE);
            if (GT_OK != rc1)
            {
                rc = rc1;
            }

            /* restore PVID */
            rc = cpssDxChBrgVlanPortVidSet(devNum,
                                           portNum,
                                           prvTgfDxChGlobalPvidDatabaseArr[devNum]->pvidArr[portNum]);
            if (GT_OK != rc1)
            {
                rc = rc1;
            }

            /* delete reference to a port */
            prvTgfDxChGlobalPvidDatabaseArr[devNum]->refCounter--;

            /* Free memory only when capturing is disabled for the last port */
            if (prvTgfDxChGlobalPvidDatabaseArr[devNum]->refCounter == 0)
            {
                cpssOsFree(prvTgfDxChGlobalPvidDatabaseArr[devNum]);
                prvTgfDxChGlobalPvidDatabaseArr[devNum] = NULL;
            }
        }
    }

    return rc;
}

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
/*******************************************************************************
* prvTgfExMxPmPclCaptureSet
*
* DESCRIPTION:
*       Start/Stop capture the traffic that egress the port
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       start   - start/stop capture
*
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
*       The Start 'Capture' is emulated by setting next for the port:
*           1. ingress PCL rule for ALL traffic from this port to TRAP to the CPU
*              we will assign it with 'Application specific cpu code' to
*              distinguish from other traffic go to the CPU
*
*       The Stop 'Capture' is emulated by setting next for the port:
*           1. remove the ingress PCL rule that function
*              tgfTrafficGeneratorPortTxEthCaptureStart added
*
*******************************************************************************/
static GT_STATUS prvTgfExMxPmPclCaptureSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  start
)
{
    CPSS_EXMXPM_TCAM_TYPE_ENT             tcamType;
    CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT     actionFormat;
    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT ruleInfo;
    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT       mask;
    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT       pattern;
    CPSS_EXMXPM_PCL_ACTION_UNT            action;
    CPSS_INTERFACE_INFO_STC               portInterface;
    CPSS_EXMXPM_PCL_LOOKUP_CFG_STC        lookupCfg;
    GT_STATUS                             rc;
    static GT_U32   first = 1;


    if ((first) || (GT_FALSE == start))
    {
        TGF_RESET_PARAM_MAC(mask);
        TGF_RESET_PARAM_MAC(pattern);
        TGF_RESET_PARAM_MAC(action);

        tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
        ruleInfo.internalTcamRuleStartIndex = TGF_PCL_RULE_INDEX_CNS;

        ruleFormat   = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E;
        actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E;

        /* need to match Only the PCL id */
        mask.ruleIngrStandardNotIp.commonIngrStd.pclId    = 0x3FF;
        pattern.ruleIngrStandardNotIp.commonIngrStd.pclId = TGF_CAPTURE_PCL_ID_CNS;

        /* trap to CPU */
        action.ingressStandard.pktCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        /* set specific CPU code for those 'captured' packets */
        action.ingressStandard.mirror.cpuCode = CPSS_NET_LAST_USER_DEFINED_E;

        /* add the rule to the PCL */
        rc = cpssExMxPmPclRuleSet(devNum, tcamType, &ruleInfo, ruleFormat,
                                  actionFormat, start, &mask, &pattern, &action);
        if(rc != GT_OK)
        {
            return rc;
        }

        first = !start;
    }

    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.portNum = portNum;
    portInterface.devPort.devNum  = devNum;

    TGF_RESET_PARAM_MAC(lookupCfg);

    if(start == GT_TRUE)
    {
        lookupCfg.enableLookup = GT_TRUE;
        lookupCfg.pclId        = TGF_CAPTURE_PCL_ID_CNS;

        lookupCfg.groupKeyTypes.nonIpKey = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E;
        lookupCfg.groupKeyTypes.ipv4Key  = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E;
        lookupCfg.groupKeyTypes.ipv6Key  = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E;
    }
    else
    {
        lookupCfg.enableLookup = GT_TRUE;
    }

    rc = cpssExMxPmPclCfgTblSet(devNum, &portInterface,
                                CPSS_PCL_DIRECTION_INGRESS_E,
                                CPSS_PCL_LOOKUP_0_E,
                                &lookupCfg);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = cpssExMxPmPclPortIngressPolicyEnableSet(devNum, portNum, start);

    return rc;
}
#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
/*******************************************************************************
* prvTgfExMxPclCaptureSet
*
* DESCRIPTION:
*       Start/Stop capture the traffic that egress the port
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       start   - start/stop capture
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The Start 'Capture' is emulated by setting next for the port:
*           1. ingress PCL rule for ALL traffic from this port to TRAP to the CPU
*              we will assign it with 'Application specific cpu code' to
*              distinguish from other traffic go to the CPU
*
*       The Stop 'Capture' is emulated by setting next for the port:
*           1. remove the ingress PCL rule that function
*              tgfTrafficGeneratorPortTxEthCaptureStart added
*
*******************************************************************************/
static GT_STATUS prvTgfExMxPclCaptureSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  start
)
{
    CPSS_EXMX_PCL_RULE_FORMAT_ENT      ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E;
    GT_U32                             ruleIndex  = 0;
    GT_STATUS                          rc         = GT_OK;
    static GT_U32                      first      = 1;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT      mask;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT      pattern;
    CPSS_EXMX_POLICY_ACTION_STC        action;

    if(GT_TRUE == start)
    {
        if(first)
        {
            TGF_RESET_PARAM_MAC(mask);
            TGF_RESET_PARAM_MAC(pattern);
            TGF_RESET_PARAM_MAC(action);

            ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E;
            ruleIndex  = TGF_PCL_RULE_INDEX_CNS;

            /* need to match Only the PCL id */
            mask.tgStd.pclId    = (GT_U8)0xFF;/*exact match on this field */
            pattern.tgStd.pclId = (GT_U8)TGF_CAPTURE_PCL_ID_CNS;

            /* trap to CPU */
            action.pktCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

            /* set specific CPU code for those 'captured' packets */
            action.mirror.mirrorToCpu = GT_TRUE;

            /* add the rule to the PCL */
            rc = cpssExMxPclRuleSet(devNum, ruleFormat, ruleIndex, &mask, &pattern, &action);
            if(rc != GT_OK)
            {
                return rc;
            }

            first = 0;
        }
    }

    return rc;
}
#endif /* EX_FAMILY */

/*******************************************************************************
* prvTgfCaptureSet
*
* DESCRIPTION:
*       Start/Stop capture the traffic that egress the port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       mode             - packet capture mode
*       start            - start/stop capture on this port.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface, mode
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The Start 'Capture' is emulated by setting next for the port:
*       1. ingress PCL rule for ALL traffic from this port to TRAP to the CPU
*          we will assign it with 'Application specific cpu code' to
*          distinguish from other traffic go to the CPU
*
*       The Stop 'Capture' is emulated by setting next for the port:
*       2. remove the ingress PCL rule that function
*          tgfTrafficGeneratorPortTxEthCaptureStart added
*
*******************************************************************************/
static GT_STATUS prvTgfCaptureSet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN TGF_CAPTURE_MODE_ENT     mode,
    IN GT_BOOL                  start
)
{
    TGF_PARAM_NOT_USED(portInterfacePtr);
    TGF_PARAM_NOT_USED(start);

#ifdef CHX_FAMILY

    switch (mode)
    {
        case TGF_CAPTURE_MODE_PCL_E:
            return prvTgfDxChPclCaptureSet(portInterfacePtr->devPort.devNum,
                                           portInterfacePtr->devPort.portNum,
                                           start);
        case TGF_CAPTURE_MODE_MIRRORING_E:
            return prvTgfDxChMirroringCaptureSet(portInterfacePtr->devPort.devNum,
                                                 portInterfacePtr->devPort.portNum,
                                                 start);
        default:
            return GT_BAD_PARAM;
    }
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    switch (mode)
    {
        case TGF_CAPTURE_MODE_PCL_E:
            return prvTgfExMxPmPclCaptureSet(portInterfacePtr->devPort.devNum,
                                             portInterfacePtr->devPort.portNum,
                                             start);
        default:
            return GT_BAD_PARAM;
    }

#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    switch (mode)
    {
        case TGF_CAPTURE_MODE_PCL_E:
            return prvTgfExMxPclCaptureSet(portInterfacePtr->devPort.devNum,
                                           portInterfacePtr->devPort.portNum,
                                           start);
        default:
            return GT_BAD_PARAM;
    }

#endif /*EX_FAMILY*/

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY) */
}

/*******************************************************************************
* tgfTrafficGeneratorPortTxEthCaptureSet
*
* DESCRIPTION:
*       Start/Stop capture the traffic that egress the port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       mode             - packet capture mode
*       start            - start/stop capture on this port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The Start capture is emulated by setting next for the port:
*           1. loopback on the port (so all traffic that egress the port will
*              ingress the same way)
*           2. ingress PCL rule for ALL traffic from this port to TRAP to the CPU
*              we will assign it with 'Application specific cpu code' to
*              distinguish from other traffic go to the CPU
*
*       The Stop capture is emulated by setting next for the port:
*           1. disable loopback on the port
*           2. remove the ingress PCL rule that function
*              tgfTrafficGeneratorPortTxEthCaptureStart added
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortTxEthCaptureSet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN TGF_CAPTURE_MODE_ENT      mode,
    IN GT_BOOL                   start
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    loops = 0;

    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);

    if(portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

    if (GT_FALSE == start)
    {
        /* let the packets be captured */
        loops = prvTgfCaptureTimeLoop;
        while(loops--)/* allow debugging by set this value = 1000 */
        {
            osTimerWkAfter(TGF_DEFAULT_CAPTURE_SLEEP_TIME_CNS);
        }
    }

    rc = tgfTrafficGeneratorPortLoopbackModeEnableSet(portInterfacePtr, start);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = prvTgfCaptureSet(portInterfacePtr, mode, start);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = tgfTrafficTableRxStartCapture(start);

    return rc;
}

/*******************************************************************************
* tgfTrafficGeneratorLoopTimeSet
*
* DESCRIPTION:
*       Set sleeping time after sending burst of packets from the CPU
*
* INPUTS:
*       timeInSec - time in seconds
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*       Needed for debugging purposes
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorLoopTimeSet
(
    IN GT_U32   timeInSec
)
{
    if(timeInSec)
    {
        prvTgfTimeLoop = timeInSec * 2;
    }
    else
    {
#ifdef  EMBEDDED
        /* run on hardware */
        prvTgfTimeLoop = 1;
#else
        /* run on simulation */
        prvTgfTimeLoop = TGF_DEFAULT_TIME_LOOP_NUM_CNS;
#endif
    }

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficGeneratorCaptureLoopTimeSet
*
* DESCRIPTION:
*       Set sleeping time before disabling packet's capturing
*
* INPUTS:
*       timeInMSec - time in miliseconds (0 for restore defaults)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*       Needed for debugging purposes
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorCaptureLoopTimeSet
(
    IN GT_U32   timeInMSec
)
{
    prvTgfCaptureTimeLoop = (timeInMSec) ? (timeInMSec / TGF_DEFAULT_CAPTURE_SLEEP_TIME_CNS) : TGF_DEFAULT_CAPTURE_LOOP_NUM_CNS;

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficGeneratorEtherTypeForVlanTagSet
*
* DESCRIPTION:
*       set etherType to recognize 'Tagged packes' that send from the traffic
*       generator to the PP
*
* INPUTS:
*       etherType - new etherType
*                   (was initialized as 0x8100)
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
GT_VOID tgfTrafficGeneratorEtherTypeForVlanTagSet
(
    IN GT_U16   etherType
)
{
    etherTypeForVlanTag = etherType;
}

/*******************************************************************************
* autoChecksumField
*
* DESCRIPTION:
*       apply auto checksum fields after applying the VFD
*
* INPUTS:
*       checksumInfoArr - (array of) info about fields that need auto checksum build
*       checksumExtraInfoPtr -  (pointer to) single extra info for auto checksum build
*
* OUTPUTS:
*       None
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*       GT_BAD_STATE - parser caused bad state that should not happen
*
* COMMENTS:
*       Buffer must be allocated for CRC (4 bytes)
*
*******************************************************************************/
static GT_STATUS autoChecksumField
(
    IN TGF_AUTO_CHECKSUM_INFO_STC *checksumInfoPtr,
    IN TGF_AUTO_CHECKSUM_EXTRA_INFO_STC *checksumExtraInfoPtr
)
{
    GT_STATUS rc;
    GT_U32  ii,kk;
    TGF_AUTO_CHECKSUM_INFO_STC *currChecksumInfoPtr;/* current checksum info pointer */
    GT_U8   *pseudoPtr;/* pointer to pseudo header buffer */
    GT_U32  pseudoNumBytes;/* pseudo header number of bytes */
    GT_U32  headerLen,totalLen,l4Length;

    currChecksumInfoPtr = &checksumInfoPtr[0];
    for(ii = 0 ; ii < TGF_AUTO_CHECKSUM_FIELD_LAST_E ; ii++,currChecksumInfoPtr++)
    {
        if(currChecksumInfoPtr->enable != GT_TRUE ||
           currChecksumInfoPtr->checkSumPtr == NULL)
        {
            continue;
        }

        switch(ii)
        {
            case TGF_AUTO_CHECKSUM_FIELD_TCP_E:
            case TGF_AUTO_CHECKSUM_FIELD_UDP_E:
                if(checksumExtraInfoPtr->isIpv4 == GT_FALSE)
                {   /* ipv6 */
                    /* Sip ,Dip */
                    for(kk = 0 ; kk < 32 ; kk++)
                    {
                        checksumExtraInfoPtr->pseudo.buffer[kk]   = checksumExtraInfoPtr->startL3HeadrPtr[8+kk];
                    }

                    /* zero */
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = 0;
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = 0;
                    /* l4 length*/
                    l4Length =  (checksumExtraInfoPtr->startL3HeadrPtr[4] << 8)  |
                                 checksumExtraInfoPtr->startL3HeadrPtr[5];
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = checksumExtraInfoPtr->startL3HeadrPtr[4];
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = checksumExtraInfoPtr->startL3HeadrPtr[5];

                    checksumExtraInfoPtr->pseudo.buffer[kk++] = 0;
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = 0;
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = 0;
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = checksumExtraInfoPtr->startL3HeadrPtr[6];

                    checksumExtraInfoPtr->pseudo.numBytes = kk;
                }
                else
                {
                    headerLen = 4 * (checksumExtraInfoPtr->startL3HeadrPtr[0] & 0xF);
                    totalLen =  (checksumExtraInfoPtr->startL3HeadrPtr[2] << 8) |
                                 checksumExtraInfoPtr->startL3HeadrPtr[3];
                    l4Length = totalLen - headerLen;

                    /* Sip ,Dip */
                    for(kk = 0 ; kk < 8 ; kk++)
                    {
                        checksumExtraInfoPtr->pseudo.buffer[kk] = checksumExtraInfoPtr->startL3HeadrPtr[12+kk];
                    }

                    /* zero */
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = 0;
                    /* protocol */
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = checksumExtraInfoPtr->startL3HeadrPtr[9];
                    /* l4 length*/
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = (GT_U8)(l4Length >> 8);
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = (GT_U8)(l4Length);

                    checksumExtraInfoPtr->pseudo.numBytes = kk;

                }

                if(ii == TGF_AUTO_CHECKSUM_FIELD_TCP_E)
                {
                    /* TCP not hold length , so take it from ipv4/6 info */
                    currChecksumInfoPtr->numBytes = l4Length;
                }

                pseudoPtr = checksumExtraInfoPtr->pseudo.buffer;
                pseudoNumBytes = checksumExtraInfoPtr->pseudo.numBytes;

                if(checksumExtraInfoPtr->udpLengthFieldPtr)
                {
                    /* UDP field of length to be same as the calculated length */
                    checksumExtraInfoPtr->udpLengthFieldPtr[0] = (GT_U8)(l4Length >> 8);
                    checksumExtraInfoPtr->udpLengthFieldPtr[1] = (GT_U8)(l4Length);

                    currChecksumInfoPtr->numBytes = l4Length;
                }

                break;
            default:
                pseudoPtr = NULL;
                pseudoNumBytes = 0;
                break;
        }

        /* apply checksum auto-calc on needed section of the packet */
        currChecksumInfoPtr->checkSumPtr[0] = 0;
        currChecksumInfoPtr->checkSumPtr[1] = 0;
        rc = tgfTrafficEnginePacketCheckSum16BitsCalc(
                currChecksumInfoPtr->startPtr,
                currChecksumInfoPtr->numBytes,
                pseudoPtr,
                pseudoNumBytes,
                currChecksumInfoPtr->checkSumPtr);
        if(rc != GT_OK)
        {
            return rc;
        }
    }


    return GT_OK;
}

void setMiiCounter(IN GT_U32 newMiiCounter)
{
    miiCounter = newMiiCounter;
}

