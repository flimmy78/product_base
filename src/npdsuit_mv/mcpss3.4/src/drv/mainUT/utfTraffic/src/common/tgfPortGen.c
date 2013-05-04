/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfPortGen.c
*
* DESCRIPTION:
*       Generic API for Port configuration.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <utf/private/prvUtfHelpers.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

#include <trafficEngine/tgfTrafficEngine.h>
#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfPortGen.h>

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
    #include <cpss/dxCh/dxChxGen/port/cpssDxChPortStat.h>
    #include <cpss/dxCh/dxChxGen/port/cpssDxChPortTx.h>
    #include <cpss/dxCh/dxChxGen/port/cpssDxChPortBufMg.h>
    #include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortCtrl.h>
    #include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgCount.h>
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
/*******************************************************************************
* prvTgfConvertGenericToDxChTailDropProfileParams
*
* DESCRIPTION:
*       Convert generic into device specific Tail Drop Profile parameters.
*
* INPUTS:
*       tailDropProfileParamsPtr - (pointer to) Tail Drop Profile parameters.
*
* OUTPUTS:
*       dxChTailDropProfileParamsPtr - (pointer to) DxCh Tail Drop Profile
*                                      parameters.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChTailDropProfileParams
(
    IN  PRV_TGF_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS  *tailDropProfileParamsPtr,
    OUT CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS     *dxChTailDropProfileParamsPtr
)
{
    cpssOsMemSet(dxChTailDropProfileParamsPtr, 0,
                 sizeof(CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS));

    /* convert entry info into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTailDropProfileParamsPtr,
                                   tailDropProfileParamsPtr, dp0MaxBuffNum);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTailDropProfileParamsPtr,
                                   tailDropProfileParamsPtr, dp1MaxBuffNum);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTailDropProfileParamsPtr,
                                   tailDropProfileParamsPtr, dp2MaxBuffNum);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTailDropProfileParamsPtr,
                                   tailDropProfileParamsPtr, dp0MaxDescrNum);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTailDropProfileParamsPtr,
                                   tailDropProfileParamsPtr, dp1MaxDescrNum);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTailDropProfileParamsPtr,
                                   tailDropProfileParamsPtr, dp2MaxDescrNum);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTailDropProfileParamsPtr,
                                   tailDropProfileParamsPtr, tcMaxBuffNum);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTailDropProfileParamsPtr,
                                   tailDropProfileParamsPtr, tcMaxDescrNum);

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertDxChToGenericTailDropProfileParams
*
* DESCRIPTION:
*       Convert device specific Tail Drop Profile parameters into generic.
*
* INPUTS:
*       dxChTailDropProfileParamsPtr - (pointer to) DxCh Tail Drop Profile
*                                      parameters.
*
* OUTPUTS:
*       tailDropProfileParamsPtr - (pointer to) Tail Drop Profile parameters.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertDxChToGenericTailDropProfileParams
(
    IN  CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS     *dxChTailDropProfileParamsPtr,
    OUT PRV_TGF_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS  *tailDropProfileParamsPtr
)
{
    cpssOsMemSet(tailDropProfileParamsPtr, 0,
                 sizeof(tailDropProfileParamsPtr));

    /* convert entry info into generic format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTailDropProfileParamsPtr,
                                   tailDropProfileParamsPtr, dp0MaxBuffNum);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTailDropProfileParamsPtr,
                                   tailDropProfileParamsPtr, dp1MaxBuffNum);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTailDropProfileParamsPtr,
                                   tailDropProfileParamsPtr, dp2MaxBuffNum);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTailDropProfileParamsPtr,
                                   tailDropProfileParamsPtr, dp0MaxDescrNum);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTailDropProfileParamsPtr,
                                   tailDropProfileParamsPtr, dp1MaxDescrNum);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTailDropProfileParamsPtr,
                                   tailDropProfileParamsPtr, dp2MaxDescrNum);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTailDropProfileParamsPtr,
                                   tailDropProfileParamsPtr, tcMaxBuffNum);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTailDropProfileParamsPtr,
                                   tailDropProfileParamsPtr, tcMaxDescrNum);

    return GT_OK;
}
#endif /* CHX_FAMILY */

/*******************************************************************************
* prvTgfPortDeviceNumByPortGet
*
* DESCRIPTION:
*       Gets device number for specified port number.
*
*
* INPUTS:
*       portNum  - port number
*
* OUTPUTS:
*       devNumPtr   - (pointer to)device number
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_BAD_PARAM        - on port number not found in port list
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPortDeviceNumByPortGet
(
    IN  GT_U8                          portNum,
    OUT GT_U8                          *devNumPtr
)
{
    GT_U32  portIter = 0;

    /* find port index */
    for (portIter = 0; (portIter < prvTgfPortsNum); portIter++)
    {
        if (prvTgfPortsArray[portIter] == portNum)
        {
            break;
        }
    }

    if (portIter >= prvTgfPortsNum)
    {
        PRV_UTF_LOG1_MAC("[TGF]: Port [%d] is not valid", portNum);
        return GT_BAD_PARAM;
    }

    *devNumPtr = prvTgfDevsArray[portIter];
    return GT_OK;
}

/******************************************************************************\
 *                       CPSS generic API section                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfPortForceLinkDownEnableSet
*
* DESCRIPTION:
*       Enable/disable Force Link Down on specified port on specified device.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - GT_TRUE for force link down, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPortForceLinkDownEnableSet
(
    IN GT_U8                          portNum,
    IN GT_BOOL                        enable
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
    GT_U8  devNum;

    if (GT_OK != prvTgfPortDeviceNumByPortGet(portNum, &devNum))
    {
        return GT_BAD_PARAM;
    }
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */

