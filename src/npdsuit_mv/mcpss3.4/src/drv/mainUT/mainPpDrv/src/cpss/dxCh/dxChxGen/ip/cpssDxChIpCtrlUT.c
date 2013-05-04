/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIpCtrlUT.c
*
* DESCRIPTION:
*       Unit Tests for the CPSS DXCH Ip HW control registers APIs
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpCtrl.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>


/* Default valid value for port id */
#define IP_CTRL_VALID_PHY_PORT_CNS  0

/* out of range trunk id */
#define MAX_TRUNK_ID_CNS    128

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpSpecialRouterTriggerEnable
(
    IN GT_U8                           devNum,
    IN CPSS_DXCH_IP_BRG_EXCP_CMD_ENT   bridgeExceptionCmd,
    IN GT_BOOL                         enableRouterTrigger
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpSpecialRouterTriggerEnable)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call with bridgeExceptionCmd [CPSS_DXCH_IP_BRG_UC_IPV4_TRAP_EXCP_CMD_E /
                                       CPSS_DXCH_IP_BRG_UC_IPV6_TRAP_EXCP_CMD_E /
                                       CPSS_DXCH_IP_BRG_UC_IPV4_SOFT_DROP_EXCP_CMD_E /
                                       CPSS_DXCH_IP_BRG_MC_IPV4_TRAP_EXCP_CMD_E /
                                       CPSS_DXCH_IP_BRG_ARP_TRAP_EXCP_CMD_E]
               and enableRouterTrigger [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call with wrong enum values bridgeExceptionCmd and enableRouterTrigger [GT_TRUE].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_DXCH_IP_BRG_EXCP_CMD_ENT   bridgeExceptionCmd;
    GT_BOOL                         enableRouterTrigger;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with bridgeExceptionCmd
           [CPSS_DXCH_IP_BRG_UC_IPV4_TRAP_EXCP_CMD_E /
           CPSS_DXCH_IP_BRG_UC_IPV6_TRAP_EXCP_CMD_E /
           CPSS_DXCH_IP_BRG_UC_IPV4_SOFT_DROP_EXCP_CMD_E /
           CPSS_DXCH_IP_BRG_MC_IPV4_TRAP_EXCP_CMD_E /
           CPSS_DXCH_IP_BRG_ARP_TRAP_EXCP_CMD_E] and enableRouterTrigger
           [GT_FALSE and GT_TRUE]. Expected: GT_OK.    */

        bridgeExceptionCmd = CPSS_DXCH_IP_BRG_UC_IPV4_TRAP_EXCP_CMD_E;
        enableRouterTrigger = GT_FALSE;

        st = cpssDxChIpSpecialRouterTriggerEnable(dev, bridgeExceptionCmd, enableRouterTrigger);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, bridgeExceptionCmd, enableRouterTrigger);

        bridgeExceptionCmd = CPSS_DXCH_IP_BRG_UC_IPV6_TRAP_EXCP_CMD_E;
        enableRouterTrigger = GT_FALSE;

        st = cpssDxChIpSpecialRouterTriggerEnable(dev, bridgeExceptionCmd, enableRouterTrigger);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, bridgeExceptionCmd, enableRouterTrigger);

        bridgeExceptionCmd = CPSS_DXCH_IP_BRG_UC_IPV4_SOFT_DROP_EXCP_CMD_E;
        enableRouterTrigger = GT_TRUE;

        st = cpssDxChIpSpecialRouterTriggerEnable(dev, bridgeExceptionCmd, enableRouterTrigger);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, bridgeExceptionCmd, enableRouterTrigger);

        bridgeExceptionCmd = CPSS_DXCH_IP_BRG_MC_IPV4_TRAP_EXCP_CMD_E;
        enableRouterTrigger = GT_TRUE;

        st = cpssDxChIpSpecialRouterTriggerEnable(dev, bridgeExceptionCmd, enableRouterTrigger);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, bridgeExceptionCmd, enableRouterTrigger);

        bridgeExceptionCmd = CPSS_DXCH_IP_BRG_ARP_TRAP_EXCP_CMD_E;
        enableRouterTrigger = GT_TRUE;

        st = cpssDxChIpSpecialRouterTriggerEnable(dev, bridgeExceptionCmd, enableRouterTrigger);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, bridgeExceptionCmd, enableRouterTrigger);


        /*
            1.2. Call with wrong enum values bridgeExceptionCmd and enableRouterTrigger [GT_TRUE].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpSpecialRouterTriggerEnable
                            (dev, bridgeExceptionCmd, enableRouterTrigger),
                            bridgeExceptionCmd);
    }

    bridgeExceptionCmd = CPSS_DXCH_IP_BRG_UC_IPV4_TRAP_EXCP_CMD_E;
    enableRouterTrigger = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpSpecialRouterTriggerEnable(dev, bridgeExceptionCmd, enableRouterTrigger);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpSpecialRouterTriggerEnable(dev, bridgeExceptionCmd, enableRouterTrigger);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpExceptionCommandSet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_IP_EXCEPTION_TYPE_ENT  exceptionType,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    IN CPSS_PACKET_CMD_ENT              command
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpExceptionCommandSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with exceptionType [CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E /
                                  CPSS_DXCH_IP_EXCP_UC_ILLEGAL_ADDRESS_E /
                                  CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E],
                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                  CPSS_IP_PROTOCOL_IPV6_E /
                                  CPSS_IP_PROTOCOL_IPV4V6_E]
                    and command [CPSS_PACKET_CMD_DROP_HARD_E /
                                 CPSS_PACKET_CMD_DROP_SOFT_E /
                                 CPSS_PACKET_CMD_TRAP_TO_CPU_E].
       Expected: GT_OK.
    1.2. Call with exceptionType[CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E /
                                 CPSS_DXCH_IP_EXCP_MC_HDR_ERROR_E /
                                 CPSS_DXCH_IP_EXCP_UC_SIP_SA_FAIL_E],
                  protocolStack [CPSS_IP_PROTOCOL_IPV4_E]
         and command [CPSS_PACKET_CMD_MIRROR_TO_CPU_E] (not possible to set).
    Expected: non GT_OK.
    1.3. Call with wrong enum values exceptionType and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with wrong enum values protocolStack and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with wrong enum values command and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/

    GT_STATUS                        st = GT_OK;
    GT_U8                            dev;
    CPSS_DXCH_IP_EXCEPTION_TYPE_ENT  exceptionType;
    CPSS_IP_PROTOCOL_STACK_ENT       protocolStack;
    CPSS_PACKET_CMD_ENT              command;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with exceptionType [CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E /
                                          CPSS_DXCH_IP_EXCP_UC_ILLEGAL_ADDRESS_E /
                                          CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E],
                           protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                          CPSS_IP_PROTOCOL_IPV6_E /
                                          CPSS_IP_PROTOCOL_IPV4V6_E]
                            and command [CPSS_PACKET_CMD_DROP_HARD_E /
                                         CPSS_PACKET_CMD_DROP_SOFT_E /
                                         CPSS_PACKET_CMD_TRAP_TO_CPU_E].
           Expected: GT_OK.
        */
        /* call with first group of params values */
        exceptionType = CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        command = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssDxChIpExceptionCommandSet(dev, exceptionType, protocolStack, command);
        if (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, exceptionType, protocolStack, command);
        else
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        /* call with second group of params values */
        exceptionType = CPSS_DXCH_IP_EXCP_UC_ILLEGAL_ADDRESS_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        command = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssDxChIpExceptionCommandSet(dev, exceptionType, protocolStack, command);
        if (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, exceptionType, protocolStack, command);
        else
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        /* call with third group of params values */
        exceptionType = CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
        command = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssDxChIpExceptionCommandSet(dev, exceptionType, protocolStack, command);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, exceptionType, protocolStack, command);


        /*
            1.2. Call with exceptionType[CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E /
                                         CPSS_DXCH_IP_EXCP_MC_HDR_ERROR_E /
                                         CPSS_DXCH_IP_EXCP_UC_SIP_SA_FAIL_E],
                          protocolStack [CPSS_IP_PROTOCOL_IPV4_E]
                 and command [CPSS_PACKET_CMD_MIRROR_TO_CPU_E] (not possible to set).
            Expected: non GT_OK.
        */

        /*call with exceptionType = CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E; */
        exceptionType = CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        command = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssDxChIpExceptionCommandSet(dev, exceptionType, protocolStack, command);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, exceptionType, protocolStack, command);

        /*call with exceptionType = CPSS_DXCH_IP_EXCP_MC_HDR_ERROR_E; */
        exceptionType = CPSS_DXCH_IP_EXCP_MC_HDR_ERROR_E;

        st = cpssDxChIpExceptionCommandSet(dev, exceptionType, protocolStack, command);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, exceptionType, protocolStack, command);

        /*call with exceptionType = CPSS_DXCH_IP_EXCP_UC_SIP_SA_FAIL_E; */
        exceptionType = CPSS_DXCH_IP_EXCP_UC_SIP_SA_FAIL_E;

        st = cpssDxChIpExceptionCommandSet(dev, exceptionType, protocolStack, command);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, exceptionType, protocolStack, command);

        /*
            1.3. Call with wrong enum values exceptionType and other valid parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpExceptionCommandSet
                            (dev, exceptionType, protocolStack, command),
                            exceptionType);

        /*
            1.4. Call with wrong enum values protocolStack and other valid parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpExceptionCommandSet
                            (dev, exceptionType, protocolStack, command),
                            protocolStack);

        /*
            1.5. Call with wrong enum values command and other valid parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpExceptionCommandSet
                           (dev, exceptionType, protocolStack, command),
                           command);
    }

    exceptionType = CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E;
    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    command = CPSS_PACKET_CMD_DROP_HARD_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpExceptionCommandSet(dev, exceptionType, protocolStack, command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpExceptionCommandSet(dev, exceptionType, protocolStack, command);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpUcRouteAgingModeSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  refreshEnable
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpUcRouteAgingModeSet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with refreshEnable [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_BOOL                 refreshEnable;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with refreshEnable [GT_TRUE and GT_FALSE].
           Expected: GT_OK. */

        refreshEnable = GT_TRUE;

        st = cpssDxChIpUcRouteAgingModeSet(dev, refreshEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, refreshEnable);

        refreshEnable = GT_FALSE;

        st = cpssDxChIpUcRouteAgingModeSet(dev, refreshEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, refreshEnable);
    }

    refreshEnable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpUcRouteAgingModeSet(dev, refreshEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpUcRouteAgingModeSet(dev, refreshEnable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpRouterSourceIdSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet,
    IN  GT_U32                          sourceId
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpRouterSourceIdSet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call with ucMcSet [CPSS_IP_UNICAST_E / CPSS_IP_MULTICAST_E] and sourceId [0 / 31].
Expected: GT_OK.
1.2. Call with sourceId[32] and other valid parameters from 1.1.
Expected: non GT_OK.
1.3. Call with wrong enum values ucMcSet and other valid parameters from 1.1.
Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet;
    GT_U32                          sourceId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with ucMcSet [CPSS_IP_UNICAST_E / CPSS_IP_MULTICAST_E]
           and sourceId [0 / 31]. Expected: GT_OK.   */

        ucMcSet = CPSS_IP_UNICAST_E;
        sourceId = 0;

        st = cpssDxChIpRouterSourceIdSet(dev, ucMcSet, sourceId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucMcSet, sourceId);

        ucMcSet = CPSS_IP_MULTICAST_E;
        sourceId = 31;

        st = cpssDxChIpRouterSourceIdSet(dev, ucMcSet, sourceId);

        if (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucMcSet, sourceId);
        else
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);

        /* 1.2. Call with sourceId[32] and other valid parameters
           from 1.1. Expected: non GT_OK.   */

        ucMcSet = CPSS_IP_UNICAST_E;
        sourceId = 32;

        st = cpssDxChIpRouterSourceIdSet(dev, ucMcSet, sourceId);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucMcSet, sourceId);

        sourceId = 0;

        /*
            1.3. Call with wrong enum values ucMcSet  and other valid parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        if (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)
        {
            UTF_ENUMS_CHECK_MAC(cpssDxChIpRouterSourceIdSet
                            (dev, ucMcSet, sourceId),
                            ucMcSet);
        }
    }

    ucMcSet = CPSS_IP_UNICAST_E;
    sourceId = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpRouterSourceIdSet(dev, ucMcSet, sourceId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpRouterSourceIdSet(dev, ucMcSet, sourceId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  ctrlMultiTargetTCQueue,
    IN  GT_U32  failRpfMultiTargetTCQueue
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with ctrlMultiTargetTCQueue [0 / 3] and failRpfMultiTargetTCQueue [3 / 0].
Expected: GT_OK.
1.2. Call with ctrlMultiTargetTCQueue [4] and failRpfMultiTargetTCQueue[0]
Expected: NOT GT_OK.
1.3. Call with ctrlMultiTargetTCQueue[0] and failRpfMultiTargetTCQueue[4]
Expected: NOT GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U32                  ctrlMultiTargetTCQueue;
    GT_U32                  failRpfMultiTargetTCQueue;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with ctrlMultiTargetTCQueue [0 / 3] and
           failRpfMultiTargetTCQueue [3 / 0]. Expected: GT_OK.  */

        ctrlMultiTargetTCQueue = 0;
        failRpfMultiTargetTCQueue = 3;

        st = cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet(dev, ctrlMultiTargetTCQueue,
                                                                failRpfMultiTargetTCQueue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ctrlMultiTargetTCQueue,
                                                    failRpfMultiTargetTCQueue);

        ctrlMultiTargetTCQueue = 3;
        failRpfMultiTargetTCQueue = 0;

        st = cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet(dev, ctrlMultiTargetTCQueue,
                                                                failRpfMultiTargetTCQueue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ctrlMultiTargetTCQueue,
                                                    failRpfMultiTargetTCQueue);

        /* 1.2. Call with ctrlMultiTargetTCQueue [4]
           and failRpfMultiTargetTCQueue[0]. Expected: NOT GT_OK.   */

        ctrlMultiTargetTCQueue = 4;
        failRpfMultiTargetTCQueue = 0;

        st = cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet(dev, ctrlMultiTargetTCQueue,
                                                                failRpfMultiTargetTCQueue);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ctrlMultiTargetTCQueue);

        /* 1.3. Call with ctrlMultiTargetTCQueue[0] and
           failRpfMultiTargetTCQueue[4]. Expected: NOT GT_OK.   */

        ctrlMultiTargetTCQueue = 0;
        failRpfMultiTargetTCQueue = 4;

        st = cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet(dev, ctrlMultiTargetTCQueue,
                                                                failRpfMultiTargetTCQueue);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, failRpfMultiTargetTCQueue = %d",
                                         dev, failRpfMultiTargetTCQueue);
    }

    ctrlMultiTargetTCQueue = 0;
    failRpfMultiTargetTCQueue = 3;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet(dev, ctrlMultiTargetTCQueue,
                                                                failRpfMultiTargetTCQueue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet(dev, ctrlMultiTargetTCQueue,
                                                            failRpfMultiTargetTCQueue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpQosProfileToMultiTargetTCQueueMapSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  qosProfile,
    IN  GT_U32  multiTargeTCQueue
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpQosProfileToMultiTargetTCQueueMapSet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with qosProfile [1 / 100 / 127] and multiTargeTCQueue [1 / 2 / 3].
Expected: GT_OK.
1.2. Call with qosProfile [1] and multiTargeTCQueue [1] (already set).
Expected: GT_OK.
1.3. Call with qosProfile [128] (out of range) and multiTargeTCQueue [0] (out of range) Expected: NOT GT_OK
1.4. Call with qosProfile [0] and multiTargeTCQueue [4] (out of range)
Expected: NOT GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U32                  qosProfile;
    GT_U32                  multiTargeTCQueue;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with qosProfile [1 / 100 / 127] and multiTargeTCQueue
           [1 / 2 / 3]. Expected: GT_OK.    */


        qosProfile = 1;
        multiTargeTCQueue = 1;

        st = cpssDxChIpQosProfileToMultiTargetTCQueueMapSet(dev, qosProfile,
                                                                multiTargeTCQueue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, qosProfile,
                                                    multiTargeTCQueue);

        qosProfile = 100;
        multiTargeTCQueue = 2;

        st = cpssDxChIpQosProfileToMultiTargetTCQueueMapSet(dev, qosProfile,
                                                                multiTargeTCQueue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, qosProfile,
                                                    multiTargeTCQueue);

        qosProfile = 127;
        multiTargeTCQueue = 3;

        st = cpssDxChIpQosProfileToMultiTargetTCQueueMapSet(dev, qosProfile,
                                                                multiTargeTCQueue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, qosProfile,
                                                    multiTargeTCQueue);


        /* 1.2. Call with qosProfile [1] and multiTargeTCQueue [1] (already set).
            Expected: GT_OK.    */

        qosProfile = 1;
        multiTargeTCQueue = 1;

        st = cpssDxChIpQosProfileToMultiTargetTCQueueMapSet(dev, qosProfile,
                                                                multiTargeTCQueue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, qosProfile,
                                                    multiTargeTCQueue);

        /* 1.3. Call with qosProfile [128] (out of range) and multiTargeTCQueue [0]
           (out of range) Expected: NOT GT_OK   */
        qosProfile = 128;
        multiTargeTCQueue = 0;

        st = cpssDxChIpQosProfileToMultiTargetTCQueueMapSet(dev, qosProfile,
                                                                multiTargeTCQueue);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, qosProfile = %d",
                                         dev, qosProfile);

        /* 1.4. Call with qosProfile [0] and multiTargeTCQueue [4]
           (out of range) Expected: NOT GT_OK   */

        qosProfile = 0;
        multiTargeTCQueue = 4;

        st = cpssDxChIpQosProfileToMultiTargetTCQueueMapSet(dev, qosProfile,
                                                                multiTargeTCQueue);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, multiTargeTCQueue = %d",
                                         dev, multiTargeTCQueue);
    }

    qosProfile = 1;
    multiTargeTCQueue = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpQosProfileToMultiTargetTCQueueMapSet(dev, qosProfile,
                                                                multiTargeTCQueue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpQosProfileToMultiTargetTCQueueMapSet(dev, qosProfile,
                                                            multiTargeTCQueue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpMultiTargetQueueFullDropCntGet
(
    IN   GT_U8    devNum,
    OUT  GT_U32   *dropPktsPtr
)*/
UTF_TEST_CASE_MAC(cpssDxChIpMultiTargetQueueFullDropCntGet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with non-null dropPktsPtr. Expected: GT_OK.
1.2. Call with dropPktsPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U32                  dropPkts;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with non-null dropPktsPtr. Expected: GT_OK.    */

        st = cpssDxChIpMultiTargetQueueFullDropCntGet(dev, &dropPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call with dropPktsPtr [NULL]. Expected: GT_BAD_PTR. */

        st = cpssDxChIpMultiTargetQueueFullDropCntGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dropPktsPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpMultiTargetQueueFullDropCntGet(dev, &dropPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpMultiTargetQueueFullDropCntGet(dev, &dropPkts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpMultiTargetQueueFullDropCntSet
(
    IN GT_U8     devNum,
    IN GT_U32    dropPkts
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpMultiTargetQueueFullDropCntSet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with dropPkts [10]. Expected: GT_OK.
1.2. Call cpssDxChIpMultiTargetQueueFullDropCntGet. Expected: GT_OK and the same dropPkts.
1.3. Call with dropPkts [0xFFFFFFFF] (no any onstraints). Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U32                  dropPkts;
    GT_U32                  dropPktsRet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with dropPkts [10]. Expected: GT_OK.   */

        dropPkts = 10;

        st = cpssDxChIpMultiTargetQueueFullDropCntSet(dev, dropPkts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropPkts);

        /* 1.2. Call cpssDxChIpMultiTargetQueueFullDropCntGet.
           Expected: GT_OK and the same dropPkts.   */

        st = cpssDxChIpMultiTargetQueueFullDropCntGet(dev, &dropPktsRet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(dropPkts, dropPktsRet,
            "cpssDxChIpMultiTargetQueueFullDropCntGet: get another value than was set: dev = %d, dropPkts = %d",
                                     dev, dropPktsRet);

        /* 1.3. Call with dropPkts [0xFFFFFFFF] (no any onstraints).
           Expected: GT_OK. */

        dropPkts = 0xFFFFFFFF;

        st = cpssDxChIpMultiTargetQueueFullDropCntSet(dev, dropPkts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropPkts);
    }

    dropPkts = 10;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpMultiTargetQueueFullDropCntSet(dev, dropPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpMultiTargetQueueFullDropCntSet(dev, dropPkts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpMultiTargetTCQueueSchedModeSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  multiTargetTcQueue,
    IN  CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT schedulingMode,
    IN  GT_U32                                  queueWeight
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpMultiTargetTCQueueSchedModeSet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with multiTargeTCQueue [0 / 3], schedulingMode[CPSS_DXCH_IP_MT_TC_QUEUE_SDWRR_SCHED_MODE_E / CPSS_DXCH_IP_MT_TC_QUEUE_SP_SCHED_MODE_E], queueWeight[10 / 255].
Expected: GT_OK.
1.2. Call with multiTargeTCQueue [0], schedulingMode[CPSS_DXCH_IP_MT_TC_QUEUE_SP_SCHED_MODE_E], queueWeight[10].
Expected: GT_OK.
1.3. Check out-of-range for queueWeight. Call with multiTargeTCQueue [0], schedulingMode[CPSS_DXCH_IP_MT_TC_QUEUE_ SDWRR _SCHED_MODE_E], queueWeight[256].
Expected: non GT_OK.
1.4. Call with out-of-range multiTargeTCQueue [4] and other parameters from 1.1.
Expected: NOT GT_OK.
1.5. Call with wrong enum values schedulingMode  and other valid parameters from 1.1.
Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U32                                  multiTargetTcQueue;
    CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT schedulingMode;
    GT_U32                                  queueWeight;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with multiTargetTcQueue [0 / 3], schedulingMode
           [CPSS_DXCH_IP_MT_TC_QUEUE_SDWRR_SCHED_MODE_E /
           CPSS_DXCH_IP_MT_TC_QUEUE_SP_SCHED_MODE_E], queueWeight[10 / 255].
            Expected: GT_OK.    */

        multiTargetTcQueue = 0;
        schedulingMode = CPSS_DXCH_IP_MT_TC_QUEUE_SDWRR_SCHED_MODE_E;
        queueWeight = 10;

        st = cpssDxChIpMultiTargetTCQueueSchedModeSet(dev, multiTargetTcQueue,
                                                      schedulingMode, queueWeight);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev,  multiTargetTcQueue,
                                    schedulingMode, queueWeight);

        queueWeight = 255;

        st = cpssDxChIpMultiTargetTCQueueSchedModeSet(dev, multiTargetTcQueue,
                                                      schedulingMode, queueWeight);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev,  multiTargetTcQueue,
                                    schedulingMode, queueWeight);


        multiTargetTcQueue = 3;
        schedulingMode = CPSS_DXCH_IP_MT_TC_QUEUE_SP_SCHED_MODE_E;

        st = cpssDxChIpMultiTargetTCQueueSchedModeSet(dev, multiTargetTcQueue,
                                                      schedulingMode, queueWeight);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev,  multiTargetTcQueue,
                                    schedulingMode, queueWeight);

        /* 1.2. Call with multiTargetTcQueue [0], schedulingMode
          [CPSS_DXCH_IP_MT_TC_QUEUE_SP_SCHED_MODE_E], queueWeight[10].
           Expected: GT_OK. */

        multiTargetTcQueue = 0;
        schedulingMode = CPSS_DXCH_IP_MT_TC_QUEUE_SP_SCHED_MODE_E;
        queueWeight = 10;

        st = cpssDxChIpMultiTargetTCQueueSchedModeSet(dev, multiTargetTcQueue,
                                                      schedulingMode, queueWeight);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev,  multiTargetTcQueue,
                                    schedulingMode, queueWeight);

        /* 1.3. Check out-of-range for queueWeight. Call with
           multiTargeTCQueue [0], schedulingMode
           [CPSS_DXCH_IP_MT_TC_QUEUE_SDWRR _SCHED_MODE_E],
           queueWeight[256]. Expected: non GT_OK.   */

        multiTargetTcQueue = 0;
        schedulingMode = CPSS_DXCH_IP_MT_TC_QUEUE_SDWRR_SCHED_MODE_E;
        queueWeight = 256;

        st = cpssDxChIpMultiTargetTCQueueSchedModeSet(dev, multiTargetTcQueue,
                                                      schedulingMode, queueWeight);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, queueWeight = %d", dev, queueWeight);

        /* 1.4. Call with out-of-range multiTargeTCQueue [4] and
           other parameters from 1.1. Expected: NOT GT_OK.  */

        multiTargetTcQueue = 4;
        schedulingMode = CPSS_DXCH_IP_MT_TC_QUEUE_SDWRR_SCHED_MODE_E;
        queueWeight = 10;

        st = cpssDxChIpMultiTargetTCQueueSchedModeSet(dev, multiTargetTcQueue,
                                                      schedulingMode, queueWeight);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, multiTargetTcQueue = %d",
                                         dev,  multiTargetTcQueue);

        multiTargetTcQueue = 0; /* restore */

        /* 1.5. Call with wrong enum values schedulingMode  and other
           valid parameters from 1.1. Expected: GT_BAD_PARAM.   */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpMultiTargetTCQueueSchedModeSet
                            (dev, multiTargetTcQueue, schedulingMode, queueWeight),
                            schedulingMode);
    }

    multiTargetTcQueue = 0;
    schedulingMode = CPSS_DXCH_IP_MT_TC_QUEUE_SDWRR_SCHED_MODE_E;
    queueWeight = 10;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpMultiTargetTCQueueSchedModeSet(dev, multiTargetTcQueue,
                                                      schedulingMode, queueWeight);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpMultiTargetTCQueueSchedModeSet(dev, multiTargetTcQueue,
                                                  schedulingMode, queueWeight);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpBridgeServiceEnable
(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_IP_BRG_SERVICE_ENT                        bridgeService,
    IN  CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT    enableDisableMode,
    IN  GT_BOOL                                             enableService
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpBridgeServiceEnable)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with bridgeService [CPSS_DXCH_IP_UC_RPF_CHECK_BRG_SERVICE_E / CPSS_DXCH_IP_SIP_SA_CHECK_BRG_SERVICE_E / CPSS_DXCH_IP_SIP_FILTER_BRG_SERVICE_E], enableDisableMode [CPSS_DXCH_IP_BRG_SERVICE_IPV4_ENABLE_DISABLE_E / CPSS_DXCH_IP_BRG_SERVICE_IPV6_ENABLE_DISABLE_E / CPSS_DXCH_IP_BRG_SERVICE_ARP_ENABLE_DISABLE_E] and enableService [GT_FALSE and GT_ TRUE].
Expected: GT_OK.
1.2. Call with CPSS_DXCH_IP_BRG_SERVICE_LAST_E and wrong enum values bridgeService and other valid parameters from 1.1.
Expected: GT_BAD_PARAM.
1.3. Call with wrong enum values enableDisableMode  and other valid parameters from 1.1.
Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_DXCH_IP_BRG_SERVICE_ENT                        bridgeService;
    CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT    enableDisableMode;
    GT_BOOL                                             enableService;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with bridgeService [CPSS_DXCH_IP_UC_RPF_CHECK_BRG_SERVICE_E
           / CPSS_DXCH_IP_SIP_SA_CHECK_BRG_SERVICE_E /
             CPSS_DXCH_IP_SIP_FILTER_BRG_SERVICE_E], enableDisableMode
             [CPSS_DXCH_IP_BRG_SERVICE_IPV4_ENABLE_DISABLE_E /
             CPSS_DXCH_IP_BRG_SERVICE_IPV6_ENABLE_DISABLE_E /
             CPSS_DXCH_IP_BRG_SERVICE_ARP_ENABLE_DISABLE_E] and
             enableService [GT_FALSE and GT_ TRUE]. Expected: GT_OK.    */


        bridgeService = CPSS_DXCH_IP_UC_RPF_CHECK_BRG_SERVICE_E;
        enableDisableMode = CPSS_DXCH_IP_BRG_SERVICE_IPV4_ENABLE_DISABLE_E;
        enableService = GT_FALSE;

        st = cpssDxChIpBridgeServiceEnable(dev, bridgeService, enableDisableMode,
                                           enableService);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, bridgeService, enableDisableMode,
                                           enableService);

        bridgeService = CPSS_DXCH_IP_SIP_FILTER_BRG_SERVICE_E;
        enableDisableMode = CPSS_DXCH_IP_BRG_SERVICE_IPV6_ENABLE_DISABLE_E;
        enableService = GT_TRUE;

        st = cpssDxChIpBridgeServiceEnable(dev, bridgeService, enableDisableMode,
                                           enableService);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, bridgeService, enableDisableMode,
                                           enableService);

        bridgeService = CPSS_DXCH_IP_UC_RPF_CHECK_BRG_SERVICE_E;
        enableDisableMode = CPSS_DXCH_IP_BRG_SERVICE_ARP_ENABLE_DISABLE_E;
        enableService = GT_TRUE;

        st = cpssDxChIpBridgeServiceEnable(dev, bridgeService, enableDisableMode,
                                           enableService);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, bridgeService, enableDisableMode,
                                           enableService);

        /* 1.2. Call with CPSS_DXCH_IP_BRG_SERVICE_LAST_E and wrong enum values
          bridgeService and other valid parameters from 1.1.  Expected: GT_BAD_PARAM. */

        bridgeService = CPSS_DXCH_IP_BRG_SERVICE_LAST_E;

        st = cpssDxChIpBridgeServiceEnable(dev, bridgeService, enableDisableMode,
                                           enableService);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, bridgeService = %d", dev, bridgeService);

        bridgeService = CPSS_DXCH_IP_UC_RPF_CHECK_BRG_SERVICE_E;    /* restore */

        UTF_ENUMS_CHECK_MAC(cpssDxChIpBridgeServiceEnable
                            (dev, bridgeService, enableDisableMode, enableService),
                            bridgeService);

        /* 1.3. Call with wrong enum values enableDisableMode  and
           other valid parameters from 1.1. Expected: GT_BAD_PARAM. */

        UTF_ENUMS_CHECK_MAC(cpssDxChIpBridgeServiceEnable
                            (dev, bridgeService, enableDisableMode, enableService),
                            enableDisableMode);
    }

    bridgeService = CPSS_DXCH_IP_UC_RPF_CHECK_BRG_SERVICE_E;
    enableDisableMode = CPSS_DXCH_IP_BRG_SERVICE_IPV4_ENABLE_DISABLE_E;
    enableService = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpBridgeServiceEnable(dev, bridgeService, enableDisableMode,
                                           enableService);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpBridgeServiceEnable(dev, bridgeService, enableDisableMode,
                                       enableService);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpMllBridgeEnable
(
    IN   GT_U8      devNum,
    IN   GT_BOOL    mllBridgeEnable
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpMllBridgeEnable)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with mllBridgeEnable [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_BOOL                 mllBridgeEnable;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with mllBridgeEnable [GT_TRUE and GT_FALSE].
           Expected: GT_OK. */

        mllBridgeEnable = GT_TRUE;

        st = cpssDxChIpMllBridgeEnable(dev, mllBridgeEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mllBridgeEnable);

        mllBridgeEnable = GT_FALSE;

        st = cpssDxChIpMllBridgeEnable(dev, mllBridgeEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mllBridgeEnable);
    }

    mllBridgeEnable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpMllBridgeEnable(dev, mllBridgeEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpMllBridgeEnable(dev, mllBridgeEnable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpMultiTargetRateShaperSet
(
    IN   GT_U8      devNum,
    IN   GT_BOOL    multiTargetRateShaperEnable,
    IN   GT_U32     windowSize
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpMultiTargetRateShaperSet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with multiTargetRateShaperEnable [GT_TRUE and GT_FALSE] and windowSize [10 / 0xFFFF].
Expected: GT_OK.
1.2. Call with multiTargetRateShaperEnable [GT_FALSE] (in this case windowSize is not relevant) and windowSize [0xFFFFFFFF].
Expected: GT_OK.
1.3. Call with multiTargetRateShaperEnable [GT_TRUE] (in this case windowSize is relevant) and out-of-range windowSize [0x10000].
Expected: non GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_BOOL    multiTargetRateShaperEnable;
    GT_U32     windowSize;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with multiTargetRateShaperEnable [GT_TRUE and
           GT_FALSE] and windowSize [10 / 0xFFFF]. Expected: GT_OK.  */

        multiTargetRateShaperEnable = GT_TRUE;
        windowSize = 10;

        st = cpssDxChIpMultiTargetRateShaperSet(dev, multiTargetRateShaperEnable,
                                                windowSize);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev,  multiTargetRateShaperEnable,
                                                windowSize);

        windowSize = 0xFFFF;

        st = cpssDxChIpMultiTargetRateShaperSet(dev, multiTargetRateShaperEnable,
                                                windowSize);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev,  multiTargetRateShaperEnable,
                                                windowSize);

        multiTargetRateShaperEnable = GT_FALSE;

        st = cpssDxChIpMultiTargetRateShaperSet(dev, multiTargetRateShaperEnable,
                                                windowSize);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev,  multiTargetRateShaperEnable,
                                                windowSize);

        /* 1.2. Call with multiTargetRateShaperEnable [GT_FALSE]
          (in this case windowSize is not relevant) and windowSize [0xFFFFFFFF].
            Expected: GT_OK.    */

        windowSize = 0xFFFFFFFF;

        st = cpssDxChIpMultiTargetRateShaperSet(dev, multiTargetRateShaperEnable,
                                                windowSize);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev,  multiTargetRateShaperEnable,
                                                windowSize);

        /* 1.3. Call with multiTargetRateShaperEnable [GT_TRUE] (in this
          case windowSize is relevant) and out-of-range windowSize [0x10000].
          Expected: non GT_OK.  */

        multiTargetRateShaperEnable = GT_TRUE;
        windowSize = 0x10000;

        st = cpssDxChIpMultiTargetRateShaperSet(dev, multiTargetRateShaperEnable,
                                                windowSize);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev,  multiTargetRateShaperEnable,
                                                windowSize);
    }

    multiTargetRateShaperEnable = GT_TRUE;
    windowSize = 10;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpMultiTargetRateShaperSet(dev, multiTargetRateShaperEnable,
                                                windowSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpMultiTargetRateShaperSet(dev, multiTargetRateShaperEnable,
                                            windowSize);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpMultiTargetUcSchedModeSet
(
    IN   GT_U8                              devNum,
    IN   GT_BOOL                            ucSPEnable,
    IN   GT_U32                             ucWeight,
    IN   GT_U32                             mcWeight,
    IN   CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT   schedMtu
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpMultiTargetUcSchedModeSet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with ucSPEnable [GT_TRUE], ucWeight[10 / 255], mcWeight[10 / 255] and schedMtu [CPSS_DXCH_IP_MT_UC_SCHED_MTU_2K_E / CPSS_DXCH_IP_MT_UC_SCHED_MTU_8K_E].
Expected: GT_OK.
1.2. Call with ucSPEnable [GT_TRUE] (in this case ucWeight is not relevant), ucWeight [0xFFFFFFFF] and other parameters from 1.1.
Expected: GT_OK.
1.3. Call with ucSPEnable [GT_FALSE] (in this case ucWeight is relevant), ucWeight [256] (out of range) and other parameters from 1.1.
Expected: non GT_OK.
1.4. Call with ucSPEnable [GT_FALSE], mcWeight [256] (out of range) and other parameters from 1.1.
Expected: non GT_OK.
1.5. Call with wrong enum values schedMtu  and other valid parameters from 1.1.
Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_BOOL                            ucSPEnable;
    GT_U32                             ucWeight;
    GT_U32                             mcWeight;
    CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT   schedMtu;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with ucSPEnable [GT_TRUE], ucWeight[10 / 255], mcWeight
           [10 / 255] and schedMtu [CPSS_DXCH_IP_MT_UC_SCHED_MTU_2K_E /
           CPSS_DXCH_IP_MT_UC_SCHED_MTU_8K_E]. Expected: GT_OK. */

        ucSPEnable = GT_TRUE;
        ucWeight = 255;
        mcWeight = 255;
        schedMtu = CPSS_DXCH_IP_MT_UC_SCHED_MTU_2K_E;

        st = cpssDxChIpMultiTargetUcSchedModeSet(dev, ucSPEnable, ucWeight,
                                                 mcWeight, schedMtu);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ucSPEnable, ucWeight,
                                                 mcWeight, schedMtu);

        ucWeight = 10;
        mcWeight = 10;

        st = cpssDxChIpMultiTargetUcSchedModeSet(dev, ucSPEnable, ucWeight,
                                                 mcWeight, schedMtu);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ucSPEnable, ucWeight,
                                                 mcWeight, schedMtu);


        schedMtu = CPSS_DXCH_IP_MT_UC_SCHED_MTU_8K_E;

        st = cpssDxChIpMultiTargetUcSchedModeSet(dev, ucSPEnable, ucWeight,
                                                 mcWeight, schedMtu);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ucSPEnable, ucWeight,
                                                 mcWeight, schedMtu);

        /* 1.2. Call with ucSPEnable [GT_TRUE] (in this case ucWeight
           is not relevant), ucWeight [0xFFFFFFFF] and other parameters
           from 1.1. Expected: GT_OK.   */

        ucSPEnable = GT_TRUE;
        ucWeight = 0xFFFFFFFF;

        st = cpssDxChIpMultiTargetUcSchedModeSet(dev, ucSPEnable, ucWeight,
                                                 mcWeight, schedMtu);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucSPEnable, ucWeight);

        /* 1.3. Call with ucSPEnable [GT_FALSE] (in this case ucWeight
           is relevant), ucWeight [256] (out of range) and other
           parameters from 1.1. Expected: non GT_OK.    */

        ucSPEnable = GT_FALSE;
        ucWeight = 256;

        st = cpssDxChIpMultiTargetUcSchedModeSet(dev, ucSPEnable, ucWeight,
                                                 mcWeight, schedMtu);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucSPEnable, ucWeight);

        ucWeight = 10; /* restore */

        /* 1.4. Call with ucSPEnable [GT_FALSE], mcWeight [256] (out of range)
           and other parameters from 1.1. Expected: non GT_OK.  */

        mcWeight = 256;

        st = cpssDxChIpMultiTargetUcSchedModeSet(dev, ucSPEnable, ucWeight,
                                                 mcWeight, schedMtu);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, mcWeight = %d",
                                     dev, ucSPEnable, mcWeight);

        mcWeight = 10;      /* restore */

        /* 1.5. Call with wrong enum values schedMtu  and other valid
           parameters from 1.1. Expected: GT_BAD_PARAM. */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpMultiTargetUcSchedModeSet
                            (dev, ucSPEnable, ucWeight, mcWeight, schedMtu),
                            schedMtu);
    }

    ucSPEnable = GT_TRUE;
    ucWeight = 255;
    mcWeight = 255;
    schedMtu = CPSS_DXCH_IP_MT_UC_SCHED_MTU_2K_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpMultiTargetUcSchedModeSet(dev, ucSPEnable, ucWeight,
                                                 mcWeight, schedMtu);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpMultiTargetUcSchedModeSet(dev, ucSPEnable, ucWeight,
                                             mcWeight, schedMtu);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpArpBcModeSet
(
    IN GT_U8               devNum,
    IN CPSS_PACKET_CMD_ENT arpBcMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpArpBcModeSet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with arpBcMode [CPSS_PACKET_CMD_NONE_E / CPSS_PACKET_CMD_TRAP_TO_CPU_E / CPSS_PACKET_CMD_MIRROR_TO_CPU_E].
Expected: GT_OK.
1.2. Call with arpBcMode [CPSS_PACKET_CMD_FORWARD_E / CPSS_PACKET_CMD_DROP_HARD_E / CPSS_PACKET_CMD_DROP_SOFT_E] (these commands are not possible).
Expected: NOT GT_OK.
1.3. Call with wrong enum values arpBcMode.
Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_PACKET_CMD_ENT     arpBcMode;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with arpBcMode [CPSS_PACKET_CMD_NONE_E /
            CPSS_PACKET_CMD_TRAP_TO_CPU_E / CPSS_PACKET_CMD_MIRROR_TO_CPU_E].
            Expected: GT_OK.    */

        arpBcMode = CPSS_PACKET_CMD_NONE_E;

        st = cpssDxChIpArpBcModeSet(dev, arpBcMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, arpBcMode);

        arpBcMode = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssDxChIpArpBcModeSet(dev, arpBcMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, arpBcMode);

        arpBcMode = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssDxChIpArpBcModeSet(dev, arpBcMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, arpBcMode);

        /* 1.2. Call with arpBcMode [CPSS_PACKET_CMD_FORWARD_E /
           CPSS_PACKET_CMD_DROP_HARD_E / CPSS_PACKET_CMD_DROP_SOFT_E]
           (these commands are not possible).   Expected: NOT GT_OK.    */

        arpBcMode = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssDxChIpArpBcModeSet(dev, arpBcMode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, arpBcMode);

        arpBcMode = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssDxChIpArpBcModeSet(dev, arpBcMode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, arpBcMode);

        arpBcMode = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssDxChIpArpBcModeSet(dev, arpBcMode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, arpBcMode);

        /* 1.3. Call with wrong enum values arpBcMode .
        Expected: GT_BAD_PARAM.*/
        UTF_ENUMS_CHECK_MAC(cpssDxChIpArpBcModeSet
                            (dev, arpBcMode),
                            arpBcMode);
    }

    arpBcMode = CPSS_PACKET_CMD_NONE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpArpBcModeSet(dev, arpBcMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpArpBcModeSet(dev, arpBcMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpPortRoutingEnable
(
    IN GT_U8                            devNum,
    IN GT_U8                            portNum,
    IN CPSS_IP_UNICAST_MULTICAST_ENT    ucMcEnable,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    IN GT_BOOL                          enableRouting
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpPortRoutingEnable)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxCh2 and above)
    1.1. Call with ucMcEnable [CPSS_IP_UNICAST_E / CPSS_IP_MULTICAST_E],
    protocolStack[CPSS_IP_PROTOCOL_IPV4_E / CPSS_IP_PROTOCOL_IPV6_E /
                  CPSS_IP_PROTOCOL_IPV4V6_E]
    and enableRouting [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChIpPortRoutingEnableGet with the same parameters.
    Expected: GT_OK and the same value.
    1.3. Call with wrong enum values ucMcEnable  and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with wrong enum values protocolStack
         and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8  dev;
    GT_U8  port;
    CPSS_IP_UNICAST_MULTICAST_ENT ucMcEnable;
    CPSS_IP_PROTOCOL_STACK_ENT    protocolStack;
    GT_BOOL                       enableRouting;
    GT_BOOL                       enableRoutingGet;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1. Call with ucMcEnable [CPSS_IP_UNICAST_E / CPSS_IP_MULTICAST_E],
                protocolStack[CPSS_IP_PROTOCOL_IPV4_E / CPSS_IP_PROTOCOL_IPV6_E /
                              CPSS_IP_PROTOCOL_IPV4V6_E]
                and enableRouting [GT_FALSE and GT_TRUE].
                Expected: GT_OK.
            */

            /* Call with ucMcEnable [CPSS_IP_UNICAST_E],
             * protocolStack[CPSS_IP_PROTOCOL_IPV4_E]
             * and enableRouting [GT_FALSE].  */

            ucMcEnable = CPSS_IP_UNICAST_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            enableRouting = GT_FALSE;

            st = cpssDxChIpPortRoutingEnable(dev, port, ucMcEnable,
                                             protocolStack, enableRouting);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, ucMcEnable,
                                             protocolStack, enableRouting);

            /*
                1.2. Call cpssDxChIpPortRoutingEnableGet with the same parameters.
                Expected: GT_OK and the same value.
            */
            st = cpssDxChIpPortRoutingEnableGet(dev, port, ucMcEnable,
                                                protocolStack, &enableRoutingGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                         "cpssDxChIpPortRoutingEnableGet: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(enableRouting, enableRoutingGet,
                                 "get another enableRouting than was set: %d", dev);


            /* Call with enableRouting [GT_TRUE]  */
            enableRouting = GT_TRUE;

            st = cpssDxChIpPortRoutingEnable(dev, port, ucMcEnable,
                                             protocolStack, enableRouting);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, ucMcEnable,
                                             protocolStack, enableRouting);

            /*
                1.2. Call cpssDxChIpPortRoutingEnableGet with the same parameters.
                Expected: GT_OK and the same value.
            */
            st = cpssDxChIpPortRoutingEnableGet(dev, port, ucMcEnable,
                                                protocolStack, &enableRoutingGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                         "cpssDxChIpPortRoutingEnableGet: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(enableRouting, enableRoutingGet,
                                 "get another enableRouting than was set: %d", dev);

            /* Call with ucMcEnable [CPSS_IP_MULTICAST_E],
             * protocolStack[CPSS_IP_PROTOCOL_IPV6_E] and enableRouting [GT_TRUE].  */

            ucMcEnable = CPSS_IP_MULTICAST_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
            enableRouting = GT_TRUE;

            st = cpssDxChIpPortRoutingEnable(dev, port, ucMcEnable,
                                             protocolStack, enableRouting);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, ucMcEnable,
                                             protocolStack, enableRouting);

            /*
                1.2. Call cpssDxChIpPortRoutingEnableGet with the same parameters.
                Expected: GT_OK and the same value.
            */
            st = cpssDxChIpPortRoutingEnableGet(dev, port, ucMcEnable,
                                                protocolStack, &enableRoutingGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                         "cpssDxChIpPortRoutingEnableGet: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(enableRouting, enableRoutingGet,
                                 "get another enableRouting than was set: %d", dev);

            /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] */
            protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

            st = cpssDxChIpPortRoutingEnable(dev, port, ucMcEnable,
                                             protocolStack, enableRouting);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, ucMcEnable,
                                             protocolStack, enableRouting);

            /* 1.3. Call with wrong enum values ucMcEnable  and
               other valid parameters from 1.1. Expected: GT_BAD_PARAM. */

            UTF_ENUMS_CHECK_MAC(cpssDxChIpPortRoutingEnable
                                (dev, port, ucMcEnable, protocolStack, enableRouting),
                                ucMcEnable);

            /* 1.4. Call with wrong enum values protocolStack  and
               other valid parameters from 1.1. Expected: GT_BAD_PARAM. */
            UTF_ENUMS_CHECK_MAC(cpssDxChIpPortRoutingEnable
                                (dev, port, ucMcEnable, protocolStack, enableRouting),
                                protocolStack);
        }

        ucMcEnable = CPSS_IP_UNICAST_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        enableRouting = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChIpPortRoutingEnable(dev, port, ucMcEnable,
                                             protocolStack, enableRouting);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChIpPortRoutingEnable(dev, port, ucMcEnable,
                                         protocolStack, enableRouting);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChIpPortRoutingEnable(dev, port, ucMcEnable,
                                         protocolStack, enableRouting);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;
    ucMcEnable = CPSS_IP_UNICAST_E;
    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    enableRouting = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpPortRoutingEnable(dev, port, ucMcEnable,
                                         protocolStack, enableRouting);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpPortRoutingEnable(dev, port, ucMcEnable,
                                     protocolStack, enableRouting);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpPortRoutingEnableGet
(
    IN  GT_U8                            devNum,
    IN  GT_U8                            portNum,
    IN  CPSS_IP_UNICAST_MULTICAST_ENT    ucMcEnable,
    IN  CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    OUT GT_BOOL                          *enableRoutingPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpPortRoutingEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxCh2 and above)
    1.1. Call with ucMcEnable [CPSS_IP_UNICAST_E],
    protocolStack[CPSS_IP_PROTOCOL_IPV4_E].
    Expected: GT_OK.
    1.2. Call with ucMcEnable [CPSS_IP_MULTICAST_E],
    protocolStack[CPSS_IP_PROTOCOL_IPV6_E / CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_OK.
    1.3. Call with ucMcEnable [CPSS_IP_MULTICAST_E],
    protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E]
    Expected: GT_BAD_PARAM.
    1.4. Call with wrong enum values ucMcEnable
    and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with wrong enum values protocolStack
    and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with bad enableRoutingPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    CPSS_IP_UNICAST_MULTICAST_ENT    ucMcEnable;
    CPSS_IP_PROTOCOL_STACK_ENT       protocolStack;
    GT_BOOL                          enableRouting;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1. Call with ucMcEnable [CPSS_IP_UNICAST_E], protocolStack
              [CPSS_IP_PROTOCOL_IPV4_E]
              Expected: GT_OK.    */

            ucMcEnable = CPSS_IP_UNICAST_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

            st = cpssDxChIpPortRoutingEnableGet(dev, port, ucMcEnable,
                                             protocolStack, &enableRouting);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, ucMcEnable,
                                             protocolStack, enableRouting);

            /* 1.2. Call with ucMcEnable [CPSS_IP_MULTICAST_E],
               protocolStack[CPSS_IP_PROTOCOL_IPV6_E]
               Expected: GT_OK.    */

            ucMcEnable = CPSS_IP_MULTICAST_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV6_E;

            st = cpssDxChIpPortRoutingEnableGet(dev, port, ucMcEnable,
                                             protocolStack, &enableRouting);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, ucMcEnable,
                                             protocolStack, enableRouting);

            /* 1.3. Call with ucMcEnable [CPSS_IP_MULTICAST_E],
               protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E]
               Expected: GT_BAD_PARAM.    */

            protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

            st = cpssDxChIpPortRoutingEnableGet(dev, port, ucMcEnable,
                                             protocolStack, &enableRouting);

            /* can not get both values for ipv4 and ipv6 in the same get*/
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st, dev, port, ucMcEnable,
                                             protocolStack, enableRouting);

            protocolStack = CPSS_IP_PROTOCOL_IPV6_E; /* restore */

            /* 1.4. Call with wrong enum values ucMcEnable  and
               other valid parameters from 1.1. Expected: GT_BAD_PARAM. */
            UTF_ENUMS_CHECK_MAC(cpssDxChIpPortRoutingEnableGet
                                (dev, port, ucMcEnable, protocolStack, &enableRouting),
                                ucMcEnable);

            /* 1.5. Call with wrong enum values protocolStack  and
               other valid parameters from 1.1. Expected: GT_BAD_PARAM. */
            UTF_ENUMS_CHECK_MAC(cpssDxChIpPortRoutingEnableGet
                                (dev, port, ucMcEnable, protocolStack, &enableRouting),
                                protocolStack);

            /* 1.6. Call with bad enableRoutingPtr [NULL].
            Expected: GT_BAD_PTR. */

            st = cpssDxChIpPortRoutingEnableGet(dev, port, ucMcEnable,
                                                protocolStack, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, protocolStack = NULL", dev);
        }

        ucMcEnable = CPSS_IP_UNICAST_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChIpPortRoutingEnableGet(dev, port, ucMcEnable,
                                             protocolStack, &enableRouting);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChIpPortRoutingEnableGet(dev, port, ucMcEnable,
                                         protocolStack, &enableRouting);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChIpPortRoutingEnableGet(dev, port, ucMcEnable,
                                         protocolStack, &enableRouting);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;
    ucMcEnable = CPSS_IP_UNICAST_E;
    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    enableRouting = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpPortRoutingEnableGet(dev, port, ucMcEnable,
                                            protocolStack, &enableRouting);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpPortRoutingEnableGet(dev, port, ucMcEnable,
                                        protocolStack, &enableRouting);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpQosProfileToRouteEntryMapSet
