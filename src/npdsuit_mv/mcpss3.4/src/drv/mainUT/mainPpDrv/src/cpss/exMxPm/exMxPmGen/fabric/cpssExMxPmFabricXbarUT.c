/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmFabricXbarUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmFabricXbar, that provides
*       CPSS ExMxPm Fabric Connectivity XBAR API
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricXbar.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define FABRIC_XBAR_INVALID_ENUM_CNS    0x5AAAAAA5

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarConfigSet
(
    IN GT_U8                              devNum,
    IN CPSS_EXMXPM_FABRIC_XBAR_CONFIG_STC *xbarCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with xbarCfgPtr {voQUnit0ActiveHyperGlink [0 / 1],
                               voQUnit1ActiveHyperGlink [0 / 1],
                               xbarTcModeEnable [GT_FALSE / GT_TRUE],
                               mode [CPSS_EXMXPM_FABRIC_XBAR_BYPASS_ACTIVE_STANDBY_E]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricXbarConfigGet with non-NULL xbarCfgPtr.
    Expected: GT_OK and the same xbarCfgPtr.
    1.3. Call with out of range xbarCfgPtr->mode [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with xbarCfgPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_XBAR_CONFIG_STC xbarCfg;
    CPSS_EXMXPM_FABRIC_XBAR_CONFIG_STC xbarCfgGet;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with xbarCfgPtr {voQUnit0ActiveHyperGlink [0 / 1],
                                           voQUnit1ActiveHyperGlink [2 / 3],
                                           xbarTcModeEnable [GT_FALSE / GT_TRUE],
                                           mode [CPSS_EXMXPM_FABRIC_XBAR_BYPASS_ACTIVE_STANDBY_E]}.
            Expected: GT_OK.
        */

        /* Call with [0] */
        xbarCfg.voQUnit0ActiveHyperGlink  = 0;
        xbarCfg.voQUnit1ActiveHyperGlink = 2;
        xbarCfg.xbarTcModeEnable = GT_FALSE;
        xbarCfg.mode       = CPSS_EXMXPM_FABRIC_XBAR_BYPASS_ACTIVE_STANDBY_E;

        st = cpssExMxPmFabricXbarConfigSet(dev, &xbarCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmFabricXbarConfigGet with non-NULL xbarCfgPtr.
            Expected: GT_OK and the same xbarCfgPtr.
        */
        st = cpssExMxPmFabricXbarConfigGet(dev, &xbarCfgGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricXbarConfigGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(xbarCfg.voQUnit0ActiveHyperGlink, xbarCfgGet.voQUnit0ActiveHyperGlink,
                   "get another xbarCfgPtr->voQUnit0ActiveHyperGlink than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xbarCfg.voQUnit1ActiveHyperGlink, xbarCfgGet.voQUnit1ActiveHyperGlink,
                   "get another xbarCfgPtr->voQUnit1ActiveHyperGlink than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xbarCfg.xbarTcModeEnable, xbarCfgGet.xbarTcModeEnable,
                   "get another xbarCfgPtr->xbarTcModeEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xbarCfg.mode, xbarCfgGet.mode,
                   "get another xbarCfgPtr->mode than was set: %d", dev);

        /* Call with [1] */
        xbarCfg.voQUnit0ActiveHyperGlink  = 1;
        xbarCfg.voQUnit1ActiveHyperGlink = 3;
        xbarCfg.xbarTcModeEnable = GT_TRUE;
        xbarCfg.mode             = CPSS_EXMXPM_FABRIC_XBAR_BYPASS_ACTIVE_STANDBY_E;

        st = cpssExMxPmFabricXbarConfigSet(dev, &xbarCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmFabricXbarConfigGet with non-NULL xbarCfgPtr.
            Expected: GT_OK and the same xbarCfgPtr.
        */
        st = cpssExMxPmFabricXbarConfigGet(dev, &xbarCfgGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricXbarConfigGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(xbarCfg.voQUnit0ActiveHyperGlink, xbarCfgGet.voQUnit0ActiveHyperGlink,
                   "get another xbarCfgPtr->voQUnit0ActiveHyperGlink than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xbarCfg.voQUnit1ActiveHyperGlink, xbarCfgGet.voQUnit1ActiveHyperGlink,
                   "get another xbarCfgPtr->voQUnit1ActiveHyperGlink than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xbarCfg.xbarTcModeEnable, xbarCfgGet.xbarTcModeEnable,
                   "get another xbarCfgPtr->xbarTcModeEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xbarCfg.mode, xbarCfgGet.mode,
                   "get another xbarCfgPtr->mode than was set: %d", dev);

        /*
            1.3. Call with out of range xbarCfgPtr->mode [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        xbarCfg.mode = FABRIC_XBAR_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricXbarConfigSet(dev, &xbarCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, xbarCfgPtr->mode = %d",
                                     dev, xbarCfg.mode);

        xbarCfg.mode = CPSS_EXMXPM_FABRIC_XBAR_BYPASS_ACTIVE_STANDBY_E;

        /*
            1.4. Call with xbarCfgPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarConfigSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, xbarCfgPtr = NULL", dev);
    }

    xbarCfg.voQUnit0ActiveHyperGlink  = 0;
    xbarCfg.voQUnit1ActiveHyperGlink = 0;
    xbarCfg.xbarTcModeEnable = GT_FALSE;
    xbarCfg.mode       = CPSS_EXMXPM_FABRIC_XBAR_BYPASS_ACTIVE_STANDBY_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarConfigSet(dev, &xbarCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarConfigSet(dev, &xbarCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarConfigGet
(
    IN  GT_U8                              devNum,
    OUT CPSS_EXMXPM_FABRIC_XBAR_CONFIG_STC *xbarCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL xbarCfgPtr.
    Expected: GT_OK.
    1.3. Call with xbarCfgPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_XBAR_CONFIG_STC xbarCfg;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL xbarCfgPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmFabricXbarConfigGet(dev, &xbarCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with xbarCfgPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarConfigGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, xbarCfgPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarConfigGet(dev, &xbarCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarConfigGet(dev, &xbarCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportConfigSet
(
    IN  GT_U8                                      devNum,
    IN  GT_FPORT                                   fport,
    IN  CPSS_EXMXPM_FABRIC_XBAR_FPORT_CONFIG_STC   *fportConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   and fportConfigPtr {portEnable [GT_FALSE / GT_TRUE],
                                       mcSrcDevFilterEnable [GT_FALSE / GT_TRUE],
                                       bcSrcDevFilterEnable [GT_FALSE / GT_TRUE],
                                       ucSrcDevFilterEnable [GT_FALSE / GT_TRUE],
                                       diagCellSuperHighPriorityAssign [GT_FALSE / GT_TRUE],
                                       diagSrcDevFilterEnable [GT_FALSE / GT_TRUE]
                                       bcCellSuperHighPriorityAssign [GT_FALSE / GT_TRUE] }.
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricXbarFportConfigGet with the same xbarNum,
                                                       fport
                                                       and non-NULL fportConfigPtr.
    Expected: GT_OK and the same fportConfigPtr.
    1.3. Call with out of range fport [6]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with fportConfigPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                    fport   = 0;
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_CONFIG_STC    fportConfig;
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_CONFIG_STC    fportConfigGet;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           and fportConfigPtr {portEnable [GT_FALSE / GT_TRUE],
                                               mcSrcDevFilterEnable [GT_FALSE / GT_TRUE],
                                               bcSrcDevFilterEnable [GT_FALSE / GT_TRUE],
                                               ucSrcDevFilterEnable [GT_FALSE / GT_TRUE],
                                               diagCellSuperHighPriorityAssign [GT_FALSE / GT_TRUE],
                                               diagSrcDevFilterEnable [GT_FALSE / GT_TRUE]
                                               bcCellSuperHighPriorityAssign [GT_FALSE / GT_TRUE] }.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;

        fportConfig.portEnable                      = GT_FALSE;
        fportConfig.mcSrcDevFilterEnable            = GT_FALSE;
        fportConfig.bcSrcDevFilterEnable            = GT_FALSE;
        fportConfig.ucSrcDevFilterEnable            = GT_FALSE;
        fportConfig.diagCellSuperHighPriorityAssign = GT_FALSE;
        fportConfig.diagSrcDevFilterEnable          = GT_FALSE;
        fportConfig.bcCellSuperHighPriorityAssign   = GT_FALSE;

        st = cpssExMxPmFabricXbarFportConfigSet(dev, fport, &fportConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmFabricXbarFportConfigGet with the same fport
                                                               and non-NULL fportConfigPtr.
            Expected: GT_OK and the same fportConfigPtr.
        */
        st = cpssExMxPmFabricXbarFportConfigGet(dev, fport, &fportConfigGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricXbarFportConfigGet: %d, %d", dev, fport);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(fportConfig.portEnable, fportConfigGet.portEnable,
                   "get another fportConfigPtr->portEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportConfig.mcSrcDevFilterEnable, fportConfigGet.mcSrcDevFilterEnable,
                   "get another fportConfigPtr->mcSrcDevFilterEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportConfig.bcSrcDevFilterEnable, fportConfigGet.bcSrcDevFilterEnable,
                   "get another fportConfigPtr->bcSrcDevFilterEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportConfig.ucSrcDevFilterEnable, fportConfigGet.ucSrcDevFilterEnable,
                   "get another fportConfigPtr->ucSrcDevFilterEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportConfig.diagCellSuperHighPriorityAssign, fportConfigGet.diagCellSuperHighPriorityAssign,
                   "get another fportConfigPtr->diagCellSuperHighPriorityAssign than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportConfig.diagSrcDevFilterEnable, fportConfigGet.diagSrcDevFilterEnable,
                   "get another fportConfigPtr->diagSrcDevFilterEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportConfig.bcCellSuperHighPriorityAssign, fportConfigGet.bcCellSuperHighPriorityAssign,
                   "get another fportConfigPtr->bcCellSuperHighPriorityAssign than was set: %d", dev);

        /* Call with fport [5] */
        fport   = 5;

        fportConfig.portEnable                      = GT_TRUE;
        fportConfig.mcSrcDevFilterEnable            = GT_TRUE;
        fportConfig.bcSrcDevFilterEnable            = GT_TRUE;
        fportConfig.ucSrcDevFilterEnable            = GT_TRUE;
        fportConfig.diagCellSuperHighPriorityAssign = GT_TRUE;
        fportConfig.diagSrcDevFilterEnable          = GT_TRUE;
        fportConfig.bcCellSuperHighPriorityAssign   = GT_TRUE;

        st = cpssExMxPmFabricXbarFportConfigSet(dev, fport, &fportConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmFabricXbarFportConfigGet with the same fport
                                                               and non-NULL fportConfigPtr.
            Expected: GT_OK and the same fportConfigPtr.
        */
        st = cpssExMxPmFabricXbarFportConfigGet(dev, fport, &fportConfigGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricXbarFportConfigGet: %d, %d", dev, fport);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(fportConfig.portEnable, fportConfigGet.portEnable,
                   "get another fportConfigPtr->portEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportConfig.mcSrcDevFilterEnable, fportConfigGet.mcSrcDevFilterEnable,
                   "get another fportConfigPtr->mcSrcDevFilterEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportConfig.bcSrcDevFilterEnable, fportConfigGet.bcSrcDevFilterEnable,
                   "get another fportConfigPtr->bcSrcDevFilterEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportConfig.ucSrcDevFilterEnable, fportConfigGet.ucSrcDevFilterEnable,
                   "get another fportConfigPtr->ucSrcDevFilterEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportConfig.diagCellSuperHighPriorityAssign, fportConfigGet.diagCellSuperHighPriorityAssign,
                   "get another fportConfigPtr->diagCellSuperHighPriorityAssign than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportConfig.diagSrcDevFilterEnable, fportConfigGet.diagSrcDevFilterEnable,
                   "get another fportConfigPtr->diagSrcDevFilterEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportConfig.bcCellSuperHighPriorityAssign, fportConfigGet.bcCellSuperHighPriorityAssign,
                   "get another fportConfigPtr->bcCellSuperHighPriorityAssign than was set: %d", dev);

        /*
            1.3. Call with out of range fport [6]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportConfigSet(dev, fport, &fportConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.4. Call with fportConfigPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportConfigSet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fportConfigPtr = NULL", dev);
    }

    fport = 0;

    fportConfig.portEnable                      = GT_FALSE;
    fportConfig.mcSrcDevFilterEnable            = GT_FALSE;
    fportConfig.bcSrcDevFilterEnable            = GT_FALSE;
    fportConfig.ucSrcDevFilterEnable            = GT_FALSE;
    fportConfig.diagCellSuperHighPriorityAssign = GT_FALSE;
    fportConfig.diagSrcDevFilterEnable          = GT_FALSE;
    fportConfig.bcCellSuperHighPriorityAssign   = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportConfigSet(dev, fport, &fportConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportConfigSet(dev, fport, &fportConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportConfigGet
(
    IN  GT_U8                                      devNum,
    IN  GT_FPORT                                   fport,
    OUT CPSS_EXMXPM_FABRIC_XBAR_FPORT_CONFIG_STC   *fportConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   and non-NULL fportConfigPtr.
    Expected: GT_OK.
    1.2. Call with out of range fport [6]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with fportConfigPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                    fport   = 0;
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_CONFIG_STC    fportConfig;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           and non-NULL fportConfigPtr.
            Expected: GT_OK.
        */

        /* Call with fport [5] */
        fport   = 0;

        st = cpssExMxPmFabricXbarFportConfigGet(dev, fport, &fportConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /* Call with fport [5] */
        fport = 5;

        st = cpssExMxPmFabricXbarFportConfigGet(dev, fport, &fportConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.3. Call with out of range fport [6]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportConfigGet(dev, fport, &fportConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.4. Call with fportConfigPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportConfigGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fportConfigPtr = NULL", dev);
    }

    fport = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportConfigGet(dev, fport, &fportConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportConfigGet(dev, fport, &fportConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportBlockingSet
(
    IN  GT_U8                                       devNum,
    IN  GT_FPORT                                    fport,
    IN  CPSS_EXMXPM_FABRIC_XBAR_FPORT_BLOCKING_STC  *fportBlockingPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportBlockingSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   and fportBlockingPtr {enableTxCellsFromFPort2 [GT_FALSE / GT_TRUE],
                                         enableTxCellsFromFPort1 [GT_FALSE / GT_TRUE],
                                         enableTxCellsFromFPort0 [GT_FALSE / GT_TRUE],
                                         enableTxCellsToFPort2 [GT_FALSE / GT_TRUE],
                                         enableTxCellsToFPort1 [GT_FALSE / GT_TRUE],
                                         enableTxCellsToFPort0 [GT_FALSE / GT_TRUE],
                                         enableRxCells [GT_FALSE / GT_TRUE],
                                         enableHighPrioTxCells [GT_FALSE / GT_TRUE],
                                         enableFPrio3TxCells [GT_FALSE / GT_TRUE],
                                         enableFPrio2TxCells [GT_FALSE / GT_TRUE],
                                         enableFPrio1TxCells [GT_FALSE / GT_TRUE],
                                         enableFPrio0TxCells [GT_FALSE / GT_TRUE]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricXbarFportBlockingGet with the same fport
                                                        and non-NULL fportBlockingPtr.
    Expected: GT_OK and the same fportBlockingPtr.
    1.3. Call with out of range fport [6]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with fportBlockingPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                    fport = 0;
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_BLOCKING_STC  fportBlocking;
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_BLOCKING_STC  fportBlockingGet;


    cpssOsBzero((GT_VOID*) &fportBlocking, sizeof(fportBlocking));
    cpssOsBzero((GT_VOID*) &fportBlockingGet, sizeof(fportBlockingGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 2]
                           and fportBlockingPtr {enableTxCellsFromFPort2 [GT_FALSE / GT_TRUE],
                                                 enableTxCellsFromFPort1 [GT_FALSE / GT_TRUE],
                                                 enableTxCellsFromFPort0 [GT_FALSE / GT_TRUE],
                                                 enableTxCellsToFPort2 [GT_FALSE / GT_TRUE],
                                                 enableTxCellsToFPort1 [GT_FALSE / GT_TRUE],
                                                 enableTxCellsToFPort0 [GT_FALSE / GT_TRUE],
                                                 enableRxCells [GT_FALSE / GT_TRUE],
                                                 enableHighPrioTxCells [GT_FALSE / GT_TRUE],
                                                 enableFPrio3TxCells [GT_FALSE / GT_TRUE],
                                                 enableFPrio2TxCells [GT_FALSE / GT_TRUE],
                                                 enableFPrio1TxCells [GT_FALSE / GT_TRUE],
                                                 enableFPrio0TxCells [GT_FALSE / GT_TRUE]}.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;
        fportBlocking.enableTxCellsFromFPort5 = GT_FALSE;
        fportBlocking.enableTxCellsFromFPort4 = GT_FALSE;
        fportBlocking.enableTxCellsFromFPort3 = GT_FALSE;
        fportBlocking.enableTxCellsFromFPort2 = GT_FALSE;
        fportBlocking.enableTxCellsFromFPort1 = GT_FALSE;
        fportBlocking.enableTxCellsFromFPort0 = GT_FALSE;
        fportBlocking.enableTxCellsToFPort5   = GT_FALSE;
        fportBlocking.enableTxCellsToFPort4   = GT_FALSE;
        fportBlocking.enableTxCellsToFPort3   = GT_FALSE;
        fportBlocking.enableTxCellsToFPort2   = GT_FALSE;
        fportBlocking.enableTxCellsToFPort1   = GT_FALSE;
        fportBlocking.enableTxCellsToFPort0   = GT_FALSE;
        fportBlocking.enableRxCells           = GT_FALSE;
        fportBlocking.enableHighPrioTxCells   = GT_FALSE;
        fportBlocking.enableFPrio3TxCells     = GT_FALSE;
        fportBlocking.enableFPrio2TxCells     = GT_FALSE;
        fportBlocking.enableFPrio1TxCells     = GT_FALSE;
        fportBlocking.enableFPrio0TxCells     = GT_FALSE;

        st = cpssExMxPmFabricXbarFportBlockingSet(dev, fport, &fportBlocking);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmFabricXbarFportBlockingGet with the samefport
                                                                and non-NULL fportBlockingPtr.
            Expected: GT_OK and the same fportBlockingPtr.
        */
        st = cpssExMxPmFabricXbarFportBlockingGet(dev, fport, &fportBlockingGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricXbarFportBlockingGet: %d, %d", dev, fport);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsFromFPort5, fportBlockingGet.enableTxCellsFromFPort5,
                   "get another fportBlockingPtr->enableTxCellsFromFPort5 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsFromFPort4, fportBlockingGet.enableTxCellsFromFPort4,
                   "get another fportBlockingPtr->enableTxCellsFromFPort4 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsFromFPort3, fportBlockingGet.enableTxCellsFromFPort3,
                   "get another fportBlockingPtr->enableTxCellsFromFPort3 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsFromFPort2, fportBlockingGet.enableTxCellsFromFPort2,
                   "get another fportBlockingPtr->enableTxCellsFromFPort2 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsFromFPort1, fportBlockingGet.enableTxCellsFromFPort1,
                   "get another fportBlockingPtr->enableTxCellsFromFPort1 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsFromFPort0, fportBlockingGet.enableTxCellsFromFPort0,
                   "get another fportBlockingPtr->enableTxCellsFromFPort0 than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsToFPort5, fportBlockingGet.enableTxCellsToFPort5,
                   "get another fportBlockingPtr->enableTxCellsToFPort5 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsToFPort4, fportBlockingGet.enableTxCellsToFPort4,
                   "get another fportBlockingPtr->enableTxCellsToFPort4 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsToFPort3, fportBlockingGet.enableTxCellsToFPort3,
                   "get another fportBlockingPtr->enableTxCellsToFPort3 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsToFPort2, fportBlockingGet.enableTxCellsToFPort2,
                   "get another fportBlockingPtr->enableTxCellsToFPort2 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsToFPort1, fportBlockingGet.enableTxCellsToFPort1,
                   "get another fportBlockingPtr->enableTxCellsToFPort1 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsToFPort0, fportBlockingGet.enableTxCellsToFPort0,
                   "get another fportBlockingPtr->enableTxCellsToFPort0 than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableRxCells, fportBlockingGet.enableRxCells,
                   "get another fportBlockingPtr->enableRxCells than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableHighPrioTxCells, fportBlockingGet.enableHighPrioTxCells,
                   "get another fportBlockingPtr->enableHighPrioTxCells than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableFPrio3TxCells, fportBlockingGet.enableFPrio3TxCells,
                   "get another fportBlockingPtr->enableFPrio3TxCells than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableFPrio2TxCells, fportBlockingGet.enableFPrio2TxCells,
                   "get another fportBlockingPtr->enableFPrio2TxCells than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableFPrio1TxCells, fportBlockingGet.enableFPrio1TxCells,
                   "get another fportBlockingPtr->enableFPrio1TxCells than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableFPrio0TxCells, fportBlockingGet.enableFPrio0TxCells,
                   "get another fportBlockingPtr->enableFPrio0TxCells than was set: %d", dev);

        /* Call with fport [5] */
        fport   = 5;

        fportBlocking.enableTxCellsFromFPort5 = GT_TRUE;
        fportBlocking.enableTxCellsFromFPort4 = GT_TRUE;
        fportBlocking.enableTxCellsFromFPort3 = GT_TRUE;
        fportBlocking.enableTxCellsFromFPort2 = GT_TRUE;
        fportBlocking.enableTxCellsFromFPort1 = GT_TRUE;
        fportBlocking.enableTxCellsFromFPort0 = GT_TRUE;
        fportBlocking.enableTxCellsToFPort5   = GT_TRUE;
        fportBlocking.enableTxCellsToFPort4   = GT_TRUE;
        fportBlocking.enableTxCellsToFPort3   = GT_TRUE;
        fportBlocking.enableTxCellsToFPort2   = GT_TRUE;
        fportBlocking.enableTxCellsToFPort1   = GT_TRUE;
        fportBlocking.enableTxCellsToFPort0   = GT_TRUE;
        fportBlocking.enableRxCells           = GT_TRUE;
        fportBlocking.enableHighPrioTxCells   = GT_TRUE;
        fportBlocking.enableFPrio3TxCells     = GT_TRUE;
        fportBlocking.enableFPrio2TxCells     = GT_TRUE;
        fportBlocking.enableFPrio1TxCells     = GT_TRUE;
        fportBlocking.enableFPrio0TxCells     = GT_TRUE;

        st = cpssExMxPmFabricXbarFportBlockingSet(dev, fport, &fportBlocking);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmFabricXbarFportBlockingGet with the same fport
                                                                and non-NULL fportBlockingPtr.
            Expected: GT_OK and the same fportBlockingPtr.
        */
        st = cpssExMxPmFabricXbarFportBlockingGet(dev, fport, &fportBlockingGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricXbarFportBlockingGet: %d, %d", dev, fport);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsFromFPort5, fportBlockingGet.enableTxCellsFromFPort5,
                   "get another fportBlockingPtr->enableTxCellsFromFPort5 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsFromFPort4, fportBlockingGet.enableTxCellsFromFPort4,
                   "get another fportBlockingPtr->enableTxCellsFromFPort4 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsFromFPort3, fportBlockingGet.enableTxCellsFromFPort3,
                   "get another fportBlockingPtr->enableTxCellsFromFPort3 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsFromFPort2, fportBlockingGet.enableTxCellsFromFPort2,
                   "get another fportBlockingPtr->enableTxCellsFromFPort2 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsFromFPort1, fportBlockingGet.enableTxCellsFromFPort1,
                   "get another fportBlockingPtr->enableTxCellsFromFPort1 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsFromFPort0, fportBlockingGet.enableTxCellsFromFPort0,
                   "get another fportBlockingPtr->enableTxCellsFromFPort0 than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsToFPort5, fportBlockingGet.enableTxCellsToFPort5,
                   "get another fportBlockingPtr->enableTxCellsToFPort5 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsToFPort4, fportBlockingGet.enableTxCellsToFPort4,
                   "get another fportBlockingPtr->enableTxCellsToFPort4 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsToFPort3, fportBlockingGet.enableTxCellsToFPort3,
                   "get another fportBlockingPtr->enableTxCellsToFPort3 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsToFPort2, fportBlockingGet.enableTxCellsToFPort2,
                   "get another fportBlockingPtr->enableTxCellsToFPort2 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsToFPort1, fportBlockingGet.enableTxCellsToFPort1,
                   "get another fportBlockingPtr->enableTxCellsToFPort1 than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableTxCellsToFPort0, fportBlockingGet.enableTxCellsToFPort0,
                   "get another fportBlockingPtr->enableTxCellsToFPort0 than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableRxCells, fportBlockingGet.enableRxCells,
                   "get another fportBlockingPtr->enableRxCells than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableHighPrioTxCells, fportBlockingGet.enableHighPrioTxCells,
                   "get another fportBlockingPtr->enableHighPrioTxCells than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableFPrio3TxCells, fportBlockingGet.enableFPrio3TxCells,
                   "get another fportBlockingPtr->enableFPrio3TxCells than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableFPrio2TxCells, fportBlockingGet.enableFPrio2TxCells,
                   "get another fportBlockingPtr->enableFPrio2TxCells than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableFPrio1TxCells, fportBlockingGet.enableFPrio1TxCells,
                   "get another fportBlockingPtr->enableFPrio1TxCells than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportBlocking.enableFPrio0TxCells, fportBlockingGet.enableFPrio0TxCells,
                   "get another fportBlockingPtr->enableFPrio0TxCells than was set: %d", dev);

        /*
            1.4. Call with out of range fport [6]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportBlockingSet(dev, fport, &fportBlocking);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.5. Call with fportBlockingPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportBlockingSet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fportBlockingPtr = NULL", dev);
    }

    fport   = 0;

    fportBlocking.enableTxCellsFromFPort5 = GT_FALSE;
    fportBlocking.enableTxCellsFromFPort4 = GT_FALSE;
    fportBlocking.enableTxCellsFromFPort3 = GT_FALSE;
    fportBlocking.enableTxCellsFromFPort2 = GT_FALSE;
    fportBlocking.enableTxCellsFromFPort1 = GT_FALSE;
    fportBlocking.enableTxCellsFromFPort0 = GT_FALSE;
    fportBlocking.enableTxCellsToFPort5   = GT_FALSE;
    fportBlocking.enableTxCellsToFPort4   = GT_FALSE;
    fportBlocking.enableTxCellsToFPort3   = GT_FALSE;
    fportBlocking.enableTxCellsToFPort2   = GT_FALSE;
    fportBlocking.enableTxCellsToFPort1   = GT_FALSE;
    fportBlocking.enableTxCellsToFPort0   = GT_FALSE;
    fportBlocking.enableRxCells           = GT_FALSE;
    fportBlocking.enableHighPrioTxCells   = GT_FALSE;
    fportBlocking.enableFPrio3TxCells     = GT_FALSE;
    fportBlocking.enableFPrio2TxCells     = GT_FALSE;
    fportBlocking.enableFPrio1TxCells     = GT_FALSE;
    fportBlocking.enableFPrio0TxCells     = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportBlockingSet(dev, fport, &fportBlocking);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportBlockingSet(dev, fport, &fportBlocking);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportBlockingGet
(
    IN  GT_U8                                       devNum,
    IN  GT_FPORT                                    fport,
    OUT CPSS_EXMXPM_FABRIC_XBAR_FPORT_BLOCKING_STC  *fportBlockingPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportBlockingGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   and non-NULL fportBlockingPtr.
    Expected: GT_OK.
    1.3. Call with out of range fport [6]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with fportBlockingPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                    fport   = 0;
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_BLOCKING_STC  fportBlocking;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           and non-NULL fportBlockingPtr.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;

        st = cpssExMxPmFabricXbarFportBlockingGet(dev, fport, &fportBlocking);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /* Call with fport [5] */
        fport = 5;

        st = cpssExMxPmFabricXbarFportBlockingGet(dev, fport, &fportBlocking);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.3. Call with out of range fport [6]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportBlockingGet(dev, fport, &fportBlocking);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.4. Call with fportBlockingPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportBlockingGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fportBlockingPtr = NULL", dev);
    }

    fport = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportBlockingGet(dev, fport, &fportBlocking);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportBlockingGet(dev, fport, &fportBlocking);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportFcSet
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      xbarNum,
    IN  GT_FPORT                                    fport,
    IN  CPSS_EXMXPM_FABRIC_XBAR_FPORT_FC_STC        *fportFcPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportFcSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   and fportFcPtr {regularPrioXoffTheshold [0 / 255],
                                   regularPrioXonTheshold [0 / 255]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricXbarFportFcGet with the same xbarNum,
                                                  fport
                                                  and non-NULL fportFcPtr.
    Expected: GT_OK and the same fportFcPtr.
    1.3. Call with out of range fport [6]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range fportFcPtr->regularPrioXoffTheshold [256]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range fportFcPtr->regularPrioXonTheshold [256]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with fportFcPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                fport   = 0;
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_FC_STC    fportFc;
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_FC_STC    fportFcGet;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           and fportFcPtr {regularPrioXoffTheshold [0 / 255],
                                           regularPrioXonTheshold [0 / 255]}.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;

        fportFc.regularPrioXoffTheshold   = 0;
        fportFc.regularPrioXonTheshold    = 0;

        st = cpssExMxPmFabricXbarFportFcSet(dev, fport, &fportFc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmFabricXbarFportFcGet with the same fport
                                                          and non-NULL fportFcPtr.
            Expected: GT_OK and the same fportFcPtr.
        */
        st = cpssExMxPmFabricXbarFportFcGet(dev, fport, &fportFcGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricXbarFportFcGet: %d, %d", dev, fport);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(fportFc.regularPrioXoffTheshold, fportFcGet.regularPrioXoffTheshold,
                   "get another fportFcPtr->regularPrioXoffTheshold than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportFc.regularPrioXonTheshold, fportFcGet.regularPrioXonTheshold,
                   "get another fportFcPtr->regularPrioXonTheshold than was set: %d", dev);

        /* Call with fport [5] */
        fport = 5;

        fportFc.regularPrioXoffTheshold   = 0xFF;
        fportFc.regularPrioXonTheshold    = 0xFF;

        st = cpssExMxPmFabricXbarFportFcSet(dev, fport, &fportFc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.3. Call cpssExMxPmFabricXbarFportFcGet with the same fport
                                                          and non-NULL fportFcPtr.
            Expected: GT_OK and the same fportFcPtr.
        */
        st = cpssExMxPmFabricXbarFportFcGet(dev, fport, &fportFcGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricXbarFportFcGet: %d, %d", dev, fport);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(fportFc.regularPrioXoffTheshold, fportFcGet.regularPrioXoffTheshold,
                   "get another fportFcPtr->regularPrioXoffTheshold than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fportFc.regularPrioXonTheshold, fportFcGet.regularPrioXonTheshold,
                   "get another fportFcPtr->regularPrioXonTheshold than was set: %d", dev);

        /*
            1.4. Call with out of range fport [6]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportFcSet(dev, fport, &fportFc);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.5. Call with out of range fportFcPtr->regularPrioXoffTheshold [256]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fportFc.regularPrioXoffTheshold = 256;

        st = cpssExMxPmFabricXbarFportFcSet(dev, fport, &fportFc);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fportFcPtr->regularPrioXoffTheshold = %d",
                                         dev, fportFc.regularPrioXoffTheshold);

        fportFc.regularPrioXoffTheshold = 0;

        /*
            1.6. Call with out of range fportFcPtr->regularPrioXonTheshold [256]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fportFc.regularPrioXonTheshold = 256;

        st = cpssExMxPmFabricXbarFportFcSet(dev, fport, &fportFc);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fportFcPtr->regularPrioXonTheshold = %d",
                                         dev, fportFc.regularPrioXonTheshold);

        fportFc.regularPrioXonTheshold = 0;

        /*
            1.7. Call with fportFcPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportFcSet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fportFcPtr = NULL", dev);
    }

    fport   = 0;

    fportFc.regularPrioXoffTheshold   = 0;
    fportFc.regularPrioXonTheshold    = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportFcSet(dev, fport, &fportFc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportFcSet(dev, fport, &fportFc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportFcGet
(
    IN  GT_U8                                       devNum,
    IN  GT_FPORT                                    fport,
    OUT CPSS_EXMXPM_FABRIC_XBAR_FPORT_FC_STC        *fportFcPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportFcGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   and non-NULL fportFcPtr.
    Expected: GT_OK.
    1.2. Call with out of range fport [6]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with fportFcPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                fport   = 0;
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_FC_STC    fportFc;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           and non-NULL fportFcPtr.
            Expected: GT_OK.
        */
        /* Call with fport [0] */
        fport = 0;

        st = cpssExMxPmFabricXbarFportFcGet(dev, fport, &fportFc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /* Call with fport [5] */
        fport = 5;

        st = cpssExMxPmFabricXbarFportFcGet(dev, fport, &fportFc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call with out of range fport [6]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportFcGet(dev, fport, &fportFc);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.3. Call with fportFcPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportFcGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fportFcPtr = NULL", dev);
    }

    fport = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportFcGet(dev, fport, &fportFc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportFcGet(dev, fport, &fportFc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportArbiterSet
(
    IN  GT_U8                                                   devNum,
    IN  GT_FPORT                                                fport,
    IN  CPSS_EXMXPM_FABRIC_XBAR_FPORT_ARBITER_CONFIG_STC   *arbConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportArbiterSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   and arbConfigPtr {srcFportRegularPrioGroup [0,0,0 / 1,1,1],
                                     srcFportSuperHighPrioGroup [0,0,0 / 1,1,1],
                                     groupRegularPrioWeight [0x0, 0x0 / 0xFF, 0xFF],
                                     groupSuperHighPrioWeight [10, 100 / 0xFF, 0xFF]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricXbarFportArbiterGet with the same fport
                                                       and non-NULL arbConfigPtr.
    Expected: GT_OK and the same arbConfigPtr.
    1.3. Call with out of range fport [6]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range arbConfigPtr->srcFportRegularPrioGroup [0][2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range arbConfigPtr->srcFportSuperHighPrioGroup [0][2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range arbConfigPtr->groupRegularPrioWeight [0][256]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range arbConfigPtr->groupSuperHighPrioWeight [0][256]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with arbConfigPtr->groupRegularPrioWeight [0,0],
                   arbConfigPtr->groupSuperHighPrioWeight [0,0] (invalid)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.9. Call with arbConfigPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                            fport     = 0;
    GT_BOOL                                             isEqual   = GT_FALSE;
    GT_U16                                              fportIter = 0;
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_ARBITER_CONFIG_STC    arbConfig;
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_ARBITER_CONFIG_STC    arbConfigGet;


    cpssOsBzero((GT_VOID*) &arbConfig, sizeof(arbConfig));
    cpssOsBzero((GT_VOID*) &arbConfigGet, sizeof(arbConfigGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           and arbConfigPtr {srcFportRegularPrioGroup [0,0,0 / 1,1,1],
                                             srcFportSuperHighPrioGroup [0,0,0 / 1,1,1],
                                             groupRegularPrioWeight [0x0, 0x0 / 0xFF, 0xFF],
                                             groupSuperHighPrioWeight [10, 100 / 0xFF, 0xFF]}.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;

        for (fportIter = 0; fportIter <= 5; fportIter++)
        {
            arbConfig.srcFportRegularPrioGroup[fportIter] = 0;
            arbConfig.srcFportSuperHighPrioGroup[fportIter] = 0;
        }

        arbConfig.groupRegularPrioWeight[0] = 0;
        arbConfig.groupRegularPrioWeight[1] = 1;

        arbConfig.groupSuperHighPrioWeight[0] = 10;
        arbConfig.groupSuperHighPrioWeight[1] = 100;

        st = cpssExMxPmFabricXbarFportArbiterSet(dev, fport, &arbConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmFabricXbarFportArbiterGet with the same fport
                                                               and non-NULL arbConfigPtr.
            Expected: GT_OK and the same arbConfigPtr.
        */
        st = cpssExMxPmFabricXbarFportArbiterGet(dev, fport, &arbConfigGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricXbarFportArbiterGet: %d, %d", dev, fport);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) arbConfig.srcFportRegularPrioGroup,
                                     (GT_VOID*) arbConfigGet.srcFportRegularPrioGroup,
                                     sizeof(arbConfig.srcFportRegularPrioGroup[0]*3))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another arbConfigPtr->srcFportRegularPrioGroup than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) arbConfig.srcFportSuperHighPrioGroup,
                                     (GT_VOID*) arbConfigGet.srcFportSuperHighPrioGroup,
                                     sizeof(arbConfig.srcFportSuperHighPrioGroup[0]*3))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another arbConfigPtr->srcFportSuperHighPrioGroup than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) arbConfig.groupRegularPrioWeight,
                                     (GT_VOID*) arbConfigGet.groupRegularPrioWeight,
                                     sizeof(arbConfig.groupRegularPrioWeight[0]*2))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another arbConfigPtr->groupRegularPrioWeight than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) arbConfig.groupSuperHighPrioWeight,
                                     (GT_VOID*) arbConfigGet.groupSuperHighPrioWeight,
                                     sizeof(arbConfig.groupSuperHighPrioWeight[0]*2))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another arbConfigPtr->groupSuperHighPrioWeight than was set: %d", dev);


        /* Call with fport [5] */
        fport = 5;

        for (fportIter = 0; fportIter <= 5; fportIter++)
        {
            arbConfig.srcFportRegularPrioGroup[fportIter] = 1;
            arbConfig.srcFportSuperHighPrioGroup[fportIter] = 1;
        }

        arbConfig.groupRegularPrioWeight[0] = 0xFF;
        arbConfig.groupRegularPrioWeight[1] = 0xFF;

        arbConfig.groupSuperHighPrioWeight[0] = 0xFF;
        arbConfig.groupSuperHighPrioWeight[1] = 0xFF;

        st = cpssExMxPmFabricXbarFportArbiterSet(dev, fport, &arbConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.3. Call cpssExMxPmFabricXbarFportArbiterGet with the same fport
                                                               and non-NULL arbConfigPtr.
            Expected: GT_OK and the same arbConfigPtr.
        */
        st = cpssExMxPmFabricXbarFportArbiterGet(dev, fport, &arbConfigGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricXbarFportArbiterGet: %d, %d", dev, fport);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) arbConfig.srcFportRegularPrioGroup,
                                     (GT_VOID*) arbConfigGet.srcFportRegularPrioGroup,
                                     sizeof(arbConfig.srcFportRegularPrioGroup[0]*3))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another arbConfigPtr->srcFportRegularPrioGroup than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) arbConfig.srcFportSuperHighPrioGroup,
                                     (GT_VOID*) arbConfigGet.srcFportSuperHighPrioGroup,
                                     sizeof(arbConfig.srcFportSuperHighPrioGroup[0]*3))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another arbConfigPtr->srcFportSuperHighPrioGroup than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) arbConfig.groupRegularPrioWeight,
                                     (GT_VOID*) arbConfigGet.groupRegularPrioWeight,
                                     sizeof(arbConfig.groupRegularPrioWeight[0]*2))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another arbConfigPtr->groupRegularPrioWeight than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) arbConfig.groupSuperHighPrioWeight,
                                     (GT_VOID*) arbConfigGet.groupSuperHighPrioWeight,
                                     sizeof(arbConfig.groupSuperHighPrioWeight[0]*2))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another arbConfigPtr->groupSuperHighPrioWeight than was set: %d", dev);

        /*
            1.3. Call with out of range fport [6]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportArbiterSet(dev, fport, &arbConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.4. Call with out of range arbConfigPtr->srcFportRegularPrioGroup [0][2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        arbConfig.srcFportRegularPrioGroup[0] = 2;

        st = cpssExMxPmFabricXbarFportArbiterSet(dev, fport, &arbConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, arbConfigPtr->srcFportRegularPrioGroup = %d",
                                         dev, arbConfig.srcFportRegularPrioGroup[0]);

        arbConfig.srcFportRegularPrioGroup[0] = 1;

        /*
            1.5. Call with out of range arbConfigPtr->srcFportSuperHighPrioGroup [0][2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        arbConfig.srcFportSuperHighPrioGroup[0] = 2;

        st = cpssExMxPmFabricXbarFportArbiterSet(dev, fport, &arbConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, arbConfigPtr->srcFportSuperHighPrioGroup = %d",
                                         dev, arbConfig.srcFportSuperHighPrioGroup[0]);

        arbConfig.srcFportSuperHighPrioGroup[0] = 1;

        /*
            1.6. Call with out of range arbConfigPtr->groupRegularPrioWeight [0][256]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        arbConfig.groupRegularPrioWeight[0] = 256;

        st = cpssExMxPmFabricXbarFportArbiterSet(dev, fport, &arbConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, arbConfigPtr->groupRegularPrioWeight = %d",
                                         dev, arbConfig.groupRegularPrioWeight[0]);

        arbConfig.groupRegularPrioWeight[0] = 10;

        /*
            1.7. Call with out of range arbConfigPtr->groupSuperHighPrioWeight [0][256]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        arbConfig.groupSuperHighPrioWeight[0] = 256;

        st = cpssExMxPmFabricXbarFportArbiterSet(dev, fport, &arbConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, arbConfigPtr->groupSuperHighPrioWeight = %d",
                                         dev, arbConfig.groupSuperHighPrioWeight[0]);

        arbConfig.groupSuperHighPrioWeight[0] = 100;

        /*
            1.8. Call with arbConfigPtr->groupRegularPrioWeight [0,0],
                           arbConfigPtr->groupSuperHighPrioWeight [0,0] (invalid)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        arbConfig.groupRegularPrioWeight[0] = 0;
        arbConfig.groupRegularPrioWeight[1] = 0;

        arbConfig.groupSuperHighPrioWeight[0] = 0;
        arbConfig.groupSuperHighPrioWeight[1] = 0;

        st = cpssExMxPmFabricXbarFportArbiterSet(dev, fport, &arbConfig);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->groupRegularPrioWeight = %d, ->groupSuperHighPrioWeight = %d",
                                         dev, arbConfig.groupSuperHighPrioWeight[0], arbConfig.groupSuperHighPrioWeight[0]);

        arbConfig.groupRegularPrioWeight[0] = 1;
        arbConfig.groupRegularPrioWeight[1] = 1;

        arbConfig.groupSuperHighPrioWeight[0] = 10;
        arbConfig.groupSuperHighPrioWeight[1] = 100;

        /*
            1.9. Call with arbConfigPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportArbiterSet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, arbConfigPtr = NULL", dev);
    }

    fport = 0;

    arbConfig.srcFportRegularPrioGroup[0] = 0;
    arbConfig.srcFportRegularPrioGroup[1] = 0;
    arbConfig.srcFportRegularPrioGroup[2] = 0;

    arbConfig.srcFportSuperHighPrioGroup[0] = 0;
    arbConfig.srcFportSuperHighPrioGroup[1] = 0;
    arbConfig.srcFportSuperHighPrioGroup[2] = 0;

    arbConfig.groupRegularPrioWeight[0] = 0;
    arbConfig.groupRegularPrioWeight[1] = 0;

    arbConfig.groupSuperHighPrioWeight[0] = 10;
    arbConfig.groupSuperHighPrioWeight[1] = 100;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportArbiterSet(dev, fport, &arbConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportArbiterSet(dev, fport, &arbConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportArbiterGet
(
    IN  GT_U8                                               devNum,
    IN  GT_FPORT                                            fport,
    OUT CPSS_EXMXPM_FABRIC_XBAR_FPORT_ARBITER_CONFIG_STC   *arbConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportArbiterGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   and non-NULL arbConfigPtr.
    Expected: GT_OK.
    1.2. Call with out of range fport [6]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with arbConfigPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                            fport   = 0;
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_ARBITER_CONFIG_STC    arbConfig;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           and non-NULL arbConfigPtr.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;

        st = cpssExMxPmFabricXbarFportArbiterGet(dev, fport, &arbConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /* Call with fport [5] */
        fport   = 5;

        st = cpssExMxPmFabricXbarFportArbiterGet(dev, fport, &arbConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call with out of range fport [6]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportArbiterGet(dev, fport, &arbConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.3. Call with arbConfigPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportArbiterGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, arbConfigPtr = NULL", dev);
    }

    fport   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportArbiterGet(dev, fport, &arbConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportArbiterGet(dev, fport, &arbConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarClassArbiterSet
(
    IN  GT_U8                                                   devNum,
    IN  GT_FPORT                                                fport,
    IN  CPSS_EXMXPM_FABRIC_XBAR_CLASS_ARBITER_CONFIG_STC        *arbConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarClassArbiterSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   and arbConfigPtr {tcArbType [CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E,
                                                CPSS_EXMXPM_FABRIC_ARB_POLICY_SP_E,
                                                CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E,
                                                CPSS_EXMXPM_FABRIC_ARB_POLICY_SP_E],
                                     tcWeight [0, 1, 64, 255],
                                     superHighPrioArbType [CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E /
                                                           CPSS_EXMXPM_FABRIC_ARB_POLICY_SP_E],
                                     superHighPrioWeight [0 / 31]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricXbarClassArbiterGet with the same fport
                                                       and non-NULL arbConfigPtr.
    Expected: GT_OK and the same arbConfigPtr.
    1.3. Call with out of range fport [6]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range arbConfigPtr->tcArbType [0][0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range arbConfigPtr->tcWeight [0][256]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range arbConfigPtr->superHighPrioArbType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with out of range arbConfigPtr->superHighPrioWeight [256]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with arbConfigPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                            fport   = 0;
    GT_BOOL                                             isEqual = GT_FALSE;
    CPSS_EXMXPM_FABRIC_XBAR_CLASS_ARBITER_CONFIG_STC    arbConfig;
    CPSS_EXMXPM_FABRIC_XBAR_CLASS_ARBITER_CONFIG_STC    arbConfigGet;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           and arbConfigPtr {tcArbType [CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E,
                                                        CPSS_EXMXPM_FABRIC_ARB_POLICY_SP_E,
                                                        CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E,
                                                        CPSS_EXMXPM_FABRIC_ARB_POLICY_SP_E],
                                             tcWeight [0, 1, 64, 255],
                                             superHighPrioArbType [CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E /
                                                                   CPSS_EXMXPM_FABRIC_ARB_POLICY_SP_E],
                                             superHighPrioWeight [0 / 31]}.
            Expected: GT_OK.
        */
        /* Call with fport [0] */
        fport   = 0;

        arbConfig.tcArbType[0] = CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E;
        arbConfig.tcArbType[1] = CPSS_EXMXPM_FABRIC_ARB_POLICY_SP_E;
        arbConfig.tcArbType[2] = CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E;
        arbConfig.tcArbType[3] = CPSS_EXMXPM_FABRIC_ARB_POLICY_SP_E;

        arbConfig.tcWeight[0] = 0;
        arbConfig.tcWeight[1] = 1;
        arbConfig.tcWeight[2] = 64;
        arbConfig.tcWeight[3] = 255;

        arbConfig.superHighPrioArbType = CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E;
        arbConfig.superHighPrioWeight  = 0;

        st = cpssExMxPmFabricXbarClassArbiterSet(dev, fport, &arbConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmFabricXbarClassArbiterGet with the same fport
                                                               and non-NULL arbConfigPtr.
            Expected: GT_OK and the same arbConfigPtr.
        */
        st = cpssExMxPmFabricXbarClassArbiterGet(dev, fport, &arbConfigGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricXbarClassArbiterGet: %d, %d", dev, fport);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) arbConfig.tcArbType,
                                     (GT_VOID*) arbConfigGet.tcArbType,
                                     sizeof(arbConfig.tcArbType[0]*4))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another arbConfigPtr->tcArbType than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) arbConfig.tcWeight,
                                     (GT_VOID*) arbConfigGet.tcWeight,
                                     sizeof(arbConfig.tcWeight[0]*4))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another arbConfigPtr->tcWeight than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(arbConfig.superHighPrioArbType, arbConfigGet.superHighPrioArbType,
                   "get another arbConfigPtr->superHighPrioArbType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(arbConfig.superHighPrioWeight, arbConfigGet.superHighPrioWeight,
                   "get another arbConfigPtr->superHighPrioWeight than was set: %d", dev);

        /* Call with fport [5] */
        fport   = 5;

        arbConfig.superHighPrioArbType = CPSS_EXMXPM_FABRIC_ARB_POLICY_SP_E;
        arbConfig.superHighPrioWeight  = 31;

        st = cpssExMxPmFabricXbarClassArbiterSet(dev, fport, &arbConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmFabricXbarClassArbiterGet with the same fport
                                                               and non-NULL arbConfigPtr.
            Expected: GT_OK and the same arbConfigPtr.
        */
        st = cpssExMxPmFabricXbarClassArbiterGet(dev, fport, &arbConfigGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricXbarClassArbiterGet: %d, %d", dev, fport);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) arbConfig.tcArbType,
                                     (GT_VOID*) arbConfigGet.tcArbType,
                                     sizeof(arbConfig.tcArbType[0]*4))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another arbConfigPtr->tcArbType than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) arbConfig.tcWeight,
                                     (GT_VOID*) arbConfigGet.tcWeight,
                                     sizeof(arbConfig.tcWeight[0]*4))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another arbConfigPtr->tcWeight than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(arbConfig.superHighPrioArbType, arbConfigGet.superHighPrioArbType,
                   "get another arbConfigPtr->superHighPrioArbType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(arbConfig.superHighPrioWeight, arbConfigGet.superHighPrioWeight,
                   "get another arbConfigPtr->superHighPrioWeight than was set: %d", dev);

        /*
            1.3. Call with out of range fport [6]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarClassArbiterSet(dev, fport, &arbConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.4. Call with out of range arbConfigPtr->tcArbType [0][0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        arbConfig.tcArbType[0] = FABRIC_XBAR_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricXbarClassArbiterSet(dev, fport, &arbConfig);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, arbConfigPtr->tcArbType = %d",
                                     dev, arbConfig.tcArbType[0]);

        arbConfig.tcArbType[0] = CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E;

        /*
            1.5. Call with out of range arbConfigPtr->tcWeight [0][256]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        arbConfig.tcWeight[0] = 256;

        st = cpssExMxPmFabricXbarClassArbiterSet(dev, fport, &arbConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, arbConfigPtr->tcWeight = %d",
                                         dev, arbConfig.tcWeight[0]);

        arbConfig.tcWeight[0] = 0;

        /*
            1.6. Call with out of range arbConfigPtr->superHighPrioArbType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        arbConfig.superHighPrioArbType = FABRIC_XBAR_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricXbarClassArbiterSet(dev, fport, &arbConfig);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, arbConfigPtr->superHighPrioArbType = %d",
                                     dev, arbConfig.superHighPrioArbType);

        arbConfig.superHighPrioArbType = CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E;

        /*
            1.7. Call with out of range arbConfigPtr->superHighPrioWeight [256]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        arbConfig.superHighPrioWeight = 256;

        st = cpssExMxPmFabricXbarClassArbiterSet(dev, fport, &arbConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, arbConfigPtr->superHighPrioWeight = %d",
                                         dev, arbConfig.superHighPrioWeight);

        arbConfig.superHighPrioWeight = 0;

        /*
            1.8. Call with arbConfigPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarClassArbiterSet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, arbConfigPtr = NULL", dev);
    }

    fport   = 0;

    arbConfig.tcArbType[0] = CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E;
    arbConfig.tcArbType[1] = CPSS_EXMXPM_FABRIC_ARB_POLICY_SP_E;
    arbConfig.tcArbType[2] = CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E;
    arbConfig.tcArbType[3] = CPSS_EXMXPM_FABRIC_ARB_POLICY_SP_E;

    arbConfig.tcWeight[0] = 0;
    arbConfig.tcWeight[1] = 1;
    arbConfig.tcWeight[2] = 64;
    arbConfig.tcWeight[3] = 255;

    arbConfig.superHighPrioArbType = CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E;
    arbConfig.superHighPrioWeight  = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarClassArbiterSet(dev, fport, &arbConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarClassArbiterSet(dev, fport, &arbConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarClassArbiterGet
(
    IN  GT_U8                                               devNum,
    IN  GT_FPORT                                            fport,
    OUT CPSS_EXMXPM_FABRIC_XBAR_CLASS_ARBITER_CONFIG_STC   *arbConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarClassArbiterGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   and non-NULL arbConfigPtr.
    Expected: GT_OK.
    1.2. Call with out of range fport [6]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with arbConfigPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                            fport   = 0;
    CPSS_EXMXPM_FABRIC_XBAR_CLASS_ARBITER_CONFIG_STC    arbConfig;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           and non-NULL arbConfigPtr.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;

        st = cpssExMxPmFabricXbarClassArbiterGet(dev, fport, &arbConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /* Call with fport [5] */
        fport   = 5;

        st = cpssExMxPmFabricXbarClassArbiterGet(dev, fport, &arbConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call with out of range fport [6]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarClassArbiterGet(dev, fport, &arbConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.3. Call with arbConfigPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarClassArbiterGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, arbConfigPtr = NULL", dev);
    }

    fport   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarClassArbiterGet(dev, fport, &arbConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarClassArbiterGet(dev, fport, &arbConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarShaperSet
(
    IN  GT_U8                                       devNum,
    IN  GT_FPORT                                    fport,
    IN  CPSS_EXMXPM_FABRIC_XBAR_SHAPER_CONFIG_STC   *shaperConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarShaperSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   and shaperConfigPtr {tcCellsRate [0, 0, 0, 0 /
                                                      0, 100, 1000, 4095],
                                        superHighPrioCellsRate [0 / 0xFFF]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricXbarShaperGet with the same fport
                                                 and non-NULL shaperConfigPtr.
    Expected: GT_OK and the same shaperConfigPtr.
    1.4. Call with out of range fport [6]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range shaperConfigPtr->tcCellsRate [0][4096]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range shaperConfigPtr->superHighPrioCellsRate [0xFFFFFF + 1]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with shaperConfigPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                    fport   = 0;
    CPSS_EXMXPM_FABRIC_XBAR_SHAPER_CONFIG_STC   shaperConfig;
    CPSS_EXMXPM_FABRIC_XBAR_SHAPER_CONFIG_STC   shaperConfigGet;
    GT_BOOL                                     isEqual = GT_FALSE;


    cpssOsBzero((GT_VOID*) &shaperConfig, sizeof(shaperConfig));
    cpssOsBzero((GT_VOID*) &shaperConfigGet, sizeof(shaperConfigGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           and shaperConfigPtr {tcCellsRate [0, 0, 0, 0 / 0, 100, 1000, 4095],
                                                superHighPrioCellsRate [0 / 0xFFF]}.
            Expected: GT_OK.
        */

        /* Call with fport [5] */
        fport   = 0;

        shaperConfig.tcCellsRate[0] = 100;
        shaperConfig.tcCellsRate[1] = 100;
        shaperConfig.tcCellsRate[2] = 100;
        shaperConfig.tcCellsRate[3] = 100;

        shaperConfig.superHighPrioCellsRate = 100;

        st = cpssExMxPmFabricXbarShaperSet(dev, fport, &shaperConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmFabricXbarShaperGet with the same fport
                                                         and non-NULL shaperConfigPtr.
            Expected: GT_OK and the same shaperConfigPtr.
        */
        st = cpssExMxPmFabricXbarShaperGet(dev, fport, &shaperConfigGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricXbarShaperGet: %d, %d", dev, fport);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &shaperConfig,
                                     (GT_VOID*) &shaperConfigGet,
                                     sizeof(shaperConfig))) ? GT_TRUE : GT_FALSE;
         UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                    "get another shaperConfig then was set: %d", dev);

        /* Call with fport [5] */
        fport   = 5;

        shaperConfig.tcCellsRate[0] = 50;
        shaperConfig.tcCellsRate[1] = 100;
        shaperConfig.tcCellsRate[2] = 906;
        shaperConfig.tcCellsRate[3] = 3624;

        shaperConfig.superHighPrioCellsRate = 4118;

        st = cpssExMxPmFabricXbarShaperSet(dev, fport, &shaperConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmFabricXbarShaperGet with the same fport
                                                         and non-NULL shaperConfigPtr.
            Expected: GT_OK and the same shaperConfigPtr.
        */
        st = cpssExMxPmFabricXbarShaperGet(dev, fport, &shaperConfigGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricXbarShaperGet: %d, %d", dev, fport);

        /*
            1.4. Call with out of range fport [6]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarShaperSet(dev, fport, &shaperConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.5. Call with out of range shaperConfigPtr->tcCellsRate [0][4096]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        shaperConfig.tcCellsRate[0] = 49 / 255 * 4;

        st = cpssExMxPmFabricXbarShaperSet(dev, fport, &shaperConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, shaperConfigPtr->tcCellsRate[0] = %d",
                                         dev, shaperConfig.tcCellsRate[0]);

        shaperConfig.tcCellsRate[0] = 0;

        /*
            1.6. Call with out of range shaperConfigPtr->superHighPrioCellsRate [0xFFFFFF + 1]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        shaperConfig.superHighPrioCellsRate = 0xFFFFFF + 1;

        st = cpssExMxPmFabricXbarShaperSet(dev, fport, &shaperConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, shaperConfigPtr->superHighPrioCellsRate = %d",
                                         dev, shaperConfig.superHighPrioCellsRate);

        shaperConfig.superHighPrioCellsRate = 0;

        /*
            1.7. Call with shaperConfigPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarShaperSet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, shaperConfigPtr = NULL", dev);
    }

    fport   = 0;

    shaperConfig.tcCellsRate[0] = 0;
    shaperConfig.tcCellsRate[1] = 0;
    shaperConfig.tcCellsRate[2] = 0;
    shaperConfig.tcCellsRate[3] = 0;

    shaperConfig.superHighPrioCellsRate = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarShaperSet(dev, fport, &shaperConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarShaperSet(dev, fport, &shaperConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarShaperGet
(
    IN  GT_U8                                       devNum,
    IN  GT_FPORT                                    fport,
    OUT CPSS_EXMXPM_FABRIC_XBAR_SHAPER_CONFIG_STC   *shaperConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarShaperGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   and non-NULL shaperConfigPtr.
    Expected: GT_OK.
    1.2. Call with out of range fport [1 / 3]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with shaperConfigPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                    fport   = 0;
    CPSS_EXMXPM_FABRIC_XBAR_SHAPER_CONFIG_STC   shaperConfig;


    cpssOsBzero((GT_VOID*) &shaperConfig, sizeof(shaperConfig));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           and non-NULL shaperConfigPtr.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport = 0;

        st = cpssExMxPmFabricXbarShaperGet(dev, fport, &shaperConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /* call with fport [5] */
        fport = 5;

        st = cpssExMxPmFabricXbarShaperGet(dev, fport, &shaperConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call with out of range fport [6]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarShaperGet(dev, fport, &shaperConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.3. Call with shaperConfigPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarShaperGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, shaperConfigPtr = NULL", dev);
    }

    fport   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarShaperGet(dev, fport, &shaperConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarShaperGet(dev, fport, &shaperConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportFifoStatusGet
(
    IN  GT_U8                                           devNum,
    IN  GT_FPORT                                        fport,
    OUT CPSS_EXMXPM_FABRIC_XBAR_FPORT_FIFO_STATUS_STC   *fportFifoStatusPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportFifoStatusGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   and non-NULL fportFifoStatusPtr.
    Expected: GT_OK.
    1.2. Call with out of range fport [6]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with fportFifoStatusPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                        fport   = 0;
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_FIFO_STATUS_STC   fportFifoStatus;


    cpssOsBzero((GT_VOID*) &fportFifoStatus, sizeof(fportFifoStatus));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           and non-NULL fportFifoStatusPtr.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport = 0;

        st = cpssExMxPmFabricXbarFportFifoStatusGet(dev, fport, &fportFifoStatus);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /* Call with fport [5] */
        fport = 5;

        st = cpssExMxPmFabricXbarFportFifoStatusGet(dev, fport, &fportFifoStatus);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call with out of range fport [6]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportFifoStatusGet(dev, fport, &fportFifoStatus);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.3. Call with fportFifoStatusPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportFifoStatusGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fportFifoStatusPtr = NULL", dev);
    }

    fport = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportFifoStatusGet(dev, fport, &fportFifoStatus);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportFifoStatusGet(dev, fport, &fportFifoStatus);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportCellCntrTypeSet
(
    IN  GT_U8                             devNum,
    IN  GT_FPORT                          fport,
    IN  CPSS_EXMXPM_FABRIC_DIR_ENT        cellDirection,
    IN  CPSS_EXMXPM_FABRIC_CELL_TYPE_ENT  cellTypeBitmap
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportCellCntrTypeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5],
                   cellDirection [CPSS_EXMXPM_FABRIC_DIR_RX_E /
                                  CPSS_EXMXPM_FABRIC_DIR_TX_E]
                   and cellTypeBitmap [CPSS_EXMXPM_FABRIC_CELL_TYPE_DIAG_E /
                                       CPSS_EXMXPM_FABRIC_CELL_TYPE_BC_E /
                                       CPSS_EXMXPM_FABRIC_CELL_TYPE_MC_E /
                                       CPSS_EXMXPM_FABRIC_CELL_TYPE_UC_E /
                                       CPSS_EXMXPM_FABRIC_CELL_TYPE_ANY_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricXbarFportCellCntrTypeGet with the same fport,
                                                            cellDirection
                                                            and non-NULL cellTypeBitmapPtr.
    Expected: GT_OK and the same cellTypeBitmap.
    1.3. Call with out of range fport [63]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                          fport             = 0;
    CPSS_EXMXPM_FABRIC_DIR_ENT        cellDirection     = CPSS_EXMXPM_FABRIC_DIR_RX_E;
    CPSS_EXMXPM_FABRIC_CELL_TYPE_ENT  cellTypeBitmap    = CPSS_EXMXPM_FABRIC_CELL_TYPE_BC_E;
    GT_U32                            cellTypeBitmapGet = CPSS_EXMXPM_FABRIC_CELL_TYPE_BC_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5],
                           cellDirection [CPSS_EXMXPM_FABRIC_DIR_RX_E /
                                          CPSS_EXMXPM_FABRIC_DIR_TX_E]
                           and cellTypeBitmap [CPSS_EXMXPM_FABRIC_CELL_TYPE_DIAG_E /
                                               CPSS_EXMXPM_FABRIC_CELL_TYPE_BC_E /
                                               CPSS_EXMXPM_FABRIC_CELL_TYPE_MC_E /
                                               CPSS_EXMXPM_FABRIC_CELL_TYPE_UC_E /
                                               CPSS_EXMXPM_FABRIC_CELL_TYPE_ANY_E].
            Expected: GT_OK.
        */
        /* Call with cellTypeBitmap [CPSS_EXMXPM_FABRIC_CELL_TYPE_DIAG_E] */
        cellTypeBitmap = CPSS_EXMXPM_FABRIC_CELL_TYPE_DIAG_E;

        st = cpssExMxPmFabricXbarFportCellCntrTypeSet(dev, fport,
                                                      cellDirection, cellTypeBitmap);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, cellDirection, cellTypeBitmap);

        /*
            1.2. Call cpssExMxPmFabricXbarFportCellCntrTypeGet with the same fport,
                                                                    cellDirection
                                                                    and non-NULL cellTypeBitmapPtr.
            Expected: GT_OK and the same cellTypeBitmap.
        */
        st = cpssExMxPmFabricXbarFportCellCntrTypeGet(dev, fport,
                                                      cellDirection, (GT_U32*)&cellTypeBitmapGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricXbarFportCellCntrTypeGet: %d, %d, %d", dev, fport, cellDirection);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cellTypeBitmap, cellTypeBitmapGet,
                   "get another cellTypeBitmap then was set: %d", dev);

        /* Call with cellTypeBitmap [CPSS_EXMXPM_FABRIC_CELL_TYPE_BC_E] */
        cellTypeBitmap = CPSS_EXMXPM_FABRIC_CELL_TYPE_BC_E;

        st = cpssExMxPmFabricXbarFportCellCntrTypeSet(dev, fport,
                                                      cellDirection, cellTypeBitmap);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, cellDirection, cellTypeBitmap);

        /*
            1.2. Call cpssExMxPmFabricXbarFportCellCntrTypeGet with the same fport,
                                                                    cellDirection
                                                                    and non-NULL cellTypeBitmapPtr.
            Expected: GT_OK and the same cellTypeBitmap.
        */
        st = cpssExMxPmFabricXbarFportCellCntrTypeGet(dev, fport,
                                                      cellDirection, (GT_U32*)&cellTypeBitmapGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricXbarFportCellCntrTypeGet: %d, %d, %d", dev, fport, cellDirection);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cellTypeBitmap, cellTypeBitmapGet,
                   "get another cellTypeBitmap then was set: %d", dev);

        /* Call with cellTypeBitmap [CPSS_EXMXPM_FABRIC_CELL_TYPE_MC_E] */
        fport          = 5;
        cellDirection  = CPSS_EXMXPM_FABRIC_DIR_TX_E;
        cellTypeBitmap = CPSS_EXMXPM_FABRIC_CELL_TYPE_MC_E;

        st = cpssExMxPmFabricXbarFportCellCntrTypeSet(dev, fport,
                                                      cellDirection, cellTypeBitmap);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, cellDirection, cellTypeBitmap);

        /*
            1.2. Call cpssExMxPmFabricXbarFportCellCntrTypeGet with the same fport,
                                                                    cellDirection
                                                                    and non-NULL cellTypeBitmapPtr.
            Expected: GT_OK and the same cellTypeBitmap.
        */
        st = cpssExMxPmFabricXbarFportCellCntrTypeGet(dev, fport,
                                                      cellDirection, (GT_U32*)&cellTypeBitmapGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricXbarFportCellCntrTypeGet: %d, %d, %d", dev, fport, cellDirection);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cellTypeBitmap, cellTypeBitmapGet,
                   "get another cellTypeBitmap then was set: %d", dev);

        /* Call with cellTypeBitmap [CPSS_EXMXPM_FABRIC_CELL_TYPE_UC_E] */
        cellTypeBitmap = CPSS_EXMXPM_FABRIC_CELL_TYPE_UC_E;

        st = cpssExMxPmFabricXbarFportCellCntrTypeSet(dev, fport,
                                                      cellDirection, cellTypeBitmap);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, cellDirection, cellTypeBitmap);

        /*
            1.2. Call cpssExMxPmFabricXbarFportCellCntrTypeGet with the same fport,
                                                                    cellDirection
                                                                    and non-NULL cellTypeBitmapPtr.
            Expected: GT_OK and the same cellTypeBitmap.
        */
        st = cpssExMxPmFabricXbarFportCellCntrTypeGet(dev, fport,
                                                      cellDirection, (GT_U32*)&cellTypeBitmapGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricXbarFportCellCntrTypeGet: %d, %d, %d", dev, fport, cellDirection);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cellTypeBitmap, cellTypeBitmapGet,
                   "get another cellTypeBitmap then was set: %d", dev);

        /* Call with cellTypeBitmap [CPSS_EXMXPM_FABRIC_CELL_TYPE_ANY_E] */
        cellTypeBitmap = CPSS_EXMXPM_FABRIC_CELL_TYPE_ANY_E;

        st = cpssExMxPmFabricXbarFportCellCntrTypeSet(dev, fport,
                                                      cellDirection, cellTypeBitmap);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, cellDirection, cellTypeBitmap);

        /*
            1.2. Call cpssExMxPmFabricXbarFportCellCntrTypeGet with the same fport,
                                                                    cellDirection
                                                                    and non-NULL cellTypeBitmapPtr.
            Expected: GT_OK and the same cellTypeBitmap.
        */
        st = cpssExMxPmFabricXbarFportCellCntrTypeGet(dev, fport,
                                                      cellDirection, (GT_U32*)&cellTypeBitmapGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricXbarFportCellCntrTypeGet: %d, %d, %d", dev, fport, cellDirection);

        /* Verifying values */

        /* clear bit 0 - it always 0 in get*/
        cellTypeBitmap &= (~1);

        UTF_VERIFY_EQUAL1_STRING_MAC(cellTypeBitmap, cellTypeBitmapGet,
                   "get another cellTypeBitmap then was set: %d", dev);

        /*
            1.3. Call with out of range fport [6]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportCellCntrTypeSet(dev, fport,
                                                      cellDirection, cellTypeBitmap);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);
    }

    fport          = 2;
    cellDirection  = CPSS_EXMXPM_FABRIC_DIR_TX_E;
    cellTypeBitmap = CPSS_EXMXPM_FABRIC_CELL_TYPE_ANY_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportCellCntrTypeSet(dev, fport,
                                                      cellDirection, cellTypeBitmap);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportCellCntrTypeSet(dev, fport,
                                                  cellDirection, cellTypeBitmap);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportCellCntrTypeGet
(
    IN  GT_U8                             devNum,
    IN  GT_FPORT                          fport,
    IN  CPSS_EXMXPM_FABRIC_DIR_ENT        cellDirection,
    OUT GT_U32                           *cellTypeBitmapPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportCellCntrTypeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5],
                   cellDirection [CPSS_EXMXPM_FABRIC_DIR_RX_E /
                                  CPSS_EXMXPM_FABRIC_DIR_TX_E]
                   and non-NULL cellTypeBitmapPtr.
    Expected: GT_OK.
    1.2. Call with out of range fport [6]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with cellTypeBitmapPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                    fport          = 0;
    CPSS_EXMXPM_FABRIC_DIR_ENT  cellDirection  = CPSS_EXMXPM_FABRIC_DIR_RX_E;
    GT_U32                      cellTypeBitmap = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5],
                           cellDirection [CPSS_EXMXPM_FABRIC_DIR_RX_E /
                                          CPSS_EXMXPM_FABRIC_DIR_TX_E]
                           and non-NULL cellTypeBitmapPtr.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport          = 0;
        cellDirection  = CPSS_EXMXPM_FABRIC_DIR_RX_E;

        st = cpssExMxPmFabricXbarFportCellCntrTypeGet(dev, fport,
                                                      cellDirection, &cellTypeBitmap);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, cellDirection);

        /* Call with fport [5] */
        fport          = 5;
        cellDirection  = CPSS_EXMXPM_FABRIC_DIR_TX_E;

        st = cpssExMxPmFabricXbarFportCellCntrTypeGet(dev, fport,
                                                      cellDirection, &cellTypeBitmap);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, cellDirection);

        /*
            1.2. Call with out of range fport [6]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportCellCntrTypeGet(dev, fport,
                                                      cellDirection, &cellTypeBitmap);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.3. Call with cellTypeBitmapPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportCellCntrTypeGet(dev, fport,
                                                      cellDirection, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cellTypeBitmapPtr = NULL", dev);
    }

    fport          = 0;
    cellDirection  = CPSS_EXMXPM_FABRIC_DIR_RX_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportCellCntrTypeGet(dev, fport,
                                                cellDirection, &cellTypeBitmap);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportCellCntrTypeGet(dev, fport,
                                                cellDirection, &cellTypeBitmap);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportCellCntrGet
(
    IN  GT_U8                             devNum,
    IN  GT_FPORT                          fport,
    IN  CPSS_EXMXPM_FABRIC_DIR_ENT        cellDirection,
    OUT GT_U32                            *cellCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportCellCntrGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5],
                   cellDirection [CPSS_EXMXPM_FABRIC_DIR_RX_E /
                                  CPSS_EXMXPM_FABRIC_DIR_TX_E]
                   and non-NULL cellCntrPtr.
    Expected: GT_OK.
    1.2. Call with out of range fport [6]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with cellCntrPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                          fport         = 0;
    CPSS_EXMXPM_FABRIC_DIR_ENT        cellDirection = CPSS_EXMXPM_FABRIC_DIR_RX_E;
    GT_U32                            cellCntr      = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5],
                           cellDirection [CPSS_EXMXPM_FABRIC_DIR_RX_E /
                                          CPSS_EXMXPM_FABRIC_DIR_TX_E]
                           and non-NULL cellCntrPtr.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport         = 0;
        cellDirection = CPSS_EXMXPM_FABRIC_DIR_RX_E;

        st = cpssExMxPmFabricXbarFportCellCntrGet(dev, fport,
                                                  cellDirection, &cellCntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, cellDirection);

        /* Call with fport [5] */
        fport         = 5;
        cellDirection = CPSS_EXMXPM_FABRIC_DIR_TX_E;

        st = cpssExMxPmFabricXbarFportCellCntrGet(dev, fport,
                                                  cellDirection, &cellCntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, cellDirection);

        /*
            1.2. Call with out of range fport [6]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportCellCntrGet(dev, fport,
                                                  cellDirection, &cellCntr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.3. Call with cellCntrPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportCellCntrGet(dev, fport,
                                                  cellDirection, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cellCntrPtr = NULL", dev);
    }

    fport         = 0;
    cellDirection = CPSS_EXMXPM_FABRIC_DIR_RX_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportCellCntrGet(dev, fport,
                                                  cellDirection, &cellCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportCellCntrGet(dev, fport,
                                                  cellDirection, &cellCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportTxFifoDroppedCellsCntrGet
(
    IN  GT_U8                             devNum,
    IN  GT_FPORT                          fport,
    IN  GT_U32                            fifo,
    OUT GT_U32                            *superHighPriCellsCntrPtr,
    OUT GT_U32                            *regularCellCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportTxFifoDroppedCellsCntrGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5],
                   fifo [0 / 5]
                   and non-NULL superHighPriCellsCntrPtr,
                   non-NULL regularCellCntrPtr.
    Expected: GT_OK.
    1.2. Call with out of range fport [6]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range fifo [6]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with superHighPriCellsCntrPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with regularCellCntrPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT    fport                 = 0;
    GT_U32      fifo                  = 0;
    GT_U32      superHighPriCellsCntr = 0;
    GT_U32      regularCellCntr       = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5],
                           fifo [0 / 5]
                           and non-NULL superHighPriCellsCntrPtr,
                           non-NULL regularCellCntrPtr.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;
        fifo    = 0;

        st = cpssExMxPmFabricXbarFportTxFifoDroppedCellsCntrGet(dev, fport, fifo,
                                                                &superHighPriCellsCntr, &regularCellCntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, fifo);

        /* Call with fport [5] */
        fport   = 5;
        fifo    = 5;

        st = cpssExMxPmFabricXbarFportTxFifoDroppedCellsCntrGet(dev, fport, fifo,
                                                                &superHighPriCellsCntr, &regularCellCntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, fifo);

        /*
            1.2. Call with out of range fport [6]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportTxFifoDroppedCellsCntrGet(dev, fport, fifo,
                                                                &superHighPriCellsCntr, &regularCellCntr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.3. Call with out of range fifo [6]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fifo = 6;

        st = cpssExMxPmFabricXbarFportTxFifoDroppedCellsCntrGet(dev, fport, fifo,
                                                                &superHighPriCellsCntr, &regularCellCntr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fifo = %d", dev, fifo);

        fifo = 0;

        /*
            1.4. Call with superHighPriCellsCntrPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportTxFifoDroppedCellsCntrGet(dev, fport, fifo,
                                                                NULL, &regularCellCntr);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, superHighPriCellsCntrPtr = NULL", dev);

        /*
            1.5. Call with regularCellCntrPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportTxFifoDroppedCellsCntrGet(dev, fport, fifo,
                                                                &superHighPriCellsCntr, NULL);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, regularCellCntrPtr = NULL", dev);
    }

    fport   = 0;
    fifo    = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportTxFifoDroppedCellsCntrGet(dev, fport, fifo,
                                                                &superHighPriCellsCntr, &regularCellCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportTxFifoDroppedCellsCntrGet(dev, fport, fifo,
                                                            &superHighPriCellsCntr, &regularCellCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportRxDroppedCellsCntrsGet
(
    IN  GT_U8                             devNum,
    IN  GT_FPORT                          fport,
    OUT GT_U32                            *invalidTargetCntrPtr,
    OUT GT_U32                            *rxFifoOverrunCntrPtr,
    OUT GT_U32                            *ownDevFilterCntrPtr

)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportRxDroppedCellsCntrsGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   and non-NULL invalidTargetCntrPtr,
                   non-NULL rxFifoOverrunCntrPtr,
                   non-NULL ownDevFilterCntrPtr.
    Expected: GT_OK.
    1.2. Call with out of range fport [6]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with superHighPriCellsCntrPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with regularCellCntrPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with ownDevFilterCntrPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT    fport             = 0;
    GT_U32      invalidTargetCntr = 0;
    GT_U32      rxFifoOverrunCntr = 0;
    GT_U32      ownDevFilterCntr  = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           and non-NULL invalidTargetCntrPtr,
                           non-NULL rxFifoOverrunCntrPtr,
                           non-NULL ownDevFilterCntrPtr.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;

        st = cpssExMxPmFabricXbarFportRxDroppedCellsCntrsGet(dev, fport,
                      &invalidTargetCntr, &rxFifoOverrunCntr, &ownDevFilterCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /* Call with fport [5] */
        fport   = 5;

        st = cpssExMxPmFabricXbarFportRxDroppedCellsCntrsGet(dev, fport,
                      &invalidTargetCntr, &rxFifoOverrunCntr, &ownDevFilterCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call with out of range fport [6]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportRxDroppedCellsCntrsGet(dev, fport,
                      &invalidTargetCntr, &rxFifoOverrunCntr, &ownDevFilterCntr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.3. Call with invalidTargetCntrPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportRxDroppedCellsCntrsGet(dev, fport,
                      NULL, &rxFifoOverrunCntr, &ownDevFilterCntr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, invalidTargetCntrPtr = NULL", dev);

        /*
            1.4. Call with rxFifoOverrunCntrPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportRxDroppedCellsCntrsGet(dev, fport,
                      &invalidTargetCntr, NULL, &ownDevFilterCntr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, rxFifoOverrunCntrPtr = NULL", dev);

        /*
            1.5. Call with ownDevFilterCntrPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportRxDroppedCellsCntrsGet(dev, fport,
                      &invalidTargetCntr, &rxFifoOverrunCntr, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ownDevFilterCntrPtr = NULL", dev);
    }

    fport   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportRxDroppedCellsCntrsGet(dev, fport,
                      &invalidTargetCntr, &rxFifoOverrunCntr, &ownDevFilterCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportRxDroppedCellsCntrsGet(dev, fport,
                      &invalidTargetCntr, &rxFifoOverrunCntr, &ownDevFilterCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportMulticastCellSwitchingSet
(
    IN  GT_U8                                  devNum,
    IN  CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP_ENT  fportGroup,
    IN  GT_U16                                 vidx,
    IN  GT_U32                                 destPortBmp
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportMulticastCellSwitchingSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fportGroup [CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP0_E / CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP1_E],
                   vidx [0 / 4095],
                   destPortBmp [0 / 31]
    Expected: GT_OK.
    1.2. Call with cpssExMxPmFabricXbarFportMulticastCellSwitchingGet with not NULL destPortBmpPtr
    Expected: GT_OK and the same destPortBmpPtr as was set.
    1.3. Call with vidx [4096] and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with destPortBmp [64] and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP_ENT  fportGroup     = CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP0_E;
    GT_U16                                 vidx           = 0;
    GT_U32                                 destPortBmp    = 0;
    GT_U32                                 destPortBmpGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fportGroup [CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP0_E / CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP1_E],
                           vidx [0 / 4095],
                           destPortBmp [0 / 31]
            Expected: GT_OK.
        */
        /* iterate with vidx = 0 */
        fportGroup  = CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP0_E;
        vidx        = 0;
        destPortBmp = 0;

        st = cpssExMxPmFabricXbarFportMulticastCellSwitchingSet(dev, fportGroup, vidx, destPortBmp);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fportGroup, vidx, destPortBmp);

        /*
            1.2. Call with cpssExMxPmFabricXbarFportMulticastCellSwitchingGet with not NULL destPortBmpPtr
            Expected: GT_OK and the same destPortBmpPtr as was set.
        */
        st = cpssExMxPmFabricXbarFportMulticastCellSwitchingGet(dev, fportGroup, vidx, &destPortBmpGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmFabricXbarFportMulticastCellSwitchingGet: %d, %d, %d", dev, fportGroup, vidx);
        UTF_VERIFY_EQUAL1_STRING_MAC(destPortBmp, destPortBmpGet, "got another destPortBmp then was set: %d", dev);

        /* iterate with vidx = 0 */
        fportGroup  = CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP1_E;
        vidx        = (GT_U16)((PRV_CPSS_EXMXPM_DEV_TBLS_MAC(dev)->XBar.integratedCrossbarMcGroupConfig0).numOfEntries - 1);
        destPortBmp = 31;

        st = cpssExMxPmFabricXbarFportMulticastCellSwitchingSet(dev, fportGroup, vidx, destPortBmp);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fportGroup, vidx, destPortBmp);

        /*
            1.2. Call with cpssExMxPmFabricXbarFportMulticastCellSwitchingGet with not NULL destPortBmpPtr
            Expected: GT_OK and the same destPortBmpPtr as was set.
        */
        st = cpssExMxPmFabricXbarFportMulticastCellSwitchingGet(dev, fportGroup, vidx, &destPortBmpGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmFabricXbarFportMulticastCellSwitchingGet: %d, %d, %d", dev, fportGroup, vidx);
        UTF_VERIFY_EQUAL1_STRING_MAC(destPortBmp, destPortBmpGet, "got another destPortBmp then was set: %d", dev);

        /*
            1.3. Call with vidx [4096] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        vidx = 4096;

        st = cpssExMxPmFabricXbarFportMulticastCellSwitchingSet(dev, fportGroup, vidx, destPortBmp);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, vidx = %d", dev, vidx);

        vidx = 0;

        /*
            1.4. Call with destPortBmp [64] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        destPortBmp = 64;

        st = cpssExMxPmFabricXbarFportMulticastCellSwitchingSet(dev, fportGroup, vidx, destPortBmp);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, destPortBmp = %d", dev, destPortBmp);
    }

    fportGroup  = CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP0_E;
    vidx        = 0;
    destPortBmp = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportMulticastCellSwitchingSet(dev, fportGroup, vidx, destPortBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportMulticastCellSwitchingSet(dev, fportGroup, vidx, destPortBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportMulticastCellSwitchingGet
(
    IN  GT_U8                                  devNum,
    IN  CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP_ENT  fportGroup,
    IN  GT_U16                                 vidx,
    OUT GT_U32                                 *destPortBmpPtr

)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportMulticastCellSwitchingGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fportGroup [CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP0_E / CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP1_E],
                   vidx [0 / 4095],
                   not NULL destPortBmpPtr
    Expected: GT_OK.
    1.2. Call with vidx [4096] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with destPortBmpPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP_ENT  fportGroup  = CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP0_E;
    GT_U16                                 vidx        = 0;
    GT_U32                                 destPortBmp = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fportGroup [CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP0_E / CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP1_E],
                           vidx [0 / 4095],
                           not NULL destPortBmpPtr
            Expected: GT_OK.
        */
        /* iterate with vidx = 0 */
        fportGroup = CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP0_E;
        vidx       = 0;

        st = cpssExMxPmFabricXbarFportMulticastCellSwitchingGet(dev, fportGroup, vidx, &destPortBmp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fportGroup, vidx);

        /* iterate with vidx = 4095 */
        fportGroup = CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP1_E;
        vidx       = (GT_U16)((PRV_CPSS_EXMXPM_DEV_TBLS_MAC(dev)->XBar.integratedCrossbarMcGroupConfig0).numOfEntries - 1);

        st = cpssExMxPmFabricXbarFportMulticastCellSwitchingGet(dev, fportGroup, vidx, &destPortBmp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fportGroup, vidx);

        /*
            1.2. Call with vidx [4096] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        vidx = 4096;

        st = cpssExMxPmFabricXbarFportMulticastCellSwitchingGet(dev, fportGroup, vidx, &destPortBmp);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, vidx = %d", dev, vidx);

        vidx = 0;

        /*
            1.3. Call with destPortBmpPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportMulticastCellSwitchingGet(dev, fportGroup, vidx, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, destPortBmpPtr = NULL", dev);
    }

    fportGroup = CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP0_E;
    vidx       = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportMulticastCellSwitchingGet(dev, fportGroup, vidx, &destPortBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportMulticastCellSwitchingGet(dev, fportGroup, vidx, &destPortBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportUnicastCellSwitchingSet
(
    IN  GT_U8     devNum,
    IN  GT_FPORT  fport,
    IN  GT_U8     destDevNum,
    IN  GT_U32    lbh,
    IN  GT_BOOL   lbhEnable,
    IN  GT_U32    destPort
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportUnicastCellSwitchingSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   destDevNum [0 / 127]
                   lbh [0 / 3]
                   lbhEnable [GT_TRUE / GT_FALSE]
                   destPort [0 / 5]
    Expected: GT_OK.
    1.2. Call with cpssExMxPmFabricXbarFportUnicastCellSwitchingGet with not NULL lbhEnablePtr, destPortPtr
    Expected: GT_OK and the same lbhEnablePtr and destPortPtr as was set.
    1.3. Call with fport [6] and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with destDevNum [128] and other params from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with lbh [4] and other params from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with destPort [6] and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_FPORT  fport      = 0;
    GT_U8     destDevNum = 0;
    GT_U32    lbh        = 0;
    GT_BOOL   lbhEnable  = GT_FALSE;
    GT_U32    destPort   = 0;

    GT_BOOL   lbhEnableGet = GT_FALSE;
    GT_U32    destPortGet  = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           destDevNum [0 / 127]
                           lbh [0 / 3]
                           lbhEnable [GT_TRUE / GT_FALSE]
                           destPort [0 / 5]
            Expected: GT_OK.
        */
        /* iterate with fport = 0 */
        fport = 0;
        destDevNum = 0;
        lbh = 0;
        lbhEnable = GT_FALSE;

        st = cpssExMxPmFabricXbarFportUnicastCellSwitchingSet(dev, fport, destDevNum, lbh, lbhEnable, destPort);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, fport, destDevNum, lbh, lbhEnable, destPort);

        /*
            1.2. Call with cpssExMxPmFabricXbarFportUnicastCellSwitchingGet with not NULL lbhEnablePtr, destPortPtr
            Expected: GT_OK and the same lbhEnablePtr and destPortPtr as was set.
        */
        st = cpssExMxPmFabricXbarFportUnicastCellSwitchingGet(dev, fport, destDevNum, lbh, &lbhEnableGet, &destPortGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmFabricXbarFportUnicastCellSwitchingGet: %d, %d, %d, %d",
                                     dev, fport, destDevNum, lbh);

        UTF_VERIFY_EQUAL1_STRING_MAC(lbhEnable, lbhEnableGet, "got another lbhEnable then wa sset: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(destPort, destPortGet, "got another destPort then wa sset: %d", dev);

        /* iterate with fport = 5 */
        fport = 5;
        destDevNum = dev;
        lbh = 3;
        lbhEnable = GT_TRUE;

        st = cpssExMxPmFabricXbarFportUnicastCellSwitchingSet(dev, fport, destDevNum, lbh, lbhEnable, destPort);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, fport, destDevNum, lbh, lbhEnable, destPort);

        /*
            1.2. Call with cpssExMxPmFabricXbarFportUnicastCellSwitchingGet with not NULL lbhEnablePtr, destPortPtr
            Expected: GT_OK and the same lbhEnablePtr and destPortPtr as was set.
        */
        st = cpssExMxPmFabricXbarFportUnicastCellSwitchingGet(dev, fport, destDevNum, lbh, &lbhEnableGet, &destPortGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmFabricXbarFportUnicastCellSwitchingGet: %d, %d, %d, %d",
                                     dev, fport, destDevNum, lbh);

        UTF_VERIFY_EQUAL1_STRING_MAC(lbhEnable, lbhEnableGet, "got another lbhEnable then wa sset: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(destPort, destPortGet, "got another destPort then wa sset: %d", dev);

        /*
            1.3. Call with fport [6] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportUnicastCellSwitchingSet(dev, fport, destDevNum, lbh, lbhEnable, destPort);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 0;

        /*
            1.4. Call with destDevNum [128] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        destDevNum = 128;

        st = cpssExMxPmFabricXbarFportUnicastCellSwitchingSet(dev, fport, destDevNum, lbh, lbhEnable, destPort);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, destDevNum = %d", dev, destDevNum);

        destDevNum = 0;

        /*
            1.5. Call with lbh [4] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        lbh = 4;

        st = cpssExMxPmFabricXbarFportUnicastCellSwitchingSet(dev, fport, destDevNum, lbh, lbhEnable, destPort);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, lbh = %d", dev, lbh);

        lbh = 0;

        /*
            1.6. Call with destPort [6] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        destPort = 6;

        st = cpssExMxPmFabricXbarFportUnicastCellSwitchingSet(dev, fport, destDevNum, lbh, lbhEnable, destPort);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, destPort = %d", dev, destPort);
    }

    fport = 0;
    destDevNum = 0;
    lbh = 0;
    lbhEnable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportUnicastCellSwitchingSet(dev, fport, destDevNum, lbh, lbhEnable, destPort);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportUnicastCellSwitchingSet(dev, fport, destDevNum, lbh, lbhEnable, destPort);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportUnicastCellSwitchingGet
(
    IN  GT_U8     devNum,
    IN  GT_FPORT  fport,
    IN  GT_U8     destDevNum,
    IN  GT_U32    lbh,
    OUT GT_BOOL   *lbhEnablePtr,
    OUT GT_U32    *destPortPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportUnicastCellSwitchingGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   destDevNum [0 / 127]
                   lbh [0 / 3]
                   not NULL lbhEnable, destPort
    Expected: GT_OK.
    1.2. Call with fport [6] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with destDevNum [128] and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with lbh [4] and other params from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with lbhEnablePtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.6. Call with destPortPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_FPORT  fport      = 0;
    GT_U8     destDevNum = 0;
    GT_U32    lbh        = 0;
    GT_BOOL   lbhEnable  = GT_FALSE;
    GT_U32    destPort   = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           destDevNum [0 / 127]
                           lbh [0 / 3]
                           not NULL lbhEnable, destPort
            Expected: GT_OK.
        */
        fport = 0;
        destDevNum = 0;
        lbh = 0;

        st = cpssExMxPmFabricXbarFportUnicastCellSwitchingGet(dev, fport, destDevNum, lbh, &lbhEnable, &destPort);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, destDevNum, lbh);

        fport = 5;
        destDevNum = dev;
        lbh = 3;

        st = cpssExMxPmFabricXbarFportUnicastCellSwitchingGet(dev, fport, destDevNum, lbh, &lbhEnable, &destPort);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, destDevNum, lbh);

        /*
            1.2. Call with fport [6] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportUnicastCellSwitchingGet(dev, fport, destDevNum, lbh, &lbhEnable, &destPort);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 0;

        /*
            1.3. Call with destDevNum [128] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        destDevNum = 128;

        st = cpssExMxPmFabricXbarFportUnicastCellSwitchingGet(dev, fport, destDevNum, lbh, &lbhEnable, &destPort);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, destDevNum = %d", dev, destDevNum);

        destDevNum = 0;

        /*
            1.4. Call with lbh [4] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        lbh = 4;

        st = cpssExMxPmFabricXbarFportUnicastCellSwitchingGet(dev, fport, destDevNum, lbh, &lbhEnable, &destPort);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, lbh = %d", dev, lbh);

        lbh = 0;

        /*
            1.5. Call with lbhEnablePtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportUnicastCellSwitchingGet(dev, fport, destDevNum, lbh, NULL, &destPort);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, lbhEnablePtr = NULL", dev);

        /*
            1.6. Call with destPortPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportUnicastCellSwitchingGet(dev, fport, destDevNum, lbh, &lbhEnable, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, destPortPtr = NULL", dev);
    }

    fport = 0;
    destDevNum = 0;
    lbh = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportUnicastCellSwitchingGet(dev, fport, destDevNum, lbh, &lbhEnable, &destPort);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportUnicastCellSwitchingGet(dev, fport, destDevNum, lbh, &lbhEnable, &destPort);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportDiagnosticCellSwitchingSet
(
    IN  GT_U8     devNum,
    IN  GT_FPORT  fport,
    IN  GT_U32    destPortBmp
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportDiagnosticCellSwitchingSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   destPortBmp [0 / 31]
    Expected: GT_OK.
    1.2. Call with cpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet with not NULL destPortBmpPtr
    Expected: GT_OK and the same destPortBmpPtr as was set.
    1.3. Call with fport [6] and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with destPortBmp [64] and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_FPORT  fport          = 0;
    GT_U32    destPortBmp    = 0;
    GT_U32    destPortBmpGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           destPortBmp [0 / 31]
            Expected: GT_OK.
        */
        /* iterate with fport = 0 */
        fport       = 0;
        destPortBmp = 0;

        st = cpssExMxPmFabricXbarFportDiagnosticCellSwitchingSet(dev, fport, destPortBmp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, destPortBmp);

        /*
            1.2. Call with cpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet with not NULL destPortBmpPtr
            Expected: GT_OK and the same destPortBmpPtr as was set.
        */
        st = cpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet(dev, fport, &destPortBmpGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet: %d, %d", dev, fport);
        UTF_VERIFY_EQUAL1_STRING_MAC(destPortBmp, destPortBmpGet, "got another destPortBmp then was set: %d", dev);

        /* iterate with fport = 5 */
        fport       = 5;
        destPortBmp = 31;

        st = cpssExMxPmFabricXbarFportDiagnosticCellSwitchingSet(dev, fport, destPortBmp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, destPortBmp);

        /*
            1.2. Call with cpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet with not NULL destPortBmpPtr
            Expected: GT_OK and the same destPortBmpPtr as was set.
        */
        st = cpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet(dev, fport, &destPortBmpGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet: %d, %d", dev, fport);
        UTF_VERIFY_EQUAL1_STRING_MAC(destPortBmp, destPortBmpGet, "got another destPortBmp then was set: %d", dev);

        /*
            1.3. Call with fport [6] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportDiagnosticCellSwitchingSet(dev, fport, destPortBmp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 0;

        /*
            1.4. Call with destPortBmp [64] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        destPortBmp = 64;

        st = cpssExMxPmFabricXbarFportDiagnosticCellSwitchingSet(dev, fport, destPortBmp);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, destPortBmp = %d", dev, destPortBmp);
    }

    fport       = 0;
    destPortBmp = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportDiagnosticCellSwitchingSet(dev, fport, destPortBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportDiagnosticCellSwitchingSet(dev, fport, destPortBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet
(
    IN  GT_U8     devNum,
    IN  GT_FPORT  fport,
    OUT GT_U32    *destPortBmpPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   not NULL destPortBmpPtr
    Expected: GT_OK.
    1.2. Call with fport [6] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with destPortBmp [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_FPORT  fport       = 0;
    GT_U32    destPortBmp = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           not NULL destPortBmpPtr
            Expected: GT_OK.
        */
        fport = 0;

        st = cpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet(dev, fport, &destPortBmp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 5;

        st = cpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet(dev, fport, &destPortBmp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call with fport [6] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet(dev, fport, &destPortBmp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 0;

        /*
            1.3. Call with destPortBmpPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, destPortBmp = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet(dev, fport, &destPortBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet(dev, fport, &destPortBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportBroadcastCellSwitchingSet
(
    IN  GT_U8     devNum,
    IN  GT_FPORT  fport,
    IN  GT_U32    destPortBmp
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportBroadcastCellSwitchingSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   destPortBmp [0 / 31]
    Expected: GT_OK.
    1.2. Call with cpssExMxPmFabricXbarFportBroadcastCellSwitchingGet with not NULL destPortBmpPtr
    Expected: GT_OK and the same destPortBmpPtr as was set.
    1.3. Call with fport [6] and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with destPortBmp [64] and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_FPORT  fport          = 0;
    GT_U32    destPortBmp    = 0;
    GT_U32    destPortBmpGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           destPortBmp [0 / 31]
            Expected: GT_OK.
        */
        /* iterate with fport = 0 */
        fport       = 0;
        destPortBmp = 0;

        st = cpssExMxPmFabricXbarFportBroadcastCellSwitchingSet(dev, fport, destPortBmp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, destPortBmp);

        /*
            1.2. Call with cpssExMxPmFabricXbarFportBroadcastCellSwitchingGet with not NULL destPortBmpPtr
            Expected: GT_OK and the same destPortBmpPtr as was set.
        */
        st = cpssExMxPmFabricXbarFportBroadcastCellSwitchingGet(dev, fport, &destPortBmpGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricXbarFportBroadcastCellSwitchingGet: %d, %d", dev, fport);
        UTF_VERIFY_EQUAL1_STRING_MAC(destPortBmp, destPortBmpGet, "got another destPortBmp then was set: %d", dev);

        /* iterate with fport = 5 */
        fport       = 5;
        destPortBmp = 31;

        st = cpssExMxPmFabricXbarFportBroadcastCellSwitchingSet(dev, fport, destPortBmp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, destPortBmp);

        /*
            1.2. Call with cpssExMxPmFabricXbarFportBroadcastCellSwitchingGet with not NULL destPortBmpPtr
            Expected: GT_OK and the same destPortBmpPtr as was set.
        */
        st = cpssExMxPmFabricXbarFportBroadcastCellSwitchingGet(dev, fport, &destPortBmpGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricXbarFportBroadcastCellSwitchingGet: %d, %d", dev, fport);
        UTF_VERIFY_EQUAL1_STRING_MAC(destPortBmp, destPortBmpGet, "got another destPortBmp then was set: %d", dev);

        /*
            1.3. Call with fport [6] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportBroadcastCellSwitchingSet(dev, fport, destPortBmp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 0;

        /*
            1.4. Call with destPortBmp [64] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        destPortBmp = 64;

        st = cpssExMxPmFabricXbarFportBroadcastCellSwitchingSet(dev, fport, destPortBmp);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, destPortBmp = %d", dev, destPortBmp);
    }

    fport       = 0;
    destPortBmp = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportBroadcastCellSwitchingSet(dev, fport, destPortBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportBroadcastCellSwitchingSet(dev, fport, destPortBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportBroadcastCellSwitchingGet
(
    IN  GT_U8     devNum,
    IN  GT_FPORT  fport,
    OUT GT_U32    *destPortBmpPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportBroadcastCellSwitchingGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   not NULL destPortBmpPtr
    Expected: GT_OK.
    1.2. Call with fport [6] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with destPortBmp [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_FPORT  fport       = 0;
    GT_U32    destPortBmp = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           not NULL destPortBmpPtr
            Expected: GT_OK.
        */
        fport = 0;

        st = cpssExMxPmFabricXbarFportBroadcastCellSwitchingGet(dev, fport, &destPortBmp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 5;

        st = cpssExMxPmFabricXbarFportBroadcastCellSwitchingGet(dev, fport, &destPortBmp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call with fport [6] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportBroadcastCellSwitchingGet(dev, fport, &destPortBmp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 0;

        /*
            1.3. Call with destPortBmpPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportBroadcastCellSwitchingGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, destPortBmp = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportBroadcastCellSwitchingGet(dev, fport, &destPortBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportBroadcastCellSwitchingGet(dev, fport, &destPortBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

#if 0
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportManagementCellSwitchingSet
(
    IN  GT_U8     devNum,
    IN  GT_FPORT  fport,
    IN  GT_U32    destPortBmp
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportManagementCellSwitchingSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   destPortBmp [0 / 31]
    Expected: GT_OK.
    1.2. Call with cpssExMxPmFabricXbarFportManagementCellSwitchingGet with not NULL destPortBmpPtr
    Expected: GT_OK and the same destPortBmpPtr as was set.
    1.3. Call with fport [6] and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with destPortBmp [64] and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_FPORT  fport          = 0;
    GT_U32    destPortBmp    = 0;
    GT_U32    destPortBmpGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           destPortBmp [0 / 31]
            Expected: GT_OK.
        */
        /* iterate with fport = 0 */
        fport       = 0;
        destPortBmp = 0;

        st = cpssExMxPmFabricXbarFportManagementCellSwitchingSet(dev, fport, destPortBmp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, destPortBmp);

        /*
            1.2. Call with cpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet with not NULL destPortBmpPtr
            Expected: GT_OK and the same destPortBmpPtr as was set.
        */
        st = cpssExMxPmFabricXbarFportManagementCellSwitchingGet(dev, fport, &destPortBmpGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricXbarFportManagementCellSwitchingGet: %d, %d", dev, fport);
        UTF_VERIFY_EQUAL1_STRING_MAC(destPortBmp, destPortBmpGet, "got another destPortBmp then was set: %d", dev);

        /* iterate with fport = 5 */
        fport       = 5;
        destPortBmp = 31;

        st = cpssExMxPmFabricXbarFportManagementCellSwitchingSet(dev, fport, destPortBmp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, destPortBmp);

        /*
            1.2. Call with cpssExMxPmFabricXbarFportManagementCellSwitchingGet with not NULL destPortBmpPtr
            Expected: GT_OK and the same destPortBmpPtr as was set.
        */
        st = cpssExMxPmFabricXbarFportManagementCellSwitchingGet(dev, fport, &destPortBmpGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricXbarFportManagementCellSwitchingGet: %d, %d", dev, fport);
        UTF_VERIFY_EQUAL1_STRING_MAC(destPortBmp, destPortBmpGet, "got another destPortBmp then was set: %d", dev);

        /*
            1.3. Call with fport [6] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportManagementCellSwitchingSet(dev, fport, destPortBmp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 0;

        /*
            1.4. Call with destPortBmp [64] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        destPortBmp = 64;

        st = cpssExMxPmFabricXbarFportManagementCellSwitchingSet(dev, fport, destPortBmp);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, destPortBmp = %d", dev, destPortBmp);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportManagementCellSwitchingSet(dev, fport, destPortBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportManagementCellSwitchingSet(dev, fport, destPortBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportManagementCellSwitchingGet
(
    IN  GT_U8     devNum,
    IN  GT_FPORT  fport,
    OUT GT_U32    *destPortBmpPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportManagementCellSwitchingGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   not NULL destPortBmpPtr
    Expected: GT_OK.
    1.2. Call with fport [6] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with destPortBmp [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_FPORT  fport       = 0;
    GT_U32    destPortBmp = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           not NULL destPortBmpPtr
            Expected: GT_OK.
        */
        fport = 0;

        st = cpssExMxPmFabricXbarFportManagementCellSwitchingGet(dev, fport, &destPortBmp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 5;

        st = cpssExMxPmFabricXbarFportManagementCellSwitchingGet(dev, fport, &destPortBmp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call with fport [6] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportManagementCellSwitchingGet(dev, fport, &destPortBmp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 0;

        /*
            1.3. Call with destPortBmpPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFportManagementCellSwitchingGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, destPortBmp = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportManagementCellSwitchingGet(dev, fport, &destPortBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportManagementCellSwitchingGet(dev, fport, &destPortBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
#endif

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_FPORT  fport,
    IN  GT_U8     srcDevNum,
    IN  GT_U32    lbh,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   srcDevNum [0 / 127]
                   lbh [0 / 3]
                   enable [GT_TRUE / GT_FALSE]
    Expected: GT_OK.
    1.2. Call with cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet with not NULL EnablePtr
    Expected: GT_OK and the same EnablePtr as was set.
    1.3. Call with fport [6] and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with srcDevNum [128] and other params from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with lbh [4] and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_FPORT  fport     = 0;
    GT_U8     srcDevNum = 0;
    GT_U32    lbh       = 0;
    GT_BOOL   enable    = GT_FALSE;
    GT_BOOL   enableGet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           srcDevNum [0 / 127]
                           lbh [0 / 3]
                           enable [GT_TRUE / GT_FALSE]
            Expected: GT_OK.
        */
        /* iterate with fport = 0 */
        fport = 0;
        srcDevNum = 0;
        lbh = 0;
        enable = GT_FALSE;

        st = cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableSet(dev, fport, srcDevNum, lbh, enable);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, fport, srcDevNum, lbh, enable);

        /*
            1.2. Call with cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet with not NULL EnablePtr
            Expected: GT_OK and the same EnablePtr as was set.
        */
        st = cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet(dev, fport, srcDevNum, lbh, &enableGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet: %d, %d, %d, %d", dev, fport, srcDevNum, lbh);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable then was set: %d", dev);

        /* iterate with fport = 5 */
        fport = 5;
        srcDevNum = dev;
        lbh = 3;
        enable = GT_TRUE;

        st = cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableSet(dev, fport, srcDevNum, lbh, enable);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, fport, srcDevNum, lbh, enable);

        /*
            1.2. Call with cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet with not NULL EnablePtr
            Expected: GT_OK and the same EnablePtr as was set.
        */
        st = cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet(dev, fport, srcDevNum, lbh, &enableGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet: %d, %d, %d, %d", dev, fport, srcDevNum, lbh);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable then was set: %d", dev);

        /*
            1.3. Call with fport [6] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableSet(dev, fport, srcDevNum, lbh, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 0;

        /*
            1.4. Call with srcDevNum [128] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        srcDevNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableSet(dev, fport, srcDevNum, lbh, enable);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, srcDevNum = %d", dev, srcDevNum);

        srcDevNum = 0;

        /*
            1.5. Call with lbh [4] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        lbh = 6;

        st = cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableSet(dev, fport, srcDevNum, lbh, enable);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, lbh = %d", dev, lbh);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableSet(dev, fport, srcDevNum, lbh, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableSet(dev, fport, srcDevNum, lbh, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_FPORT  fport,
    IN  GT_U8     srcDevNum,
    IN  GT_U32    lbh,
    OUT GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 5]
                   srcDevNum [0 / 127]
                   lbh [0 / 3]
                   not NULL enablePtr
    Expected: GT_OK.
    1.2. Call with fport [6] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with srcDevNum [128] and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with lbh [4] and other params from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with enablePtr [NULL] and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_FPORT  fport     = 0;
    GT_U8     srcDevNum = 0;
    GT_U32    lbh       = 0;
    GT_BOOL   enable    = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 5]
                           srcDevNum [0 / 127]
                           lbh [0 / 3]
                           not NULL enablePtr
            Expected: GT_OK.
        */
        fport = 0;
        srcDevNum = 0;
        lbh = 0;

        st = cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet(dev, fport, srcDevNum, lbh, &enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, srcDevNum, lbh);

        fport = 5;
        srcDevNum = dev;
        lbh = 3;

        st = cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet(dev, fport, srcDevNum, lbh, &enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, srcDevNum, lbh);

        /*
            1.2. Call with fport [6] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 6;

        st = cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet(dev, fport, srcDevNum, lbh, &enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 0;

        /*
            1.3. Call with srcDevNum [128] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        srcDevNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet(dev, fport, srcDevNum, lbh, &enable);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, srcDevNum  = %d", dev, srcDevNum);

        srcDevNum = 0;

        /*
            1.4. Call with lbh [4] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        lbh = 6;

        st = cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet(dev, fport, srcDevNum, lbh, &enable);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, lbh  = %d", dev, lbh);

        lbh = 0;

        /*
            1.5. Call with enablePtr [NULL] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        st = cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet(dev, fport, srcDevNum, lbh, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr  = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet(dev, fport, srcDevNum, lbh, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet(dev, fport, srcDevNum, lbh, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricMulticastActiveActiveSelectSet
(
    IN  GT_U8                                            devNum,
    IN  CPSS_EXMXPM_FABRIC_MC_ACTIVE_ACTIVE_HGLINK_ENT   activePorts
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricMulticastActiveActiveSelectSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with activePorts [CPSS_EXMXPM_FABRIC_MC_ACTIVE_ACTIVE_HGLINK_0_2_E /
                                CPSS_EXMXPM_FABRIC_MC_ACTIVE_ACTIVE_HGLINK_0_3_E]
    Expected: GT_OK
    1.2. Call with cpssExMxPmFabricMulticastActiveActiveSelectGet with not NULL activePortsPtr.
    expected: GT_OK and the same activePortsPtr as wa sset.
    1.3. Call with activePorts [0x5AAAAAA5]
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_MC_ACTIVE_ACTIVE_HGLINK_ENT   activePorts;
    CPSS_EXMXPM_FABRIC_MC_ACTIVE_ACTIVE_HGLINK_ENT   activePortsGet;


    activePorts    = CPSS_EXMXPM_FABRIC_MC_ACTIVE_ACTIVE_HGLINK_0_2_E;
    activePortsGet = CPSS_EXMXPM_FABRIC_MC_ACTIVE_ACTIVE_HGLINK_0_2_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with activePorts [CPSS_EXMXPM_FABRIC_MC_ACTIVE_ACTIVE_HGLINK_0_2_E /
                                        CPSS_EXMXPM_FABRIC_MC_ACTIVE_ACTIVE_HGLINK_0_3_E]
            Expected: GT_OK
        */
        activePorts = CPSS_EXMXPM_FABRIC_MC_ACTIVE_ACTIVE_HGLINK_0_2_E;

        st = cpssExMxPmFabricMulticastActiveActiveSelectSet(dev, activePorts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, activePorts);

        /*
            1.2. Call with cpssExMxPmFabricMulticastActiveActiveSelectGet with not NULL activePortsPtr.
            Expected: GT_OK and the same activePortsPtr as wa sset.
        */
        st = cpssExMxPmFabricMulticastActiveActiveSelectGet(dev, &activePortsGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmFabricMulticastActiveActiveSelectGet: %d, ", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(activePorts, activePortsGet, "got another activePorts then was set: %d, ", dev);


        activePorts = CPSS_EXMXPM_FABRIC_MC_ACTIVE_ACTIVE_HGLINK_0_3_E;

        st = cpssExMxPmFabricMulticastActiveActiveSelectSet(dev, activePorts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, activePorts);

        /*
            1.2. Call with cpssExMxPmFabricMulticastActiveActiveSelectGet with not NULL activePortsPtr.
            Expected: GT_OK and the same activePortsPtr as wa sset.
        */
        st = cpssExMxPmFabricMulticastActiveActiveSelectGet(dev, &activePortsGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmFabricMulticastActiveActiveSelectGet: %d, ", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(activePorts, activePortsGet, "got another activePorts then was set: %d, ", dev);

        /*
            1.3. Call with activePorts [0x5AAAAAA5]
            Expected: GT_BAD_PARAM.
        */
        activePorts = FABRIC_XBAR_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricMulticastActiveActiveSelectSet(dev, activePorts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, activePorts);
    }

    activePorts = CPSS_EXMXPM_FABRIC_MC_ACTIVE_ACTIVE_HGLINK_0_2_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricMulticastActiveActiveSelectSet(dev, activePorts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricMulticastActiveActiveSelectSet(dev, activePorts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricMulticastActiveActiveSelectGet
(
    IN  GT_U8                                            devNum,
    OUT CPSS_EXMXPM_FABRIC_MC_ACTIVE_ACTIVE_HGLINK_ENT   *activePortsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricMulticastActiveActiveSelectGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL activePortsPtr.
    Expected: GT_OK
    1.2. Call with activePortsPtr [NULL].
    Expected: GT_OK
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_MC_ACTIVE_ACTIVE_HGLINK_ENT   activePorts;


    activePorts = CPSS_EXMXPM_FABRIC_MC_ACTIVE_ACTIVE_HGLINK_0_2_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL activePortsPtr.
            Expected: GT_OK
        */
        st = cpssExMxPmFabricMulticastActiveActiveSelectGet(dev, &activePorts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with activePortsPtr [NULL].
            Expected: GT_OK
        */
        st = cpssExMxPmFabricMulticastActiveActiveSelectGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, activePortsPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricMulticastActiveActiveSelectGet(dev, &activePorts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricMulticastActiveActiveSelectGet(dev, &activePorts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFcActiveActiveMappingSet
(
    IN GT_U8 devNum,
    IN GT_U32 voqNum,
    IN CPSS_EXMXPM_FABRIC_VOQ_SRC_FC_PORT_ENT sourcefPort,
    IN GT_U32 fcVectorBit
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFcActiveActiveMappingSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqNum [0 / 23], 
                   sourcefPort [CPSS_EXMXPM_FABRIC_VOQ_SRC_FC_PORT_0_2_E,
                                CPSS_EXMXPM_FABRIC_VOQ_SRC_FC_PORT_1_3_E], 
                   fcVectorBit [0 / 11].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricXbarFcActiveActiveMappingGet with not NULL pointers and other params from 1.1.
    Expected: GT_OK.
    1.3. Call with voqNum [24] (out of range).
    Expectdd: NOT GT_OK.
    1.4. Call with fcVectorBit [12] (out of range).
    Expectdd: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqNum = 0;
    CPSS_EXMXPM_FABRIC_VOQ_SRC_FC_PORT_ENT sourcefPort;
    GT_U32      fcVectorBit = 0;
    CPSS_EXMXPM_FABRIC_VOQ_SRC_FC_PORT_ENT sourcefPortGet;
    GT_U32      fcVectorBitGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqNum [0 / 23], 
                           sourcefPort [CPSS_EXMXPM_FABRIC_VOQ_SRC_FC_PORT_0_2_E,
                                        CPSS_EXMXPM_FABRIC_VOQ_SRC_FC_PORT_1_3_E], 
                           fcVectorBit [0 / 11].
            Expected: GT_OK.
        */
        /* iterate with voqNum = 0 */
        voqNum = 0;
        sourcefPort = CPSS_EXMXPM_FABRIC_VOQ_SRC_FC_PORT_0_2_E;
        fcVectorBit = 0;

        st = cpssExMxPmFabricXbarFcActiveActiveMappingSet(dev, voqNum, sourcefPort, fcVectorBit);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, voqNum, sourcefPort, fcVectorBit);

        /*
            1.2. Call cpssExMxPmFabricXbarFcActiveActiveMappingGet with not NULL pointers and other params from 1.1.
            Expected: GT_OK.
        */
        st = cpssExMxPmFabricXbarFcActiveActiveMappingGet(dev, voqNum, &sourcefPortGet, &fcVectorBitGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricXbarFcActiveActiveMappingGet: %d, %d", dev, voqNum);

        UTF_VERIFY_EQUAL1_STRING_MAC(sourcefPort, sourcefPortGet, "got another sourcefPort than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fcVectorBit, fcVectorBitGet, "got another fcVectorBit than was set: %d", dev);

        /* iterate with voqNum = 23 */
        voqNum = 23;
        sourcefPort = CPSS_EXMXPM_FABRIC_VOQ_SRC_FC_PORT_1_3_E;
        fcVectorBit = 11;

        st = cpssExMxPmFabricXbarFcActiveActiveMappingSet(dev, voqNum, sourcefPort, fcVectorBit);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, voqNum, sourcefPort, fcVectorBit);

        /*
            1.2. Call cpssExMxPmFabricXbarFcActiveActiveMappingGet with not NULL pointers and other params from 1.1.
            Expected: GT_OK.
        */
        st = cpssExMxPmFabricXbarFcActiveActiveMappingGet(dev, voqNum, &sourcefPortGet, &fcVectorBitGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricXbarFcActiveActiveMappingGet: %d, %d", dev, voqNum);

        UTF_VERIFY_EQUAL1_STRING_MAC(sourcefPort, sourcefPortGet, "got another sourcefPort than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fcVectorBit, fcVectorBitGet, "got another fcVectorBit than was set: %d", dev);

        /*
            1.3. Call with voqNum [24] (out of range).
            Expectdd: NOT GT_OK.
        */
        voqNum = 24;

        st = cpssExMxPmFabricXbarFcActiveActiveMappingSet(dev, voqNum, sourcefPort, fcVectorBit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqNum);

        voqNum = 0;

        /*
            1.4. Call with fcVectorBit [12] (out of range).
            Expectdd: NOT GT_OK.
        */
        fcVectorBit = 12;

        st = cpssExMxPmFabricXbarFcActiveActiveMappingSet(dev, voqNum, sourcefPort, fcVectorBit);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fcVectorBit = %d", dev, fcVectorBit);
    }

    voqNum = 0;
    sourcefPort = CPSS_EXMXPM_FABRIC_VOQ_SRC_FC_PORT_0_2_E;
    fcVectorBit = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFcActiveActiveMappingSet(dev, voqNum, sourcefPort, fcVectorBit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFcActiveActiveMappingSet(dev, voqNum, sourcefPort, fcVectorBit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricXbarFcActiveActiveMappingGet
(
    IN GT_U8    devNum,
    IN GT_U32   voqNum,
    OUT CPSS_EXMXPM_FABRIC_VOQ_SRC_FC_PORT_ENT *sourcefPort,
    OUT GT_U32  *fcVectorBit
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricXbarFcActiveActiveMappingGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqNum [0 / 23], 
                   not NULL sourcefPort, fcVectorBit.
    Expected: GT_OK.
    1.2. Call with sourcefPort [NULL].
    Expectdd: GT_BAD_PTR.
    1.3. Call with fcVectorBit [NULL].
    Expectdd: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqNum = 0;
    CPSS_EXMXPM_FABRIC_VOQ_SRC_FC_PORT_ENT sourcefPort;
    GT_U32      fcVectorBit = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqNum [0 / 23], 
                           not NULL sourcefPort, fcVectorBit.
            Expected: GT_OK.
        */
        voqNum = 0;

        st = cpssExMxPmFabricXbarFcActiveActiveMappingGet(dev, voqNum, &sourcefPort, &fcVectorBit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqNum);

        voqNum = 23;

        st = cpssExMxPmFabricXbarFcActiveActiveMappingGet(dev, voqNum, &sourcefPort, &fcVectorBit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqNum);

        /*
            1.2. Call with sourcefPort [NULL].
            Expectdd: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFcActiveActiveMappingGet(dev, voqNum, NULL, &fcVectorBit);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, sourcefPort = %d", dev);

        /*
            1.3. Call with fcVectorBit [NULL].
            Expectdd: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricXbarFcActiveActiveMappingGet(dev, voqNum, &sourcefPort, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fcVectorBit = %d", dev);
    }

    voqNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricXbarFcActiveActiveMappingGet(dev, voqNum, &sourcefPort, &fcVectorBit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricXbarFcActiveActiveMappingGet(dev, voqNum, &sourcefPort, &fcVectorBit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmFabricXbar suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmFabricXbar)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportBlockingSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportBlockingGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportFcSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportFcGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportArbiterSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportArbiterGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarClassArbiterSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarClassArbiterGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarShaperSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarShaperGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportFifoStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportCellCntrTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportCellCntrTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportCellCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportTxFifoDroppedCellsCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportRxDroppedCellsCntrsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportMulticastCellSwitchingSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportMulticastCellSwitchingGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportUnicastCellSwitchingSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportUnicastCellSwitchingGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportDiagnosticCellSwitchingSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportBroadcastCellSwitchingSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportBroadcastCellSwitchingGet)
    /* UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportManagementCellSwitchingSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportManagementCellSwitchingGet) DELETE */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricMulticastActiveActiveSelectSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricMulticastActiveActiveSelectGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFcActiveActiveMappingSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricXbarFcActiveActiveMappingGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmFabricXbar)