#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChPortForceLinkDownEnableSet(devNum, portNum, enable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmPortForceLinkDownSet(devNum, portNum, enable);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPortEgressCntrModeSet
*
* DESCRIPTION:
*       Configure a set of egress counters that work in specified bridge egress
*       counters mode on specified device.
*
* INPUTS:
*       cntrSetNum - counter set number.
*       setModeBmp - counter mode bitmap. For example:
*                   GT_EGRESS_CNT_PORT | GT_EGRESS_CNT_VLAN | GT_EGRESS_CNT_DP.
*       portNum   - physical port number, if corresponding bit in setModeBmp is 1.
*       vlanId    - VLAN Id, if corresponding bit in setModeBmp is 1.
*       tc        - traffic class queue, if corresponding bit in setModeBmp is 1.
*       dpLevel   - drop precedence level, if corresponding bit in setModeBmp is 1.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported DP levels:  CPSS_DP_GREEN_E, CPSS_DP_RED_E.
*
*******************************************************************************/
GT_STATUS prvTgfPortEgressCntrModeSet
(
    IN  GT_U8                           cntrSetNum,
    IN  CPSS_PORT_EGRESS_CNT_MODE_ENT   setModeBmp,
    IN  GT_U8                           portNum,
    IN  GT_U16                          vlanId,
    IN  GT_U8                           tc,
    IN  CPSS_DP_LEVEL_ENT               dpLevel
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
    GT_U8  devNum;

    if (GT_OK != prvTgfPortDeviceNumByPortGet(portNum, &devNum))
    {
        return GT_BAD_PARAM;
    }
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */

#ifdef CHX_FAMILY
    /* support device with less TC */
    tc %=
        PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.txQueuesNum;

    /* call device specific API */
    return cpssDxChPortEgressCntrModeSet(
        devNum, cntrSetNum, setModeBmp, portNum, vlanId, tc, dpLevel);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmBrgCntBridgeEgressCntrModeSet(
        devNum, (CPSS_PORT_EGRESS_CNT_MODE_ENT)cntrSetNum,
        setModeBmp, portNum, vlanId, tc, dpLevel);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPortEgressCntrsGet
*
* DESCRIPTION:
*       Gets a egress counters from specific counter-set.
*
* INPUTS:
*       cntrSetNum - counter set number : 0, 1
*
* OUTPUTS:
*       egrCntrPtr - (pointer to) structure of egress counters current values.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortEgressCntrsGet
(
    IN  GT_U8                       cntrSetNum,
    OUT CPSS_PORT_EGRESS_CNTR_STC   *egrCntrPtr
)
{
#if (defined EXMXPM_FAMILY)
    CPSS_EXMXPM_BRG_EGRESS_CNTR_STC pmEgrCntr;
#endif

#if (defined CHX_FAMILY)
    CPSS_PORT_EGRESS_CNTR_STC   egrCntr;
#endif

#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
   GT_U8                       devNum  = 0;
   GT_STATUS                   rc, rc1 = GT_OK;

   /* clear output counters */
   cpssOsMemSet(egrCntrPtr, 0, sizeof(CPSS_PORT_EGRESS_CNTR_STC));

#endif /* (defined CHX_FAMILY) !! (defined EXMXPM_FAMILY) */

#ifdef CHX_FAMILY
   /* prepare device iterator */
   PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

   /* go over all active devices */
   while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
   {
       /* call device specific API */
       rc = cpssDxChPortEgressCntrsGet(
           devNum, cntrSetNum, &egrCntr);
       if (GT_OK != rc)
       {
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssDxChPortEgressCntrsGet FAILED, rc = [%d]", rc);

           rc1 = rc;
           continue;
       }

       /* add result to output counters */
       egrCntrPtr->brgEgrFilterDisc += egrCntr.brgEgrFilterDisc;
       egrCntrPtr->egrFrwDropFrames += egrCntr.egrFrwDropFrames;
       egrCntrPtr->outBcFrames      += egrCntr.outBcFrames;
       egrCntrPtr->outCtrlFrames    += egrCntr.outCtrlFrames;
       egrCntrPtr->outMcFrames      += egrCntr.outMcFrames;
       egrCntrPtr->outUcFrames      += egrCntr.outUcFrames;
       egrCntrPtr->txqFilterDisc    += egrCntr.txqFilterDisc;
   }

   return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
   /* prepare device iterator */
   rc = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
   if (GT_OK != rc)
   {
       PRV_UTF_LOG1_MAC(
           "[TGF]: prvUtfNextDeviceReset FAILED, rc = [%d]", rc);

       return rc;
   }

   /* go over all active devices */
   while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
   {
       /* call device specific API */
       rc = cpssExMxPmBrgCntBridgeEgressCntrsGet(
        devNum, (CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT)cntrSetNum, &pmEgrCntr);
       if (GT_OK != rc)
       {
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssExMxPmBrgCntBridgeEgressCntrsGet FAILED, rc = [%d]", rc);

           rc1 = rc;
           continue;
       }

       /* add result to output counters */
       egrCntrPtr->brgEgrFilterDisc += pmEgrCntr.brgEgrFilterPkts;
       egrCntrPtr->egrFrwDropFrames +=
           pmEgrCntr.tailDropPkts + pmEgrCntr.mcTailDropPkts;
       egrCntrPtr->outBcFrames      += pmEgrCntr.outBcPkts;
       egrCntrPtr->outCtrlFrames    += pmEgrCntr.ctrlPkts;
       egrCntrPtr->outMcFrames      += pmEgrCntr.outMcPkts;
       egrCntrPtr->outUcFrames      += pmEgrCntr.outUcPkts;
       egrCntrPtr->txqFilterDisc    += pmEgrCntr.frwdRestrictPkts;
   }

   return rc1;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPortMacCounterGet
*
* DESCRIPTION:
*       Gets Ethernet MAC counter for a specified port on specified device.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*                  CPU port counters are valid only when using "Ethernet CPU
*                  port" (not using the SDMA interface).
*                  when using the SDMA interface refer to the API
*                  cpssDxChNetIfSdmaRxCountersGet(...)
*       cntrName - specific counter name
*
* OUTPUTS:
*       cntrValuePtr - (pointer to) current counter value.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number, device or counter name
*       GT_HW_ERROR  - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     The 10G MAC MIB counters are 64-bit wide.
*     Not supported counters: CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E,
*     CPSS_BadFC_RCV_E, CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E.
*
*******************************************************************************/
GT_STATUS prvTgfPortMacCounterGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    OUT GT_U64                      *cntrValuePtr
)
{
#if (defined CHX_FAMILY)
   GT_STATUS                        rc = GT_OK;

   /* Get Port MAC MIB counters */
   rc = cpssDxChMacCounterGet(devNum, portNum, cntrName, cntrValuePtr);
   if (GT_OK != rc)
   {
       PRV_UTF_LOG1_MAC(
           "[TGF]: cpssDxChPortMacCountersOnPortGet FAILED, rc = [%d]", rc);
   }


   return rc;
#endif /* CHX_FAMILY */


#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(cntrName);
    TGF_PARAM_NOT_USED(cntrValuePtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortMacCountersCaptureOnPortGet
*
* DESCRIPTION:
*     Gets captured Ethernet MAC counter for a particular Port.
*
* INPUTS:
*       portNum  - physical port number
*
* OUTPUTS:
*       portMacCounterSetArrayPtr - (pointer to) array of current counter values.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_STATE - previous capture operation isn't finished.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       Overiding of previous capture by another port in a group is occurs,
*       when the capture triggered port
*       (see cpssDxChPortMacCountersCaptureTriggerSet) and previous
*       capture triggered port are in the same group.
*       (This comment dosn't connected to HyperG.Stack ports.)
*        Not supported counters: CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E,
*        CPSS_BadFC_RCV_E, CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E.
*
*******************************************************************************/
GT_STATUS prvTgfPortMacCountersCaptureOnPortGet
(
    IN  GT_U8                           portNum,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC   *portMacCounterSetArrayPtr
)
{
#if (defined CHX_FAMILY)
   GT_U8                            devNum  = 0;
   GT_STATUS                        rc = GT_OK;
   GT_BOOL                          captureIsDonePtr;
   GT_U32                           i = 0;


   if (GT_OK != prvTgfPortDeviceNumByPortGet(portNum, &devNum))
   {
       return GT_BAD_PARAM;
   }

   /* Set trigger for given port */
   rc = cpssDxChPortMacCountersCaptureTriggerSet(devNum, portNum);
   if (GT_OK != rc)
   {
       PRV_UTF_LOG1_MAC(
           "[TGF]: cpssDxChPortMacCountersCaptureTriggerSet FAILED, rc = [%d]", rc);
   }

   captureIsDonePtr = GT_FALSE;
   i = 0;

   /* do busy wait on capture status */
   while (!captureIsDonePtr)
   {
       /* check that the capture was done */
       rc = cpssDxChPortMacCountersCaptureTriggerGet(devNum, portNum, &captureIsDonePtr);
       if (GT_OK != rc)
       {
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssDxChPortMacCountersCaptureTriggerGet FAILED, rc = [%d]", rc);
       }

       i++;

       if (i == 1000)
       {
           break;
       }
   }

   if (captureIsDonePtr == GT_FALSE)
   {
       PRV_UTF_LOG1_MAC(
           "[TGF]: cpssDxChPortMacCountersCaptureTriggerGet FAILED, captureIsDonePtr = [%d]", captureIsDonePtr);
   }

   /* Get Port Captured MAC MIB counters */
   rc = cpssDxChPortMacCountersCaptureOnPortGet(devNum, portNum,
                                                portMacCounterSetArrayPtr);
   if (GT_OK != rc)
   {
       PRV_UTF_LOG1_MAC(
           "[TGF]: cpssDxChPortMacCountersCaptureOnPortGet FAILED, rc = [%d]", rc);
   }

   return rc;
#endif /* CHX_FAMILY */


#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(portMacCounterSetArrayPtr);


    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortMacCountersOnPortGet
*
* DESCRIPTION:
*     Gets Ethernet MAC counter for a particular Port.
*
* INPUTS:
*       portNum  - physical port number
*
* OUTPUTS:
*       portMacCounterSetArrayPtr - (pointer to) array of current counter values.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_STATE - previous capture operation isn't finished.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*     Not supported counters: CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E,
*     CPSS_BadFC_RCV_E, CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E.
*
*******************************************************************************/
GT_STATUS prvTgfPortMacCountersOnPortGet
(
    IN  GT_U8                           portNum,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC   *portMacCounterSetArrayPtr
)
{
#if (defined CHX_FAMILY)
   GT_U8                            devNum  = 0;
   GT_STATUS                        rc = GT_OK;


   if (GT_OK != prvTgfPortDeviceNumByPortGet(portNum, &devNum))
   {
       return GT_BAD_PARAM;
   }

   /* Get Port MAC MIB counters */
   rc = cpssDxChPortMacCountersOnPortGet(devNum, portNum,
                                          portMacCounterSetArrayPtr);
   if (GT_OK != rc)
   {
       PRV_UTF_LOG1_MAC(
           "[TGF]: cpssDxChPortMacCountersOnPortGet FAILED, rc = [%d]", rc);
   }


   return rc;
#endif /* CHX_FAMILY */


#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(portMacCounterSetArrayPtr);


    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}


/*******************************************************************************
* prvTgfPortMacCountersRxHistogramEnable
*
* DESCRIPTION:
*       Enable/disable updating of the RMON Etherstat histogram
*       counters for received packets per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*           - Ports 0 through 5
*           - Ports 6 through 11
*           - Ports 12 through 17
*           - Ports 18 through 23
*
* INPUTS:
*       portNum     - physical port number
*       enable      - enable updating of the counters for received packets
*                   GT_FALSE - Counters are updated.
*                   GT_TRUE - Counters are not updated.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_STATE - previous capture operation isn't finished.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
*******************************************************************************/
GT_STATUS prvTgfPortMacCountersRxHistogramEnable
(
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
{
#if (defined CHX_FAMILY)
    GT_U8  devNum;

    if (GT_OK != prvTgfPortDeviceNumByPortGet(portNum, &devNum))
    {
        return GT_BAD_PARAM;
    }
    /* call device specific API */
    return cpssDxChPortMacCountersRxHistogramEnable(devNum, portNum, enable);
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(enable);


    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortMacCountersTxHistogramEnable
*
* DESCRIPTION:
*       Enable/disable updating of the RMON Etherstat histogram
*       counters for transmitted packets per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*           - Ports 0 through 5
*           - Ports 6 through 11
*           - Ports 12 through 17
*           - Ports 18 through 23
*
* INPUTS:
*       portNum     - physical port number
*       enable      - enable updating of the counters for transmitted packets
*                   GT_FALSE - Counters are updated.
*                   GT_TRUE - Counters are not updated.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_STATE - previous capture operation isn't finished.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
*******************************************************************************/
GT_STATUS prvTgfPortMacCountersTxHistogramEnable
(
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
{
#if (defined CHX_FAMILY)
    GT_U8  devNum;

    if (GT_OK != prvTgfPortDeviceNumByPortGet(portNum, &devNum))
    {
        return GT_BAD_PARAM;
    }

    /* call device specific API */
    return cpssDxChPortMacCountersTxHistogramEnable(devNum, portNum, enable);
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(enable);


    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortMacCountersClearOnReadSet
*
* DESCRIPTION:
*       Enable or disable MAC Counters Clear on read per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*       - Ports 0 through 5
*       - Ports 6 through 11
*       - Ports 12 through 17
*       - Ports 18 through 23
*
* INPUTS:
*       portNum     - physical port number (or CPU port)
*       enable      - enable clear on read for MAC counters
*                   GT_FALSE - Counters are not cleared.
*                   GT_TRUE - Counters are cleared.

*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS prvTgfPortMacCountersClearOnReadSet
(
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
{

#if (defined CHX_FAMILY)
    GT_U8  devNum;

    if (GT_OK != prvTgfPortDeviceNumByPortGet(portNum, &devNum))
    {
        return GT_BAD_PARAM;
    }

    /* call device specific API */
    return cpssDxChPortMacCountersClearOnReadSet(devNum, portNum, enable);

#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(enable);


    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortMruSet
*
* DESCRIPTION:
*       Sets the Maximal Receive Packet size for specified port
*       on specified device.
*
* INPUTS:
*       portNum     - physical port number (or CPU port)
*       mru         - max receive packet size in bytes. (0..16382)
*                     value must be even
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS prvTgfPortMruSet
(
    IN GT_U8    portNum,
    IN GT_U32   mru
)
{

#if (defined CHX_FAMILY)
    GT_U8  devNum;

    if (GT_OK != prvTgfPortDeviceNumByPortGet(portNum, &devNum))
    {
        return GT_BAD_PARAM;
    }

    /* call device specific API */
    return cpssDxChPortMruSet(devNum, portNum, mru);

#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(mru);


    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortMruGet
*
* DESCRIPTION:
*       Sets the Maximal Receive Packet size for specified port
*       on specified device.
*
* INPUTS:
*       portNum     - physical port number (or CPU port)
*
* OUTPUTS:
*       mruPtr      - max receive packet size in bytes.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS prvTgfPortMruGet
(
    IN  GT_U8    portNum,
    OUT GT_U32   *mruPtr
)
{

#if (defined CHX_FAMILY)
    GT_U8  devNum;

    if (GT_OK != prvTgfPortDeviceNumByPortGet(portNum, &devNum))
    {
        return GT_BAD_PARAM;
    }

    /* call device specific API */
    return cpssDxChPortMruGet(devNum, portNum, mruPtr);

#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(mruPtr);


    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortFcHolSysModeSet
*
* DESCRIPTION:
*       Set Flow Control or HOL system mode on the specified device.
*
* INPUTS:
*       devNum      - physical device number
*       modeFcHol   - Flow Control or HOL system mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPortFcHolSysModeSet
(
    IN  GT_U8                   devNum,
    IN  PRV_TGF_PORT_HOL_FC_ENT modeFcHol
)
{

#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_PORT_HOL_FC_ENT   dxChModeFcHol;

    switch(modeFcHol)
    {
        case PRV_TGF_PORT_PORT_FC_E: dxChModeFcHol = CPSS_DXCH_PORT_FC_E;
                                     break;

        case PRV_TGF_PORT_PORT_HOL_E: dxChModeFcHol = PRV_TGF_PORT_PORT_HOL_E;
                                      break;

        default: return GT_BAD_PARAM;
    }

    /* call device specific API */
    rc = cpssDxChPortFcHolSysModeSet(devNum, dxChModeFcHol);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortFcHolSysModeSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(modeFcHol);


    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxTailDropProfileSet
*
* DESCRIPTION:
*       Enables/Disables sharing of buffers and descriptors for all queues of an
*       egress port and set maximal port's limits of buffers and descriptors.
*
* INPUTS:
*       devNum        - physical device number
*       profileSet    - the Profile Set in which the Traffic
*                       Class Drop Parameters is associated
*       enableSharing - If GT_TRUE Sharing is enabled. If a packet is to be queued
*                      to an empty queue, it is queued even if the number of buffers
*                       or descriptors allocated to this port exceeds the
*                       < portMaxBuffLimit > and < portMaxDescrLimit >.
*                       If GT_FALSE Sharing is disabled. If the sum of buffers or
*                       descriptors allocated per queue is larger than the number of
*                       buffers or descriptors allocated to this port
*                       < portMaxBuffLimit > and < portMaxDescrLimit >, then some of
*                       the queues may be starved.
*       portMaxBuffLimit - maximal number of buffers for a port.
*       portMaxDescrLimit - maximal number of descriptors for a port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxTailDropProfileSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    IN  GT_BOOL                             enableSharing,
    IN  GT_U16                              portMaxBuffLimit,
    IN  GT_U16                              portMaxDescrLimit
)
{

#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxTailDropProfileSet(devNum, profileSet, enableSharing,
                                          portMaxBuffLimit, portMaxDescrLimit);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxTailDropProfileSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(profileSet);
    TGF_PARAM_NOT_USED(enableSharing);
    TGF_PARAM_NOT_USED(portMaxBuffLimit);
    TGF_PARAM_NOT_USED(portMaxDescrLimit);


    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxTailDropProfileGet
*
* DESCRIPTION:
*       Get sharing status of buffers and descriptors for all queues of an
*       egress port and get maximal port's limits of buffers and descriptors.
*
* INPUTS:
*       devNum        - physical device number
*       profileSet    - the Profile Set in which the Traffic
*                       Class Drop Parameters is associated
*
* OUTPUTS:
*       enableSharingPtr - Pointer to sharing status.
*                      If GT_TRUE Sharing is enabled. If a packet is to be queued
*                      to an empty queue, it is queued even if the number of buffers
*                       or descriptors allocated to this port exceeds the
*                       < portMaxBuffLimit > and < portMaxDescrLimit >.
*                       If GT_FALSE Sharing is disabled. If the sum of buffers or
*                       descriptors allocated per queue is larger than the number of
*                       buffers or descriptors allocated to this port
*                       < portMaxBuffLimit > and < portMaxDescrLimit >, then some of
*                       the queues may be starved.
*       portMaxBuffLimitPtr - Pointer to maximal number of buffers for a port
*       portMaxDescrLimitPtr - Pointer to maximal number of descriptors for a port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxTailDropProfileGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    OUT GT_BOOL                             *enableSharingPtr,
    OUT GT_U16                              *portMaxBuffLimitPtr,
    OUT GT_U16                              *portMaxDescrLimitPtr
)
{

#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxTailDropProfileGet(devNum, profileSet, enableSharingPtr,
                                          portMaxBuffLimitPtr, portMaxDescrLimitPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxTailDropProfileGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(profileSet);
    TGF_PARAM_NOT_USED(enableSharingPtr);
    TGF_PARAM_NOT_USED(portMaxBuffLimitPtr);
    TGF_PARAM_NOT_USED(portMaxDescrLimitPtr);


    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}
/*******************************************************************************
* prvTgfPortTx4TcTailDropProfileSet
*
* DESCRIPTION:
*       Set tail drop profiles limits for particular TC.
*
* INPUTS:
*       devNum             - physical device number
*       profileSet         - the Profile Set in which the Traffic
*                            Class Drop Parameters is associated.
*       trafficClass       - the Traffic Class associated with this set of
*                            Drop Parameters.
*       tailDropProfileParamsPtr -
*                       the Drop Profile Parameters to associate
*                       with the Traffic Class in this Profile set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTx4TcTailDropProfileSet
(
    IN    GT_U8                                   devNum,
    IN    CPSS_PORT_TX_DROP_PROFILE_SET_ENT       profileSet,
    IN    GT_U8                                   trafficClass,
    IN    PRV_TGF_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS *tailDropProfileParamsPtr
)
{

#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS   dxChTailDropProfileParams;

    rc = prvTgfConvertGenericToDxChTailDropProfileParams(tailDropProfileParamsPtr,
                                                         &dxChTailDropProfileParams);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChtailDropProfileParams FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChPortTx4TcTailDropProfileSet(devNum, profileSet, trafficClass,
                                             &dxChTailDropProfileParams);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxBindPortToDpSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(profileSet);
    TGF_PARAM_NOT_USED(trafficClass);
    TGF_PARAM_NOT_USED(tailDropProfileParamsPtr);


    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTx4TcTailDropProfileGet
*
* DESCRIPTION:
*       Get tail drop profiles limits for particular TC.
*
* INPUTS:
*       devNum             - physical device number
*       profileSet         - the Profile Set in which the Traffic
*                            Class Drop Parameters is associated.
*       trafficClass       - the Traffic Class associated with this set of
*                            Drop Parameters.
*
* OUTPUTS:
*       tailDropProfileParamsPtr - Pointer to
*                       the Drop Profile Parameters to associate
*                       with the Traffic Class in this Profile.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTx4TcTailDropProfileGet
(
    IN    GT_U8                                   devNum,
    IN    CPSS_PORT_TX_DROP_PROFILE_SET_ENT       profileSet,
    IN    GT_U8                                   trafficClass,
    OUT   PRV_TGF_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS *tailDropProfileParamsPtr
)
{

#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS   dxChTailDropProfileParams;
    cpssOsMemSet(&dxChTailDropProfileParams, 0, sizeof(dxChTailDropProfileParams));

    /* call device specific API */
    rc = cpssDxChPortTx4TcTailDropProfileGet(devNum, profileSet, trafficClass,
                                             &dxChTailDropProfileParams);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxBindPortToDpSet FAILED, rc = [%d]", rc);

        return rc;
    }

    rc = prvTgfConvertDxChToGenericTailDropProfileParams(&dxChTailDropProfileParams,
                                                         tailDropProfileParamsPtr);

    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertDxChToGenericTailDropProfileParams FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(profileSet);
    TGF_PARAM_NOT_USED(trafficClass);
    TGF_PARAM_NOT_USED(tailDropProfileParamsPtr);


    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
*  prvTgfPortBindPortToDpSet
*
* DESCRIPTION:
*       Bind a Physical Port to a specific Drop Profile Set.
*
* INPUTS:
*       devNum        - physical device number
*       portNum       - physical port number
*       profileSet    - the Profile Set in which the Traffic
*                       Class Drop Parameters is associated
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxBindPortToDpSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               portNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet
)
{

#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxBindPortToDpSet(devNum, portNum, profileSet);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxBindPortToDpSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(profileSet);


    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
*  prvTgfPortTxBindPortToDpGet
*
* DESCRIPTION:
*       Get Drop Profile Set according to a Physical Port.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*
* OUTPUTS:
*       profileSetPtr    - Pointer to the Profile Set in which the Traffic
*                          Class Drop Parameters is associated
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxBindPortToDpGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               portNum,
    OUT CPSS_PORT_TX_DROP_PROFILE_SET_ENT   *profileSetPtr
)
{

#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxBindPortToDpGet(devNum, portNum, profileSetPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxBindPortToDpGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(profileSetPtr);


    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxQueueTxEnableSet
*
* DESCRIPTION:
*       Enable/Disable transmission from a Traffic Class queue
*       on the specified port of specified device.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       tcQueue - traffic class queue on this Physical Port
*       enable  - GT_TRUE, enable transmission from the queue
*                GT_FALSE, disable transmission from the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxQueueTxEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     tcQueue,
    IN  GT_BOOL   enable
)
{

#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxQueueTxEnableSet(devNum, portNum, tcQueue, enable);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxQueueTxEnableSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(tcQueue);
    TGF_PARAM_NOT_USED(enable);


    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxResourceHistogramThresholdSet
*
* DESCRIPTION:
*       Sets threshold for Histogram counter increment.
*
* INPUTS:
*       devNum      - physical device number
*       cntrNum     - Histogram Counter number. Range 0..3.
*       threshold   - If the Global Descriptors Counter exceeds this Threshold,
*                     the Histogram Counter is incremented by 1.
*                     Range 0..0x3FFF.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, cntrNum
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxResourceHistogramThresholdSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  cntrNum,
    IN  GT_U32  threshold
)
{

#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxResourceHistogramThresholdSet(devNum, cntrNum, threshold);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxResourceHistogramThresholdSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(cntrNum);
    TGF_PARAM_NOT_USED(threshold);


    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxResourceHistogramCounterGet
*
* DESCRIPTION:
*       Gets Histogram Counter.
*
* INPUTS:
*       devNum  - physical device number
*       cntrNum - Histogram Counter number. Range 0..3.
*
* OUTPUTS:
*       cntrPtr - (pointer to) Histogram Counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, cntrNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Histogram Counter is cleared on read.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxResourceHistogramCounterGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  cntrNum,
    OUT GT_U32  *cntrPtr
)
{

#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxResourceHistogramCounterGet(devNum, cntrNum, cntrPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxResourceHistogramCounterGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(cntrNum);
    TGF_PARAM_NOT_USED(cntrPtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxGlobalBufNumberGet
*
* DESCRIPTION:
*       Gets total number of virtual buffers enqueued.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       numberPtr - (pointer to) number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The buffers associated with enqueued multi-target descriptors are
*       counted multiple times, once for each multi-target descriptor instance.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxGlobalBufNumberGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *numberPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxGlobalBufNumberGet(devNum, numberPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxGlobalBufNumberGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(numberPtr);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxGlobalDescNumberGet
*
* DESCRIPTION:
*       Gets total number of descriptors allocated.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       numberPtr - (pointer to) number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxGlobalDescNumberGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *numberPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxGlobalDescNumberGet(devNum, numberPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxGlobalDescNumberGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(numberPtr);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxSharedResourceDescNumberGet
*
* DESCRIPTION:
*       Gets total number of descriptors allocated from the shared descriptors 
*       pool.
*
* INPUTS:
*       devNum  - physical device number
*       poolNum - shared pool number, range 0..7
*
* OUTPUTS:
*       numberPtr - (pointer to) number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong poolNum or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSharedResourceDescNumberGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 poolNum,
    OUT GT_U32 *numberPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxSharedResourceDescNumberGet(devNum, poolNum, numberPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxSharedResourceDescNumberGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(poolNum);
    TGF_PARAM_NOT_USED(numberPtr);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxSharedResourceBufNumberGet
*
* DESCRIPTION:
*       Gets the number of virtual buffers enqueued from the shared buffer pool.
*
* INPUTS:
*       devNum  - physical device number
*       poolNum - shared pool number, range 0..7
*
* OUTPUTS:
*       numberPtr - (pointer to) number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong poolNum or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSharedResourceBufNumberGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 poolNum,
    OUT GT_U32 *numberPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxSharedResourceBufNumberGet(devNum, poolNum, numberPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxSharedResourceBufNumberGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(poolNum);
    TGF_PARAM_NOT_USED(numberPtr);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxSniffedBuffersLimitSet
*
* DESCRIPTION:
*       Set maximal buffers limits for mirrored packets.
*
* INPUTS:
*       devNum            - physical device number
*       rxSniffMaxBufNum  - The number of buffers allocated for packets
*                           forwarded to the ingress analyzer port due to
*                           mirroring. Range 0..0xFFFF
*       txSniffMaxBufNum  - The number of buffers allocated for packets
*                           forwarded to the egress analyzer port due to
*                           mirroring. Range 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSniffedBuffersLimitSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  rxSniffMaxBufNum,
    IN  GT_U32  txSniffMaxBufNum
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxSniffedBuffersLimitSet(devNum, rxSniffMaxBufNum, txSniffMaxBufNum);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxSniffedBuffersLimitSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(rxSniffMaxBufNum);
    TGF_PARAM_NOT_USED(txSniffMaxBufNum);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxSniffedPcktDescrLimitSet
*
* DESCRIPTION:
*       Set maximal descriptors limits for mirrored packets.
*
*
* INPUTS:
*       devNum            - physical device number
*       rxSniffMaxDescNum - The number of descriptors allocated for packets
*                           forwarded to the ingress analyzer port due to
*                           mirroring.
*                           For DxCh1,DxCh2: range 0..0xFFF
*                           For DxCh3,xCat,xCat2: range 0..0x3FFF
*                           For Lion: range 0..0xFFFF
*       txSniffMaxDescNum - The number of descriptors allocated for packets
*                           forwarded to the egress analyzer port due to
*                           mirroring.
*                           For DxCh,DxCh2: range 0..0xFFF
*                           For DxCh3,xCat,xCat2: range 0..0x3FFF
*                           For Lion: range 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSniffedPcktDescrLimitSet
(
    IN    GT_U8                  devNum,
    IN    GT_U32                 rxSniffMaxDescNum,
    IN    GT_U32                 txSniffMaxDescNum
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxSniffedPcktDescrLimitSet(devNum, rxSniffMaxDescNum, txSniffMaxDescNum);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxSniffedPcktDescrLimitSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(rxSniffMaxDescNum);
    TGF_PARAM_NOT_USED(txSniffMaxDescNum);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxSniffedPcktDescrLimitGet
*
* DESCRIPTION:
*       Get maximal descriptors limits for mirrored packets.
*
*
* INPUTS:
*       devNum            - physical device number
*
* OUTPUTS:
*       rxSniffMaxDescNumPtr - (pointer to) The number of descriptors allocated
*                              for packets forwarded to the ingress analyzer
*                              port due to mirroring.
*       txSniffMaxDescNumPtr - (pointer to) The number of descriptors allocated
*                              for packets forwarded to the egress analyzer
*                              port due to mirroring.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSniffedPcktDescrLimitGet
(
    IN    GT_U8                  devNum,
    OUT   GT_U32                *rxSniffMaxDescNumPtr,
    OUT   GT_U32                *txSniffMaxDescNumPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxSniffedPcktDescrLimitGet(devNum, rxSniffMaxDescNumPtr, txSniffMaxDescNumPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortTxSniffedPcktDescrLimitGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(rxSniffMaxDescNumPtr);
    TGF_PARAM_NOT_USED(txSniffMaxDescNumPtr);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxSniffedDescNumberGet
*
* DESCRIPTION:
*       Gets total number of mirrored descriptors allocated.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       rxNumberPtr - (pointer to) number of ingress mirrored descriptors.
*       txNumberPtr - (pointer to) number of egress mirrored descriptors.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSniffedDescNumberGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *rxNumberPtr,
    OUT GT_U32 *txNumberPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxSniffedDescNumberGet(devNum, rxNumberPtr, txNumberPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxSniffedDescNumberGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(rxNumberPtr);
    TGF_PARAM_NOT_USED(txNumberPtr);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxSniffedBufNumberGet
*
* DESCRIPTION:
*       Gets total number of mirrored buffers allocated.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       rxNumberPtr - (pointer to) number of ingress mirrored buffers.
*       txNumberPtr - (pointer to) number of egress mirrored buffers.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSniffedBufNumberGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *rxNumberPtr,
    OUT GT_U32 *txNumberPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxSniffedBufNumberGet(devNum, rxNumberPtr, txNumberPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxSniffedBufNumberGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(rxNumberPtr);
    TGF_PARAM_NOT_USED(txNumberPtr);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxMcastDescNumberGet
*
* DESCRIPTION:
*       Gets total number of multi-target descriptors allocated.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       numberPtr - (pointer to) number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Multi-target descriptors are descriptors that are duplicated to 
*       multiple egress targets, e.g. Multicast, Broadcast, or unknown Unicast
*       flooded packets.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxMcastDescNumberGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *numberPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxMcastDescNumberGet(devNum, numberPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxMcastDescNumberGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(numberPtr);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxMcastBufNumberGet
*
* DESCRIPTION:
*       Gets total number of multi-target buffers allocated (virtual buffers).
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       numberPtr - (pointer to) number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The buffers associated with enqueued multi-target descriptors are
*       counted multiple times, once for each multi-target descriptor instance.
*       Multi-target descriptors are descriptors that are duplicated to 
*       multiple egress targets, e.g. Multicast, Broadcast, or unknown Unicast
*       flooded packets.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxMcastBufNumberGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *numberPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxMcastBufNumberGet(devNum, numberPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxMcastBufNumberGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(numberPtr);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxTcProfileSharedPoolSet
*
* DESCRIPTION:
*       Sets the shared pool associated for traffic class and Profile.
*
* INPUTS:
*       devNum  - physical device number
*       pfSet   - the Profile Set in which the Traffic
*                 Class Drop Parameters is associated
*       tc      - the Traffic Class, range 0..7.
*       poolNum - shared pool associated, range 0..7.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxTcProfileSharedPoolSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT pfSet,
    IN  GT_U8                             tc,
    IN  GT_U32                            poolNum
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxTcProfileSharedPoolSet(devNum, pfSet, tc, poolNum);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxTcProfileSharedPoolSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(pfSet);
    TGF_PARAM_NOT_USED(tc);
    TGF_PARAM_NOT_USED(poolNum);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxTcProfileSharedPoolGet
*
* DESCRIPTION:
*       Gets the shared pool associated for traffic class and Profile.
*
* INPUTS:
*       devNum - physical device number
*       pfSet  - the Profile Set in which the Traffic
*                Class Drop Parameters is associated
*       tc     - the Traffic Class, range 0..7.
*
* OUTPUTS:
*       poolNumPtr - (pointer to) shared pool associated.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxTcProfileSharedPoolGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT pfSet,
    IN  GT_U8                             tc,
    OUT GT_U32                            *poolNumPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxTcProfileSharedPoolGet(devNum, pfSet, tc, poolNumPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxTcProfileSharedPoolGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(pfSet);
    TGF_PARAM_NOT_USED(tc);
    TGF_PARAM_NOT_USED(poolNumPtr);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxSharedPoolLimitsSet
*
* DESCRIPTION:
*       Set maximal descriptors and buffers limits for shared pool.
*
* INPUTS:
*       devNum     - physical device number
*       poolNum    - Shared pool number. Range 0..7
*       maxBufNum  - The number of buffers allocated for a shared pool. 
*                    Range 0..0x3FFF.
*       maxDescNum - The number of descriptors allocated for a shared pool. 
*                    Range 0..0x3FFF.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSharedPoolLimitsSet
(
    IN  GT_U8  devNum,
    IN  GT_U32 poolNum,  
    IN  GT_U32 maxBufNum,
    IN  GT_U32 maxDescNum
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxSharedPoolLimitsSet(devNum, poolNum, maxBufNum, maxDescNum);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxSharedPoolLimitsSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(poolNum);
    TGF_PARAM_NOT_USED(maxBufNum);
    TGF_PARAM_NOT_USED(maxDescNum);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxSharedPoolLimitsGet
*
* DESCRIPTION:
*       Get maximal descriptors and buffers limits for shared pool.
*
* INPUTS:
*       devNum  - physical device number
*       poolNum - Shared pool number. Range 0..7
*
* OUTPUTS:
*       maxBufNumPtr  - (pointer to) The number of buffers allocated for a 
*                       shared pool.
*       maxDescNumPtr - (pointer to) The number of descriptors allocated 
*                       for a shared pool.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSharedPoolLimitsGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 poolNum,  
    OUT GT_U32 *maxBufNumPtr,
    OUT GT_U32 *maxDescNumPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxSharedPoolLimitsGet(devNum, poolNum, maxBufNumPtr, maxDescNumPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxSharedPoolLimitsGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(poolNum);
    TGF_PARAM_NOT_USED(maxBufNumPtr);
    TGF_PARAM_NOT_USED(maxDescNumPtr);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxTcSharedProfileEnableSet
*
* DESCRIPTION:
*       Enable/Disable usage of the shared  descriptors / buffer pool for
*       packets with the traffic class (tc) that are transmited via a
*       port that is associated with the Profile (pfSet).
*
* INPUTS:
*       devNum     - physical device number
*       pfSet      - the Profile Set in which the Traffic
*                  Class Drop Parameters is associated
*       tc         - the Traffic Class, range 0..7.
*       enableMode - Drop Precedence (DPs) enabled mode for sharing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxTcSharedProfileEnableSet
(
    IN  GT_U8                              devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT  pfSet,
    IN  GT_U8                              tc,
    IN  PRV_TGF_PORT_TX_SHARED_DP_MODE_ENT enableMode
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_PORT_TX_SHARED_DP_MODE_ENT dxChEnableMode;
 
    /* convert into device specific format */
    switch(enableMode)
    {
        case PRV_TGF_PORT_TX_SHARED_DP_MODE_DISABLE_E:
            dxChEnableMode = CPSS_PORT_TX_SHARED_DP_MODE_DISABLE_E;
            break;

        case PRV_TGF_PORT_TX_SHARED_DP_MODE_ALL_E:
            dxChEnableMode = CPSS_PORT_TX_SHARED_DP_MODE_ALL_E;
            break;

        case PRV_TGF_PORT_TX_SHARED_DP_MODE_DP0_E:
            dxChEnableMode = CPSS_PORT_TX_SHARED_DP_MODE_DP0_E;
            break;

        case PRV_TGF_PORT_TX_SHARED_DP_MODE_DP0_DP1_E:
            dxChEnableMode = CPSS_PORT_TX_SHARED_DP_MODE_DP0_DP1_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* call device specific API */
    rc = cpssDxChPortTxTcSharedProfileEnableSet(devNum, pfSet, tc, dxChEnableMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxTcSharedProfileEnableSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(pfSet);
    TGF_PARAM_NOT_USED(tc);
    TGF_PARAM_NOT_USED(enableMode);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxTcSharedProfileEnableGet
*
* DESCRIPTION:
*       Gets usage of the shared  descriptors / buffer pool status for
*       packets with the traffic class (tc) that are transmited via a
*       port that is associated with the Profile (pfSet).
*
* INPUTS:
*       devNum - physical device number
*       pfSet  - the Profile Set in which the Traffic
*                Class Drop Parameters is associated
*       tc     - the Traffic Class, range 0..7.
*
* OUTPUTS:
*       enableModePtr - (pointer to) Drop Precedence (DPs) enabled mode for 
*                       sharing.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxTcSharedProfileEnableGet
(
    IN  GT_U8                              devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT  pfSet,
    IN  GT_U8                              tc,
    OUT PRV_TGF_PORT_TX_SHARED_DP_MODE_ENT *enableModePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_PORT_TX_SHARED_DP_MODE_ENT dxChEnableMode;

    /* call device specific API */
    rc = cpssDxChPortTxTcSharedProfileEnableGet(devNum, pfSet, tc, &dxChEnableMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxTcSharedProfileEnableGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert from device specific format */
    switch(dxChEnableMode)
    {
        case CPSS_PORT_TX_SHARED_DP_MODE_DISABLE_E:
            *enableModePtr = PRV_TGF_PORT_TX_SHARED_DP_MODE_DISABLE_E;
            break;

        case CPSS_PORT_TX_SHARED_DP_MODE_ALL_E:
            *enableModePtr = PRV_TGF_PORT_TX_SHARED_DP_MODE_ALL_E;
            break;

        case CPSS_PORT_TX_SHARED_DP_MODE_DP0_E:
            *enableModePtr = PRV_TGF_PORT_TX_SHARED_DP_MODE_DP0_E;
            break;

        case CPSS_PORT_TX_SHARED_DP_MODE_DP0_DP1_E:
            *enableModePtr = PRV_TGF_PORT_TX_SHARED_DP_MODE_DP0_DP1_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(pfSet);
    TGF_PARAM_NOT_USED(tc);
    TGF_PARAM_NOT_USED(enableModePtr);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxProfileWeightedRandomTailDropEnableSet
*
* DESCRIPTION:
*       Enable/disable Weighted Random Tail Drop Threshold to overcome
*       synchronization.
*
* INPUTS:
*       devNum      - device number.
*       pfSet       - the Profile Set in which the Traffic
*                     Class Drop Parameters is associated.
*       dp          - Drop Precedence
*                     (APPLICABLE RANGES: Lion 0..2; xCat2 0..1) 
*       tc          - the Traffic Class, range 0..7.
*                     (APPLICABLE DEVICES: xCat2)
*       enablersPtr - (pointer to) Tail Drop limits enabling 
*                     for Weigthed Random Tail Drop
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxProfileWeightedRandomTailDropEnableSet
(
    IN GT_U8                                       devNum,
    IN CPSS_PORT_TX_DROP_PROFILE_SET_ENT           pfSet,
    IN GT_U32                                      dp,
    IN GT_U8                                       tc,
    IN PRV_TGF_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC *enablersPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS                                rc = GT_OK;
    CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC dxChEnablers = {0};
 
    /* convert into device specific format */
    dxChEnablers.tcDpLimit       = enablersPtr->tcDpLimit;
    dxChEnablers.portLimit       = enablersPtr->portLimit;
    dxChEnablers.tcLimit         = enablersPtr->tcLimit;
    dxChEnablers.sharedPoolLimit = enablersPtr->sharedPoolLimit;

    /* call device specific API */
    rc = cpssDxChPortTxProfileWeightedRandomTailDropEnableSet(devNum, pfSet, dp, tc, &dxChEnablers);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxProfileWeightedRandomTailDropEnableSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(pfSet);
    TGF_PARAM_NOT_USED(dp);
    TGF_PARAM_NOT_USED(tc);
    TGF_PARAM_NOT_USED(enablersPtr);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxTailDropWrtdMasksSet
*
* DESCRIPTION:
*       Sets Weighted Random Tail Drop (WRTD) masks.
*
* INPUTS:
*       devNum     - physical device number
*       maskLsbPtr - WRTD masked least significant bits.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxTailDropWrtdMasksSet
(
    IN GT_U8                         devNum,
    IN PRV_TGF_PORT_TX_WRTD_MASK_LSB *maskLsbPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS                  rc = GT_OK;
    CPSS_PORT_TX_WRTD_MASK_LSB_STC dxChMaskLsb = {0};
 
    /* convert into device specific format */
    dxChMaskLsb.tcDp = maskLsbPtr->tcDp;
    dxChMaskLsb.port = maskLsbPtr->port;
    dxChMaskLsb.tc   = maskLsbPtr->tc;
    dxChMaskLsb.pool = maskLsbPtr->pool;

    /* call device specific API */
    rc = cpssDxChPortTxTailDropWrtdMasksSet(devNum, &dxChMaskLsb);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxTailDropWrtdMasksSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(maskLsbPtr);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxMcastBuffersLimitSet
*
* DESCRIPTION:
*       Set maximal buffers limits for multicast packets.
*
* INPUTS:
*       devNum         - physical device number
*       mcastMaxBufNum - The number of buffers allocated for multicast
*                        packets. Range 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range mcastMaxBufNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None. 
*
*******************************************************************************/
GT_STATUS prvTgfPortTxMcastBuffersLimitSet
(
    IN  GT_U8  devNum,
    IN  GT_U32 mcastMaxBufNum
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxMcastBuffersLimitSet(devNum, mcastMaxBufNum);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxMcastBuffersLimitSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(mcastMaxBufNum);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxMcastBuffersLimitGet
*
* DESCRIPTION:
*       Get maximal buffers limits for multicast packets.
*
* INPUTS:
*       devNum            - physical device number
*
* OUTPUTS:
*       mcastMaxBufNumPtr - (pointer to) the number of buffers allocated
*                           for multicast packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxMcastBuffersLimitGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *mcastMaxBufNumPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxMcastBuffersLimitGet(devNum, mcastMaxBufNumPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxMcastBuffersLimitGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(mcastMaxBufNumPtr);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxMcastPcktDescrLimitSet
*
* DESCRIPTION:
*       Set maximal descriptors limits for multicast packets.
*
* INPUTS:
*       devNum          - physical device number
*       mcastMaxDescNum - The number of descriptors allocated for multicast
*                         packets.
*                         For all devices except Lion and above: in units of 128 
*                         descriptors, the actual number descriptors
*                         allocated will be 128 * mcastMaxDescNum.
*                         For Lion and above: actual descriptors number (granularity 
*                         of 1).
*                         For DxCh,DxCh2,DxCh3,DxChXcat: range 0..0xF80
*                         For Lion and above: range 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxMcastPcktDescrLimitSet
(
    IN    GT_U8  devNum,
    IN    GT_U32 mcastMaxDescNum
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxMcastPcktDescrLimitSet(devNum, mcastMaxDescNum);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxMcastPcktDescrLimitSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(mcastMaxDescNum);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxMcastPcktDescrLimitGet
*
* DESCRIPTION:
*       Get maximal descriptors limits for multicast packets.
*
* INPUTS:
*       devNum              - physical device number
*
* OUTPUTS:
*       mcastMaxDescNumPtr  - (pointer to) the number of descriptors allocated
*                             for multicast packets.
*                             For all devices except Lion and above: in units of 128 
*                             descriptors, the actual number descriptors
*                             allocated will be 128 * mcastMaxDescNum.
*                             For Lion and above: actual descriptors number (granularity 
*                             of 1).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxMcastPcktDescrLimitGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *mcastMaxDescNumPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxMcastPcktDescrLimitGet(devNum, mcastMaxDescNumPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxMcastPcktDescrLimitGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(mcastMaxDescNumPtr);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortSpeedGet
*
* DESCRIPTION:
*       Gets speed for specified port on specified device.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on no initialized SERDES per port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        1.This API also checks if at least one serdes per port was initialized.
*        In case there was no initialized SERDES per port GT_NOT_INITIALIZED is
*        returned.
*
*       2.Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS prvTgfPortSpeedGet
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortSpeedGet(devNum, portNum, speedPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfPortSpeedGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(speedPtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortFlowControlEnableSet
*
* DESCRIPTION:
*       Enable/disable receiving and transmission of 802.3x Flow Control frames
*       in full duplex on specified port on specified device.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - Flow Control state: Both disabled,
*                  Both enabled, Only Rx or Only Tx enabled.
*                  Note: only XG ports can be configured in all 4 options,
*                        Tri-Speed and FE ports may use only first two.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, state or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortFlowControlEnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_PORT_FLOW_CONTROL_ENT  state
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortFlowControlEnableSet(devNum, portNum, state);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortFlowControlEnableSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(state);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxSharingGlobalResourceEnableSet
*
* DESCRIPTION:
*       Enable/Disable sharing of resources for enqueuing of packets.
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE   - enable TX queue resourses sharing
*                 GT_FALSE - disable TX queue resources sharing
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSharingGlobalResourceEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc = GT_OK;

    /* call device specific API */
    rc = cpssDxChPortTxSharingGlobalResourceEnableSet(devNum, enable);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxSharingGlobalResourceEnableSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(enable);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxSchedulerArbGroupSet
*
* DESCRIPTION:
*       Set Traffic Class Queue scheduling arbitration group on
*       specificed profile of specified device.
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Tx Queue scheduler Profile Set in which the arbGroup
*                    parameter is associated.
*       tcQueue    - traffic class queue
*       arbGroup   - scheduling arbitration group:
*                     1) Strict Priority
*                     2) WRR Group 1
*                     3) WRR Group 0
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong profile number, device or arbGroup
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - previous updated operation not yet completed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSchedulerArbGroupSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  profileSet,
    IN  GT_U32                                  tcQueue,
    IN  CPSS_PORT_TX_Q_ARB_GROUP_ENT            arbGroup
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT profile;
    GT_U8     tc;

    profile = (CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT)profileSet;
    tc = (GT_U8)tcQueue;
    /* call device specific API */
    rc = cpssDxChPortTxQArbGroupSet(devNum, tc, arbGroup, profile);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxQArbGroupSet FAILED, rc = [%d]", rc);
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(profileSet);
    TGF_PARAM_NOT_USED(tcQueue);
    TGF_PARAM_NOT_USED(arbGroup);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}


/*******************************************************************************
* prvTgfPortTxSchedulerArbGroupGet
*
* DESCRIPTION:
*       Get Traffic Class Queue scheduling arbitration group on
*       specificed profile of specified device.
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Tx Queue scheduler Profile Set in which the arbGroup
*                    parameter is associated.
*       tcQueue    - traffic class queue 
*
* OUTPUTS:
*       arbGroupPtr   - Pointer to scheduling arbitration group:
*                       1) Strict Priority
*                       2) WRR Group 1
*                       3) WRR Group 0
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong profile number, device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSchedulerArbGroupGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  profileSet,
    IN  GT_U32                                  tcQueue,
    OUT CPSS_PORT_TX_Q_ARB_GROUP_ENT            *arbGroupPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT profile;
    GT_U8     tc;

    profile = (CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT)profileSet;
    tc = (GT_U8)tcQueue;

    /* call device specific API */
    rc = cpssDxChPortTxQArbGroupGet(devNum, tc, profile, arbGroupPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxQArbGroupGet FAILED, rc = [%d]", rc);
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(profileSet);
    TGF_PARAM_NOT_USED(tcQueue);
    TGF_PARAM_NOT_USED(arbGroupPtr);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxSchedulerProfileIndexSet
*
* DESCRIPTION:
*       Bind a port to scheduler profile set.
*
* INPUTS:
*       devNum     - device number
*       portNum    - physical port number,
*                    or the Queue Group number in case of Aggregate Mode
*                    in ExMxPm XG device.
*       index      - The scheduler profile index. range 0..7.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSchedulerProfileIndexSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U32  index
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT profile;

    profile = (CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT)index;
    
    /* call device specific API */
    rc = cpssDxChPortTxBindPortToSchedulerProfileSet(devNum, portNum, profile);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxBindPortToSchedulerProfileSet FAILED, rc = [%d]", rc);
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(index);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortTxSchedulerProfileIndexGet
*
* DESCRIPTION:
*       Get scheduler profile set that is binded to the port.
*
* INPUTS:
*       devNum        - device number
*       portNum       - physical port number,
*                       or the Queue Group number in case of Aggregate Mode
*                       in ExMxPm XG device.
*
* OUTPUTS:
*       indexPtr      - (pointer to) The scheduler profile index. range 0..7.
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSchedulerProfileIndexGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_U32  *indexPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT profile;
    
    /* call device specific API */
    rc = cpssDxChPortTxBindPortToSchedulerProfileGet(devNum, portNum, &profile);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPortTxBindPortToSchedulerProfileGet FAILED, rc = [%d]", rc);
        return rc;
    }

    *indexPtr = (GT_U32) profile;

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(indexPtr);

    return GT_NOT_SUPPORTED;
#endif /* !(defined CHX_FAMILY) */
}