(
    IN  GT_U8                devNum,
    IN  GT_U32               qosProfile,
    IN  GT_U32               routeEntryOffset
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpQosProfileToRouteEntryMapSet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with qosProfile [1 / 100 / 127] and routeEntryOffset [1 / 2 / 3].
Expected: GT_OK.
1.2. Call with qosProfile [1] and routeEntryOffset [0xFFFFFFFF] (no any constraints).
Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U32                  qosProfile;
    GT_U32                  routeEntryOffset;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with qosProfile [1 / 100 / 127] and
           routeEntryOffset [1 / 2 / 3]. Expected: GT_OK.   */

        qosProfile = 1;
        routeEntryOffset = 1;

        st = cpssDxChIpQosProfileToRouteEntryMapSet(dev, qosProfile,
                                                    routeEntryOffset);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, qosProfile,
                                                    routeEntryOffset);
        qosProfile = 100;
        routeEntryOffset = 2;

        st = cpssDxChIpQosProfileToRouteEntryMapSet(dev, qosProfile,
                                                    routeEntryOffset);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, qosProfile,
                                                    routeEntryOffset);
        qosProfile = 127;
        routeEntryOffset = 3;

        st = cpssDxChIpQosProfileToRouteEntryMapSet(dev, qosProfile,
                                                    routeEntryOffset);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, qosProfile,
                                                    routeEntryOffset);

        /* 1.2. Call with qosProfile [1] and routeEntryOffset
           [0xFFFFFFFF] (no any constraints). Expected: GT_OK.  */

        qosProfile = 1;
        routeEntryOffset = 0xFFFFFFFF;

        st = cpssDxChIpQosProfileToRouteEntryMapSet(dev, qosProfile,
                                                    routeEntryOffset);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, qosProfile,
                                                    routeEntryOffset);
    }

    qosProfile = 1;
    routeEntryOffset = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpQosProfileToRouteEntryMapSet(dev, qosProfile,
                                                    routeEntryOffset);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpQosProfileToRouteEntryMapSet(dev, qosProfile,
                                                routeEntryOffset);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpRoutingEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enableRouting
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpRoutingEnable)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call with enableRouting [GT_TRUE and GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssDxChIpRoutingEnableGet with the same params.
    Expected: GT_OK and the same value.
*/
    GT_STATUS  st = GT_OK;
    GT_U8      dev;
    GT_BOOL    enableRouting;
    GT_BOOL    enableRoutingGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enableRouting [GT_TRUE and GT_FALSE].
            Expected: GT_OK.
        */
        enableRouting = GT_TRUE;

        st = cpssDxChIpRoutingEnable(dev, enableRouting);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enableRouting);

        /*
            1.2. Call cpssDxChIpRoutingEnableGet with the same params.
            Expected: GT_OK and the same value.
        */
        st = cpssDxChIpRoutingEnableGet(dev, &enableRoutingGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enableRouting);


        enableRouting = GT_FALSE;

        st = cpssDxChIpRoutingEnable(dev, enableRouting);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enableRouting);

        /*
            1.2. Call cpssDxChIpRoutingEnableGet with the same params.
            Expected: GT_OK and the same value.
        */
        st = cpssDxChIpRoutingEnableGet(dev, &enableRoutingGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enableRouting);
    }

    enableRouting = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpRoutingEnable(dev, enableRouting);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    st = cpssDxChIpRoutingEnable(dev, enableRouting);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpRoutingEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpRoutingEnableGet)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with wrong enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChIpRoutingEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpRoutingEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpRoutingEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpRoutingEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpCntGet
(
    IN  GT_U8                        devNum,
    IN  CPSS_IP_CNT_SET_ENT          cntSet,
    OUT CPSS_DXCH_IP_COUNTER_SET_STC *counters
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpCntGet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with cntSet [CPSS_IP_CNT_SET0_E / CPSS_IP_CNT_SET1_E / CPSS_IP_CNT_SET2_E / CPSS_IP_CNT_SET3_E] and non-null counters.
Expected: GT_OK.
1.2. Call with wrong enum values cntSet  and other valid parameters from 1.1.
Expected: GT_BAD_PARAM.
1.3. Call with cntSet [CPSS_IP_CNT_SET0_E], counters [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_IP_CNT_SET_ENT          cntSet;
    CPSS_DXCH_IP_COUNTER_SET_STC counters;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with cntSet [CPSS_IP_CNT_SET0_E / CPSS_IP_CNT_SET1_E /
           CPSS_IP_CNT_SET2_E / CPSS_IP_CNT_SET3_E] and non-null counters.
           Expected: GT_OK.     */

        cntSet = CPSS_IP_CNT_SET0_E;

        st = cpssDxChIpCntGet(dev, cntSet, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntSet);

        cntSet = CPSS_IP_CNT_SET1_E;

        st = cpssDxChIpCntGet(dev, cntSet, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntSet);

        cntSet = CPSS_IP_CNT_SET2_E;

        st = cpssDxChIpCntGet(dev, cntSet, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntSet);

        cntSet = CPSS_IP_CNT_SET3_E;

        st = cpssDxChIpCntGet(dev, cntSet, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntSet);

        /* 1.2. Call with wrong enum values cntSet  and other
           valid parameters from 1.1. Expected: GT_BAD_PARAM.   */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpCntGet
                            (dev, cntSet, &counters),
                            cntSet);

        /* 1.3. Call with cntSet [CPSS_IP_CNT_SET0_E], counters [NULL].
           Expected: GT_BAD_PTR.  */

        cntSet = CPSS_IP_CNT_SET0_E;

        st = cpssDxChIpCntGet(dev, cntSet, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, counters = NULL", dev);

    }

    cntSet = CPSS_IP_CNT_SET0_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpCntGet(dev, cntSet, &counters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpCntGet(dev, cntSet, &counters);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpCntSetModeSet
(
    IN  GT_U8                                      devNum,
    IN  CPSS_IP_CNT_SET_ENT                        cntSet,
    IN  CPSS_DXCH_IP_CNT_SET_MODE_ENT              cntSetMode,
    IN  CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC *interfaceModeCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpCntSetModeSet)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call with cntSet [CPSS_IP_CNT_SET0_E / CPSS_IP_CNT_SET1_E /
                           CPSS_IP_CNT_SET2_E / CPSS_IP_CNT_SET3_E],
         cntSetMode [CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E /
                     CPSS_DXCH_IP_CNT_SET_ROUTE_ENTRY_MODE_E]
         and interfaceModeCfgPtr {portTrunkCntMode
         [CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E /
          CPSS_DXCH_IP_PORT_CNT_MODE_E / CPSS_DXCH_IP_TRUNK_CNT_MODE_E],
         ipMode [CPSS_IP_PROTOCOL_IPV4_E / CPSS_IP_PROTOCOL_IPV6_E /
         CPSS_IP_PROTOCOL_IPV4V6_E],
         vlanMode [CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E /
                   CPSS_DXCH_IP_USE_VLAN_CNT_MODE_E], devNum [dev],
         portTrunk {port[0], trunk[0]}, vlanId [BRG_VLAN_TESTED_VLAN_ID_CNS = 100]}.
    Expected: GT_OK.
    1.2. Call with wrong enum values cntSet
         and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with wrong enum values cntSetMode
         and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with cntSetMode [CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E],
         wrong enum values interfaceModeCfgPtr->portTrunkCntMode
         and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with cntSetMode [CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E],
         wrong enum values interfaceModeCfgPtr->ipMode
         and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with cntSetMode [CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E],
         wrong enum values interfaceModeCfgPtr->vlanMode
         and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with cntSetMode [CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E],
         interfaceModeCfgPtr {portTrunkCntMode[CPSS_DXCH_IP_TRUNK_CNT_MODE_E]
         (in this case port is not relevant), out of range portTrunk.port
         [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] and other valid parameters from 1.1.
    Expected: GT_OK.
    1.8. Call with cntSetMode [CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E],
         interfaceModeCfgPtr {portTrunkCntMode[CPSS_DXCH_IP_PORT_CNT_MODE_E]
         (in this case trunk is not relevant), out of range portTrunk.trunk
         [MAX_TRUNK_ID_CNS = 128] and other valid parameters from 1.1.
    Expected:  GT_BAD_PARAM.
    1.9 Call with cntSetMode [CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E],
         interfaceModeCfgPtr {portTrunkCntMode[CPSS_DXCH_IP_PORT_CNT_MODE_E]
         (in this case trunk is not relevant), portTrunk.port = 0  and other valid parameters from 1.1.
    Expected: GT_OK.
    1.10. Call with cntSetMode [CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E],
         interfaceModeCfgPtr {vlanMode [CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E]
         (in this case vlanId is not relevant), out of range vlanId
         [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] and other valid parameters from 1.1.
    Expected: GT_OK.
    1.11. Call with cntSetMode [CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E],
          interfaceModeCfgPtr [NULL] and other valid parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_IP_CNT_SET_ENT                        cntSet;
    CPSS_DXCH_IP_CNT_SET_MODE_ENT              cntSetMode;
    CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC interfaceModeCfg;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with cntSet [CPSS_IP_CNT_SET0_E / CPSS_IP_CNT_SET1_E
          / CPSS_IP_CNT_SET2_E / CPSS_IP_CNT_SET3_E], cntSetMode
          [CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E /
          CPSS_DXCH_IP_CNT_SET_ROUTE_ENTRY_MODE_E] and interfaceModeCfgPtr
          {portTrunkCntMode[CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E /
          CPSS_DXCH_IP_PORT_CNT_MODE_E / CPSS_DXCH_IP_TRUNK_CNT_MODE_E],
          ipMode [CPSS_IP_PROTOCOL_IPV4_E / CPSS_IP_PROTOCOL_IPV6_E /
          CPSS_IP_PROTOCOL_IPV4V6_E], vlanMode [CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E
          / CPSS_DXCH_IP_USE_VLAN_CNT_MODE_E], devNum [dev], portTrunk
          {port[0], trunk[0]}, vlanId [BRG_VLAN_TESTED_VLAN_ID_CNS = 100]}.
          Expected: GT_OK.  */

        cntSet = CPSS_IP_CNT_SET0_E;
        cntSetMode = CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E;
        cpssOsBzero((GT_VOID*) &interfaceModeCfg, sizeof(interfaceModeCfg));
        interfaceModeCfg.portTrunkCntMode = CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E;
        interfaceModeCfg.ipMode = CPSS_IP_PROTOCOL_IPV4_E;
        interfaceModeCfg.vlanMode = CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E;
        interfaceModeCfg.devNum = dev;
        interfaceModeCfg.portTrunk.port = 0;
        interfaceModeCfg.portTrunk.trunk = 0;
        interfaceModeCfg.vlanId = 100;

        st = cpssDxChIpCntSetModeSet(dev, cntSet, cntSetMode, &interfaceModeCfg);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntSet, cntSetMode);

        cntSet = CPSS_IP_CNT_SET3_E;
        cntSetMode = CPSS_DXCH_IP_CNT_SET_ROUTE_ENTRY_MODE_E;
        interfaceModeCfg.portTrunkCntMode = CPSS_DXCH_IP_TRUNK_CNT_MODE_E;
        interfaceModeCfg.ipMode = CPSS_IP_PROTOCOL_IPV4V6_E;
        interfaceModeCfg.vlanMode = CPSS_DXCH_IP_USE_VLAN_CNT_MODE_E;

        st = cpssDxChIpCntSetModeSet(dev, cntSet, cntSetMode, &interfaceModeCfg);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntSet, cntSetMode);

        /* restore: */

        cntSet = CPSS_IP_CNT_SET0_E;
        cntSetMode = CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E;
        interfaceModeCfg.portTrunkCntMode = CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E;
        interfaceModeCfg.ipMode = CPSS_IP_PROTOCOL_IPV4_E;
        interfaceModeCfg.vlanMode = CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E;

        /* 1.2. Call with wrong enum values cntSet  and other valid
           parameters from 1.1. Expected: GT_BAD_PARAM. */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpCntSetModeSet
                            (dev, cntSet, cntSetMode, &interfaceModeCfg),
                            cntSet);

        /* 1.3. Call with wrong enum values cntSetMode  and other
           valid parameters from 1.1. Expected: GT_BAD_PARAM.   */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpCntSetModeSet
                            (dev, cntSet, cntSetMode, &interfaceModeCfg),
                            cntSetMode);

        /* 1.4. Call with cntSetMode [CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E],
           wrong enum values interfaceModeCfgPtr->portTrunkCntMode  and
           other valid parameters from 1.1. Expected: GT_BAD_PARAM. */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpCntSetModeSet
                            (dev, cntSet, cntSetMode, &interfaceModeCfg),
                            interfaceModeCfg.portTrunkCntMode);

        /* 1.5. Call with cntSetMode [CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E],
           wrong enum values interfaceModeCfgPtr->ipMode  and other valid
           parameters from 1.1. Expected: GT_BAD_PARAM. */

        UTF_ENUMS_CHECK_MAC(cpssDxChIpCntSetModeSet
                            (dev, cntSet, cntSetMode, &interfaceModeCfg),
                            interfaceModeCfg.ipMode);

        /* 1.6. Call with cntSetMode [CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E],
           wrong enum values interfaceModeCfgPtr->vlanMode  and other
           valid parameters from 1.1.   Expected: GT_BAD_PARAM. */

        UTF_ENUMS_CHECK_MAC(cpssDxChIpCntSetModeSet
                            (dev, cntSet, cntSetMode, &interfaceModeCfg),
                            interfaceModeCfg.vlanMode);

        /* 1.7. Call with cntSetMode [CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E],
           interfaceModeCfgPtr {portTrunkCntMode[CPSS_DXCH_IP_TRUNK_CNT_MODE_E]
           (change only trunk as port is inside union), and other valid 
           parameters from 1.1. Expected: GT_OK. */

        interfaceModeCfg.portTrunkCntMode = CPSS_DXCH_IP_TRUNK_CNT_MODE_E;
        interfaceModeCfg.portTrunk.trunk = 0;

        st = cpssDxChIpCntSetModeSet(dev, cntSet, cntSetMode, &interfaceModeCfg);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
         "%d, interfaceModeCfg.portTrunkCntMode = %d, interfaceModeCfg.portTrunk.port = %d",
         dev, interfaceModeCfg.portTrunkCntMode, interfaceModeCfg.portTrunk.port);

        /* restore */
        interfaceModeCfg.portTrunkCntMode = CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E;
        interfaceModeCfg.portTrunk.port = 0;


        /* 1.8. Call with cntSetMode [CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E],
           interfaceModeCfgPtr {portTrunkCntMode[CPSS_DXCH_IP_PORT_CNT_MODE_E]
           (in this case trunk is not relevant), out of range portTrunk.trunk
           [MAX_TRUNK_ID_CNS = 128] and other valid parameters from 1.1.
           Expected: GT_BAD_PARAM. */

        interfaceModeCfg.portTrunkCntMode = CPSS_DXCH_IP_TRUNK_CNT_MODE_E;
        interfaceModeCfg.portTrunk.trunk  = MAX_TRUNK_ID_CNS;

        st = cpssDxChIpCntSetModeSet(dev, cntSet, cntSetMode, &interfaceModeCfg);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st,
         "%d, interfaceModeCfg.portTrunkCntMode = %d, interfaceModeCfg.portTrunk.trunk = %d",
         dev, interfaceModeCfg.portTrunkCntMode, interfaceModeCfg.portTrunk.trunk);


        /* 1.9. Call with cntSetMode [CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E],
           interfaceModeCfgPtr {portTrunkCntMode[CPSS_DXCH_IP_PORT_CNT_MODE_E]
           (in this case trunk is not relevant), portTrunk.port = 0
           and other valid parameters from 1.1.
           Expected: GT_BAD_PARAM. */

        interfaceModeCfg.portTrunkCntMode = CPSS_DXCH_IP_PORT_CNT_MODE_E;
        interfaceModeCfg.portTrunk.port = 0;

        st = cpssDxChIpCntSetModeSet(dev, cntSet, cntSetMode, &interfaceModeCfg);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, interfaceModeCfg.portTrunkCntMode = %d, interfaceModeCfg.portTrunk.port = %d",
                                     dev, interfaceModeCfg.portTrunkCntMode, interfaceModeCfg.portTrunk.port);

        /* restore */
        interfaceModeCfg.portTrunkCntMode = CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E;
        interfaceModeCfg.portTrunk.trunk = 0;

        /* 1.10. Call with cntSetMode [CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E],
           interfaceModeCfgPtr {vlanMode [CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E]
           (in this case vlanId is not relevant), out of range vlanId
           [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] and other valid parameters from 1.1.
           Expected: GT_OK. */

        interfaceModeCfg.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChIpCntSetModeSet(dev, cntSet, cntSetMode, &interfaceModeCfg);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                "%d, interfaceModeCfg.vlanMode = %d, interfaceModeCfg.vlanId = %d",
                         dev, interfaceModeCfg.vlanMode, interfaceModeCfg.vlanId);
        /* restore */
        interfaceModeCfg.vlanId = 100;


        /* 1.11. Call with cntSetMode [CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E],
           interfaceModeCfgPtr [NULL] and other valid parameters from 1.1.
           Expected: GT_BAD_PTR.    */

        st = cpssDxChIpCntSetModeSet(dev, cntSet, cntSetMode, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, interfaceModeCfgPtr = NULL", dev);
    }

    cntSet = CPSS_IP_CNT_SET0_E;
    cntSetMode = CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E;
    cpssOsBzero((GT_VOID*) &interfaceModeCfg, sizeof(interfaceModeCfg));
    interfaceModeCfg.portTrunkCntMode = CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E;
    interfaceModeCfg.ipMode = CPSS_IP_PROTOCOL_IPV4_E;
    interfaceModeCfg.vlanMode = CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E;
    interfaceModeCfg.devNum = dev;
    interfaceModeCfg.portTrunk.port = 0;
    interfaceModeCfg.portTrunk.trunk = 0;
    interfaceModeCfg.vlanId = 100;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpCntSetModeSet(dev, cntSet, cntSetMode, &interfaceModeCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpCntSetModeSet(dev, cntSet, cntSetMode, &interfaceModeCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpCntSet
(
    IN GT_U8                         devNum,
    IN CPSS_IP_CNT_SET_ENT           cntSet,
    IN CPSS_DXCH_IP_COUNTER_SET_STC *counters
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpCntSet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with cntSet [CPSS_IP_CNT_SET0_E / CPSS_IP_CNT_SET1_E / CPSS_IP_CNT_SET2_E / CPSS_IP_CNT_SET3_E] and counters{inUcPkts [1], inMcPkts [1], inUcNonRoutedExcpPkts [0], inUcNonRoutedNonExcpPkts [0], inMcNonRoutedExcpPkts [0], inMcNonRoutedNonExcpPkts [0], inUcTrappedMirrorPkts [1], inMcTrappedMirrorPkts [1], mcRfpFailPkts [0], outUcRoutedPkts [1]}.
Expected: GT_OK.
1.2. Call cpssDxChIpCntGet with the same cntSet.
Expected: GT_OK and the same counters.
1.3. Call with wrong enum values cntSet  and other valid parameters from 1.1.
Expected: GT_BAD_PARAM.
1.4. Call with counters [NULL] and other valid parameters from 1.1.
Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_IP_CNT_SET_ENT           cntSet;
    CPSS_DXCH_IP_COUNTER_SET_STC  counters;
    CPSS_DXCH_IP_COUNTER_SET_STC  countersRet;
    GT_BOOL     isEqual;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with cntSet [CPSS_IP_CNT_SET0_E / CPSS_IP_CNT_SET1_E /
           CPSS_IP_CNT_SET2_E / CPSS_IP_CNT_SET3_E] and counters{inUcPkts [1],
           inMcPkts [1], inUcNonRoutedExcpPkts [0], inUcNonRoutedNonExcpPkts [0],
           inMcNonRoutedExcpPkts [0], inMcNonRoutedNonExcpPkts [0],
           inUcTrappedMirrorPkts [1], inMcTrappedMirrorPkts [1],
           mcRfpFailPkts [0], outUcRoutedPkts [1]}. Expected: GT_OK.    */

        cntSet = CPSS_IP_CNT_SET3_E;
        cpssOsBzero((GT_VOID*) &counters, sizeof(counters));
        counters.inUcPkts  = 1;
        counters.inMcPkts  = 1;
        counters.inUcNonRoutedExcpPkts = 0;
        counters.inUcNonRoutedNonExcpPkts = 0;
        counters.inMcNonRoutedExcpPkts = 0;
        counters.inMcNonRoutedNonExcpPkts = 0;
        counters.inUcTrappedMirrorPkts = 1;
        counters.inMcTrappedMirrorPkts = 1;
        counters.mcRfpFailPkts  = 0;
        counters.outUcRoutedPkts = 1;

        st = cpssDxChIpCntSet(dev, cntSet, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntSet);

        cntSet = CPSS_IP_CNT_SET0_E;

        st = cpssDxChIpCntSet(dev, cntSet, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntSet);

        /* 1.2. Call cpssDxChIpCntGet with the same cntSet.
           Expected: GT_OK and the same counters.   */

        cpssOsBzero((GT_VOID*) &countersRet, sizeof(countersRet));

        st = cpssDxChIpCntGet(dev, cntSet, &countersRet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChIpCntGet: %d, %d", dev, cntSet);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&counters, (GT_VOID*)&countersRet, sizeof (counters)))
                  ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                                     "get another counters than was set: %d, %d", dev, cntSet);

        /* 1.3. Call with wrong enum values cntSet  and other
           valid parameters from 1.1. Expected: GT_BAD_PARAM.   */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpCntSet
                            (dev, cntSet, &counters),
                            cntSet);

        /* 1.4. Call with counters [NULL] and other valid parameters
           from 1.1. Expected: GT_BAD_PTR.  */

        st = cpssDxChIpCntSet(dev, cntSet, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, counters = NULL", dev);
    }

    cntSet = CPSS_IP_CNT_SET3_E;
    cpssOsBzero((GT_VOID*) &counters, sizeof(counters));
    counters.inUcPkts  = 1;
    counters.inMcPkts  = 1;
    counters.inUcNonRoutedExcpPkts = 0;
    counters.inUcNonRoutedNonExcpPkts = 0;
    counters.inMcNonRoutedExcpPkts = 0;
    counters.inMcNonRoutedNonExcpPkts = 0;
    counters.inUcTrappedMirrorPkts = 1;
    counters.inMcTrappedMirrorPkts = 1;
    counters.mcRfpFailPkts  = 0;
    counters.outUcRoutedPkts = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpCntSet(dev, cntSet, &counters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpCntSet(dev, cntSet, &counters);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpSetMllCntInterface
(
    IN GT_U8                                      devNum,
    IN GT_U32                                     mllCntSet,
    IN CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC *interfaceCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpSetMllCntInterface)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with mllCntSet [0] and interfaceCfgPtr
     {portTrunkCntMode[CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E /
     CPSS_DXCH_IP_PORT_CNT_MODE_E / CPSS_DXCH_IP_TRUNK_CNT_MODE_E],
     ipMode [CPSS_IP_PROTOCOL_IPV4_E / CPSS_IP_PROTOCOL_IPV6_E /
     CPSS_IP_PROTOCOL_IPV4V6_E], vlanMode [CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E /
     CPSS_DXCH_IP_USE_VLAN_CNT_MODE_E], devNum [dev], portTrunk {port[0], trunk[0]},
     vlanId [BRG_VLAN_TESTED_VLAN_ID_CNS = 100]}.
Expected: GT_OK.
1.2. Call with out of range mllCntSet [5] and other valid parameters from 1.1.
Expected: NOT GT_OK.
1.3. Call with wrong enum values interfaceCfgPtr->portTrunkCntMode
     and other valid parameters from 1.1.
Expected: GT_BAD_PARAM.
1.4. Call with wrong enum values interfaceCfgPtr->ipMode
     and other valid parameters from 1.1.
Expected: GT_BAD_PARAM.
1.5. Call with wrong enum values interfaceCfgPtr->vlanMode
     and other valid parameters from 1.1.
Expected: GT_BAD_PARAM.
1.6. Call with interfaceCfgPtr {portTrunkCntMode[CPSS_DXCH_IP_PORT_CNT_MODE_E],
     out of range portTrunk.port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
     and other valid parameters from 1.1.
Expected: NOT GT_OK.
1.7. Call with interfaceCfgPtr {portTrunkCntMode[CPSS_DXCH_IP_TRUNK_CNT_MODE_E],
     out of range portTrunk.trunk [MAX_TRUNK_ID_CNS = 128]
     and other valid parameters from 1.1.
Expected: NOT GT_OK.
1.8. Call with interfaceCfgPtr {portTrunkCntMode[CPSS_DXCH_IP_TRUNK_CNT_MODE_E]
     (in this case trunk is not relevant), out of range portTrunk.port
     [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] and other valid parameters from 1.1.
Expected: GT_BAD_PARAM
1.9. Call with interfaceCfgPtr {portTrunkCntMode[CPSS_DXCH_IP_PORT_CNT_MODE_E]
     (in this case trunk is not relevant), out of range portTrunk.trunk
    [MAX_TRUNK_ID_CNS = 128]  and other valid parameters from 1.1.
Expected: GT_BAD_PARAM
1.10. Call with interfaceCfgPtr {portTrunkCntMode[CPSS_DXCH_IP_PORT_CNT_MODE_E]
     (in this case trunk is not relevant), out of range portTrunk.port = 0
     and other valid parameters from 1.1.
Expected: GT_OK.
1.11. Call with interfaceCfgPtr {vlanMode [CPSS_DXCH_IP_USE_VLAN_CNT_MODE_E],
      out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096]
      and other valid parameters from 1.1.
Expected: non GT_OK.
1.12. Call with interfaceCfgPtr {vlanMode [CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E]
      (in this case vlanId is not relevant), out of range vlanId
      [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] and other valid parameters from 1.1.
Expected: GT_OK.
1.12. Call with interfaceCfgPtr [NULL] and other valid parameters from 1.1.
Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U32                                     mllCntSet;
    CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC interfaceCfg;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with mllCntSet [0] and interfaceCfgPtr
          {portTrunkCntMode[CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E /
          CPSS_DXCH_IP_PORT_CNT_MODE_E / CPSS_DXCH_IP_TRUNK_CNT_MODE_E],
          ipMode [CPSS_IP_PROTOCOL_IPV4_E / CPSS_IP_PROTOCOL_IPV6_E /
          CPSS_IP_PROTOCOL_IPV4V6_E], vlanMode [CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E
          / CPSS_DXCH_IP_USE_VLAN_CNT_MODE_E], devNum [dev], portTrunk {port[0],
          trunk[0]}, vlanId [BRG_VLAN_TESTED_VLAN_ID_CNS = 100]}.Expected: GT_OK.   */

        mllCntSet = 0;
        cpssOsBzero((GT_VOID*) &interfaceCfg, sizeof(interfaceCfg));
        interfaceCfg.portTrunkCntMode = CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E;
        interfaceCfg.ipMode = CPSS_IP_PROTOCOL_IPV4_E;
        interfaceCfg.vlanMode = CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E;
        interfaceCfg.devNum = dev;
        interfaceCfg.portTrunk.port = 0;
        interfaceCfg.portTrunk.trunk = 0;
        interfaceCfg.vlanId = 100;

        st = cpssDxChIpSetMllCntInterface(dev, mllCntSet, &interfaceCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mllCntSet);

        interfaceCfg.portTrunkCntMode = CPSS_DXCH_IP_TRUNK_CNT_MODE_E;
        interfaceCfg.ipMode = CPSS_IP_PROTOCOL_IPV4V6_E;
        interfaceCfg.vlanMode = CPSS_DXCH_IP_USE_VLAN_CNT_MODE_E;

        st = cpssDxChIpSetMllCntInterface(dev, mllCntSet, &interfaceCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mllCntSet);

        /* restore: */

        interfaceCfg.portTrunkCntMode = CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E;
        interfaceCfg.ipMode = CPSS_IP_PROTOCOL_IPV4_E;
        interfaceCfg.vlanMode = CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E;

        /* 1.2. Call with out of range mllCntSet [5] and other valid
           parameters from 1.1. Expected: NOT GT_OK.    */

        mllCntSet = 5;

        st = cpssDxChIpSetMllCntInterface(dev, mllCntSet, &interfaceCfg);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, mllCntSet);

        mllCntSet = 0;

        /* 1.3. Call with wrong enum values interfaceCfgPtr->portTrunkCntMode
            and other valid parameters from 1.1. Expected: GT_BAD_PARAM.*/
        UTF_ENUMS_CHECK_MAC(cpssDxChIpSetMllCntInterface
                            (dev, mllCntSet, &interfaceCfg),
                            interfaceCfg.portTrunkCntMode);

        /* 1.4. Call with wrong enum values interfaceCfgPtr->ipMode  and
           other valid parameters from 1.1. Expected: GT_BAD_PARAM. */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpSetMllCntInterface
                            (dev, mllCntSet, &interfaceCfg),
                            interfaceCfg.ipMode);

        /* 1.5. Call with wrong enum values interfaceCfgPtr->vlanMode
           and other valid parameters from 1.1. Expected: GT_BAD_PARAM. */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpSetMllCntInterface
                            (dev, mllCntSet, &interfaceCfg),
                            interfaceCfg.vlanMode);

        /* 1.8. Call with interfaceCfgPtr {portTrunkCntMode[CPSS_DXCH_IP_TRUNK_CNT_MODE_E]
           (in this case port is not relevant but part of union), so set trunk
           and other valid parameters from 1.1. Expected: GT_OK. */

        interfaceCfg.portTrunkCntMode = CPSS_DXCH_IP_TRUNK_CNT_MODE_E;
        interfaceCfg.portTrunk.trunk = 0;

        st = cpssDxChIpSetMllCntInterface(dev, mllCntSet, &interfaceCfg);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
         "%d, interfaceCfg.portTrunkCntMode = %d, interfaceCfg.portTrunk.port = %d",
                dev, interfaceCfg.portTrunkCntMode, interfaceCfg.portTrunk.port);
        /* restore */
        interfaceCfg.portTrunkCntMode = CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E;
        interfaceCfg.portTrunk.port = 0;


        /* 1.9. Call with interfaceCfgPtr {portTrunkCntMode[CPSS_DXCH_IP_PORT_CNT_MODE_E]
           (in this case trunk is not relevant), out of range portTrunk.trunk
           [MAX_TRUNK_ID_CNS = 128] and other valid parameters from 1.1.
           Expected: GT_BAD_PARAM. */

        interfaceCfg.portTrunkCntMode = CPSS_DXCH_IP_TRUNK_CNT_MODE_E;
        interfaceCfg.portTrunk.trunk = MAX_TRUNK_ID_CNS;

        st = cpssDxChIpSetMllCntInterface(dev, mllCntSet, &interfaceCfg);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st,
            "%d, interfaceCfg.portTrunkCntMode = %d, interfaceCfg.portTrunk.trunk = %d",
                    dev, interfaceCfg.portTrunkCntMode, interfaceCfg.portTrunk.trunk);


        /* 1.10. Call with interfaceCfgPtr {portTrunkCntMode[CPSS_DXCH_IP_PORT_CNT_MODE_E]
           (in this case trunk is not relevant), oportTrunk.port =0
           and other valid parameters from 1.1.
           Expected: GT_OK */

        interfaceCfg.portTrunkCntMode = CPSS_DXCH_IP_PORT_CNT_MODE_E;
        interfaceCfg.portTrunk.port = 0;

        st = cpssDxChIpSetMllCntInterface(dev, mllCntSet, &interfaceCfg);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, interfaceCfg.portTrunkCntMode = %d, interfaceCfg.portTrunk.port = %d",
                                     dev, interfaceCfg.portTrunkCntMode, interfaceCfg.portTrunk.port);

        /* restore */
        interfaceCfg.portTrunkCntMode = CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E;
        interfaceCfg.portTrunk.trunk = 0;

        /* 1.11. Call with
           interfaceCfgPtr {vlanMode [CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E]
           (in this case vlanId is not relevant), out of range vlanId
           [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] and other valid parameters from 1.1.
           Expected: GT_OK. */

        interfaceCfg.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChIpSetMllCntInterface(dev, mllCntSet, &interfaceCfg);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                "%d, interfaceCfg.vlanMode = %d, interfaceCfg.vlanId = %d",
                          dev, interfaceCfg.vlanMode, interfaceCfg.vlanId);
        /* restore */
        interfaceCfg.vlanId = 100;


        /* 1.12. Call with
           interfaceCfgPtr [NULL] and other valid parameters from 1.1.
           Expected: GT_BAD_PTR.    */

        st = cpssDxChIpSetMllCntInterface(dev, mllCntSet, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, interfaceCfgPtr = NULL", dev);
    }

    mllCntSet = 0;
    cpssOsBzero((GT_VOID*) &interfaceCfg, sizeof(interfaceCfg));
    interfaceCfg.portTrunkCntMode = CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E;
    interfaceCfg.ipMode = CPSS_IP_PROTOCOL_IPV4_E;
    interfaceCfg.vlanMode = CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E;
    interfaceCfg.devNum = dev;
    interfaceCfg.portTrunk.port = 0;
    interfaceCfg.portTrunk.trunk = 0;
    interfaceCfg.vlanId = 100;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpSetMllCntInterface(dev, mllCntSet, &interfaceCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpSetMllCntInterface(dev, mllCntSet, &interfaceCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpMllCntGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  mllCntSet,
    OUT GT_U32  *mllOutMCPktsPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpMllCntGet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with mllCntSet [0] and non-null mllOutMCPktsPtr.
Expected: GT_OK.
1.2. Call with mllCntSet [10] and non-null mllOutMCPktsPtr.
Expected: GT_BAD_PARAM.
1.3. Call with mllCntSet [0] and mllOutMCPkts [NULL].
Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U32  mllCntSet;
    GT_U32  mllOutMCPkts;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with mllCntSet [0] and non-null mllOutMCPktsPtr.
           Expected: GT_OK. */

        mllCntSet = 0;

        st = cpssDxChIpMllCntGet(dev, mllCntSet, &mllOutMCPkts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mllCntSet);

        /* 1.2. Call with mllCntSet [10] and non-null mllOutMCPktsPtr.
                Expected: GT_BAD_PARAM. */

        mllCntSet = 10;

        st = cpssDxChIpMllCntGet(dev, mllCntSet, &mllOutMCPkts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mllCntSet);

        /* 1.3. Call with mllCntSet [0] and mllOutMCPkts [NULL].
                Expected: GT_BAD_PTR.   */

        mllCntSet = 0;

        st = cpssDxChIpMllCntGet(dev, mllCntSet, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mllOutMCPktsPtr = NULL", dev);
    }

    mllCntSet = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpMllCntGet(dev, mllCntSet, &mllOutMCPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpMllCntGet(dev, mllCntSet, &mllOutMCPkts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpMllCntSet
(
    IN GT_U8    devNum,
    IN GT_U32   mllCntSet,
    IN GT_U32   mllOutMCPkts
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpMllCntSet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with mllCntSet [1]and mllOutMCPkts [10]. Expected: GT_OK.
1.2. Call cpssDxChIpMllCntGet with the same mllCntSet. Expected: GT_OK and the same mllOutMCPkts.
1.3. Check out of range mllCntSet. Call with mllCntSet [10] and mllOutMCPkts [10] (no any constraints). Expected: GT_BAD_PARAM.
1.4. Call with mllCntSet [1] and mllOutMCPkts [0xFFFFFFFF] (no any constraints). Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U32   mllCntSet;
    GT_U32   mllOutMCPkts;
    GT_U32   mllOutMCPktsRet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with mllCntSet [1]and mllOutMCPkts [10]. Expected: GT_OK.*/

        mllCntSet = 1;
        mllOutMCPkts = 10;

        st = cpssDxChIpMllCntSet(dev, mllCntSet, mllOutMCPkts);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mllCntSet, mllOutMCPkts);

        /* 1.2. Call cpssDxChIpMllCntGet with the same mllCntSet.
                Expected: GT_OK and the same mllOutMCPkts.  */

        st = cpssDxChIpMllCntGet(dev, mllCntSet, &mllOutMCPktsRet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChIpMllCntGet: %d, %d",
                                     dev, mllCntSet);

        UTF_VERIFY_EQUAL2_STRING_MAC(mllOutMCPkts, mllOutMCPktsRet,
            "get another mllOutMCPkts than was set: %d, %d", dev, mllCntSet);

        /* 1.3. Check out of range mllCntSet. Call with mllCntSet [10] and
           mllOutMCPkts [10] (no any constraints). Expected: GT_BAD_PARAM.  */

        mllCntSet = 10;
        mllOutMCPkts = 10;

        st = cpssDxChIpMllCntSet(dev, mllCntSet, mllOutMCPkts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mllCntSet);

        /* 1.4. Call with mllCntSet [1] and mllOutMCPkts [0xFFFFFFFF]
           (no any constraints). Expected: GT_OK.   */

        mllCntSet = 1;
        mllOutMCPkts = 0xFFFFFFFF;

        st = cpssDxChIpMllCntSet(dev, mllCntSet, mllOutMCPkts);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mllCntSet, mllOutMCPkts);
    }

    mllCntSet = 1;
    mllOutMCPkts = 10;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpMllCntSet(dev, mllCntSet, mllOutMCPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpMllCntSet(dev, mllCntSet, mllOutMCPkts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpDropCntSet
(
    IN GT_U8 devNum,
    IN GT_U32  dropPkts
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpDropCntSet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with dropPkts [10]. Expected: GT_OK.
1.2. Call cpssDxChIpDropCntGet.
Expected: GT_OK and the same dropPkts.
1.3. Call with dropPkts [0xFFFFFFFF] (no any constraints). Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U32                  dropPkts;
    GT_U32                  dropPktsRet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with dropPkts [10]. Expected: GT_OK.   */

        dropPkts = 10;

        st = cpssDxChIpDropCntSet(dev, dropPkts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropPkts);

        /* 1.2. Call cpssDxChIpDropCntGet. Expected: GT_OK and the same dropPkts.*/

        st = cpssDxChIpDropCntGet(dev, &dropPktsRet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChIpDropCntGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(dropPkts, dropPktsRet,
            "get another dropPkts than was set: %d", dev);

        /* 1.3. Call with dropPkts [0xFFFFFFFF] (no any constraints).
           Expected: GT_OK. */

        dropPkts = 0xFFFFFFFF;

        st = cpssDxChIpDropCntSet(dev, dropPkts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropPkts);
    }

    dropPkts = 10;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpDropCntSet(dev, dropPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpDropCntSet(dev, dropPkts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpSetDropCntMode
(
    IN GT_U8                          devNum,
    IN CPSS_DXCH_IP_DROP_CNT_MODE_ENT dropCntMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpSetDropCntMode)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with dropCntMode [CPSS_DXCH_IP_DROP_CNT_COUNT_ALL_MODE_E / CPSS_DXCH_IP_DROP_CNT_IP_HEADER_MODE_E / CPSS_DXCH_IP_DROP_CNT_ACCESS_MATRIX_MODE_E].
Expected: GT_OK.
1.2. Call with wrong enum values dropCntMode .
Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_DXCH_IP_DROP_CNT_MODE_ENT  dropCntMode;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with dropCntMode [CPSS_DXCH_IP_DROP_CNT_COUNT_ALL_MODE_E
           / CPSS_DXCH_IP_DROP_CNT_IP_HEADER_MODE_E /
             CPSS_DXCH_IP_DROP_CNT_ACCESS_MATRIX_MODE_E]. Expected: GT_OK.  */

        dropCntMode = CPSS_DXCH_IP_DROP_CNT_COUNT_ALL_MODE_E;

        st = cpssDxChIpSetDropCntMode(dev, dropCntMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropCntMode);

        dropCntMode = CPSS_DXCH_IP_DROP_CNT_IP_HEADER_MODE_E;

        st = cpssDxChIpSetDropCntMode(dev, dropCntMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropCntMode);

        dropCntMode = CPSS_DXCH_IP_DROP_CNT_ACCESS_MATRIX_MODE_E;

        st = cpssDxChIpSetDropCntMode(dev, dropCntMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropCntMode);

        /* 1.2. Call with wrong enum values dropCntMode .
                Expected: GT_BAD_PARAM. */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpSetDropCntMode
                            (dev, dropCntMode),
                            dropCntMode);
    }

    dropCntMode = CPSS_DXCH_IP_DROP_CNT_COUNT_ALL_MODE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpSetDropCntMode(dev, dropCntMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpSetDropCntMode(dev, dropCntMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpDropCntGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *dropPktsPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpDropCntGet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with non-null dropPktsPtr. Expected: GT_OK.
1.2. Call with dropPktsPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U32                  dropPkts;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with non-null dropPktsPtr. Expected: GT_OK.    */

        st = cpssDxChIpDropCntGet(dev, &dropPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call with dropPktsPtr [NULL]. Expected: GT_BAD_PTR.*/

        st = cpssDxChIpDropCntGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dropPktsPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpDropCntGet(dev, &dropPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpDropCntGet(dev, &dropPkts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpMtuProfileSet
(
    IN GT_U8  devNum,
    IN GT_U32 mtuProfileIndex,
    IN GT_U32 mtu
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpMtuProfileSet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with mtuProfileIndex [0 / 7] and mtu [100].
Expected: GT_OK.
1.2. Call with out of range mtuProfileIndex [8] and mtu [100].
Expected: NOT GT_OK.
1.3. Call with mtuProfileIndex [1] and mtu [0xFFFFFFFF] (no any constraints).
Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U32                  mtuProfileIndex;
    GT_U32                  mtu;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with mtuProfileIndex [0 / 7] and mtu [100].
                Expected: GT_OK.    */

        mtuProfileIndex = 0;
        mtu = 100;

        st = cpssDxChIpMtuProfileSet(dev, mtuProfileIndex, mtu);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mtuProfileIndex, mtu);

        mtuProfileIndex = 7;

        st = cpssDxChIpMtuProfileSet(dev, mtuProfileIndex, mtu);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mtuProfileIndex, mtu);

        /* 1.2. Call with out of range mtuProfileIndex [8] and mtu [100].
                Expected: NOT GT_OK.    */

        mtuProfileIndex = 8;

        st = cpssDxChIpMtuProfileSet(dev, mtuProfileIndex, mtu);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, mtuProfileIndex);

        /* 1.3. Call with mtuProfileIndex [1] and mtu [0xFFFFFFFF]
           (no any constraints). Expected: GT_OK.   */

        mtuProfileIndex = 1;
        mtu = 0xFFFFFFFF;

        st = cpssDxChIpMtuProfileSet(dev, mtuProfileIndex, mtu);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mtuProfileIndex, mtu);
    }

    mtuProfileIndex = 0;
    mtu = 100;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpMtuProfileSet(dev, mtuProfileIndex, mtu);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpMtuProfileSet(dev, mtuProfileIndex, mtu);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpv6AddrPrefixScopeSet
(
    IN  GT_U8                           devNum,
    IN  GT_IPV6ADDR                     prefix,
    IN  GT_U32                          prefixLen,
    IN  CPSS_IPV6_PREFIX_SCOPE_ENT      addressScope,
    IN  GT_U32                          prefixScopeIndex
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpv6AddrPrefixScopeSet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with prefix [200, …, 200, 10, 1],prefixLen [1 / 4 / 8 / 16], addressScope [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E / CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E / CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E / CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E] and prefixScopeIndex [0 / 1 / 2 / 3].
Expected: GT_OK.
1.2. Call with out of range prefixLen [17] and other parameters from 1.1.
Expected: NOT GT_OK.
1.3. Call with wrong enum values addressScope  and other parameters from 1.1.
Expected: GT_BAD_PARAM.
1.4. Call with out of range prefixScopeIndex [5] and other parameters from 1.1.
Expected: NOT GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_IPV6ADDR                     prefix;
    GT_U32                          prefixLen;
    CPSS_IPV6_PREFIX_SCOPE_ENT      addressScope;
    GT_U32                          prefixScopeIndex;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with prefix [200, …, 200, 10, 1],prefixLen [1 / 4 / 8
           / 16], addressScope [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
           CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
           CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
           CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E] and prefixScopeIndex
           [0 / 1 / 2 / 3]. Expected: GT_OK.    */

        cpssOsMemSet((GT_VOID*) &(prefix), 200, sizeof(prefix));
        prefix.arIP[14] = 10;
        prefix.arIP[15] = 1;
        prefixLen = 1;
        addressScope = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        prefixScopeIndex = 0;

        st = cpssDxChIpv6AddrPrefixScopeSet(dev, prefix, prefixLen,
                                            addressScope, prefixScopeIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "%d, prefixLen=%d, addressScope=%d, prefixScopeIndex=%d",
                   dev, prefixLen, addressScope, prefixScopeIndex);

        prefixLen = 4;
        addressScope = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        prefixScopeIndex = 1;

        st = cpssDxChIpv6AddrPrefixScopeSet(dev, prefix, prefixLen,
                                            addressScope, prefixScopeIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "%d, prefixLen=%d, addressScope=%d, prefixScopeIndex=%d",
                   dev, prefixLen, addressScope, prefixScopeIndex);

        prefixLen = 16;
        addressScope = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
        prefixScopeIndex = 3;

        st = cpssDxChIpv6AddrPrefixScopeSet(dev, prefix, prefixLen,
                                            addressScope, prefixScopeIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "%d, prefixLen=%d, addressScope=%d, prefixScopeIndex=%d",
                   dev, prefixLen, addressScope, prefixScopeIndex);

        /* 1.2. Call with out of range prefixLen [17] and other parameters
                from 1.1. Expected: NOT GT_OK.    */

        prefixLen = 17;
        addressScope = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        prefixScopeIndex = 0;

        st = cpssDxChIpv6AddrPrefixScopeSet(dev, prefix, prefixLen,
                                            addressScope, prefixScopeIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, prefixLen = %d",
                                         dev, prefixLen);
        prefixLen = 1;  /* restore */

        /* 1.3. Call with wrong enum values addressScope  and
           other parameters from 1.1. Expected: GT_BAD_PARAM.   */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpv6AddrPrefixScopeSet
                            (dev, prefix, prefixLen, addressScope, prefixScopeIndex),
                            addressScope);

        /* 1.4. Call with out of range prefixScopeIndex [5] and other
           parameters from 1.1. Expected: NOT GT_OK.    */

        prefixScopeIndex = 5;

        st = cpssDxChIpv6AddrPrefixScopeSet(dev, prefix, prefixLen,
                                            addressScope, prefixScopeIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, prefixScopeIndex = %d",
                                     dev, prefixScopeIndex);

        prefixScopeIndex = 0;  /* restore */
    }

    cpssOsMemSet((GT_VOID*) &(prefix), 200, sizeof(prefix));
    prefix.arIP[14] = 10;
    prefix.arIP[15] = 1;
    prefixLen = 1;
    addressScope = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    prefixScopeIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpv6AddrPrefixScopeSet(dev, prefix, prefixLen,
                                            addressScope, prefixScopeIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpv6AddrPrefixScopeSet(dev, prefix, prefixLen,
                                        addressScope, prefixScopeIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpv6UcScopeCommandSet
(
    IN GT_U8                      devNum,
    IN CPSS_IPV6_PREFIX_SCOPE_ENT addressScopeSrc,
    IN CPSS_IPV6_PREFIX_SCOPE_ENT addressScopeDest,
    IN GT_BOOL                    borderCrossed,
    IN CPSS_PACKET_CMD_ENT        scopeCommand
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpv6UcScopeCommandSet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with addressScopeSrc [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E / CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E / CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E / CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E], addressScopeDest [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E / CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E / CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E / CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E], borderCrossed [GT_TRUE and GT_FALSE] and scopeCommand [CPSS_PACKET_CMD_ROUTE_E / CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E / CPSS_PACKET_CMD_TRAP_TO_CPU_E / CPSS_PACKET_CMD_DROP_SOFT_E / CPSS_PACKET_CMD_DROP_HARD_E].
Expected: GT_OK.
1.2. Call with scopeCommand [CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E / CPSS_PACKET_CMD_BRIDGE_E / CPSS_PACKET_CMD_NONE_E / CPSS_PACKET_CMD_FORWARD_E / CPSS_PACKET_CMD_MIRROR_TO_CPU_E] (these commands are not possible) and other parameters from 1.1.
Expected: NOT GT_OK.
1.3. Call with wrong enum values addressScopeSrc  and other parameters from 1.1.
Expected: GT_BAD_PARAM.
1.4. Call with wrong enum values addressScopeDest  and other parameters from 1.1.
Expected: GT_BAD_PARAM.
1.5. Call with wrong enum values scopeCommand  and other parameters from 1.1.
Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_IPV6_PREFIX_SCOPE_ENT addressScopeSrc;
    CPSS_IPV6_PREFIX_SCOPE_ENT addressScopeDest;
    GT_BOOL                    borderCrossed;
    CPSS_PACKET_CMD_ENT        scopeCommand;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with addressScopeSrc [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E
           / CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
           CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
           CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E], addressScopeDest
           [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E /
           CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
           CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
           CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E], borderCrossed
           [GT_TRUE and GT_FALSE] and scopeCommand [CPSS_PACKET_CMD_ROUTE_E
           / CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E / CPSS_PACKET_CMD_TRAP_TO_CPU_E
           / CPSS_PACKET_CMD_DROP_SOFT_E / CPSS_PACKET_CMD_DROP_HARD_E].
            Expected: GT_OK.    */

        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
        borderCrossed = GT_TRUE;
        scopeCommand = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssDxChIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                           borderCrossed, scopeCommand);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, addressScopeSrc,
                              addressScopeDest, borderCrossed, scopeCommand);

        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        borderCrossed = GT_TRUE;
        scopeCommand = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;

        st = cpssDxChIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                           borderCrossed, scopeCommand);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, addressScopeSrc,
                              addressScopeDest, borderCrossed, scopeCommand);

        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        borderCrossed = GT_FALSE;
        scopeCommand = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssDxChIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                           borderCrossed, scopeCommand);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, addressScopeSrc,
                              addressScopeDest, borderCrossed, scopeCommand);

        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        borderCrossed = GT_FALSE;
        scopeCommand = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssDxChIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                           borderCrossed, scopeCommand);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, addressScopeSrc,
                              addressScopeDest, borderCrossed, scopeCommand);

        /* 1.2. Call with scopeCommand [CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E
           / CPSS_PACKET_CMD_BRIDGE_E / CPSS_PACKET_CMD_NONE_E /
             CPSS_PACKET_CMD_FORWARD_E / CPSS_PACKET_CMD_MIRROR_TO_CPU_E]
           (these commands are not possible) and other parameters from 1.1.
            Expected: NOT GT_OK.    */

        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
        borderCrossed = GT_TRUE;
        scopeCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;

        st = cpssDxChIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                           borderCrossed, scopeCommand);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, scopeCommand = %d",
                                         dev, scopeCommand);

        scopeCommand = CPSS_PACKET_CMD_BRIDGE_E;

        st = cpssDxChIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                           borderCrossed, scopeCommand);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, scopeCommand = %d",
                                         dev, scopeCommand);

        scopeCommand = CPSS_PACKET_CMD_NONE_E;

        st = cpssDxChIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                           borderCrossed, scopeCommand);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, scopeCommand = %d",
                                         dev, scopeCommand);

        scopeCommand = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssDxChIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                           borderCrossed, scopeCommand);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, scopeCommand = %d",
                                         dev, scopeCommand);

        scopeCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssDxChIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                           borderCrossed, scopeCommand);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, scopeCommand = %d",
                                         dev, scopeCommand);

        scopeCommand = CPSS_PACKET_CMD_ROUTE_E; /* restore */

        /* 1.3. Call with wrong enum values addressScopeSrc  and
           other parameters from 1.1. Expected: GT_BAD_PARAM.   */

        UTF_ENUMS_CHECK_MAC(cpssDxChIpv6UcScopeCommandSet
                            (dev, addressScopeSrc, addressScopeDest, borderCrossed, scopeCommand),
                            addressScopeSrc);

        /* 1.4. Call with wrong enum values addressScopeDest  and
           other parameters from 1.1. Expected: GT_BAD_PARAM.   */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpv6UcScopeCommandSet
                            (dev, addressScopeSrc, addressScopeDest, borderCrossed, scopeCommand),
                            addressScopeDest);

        /* 1.5. Call with wrong enum values scopeCommand  and other
           parameters from 1.1. Expected: GT_BAD_PARAM. */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpv6UcScopeCommandSet
                            (dev, addressScopeSrc, addressScopeDest, borderCrossed, scopeCommand),
                            scopeCommand);
    }

    addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
    borderCrossed = GT_TRUE;
    scopeCommand = CPSS_PACKET_CMD_ROUTE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                           borderCrossed, scopeCommand);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpv6UcScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                       borderCrossed, scopeCommand);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpv6McScopeCommandSet
(
    IN GT_U8                            devNum,
    IN CPSS_IPV6_PREFIX_SCOPE_ENT       addressScopeSrc,
    IN CPSS_IPV6_PREFIX_SCOPE_ENT       addressScopeDest,
    IN GT_BOOL                          borderCrossed,
    IN CPSS_PACKET_CMD_ENT              scopeCommand,
    IN CPSS_IPV6_MLL_SELECTION_RULE_ENT mllSelectionRule
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpv6McScopeCommandSet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
1.1. Call with addressScopeSrc [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E / CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E / CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E / CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E], addressScopeDest [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E / CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E / CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E / CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E], borderCrossed [GT_TRUE and GT_FALSE], scopeCommand [CPSS_PACKET_CMD_ROUTE_E / CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E / CPSS_PACKET_CMD_TRAP_TO_CPU_E / CPSS_PACKET_CMD_DROP_SOFT_E / CPSS_PACKET_CMD_DROP_HARD_E / CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E / CPSS_PACKET_CMD_BRIDGE_E] and mllSelectionRule [CPSS_IPV6_MLL_SELECTION_RULE_LOCAL_E / CPSS_IPV6_MLL_SELECTION_RULE_GLOBAL_E].
Expected: GT_OK.
1.2. Call with scopeCommand [CPSS_PACKET_CMD_FORWARD_E / CPSS_PACKET_CMD_MIRROR_TO_CPU_E / CPSS_PACKET_CMD_NONE_E] (these commands are not possible) and other parameters from 1.1.
Expected: NOT GT_OK.
1.3. Call with wrong enum values addressScopeSrc  and other parameters from 1.1.
Expected: GT_BAD_PARAM.
1.4. Call with wrong enum values addressScopeDest  and other parameters from 1.1.
Expected: GT_BAD_PARAM.
1.5. Call with wrong enum values scopeCommand  and other parameters from 1.1.
Expected: GT_BAD_PARAM.
1.6. Call with wrong enum values mllSelectionRule  and other parameters from 1.1.
Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_IPV6_PREFIX_SCOPE_ENT       addressScopeSrc;
    CPSS_IPV6_PREFIX_SCOPE_ENT       addressScopeDest;
    GT_BOOL                          borderCrossed;
    CPSS_PACKET_CMD_ENT              scopeCommand;
    CPSS_IPV6_MLL_SELECTION_RULE_ENT mllSelectionRule;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with addressScopeSrc [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E
           / CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E /
           CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E /
           CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E], addressScopeDest
           [CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E / CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E
           / CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E / CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E],
           borderCrossed [GT_TRUE and GT_FALSE], scopeCommand [CPSS_PACKET_CMD_ROUTE_E
           / CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E / CPSS_PACKET_CMD_TRAP_TO_CPU_E /
           CPSS_PACKET_CMD_DROP_SOFT_E / CPSS_PACKET_CMD_DROP_HARD_E /
           CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E / CPSS_PACKET_CMD_BRIDGE_E] and
           mllSelectionRule [CPSS_IPV6_MLL_SELECTION_RULE_LOCAL_E /
           CPSS_IPV6_MLL_SELECTION_RULE_GLOBAL_E]. Expected: GT_OK. */

        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
        borderCrossed = GT_TRUE;
        scopeCommand = CPSS_PACKET_CMD_ROUTE_E;
        mllSelectionRule = CPSS_IPV6_MLL_SELECTION_RULE_LOCAL_E;

        st = cpssDxChIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                           borderCrossed, scopeCommand, mllSelectionRule);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest,
                                    borderCrossed, scopeCommand, mllSelectionRule);

        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        borderCrossed = GT_TRUE;
        scopeCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
        mllSelectionRule = CPSS_IPV6_MLL_SELECTION_RULE_LOCAL_E;

        st = cpssDxChIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                           borderCrossed, scopeCommand, mllSelectionRule);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest,
                                    borderCrossed, scopeCommand, mllSelectionRule);

        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        borderCrossed = GT_FALSE;
        scopeCommand = CPSS_PACKET_CMD_BRIDGE_E;
        mllSelectionRule = CPSS_IPV6_MLL_SELECTION_RULE_GLOBAL_E;

        st = cpssDxChIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                           borderCrossed, scopeCommand, mllSelectionRule);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest,
                                    borderCrossed, scopeCommand, mllSelectionRule);

        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        borderCrossed = GT_FALSE;
        scopeCommand = CPSS_PACKET_CMD_DROP_HARD_E;
        mllSelectionRule = CPSS_IPV6_MLL_SELECTION_RULE_GLOBAL_E;

        st = cpssDxChIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                           borderCrossed, scopeCommand, mllSelectionRule);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, addressScopeSrc, addressScopeDest,
                                    borderCrossed, scopeCommand, mllSelectionRule);

        /* 1.2. Call with scopeCommand [CPSS_PACKET_CMD_FORWARD_E /
           CPSS_PACKET_CMD_MIRROR_TO_CPU_E / CPSS_PACKET_CMD_NONE_E]
           (these commands are not possible) and other parameters from 1.1.
            Expected: NOT GT_OK.    */


        addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
        borderCrossed = GT_TRUE;
        scopeCommand = CPSS_PACKET_CMD_FORWARD_E;
        mllSelectionRule = CPSS_IPV6_MLL_SELECTION_RULE_LOCAL_E;

        st = cpssDxChIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                           borderCrossed, scopeCommand, mllSelectionRule);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, scopeCommand = %d",
                                         dev, scopeCommand);

        scopeCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssDxChIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                           borderCrossed, scopeCommand, mllSelectionRule);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, scopeCommand = %d",
                                         dev, scopeCommand);

        scopeCommand = CPSS_PACKET_CMD_NONE_E;

        st = cpssDxChIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                           borderCrossed, scopeCommand, mllSelectionRule);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, scopeCommand = %d",
                                         dev, scopeCommand);

        scopeCommand = CPSS_PACKET_CMD_ROUTE_E; /* restore */

        /* 1.3. Call with wrong enum values addressScopeSrc  and
           other parameters from 1.1. Expected: GT_BAD_PARAM.   */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpv6McScopeCommandSet(dev, addressScopeSrc,
                    addressScopeDest, borderCrossed, scopeCommand, mllSelectionRule),
                    addressScopeSrc);

        /* 1.4. Call with wrong enum values addressScopeDest  and
           other parameters from 1.1. Expected: GT_BAD_PARAM.   */

        UTF_ENUMS_CHECK_MAC(cpssDxChIpv6McScopeCommandSet(dev, addressScopeSrc,
                    addressScopeDest, borderCrossed, scopeCommand, mllSelectionRule),
                    addressScopeDest);

        /* 1.5. Call with wrong enum values scopeCommand  and other
           parameters from 1.1. Expected: GT_BAD_PARAM. */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpv6McScopeCommandSet(dev, addressScopeSrc,
                    addressScopeDest, borderCrossed, scopeCommand, mllSelectionRule),
                    scopeCommand);

        /* 1.6. Call with wrong enum values mllSelectionRule  and
           other parameters from 1.1. Expected: GT_BAD_PARAM.   */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpv6McScopeCommandSet(dev, addressScopeSrc,
                    addressScopeDest, borderCrossed, scopeCommand, mllSelectionRule),
                    mllSelectionRule);
    }

    addressScopeSrc = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
    addressScopeDest = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
    borderCrossed = GT_TRUE;
    scopeCommand = CPSS_PACKET_CMD_ROUTE_E;
    mllSelectionRule = CPSS_IPV6_MLL_SELECTION_RULE_LOCAL_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                           borderCrossed, scopeCommand, mllSelectionRule);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpv6McScopeCommandSet(dev, addressScopeSrc, addressScopeDest,
                                       borderCrossed, scopeCommand, mllSelectionRule);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpRouterMacSaBaseSet
(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *macPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpRouterMacSaBaseSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1.1. Call with macPtr [{0,1, 2, 3, 4, 5}].
    Expected: GT_OK.
    1.1.2. Call cpssDxChIpRouterMacSaBaseGet  with non-NULL macPtr
    Expected: GT_OK and mac tha same as just written.
    1.1.3. Call with macPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_ETHERADDR    mac = {{1, 2, 3, 4, 5}};
    GT_ETHERADDR    retMac;
    GT_BOOL         isEqual = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1.1. Call with macPtr [{0,1, 2, 3, 4, 5}].
        Expected: GT_OK. */
        st = cpssDxChIpRouterMacSaBaseSet(dev, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.1.2. Call cpssDxChIpRouterMacSaBaseGet  with non-NULL macPtr
        Expected: GT_OK and mac tha same as just written. */
        st = cpssDxChIpRouterMacSaBaseGet(dev, &retMac);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                     "cpssDxChIpRouterMacSaBaseGet: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&mac, (GT_VOID*)&retMac, sizeof (mac.arEther[0])*5))
                   ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                     "get another mac than was set: %d", dev);

        /* 1.1.3. Call with macPtr [NULL].
        Expected: GT_BAD_PTR. */
        st = cpssDxChIpRouterMacSaBaseSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpRouterMacSaBaseSet(dev, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpRouterMacSaBaseSet(dev, &mac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpRouterMacSaBaseGet
(
    IN  GT_U8           devNum,
    OUT GT_ETHERADDR    *macPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpRouterMacSaBaseGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1.1. Call with non-NULL macPtr
    Expected: GT_OK.
    1.1.2. Call with macPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_ETHERADDR    retMac;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1.1. Call with non-NULL macPtr
        Expected: GT_OK. */
        st = cpssDxChIpRouterMacSaBaseGet(dev, &retMac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.1.2. Call with macPtr [NULL].
        Expected: GT_BAD_PTR. */
        st = cpssDxChIpRouterMacSaBaseGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpRouterMacSaBaseGet(dev, &retMac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpRouterMacSaBaseGet(dev, &retMac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpRouterMacSaLsbModeSet
(
    IN  GT_U8                        devNum,
    IN  CPSS_MAC_SA_LSB_MODE_ENT     saLsbMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpRouterMacSaLsbModeSet)
{
/*
    ITERATE_DEVICES (DxCh1, DxCh2)
    1.1. Call with saLsbMode [CPSS_SA_LSB_PER_PORT_E, CPSS_SA_LSB_PER_VLAN_E]. Expected: GT_OK.
    1.2. Call cpssDxChIpRouterMacSaLsbModeGet with non-NULL saLsbMode. Expected: GT_OK and saLsbMode the same as has been just set.
    1.3. Call with wrong enum values saLsbMode . Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_MAC_SA_LSB_MODE_ENT    saLsbMode    = CPSS_SA_LSB_PER_PORT_E;
    CPSS_MAC_SA_LSB_MODE_ENT    retSaLsbMode = CPSS_SA_LSB_PER_PORT_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with saLsbMode [CPSS_SA_LSB_PER_PORT_E,
        CPSS_SA_LSB_PER_VLAN_E]. Expected: GT_OK. */
        /* 1.2. Call cpssDxChIpRouterMacSaLsbModeGet with non-NULL saLsbMode.
        Expected: GT_OK and saLsbMode the same as has been just set. */

        /* 1.1. for saLsbMode = CPSS_SA_LSB_PER_PORT_E */
        saLsbMode = CPSS_SA_LSB_PER_PORT_E;

        st = cpssDxChIpRouterMacSaLsbModeSet(dev, saLsbMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, saLsbMode);

        /* 1.2. for saLsbMode = CPSS_SA_LSB_PER_PORT_E */
        st = cpssDxChIpRouterMacSaLsbModeGet(dev, &retSaLsbMode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "cpssDxChIpRouterMacSaLsbModeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(saLsbMode, retSaLsbMode,
            "get another saLsbMode than was set: %d", dev);

        /* 1.1. for saLsbMode = CPSS_SA_LSB_PER_VLAN_E */
        saLsbMode = CPSS_SA_LSB_PER_VLAN_E;

        st = cpssDxChIpRouterMacSaLsbModeSet(dev, saLsbMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, saLsbMode);

        /* 1.2. for saLsbMode = CPSS_SA_LSB_PER_VLAN_E */
        st = cpssDxChIpRouterMacSaLsbModeGet(dev, &retSaLsbMode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "cpssDxChIpRouterMacSaLsbModeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(saLsbMode, retSaLsbMode,
            "get another saLsbMode than was set: %d", dev);

        /* 1.3. Call with wrong enum values saLsbMode .
        Expected: GT_BAD_PARAM. */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpRouterMacSaLsbModeSet
                            (dev, saLsbMode),
                            saLsbMode);
    }

    saLsbMode = CPSS_SA_LSB_PER_PORT_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpRouterMacSaLsbModeSet(dev, saLsbMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpRouterMacSaLsbModeSet(dev, saLsbMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpRouterMacSaLsbModeGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_MAC_SA_LSB_MODE_ENT     *saLsbMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpRouterMacSaLsbModeGet)
{
/*
    ITERATE_DEVICES (DxCh1, DxCh2)
    1.1. Call with non-NULL saLsbMode. Expected: GT_OK.
    1.2. Call with saLsbMode [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_MAC_SA_LSB_MODE_ENT    retSaLsbMode = CPSS_SA_LSB_PER_PORT_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1.1. Call with non-NULL saLsbMode
        Expected: GT_OK. */

        st = cpssDxChIpRouterMacSaLsbModeGet(dev, &retSaLsbMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.1.2. Call with saLsbMode [NULL].
        Expected: GT_BAD_PTR. */
        st = cpssDxChIpRouterMacSaLsbModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpRouterMacSaLsbModeGet(dev, &retSaLsbMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpRouterMacSaLsbModeGet(dev, &retSaLsbMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpPortRouterMacSaLsbModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_MAC_SA_LSB_MODE_ENT    saLsbMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpPortRouterMacSaLsbModeSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (DxCh3 and above)
    1.1.1. Call with saLsbMode [CPSS_SA_LSB_PER_PORT_E /
                                CPSS_SA_LSB_PER_PKT_VID_E /
                                CPSS_SA_LSB_PER_VLAN_E].
    Expected: GT_OK.
    1.1.2. Call cpssDxChIpPortRouterMacSaLsbModeGet with non-NULL saLsbModePtr.
    Expected: GT_OK and the same saLsbMode.
    1.1.3. Call with wrong enum values saLsbMode .
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = IP_CTRL_VALID_PHY_PORT_CNS;

    CPSS_MAC_SA_LSB_MODE_ENT    saLsbMode    = CPSS_SA_LSB_PER_PORT_E;
    CPSS_MAC_SA_LSB_MODE_ENT    saLsbModeGet = CPSS_SA_LSB_PER_PORT_E;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with saLsbMode [CPSS_SA_LSB_PER_PORT_E /
                                            CPSS_SA_LSB_PER_PKT_VID_E /
                                            CPSS_SA_LSB_PER_VLAN_E].
                Expected: GT_OK.
            */

            /* Call with saLsbMode [CPSS_SA_LSB_PER_PORT_E] */
            saLsbMode = CPSS_SA_LSB_PER_PORT_E;

            st = cpssDxChIpPortRouterMacSaLsbModeSet(dev, port, saLsbMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, saLsbMode);

            /*
                1.1.2. Call cpssDxChIpPortRouterMacSaLsbModeGet with non-NULL saLsbModePtr.
                Expected: GT_OK and the same saLsbMode.
            */
            st = cpssDxChIpPortRouterMacSaLsbModeGet(dev, port, &saLsbModeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChIpPortRouterMacSaLsbModeGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(saLsbMode, saLsbModeGet,
                       "get another saLsbMode than was set: %d, %d", dev, port);

            /* Call with saLsbMode [CPSS_SA_LSB_PER_PKT_VID_E] */
            saLsbMode = CPSS_SA_LSB_PER_PKT_VID_E;

            st = cpssDxChIpPortRouterMacSaLsbModeSet(dev, port, saLsbMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, saLsbMode);

            /*
                1.1.2. Call cpssDxChIpPortRouterMacSaLsbModeGet with non-NULL saLsbModePtr.
                Expected: GT_OK and the same saLsbMode.
            */
            st = cpssDxChIpPortRouterMacSaLsbModeGet(dev, port, &saLsbModeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChIpPortRouterMacSaLsbModeGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(saLsbMode, saLsbModeGet,
                       "get another saLsbMode than was set: %d, %d", dev, port);

            /* Call with saLsbMode [CPSS_SA_LSB_PER_VLAN_E] */
            saLsbMode = CPSS_SA_LSB_PER_VLAN_E;

            st = cpssDxChIpPortRouterMacSaLsbModeSet(dev, port, saLsbMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, saLsbMode);

            /*
                1.1.2. Call cpssDxChIpPortRouterMacSaLsbModeGet with non-NULL saLsbModePtr.
                Expected: GT_OK and the same saLsbMode.
            */
            st = cpssDxChIpPortRouterMacSaLsbModeGet(dev, port, &saLsbModeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChIpPortRouterMacSaLsbModeGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(saLsbMode, saLsbModeGet,
                       "get another saLsbMode than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with wrong enum values saLsbMode.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChIpPortRouterMacSaLsbModeSet
                                (dev, port, saLsbMode),
                                saLsbMode);
        }

        saLsbMode = CPSS_SA_LSB_PER_PORT_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChIpPortRouterMacSaLsbModeSet(dev, port, saLsbMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChIpPortRouterMacSaLsbModeSet(dev, port, saLsbMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChIpPortRouterMacSaLsbModeSet(dev, port, saLsbMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    saLsbMode = CPSS_SA_LSB_PER_PORT_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = IP_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpPortRouterMacSaLsbModeSet(dev, port, saLsbMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpPortRouterMacSaLsbModeSet(dev, port, saLsbMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssDxChIpPortRouterMacSaLsbModeGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    OUT CPSS_MAC_SA_LSB_MODE_ENT    *saLsbModePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpPortRouterMacSaLsbModeGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (DxCh3 and above)
    1.1.1. Call with non-NULL saLsbModePtr.
    Expected: GT_OK.
    1.1.2. Call with saLsbModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = IP_CTRL_VALID_PHY_PORT_CNS;

    CPSS_MAC_SA_LSB_MODE_ENT    saLsbMode = CPSS_SA_LSB_PER_PORT_E;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL saLsbModePtr.
                Expected: GT_OK.
            */
            st = cpssDxChIpPortRouterMacSaLsbModeGet(dev, port, &saLsbMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with saLsbModePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChIpPortRouterMacSaLsbModeGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, saLsbModePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChIpPortRouterMacSaLsbModeGet(dev, port, &saLsbMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChIpPortRouterMacSaLsbModeGet(dev, port, &saLsbMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChIpPortRouterMacSaLsbModeGet(dev, port, &saLsbMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = IP_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpPortRouterMacSaLsbModeGet(dev, port, &saLsbMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpPortRouterMacSaLsbModeGet(dev, port, &saLsbMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpRouterPortVlanMacSaLsbSet
(
    IN GT_U8   devNum,
    IN GT_U16  vlanPortId,
    IN GT_U8   saMac
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpRouterPortVlanMacSaLsbSet)
{
/*
    ITERATE_DEVICES (DxCh1, DxCh2)
    1.1. Call with vlanPortid [32] and saMac [251]. Expected: GT_OK.
    1.2. Call cpssDxChIpRouterPortVlanMacSaLsbGet with vlanPortId [32] and non-NULL saMac. Expected: GT_OK and saMac the same as has been just set.
    1.3. Call with out of range vlanPortId [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096]. Expected: NON GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16  vlanPortId = 0;
    GT_U8   saMac      = 0;
    GT_U8   retSaMac   = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with vlanPortid [32] and saMac [251].
        Expected: GT_OK. */
        vlanPortId = 32;
        saMac = 251;

        st = cpssDxChIpRouterPortVlanMacSaLsbSet(dev, vlanPortId, saMac);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanPortId, saMac);

        /* 1.2. Call cpssDxChIpRouterPortVlanMacSaLsbGet with vlanPortId [32]
        and non-NULL saMac.
        Expected: GT_OK and saMac the same as has been just set. */
        st = cpssDxChIpRouterPortVlanMacSaLsbGet(dev, vlanPortId, &retSaMac);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
            "cpssDxChIpRouterPortVlanMacSaLsbGet: %d, %d", dev, vlanPortId);

        UTF_VERIFY_EQUAL2_STRING_MAC(saMac, retSaMac,
            "get another saMac than was set: %d, %d", dev, vlanPortId);

        /* 1.3. Call with out of range
        vlanPortId [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096].
        Expected: NON GT_OK. */
        vlanPortId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChIpRouterPortVlanMacSaLsbSet(dev, vlanPortId, saMac);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanPortId, saMac);
    }

    vlanPortId = 1;
    saMac = 251;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpRouterPortVlanMacSaLsbSet(dev, vlanPortId, saMac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpRouterPortVlanMacSaLsbSet(dev, vlanPortId, saMac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpRouterPortVlanMacSaLsbGet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanPortId,
    OUT GT_U8   *saMac
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpRouterPortVlanMacSaLsbGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanPortid [32] and non-NULL saMac. Expected: GT_OK.
    1.2. Call with out of range vlanPortId [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096]. Expected: NON GT_OK.
    1.3. Call with vlanPortId [1] and saMac [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16  vlanPortId = 0;
    GT_U8   retSaMac   = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with vlanPortid [32] and non-NULL saMac.
        Expected: GT_OK. */
        vlanPortId = 32;

        st = cpssDxChIpRouterPortVlanMacSaLsbGet(dev, vlanPortId, &retSaMac);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanPortId);

        /* 1.2. Call with out of range
        vlanPortId [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096].
        Expected: NON GT_OK. */
        vlanPortId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChIpRouterPortVlanMacSaLsbGet(dev, vlanPortId, &retSaMac);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanPortId);

        /* 1.3. Call with vlanPortId [1] and saMac [NULL].
        Expected: GT_BAD_PTR. */
        vlanPortId = 1;

        st = cpssDxChIpRouterPortVlanMacSaLsbGet(dev, vlanPortId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL",
                                     dev, vlanPortId);
    }

    vlanPortId = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpRouterPortVlanMacSaLsbGet(dev, vlanPortId, &retSaMac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpRouterPortVlanMacSaLsbGet(dev, vlanPortId, &retSaMac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpRouterPortMacSaLsbSet
(
    IN GT_U8   devNum,
    IN GT_U8   portNum,
    IN GT_U8   saMac
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpRouterPortMacSaLsbSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (DxCh3 and above)
    1.1.1. Call with saMac [0 / 0xFF].
    Expected: GT_OK.
    1.1.2. Call cpssDxChIpRouterPortMacSaLsbGet with non-NULL saMacPtr.
    Expected: GT_OK and the same saMac.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = IP_CTRL_VALID_PHY_PORT_CNS;

    GT_U8       saMac    = 0;
    GT_U8       saMacGet = 0;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with saMac [0 / 0xFF].
                Expected: GT_OK.
            */

            /* Call with saMac [0] */
            saMac = 0;

            st = cpssDxChIpRouterPortMacSaLsbSet(dev, port, saMac);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, saMac);

            /*
                1.1.2. Call cpssDxChIpRouterPortMacSaLsbGet with non-NULL saMacPtr.
                Expected: GT_OK and the same saMac.
            */
            st = cpssDxChIpRouterPortMacSaLsbGet(dev, port, &saMacGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChIpRouterPortMacSaLsbGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(saMac, saMacGet,
                       "get another saMac than was set: %d, %d", dev, port);

            /* Call with saMac [0xFF] */
            saMac = 0xFF;

            st = cpssDxChIpRouterPortMacSaLsbSet(dev, port, saMac);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, saMac);

            /*
                1.1.2. Call cpssDxChIpRouterPortMacSaLsbGet with non-NULL saMacPtr.
                Expected: GT_OK and the same saMac.
            */
            st = cpssDxChIpRouterPortMacSaLsbGet(dev, port, &saMacGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChIpRouterPortMacSaLsbGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(saMac, saMacGet,
                       "get another saMac than was set: %d, %d", dev, port);
        }

        saMac = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChIpRouterPortMacSaLsbSet(dev, port, saMac);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChIpRouterPortMacSaLsbSet(dev, port, saMac);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChIpRouterPortMacSaLsbSet(dev, port, saMac);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    saMac = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = IP_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpRouterPortMacSaLsbSet(dev, port, saMac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpRouterPortMacSaLsbSet(dev, port, saMac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpRouterPortMacSaLsbGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_U8   *saMacPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpRouterPortMacSaLsbGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (DxCh3 and above)
    1.1.1. Call with non-NULL saLsbModePtr.
    Expected: GT_OK.
    1.1.2. Call with saLsbModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = IP_CTRL_VALID_PHY_PORT_CNS;

    GT_U8   saMac = 0;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL saLsbModePtr.
                Expected: GT_OK.
            */
            st = cpssDxChIpRouterPortMacSaLsbGet(dev, port, &saMac);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with saLsbModePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChIpRouterPortMacSaLsbGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, saLsbModePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChIpRouterPortMacSaLsbGet(dev, port, &saMac);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChIpRouterPortMacSaLsbGet(dev, port, &saMac);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChIpRouterPortMacSaLsbGet(dev, port, &saMac);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = IP_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpRouterPortMacSaLsbGet(dev, port, &saMac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpRouterPortMacSaLsbGet(dev, port, &saMac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpRouterVlanMacSaLsbSet
(
    IN GT_U8   devNum,
    IN GT_U16  vlan,
    IN GT_U8   saMac
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpRouterVlanMacSaLsbSet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call with vlan [100 / 4095]
                   and saMac [0 / 0xFF].
    Expected: GT_OK.
    1.2. Call cpssDxChIpRouterVlanMacSaLsbGet with the same vlan and non-NULL saMacPtr.
    Expected: GT_OK and the same saMac.
    1.3. Call with out of range vlan [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16      vlan     = 0;
    GT_U8       saMac    = 0;
    GT_U8       saMacGet = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlan [100 / 4095]
                           and saMac [0 / 0xFF].
            Expected: GT_OK.
        */

        /* Call with vlan [100] */
        vlan  = 100;
        saMac = 0;

        st = cpssDxChIpRouterVlanMacSaLsbSet(dev, vlan, saMac);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlan, saMac);

        /*
            1.2. Call cpssDxChIpRouterVlanMacSaLsbGet with the same vlan and non-NULL saMacPtr.
            Expected: GT_OK and the same saMac.
        */
        st = cpssDxChIpRouterVlanMacSaLsbGet(dev, vlan, &saMacGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChIpRouterVlanMacSaLsbGet: %d, %d", dev, vlan);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(saMac, saMacGet,
                   "get another saMac than was set: %d", dev);

        /* Call with vlan [4095] */
        vlan  = 4095;
        saMac = 0xFF;

        st = cpssDxChIpRouterVlanMacSaLsbSet(dev, vlan, saMac);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlan, saMac);

        /*
            1.2. Call cpssDxChIpRouterVlanMacSaLsbGet with the same vlan and non-NULL saMacPtr.
            Expected: GT_OK and the same saMac.
        */
        st = cpssDxChIpRouterVlanMacSaLsbGet(dev, vlan, &saMacGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChIpRouterVlanMacSaLsbGet: %d, %d", dev, vlan);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(saMac, saMacGet,
                   "get another saMac than was set: %d", dev);

        /*
            1.3. Call with out of range vlan [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        vlan = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChIpRouterVlanMacSaLsbSet(dev, vlan, saMac);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlan);
    }

    vlan  = 100;
    saMac = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpRouterVlanMacSaLsbSet(dev, vlan, saMac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpRouterVlanMacSaLsbSet(dev, vlan, saMac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpRouterVlanMacSaLsbGet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlan,
    OUT GT_U8   *saMacPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpRouterVlanMacSaLsbGet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call with vlan [100 / 4095]
                   and non-NULL saMacPtr.
    Expected: GT_OK.
    1.2. Call with out of range vlan [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with saMacPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16      vlan  = 0;
    GT_U8       saMac = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlan [100 / 4095]
                           and non-NULL saMacPtr.
            Expected: GT_OK.
        */

        /* Call with vlan [100] */
        vlan = 100;

        st = cpssDxChIpRouterVlanMacSaLsbGet(dev, vlan, &saMac);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlan);

        /* Call with vlan [4095] */
        vlan = 4095;

        st = cpssDxChIpRouterVlanMacSaLsbGet(dev, vlan, &saMac);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlan);

        /*
            1.2. Call with out of range vlan [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        vlan = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChIpRouterVlanMacSaLsbGet(dev, vlan, &saMac);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlan);

        vlan = 0;

        /*
            1.3. Call with saMacPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpRouterVlanMacSaLsbGet(dev, vlan, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, saMacPtr = NULL", dev);
    }

    vlan = 100;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpRouterVlanMacSaLsbGet(dev, vlan, &saMac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpRouterVlanMacSaLsbGet(dev, vlan, &saMac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpRouterMacSaModifyEnable
(
    IN  GT_U8                        devNum,
    IN  GT_U8                        portNum,
    IN  GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpRouterMacSaModifyEnable)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1.1. Call with enable [GT_TRUE/ GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChIpRouterMacSaModifyEnableGet with  non-NULL enable.
    Expected: GT_OK and enable values the same as have been just set.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = IP_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     retEnable = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1. Call with portNum [0/ 31] and enable [GT_TRUE/ GT_FALSE].
            Expected: GT_OK. */
            /* 1.2. Call cpssDxChIpRouterMacSaModifyEnableGet with portNum [0/ 31]
            and non-NULL enable.
            Expected: GT_OK and enable values the same as have been just set. */

            /* 1.1. for portNum = 0 and enable =GT_TRUE*/
            enable = GT_TRUE;

            st = cpssDxChIpRouterMacSaModifyEnable(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /* 1.2. for portNum = 0 and enable =GT_TRUE*/
            st = cpssDxChIpRouterMacSaModifyEnableGet(dev, port, &retEnable);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                "cpssDxChIpRouterMacSaModifyEnableGet: %d, %d", dev, port);

            UTF_VERIFY_EQUAL2_STRING_MAC(enable, retEnable,
                "get another enable value than was set: %d, %d", dev, port);

            /* 1.1. enable =GT_FALSE*/
            enable = GT_FALSE;

            st = cpssDxChIpRouterMacSaModifyEnable(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /* 1.2. for enable =GT_FALSE*/
            st = cpssDxChIpRouterMacSaModifyEnableGet(dev, port, &retEnable);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                "cpssDxChIpRouterMacSaModifyEnableGet: %d, %d", dev, port);

            UTF_VERIFY_EQUAL2_STRING_MAC(enable, retEnable,
                "get another enable value than was set: %d, %d", dev, port);
        }

        enable = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChIpRouterMacSaModifyEnable(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChIpRouterMacSaModifyEnable(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChIpRouterMacSaModifyEnable(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;
    port   = IP_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpRouterMacSaModifyEnable(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpRouterMacSaModifyEnable(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpRouterMacSaModifyEnableGet
(
    IN  GT_U8                        devNum,
    IN  GT_U8                        portNum,
    OUT GT_BOOL                      *enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpRouterMacSaModifyEnableGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1.1. Call with non-NULL enable. Expected: GT_OK.
    1.1.2. Call with enable [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = IP_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL     retEnable = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1. Call with non-NULL enable. Expected: GT_OK. */
            st = cpssDxChIpRouterMacSaModifyEnableGet(dev, port, &retEnable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.2. Call with enable [NULL]. Expected: GT_BAD_PTR. */
            st = cpssDxChIpRouterMacSaModifyEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL",
                                         dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChIpRouterMacSaModifyEnableGet(dev, port, &retEnable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChIpRouterMacSaModifyEnableGet(dev, port, &retEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChIpRouterMacSaModifyEnableGet(dev, port, &retEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    port = IP_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpRouterMacSaModifyEnableGet(dev, port, &retEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpRouterMacSaModifyEnableGet(dev, port, &retEnable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpEcmpUcRpfCheckEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpEcmpUcRpfCheckEnableSet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssDxChIpEcmpUcRpfCheckEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssDxChIpEcmpUcRpfCheckEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssDxChIpEcmpUcRpfCheckEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssDxChIpEcmpUcRpfCheckEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChIpEcmpUcRpfCheckEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        /* Call with enable [GT_FALSE] */
        enable = GT_FALSE;

        st = cpssDxChIpEcmpUcRpfCheckEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssDxChIpEcmpUcRpfCheckEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssDxChIpEcmpUcRpfCheckEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChIpEcmpUcRpfCheckEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);
    }

    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpEcmpUcRpfCheckEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpEcmpUcRpfCheckEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpEcmpUcRpfCheckEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpEcmpUcRpfCheckEnableGet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChIpEcmpUcRpfCheckEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpEcmpUcRpfCheckEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpEcmpUcRpfCheckEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpEcmpUcRpfCheckEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpPortGroupCntGet
(
    IN  GT_U8                        devNum,
    IN  GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN  CPSS_IP_CNT_SET_ENT          cntSet,
    OUT CPSS_DXCH_IP_COUNTER_SET_STC *countersPtr
)
*/

typedef enum
{
    PRV_UTF_COUNTER_SET   = 0,
    PRV_UTF_COUNTER_PORT_GROUP_SET = 1,
    PRV_UTF_COUNTER_GET   = 2,
    PRV_UTF_COUNTER_PORT_GROUP_GET = 3
}PRV_UTF_COUNTER_SET_GET_ENT;


static GT_STATUS prvUtfPrivateCpssDxChIpPortGroupCntGetSet
(
    IN   GT_U8                          devNum,
    IN   GT_PORT_GROUPS_BMP             portGroupsBmp,
    IN   GT_U32                         regAddr,
    IN   PRV_UTF_COUNTER_SET_GET_ENT    setGetOption,
    IN   GT_U32                         counterValue,
    OUT  GT_U32                         *counterValuePtr
)
{
    CPSS_DXCH_IP_COUNTER_SET_STC       ipCounter;   /* ip counter */
    GT_STATUS                          st;          /* return status */
    GT_U32                             i;           /* iterator */

    /* reset ipCounter */
    cpssOsMemSet(&ipCounter, 0, sizeof(CPSS_DXCH_IP_COUNTER_SET_STC));

    for(i = 0; i < 4; i++)
   {
       if(regAddr == (0x02800900 + i * 0x100))
       {
           switch(setGetOption)
           {
              case PRV_UTF_COUNTER_SET:
                  ipCounter.inUcPkts = counterValue;
                  return cpssDxChIpCntSet(devNum, i, &ipCounter);

              case PRV_UTF_COUNTER_PORT_GROUP_SET:
                  ipCounter.inUcPkts = counterValue;
                  return cpssDxChIpPortGroupCntSet(devNum, portGroupsBmp,
                                                   i, &ipCounter);
              case PRV_UTF_COUNTER_GET:
                  st = cpssDxChIpCntGet(devNum, i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.inUcPkts;
                  return GT_OK;

              case PRV_UTF_COUNTER_PORT_GROUP_GET:
                  st = cpssDxChIpPortGroupCntGet(devNum, portGroupsBmp,
                                                 i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.inUcPkts;
                  return GT_OK;

              default:
                  return GT_BAD_PARAM;
           }
       }
       if(regAddr == (0x02800904 + i * 0x100))
       {
           switch(setGetOption)
           {
              case PRV_UTF_COUNTER_SET:
                  ipCounter.inMcPkts = counterValue;
                  return cpssDxChIpCntSet(devNum, i, &ipCounter);

              case PRV_UTF_COUNTER_PORT_GROUP_SET:
                  ipCounter.inMcPkts = counterValue;
                  return cpssDxChIpPortGroupCntSet(devNum, portGroupsBmp,
                                                   i, &ipCounter);
              case PRV_UTF_COUNTER_GET:
                  st = cpssDxChIpCntGet(devNum, i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.inMcPkts;
                  return GT_OK;

              case PRV_UTF_COUNTER_PORT_GROUP_GET:
                  st = cpssDxChIpPortGroupCntGet(devNum, portGroupsBmp,
                                                 i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.inMcPkts;
                  return GT_OK;

              default:
                  return GT_BAD_PARAM;
           }

       }
       if(regAddr == (0x02800908 + i * 0x100))
       {
           switch(setGetOption)
           {
              case PRV_UTF_COUNTER_SET:
                  ipCounter.inUcNonRoutedNonExcpPkts = counterValue;
                  return cpssDxChIpCntSet(devNum, i, &ipCounter);

              case PRV_UTF_COUNTER_PORT_GROUP_SET:
                  ipCounter.inUcNonRoutedNonExcpPkts = counterValue;
                  return cpssDxChIpPortGroupCntSet(devNum, portGroupsBmp,
                                                   i, &ipCounter);
              case PRV_UTF_COUNTER_GET:
                  st = cpssDxChIpCntGet(devNum, i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.inUcNonRoutedNonExcpPkts;
                  return GT_OK;

              case PRV_UTF_COUNTER_PORT_GROUP_GET:
                  st = cpssDxChIpPortGroupCntGet(devNum, portGroupsBmp,
                                                 i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.inUcNonRoutedNonExcpPkts;
                  return GT_OK;

              default:
                  return GT_BAD_PARAM;
           }
       }
       if(regAddr == (0x0280090C + i * 0x100))
       {
           switch(setGetOption)
           {
              case PRV_UTF_COUNTER_SET:
                  ipCounter.inUcNonRoutedExcpPkts = counterValue;
                  return cpssDxChIpCntSet(devNum, i, &ipCounter);

              case PRV_UTF_COUNTER_PORT_GROUP_SET:
                  ipCounter.inUcNonRoutedExcpPkts = counterValue;
                  return cpssDxChIpPortGroupCntSet(devNum, portGroupsBmp,
                                                   i, &ipCounter);
              case PRV_UTF_COUNTER_GET:
                  st = cpssDxChIpCntGet(devNum, i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.inUcNonRoutedExcpPkts;
                  return GT_OK;

              case PRV_UTF_COUNTER_PORT_GROUP_GET:
                  st = cpssDxChIpPortGroupCntGet(devNum, portGroupsBmp,
                                                 i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.inUcNonRoutedExcpPkts;
                  return GT_OK;

              default:
                  return GT_BAD_PARAM;
           }
       }
       if(regAddr == (0x02800910 + i * 0x100))
       {
           switch(setGetOption)
           {
              case PRV_UTF_COUNTER_SET:
                  ipCounter.inMcNonRoutedNonExcpPkts = counterValue;
                  return cpssDxChIpCntSet(devNum, i, &ipCounter);

              case PRV_UTF_COUNTER_PORT_GROUP_SET:
                  ipCounter.inMcNonRoutedNonExcpPkts = counterValue;
                  return cpssDxChIpPortGroupCntSet(devNum, portGroupsBmp,
                                                   i, &ipCounter);
              case PRV_UTF_COUNTER_GET:
                  st = cpssDxChIpCntGet(devNum, i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.inMcNonRoutedNonExcpPkts;
                  return GT_OK;

              case PRV_UTF_COUNTER_PORT_GROUP_GET:
                  st = cpssDxChIpPortGroupCntGet(devNum, portGroupsBmp,
                                                 i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.inMcNonRoutedNonExcpPkts;
                  return GT_OK;

              default:
                  return GT_BAD_PARAM;
           }
       }
       if(regAddr == (0x02800914 + i * 0x100))
       {
           switch(setGetOption)
           {
              case PRV_UTF_COUNTER_SET:
                  ipCounter.inMcNonRoutedExcpPkts = counterValue;
                  return cpssDxChIpCntSet(devNum, i, &ipCounter);

              case PRV_UTF_COUNTER_PORT_GROUP_SET:
                  ipCounter.inMcNonRoutedExcpPkts = counterValue;
                  return cpssDxChIpPortGroupCntSet(devNum, portGroupsBmp,
                                                   i, &ipCounter);
              case PRV_UTF_COUNTER_GET:
                  st = cpssDxChIpCntGet(devNum, i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.inMcNonRoutedExcpPkts;
                  return GT_OK;

              case PRV_UTF_COUNTER_PORT_GROUP_GET:
                  st = cpssDxChIpPortGroupCntGet(devNum, portGroupsBmp,
                                                 i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.inMcNonRoutedExcpPkts;
                  return GT_OK;

              default:
                  return GT_BAD_PARAM;
           }
       }
       if(regAddr == (0x02800918 + i * 0x100))
       {
           switch(setGetOption)
           {
              case PRV_UTF_COUNTER_SET:
                  ipCounter.inUcTrappedMirrorPkts = counterValue;
                  return cpssDxChIpCntSet(devNum, i, &ipCounter);

              case PRV_UTF_COUNTER_PORT_GROUP_SET:
                  ipCounter.inUcTrappedMirrorPkts = counterValue;
                  return cpssDxChIpPortGroupCntSet(devNum, portGroupsBmp,
                                                   i, &ipCounter);
              case PRV_UTF_COUNTER_GET:
                  st = cpssDxChIpCntGet(devNum, i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.inUcTrappedMirrorPkts;
                  return GT_OK;

              case PRV_UTF_COUNTER_PORT_GROUP_GET:
                  st = cpssDxChIpPortGroupCntGet(devNum, portGroupsBmp,
                                                 i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.inUcTrappedMirrorPkts;
                  return GT_OK;

              default:
                  return GT_BAD_PARAM;
           }
       }
       if(regAddr == (0x0280091C + i * 0x100))
       {
           switch(setGetOption)
           {
              case PRV_UTF_COUNTER_SET:
                  ipCounter.inMcTrappedMirrorPkts = counterValue;
                  return cpssDxChIpCntSet(devNum, i, &ipCounter);

              case PRV_UTF_COUNTER_PORT_GROUP_SET:
                  ipCounter.inMcTrappedMirrorPkts = counterValue;
                  return cpssDxChIpPortGroupCntSet(devNum, portGroupsBmp,
                                                   i, &ipCounter);
              case PRV_UTF_COUNTER_GET:
                  st = cpssDxChIpCntGet(devNum, i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.inMcTrappedMirrorPkts;
                  return GT_OK;

              case PRV_UTF_COUNTER_PORT_GROUP_GET:
                  st = cpssDxChIpPortGroupCntGet(devNum, portGroupsBmp,
                                                 i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.inMcTrappedMirrorPkts;
                  return GT_OK;

              default:
                  return GT_BAD_PARAM;
           }
       }
       if(regAddr == (0x02800920 + i * 0x100))
       {
           switch(setGetOption)
           {
              case PRV_UTF_COUNTER_SET:
                  ipCounter.mcRfpFailPkts = counterValue;
                  return cpssDxChIpCntSet(devNum, i, &ipCounter);

              case PRV_UTF_COUNTER_PORT_GROUP_SET:
                  ipCounter.mcRfpFailPkts = counterValue;
                  return cpssDxChIpPortGroupCntSet(devNum, portGroupsBmp,
                                                   i, &ipCounter);
              case PRV_UTF_COUNTER_GET:
                  st = cpssDxChIpCntGet(devNum, i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.mcRfpFailPkts;
                  return GT_OK;

              case PRV_UTF_COUNTER_PORT_GROUP_GET:
                  st = cpssDxChIpPortGroupCntGet(devNum, portGroupsBmp,
                                                 i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.mcRfpFailPkts;
                  return GT_OK;

              default:
                  return GT_BAD_PARAM;
           }
       }
       if(regAddr == (0x02800924 + i * 0x100))
       {
           switch(setGetOption)
           {
              case PRV_UTF_COUNTER_SET:
                  ipCounter.outUcRoutedPkts = counterValue;
                  return cpssDxChIpCntSet(devNum, i, &ipCounter);

              case PRV_UTF_COUNTER_PORT_GROUP_SET:
                  ipCounter.outUcRoutedPkts = counterValue;
                  return cpssDxChIpPortGroupCntSet(devNum, portGroupsBmp,
                                                   i, &ipCounter);
              case PRV_UTF_COUNTER_GET:
                  st = cpssDxChIpCntGet(devNum, i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.outUcRoutedPkts;
                  return GT_OK;

              case PRV_UTF_COUNTER_PORT_GROUP_GET:
                  st = cpssDxChIpPortGroupCntGet(devNum, portGroupsBmp,
                                                 i, &ipCounter);
                  if(st != GT_OK)
                      return st;

                  *counterValuePtr = ipCounter.outUcRoutedPkts;
                  return GT_OK;

              default:
                  return GT_BAD_PARAM;
           }
       }
   }
    return GT_OK;
}

static GT_STATUS prvUtfPrivateCpssDxChIpCntSet
(
    IN   GT_U8                          devNum,
    IN   GT_U32                         regAddr,
    IN   GT_U32                         counterValue
)
{
    return prvUtfPrivateCpssDxChIpPortGroupCntGetSet(devNum, 0, regAddr,
                                                     PRV_UTF_COUNTER_SET,
                                                     counterValue,
                                                     NULL);
}

static GT_STATUS prvUtfPrivateCpssDxChIpPortGroupCntSet
(
    IN   GT_U8                          devNum,
    IN   GT_PORT_GROUPS_BMP             portGroupsBmp,
    IN   GT_U32                         regAddr,
    IN   GT_U32                         counterValue
)
{
    return prvUtfPrivateCpssDxChIpPortGroupCntGetSet(devNum, portGroupsBmp,
                                               regAddr,
                                               PRV_UTF_COUNTER_PORT_GROUP_SET,
                                               counterValue,
                                               NULL);
}

static GT_STATUS prvUtfPrivateCpssDxChIpCntGet
(
    IN   GT_U8                          devNum,
    IN   GT_U32                         regAddr,
    OUT  GT_U32                         *counterValuePtr
)
{
    return prvUtfPrivateCpssDxChIpPortGroupCntGetSet(devNum, 0,
                                                     regAddr,
                                                     PRV_UTF_COUNTER_GET,
                                                     0,
                                                     counterValuePtr);
}

static GT_STATUS prvUtfPrivateCpssDxChIpPortGroupCntGet
(
    IN   GT_U8                          devNum,
    IN   GT_PORT_GROUPS_BMP             portGroupsBmp,
    IN   GT_U32                         regAddr,
    OUT  GT_U32                         *counterValuePtr
)
{
    return prvUtfPrivateCpssDxChIpPortGroupCntGetSet(devNum, portGroupsBmp,
                                               regAddr,
                                               PRV_UTF_COUNTER_PORT_GROUP_GET,
                                               0,
                                               counterValuePtr);
}

UTF_TEST_CASE_MAC(cpssDxChIpPortGroupCntGet)
{
    GT_U32 regAddr = 0xFFFFFFFF; /* register address not initialized */
    GT_U32 i;                    /* iterator */
    GT_U32 cntSet;               /* cntSet iterator */
    GT_U8     dev;               /* device number */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        regAddr = 0x02800900;
        break;
    }

    if(regAddr != 0xFFFFFFFF)
    {

        for(i = 0; i < 10; i++)
            for(cntSet = 0; cntSet < 4; cntSet++)
            {
                regAddr = 0x02800900 + i*4 + cntSet * 0x100;

                prvUtfPerPortGroupCounterPerRegGet(regAddr, 0, 32,
                                                   prvUtfPrivateCpssDxChIpCntGet,
                                                   prvUtfPrivateCpssDxChIpCntSet,
                                                   prvUtfPrivateCpssDxChIpPortGroupCntGet,
                                                   prvUtfPrivateCpssDxChIpPortGroupCntSet);
            }
    }

}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpPortGroupDropCntGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *dropPktsPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpPortGroupDropCntSet)
{
    GT_U32 regAddr = 0xFFFFFFFF; /* register address not initialized */
    GT_U8     dev;               /* device number */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        regAddr = 0x02800950;
        break;
    }

    if(regAddr != 0xFFFFFFFF)
    {
        prvUtfPerPortGroupCounterGet(regAddr, 0, 32,
              cpssDxChIpDropCntGet,
              cpssDxChIpDropCntSet,
              cpssDxChIpPortGroupDropCntGet,
              cpssDxChIpPortGroupDropCntSet);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet
(
    IN   GT_U8                  devNum,
    IN   GT_PORT_GROUPS_BMP     portGroupsBmp,
    OUT  GT_U32                 *dropPktsPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet)
{
    GT_U32 regAddr = 0xFFFFFFFF; /* register address not initialized */
    GT_U8     dev;               /* device number */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if(PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            regAddr = 0x0d800984;
        }
        else
        {
            regAddr = 0x0C800984;
        }
        break;
    }

    if(regAddr != 0xFFFFFFFF)
        prvUtfPerPortGroupCounterGet(regAddr, 0, 32,
                                     cpssDxChIpMultiTargetQueueFullDropCntGet,
                                     cpssDxChIpMultiTargetQueueFullDropCntSet,
                                     cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet,
                                     cpssDxChIpPortGroupMultiTargetQueueFullDropCntSet);

}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpPortGroupMllCntGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  mllCntSet,
    OUT GT_U32                  *mllOutMCPktsPtr
)
*/
static GT_STATUS prvUtfCpssDxChIpPortGroupMllCntSet0Get
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *mllOutMCPktsPtr
)
{
    return cpssDxChIpPortGroupMllCntGet(devNum,portGroupsBmp, 0, mllOutMCPktsPtr);

}
static GT_STATUS prvUtfCpssDxChIpPortGroupMllCntSet1Get
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *mllOutMCPktsPtr
)
{
    return cpssDxChIpPortGroupMllCntGet(devNum,portGroupsBmp, 1 ,mllOutMCPktsPtr);

}
static GT_STATUS prvUtfCpssDxChIpMllCntSet0Get
(
    IN  GT_U8                   devNum,
    OUT GT_U32                  *mllOutMCPktsPtr
)
{
    return cpssDxChIpMllCntGet(devNum, 0, mllOutMCPktsPtr);

}
static GT_STATUS prvUtfCpssDxChIpMllCntSet1Get
(
    IN  GT_U8                   devNum,
    OUT GT_U32                  *mllOutMCPktsPtr
)
{
    return cpssDxChIpMllCntGet(devNum, 1 ,mllOutMCPktsPtr);

}

static GT_STATUS prvUtfCpssDxChIpPortGroupMllCntSet0Set
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  mllOutMCPkts
)
{
    return cpssDxChIpPortGroupMllCntSet(devNum,portGroupsBmp, 0, mllOutMCPkts);

}
static GT_STATUS prvUtfCpssDxChIpPortGroupMllCntSet1Set
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  mllOutMCPkts
)
{
    return cpssDxChIpPortGroupMllCntSet(devNum,portGroupsBmp, 1 ,mllOutMCPkts);

}
static GT_STATUS prvUtfCpssDxChIpMllCntSet0Set
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  mllOutMCPkts
)
{
    return cpssDxChIpMllCntSet(devNum, 0, mllOutMCPkts);

}
static GT_STATUS prvUtfCpssDxChIpMllCntSet1Set
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  mllOutMCPkts
)
{
    return cpssDxChIpMllCntSet(devNum, 1 ,mllOutMCPkts);
}


UTF_TEST_CASE_MAC(cpssDxChIpPortGroupMllCntGet)
{
    GT_U32 regAddr = 0xFFFFFFFF; /* register address not initialized */
    GT_U8     dev;               /* device number */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if(PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            regAddr = 0x0d800900;
        }
        else
        {
            regAddr = 0x0C800900;
        }
        break;
    }

    if(regAddr != 0xFFFFFFFF)
    {
        prvUtfPerPortGroupCounterGet(regAddr, 0, 32,
                                     prvUtfCpssDxChIpMllCntSet0Get,
                                     prvUtfCpssDxChIpMllCntSet0Set,
                                     prvUtfCpssDxChIpPortGroupMllCntSet0Get,
                                     prvUtfCpssDxChIpPortGroupMllCntSet0Set);

        regAddr += 0x100;
        prvUtfPerPortGroupCounterGet(regAddr, 0, 32,
                                     prvUtfCpssDxChIpMllCntSet1Get,
                                     prvUtfCpssDxChIpMllCntSet1Set,
                                     prvUtfCpssDxChIpPortGroupMllCntSet1Get,
                                     prvUtfCpssDxChIpPortGroupMllCntSet1Set);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpPortGroupCntSet
(
    IN GT_U8                         devNum,
    IN GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN CPSS_IP_CNT_SET_ENT           cntSet,
    IN CPSS_DXCH_IP_COUNTER_SET_STC *counters
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpPortGroupCntSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxCh2 and above)
    1.1.1. Call with cntSet [CPSS_IP_CNT_SET0_E].
         and counters{inUcPkts [1],
            inMcPkts [1],
            inUcNonRoutedExcpPkts [0],
            inUcNonRoutedNonExcpPkts [0],
            inMcNonRoutedExcpPkts [0],
            inMcNonRoutedNonExcpPkts [0],
            inUcTrappedMirrorPkts [1],
            inMcTrappedMirrorPkts [1],
            mcRfpFailPkts [0],
            outUcRoutedPkts [1]}.
    Expected: GT_OK.
    1.1.5. Call cpssDxChIpPortGroupCntGet with the same cntSet.
    Expected: GT_OK and the same counters.
    1.1.2. Call with cntSet [CPSS_IP_CNT_SET1_E].
        and counters{inUcPkts [10],
            inMcPkts [10],
            inUcNonRoutedExcpPkts [100],
            inUcNonRoutedNonExcpPkts [100],
            inMcNonRoutedExcpPkts [1000],
            inMcNonRoutedNonExcpPkts [1000],
            inUcTrappedMirrorPkts [1100],
            inMcTrappedMirrorPkts [1100],
            mcRfpFailPkts [1000],
            outUcRoutedPkts [1001]}.
    Expected: GT_OK.
    1.1.5. Call cpssDxChIpPortGroupCntGet with the same cntSet.
    Expected: GT_OK and the same counters.
    1.1.3. Call with cntSet [CPSS_IP_CNT_SET2_E].
        and counters{inUcPkts [30],
            inMcPkts [30],
            inUcNonRoutedExcpPkts [300],
            inUcNonRoutedNonExcpPkts [300],
            inMcNonRoutedExcpPkts [3000],
            inMcNonRoutedNonExcpPkts [3000],
            inUcTrappedMirrorPkts [3300],
            inMcTrappedMirrorPkts [3300],
            mcRfpFailPkts [3000],
            outUcRoutedPkts [3003]}.
    Expected: GT_OK.
    1.1.5. Call cpssDxChIpPortGroupCntGet with the same cntSet.
    Expected: GT_OK and the same counters.
    1.1.4. Call with cntSet [CPSS_IP_CNT_SET3_E].
        and counters{inUcPkts [990],
            inMcPkts [990],
            inUcNonRoutedExcpPkts [9900],
            inUcNonRoutedNonExcpPkts [9900],
            inMcNonRoutedExcpPkts [99000],
            inMcNonRoutedNonExcpPkts [99000],
            inUcTrappedMirrorPkts [999900],
            inMcTrappedMirrorPkts [999900],
            mcRfpFailPkts [99000],
            outUcRoutedPkts [990099]}.
    Expected: GT_OK.
    1.1.5. Call cpssDxChIpPortGroupCntGet with the same cntSet.
    Expected: GT_OK and the same counters.
    1.1.6. Call with wrong enum values cntSet  and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.1.7. Call with counters [NULL] and other valid parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;

    CPSS_IP_CNT_SET_ENT           cntSet = CPSS_IP_CNT_SET0_E;
    CPSS_DXCH_IP_COUNTER_SET_STC  counters = {0};
    CPSS_DXCH_IP_COUNTER_SET_STC  countersRet = {0};

    GT_BOOL     isEqual;
    GT_U32      portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with cntSet [CPSS_IP_CNT_SET0_E].
                    and counters{inUcPkts [1],
                        inMcPkts [1],
                        inUcNonRoutedExcpPkts [0],
                        inUcNonRoutedNonExcpPkts [0],
                        inMcNonRoutedExcpPkts [0],
                        inMcNonRoutedNonExcpPkts [0],
                        inUcTrappedMirrorPkts [1],
                        inMcTrappedMirrorPkts [1],
                        mcRfpFailPkts [0],
                        outUcRoutedPkts [1]}.
                Expected: GT_OK.
            */
            cntSet = CPSS_IP_CNT_SET0_E;
            counters.inUcPkts  = 1;
            counters.inMcPkts  = 1;
            counters.inUcNonRoutedExcpPkts = 0;
            counters.inUcNonRoutedNonExcpPkts = 0;
            counters.inMcNonRoutedExcpPkts = 0;
            counters.inMcNonRoutedNonExcpPkts = 0;
            counters.inUcTrappedMirrorPkts = 1;
            counters.inMcTrappedMirrorPkts = 1;
            counters.mcRfpFailPkts  = 0;
            counters.outUcRoutedPkts = 1;

            st = cpssDxChIpPortGroupCntSet(dev, portGroupsBmp, cntSet, &counters);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntSet);

            /*
               1.1.5. Call cpssDxChIpPortGroupCntGet with the same cntSet.
               Expected: GT_OK and the same counters.
            */
            cpssOsBzero((GT_VOID*) &countersRet, sizeof(countersRet));

            st = cpssDxChIpPortGroupCntGet(dev, portGroupsBmp, cntSet, &countersRet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                        "cpssDxChIpPortGroupCntGet: %d, %d", dev, cntSet);

            isEqual =
                (0 == cpssOsMemCmp((GT_VOID*)&counters, (GT_VOID*)&countersRet, sizeof (counters)))
                      ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                      "get another counters than was set: %d, %d", dev, cntSet);

            /*
                1.1.2. Call with cntSet [CPSS_IP_CNT_SET1_E].
                    and counters{inUcPkts [10],
                        inMcPkts [10],
                        inUcNonRoutedExcpPkts [100],
                        inUcNonRoutedNonExcpPkts [100],
                        inMcNonRoutedExcpPkts [1000],
                        inMcNonRoutedNonExcpPkts [1000],
                        inUcTrappedMirrorPkts [1100],
                        inMcTrappedMirrorPkts [1100],
                        mcRfpFailPkts [1000],
                        outUcRoutedPkts [1001]}.
                Expected: GT_OK.
            */
            cntSet = CPSS_IP_CNT_SET1_E;

            counters.inUcPkts  = 10;
            counters.inMcPkts  = 10;
            counters.inUcNonRoutedExcpPkts = 100;
            counters.inUcNonRoutedNonExcpPkts = 100;
            counters.inMcNonRoutedExcpPkts = 1000;
            counters.inMcNonRoutedNonExcpPkts = 1000;
            counters.inUcTrappedMirrorPkts = 1100;
            counters.inMcTrappedMirrorPkts = 1100;
            counters.mcRfpFailPkts  = 1000;
            counters.outUcRoutedPkts = 1001;

            st = cpssDxChIpPortGroupCntSet(dev, portGroupsBmp, cntSet, &counters);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntSet);

            /*
               1.1.5. Call cpssDxChIpPortGroupCntGet with the same cntSet.
               Expected: GT_OK and the same counters.
            */
            cpssOsBzero((GT_VOID*) &countersRet, sizeof(countersRet));

            st = cpssDxChIpPortGroupCntGet(dev, portGroupsBmp, cntSet, &countersRet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                        "cpssDxChIpPortGroupCntGet: %d, %d", dev, cntSet);

            isEqual =
                (0 == cpssOsMemCmp((GT_VOID*)&counters, (GT_VOID*)&countersRet, sizeof (counters)))
                      ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                      "get another counters than was set: %d, %d", dev, cntSet);

            /*
                1.1.3. Call with cntSet [CPSS_IP_CNT_SET2_E].
                    and counters{inUcPkts [30],
                        inMcPkts [30],
                        inUcNonRoutedExcpPkts [300],
                        inUcNonRoutedNonExcpPkts [300],
                        inMcNonRoutedExcpPkts [3000],
                        inMcNonRoutedNonExcpPkts [3000],
                        inUcTrappedMirrorPkts [3300],
                        inMcTrappedMirrorPkts [3300],
                        mcRfpFailPkts [3000],
                        outUcRoutedPkts [3003]}.
                Expected: GT_OK.
            */
            cntSet = CPSS_IP_CNT_SET2_E;
            counters.inUcPkts  = 30;
            counters.inMcPkts  = 30;
            counters.inUcNonRoutedExcpPkts = 300;
            counters.inUcNonRoutedNonExcpPkts = 300;
            counters.inMcNonRoutedExcpPkts = 3000;
            counters.inMcNonRoutedNonExcpPkts = 3000;
            counters.inUcTrappedMirrorPkts = 3300;
            counters.inMcTrappedMirrorPkts = 3300;
            counters.mcRfpFailPkts  = 3000;
            counters.outUcRoutedPkts = 3003;

            st = cpssDxChIpPortGroupCntSet(dev, portGroupsBmp, cntSet, &counters);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntSet);

            /*
               1.1.5. Call cpssDxChIpPortGroupCntGet with the same cntSet.
               Expected: GT_OK and the same counters.
            */
            cpssOsBzero((GT_VOID*) &countersRet, sizeof(countersRet));

            st = cpssDxChIpPortGroupCntGet(dev, portGroupsBmp, cntSet, &countersRet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                        "cpssDxChIpPortGroupCntGet: %d, %d", dev, cntSet);

            isEqual =
                (0 == cpssOsMemCmp((GT_VOID*)&counters, (GT_VOID*)&countersRet, sizeof (counters)))
                      ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                      "get another counters than was set: %d, %d", dev, cntSet);

            /*
                1.1.4. Call with cntSet [CPSS_IP_CNT_SET3_E].
                    and counters{inUcPkts [990],
                        inMcPkts [990],
                        inUcNonRoutedExcpPkts [9900],
                        inUcNonRoutedNonExcpPkts [9900],
                        inMcNonRoutedExcpPkts [99000],
                        inMcNonRoutedNonExcpPkts [99000],
                        inUcTrappedMirrorPkts [999900],
                        inMcTrappedMirrorPkts [999900],
                        mcRfpFailPkts [99000],
                        outUcRoutedPkts [990099]}.
                Expected: GT_OK.
            */
            cntSet = CPSS_IP_CNT_SET3_E;
            counters.inUcPkts  = 990;
            counters.inMcPkts  = 990;
            counters.inUcNonRoutedExcpPkts = 9900;
            counters.inUcNonRoutedNonExcpPkts = 9900;
            counters.inMcNonRoutedExcpPkts = 99000;
            counters.inMcNonRoutedNonExcpPkts = 99000;
            counters.inUcTrappedMirrorPkts = 999900;
            counters.inMcTrappedMirrorPkts = 999900;
            counters.mcRfpFailPkts  = 99000;
            counters.outUcRoutedPkts = 990099;

            st = cpssDxChIpPortGroupCntSet(dev, portGroupsBmp, cntSet, &counters);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntSet);

            /*
               1.1.5. Call cpssDxChIpPortGroupCntGet with the same cntSet.
               Expected: GT_OK and the same counters.
            */
            cpssOsBzero((GT_VOID*) &countersRet, sizeof(countersRet));

            st = cpssDxChIpPortGroupCntGet(dev, portGroupsBmp, cntSet, &countersRet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                        "cpssDxChIpPortGroupCntGet: %d, %d", dev, cntSet);

            isEqual =
                (0 == cpssOsMemCmp((GT_VOID*)&counters, (GT_VOID*)&countersRet, sizeof (counters)))
                      ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                      "get another counters than was set: %d, %d", dev, cntSet);

            /*
                1.1.6. Call with wrong enum values cntSet  and other valid parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChIpPortGroupCntSet
                                (dev, portGroupsBmp, cntSet, &counters),
                                cntSet);

            /*
                1.1.7. Call with counters [NULL] and other valid parameters from 1.1.
                Expected: GT_BAD_PTR.
            */

            st = cpssDxChIpPortGroupCntSet(dev, portGroupsBmp, cntSet, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, counters = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChIpPortGroupCntSet(dev, portGroupsBmp, cntSet, &counters);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChIpPortGroupCntSet(dev, portGroupsBmp, cntSet, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    cntSet = CPSS_IP_CNT_SET3_E;
    counters.inUcPkts  = 1;
    counters.inMcPkts  = 1;
    counters.inUcNonRoutedExcpPkts = 0;
    counters.inUcNonRoutedNonExcpPkts = 0;
    counters.inMcNonRoutedExcpPkts = 0;
    counters.inMcNonRoutedNonExcpPkts = 0;
    counters.inUcTrappedMirrorPkts = 1;
    counters.inMcTrappedMirrorPkts = 1;
    counters.mcRfpFailPkts  = 0;
    counters.outUcRoutedPkts = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpPortGroupCntSet(dev, portGroupsBmp, cntSet, &counters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpPortGroupCntSet(dev, portGroupsBmp, cntSet, &counters);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpPortGroupDropCntGet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_GROUPS_BMP   portGroupsBmp,
    OUT GT_U32              *dropPktsPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpPortGroupDropCntGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxCh2 and above)
    1.1.1. Call with non-null dropPktsPtr.
    Expected: GT_OK.
    1.1.2. Call with dropPktsPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;

    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  dropPkts;
    GT_U32                  portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with non-null dropPktsPtr.
                Expected: GT_OK.
            */
            st = cpssDxChIpPortGroupDropCntGet(dev, portGroupsBmp, &dropPkts);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call with dropPktsPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChIpPortGroupDropCntGet(dev, portGroupsBmp, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dropPktsPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChIpPortGroupDropCntGet(dev, portGroupsBmp, &dropPkts);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChIpPortGroupDropCntGet(dev, portGroupsBmp, &dropPkts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpPortGroupDropCntGet(dev, portGroupsBmp, &dropPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpPortGroupDropCntGet(dev, portGroupsBmp, &dropPkts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpPortGroupMllCntSet
(
    IN GT_U8                    devNum,
    IN GT_PORT_GROUPS_BMP       portGroupsBmp,
    IN GT_U32                   mllCntSet,
    IN GT_U32                   mllOutMCPkts
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpPortGroupMllCntSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxCh2 and above)
    1.1.1. Call with mllCntSet [0 / 1]and mllOutMCPkts [0 / 10].
    Expected: GT_OK.
    1.1.2. Call cpssDxChIpPortGroupMllCntGet with the same mllCntSet.
    Expected: GT_OK and the same mllOutMCPkts.
    1.1.3. Check out of range mllCntSet.  Call with mllCntSet [10].
    Expected: NOT GT_OK.
    1.1.4. Call with mllCntSet [1] and mllOutMCPkts [0xFFFFFFFF] (no any constraints).
    Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId;

    GT_U32   mllCntSet    = 0;
    GT_U32   mllOutMCPkts = 0;
    GT_U32   mllOutMCPktsRet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with mllCntSet [0 / 1]and mllOutMCPkts [0 / 10].
                Expected: GT_OK.
            */
            /*call with mllCntSet = 0 and  mllOutMCPkts = 0; */
            mllCntSet = 0;
            mllOutMCPkts = 0;

            st = cpssDxChIpPortGroupMllCntSet(dev, portGroupsBmp, mllCntSet, mllOutMCPkts);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mllCntSet, mllOutMCPkts);

            /*
                1.1.2. Call cpssDxChIpPortGroupMllCntGet with the same mllCntSet.
                Expected: GT_OK and the same mllOutMCPkts.
            */
            st = cpssDxChIpPortGroupMllCntGet(dev, portGroupsBmp, mllCntSet, &mllOutMCPktsRet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChIpPortGroupMllCntGet: %d, %d",
                                         dev, mllCntSet);

            UTF_VERIFY_EQUAL2_STRING_MAC(mllOutMCPkts, mllOutMCPktsRet,
                "get another mllOutMCPkts than was set: %d, %d", dev, mllCntSet);

            /*call with mllCntSet = 1 and  mllOutMCPkts = 10; */
            mllCntSet = 1;
            mllOutMCPkts = 10;

            st = cpssDxChIpPortGroupMllCntSet(dev, portGroupsBmp, mllCntSet, mllOutMCPkts);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mllCntSet, mllOutMCPkts);

            /*
                1.1.2. Call cpssDxChIpPortGroupMllCntGet with the same mllCntSet.
                Expected: GT_OK and the same mllOutMCPkts.
            */
            st = cpssDxChIpPortGroupMllCntGet(dev, portGroupsBmp, mllCntSet, &mllOutMCPktsRet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChIpPortGroupMllCntGet: %d, %d",
                                         dev, mllCntSet);

            UTF_VERIFY_EQUAL2_STRING_MAC(mllOutMCPkts, mllOutMCPktsRet,
                "get another mllOutMCPkts than was set: %d, %d", dev, mllCntSet);

            /*
                1.1.3. Check out of range mllCntSet.  Call with mllCntSet [10].
                Expected: NOT GT_OK.
            */
            mllCntSet = 10;
            mllOutMCPkts = 10;

            st = cpssDxChIpPortGroupMllCntSet(dev, portGroupsBmp, mllCntSet, mllOutMCPkts);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, mllCntSet);

            /*
                1.1.4. Call with mllCntSet [1] and mllOutMCPkts [0xFFFFFFFF] (no any constraints).
                Expected: GT_OK.
            */
            mllCntSet = 1;
            mllOutMCPkts = 0xFFFFFFFF;

            st = cpssDxChIpPortGroupMllCntSet(dev, portGroupsBmp, mllCntSet, mllOutMCPkts);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mllCntSet, mllOutMCPkts);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChIpPortGroupMllCntSet(dev, portGroupsBmp, mllCntSet, mllOutMCPkts);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChIpPortGroupMllCntSet(dev, portGroupsBmp, mllCntSet, mllOutMCPkts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;
    mllCntSet = 1;
    mllOutMCPkts = 10;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpPortGroupMllCntSet(dev, portGroupsBmp, mllCntSet, mllOutMCPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpPortGroupMllCntSet(dev, portGroupsBmp, mllCntSet, mllOutMCPkts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpPortGroupMultiTargetQueueFullDropCntSet
(
    IN GT_U8               devNum,
    IN GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN GT_U32              dropPkts
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpPortGroupMultiTargetQueueFullDropCntSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxCh2 and above)
    1.1.1. Call with dropPkts [10 / 1000 /1000000].
    Expected: GT_OK.
    1.1.2. Call cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet.
    Expected: GT_OK and the same dropPkts.
    1.1.3. Call with dropPkts [0xFFFFFFFF] (no any onstraints).
    Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  dropPkts = 0;
    GT_U32                  dropPktsRet;
    GT_U32                  portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
               1.1.1. Call with dropPkts [10 / 1000 /1000000].
               Expected: GT_OK.
            */
            /*call with dropPkts = 10*/
            dropPkts = 10;
            st = cpssDxChIpPortGroupMultiTargetQueueFullDropCntSet(dev,
                                                    portGroupsBmp, dropPkts);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropPkts);

            /*
               1.1.2. Call cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet.
               Expected: GT_OK and the same dropPkts.
            */
            st = cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet(dev,
                                                    portGroupsBmp, &dropPktsRet);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(dropPkts, dropPktsRet,
                "cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet: get another value than was set: dev = %d, dropPkts = %d",
                                         dev, dropPktsRet);

            /*call with dropPkts = 1000*/
            dropPkts = 1000;
            st = cpssDxChIpPortGroupMultiTargetQueueFullDropCntSet(dev,
                                                    portGroupsBmp, dropPkts);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropPkts);

            /*
               1.1.2. Call cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet.
               Expected: GT_OK and the same dropPkts.
            */
            st = cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet(dev,
                                                    portGroupsBmp, &dropPktsRet);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(dropPkts, dropPktsRet,
                "cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet: get another value than was set: dev = %d, dropPkts = %d",
                                         dev, dropPktsRet);

            /*call with dropPkts = 1000000*/
            dropPkts = 1000000;
            st = cpssDxChIpPortGroupMultiTargetQueueFullDropCntSet(dev,
                                                    portGroupsBmp, dropPkts);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropPkts);

            /*
               1.1.2. Call cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet.
               Expected: GT_OK and the same dropPkts.
            */
            st = cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet(dev,
                                                    portGroupsBmp, &dropPktsRet);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(dropPkts, dropPktsRet,
                "cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet: get another value than was set: dev = %d, dropPkts = %d",
                                         dev, dropPktsRet);

            /*
               1.1.3. Call with dropPkts [0xFFFFFFFF] (no any onstraints).
               Expected: GT_OK.
            */
            dropPkts = 0xFFFFFFFF;

            st = cpssDxChIpPortGroupMultiTargetQueueFullDropCntSet(dev,
                                                portGroupsBmp, dropPkts);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropPkts);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChIpPortGroupMultiTargetQueueFullDropCntSet(dev,
                                                portGroupsBmp, dropPkts);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChIpPortGroupMultiTargetQueueFullDropCntSet(dev, portGroupsBmp, dropPkts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;
    dropPkts = 10;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpPortGroupMultiTargetQueueFullDropCntSet(dev,
                                                    portGroupsBmp, dropPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpPortGroupMultiTargetQueueFullDropCntSet(dev,
                                                    portGroupsBmp, dropPkts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpUcRpfVlanModeSet
(
    IN  GT_U8                        devNum,
    IN  GT_U16                       vid,
    IN  CPSS_DXCH_IP_VLAN_URPF_MODE_ENT   uRpfMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpUcRpfVlanModeSet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with vid [0 / 100 / 0xFFF],
                   uRpfMode [CPSS_DXCH_IP_VLAN_URPF_DISABLE_MODE_E /
                             CPSS_DXCH_IP_VLAN_URPF_VLAN_MODE_E /
                             CPSS_DXCH_IP_VLAN_URPF_PORT_TRUNK_MODE_E].
    Expected: GT_OK.
    1.2. Call cpssDxChIpUcRpfVlanModeGet with the same vid.
    Expected: GT_OK and the same uRpfMode.
    1.3. Call with out of range vid[PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with wrong enum values uRpfMode and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U16                            vid;
    CPSS_DXCH_IP_VLAN_URPF_MODE_ENT   uRpfMode;
    CPSS_DXCH_IP_VLAN_URPF_MODE_ENT   uRpfModeGet;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vid [0 / 100 / 0xFFF],
                           uRpfMode [CPSS_DXCH_IP_VLAN_URPF_DISABLE_MODE_E /
                                     CPSS_DXCH_IP_VLAN_URPF_VLAN_MODE_E /
                                     CPSS_DXCH_IP_VLAN_URPF_PORT_TRUNK_MODE_E].
            Expected: GT_OK.
        */
        vid = 0;
        uRpfMode = CPSS_DXCH_IP_VLAN_URPF_DISABLE_MODE_E;

        st = cpssDxChIpUcRpfVlanModeSet(dev, vid, uRpfMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vid, uRpfMode);

        /*
            1.2. Call cpssDxChIpUcRpfVlanModeGet with the same vid.
            Expected: GT_OK and the same uRpfMode.
        */
        st = cpssDxChIpUcRpfVlanModeGet(dev, vid, &uRpfModeGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vid);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(uRpfMode, uRpfModeGet,
                                     "cpssDxChIpUcRpfVlanModeGet: get another value than was set: %d, %d",
                                     dev, uRpfModeGet);

        /*
            1.1. Call with vid [0 / 100 / 0xFFF],
                           uRpfMode [CPSS_DXCH_IP_VLAN_URPF_DISABLE_MODE_E /
                                     CPSS_DXCH_IP_VLAN_URPF_VLAN_MODE_E /
                                     CPSS_DXCH_IP_VLAN_URPF_PORT_TRUNK_MODE_E].
            Expected: GT_OK.
        */
        vid = 100;
        uRpfMode = CPSS_DXCH_IP_VLAN_URPF_VLAN_MODE_E;

        st = cpssDxChIpUcRpfVlanModeSet(dev, vid, uRpfMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vid, uRpfMode);

        /*
            1.2. Call cpssDxChIpUcRpfVlanModeGet with the same vid.
            Expected: GT_OK and the same uRpfMode.
        */
        st = cpssDxChIpUcRpfVlanModeGet(dev, vid, &uRpfModeGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vid);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(uRpfMode, uRpfModeGet,
                                     "cpssDxChIpUcRpfVlanModeGet: get another value than was set: %d, %d",
                                     dev, uRpfModeGet);

        /*
            1.1. Call with vid [0 / 100 / 0xFFF],
                           uRpfMode [CPSS_DXCH_IP_VLAN_URPF_DISABLE_MODE_E /
                                     CPSS_DXCH_IP_VLAN_URPF_VLAN_MODE_E /
                                     CPSS_DXCH_IP_VLAN_URPF_PORT_TRUNK_MODE_E].
            Expected: GT_OK.
        */
        vid = 0xFFF;
        uRpfMode = CPSS_DXCH_IP_VLAN_URPF_PORT_TRUNK_MODE_E;

        st = cpssDxChIpUcRpfVlanModeSet(dev, vid, uRpfMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vid, uRpfMode);

        /*
            1.2. Call cpssDxChIpUcRpfVlanModeGet with the same vid.
            Expected: GT_OK and the same uRpfMode.
        */
        st = cpssDxChIpUcRpfVlanModeGet(dev, vid, &uRpfModeGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vid);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(uRpfMode, uRpfModeGet,
                                     "cpssDxChIpUcRpfVlanModeGet: get another value than was set: %d, %d",
                                     dev, uRpfModeGet);

        /*
            1.3. Call with out of range vid[PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and other valid parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChIpUcRpfVlanModeSet(dev, vid, uRpfMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vid);

        vid = 100;

        /*
            1.4. Call with wrong enum values uRpfMode and other valid parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpUcRpfVlanModeSet
                            (dev, vid, uRpfMode),
                            uRpfMode);
    }

    vid = 0;
    uRpfMode = CPSS_DXCH_IP_VLAN_URPF_DISABLE_MODE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E | UTF_LION_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpUcRpfVlanModeSet(dev, vid, uRpfMode);
        if(PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_SUPPORTED, st, dev);
        }
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpUcRpfVlanModeSet(dev, vid, uRpfMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpUcRpfVlanModeGet
(
    IN  GT_U8                               devNum,
    IN  GT_U16                              vid,
    IN  CPSS_DXCH_IP_VLAN_URPF_MODE_ENT     *uRpfModePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpUcRpfVlanModeGet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with vid [0 / 100 / 0xFFF],
                   non NULL uRpfModePtr.
    Expected: GT_OK.
    1.2. Call with out of range vid[PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with NULL uRpfModePtr and other valid parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U16                            vid;
    CPSS_DXCH_IP_VLAN_URPF_MODE_ENT   uRpfMode;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vid [0 / 100 / 0xFFF],
                           non NULL uRpfModePtr.
            Expected: GT_OK.
        */

        /* call with vid = 0 */
        vid = 0;

        st = cpssDxChIpUcRpfVlanModeGet(dev, vid, &uRpfMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vid);

        /* call with vid = 100 */
        vid = 100;

        st = cpssDxChIpUcRpfVlanModeGet(dev, vid, &uRpfMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vid);

        /* call with vid = 0xFFF */
        vid = 0xFFF;

        st = cpssDxChIpUcRpfVlanModeGet(dev, vid, &uRpfMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vid);

        /*
            1.2. Call with out of range vid[PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and other valid parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChIpUcRpfVlanModeGet(dev, vid, &uRpfMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vid);

        vid = 100;

        /*
            1.3. Call with NULL uRpfModePtr and other valid parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpUcRpfVlanModeGet(dev, vid, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, uRpfModePtr = NULL", dev);
    }

    vid = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E | UTF_LION_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpUcRpfVlanModeGet(dev, vid, &uRpfMode);
        if(PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_SUPPORTED, st, dev);
        }
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpUcRpfVlanModeGet(dev, vid, &uRpfMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpPortSipSaEnableSet
(
    IN  GT_U8                        devNum,
    IN  GT_U8                        portNum,
    IN  GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpPortSipSaEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (Lion and above)
    1.1.1. Call with enable [GT_TRUE/ GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChIpPortSipSaEnableGet with  non-NULL enablePtr.
    Expected: GT_OK  and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = IP_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_TRUE/ GT_FALSE].
                Expected: GT_OK.
            */

            /* call with enable = GT_TRUE */
            enable = GT_TRUE;

            st = cpssDxChIpPortSipSaEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssDxChIpPortSipSaEnableGet with  non-NULL enablePtr.
                Expected: GT_OK  and the same enable.
            */
            st = cpssDxChIpPortSipSaEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                "cpssDxChIpPortSipSaEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                "get another enable value than was set: %d, %d", dev, port);

            /* call with enable = GT_FALSE */
            enable = GT_FALSE;

            st = cpssDxChIpPortSipSaEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssDxChIpPortSipSaEnableGet with  non-NULL enablePtr.
                Expected: GT_OK  and the same enable.
            */
            st = cpssDxChIpPortSipSaEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                "cpssDxChIpPortSipSaEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                "get another enable value than was set: %d, %d", dev, port);
        }

        enable = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChIpPortSipSaEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChIpPortSipSaEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChIpPortSipSaEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;
    port   = IP_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpPortSipSaEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpPortSipSaEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpPortSipSaEnableGet
(
    IN  GT_U8                        devNum,
    IN  GT_U8                        portNum,
    IN  GT_BOOL                      *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpPortSipSaEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (Lion and above)
    1.1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = IP_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssDxChIpPortSipSaEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChIpPortSipSaEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChIpPortSipSaEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChIpPortSipSaEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChIpPortSipSaEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    port = IP_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpPortSipSaEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpPortSipSaEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpPortGroupMultiTargetRateShaperSet
(
    IN  GT_U8              devNum,
    IN  GT_PORT_GROUPS_BMP portGroupsBmp,
    IN  GT_BOOL            multiTargetRateShaperEnable,
    IN  GT_U32             windowSize
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpPortGroupMultiTargetRateShaperSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxCh2 and above)
    1.1.1. Call with multiTargetRateShaperEnable [GT_TRUE and GT_FALSE]
                     and windowSize [10 / 0xFFFF].
    Expected: GT_OK.
    1.1.2. Call with multiTargetRateShaperEnable [GT_FALSE]
                     and windowSize [0xFFFF + 1] (not relevant).
    Expected: GT_OK.
    1.1.3. Call with multiTargetRateShaperEnable [GT_TRUE]
                     and out-of-range windowSize [0xFFFF + 1].
    Expected: NOT GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId;

    GT_BOOL                 multiTargetRateShaperEnable = GT_FALSE;
    GT_U32                  windowSize = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with multiTargetRateShaperEnable [GT_TRUE and GT_FALSE]
                                 and windowSize [10 / 0xFFFF].
                Expected: GT_OK.
            */
            multiTargetRateShaperEnable = GT_TRUE;

            /* call with windowSize = 10 */
            windowSize = 10;

            st = cpssDxChIpPortGroupMultiTargetRateShaperSet(dev, portGroupsBmp,
                                    multiTargetRateShaperEnable, windowSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev,  multiTargetRateShaperEnable,
                                        windowSize);

            /* call with windowSize = 0xFFFF */
            windowSize = 0xFFFF;

            st = cpssDxChIpPortGroupMultiTargetRateShaperSet(dev, portGroupsBmp,
                                    multiTargetRateShaperEnable, windowSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev,  multiTargetRateShaperEnable,
                                        windowSize);

            /*
                1.1.1. Call with multiTargetRateShaperEnable [GT_TRUE and GT_FALSE]
                                 and windowSize [10 / 0xFFFF].
                Expected: GT_OK.
            */
            multiTargetRateShaperEnable = GT_FALSE;

            /* call with windowSize = 10 */
            windowSize = 10;

            st = cpssDxChIpPortGroupMultiTargetRateShaperSet(dev, portGroupsBmp,
                                    multiTargetRateShaperEnable, windowSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev,  multiTargetRateShaperEnable,
                                        windowSize);

            /* call with windowSize = 0xFFFF */
            windowSize = 0xFFFF;

            st = cpssDxChIpPortGroupMultiTargetRateShaperSet(dev, portGroupsBmp,
                                    multiTargetRateShaperEnable, windowSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev,  multiTargetRateShaperEnable,
                                        windowSize);

            /*
                1.1.2. Call with multiTargetRateShaperEnable [GT_FALSE]
                                 and windowSize [0xFFFF + 1] (not relevant).
                Expected: GT_OK.
            */
            multiTargetRateShaperEnable = GT_FALSE;
            windowSize = 0xFFFF + 1;

            st = cpssDxChIpPortGroupMultiTargetRateShaperSet(dev, portGroupsBmp,
                                    multiTargetRateShaperEnable, windowSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev,  multiTargetRateShaperEnable,
                                        windowSize);

            windowSize = 10;

            /*
                1.1.3. Call with multiTargetRateShaperEnable [GT_TRUE]
                                 and out-of-range windowSize [0xFFFF + 1].
                Expected: NOT GT_OK.
            */
            multiTargetRateShaperEnable = GT_TRUE;
            windowSize = 0xFFFF + 1;

            st = cpssDxChIpPortGroupMultiTargetRateShaperSet(dev, portGroupsBmp,
                                    multiTargetRateShaperEnable, windowSize);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev,  multiTargetRateShaperEnable,
                                            windowSize);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        /* set valid input parameters */
        multiTargetRateShaperEnable = GT_TRUE;
        windowSize = 10;

        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChIpPortGroupMultiTargetRateShaperSet(dev, portGroupsBmp,
                                    multiTargetRateShaperEnable, windowSize);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChIpPortGroupMultiTargetRateShaperSet(dev, portGroupsBmp,
                                multiTargetRateShaperEnable, windowSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    multiTargetRateShaperEnable = GT_TRUE;
    windowSize = 10;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpPortGroupMultiTargetRateShaperSet(dev, portGroupsBmp,
                                multiTargetRateShaperEnable, windowSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpPortGroupMultiTargetRateShaperSet(dev, portGroupsBmp,
                            multiTargetRateShaperEnable, windowSize);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet
(
    IN  GT_U8               dev,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    OUT GT_U32              *routerBridgedExceptionPktsPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(DxCh2 and above)
    1.1.1. Call with not nullportGroupsBmp.
    Expected: GT_OK.
    1.1.2. Call api with wrong routerBridgedExceptionPktsPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;
    GT_U32    portGroupId;

    GT_U8               dev;
    GT_PORT_GROUPS_BMP  portGroupsBmp = 1;
    GT_U32              routerBridgedExceptionPkts;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with not nullportGroupsBmp.
                Expected: GT_OK.
            */
            st = cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet(dev,
                                     portGroupsBmp, &routerBridgedExceptionPkts);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call api with wrong routerBridgedExceptionPktsPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet(dev,
                                     portGroupsBmp, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                   "%d, routerBridgedExceptionPktsPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet(dev,
                                     portGroupsBmp, &routerBridgedExceptionPkts);
            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet(dev,
                                     portGroupsBmp, &routerBridgedExceptionPkts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet(dev,
                                     portGroupsBmp, &routerBridgedExceptionPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet(dev,
                                     portGroupsBmp, &routerBridgedExceptionPkts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntSet
(
    IN  GT_U8              dev,
    IN  GT_PORT_GROUPS_BMP portGroupsBmp,
    IN  GT_U32             routerBridgedExceptionPkts
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(DxCh2 and above)
    1.1.1. Call with routerBridgedExceptionPkts[0 / 100 / 555],
    Expected: GT_OK.
    1.1.2. Call cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
*/
    GT_STATUS st = GT_OK;
    GT_U32    portGroupId;

    GT_U8              dev;
    GT_PORT_GROUPS_BMP portGroupsBmp = 1;
    GT_U32             routerBridgedExceptionPkts = 0;
    GT_U32             routerBridgedExceptionPktsGet = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with routerBridgedExceptionPkts[0 / 100 / 555],
                Expected: GT_OK.
            */

            /* call with routerBridgedExceptionPkts[0] */
            routerBridgedExceptionPkts = 0;

            st = cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntSet(dev,
                                     portGroupsBmp, routerBridgedExceptionPkts);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet(dev,
                                     portGroupsBmp, &routerBridgedExceptionPktsGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(routerBridgedExceptionPkts,
                                         routerBridgedExceptionPktsGet,
                           "got another routerBridgedExceptionPkts then was set: %d", dev);

            /* call with routerBridgedExceptionPkts[100] */
            routerBridgedExceptionPkts = 100;

            st = cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntSet(dev,
                                     portGroupsBmp, routerBridgedExceptionPkts);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call get func again, 1.1.2. */
            st = cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet(dev,
                                     portGroupsBmp, &routerBridgedExceptionPktsGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(routerBridgedExceptionPkts,
                                         routerBridgedExceptionPktsGet,
                           "got another routerBridgedExceptionPkts then was set: %d", dev);

            /* call with routerBridgedExceptionPkts[555] */
            routerBridgedExceptionPkts = 555;

            st = cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntSet(dev,
                                     portGroupsBmp, routerBridgedExceptionPkts);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call get func again, 1.1.2. */
            st = cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet(dev,
                                     portGroupsBmp, &routerBridgedExceptionPktsGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(routerBridgedExceptionPkts,
                                         routerBridgedExceptionPktsGet,
                           "got another routerBridgedExceptionPkts then was set: %d", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntSet(dev,
                                     portGroupsBmp, routerBridgedExceptionPkts);
            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntSet(dev,
                                     portGroupsBmp, routerBridgedExceptionPkts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    /* restore correct values */
    routerBridgedExceptionPkts = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntSet(dev,
                                     portGroupsBmp, routerBridgedExceptionPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntSet(dev,
                                     portGroupsBmp, routerBridgedExceptionPkts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpRouterBridgedPacketsExceptionCntGet
(
    IN  GT_U8   dev,
    OUT GT_U32  *routerBridgedExceptionPktsPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpRouterBridgedPacketsExceptionCntGet)
{
/*
    ITERATE_DEVICES(DxCh2 and above)
    1.1. Call with not null routerBridgedExceptionPktsPtr.
    Expected: GT_OK.
    1.2. Call api with wrong routerBridgedExceptionPktsPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U32  routerBridgedExceptionPkts;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not null routerBridgedExceptionPktsPtr.
            Expected: GT_OK.
        */
        st = cpssDxChIpRouterBridgedPacketsExceptionCntGet(dev, &routerBridgedExceptionPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call api with wrong routerBridgedExceptionPktsPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpRouterBridgedPacketsExceptionCntGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                   "%d, routerBridgedExceptionPktsPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpRouterBridgedPacketsExceptionCntGet(dev, &routerBridgedExceptionPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpRouterBridgedPacketsExceptionCntGet(dev, &routerBridgedExceptionPkts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpRouterBridgedPacketsExceptionCntSet
(
    IN  GT_U8  dev,
    IN  GT_U32 routerBridgedExceptionPkts
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpRouterBridgedPacketsExceptionCntSet)
{
/*
    ITERATE_DEVICES(DxCh2 and above)
    1.1. Call with routerBridgedExceptionPkts[0 / 100 / 555],
    Expected: GT_OK.
    1.2. Call cpssDxChIpRouterBridgedPacketsExceptionCntGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
*/
    GT_STATUS st = GT_OK;

    GT_U8  dev;
    GT_U32 routerBridgedExceptionPkts = 0;
    GT_U32 routerBridgedExceptionPktsGet = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with routerBridgedExceptionPkts[0 / 100 / 555],
            Expected: GT_OK.
        */

        /* call with routerBridgedExceptionPkts[0] */
        routerBridgedExceptionPkts = 0;

        st = cpssDxChIpRouterBridgedPacketsExceptionCntSet(dev, routerBridgedExceptionPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChIpRouterBridgedPacketsExceptionCntGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChIpRouterBridgedPacketsExceptionCntGet(dev,
                                &routerBridgedExceptionPktsGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChIpRouterBridgedPacketsExceptionCntGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(routerBridgedExceptionPkts,
                                     routerBridgedExceptionPktsGet,
                       "got another routerBridgedExceptionPkts then was set: %d", dev);

        /* call with routerBridgedExceptionPkts[100] */
        routerBridgedExceptionPkts = 100;

        st = cpssDxChIpRouterBridgedPacketsExceptionCntSet(dev, routerBridgedExceptionPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call get func again, 1.2. */
        st = cpssDxChIpRouterBridgedPacketsExceptionCntGet(dev,
                                &routerBridgedExceptionPktsGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChIpRouterBridgedPacketsExceptionCntGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(routerBridgedExceptionPkts,
                                     routerBridgedExceptionPktsGet,
                       "got another routerBridgedExceptionPkts then was set: %d", dev);

        /* call with routerBridgedExceptionPkts[555] */
        routerBridgedExceptionPkts = 555;

        st = cpssDxChIpRouterBridgedPacketsExceptionCntSet(dev, routerBridgedExceptionPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call get func again, 1.2. */
        st = cpssDxChIpRouterBridgedPacketsExceptionCntGet(dev,
                                &routerBridgedExceptionPktsGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChIpRouterBridgedPacketsExceptionCntGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(routerBridgedExceptionPkts,
                                  routerBridgedExceptionPktsGet,
                       "got another routerBridgedExceptionPkts then was set: %d", dev);
    }

    /* restore correct values */
    routerBridgedExceptionPkts = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpRouterBridgedPacketsExceptionCntSet(dev, routerBridgedExceptionPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpRouterBridgedPacketsExceptionCntSet(dev, routerBridgedExceptionPkts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChIpCtrl suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChIpCtrl)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpSpecialRouterTriggerEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpExceptionCommandSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpUcRouteAgingModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpRouterSourceIdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpQosProfileToMultiTargetTCQueueMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpMultiTargetQueueFullDropCntGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpMultiTargetQueueFullDropCntSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpMultiTargetTCQueueSchedModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpBridgeServiceEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpMllBridgeEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpMultiTargetRateShaperSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpMultiTargetUcSchedModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpArpBcModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpPortRoutingEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpPortRoutingEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpQosProfileToRouteEntryMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpRoutingEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpRoutingEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpCntGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpCntSetModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpCntSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpSetMllCntInterface)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpMllCntGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpMllCntSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpDropCntSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpSetDropCntMode)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpDropCntGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpMtuProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpv6AddrPrefixScopeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpv6UcScopeCommandSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpv6McScopeCommandSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpRouterMacSaBaseSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpRouterMacSaBaseGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpRouterMacSaLsbModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpRouterMacSaLsbModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpPortRouterMacSaLsbModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpPortRouterMacSaLsbModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpRouterPortVlanMacSaLsbSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpRouterPortVlanMacSaLsbGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpRouterPortMacSaLsbSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpRouterPortMacSaLsbGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpRouterVlanMacSaLsbSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpRouterVlanMacSaLsbGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpRouterMacSaModifyEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpRouterMacSaModifyEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpEcmpUcRpfCheckEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpEcmpUcRpfCheckEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpPortGroupCntGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpPortGroupDropCntSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpPortGroupMllCntGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpPortGroupCntSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpPortGroupDropCntGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpPortGroupMllCntSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpPortGroupMultiTargetQueueFullDropCntSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpUcRpfVlanModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpUcRpfVlanModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpPortSipSaEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpPortSipSaEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpPortGroupMultiTargetRateShaperSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpRouterBridgedPacketsExceptionCntGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpRouterBridgedPacketsExceptionCntSet)

UTF_SUIT_END_TESTS_MAC(cpssDxChIpCtrl)

