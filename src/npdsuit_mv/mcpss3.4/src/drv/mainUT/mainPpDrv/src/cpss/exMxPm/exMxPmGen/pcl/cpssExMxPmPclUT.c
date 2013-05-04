/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmPclUT.c
*
* DESCRIPTION:
*       Unit tests for CPSS ExMxPm Pcl facility API.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/exMxPm/exMxPmGen/pcl/cpssExMxPmPcl.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* Invalid enum */
#define EXMXPM_PCL_INVALID_ENUM_CNS     0x5AAAAAA5

/* out of range trunk value */
#define MAX_TRUNK_ID_CNS                256

/*  Internal function. Is used for filling PCL action structure     */
/*  with default values which are used for most of all tests.       */
static void pclActionDefaultSet (OUT CPSS_EXMXPM_PCL_ACTION_UNT     *actionPtr);

/*----------------------------------------------------------------------------*/

/*  Internal function. Is used for filling PCL mask & pattern structure     */
/*  with default values which are used for most of all tests.               */

static void pclRuleMaskPatternDefaultSet
(
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_UNT     *maskPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_UNT     *patternPtr
);


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclPolicyEnableSet
(
    IN GT_U8                             devNum,
    IN CPSS_PCL_DIRECTION_ENT            direction,
    IN GT_BOOL                           enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclPolicyEnableSet)
{
/*
ITERATE_DEVICES (ExMxPm)
1.1. Call function with direction [CPSS_PCL_DIRECTION_INGRESS_E / CPSS_PCL_DIRECTION_EGRESS_E] and enable [GT_TRUE / GT_FALSE]. Expected: GT_OK.
1.2. Call cpssExMxPmPclPolicyEnableGet with non-NULL enablePtr. Expected: GT_OK and the same enable.
1.3. Call function with enable [GT_TRUE] and out of range direction [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
1.4. Call function with enable [GT_FALSE] and out of range direction [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_PCL_DIRECTION_ENT  direction;
    GT_BOOL                 enable;
    GT_BOOL                 enableRet;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with direction [CPSS_PCL_DIRECTION_INGRESS_E
           / CPSS_PCL_DIRECTION_EGRESS_E] and enable [GT_TRUE / GT_FALSE].
           Expected: GT_OK. */

        direction = CPSS_PCL_DIRECTION_INGRESS_E;
        enable = GT_TRUE;

        st = cpssExMxPmPclPolicyEnableSet(dev, direction, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, direction, enable);

        direction = CPSS_PCL_DIRECTION_EGRESS_E;
        enable = GT_FALSE;

        st = cpssExMxPmPclPolicyEnableSet(dev, direction, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, direction, enable);

        /* 1.2. Call cpssExMxPmPclPolicyEnableGet with non-NULL enablePtr.
           Expected: GT_OK and the same enable. */

        st = cpssExMxPmPclPolicyEnableGet(dev, direction, &enableRet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPclPolicyEnableGet: %d", dev);

        if (GT_OK == st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableRet,
             "cpssExMxPmPclPolicyEnableGet: get another enable than was set: dev = %d, direction = %d",
                                         dev, direction);
        }

        direction = CPSS_PCL_DIRECTION_INGRESS_E;
        enable = GT_TRUE;

        st = cpssExMxPmPclPolicyEnableGet(dev, direction, &enableRet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPclPolicyEnableGet: %d", dev);

        if (GT_OK == st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableRet,
             "cpssExMxPmPclPolicyEnableGet: get another enable than was set: dev = %d, direction = %d",
                                         dev, direction);
        }


        /* 1.3. Call function with enable [GT_TRUE] and out of range direction
           [0x5AAAAAA5]. Expected: GT_BAD_PARAM.    */

        direction = EXMXPM_PCL_INVALID_ENUM_CNS;
        enable = GT_TRUE;

        st = cpssExMxPmPclPolicyEnableSet(dev, direction, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, direction);

        /* 1.4. Call function with enable [GT_FALSE] and out of range
           direction [0x5AAAAAA5]. Expected: GT_BAD_PARAM.    */

        enable = GT_FALSE;

        st = cpssExMxPmPclPolicyEnableSet(dev, direction, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, direction);
    }

    direction = CPSS_PCL_DIRECTION_INGRESS_E;
    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclPolicyEnableSet(dev, direction, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclPolicyEnableSet(dev, direction, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclPolicyEnableGet
(
    IN GT_U8                             devNum,
    IN CPSS_PCL_DIRECTION_ENT            direction,
    OUT GT_BOOL                         * enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclPolicyEnableGet)
{
/*
ITERATE_DEVICES (ExMxPm)
1.1. Call function with direction [CPSS_PCL_DIRECTION_INGRESS_E / CPSS_PCL_DIRECTION_EGRESS_E] and non-NULL enablePtr. Expected: GT_OK.
1.2. Call function with out of range direction [0x5AAAAAA5] and non-NULL enablePtr. Expected: GT_BAD_PARAM.
1.3. Call function with direction [CPSS_PCL_DIRECTION_INGRESS_E] and enablePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_PCL_DIRECTION_ENT  direction;
    GT_BOOL                 enable;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with direction [CPSS_PCL_DIRECTION_INGRESS_E
           / CPSS_PCL_DIRECTION_EGRESS_E] and non-NULL enablePtr.
           Expected: GT_OK. */

        direction = CPSS_PCL_DIRECTION_INGRESS_E;

        st = cpssExMxPmPclPolicyEnableGet(dev, direction, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, direction);

        direction = CPSS_PCL_DIRECTION_EGRESS_E;

        st = cpssExMxPmPclPolicyEnableGet(dev, direction, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, direction);

        /* 1.2. Call function with out of range direction [0x5AAAAAA5] and
           non-NULL enablePtr. Expected: GT_BAD_PARAM.  */

        direction = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclPolicyEnableGet(dev, direction, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, direction);

        direction = CPSS_PCL_DIRECTION_INGRESS_E;   /* restore */

        /* 1.3. Call function with direction [CPSS_PCL_DIRECTION_INGRESS_E]
           and enablePtr [NULL]. Expected: GT_BAD_PTR.  */

        st = cpssExMxPmPclPolicyEnableGet(dev, direction, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    direction = CPSS_PCL_DIRECTION_INGRESS_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclPolicyEnableGet(dev, direction, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclPolicyEnableGet(dev, direction, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclPortIngressPolicyEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_U8    port,
    IN  GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclPortIngressPolicyEnableSet)
{
/*
ITERATE_DEVICES_VIRT_PORT (ExMxPm)
1.1.1. Call function with enable [GT_TRUE / GT_FALSE]. Expected: GT_OK.
1.1.2. Call cpssExMxPmPclPortIngressPolicyEnableGet with non-NULL enablePtr. Expected: GT_OK and the same enable.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     enable = GT_TRUE;
    GT_BOOL     enableRet = GT_TRUE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with enable [GT_TRUE / GT_FALSE].
               Expected: GT_OK. */

            enable = GT_TRUE;

            st = cpssExMxPmPclPortIngressPolicyEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            enable = GT_FALSE;

            st = cpssExMxPmPclPortIngressPolicyEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /* 1.1.2. Call cpssExMxPmPclPortIngressPolicyEnableGet with
               non-NULL enablePtr. Expected: GT_OK and the same enable. */

            st = cpssExMxPmPclPortIngressPolicyEnableGet(dev, port, &enableRet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPclPortIngressPolicyEnableGet: %d", dev);

            if (GT_OK == st)
            {
                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableRet,
                 "cpssExMxPmPclPortIngressPolicyEnableGet: get another enable than was set: dev = %d", dev);
            }

        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPclPortIngressPolicyEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPclPortIngressPolicyEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK       */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPclPortIngressPolicyEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;
    enable = GT_TRUE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclPortIngressPolicyEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclPortIngressPolicyEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclPortIngressPolicyEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    port,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclPortIngressPolicyEnableGet)
{
/*
ITERATE_DEVICES_VIRT_PORT (ExMxPm)
1.1.1. Call function with non-NULL enablePtr. Expected: GT_OK.
1.1.2. Call function with enablePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     enable;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with non-NULL enablePtr. Expected: GT_OK.*/

            st = cpssExMxPmPclPortIngressPolicyEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call function with enablePtr [NULL]. Expected: GT_BAD_PTR.*/

            st = cpssExMxPmPclPortIngressPolicyEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPclPortIngressPolicyEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPclPortIngressPolicyEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPclPortIngressPolicyEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclPortIngressPolicyEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclPortIngressPolicyEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclPortLookupCfgTblAccessModeSet
(
    IN GT_U8                                            devNum,
    IN GT_U8                                            port,
    IN CPSS_PCL_DIRECTION_ENT                           direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT                       lookupNum,
    IN CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_ENT mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclPortLookupCfgTblAccessModeSet)
{
/*
ITERATE_DEVICES_VIRT_PORT (ExMxPm)
1.1.1. Call function with direction [CPSS_PCL_DIRECTION_INGRESS_E / CPSS_PCL_DIRECTION_EGRESS_E], lookupNum [CPSS_PCL_LOOKUP_0_E / CPSS_PCL_LOOKUP_1_E] and mode [CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_BY_PORT_E / CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_BY_VLAN_E]. Expected: GT_OK.
1.1.2. Call cpssExMxPmPclPortLookupCfgTblAccessModeGet with non-NULL modePtr and other parameters the same as in 1.1.1. Expected: GT_OK and the same mode.
1.1.3. Call function with out of range direction [0x5AAAAAA5] and other parameters the same as in 1.1.1. Expected: GT_BAD_PARAM.
1.1.4. Call function with out of range lookupNum [0x5AAAAAA5] and other parameters the same as in 1.1.1. Expected: GT_BAD_PARAM.
1.1.5. Call function with out of range mode [0x5AAAAAA5] and other parameters the same as in 1.1.1. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st       = GT_OK;
    GT_U8       dev;
    GT_U8       port;
    CPSS_PCL_DIRECTION_ENT                           direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT                       lookupNum;
    CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_ENT mode;
    CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_ENT modeRet;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with direction [CPSS_PCL_DIRECTION_INGRESS_E
              / CPSS_PCL_DIRECTION_EGRESS_E], lookupNum [CPSS_PCL_LOOKUP_0_E
              / CPSS_PCL_LOOKUP_1_E] and mode
              [CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_BY_PORT_E /
               CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_BY_VLAN_E].
               Expected: GT_OK. */

            direction = CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_0_E;
            mode = CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_BY_PORT_E;

            st = cpssExMxPmPclPortLookupCfgTblAccessModeSet(dev, port, direction,
                                                            lookupNum, mode);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, direction,
                                                            lookupNum, mode);

            direction = CPSS_PCL_DIRECTION_EGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_1_E;
            mode = CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_BY_VLAN_E;

            st = cpssExMxPmPclPortLookupCfgTblAccessModeSet(dev, port, direction,
                                                            lookupNum, mode);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, direction,
                                                            lookupNum, mode);

            /* 1.1.2. Call cpssExMxPmPclPortLookupCfgTblAccessModeGet with
               non-NULL modePtr and other parameters the same as in 1.1.1.
               Expected: GT_OK and the same mode.   */

            st = cpssExMxPmPclPortLookupCfgTblAccessModeGet(dev, port, direction,
                                                            lookupNum, &modeRet);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                "cpssExMxPmPclPortLookupCfgTblAccessModeGet: %d, %d, %d, %d",
                                         dev, port, direction, lookupNum);

            if (GT_OK == st)
            {
                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeRet,
                 "cpssExMxPmPclPortLookupCfgTblAccessModeGet: get another mode than was set: dev = %d", dev);
            }

            /* 1.1.3. Call function with out of range direction [0x5AAAAAA5] and
               other parameters the same as in 1.1.1. Expected: GT_BAD_PARAM. */

            direction = EXMXPM_PCL_INVALID_ENUM_CNS;
            lookupNum = CPSS_PCL_LOOKUP_0_E;
            mode = CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_BY_PORT_E;

            st = cpssExMxPmPclPortLookupCfgTblAccessModeSet(dev, port, direction,
                                                            lookupNum, mode);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, direction = %d",
                                         dev, direction);

            direction = CPSS_PCL_DIRECTION_INGRESS_E;   /* restore */

            /* 1.1.4. Call function with out of range lookupNum [0x5AAAAAA5] and
               other parameters the same as in 1.1.1. Expected: GT_BAD_PARAM. */

            lookupNum = EXMXPM_PCL_INVALID_ENUM_CNS;

            st = cpssExMxPmPclPortLookupCfgTblAccessModeSet(dev, port, direction,
                                                            lookupNum, mode);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, lookupNum = %d",
                                         dev, lookupNum);

            lookupNum = CPSS_PCL_LOOKUP_0_E;    /* restore */

            /* 1.1.5. Call function with out of range mode [0x5AAAAAA5] and
               other parameters the same as in 1.1.1. Expected: GT_BAD_PARAM. */

            mode = EXMXPM_PCL_INVALID_ENUM_CNS;

            st = cpssExMxPmPclPortLookupCfgTblAccessModeSet(dev, port, direction,
                                                            lookupNum, mode);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, mode = %d",
                                         dev, mode);

            mode = CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_BY_PORT_E;
        }

        direction = CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum = CPSS_PCL_LOOKUP_0_E;
        mode = CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_BY_PORT_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPclPortLookupCfgTblAccessModeSet(dev, port, direction,
                                                            lookupNum, mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPclPortLookupCfgTblAccessModeSet(dev, port, direction,
                                                        lookupNum, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPclPortLookupCfgTblAccessModeSet(dev, port, direction,
                                                        lookupNum, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;
    direction = CPSS_PCL_DIRECTION_INGRESS_E;
    lookupNum = CPSS_PCL_LOOKUP_0_E;
    mode = CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_BY_PORT_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclPortLookupCfgTblAccessModeSet(dev, port, direction,
                                                        lookupNum, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclPortLookupCfgTblAccessModeSet(dev, port, direction,
                                                    lookupNum, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclPortLookupCfgTblAccessModeGet
(
    IN  GT_U8                                            devNum,
    IN  GT_U8                                            port,
    IN  CPSS_PCL_DIRECTION_ENT                           direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT                       lookupNum,
    OUT CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_ENT *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclPortLookupCfgTblAccessModeGet)
{
/*
ITERATE_DEVICES_VIRT_PORT (ExMxPm)
1.1.1. Call function with direction [CPSS_PCL_DIRECTION_INGRESS_E / CPSS_PCL_DIRECTION_EGRESS_E], lookupNum [CPSS_PCL_LOOKUP_0_E / CPSS_PCL_LOOKUP_1_E] and non-NULL modePtr. Expected: GT_OK.
1.1.2. Call function with out of range direction [0x5AAAAAA5] and other parameters the same as in 1.1.1. Expected: GT_BAD_PARAM.
1.1.3. Call function with out of range lookupNum [0x5AAAAAA5] and other parameters the same as in 1.1.1. Expected: GT_BAD_PARAM.
1.1.4. Call function with modePtr [NULL] and other parameters the same as in 1.1.1. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st       = GT_OK;
    GT_U8       dev;
    GT_U8       port;
    CPSS_PCL_DIRECTION_ENT                           direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT                       lookupNum;
    CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_ENT mode;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with direction [CPSS_PCL_DIRECTION_INGRESS_E
               / CPSS_PCL_DIRECTION_EGRESS_E], lookupNum [CPSS_PCL_LOOKUP_0_E /
                 CPSS_PCL_LOOKUP_1_E] and non-NULL modePtr. Expected: GT_OK. */

            direction = CPSS_PCL_DIRECTION_EGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_1_E;

            st = cpssExMxPmPclPortLookupCfgTblAccessModeGet(dev, port,
                                           direction, lookupNum, &mode);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction,
                                        lookupNum);

            direction = CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_0_E;

            st = cpssExMxPmPclPortLookupCfgTblAccessModeGet(dev, port,
                                           direction, lookupNum, &mode);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction,
                                        lookupNum);

            /* 1.1.2. Call function with out of range direction [0x5AAAAAA5]
               and other parameters the same as in 1.1.1.
               Expected: GT_BAD_PARAM.  */

            direction = EXMXPM_PCL_INVALID_ENUM_CNS;

            st = cpssExMxPmPclPortLookupCfgTblAccessModeGet(dev, port,
                                           direction, lookupNum, &mode);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, direction = %d",
                                         dev, direction);

            direction = CPSS_PCL_DIRECTION_INGRESS_E;   /* restore */

            /* 1.1.3. Call function with out of range lookupNum [0x5AAAAAA5]
               and other parameters the same as in 1.1.1.
               Expected: GT_BAD_PARAM.  */

            lookupNum = EXMXPM_PCL_INVALID_ENUM_CNS;

            st = cpssExMxPmPclPortLookupCfgTblAccessModeGet(dev, port,
                                           direction, lookupNum, &mode);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, lookupNum = %d",
                                         dev, lookupNum);

            lookupNum = CPSS_PCL_LOOKUP_0_E;   /* restore */

            /* 1.1.4. Call function with modePtr [NULL] and other parameters
               the same as in 1.1.1. Expected: GT_BAD_PTR.  */

            st = cpssExMxPmPclPortLookupCfgTblAccessModeGet(dev, port,
                                           direction, lookupNum, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, modePtr = NULL",
                                         dev);
        }

        direction = CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum = CPSS_PCL_LOOKUP_0_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPclPortLookupCfgTblAccessModeGet(dev, port,
                                           direction, lookupNum, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPclPortLookupCfgTblAccessModeGet(dev, port,
                                       direction, lookupNum, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPclPortLookupCfgTblAccessModeGet(dev, port,
                                       direction, lookupNum, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;
    direction = CPSS_PCL_DIRECTION_INGRESS_E;
    lookupNum = CPSS_PCL_LOOKUP_0_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclPortLookupCfgTblAccessModeGet(dev, port,
                                       direction, lookupNum, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclPortLookupCfgTblAccessModeGet(dev, port,
                                   direction, lookupNum, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclCfgTblSet
(
    IN GT_U8                             devNum,
    IN CPSS_INTERFACE_INFO_STC           *interfaceInfoPtr,
    IN CPSS_PCL_DIRECTION_ENT            direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT        lookupNum,
    IN CPSS_EXMXPM_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclCfgTblSet)
{
/*
ITERATE_DEVICES (ExMxPm)
1.1. Call function with non-NULL interfaceInfoPtr { type [CPSS_INTERFACE_PORT_E / CPSS_INTERFACE_VID_E];
                                                    devPort { devNum [0]; portNum [8]};
                                                    trunkId [1];
                                                    vidx [100];
                                                    vlanId [100] (accordingly to the type) },
                                  direction [CPSS_PCL_DIRECTION_INGRESS_E / CPSS_PCL_DIRECTION_EGRESS_E],
                                  lookupNum [CPSS_PCL_LOOKUP_0_E / CPSS_PCL_LOOKUP_1_E]
                                  lookupCfgPtr { enableLookup [GT_TRUE / GT_FALSE];
                                                pclId [2];
                                                groupKeyTypes { nonIpKey [CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E / CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_NOT_IPV6_E];
                                                ipv4Key [CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E / CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_IPV4_L4_E ];
                                                ipv6Key [CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E / CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_IPV6_L2_E]} }.
Expected: GT_OK.
1.2. Call cpssExMxPmPclCfgTblGet with non-NULL lookupCfgPtr and other parameters the same as in 1.1. Expected: GT_OK and the same lookupCfgPtr.
1.3. Call function with out of range interfaceInfoPtr->type [0x5AAAAAA5] and other  parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.4. Call function with interfaceInfoPtr->type [CPSS_INTERFACE_PORT_E], out of range interfaceInfoPtr->devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] and other  parameters the same as in 1.1. Expected: NON GT_OK.
1.5. Call function with interfaceInfoPtr->type [CPSS_INTERFACE_VID_E], out of range interfaceInfoPtr->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] and other  parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.6. Call with with interfaceInfoPtr->type [CPSS_INTERFACE_TRUNK_E / CPSS_INTERFACE_VIDX_E / CPSS_INTERFACE_DEVICE_E / CPSS_INTERFACE_FABRIC_VIDX_E] not supported.Expectedc: NOT GT_OK.
1.7. Call function with interfaceInfoPtr [NULL] and other  parameters the same as in 1.1. Expected: GT_BAD_PTR.
1.8. Call function with out of range direction [0x5AAAAAA5] and other  parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.9. Call function with out of range lookupNum [0x5AAAAAA5] and other  parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.10. Call function with lookupCfgPtr->enableLookup [GT_TRUE], out of range lookupCfgPtr->pclId [1024] and other parameters the same as in 1.1. Expected: NON GT_OK.
1.11. Call function with lookupCfgPtr->enableLookup [GT_TRUE], out of range lookupCfgPtr->nonIpKey [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.12. Call function with lookupCfgPtr->enableLookup [GT_TRUE], lookupCfgPtr->ipv4Key [CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E] (invalid) and other parameters the same as in 1.1. Expected: NON GT_OK.
1.13. Call function with lookupCfgPtr->enableLookup [GT_FALSE], lookupCfgPtr->ipv4Key [CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E] (invalid) and other parameters the same as in 1.1. Expected: NON GT_OK.
1.14. Call function with lookupCfgPtr->enableLookup [GT_TRUE], out of range lookupCfgPtr->ipv4Key [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.15. Call function with lookupCfgPtr->enableLookup [GT_TRUE], lookupCfgPtr->ipv6Key [CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_NOT_IPV6_E] and other parameters the same as in 1.1. Expected: NON GT_OK.
1.16. Call function with lookupCfgPtr->enableLookup [GT_FALSE], lookupCfgPtr->ipv6Key [CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_NOT_IPV6_E] and other  parameters the same as in 1.1. Expected: GT_OK.
1.17. Call function with lookupCfgPtr->enableLookup [GT_TRUE], out of range lookupCfgPtr->ipv6Key [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.18. Call function with lookupCfgPtr [NULL] and other  parameters the same as in 1.1. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_INTERFACE_INFO_STC           interfaceInfo;
    CPSS_PCL_DIRECTION_ENT            direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT        lookupNum;
    CPSS_EXMXPM_PCL_LOOKUP_CFG_STC    lookupCfg;
    CPSS_EXMXPM_PCL_LOOKUP_CFG_STC    lookupCfgRet;
    GT_BOOL     isEqual;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL interfaceInfoPtr { type
           [CPSS_INTERFACE_PORT_E / CPSS_INTERFACE_TRUNK_E /
           CPSS_INTERFACE_VIDX_E / CPSS_INTERFACE_VID_E]; devPort
           { devNum [32]; portNum [8]}; trunkId [16]; vidx [100];
           vlanId [100] (accordingly to the type) }, direction
           [CPSS_PCL_DIRECTION_INGRESS_E / CPSS_PCL_DIRECTION_EGRESS_E],
           lookupNum [CPSS_PCL_LOOKUP_0_E / CPSS_PCL_LOOKUP_1_E] and
           non-NULL lookupCfgPtr { enableLookup [GT_TRUE / GT_FALSE];
           pclId [2]; groupKeyTypes { nonIpKey
           [CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E /
           CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_NOT_IPV6_E];
           ipv4Key [CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E /
           CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_IPV4_L4_E ]; ipv6Key
           [CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E /
           CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_IPV6_L2_E]} }.
           Expected: GT_OK. */

        cpssOsBzero((GT_VOID*)&interfaceInfo, sizeof(CPSS_INTERFACE_INFO_STC));
        cpssOsBzero((GT_VOID*)&lookupCfg, sizeof(CPSS_EXMXPM_PCL_LOOKUP_CFG_STC));

        interfaceInfo.type = CPSS_INTERFACE_PORT_E;

        interfaceInfo.devPort.devNum = 0;
        interfaceInfo.devPort.portNum = 8;

        interfaceInfo.trunkId = 1;
        interfaceInfo.vidx = 100;
        interfaceInfo.vlanId = 100;

        direction = CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum = CPSS_PCL_LOOKUP_0_E;

        lookupCfg.enableLookup = GT_FALSE;
        lookupCfg.pclId = 2;

        lookupCfg.groupKeyTypes.nonIpKey = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E;
        lookupCfg.groupKeyTypes.ipv4Key  = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E;
        lookupCfg.groupKeyTypes.ipv6Key  = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);


        interfaceInfo.type = CPSS_INTERFACE_VID_E;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);


        interfaceInfo.type = CPSS_INTERFACE_PORT_E;

        direction = CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum = CPSS_PCL_LOOKUP_1_E;

        lookupCfg.enableLookup = GT_TRUE;

        lookupCfg.groupKeyTypes.nonIpKey = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_E;
        lookupCfg.groupKeyTypes.ipv4Key  = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IPV4_L4_E;
        lookupCfg.groupKeyTypes.ipv6Key  = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_E;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* restore */
        interfaceInfo.type = CPSS_INTERFACE_PORT_E;

        direction = CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum = CPSS_PCL_LOOKUP_0_E;

        lookupCfg.enableLookup = GT_FALSE;

        lookupCfg.groupKeyTypes.nonIpKey = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_NOT_IPV6_E;
        lookupCfg.groupKeyTypes.ipv4Key = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_E;
        lookupCfg.groupKeyTypes.ipv6Key = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_IPV6_L2_E;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call cpssExMxPmPclCfgTblGet with non-NULL lookupCfgPtr and
           other parameters the same as in 1.1.
           Expected: GT_OK and the same lookupCfgPtr.   */

        cpssOsBzero((GT_VOID*) &lookupCfgRet, sizeof(lookupCfgRet));

        st = cpssExMxPmPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfgRet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPclCfgTblGet: %d", dev);

        if (GT_OK == st)
        {
            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&lookupCfg, (GT_VOID*)&lookupCfgRet, sizeof (lookupCfg)))
                  ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                             "get another lookupCfg than was set: %d", dev);
        }

        /* 1.3. Call function with out of range interfaceInfoPtr->type
           [0x5AAAAAA5] and other  parameters the same as in 1.1.
           Expected: GT_BAD_PARAM.  */

        interfaceInfo.type = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
            "%d, interfaceInfoPtr->type = %d", dev, interfaceInfo.type);

        interfaceInfo.type = CPSS_INTERFACE_PORT_E;

        /* 1.4. Call function with interfaceInfoPtr->type
           [CPSS_INTERFACE_PORT_E], out of range interfaceInfoPtr->devPort.
           portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] and other  parameters the
           same as in 1.1. Expected: NON GT_OK. */

        interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        interfaceInfo.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
            "%d, interfaceInfo.devPort.portNum = %d", dev, interfaceInfo.devPort.portNum);

        /* 1.5. Call function with interfaceInfoPtr->type
           [CPSS_INTERFACE_VID_E], out of range interfaceInfoPtr->vlanId
           [PRV_CPSS_MAX_NUM_VLANS_CNS] and other  parameters the same as
           in 1.1. Expected: GT_BAD_PARAM.  */

        interfaceInfo.type = CPSS_INTERFACE_VID_E;
        interfaceInfo.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
            "%d, interfaceInfo.vlanId = %d", dev, interfaceInfo.vlanId);
        /*
            1.6. Call with with interfaceInfoPtr->type [CPSS_INTERFACE_TRUNK_E / CPSS_INTERFACE_VIDX_E /
                                                        CPSS_INTERFACE_DEVICE_E / CPSS_INTERFACE_FABRIC_VIDX_E] not supported.
            Expectedc: NOT GT_OK.
        */
        interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        interfaceInfo.trunkId = 100;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, interfaceInfo.type = %d", dev, interfaceInfo.type);

        interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
        interfaceInfo.vidx = 100;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, interfaceInfo.type = %d", dev, interfaceInfo.type);

        interfaceInfo.type = CPSS_INTERFACE_DEVICE_E;
        interfaceInfo.devNum = dev;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, interfaceInfo.type = %d", dev, interfaceInfo.type);

        interfaceInfo.type = CPSS_INTERFACE_FABRIC_VIDX_E;
        interfaceInfo.fabricVidx = 100;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, interfaceInfo.type = %d", dev, interfaceInfo.type);

        /* 1.7. Call function with interfaceInfoPtr [NULL] and other
           parameters the same as in 1.1. Expected: GT_BAD_PTR. */

        st = cpssExMxPmPclCfgTblSet(dev, NULL, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, interfaceInfoPtr = NULL", dev);

        /* 1.8. Call function with out of range direction [0x5AAAAAA5]
           and other  parameters the same as in 1.1. Expected: GT_BAD_PARAM.*/

        direction = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                                     "%d, direction = %d", dev, direction);

        direction = CPSS_PCL_DIRECTION_INGRESS_E;

        /* 1.9. Call function with out of range lookupNum [0x5AAAAAA5]
           and other  parameters the same as in 1.1. Expected: GT_BAD_PARAM.*/

        lookupNum = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                                     "%d, lookupNum = %d", dev, lookupNum);

        lookupNum = CPSS_PCL_LOOKUP_0_E;

        /* 1.10. Call function with lookupCfgPtr->enableLookup [GT_TRUE],
           out of range lookupCfgPtr->pclId [1024] and other  parameters the
           same as in 1.1. Expected: NON GT_OK. */

        lookupCfg.enableLookup = GT_TRUE;
        lookupCfg.pclId = 1024;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                     "%d, lookupCfg.pclId = %d", dev, lookupCfg.pclId);

        /* 1.11. Call function with lookupCfgPtr->enableLookup [GT_TRUE],
           out of range lookupCfgPtr->nonIpKey [0x5AAAAAA5] and other
           parameters the same as in 1.1. Expected: GT_BAD_PARAM.   */

        lookupCfg.enableLookup = GT_TRUE;
        lookupCfg.groupKeyTypes.nonIpKey = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                                     "%d, lookupCfg.groupKeyTypes.nonIpKey = %d", dev,
                                     lookupCfg.groupKeyTypes.nonIpKey);

        lookupCfg.groupKeyTypes.nonIpKey = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E;

        /* 1.12. Call function with lookupCfgPtr->enableLookup [GT_TRUE],
           lookupCfgPtr->ipv4Key [CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E]
           (invalid) and other  parameters the same as in 1.1. Expected: NON GT_OK.*/

        lookupCfg.enableLookup = GT_TRUE;
        lookupCfg.groupKeyTypes.ipv4Key = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                     "%d, lookupCfg.groupKeyTypes.ipv4Key = %d",
                                        dev, lookupCfg.groupKeyTypes.ipv4Key);

        /* 1.13. Call function with lookupCfgPtr->enableLookup [GT_FALSE],
           lookupCfgPtr->ipv4Key
           [CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E] (invalid)
           and other  parameters the same as in 1.1. Expected: NON GT_OK.   */
        /* Invalid ipv4Key causes non-GT_OK status returned by CPSS PCL driver */
        lookupCfg.enableLookup = GT_FALSE;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
               "%d, lookupCfg.enableLookup = %d, lookupCfg.groupKeyTypes.ipv4Key = %d",
               dev, lookupCfg.enableLookup, lookupCfg.groupKeyTypes.ipv4Key);

        lookupCfg.groupKeyTypes.ipv4Key = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E;

        /* 1.14. Call function with lookupCfgPtr->enableLookup [GT_TRUE],
           out of range lookupCfgPtr->ipv4Key [0x5AAAAAA5](invalid) and other
           parameters the same as in 1.1. Expected: GT_BAD_PARAM.   */

        lookupCfg.enableLookup = GT_TRUE;
        lookupCfg.groupKeyTypes.ipv4Key = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                                     "%d, lookupCfg.groupKeyTypes.ipv4Key = %d", dev,
                                     lookupCfg.groupKeyTypes.ipv4Key);

        lookupCfg.groupKeyTypes.ipv4Key = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E;

        /* 1.15. Call function with lookupCfgPtr->enableLookup [GT_TRUE],
           lookupCfgPtr->ipv6Key
           [CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_NOT_IPV6_E](invalid)
           and other parameters the same as in 1.1. Expected: NON GT_OK.  */

        lookupCfg.enableLookup = GT_TRUE;
        lookupCfg.groupKeyTypes.ipv6Key = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_NOT_IPV6_E;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                     "%d, lookupCfg.groupKeyTypes.ipv6Key = %d", dev,
                                         lookupCfg.groupKeyTypes.ipv6Key);

        /* 1.16. Call function with lookupCfgPtr->enableLookup [GT_FALSE],
           lookupCfgPtr->groupKeyTypes.ipv6Key
           [CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_NOT_IPV6_E] (invalid)
           and other parameters the same as in 1.1. Expected: NON GT_OK.  */
        /* Invalid ipv6Key causes non-GT_OK status returned by CPSS PCL driver */
        lookupCfg.enableLookup = GT_FALSE;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
               "%d, lookupCfg.enableLookup = %d, lookupCfg.groupKeyTypes.ipv6Key = %d",
               dev, lookupCfg.enableLookup, lookupCfg.groupKeyTypes.ipv6Key);

        /* 1.17. Call function with lookupCfgPtr->enableLookup [GT_TRUE],
           out of range lookupCfgPtr->ipv6Key [0x5AAAAAA5] and other
           parameters the same as in 1.1. Expected: GT_BAD_PARAM.   */

        lookupCfg.enableLookup = GT_TRUE;
        lookupCfg.groupKeyTypes.ipv6Key = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                                     "%d, lookupCfg.groupKeyTypes.ipv6Key = %d", dev,
                                     lookupCfg.groupKeyTypes.ipv6Key);

        lookupCfg.groupKeyTypes.ipv6Key = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E;


        /* 1.18. Call function with lookupCfgPtr [NULL] and other  parameters
           the same as in 1.1. Expected: GT_BAD_PTR.    */

        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, lookupCfgPtr = NULL", dev);
    }

    cpssOsBzero((GT_VOID*)&interfaceInfo, sizeof(CPSS_INTERFACE_INFO_STC));
    cpssOsBzero((GT_VOID*)&lookupCfg, sizeof(CPSS_EXMXPM_PCL_LOOKUP_CFG_STC));

    interfaceInfo.type = CPSS_INTERFACE_PORT_E;

    interfaceInfo.devPort.devNum = 0;
    interfaceInfo.devPort.portNum = 8;

    interfaceInfo.trunkId = 1;
    interfaceInfo.vidx = 100;
    interfaceInfo.vlanId = 100;

    direction = CPSS_PCL_DIRECTION_INGRESS_E;
    lookupNum = CPSS_PCL_LOOKUP_0_E;

    lookupCfg.enableLookup = GT_FALSE;
    lookupCfg.pclId = 2;

    lookupCfg.groupKeyTypes.nonIpKey = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E;
    lookupCfg.groupKeyTypes.ipv4Key  = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E;
    lookupCfg.groupKeyTypes.ipv6Key  = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclCfgTblGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_INTERFACE_INFO_STC           *interfaceInfoPtr,
    IN  CPSS_PCL_DIRECTION_ENT            direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT        lookupNum,
    OUT CPSS_EXMXPM_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclCfgTblGet)
{
/*
ITERATE_DEVICES (ExMxPm)
1.1. Call function with non-NULL interfaceInfoPtr { type [CPSS_INTERFACE_PORT_E / CPSS_INTERFACE_VID_E]; devPort { devNum [32]; portNum [8]}; trunkId [16]; vidx [100]; vlanId [100] (accordingly to the type) }, direction [CPSS_PCL_DIRECTION_INGRESS_E / CPSS_PCL_DIRECTION_EGRESS_E], lookupNum [CPSS_PCL_LOOKUP_0_E / CPSS_PCL_LOOKUP_1_E] and non-NULL lookupCfgPtr. Expected: GT_OK.
1.2. Call function with out of range interfaceInfoPtr->type [0x5AAAAAA5] and other  parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.3. Call function with interfaceInfoPtr->type [CPSS_INTERFACE_PORT_E], out of range interfaceInfoPtr->devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] and other  parameters the same as in 1.1. Expected: NON GT_OK.
1.4. Call function with interfaceInfoPtr->type [CPSS_INTERFACE_VID_E], out of range interfaceInfoPtr->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] and other  parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.5. Call with with interfaceInfoPtr->type [CPSS_INTERFACE_TRUNK_E / CPSS_INTERFACE_VIDX_E / CPSS_INTERFACE_DEVICE_E / CPSS_INTERFACE_FABRIC_VIDX_E] not supported.Expectedc: NOT GT_OK.
1.6. Call function with interfaceInfoPtr [NULL] and other  parameters the same as in 1.1. Expected: GT_BAD_PTR.
1.7. Call function with out of range direction [0x5AAAAAA5] and other  parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.8. Call function with out of range lookupNum [0x5AAAAAA5] and other  parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.9. Call function with lookupCfgPtr [NULL] and other  parameters the same as in 1.1. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_INTERFACE_INFO_STC           interfaceInfo;
    CPSS_PCL_DIRECTION_ENT            direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT        lookupNum;
    CPSS_EXMXPM_PCL_LOOKUP_CFG_STC    lookupCfg;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL interfaceInfoPtr { type
           [CPSS_INTERFACE_PORT_E / CPSS_INTERFACE_TRUNK_E /
           CPSS_INTERFACE_VIDX_E / CPSS_INTERFACE_VID_E]; devPort
           { devNum [32]; portNum [8]}; trunkId [16]; vidx [100];
           vlanId [100] (accordingly to the type) }, direction
           [CPSS_PCL_DIRECTION_INGRESS_E / CPSS_PCL_DIRECTION_EGRESS_E],
           lookupNum [CPSS_PCL_LOOKUP_0_E / CPSS_PCL_LOOKUP_1_E] and
           non-NULL lookupCfgPtr. Expected: GT_OK. */

        cpssOsBzero((GT_VOID*)&interfaceInfo, sizeof(CPSS_INTERFACE_INFO_STC));

        interfaceInfo.type = CPSS_INTERFACE_PORT_E;

        interfaceInfo.devPort.devNum = 0;
        interfaceInfo.devPort.portNum = 8;

        interfaceInfo.trunkId = 1;
        interfaceInfo.vidx = 100;
        interfaceInfo.vlanId = 100;

        direction = CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum = CPSS_PCL_LOOKUP_0_E;

        st = cpssExMxPmPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);


        interfaceInfo.type = CPSS_INTERFACE_VID_E;

        st = cpssExMxPmPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);


        interfaceInfo.type = CPSS_INTERFACE_VID_E;

        direction = CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum = CPSS_PCL_LOOKUP_1_E;

        st = cpssExMxPmPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* restore */
        interfaceInfo.type = CPSS_INTERFACE_PORT_E;

        direction = CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum = CPSS_PCL_LOOKUP_0_E;

        /* 1.2. Call function with out of range interfaceInfoPtr->type
           [0x5AAAAAA5] and other  parameters the same as in 1.1.
           Expected: GT_BAD_PARAM.  */

        interfaceInfo.type = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
            "%d, interfaceInfoPtr->type = %d", dev, interfaceInfo.type);

        interfaceInfo.type = CPSS_INTERFACE_PORT_E;

        /* 1.3. Call function with interfaceInfoPtr->type
           [CPSS_INTERFACE_PORT_E], out of range interfaceInfoPtr->devPort.
           portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] and other  parameters the
           same as in 1.1. Expected: NON GT_OK. */

        interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        interfaceInfo.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
            "%d, interfaceInfo.devPort.portNum = %d", dev, interfaceInfo.devPort.portNum);

        interfaceInfo.devPort.portNum = 0;

        /* 1.4. Call function with interfaceInfoPtr->type
           [CPSS_INTERFACE_VID_E], out of range interfaceInfoPtr->vlanId
           [PRV_CPSS_MAX_NUM_VLANS_CNS] and other  parameters the same as
           in 1.1. Expected: GT_BAD_PARAM.  */

        interfaceInfo.type = CPSS_INTERFACE_VID_E;
        interfaceInfo.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
            "%d, interfaceInfo.vlanId = %d", dev, interfaceInfo.vlanId);

        /*
            1.5. Call with with interfaceInfoPtr->type [CPSS_INTERFACE_TRUNK_E / CPSS_INTERFACE_VIDX_E /
                                                        CPSS_INTERFACE_DEVICE_E / CPSS_INTERFACE_FABRIC_VIDX_E] not supported.
            Expectedc: NOT GT_OK.
        */
        interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        interfaceInfo.trunkId = 100;

        st = cpssExMxPmPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, interfaceInfo.type = %d", dev, interfaceInfo.type);

        interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
        interfaceInfo.vidx = 100;

        st = cpssExMxPmPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, interfaceInfo.type = %d", dev, interfaceInfo.type);

        interfaceInfo.type = CPSS_INTERFACE_DEVICE_E;
        interfaceInfo.devNum = dev;

        st = cpssExMxPmPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, interfaceInfo.type = %d", dev, interfaceInfo.type);

        interfaceInfo.type = CPSS_INTERFACE_FABRIC_VIDX_E;
        interfaceInfo.fabricVidx = 100;

        st = cpssExMxPmPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, interfaceInfo.type = %d", dev, interfaceInfo.type);

        /* 1.6. Call function with interfaceInfoPtr [NULL] and other
           parameters the same as in 1.1. Expected: GT_BAD_PTR. */

        st = cpssExMxPmPclCfgTblGet(dev, NULL, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, interfaceInfoPtr = NULL", dev);

        /* 1.7. Call function with out of range direction [0x5AAAAAA5]
           and other  parameters the same as in 1.1. Expected: GT_BAD_PARAM.*/

        direction = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                                     "%d, direction = %d", dev, direction);

        direction = CPSS_PCL_DIRECTION_INGRESS_E;

        /* 1.8. Call function with out of range lookupNum [0x5AAAAAA5]
           and other  parameters the same as in 1.1. Expected: GT_BAD_PARAM.*/

        lookupNum = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                                     "%d, lookupNum = %d", dev, lookupNum);

        lookupNum = CPSS_PCL_LOOKUP_0_E;

        /* 1.9. Call function with lookupCfgPtr [NULL] and other  parameters
           the same as in 1.1. Expected: GT_BAD_PTR.    */

        st = cpssExMxPmPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, lookupCfgPtr = NULL", dev);
    }

    cpssOsBzero((GT_VOID*)&interfaceInfo, sizeof(CPSS_INTERFACE_INFO_STC));

    interfaceInfo.type = CPSS_INTERFACE_PORT_E;

    interfaceInfo.devPort.devNum = 0;
    interfaceInfo.devPort.portNum = 8;

    interfaceInfo.trunkId = 1;
    interfaceInfo.vidx = 100;
    interfaceInfo.vlanId = 100;

    direction = CPSS_PCL_DIRECTION_INGRESS_E;
    lookupNum = CPSS_PCL_LOOKUP_0_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclInvalidUdbCmdSet
(
    IN GT_U8                          devNum,
    IN CPSS_EXMXPM_UDB_ERROR_CMD_ENT  udbErrorCmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclInvalidUdbCmdSet)
{
/*
ITERATE_DEVICES (ExMxPm)
1.1. Call function with udbErrorCmd [CPSS_EXMXPM_UDB_ERROR_CMD_LOOKUP_E / CPSS_EXMXPM_UDB_ERROR_CMD_TRAP_TO_CPU_E / CPSS_EXMXPM_UDB_ERROR_CMD_DROP_HARD_E / CPSS_EXMXPM_UDB_ERROR_CMD_DROP_SOFT_E]. Expected: GT_OK.
1.2. Call cpssExMxPmPclInvalidUdbCmdGet with non-NULL udbErrorCmdPtr. Expected: GT_OK and the same udbErrorCmd.
1.3. Call function with out of range udbErrorCmd [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_EXMXPM_UDB_ERROR_CMD_ENT  udbErrorCmd;
    CPSS_EXMXPM_UDB_ERROR_CMD_ENT  udbErrorCmdRet;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with udbErrorCmd [CPSS_EXMXPM_UDB_ERROR_CMD_LOOKUP_E
           / CPSS_EXMXPM_UDB_ERROR_CMD_TRAP_TO_CPU_E /
           CPSS_EXMXPM_UDB_ERROR_CMD_DROP_HARD_E /
           CPSS_EXMXPM_UDB_ERROR_CMD_DROP_SOFT_E]. Expected: GT_OK. */

        udbErrorCmd = CPSS_EXMXPM_UDB_ERROR_CMD_LOOKUP_E;

        st = cpssExMxPmPclInvalidUdbCmdSet(dev, udbErrorCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, udbErrorCmd);

        udbErrorCmd = CPSS_EXMXPM_UDB_ERROR_CMD_TRAP_TO_CPU_E;

        st = cpssExMxPmPclInvalidUdbCmdSet(dev, udbErrorCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, udbErrorCmd);

        udbErrorCmd = CPSS_EXMXPM_UDB_ERROR_CMD_DROP_HARD_E;

        st = cpssExMxPmPclInvalidUdbCmdSet(dev, udbErrorCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, udbErrorCmd);

        udbErrorCmd = CPSS_EXMXPM_UDB_ERROR_CMD_DROP_SOFT_E;

        st = cpssExMxPmPclInvalidUdbCmdSet(dev, udbErrorCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, udbErrorCmd);

        /* 1.2. Call cpssExMxPmPclInvalidUdbCmdGet with non-NULL
           udbErrorCmdPtr. Expected: GT_OK and the same udbErrorCmd.    */

        st = cpssExMxPmPclInvalidUdbCmdGet(dev, &udbErrorCmdRet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPclInvalidUdbCmdGet: %d", dev);

        if (GT_OK == st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(udbErrorCmd, udbErrorCmdRet,
             "cpssExMxPmPclInvalidUdbCmdGet: get another udbErrorCmd than was set: dev = %d", dev);
        }

        /* 1.3. Call function with out of range udbErrorCmd [0x5AAAAAA5].
           Expected: GT_BAD_PARAM.  */

        udbErrorCmd = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclInvalidUdbCmdSet(dev, udbErrorCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, udbErrorCmd);

    }

    udbErrorCmd = CPSS_EXMXPM_UDB_ERROR_CMD_LOOKUP_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclInvalidUdbCmdSet(dev, udbErrorCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclInvalidUdbCmdSet(dev, udbErrorCmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclInvalidUdbCmdGet
(
    IN  GT_U8                          devNum,
    OUT CPSS_EXMXPM_UDB_ERROR_CMD_ENT  *udbErrorCmdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclInvalidUdbCmdGet)
{
/*
ITERATE_DEVICES (ExMxPm)
1.1. Call function with non-NULL udbErrorCmdPtr. Expected: GT_OK.
1.2. Call function with udbErrorCmdPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_EXMXPM_UDB_ERROR_CMD_ENT  udbErrorCmd;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL udbErrorCmdPtr. Expected: GT_OK.*/

        st = cpssExMxPmPclInvalidUdbCmdGet(dev, &udbErrorCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with udbErrorCmdPtr [NULL].
           Expected: GT_BAD_PTR.    */

        st = cpssExMxPmPclInvalidUdbCmdGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, udbErrorCmdPtr = NULL", dev);

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclInvalidUdbCmdGet(dev, &udbErrorCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclInvalidUdbCmdGet(dev, &udbErrorCmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclTcpUdpPortComparatorSet
(
    IN GT_U8                             devNum,
    IN CPSS_PCL_DIRECTION_ENT            direction,
    IN CPSS_L4_PROTOCOL_ENT              l4Protocol,
    IN GT_U32                            entryIndex,
    IN CPSS_L4_PROTOCOL_PORT_TYPE_ENT    l4PortType,
    IN CPSS_COMPARE_OPERATOR_ENT         compareOperator,
    IN GT_U16                            value
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclTcpUdpPortComparatorSet)
{
/*
ITERATE_DEVICES (ExMxPm)
1.1. Call function with direction [CPSS_PCL_DIRECTION_INGRESS_E / CPSS_PCL_DIRECTION_EGRESS_E], l4Protocol [CPSS_L4_PROTOCOL_TCP_E / CPSS_L4_PROTOCOL_UDP_E], entryIndex [1 / 7], l4PortType [CPSS_L4_PROTOCOL_PORT_SRC_E / CPSS_L4_PROTOCOL_PORT_DST_E], compareOperator [CPSS_COMPARE_OPERATOR_LTE /  CPSS_COMPARE_OPERATOR_GTE /  CPSS_COMPARE_OPERATOR_NEQ] and value [100 / 1000 / 10000 / 65535]. Expected: GT_OK.
1.2. Call cpssExMxPmPclTcpUdpPortComparatorGet with non-NULL l4PortTypePtr, non-NULL compareOperatorPtr, non-NULL valuePtr and other parameters the same as in 1.1. Expected: GT_OK and the same l4PortType, compareOperator and value.
1.3. Call function with out of range direction [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.4. Call function with out of range l4Protocol [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.5. Call function with out of range entryIndex [8] and other parameters the same as in 1.1. Expected: NON GT_OK.
1.6. Call function with out of range l4PortType [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.7. Call function with out of range compareOperator  [CPSS_COMPARE_OPERATOR_INVALID_E] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.8. Call function with value [0xFFFF] and other parameters the same as in 1.1. Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_PCL_DIRECTION_ENT            direction;
    CPSS_L4_PROTOCOL_ENT              l4Protocol;
    GT_U32                            entryIndex;
    CPSS_L4_PROTOCOL_PORT_TYPE_ENT    l4PortType;
    CPSS_COMPARE_OPERATOR_ENT         compareOperator;
    GT_U16                            value;

    CPSS_L4_PROTOCOL_PORT_TYPE_ENT    l4PortTypeRet;
    CPSS_COMPARE_OPERATOR_ENT         compareOperatorRet;
    GT_U16                            valueRet;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with direction [CPSS_PCL_DIRECTION_INGRESS_E /
           CPSS_PCL_DIRECTION_EGRESS_E], l4Protocol [CPSS_L4_PROTOCOL_TCP_E /
           CPSS_L4_PROTOCOL_UDP_E], entryIndex [1 / 7], l4PortType
           [CPSS_L4_PROTOCOL_PORT_SRC_E / CPSS_L4_PROTOCOL_PORT_DST_E],
           compareOperator [CPSS_COMPARE_OPERATOR_LTE /
           CPSS_COMPARE_OPERATOR_GTE /  CPSS_COMPARE_OPERATOR_NEQ /
           CPSS_COMPARE_OPERATOR_INVALID_E] and value
           [100 / 1000 / 10000 / 65535]. Expected: GT_OK.   */

        direction = CPSS_PCL_DIRECTION_INGRESS_E;
        l4Protocol = CPSS_L4_PROTOCOL_TCP_E;
        entryIndex = 1;
        l4PortType = CPSS_L4_PROTOCOL_PORT_SRC_E;
        compareOperator = CPSS_COMPARE_OPERATOR_LTE;
        value = 100;

        st = cpssExMxPmPclTcpUdpPortComparatorSet(dev, direction, l4Protocol,
                             entryIndex, l4PortType, compareOperator, value);
        UTF_VERIFY_EQUAL7_PARAM_MAC(GT_OK, st, dev, direction, l4Protocol,
                             entryIndex, l4PortType, compareOperator, value);

        compareOperator = CPSS_COMPARE_OPERATOR_INVALID_E;
        value = 10000;

        st = cpssExMxPmPclTcpUdpPortComparatorSet(dev, direction, l4Protocol,
                             entryIndex, l4PortType, compareOperator, value);
        UTF_VERIFY_EQUAL7_PARAM_MAC(GT_OK, st, dev, direction, l4Protocol,
                             entryIndex, l4PortType, compareOperator, value);

        direction = CPSS_PCL_DIRECTION_EGRESS_E;
        l4Protocol = CPSS_L4_PROTOCOL_UDP_E;
        entryIndex = 7;
        l4PortType = CPSS_L4_PROTOCOL_PORT_DST_E;
        compareOperator = CPSS_COMPARE_OPERATOR_GTE;
        value = 1000;

        st = cpssExMxPmPclTcpUdpPortComparatorSet(dev, direction, l4Protocol,
                             entryIndex, l4PortType, compareOperator, value);
        UTF_VERIFY_EQUAL7_PARAM_MAC(GT_OK, st, dev, direction, l4Protocol,
                             entryIndex, l4PortType, compareOperator, value);

        entryIndex = 0;
        compareOperator = CPSS_COMPARE_OPERATOR_NEQ;
        value = 65535;

        st = cpssExMxPmPclTcpUdpPortComparatorSet(dev, direction, l4Protocol,
                             entryIndex, l4PortType, compareOperator, value);
        UTF_VERIFY_EQUAL7_PARAM_MAC(GT_OK, st, dev, direction, l4Protocol,
                             entryIndex, l4PortType, compareOperator, value);

        /* 1.2. Call cpssExMxPmPclTcpUdpPortComparatorGet with non-NULL
           l4PortTypePtr, non-NULL compareOperatorPtr, non-NULL valuePtr and
           other parameters the same as in 1.1. Expected: GT_OK and the same
           l4PortType, compareOperator and value.   */

        st = cpssExMxPmPclTcpUdpPortComparatorGet(dev, direction, l4Protocol,
                             entryIndex, &l4PortTypeRet, &compareOperatorRet, &valueRet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPclTcpUdpPortComparatorGet: %d", dev);

        if (GT_OK == st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(l4PortType, l4PortTypeRet,
             "cpssExMxPmPclTcpUdpPortComparatorGet: get another l4PortType than was set: dev = %d", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(compareOperator, compareOperatorRet,
             "cpssExMxPmPclTcpUdpPortComparatorGet: get another compareOperator than was set: dev = %d", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(value, valueRet,
             "cpssExMxPmPclTcpUdpPortComparatorGet: get another value than was set: dev = %d", dev);
        }

        /* 1.3. Call function with out of range direction [0x5AAAAAA5] and
           other parameters the same as in 1.1. Expected: GT_BAD_PARAM. */

        direction = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclTcpUdpPortComparatorSet(dev, direction, l4Protocol,
                             entryIndex, l4PortType, compareOperator, value);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, direction = %d",
                                     dev, direction);

        direction = CPSS_PCL_DIRECTION_INGRESS_E;

        /* 1.4. Call function with out of range l4Protocol [0x5AAAAAA5] and
           other parameters the same as in 1.1. Expected: GT_BAD_PARAM. */

        l4Protocol = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclTcpUdpPortComparatorSet(dev, direction, l4Protocol,
                             entryIndex, l4PortType, compareOperator, value);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, l4Protocol = %d",
                                     dev, l4Protocol);

        l4Protocol = CPSS_L4_PROTOCOL_TCP_E;

        /* 1.5. Call function with out of range entryIndex [8] and other
           parameters the same as in 1.1. Expected: NON GT_OK.  */

        entryIndex = 8;

        st = cpssExMxPmPclTcpUdpPortComparatorSet(dev, direction, l4Protocol,
                             entryIndex, l4PortType, compareOperator, value);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, entryIndex = %d",
                                     dev, entryIndex);

        entryIndex = 1;

        /* 1.6. Call function with out of range l4PortType [0x5AAAAAA5] and
           other parameters the same as in 1.1. Expected: GT_BAD_PARAM. */

        l4PortType = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclTcpUdpPortComparatorSet(dev, direction, l4Protocol,
                             entryIndex, l4PortType, compareOperator, value);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, l4PortType = %d",
                                     dev, l4PortType);

        l4PortType = CPSS_L4_PROTOCOL_PORT_SRC_E;

        /* 1.7. Call function with out of range compareOperator
          [0x5AAAAAA5] and other parameters the
           same as in 1.1. Expected: GT_BAD_PARAM.  */

        compareOperator = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclTcpUdpPortComparatorSet(dev, direction, l4Protocol,
                             entryIndex, l4PortType, compareOperator, value);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, compareOperator = %d",
                                     dev, compareOperator);

        compareOperator = CPSS_COMPARE_OPERATOR_LTE;

        /*  1.8. Call function with value [0xFFFF] and other parameters
            the same as in 1.1. Expected: GT_OK.    */

        st = cpssExMxPmPclTcpUdpPortComparatorSet(dev, direction, l4Protocol,
                             entryIndex, l4PortType, compareOperator, value);
        UTF_VERIFY_EQUAL7_PARAM_MAC(GT_OK, st, dev, direction, l4Protocol,
                             entryIndex, l4PortType, compareOperator, value);
    }

    direction = CPSS_PCL_DIRECTION_INGRESS_E;
    l4Protocol = CPSS_L4_PROTOCOL_TCP_E;
    entryIndex = 1;
    l4PortType = CPSS_L4_PROTOCOL_PORT_SRC_E;
    compareOperator = CPSS_COMPARE_OPERATOR_LTE;
    value = 100;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclTcpUdpPortComparatorSet(dev, direction, l4Protocol,
                             entryIndex, l4PortType, compareOperator, value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclTcpUdpPortComparatorSet(dev, direction, l4Protocol,
                         entryIndex, l4PortType, compareOperator, value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclTcpUdpPortComparatorGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_PCL_DIRECTION_ENT            direction,
    IN  CPSS_L4_PROTOCOL_ENT              l4Protocol,
    IN  GT_U32                            entryIndex,
    OUT CPSS_L4_PROTOCOL_PORT_TYPE_ENT    *l4PortTypePtr,
    OUT CPSS_COMPARE_OPERATOR_ENT         *compareOperatorPtr,
    OUT GT_U16                            *valuePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclTcpUdpPortComparatorGet)
{
/*
ITERATE_DEVICES (ExMxPm)
1.1. Call function with direction [CPSS_PCL_DIRECTION_INGRESS_E / CPSS_PCL_DIRECTION_EGRESS_E], l4Protocol [CPSS_L4_PROTOCOL_TCP_E / CPSS_L4_PROTOCOL_UDP_E], entryIndex [1 / 7], non-NULL l4PortTypePtr, non-NULL compareOperatorPtr and non-NULL valuePtr. Expected: GT_OK.
1.2. Call function with out of range direction [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.3. Call function with out of range l4Protocol [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.4. Call function with out of range entryIndex [8] and other parameters the same as in 1.1. Expected: NON GT_OK.
1.5. Call function with l4PortTypePtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
1.6. Call function with compareOperatorPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
1.7. Call function with valuePtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_PCL_DIRECTION_ENT            direction;
    CPSS_L4_PROTOCOL_ENT              l4Protocol;
    GT_U32                            entryIndex;
    CPSS_L4_PROTOCOL_PORT_TYPE_ENT    l4PortType;
    CPSS_COMPARE_OPERATOR_ENT         compareOperator;
    GT_U16                            value;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with direction [CPSS_PCL_DIRECTION_INGRESS_E /
           CPSS_PCL_DIRECTION_EGRESS_E], l4Protocol [CPSS_L4_PROTOCOL_TCP_E /
           CPSS_L4_PROTOCOL_UDP_E], entryIndex [1 / 7], non-NULL l4PortTypePtr,
           non-NULL compareOperatorPtr and non-NULL valuePtr. Expected: GT_OK.*/

        direction = CPSS_PCL_DIRECTION_INGRESS_E;
        l4Protocol = CPSS_L4_PROTOCOL_TCP_E;
        entryIndex = 1;

        st = cpssExMxPmPclTcpUdpPortComparatorGet(dev, direction, l4Protocol,
                      entryIndex, &l4PortType, &compareOperator, &value);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, direction, l4Protocol,
                                    entryIndex);

        direction = CPSS_PCL_DIRECTION_EGRESS_E;
        l4Protocol = CPSS_L4_PROTOCOL_UDP_E;
        entryIndex = 7;

        st = cpssExMxPmPclTcpUdpPortComparatorGet(dev, direction, l4Protocol,
                      entryIndex, &l4PortType, &compareOperator, &value);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, direction, l4Protocol,
                                    entryIndex);

        /* 1.2. Call function with out of range direction [0x5AAAAAA5] and
           other parameters the same as in 1.1. Expected: GT_BAD_PARAM. */

        direction = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclTcpUdpPortComparatorGet(dev, direction, l4Protocol,
                      entryIndex, &l4PortType, &compareOperator, &value);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, direction = %d",
                                     dev, direction);

        direction = CPSS_PCL_DIRECTION_INGRESS_E;

        /* 1.3. Call function with out of range l4Protocol [0x5AAAAAA5]
           and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.*/

        l4Protocol = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclTcpUdpPortComparatorGet(dev, direction, l4Protocol,
                      entryIndex, &l4PortType, &compareOperator, &value);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, l4Protocol = %d",
                                     dev, l4Protocol);

        l4Protocol = CPSS_L4_PROTOCOL_TCP_E;

        /* 1.4. Call function with out of range entryIndex [8] and other
           parameters the same as in 1.1. Expected: NON GT_OK.  */

        entryIndex = 8;

        st = cpssExMxPmPclTcpUdpPortComparatorGet(dev, direction, l4Protocol,
                      entryIndex, &l4PortType, &compareOperator, &value);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, entryIndex = %d",
                                     dev, entryIndex);

        entryIndex = 1;

        /* 1.5. Call function with l4PortTypePtr [NULL] and other parameters
           the same as in 1.1. Expected: GT_BAD_PTR.    */

        st = cpssExMxPmPclTcpUdpPortComparatorGet(dev, direction, l4Protocol,
                      entryIndex, NULL, &compareOperator, &value);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, l4PortTypePtr = NULL", dev);

        /* 1.6. Call function with compareOperatorPtr [NULL] and other
           parameters the same as in 1.1. Expected: GT_BAD_PTR. */

        st = cpssExMxPmPclTcpUdpPortComparatorGet(dev, direction, l4Protocol,
                      entryIndex, &l4PortType, NULL, &value);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, compareOperatorPtr = NULL", dev);

        /* 1.7. Call function with valuePtr [NULL] and other parameters the
           same as in 1.1. Expected: GT_BAD_PTR.    */

        st = cpssExMxPmPclTcpUdpPortComparatorGet(dev, direction, l4Protocol,
                      entryIndex, &l4PortType, &compareOperator, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, valuePtr = NULL", dev);
    }

    direction = CPSS_PCL_DIRECTION_INGRESS_E;
    l4Protocol = CPSS_L4_PROTOCOL_TCP_E;
    entryIndex = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclTcpUdpPortComparatorGet(dev, direction, l4Protocol,
                      entryIndex, &l4PortType, &compareOperator, &value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclTcpUdpPortComparatorGet(dev, direction, l4Protocol,
                  entryIndex, &l4PortType, &compareOperator, &value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclProfileAssignmentModeSet
(
    IN GT_U8                                        devNum,
    IN CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENT  mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclProfileAssignmentModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mode [CPSS_CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENHANCED_E / CPSS_CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_IGNORANT_E / CPSS_CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_STANDARD_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmPclProfileAssignmentModeGet with not NULL modePtr.
    Expected: GT_OK and the same mode as was set.
    1.3. Call with mode [0x5AAAAAA5] out of range.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENT  mode;
    CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENT  modeGet;


    mode    = CPSS_CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENHANCED_E;
    modeGet = CPSS_CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENHANCED_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mode [CPSS_CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENHANCED_E /
                                 CPSS_CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_IGNORANT_E /
                                 CPSS_CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_STANDARD_E].
            Expected: GT_OK.
        */
        /* iterate with mode = CPSS_CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENHANCED_E */
        mode = CPSS_CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENHANCED_E;

        st = cpssExMxPmPclProfileAssignmentModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmPclProfileAssignmentModeGet with not NULL modePtr.
            Expected: GT_OK and the same mode as was set.
        */
        st = cpssExMxPmPclProfileAssignmentModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPclProfileAssignmentModeGet: %d", dev);

        /* iterate with mode = CPSS_CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_IGNORANT_E */
        mode = CPSS_CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_IGNORANT_E;

        st = cpssExMxPmPclProfileAssignmentModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmPclProfileAssignmentModeGet with not NULL modePtr.
            Expected: GT_OK and the same mode as was set.
        */
        st = cpssExMxPmPclProfileAssignmentModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPclProfileAssignmentModeGet: %d", dev);

        /* iterate with mode = CPSS_CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_STANDARD_E */
        mode = CPSS_CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_STANDARD_E;

        st = cpssExMxPmPclProfileAssignmentModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmPclProfileAssignmentModeGet with not NULL modePtr.
            Expected: GT_OK and the same mode as was set.
        */
        st = cpssExMxPmPclProfileAssignmentModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPclProfileAssignmentModeGet: %d", dev);

        /*
            1.3. Call with mode [0x5AAAAAA5] out of range.
            Expected: GT_BAD_PARAM.
        */
        mode = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclProfileAssignmentModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENHANCED_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclProfileAssignmentModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclProfileAssignmentModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclProfileAssignmentModeGet
(
    IN  GT_U8                                        devNum,
    OUT CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENT  *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclProfileAssignmentModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL modePtr.
    Expected: GT_OK.
    1.2. Call with modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENT  mode;


    mode    = CPSS_CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENHANCED_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL modePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPclProfileAssignmentModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclProfileAssignmentModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, modePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclProfileAssignmentModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclProfileAssignmentModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclUdbTableEntrySet
(
    IN GT_U8                                 devNum,
    IN GT_U32                                entryIndex,
    IN CPSS_EXMXPM_PCL_UDB_TABLE_ENTRY_STC  *udbCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclUdbTableEntrySet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with entryIndex[0],
                   udbCfgPtr {udbCfg[23]{valid [GT_TRUE],
                                         offsetType [CPSS_EXMXPM_PCL_OFFSET_L2_E],
                                         offset [0],
                                         tcpUdpComparatorsEnable [GT_FALSE] }
                              inlifIdEnable [GT_FALSE],
                              profileIdEnable [GT_FALSE] }
    Expected: GT_OK.
    1.2. Call cpssExMxPmPclUdbTableEntryGet with not NULL udbCfgPtr and other params from 1.1.
    Expected: GT_OK and the same udbCfgPtr as was set.
    1.3. Call with udbCfgPtr->udbCfg[0].offsetType[0x5AAAAAA5] and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with udbCfgPtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       iTemp   = 0;
    GT_BOOL     isEqual = GT_FALSE;

    GT_U32      entryIndex    = 0;

    CPSS_EXMXPM_PCL_UDB_TABLE_ENTRY_STC   udbCfg;
    CPSS_EXMXPM_PCL_UDB_TABLE_ENTRY_STC   udbCfgGet;


    cpssOsBzero((GT_VOID*) &udbCfg, sizeof(udbCfg));
    cpssOsBzero((GT_VOID*) &udbCfgGet, sizeof(udbCfgGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with entryIndex[0],
                           udbCfgPtr {udbCfg[23]{valid [GT_TRUE],
                                                 offsetType [CPSS_EXMXPM_PCL_OFFSET_L2_E],
                                                 offset [0],
                                                 tcpUdpComparatorsEnable [GT_FALSE] }
                                      inlifIdEnable [GT_FALSE],
                                      profileIdEnable [GT_FALSE] }
            Expected: GT_OK.
        */
        entryIndex = 0;

        udbCfg.inlifIdEnable   = GT_FALSE;
        udbCfg.profileIdEnable = GT_FALSE;

        for(iTemp=0; iTemp<23; ++iTemp)
        {
            udbCfg.udbCfg[iTemp].valid = GT_TRUE;
            udbCfg.udbCfg[iTemp].offsetType = CPSS_EXMXPM_PCL_OFFSET_L2_E;
            udbCfg.udbCfg[iTemp].offset = 0;
            udbCfg.udbCfg[iTemp].tcpUdpComparatorsEnable = GT_FALSE;
        }

        st = cpssExMxPmPclUdbTableEntrySet(dev, entryIndex, &udbCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.2. Call cpssExMxPmPclUdbTableEntryGet with not NULL udbCfgPtr and other params from 1.1.
            Expected: GT_OK and the same udbCfgPtr as was set.
        */
        st = cpssExMxPmPclUdbTableEntryGet(dev, entryIndex, &udbCfgGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPclUdbTableEntryGet: %d, %d", dev, entryIndex);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &udbCfg, (GT_VOID*) &udbCfgGet, sizeof(udbCfg) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another udbCfg than was set: %d", dev);

        /*
            1.3. Call with udbCfgPtr->udbCfg[0].offsetType[0x5AAAAAA5] and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        udbCfg.udbCfg[0].offsetType = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclUdbTableEntrySet(dev, entryIndex, &udbCfg);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, ->udbCfg[0].offsetType = %d",
                                     dev, entryIndex, udbCfg.udbCfg[0].offsetType);

        /*
            1.4. Call with udbCfgPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclUdbTableEntrySet(dev, entryIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, udbCfgPtr = NULL", dev);
    }

    entryIndex = 0;

    udbCfg.inlifIdEnable   = GT_FALSE;
    udbCfg.profileIdEnable = GT_FALSE;

    for(iTemp=0; iTemp<23; ++iTemp)
    {
        udbCfg.udbCfg[iTemp].valid = GT_TRUE;
        udbCfg.udbCfg[iTemp].offsetType = CPSS_EXMXPM_PCL_OFFSET_L2_E;
        udbCfg.udbCfg[iTemp].offset = 0;
        udbCfg.udbCfg[iTemp].tcpUdpComparatorsEnable = GT_FALSE;
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclUdbTableEntrySet(dev, entryIndex, &udbCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclUdbTableEntrySet(dev, entryIndex, &udbCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclUdbTableEntryGet
(
    IN  GT_U8                                 devNum,
    IN  GT_U32                                entryIndex,
    OUT CPSS_EXMXPM_PCL_UDB_TABLE_ENTRY_STC  *udbCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclUdbTableEntryGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with entryIndex [0], and not NULL udbCfgPtr.
    Expected: GT_OK.
    1.2. Call with udbCfgPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32                                entryIndex;
    CPSS_EXMXPM_PCL_UDB_TABLE_ENTRY_STC   udbCfg;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    entryIndex = 0;

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with entryIndex [0], and not NULL udbCfgPtr.
            Expected: GT_OK.
        */

        st = cpssExMxPmPclUdbTableEntryGet(dev, entryIndex, &udbCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.2. Call with udbCfgPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclUdbTableEntryGet(dev, entryIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, udbCfgPtr = NULL" ,dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclUdbTableEntryGet(dev, entryIndex, &udbCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclUdbTableEntryGet(dev, entryIndex, &udbCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclIngressPolicyEngineCfgSet
(
    IN GT_U8                                            devNum,
    IN CPSS_EXMXPM_PCL_INGRESS_POLICY_ENGINE_CFG_STC    *pclEngineCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclIngressPolicyEngineCfgSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with pclEngineCfgPtr{ lookup0_0_actionFormat [CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E / CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_E],
                                    lookup0_1_actionFormat [CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E / CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_E],
                                    lookup1_0_actionFormat [CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E / CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_E],
                                    lookup1_1_actionFormat [CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E / CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_E],
                                    externalUdbStandardKeyMode [CPSS_EXMXPM_PCL_EXTERNAL_UDB_STANDARD_KEY_MODE_20_BYTES_E / CPSS_EXMXPM_PCL_EXTERNAL_UDB_STANDARD_KEY_MODE_40_BYTES_E],
                                    externalUdbExtendedKeyMode [CPSS_EXMXPM_PCL_EXTERNAL_UDB_EXTENDED_KEY_MODE_40_BYTES_E / CPSS_EXMXPM_PCL_EXTERNAL_UDB_EXTENDED_KEY_MODE_60_BYTES_E] }
    Expected: GT_OK.
    1.2. Call cpssExMxPmPclIngressPolicyEngineCfgGet with not NULL pclEngineCfgPtr.
    Expected: GT_OK and the same params as was set.
    1.3. Call with pclEngineCfgPtr->lookup0_0_actionFormat [0x5AAAAAA5] and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with pclEngineCfgPtr->lookup0_1_actionFormat [0x5AAAAAA5] and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with pclEngineCfgPtr->lookup1_0_actionFormat [0x5AAAAAA5] and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with pclEngineCfgPtr->lookup1_1_actionFormat [0x5AAAAAA5] and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with pclEngineCfgPtr->externalUdbStandardKeyMode [0x5AAAAAA5] and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.8. Call with pclEngineCfgPtr->externalUdbExtendedKeyMode [0x5AAAAAA5] and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call with pclEngineCfgPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_PCL_INGRESS_POLICY_ENGINE_CFG_STC   pclEngineCfg;
    CPSS_EXMXPM_PCL_INGRESS_POLICY_ENGINE_CFG_STC   pclEngineCfgGet;

    cpssOsBzero((GT_VOID*) &pclEngineCfg, sizeof(pclEngineCfg));
    cpssOsBzero((GT_VOID*) &pclEngineCfgGet, sizeof(pclEngineCfgGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with pclEngineCfgPtr{ lookup0_0_actionFormat [CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E / CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_E],
                                            lookup0_1_actionFormat [CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E / CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_E],
                                            lookup1_0_actionFormat [CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E / CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_E],
                                            lookup1_1_actionFormat [CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E / CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_E],
                                            externalUdbStandardKeyMode [CPSS_EXMXPM_PCL_EXTERNAL_UDB_STANDARD_KEY_MODE_20_BYTES_E / CPSS_EXMXPM_PCL_EXTERNAL_UDB_STANDARD_KEY_MODE_40_BYTES_E],
                                            externalUdbExtendedKeyMode [CPSS_EXMXPM_PCL_EXTERNAL_UDB_EXTENDED_KEY_MODE_40_BYTES_E / CPSS_EXMXPM_PCL_EXTERNAL_UDB_EXTENDED_KEY_MODE_60_BYTES_E] }
            Expected: GT_OK.
        */
        pclEngineCfg.lookup0_0_actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E;
        pclEngineCfg.lookup0_1_actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E;
        pclEngineCfg.lookup1_0_actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E;
        pclEngineCfg.lookup1_1_actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E;

        pclEngineCfg.externalUdbStandardKeyMode = CPSS_EXMXPM_PCL_EXTERNAL_UDB_STANDARD_KEY_MODE_20_BYTES_E;
        pclEngineCfg.externalUdbExtendedKeyMode = CPSS_EXMXPM_PCL_EXTERNAL_UDB_EXTENDED_KEY_MODE_40_BYTES_E;

        st = cpssExMxPmPclIngressPolicyEngineCfgSet(dev, &pclEngineCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmPclIngressPolicyEngineCfgGet with not NULL pclEngineCfgPtr.
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmPclIngressPolicyEngineCfgGet(dev, &pclEngineCfgGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPclIngressPolicyEngineCfgGet: %d", dev);

        pclEngineCfg.lookup0_0_actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_E;
        pclEngineCfg.lookup0_1_actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_E;
        pclEngineCfg.lookup1_0_actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_E;
        pclEngineCfg.lookup1_1_actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_E;

        pclEngineCfg.externalUdbStandardKeyMode = CPSS_EXMXPM_PCL_EXTERNAL_UDB_STANDARD_KEY_MODE_40_BYTES_E;
        pclEngineCfg.externalUdbExtendedKeyMode = CPSS_EXMXPM_PCL_EXTERNAL_UDB_EXTENDED_KEY_MODE_60_BYTES_E;

        st = cpssExMxPmPclIngressPolicyEngineCfgSet(dev, &pclEngineCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmPclIngressPolicyEngineCfgGet with not NULL pclEngineCfgPtr.
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmPclIngressPolicyEngineCfgGet(dev, &pclEngineCfgGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPclIngressPolicyEngineCfgGet: %d", dev);

        /*
            1.3. Call with pclEngineCfgPtr->lookup0_0_actionFormat [0x5AAAAAA5] and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pclEngineCfg.lookup0_0_actionFormat = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclIngressPolicyEngineCfgSet(dev, &pclEngineCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, pclEngineCfgPtr->lookup0_0_actionFormat = %d",
                                     dev, pclEngineCfg.lookup0_0_actionFormat);

        pclEngineCfg.lookup0_0_actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_E;

        /*
            1.4. Call with pclEngineCfgPtr->lookup0_1_actionFormat [0x5AAAAAA5] and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pclEngineCfg.lookup0_1_actionFormat = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclIngressPolicyEngineCfgSet(dev, &pclEngineCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, pclEngineCfgPtr->lookup0_1_actionFormat = %d",
                                     dev, pclEngineCfg.lookup0_1_actionFormat);

        pclEngineCfg.lookup0_1_actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_E;

        /*
            1.5. Call with pclEngineCfgPtr->lookup1_0_actionFormat [0x5AAAAAA5] and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pclEngineCfg.lookup1_0_actionFormat = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclIngressPolicyEngineCfgSet(dev, &pclEngineCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, pclEngineCfgPtr->lookup1_0_actionFormat = %d",
                                     dev, pclEngineCfg.lookup1_0_actionFormat);

        pclEngineCfg.lookup1_0_actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_E;

        /*
            1.6. Call with pclEngineCfgPtr->lookup1_1_actionFormat [0x5AAAAAA5] and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pclEngineCfg.lookup1_1_actionFormat = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclIngressPolicyEngineCfgSet(dev, &pclEngineCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, pclEngineCfgPtr->lookup1_1_actionFormat = %d",
                                     dev, pclEngineCfg.lookup1_1_actionFormat);

        pclEngineCfg.lookup1_1_actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_E;

        /*
            1.7. Call with pclEngineCfgPtr->externalUdbStandardKeyMode [0x5AAAAAA5] and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pclEngineCfg.externalUdbStandardKeyMode = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclIngressPolicyEngineCfgSet(dev, &pclEngineCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, pclEngineCfgPtr->externalUdbStandardKeyMode = %d",
                                     dev, pclEngineCfg.externalUdbStandardKeyMode);

        pclEngineCfg.externalUdbStandardKeyMode = CPSS_EXMXPM_PCL_EXTERNAL_UDB_STANDARD_KEY_MODE_20_BYTES_E;

        /*
            1.8. Call with pclEngineCfgPtr->externalUdbExtendedKeyMode [0x5AAAAAA5] and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pclEngineCfg.externalUdbExtendedKeyMode = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclIngressPolicyEngineCfgSet(dev, &pclEngineCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, pclEngineCfgPtr->externalUdbExtendedKeyMode = %d",
                                     dev, pclEngineCfg.externalUdbExtendedKeyMode);

        pclEngineCfg.externalUdbExtendedKeyMode = CPSS_EXMXPM_PCL_EXTERNAL_UDB_EXTENDED_KEY_MODE_40_BYTES_E;

        /*
            1.9. Call with pclEngineCfgPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclIngressPolicyEngineCfgSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pclEngineCfgPtr = NULL", dev);
    }

    pclEngineCfg.lookup0_0_actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E;
    pclEngineCfg.lookup0_1_actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E;
    pclEngineCfg.lookup1_0_actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E;
    pclEngineCfg.lookup1_1_actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E;

    pclEngineCfg.externalUdbStandardKeyMode = CPSS_EXMXPM_PCL_EXTERNAL_UDB_STANDARD_KEY_MODE_20_BYTES_E;
    pclEngineCfg.externalUdbExtendedKeyMode = CPSS_EXMXPM_PCL_EXTERNAL_UDB_EXTENDED_KEY_MODE_40_BYTES_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclIngressPolicyEngineCfgSet(dev, &pclEngineCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclIngressPolicyEngineCfgSet(dev, &pclEngineCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclIngressPolicyEngineCfgGet
(
    IN  GT_U8                                            devNum,
    OUT CPSS_EXMXPM_PCL_INGRESS_POLICY_ENGINE_CFG_STC    *pclEngineCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclIngressPolicyEngineCfgGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL pclEngineCfgPtr.
    Expected: GT_OK.
    1.2. Call with pclEngineCfgPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_PCL_INGRESS_POLICY_ENGINE_CFG_STC   pclEngineCfg;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL pclEngineCfgPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPclIngressPolicyEngineCfgGet(dev, &pclEngineCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with pclEngineCfgPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclIngressPolicyEngineCfgGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pclEngineCfgPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclIngressPolicyEngineCfgGet(dev, &pclEngineCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclIngressPolicyEngineCfgGet(dev, &pclEngineCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclProfileIdTblSet
(
    IN GT_U8                             devNum,
    IN CPSS_INTERFACE_INFO_STC           *interfaceInfoPtr,
    IN GT_U32                            profileId
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclProfileIdTblSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. CAll with interfaceInfo { type [CPSS_INTERFACE_PORT_E],
                                   devPort { devNum[dev], portNum [0]} },
                   profileId [0]
    Expected: GT_OK.
    1.2. Call with cpssExMxPmPclProfileIdTblGet with not NULL profileIdPtr and other params from 1.1.
    Expected: GT_OK and the same params as was set. (for 1.1. and 1.6.)
    1.3. Call with interfaceInfoPtr->type [0x5AAAAAA5] out of range
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with interfaceInfoPtr->devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] out of range
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with interfaceInfoPtr->devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] not relevant,
                   interfaceInfoPtr->type [CPSS_INTERFACE_VID_E],
                   interfaceInfoPtr->vlanId [100],
                   and other params from 1.1.
    Expected: GT_OK.

    1.6. Call with interfaceInfo { type [CPSS_INTERFACE_VID_E],
                                   vlanId [100] },
                   profileId [31]
    Expected: GT_OK.
    1.7. Call with interfaceInfoPtr->vlanId [4096],
                   and other params from 1.6.
    Expected: NOT GT_OK.
    1.9. Call with profileId [32] and other params from 1.6.
    Expected: NOT GT_OK.
    1.10. Call with interfaceInfo->type[CPSS_INTERFACE_TRUNK_E] and other params from 1.6.
    Expected: NOT GT_OK.
    1.11. Call with interfaceInfoPtr [NULL] and other params from 1.6.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_INTERFACE_INFO_STC           interfaceInfo;
    GT_U32                            profileId    = 0;
    GT_U32                            profileIdGet = 0;


    cpssOsBzero((GT_VOID*) &interfaceInfo, sizeof(interfaceInfo));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. CAll with interfaceInfo { type [CPSS_INTERFACE_PORT_E],
                                           devPort { devNum[dev], portNum [0]} },
                           profileId [0]
            Expected: GT_OK.
        */
        interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        interfaceInfo.devPort.devNum = dev;
        interfaceInfo.devPort.portNum = 0;
        profileId = 0;

        st = cpssExMxPmPclProfileIdTblSet(dev, &interfaceInfo, profileId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with cpssExMxPmPclProfileIdTblGet with not NULL profileIdPtr and other params from 1.1.
            Expected: GT_OK and the same params as was set. (for 1.1. and 1.6.)
        */
        st = cpssExMxPmPclProfileIdTblGet(dev, &interfaceInfo, &profileIdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPclProfileIdTblGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profileId, profileIdGet, "got another profileId than was set: %d", dev);

        /*
            1.3. Call with interfaceInfoPtr->type [0x5AAAAAA5] out of range
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        interfaceInfo.type = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclProfileIdTblSet(dev, &interfaceInfo, profileId);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, interfaceInfoPtr->type = %d",
                                    dev, interfaceInfo.type);

        interfaceInfo.type = CPSS_INTERFACE_PORT_E;

        /*
            1.4. Call with interfaceInfoPtr->devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] out of range
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        interfaceInfo.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmPclProfileIdTblSet(dev, &interfaceInfo, profileId);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, interfaceInfoPtr->devPort.portNum = %d",
                                    dev, interfaceInfo.devPort.portNum);

        interfaceInfo.devPort.portNum = 0;

        /*
            1.5. Call with interfaceInfoPtr->devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] not relevant,
                           interfaceInfoPtr->type [CPSS_INTERFACE_VID_E],
                           interfaceInfoPtr->vlanId [100],
                           and other params from 1.1.
            Expected: GT_OK.
        */
        interfaceInfo.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        interfaceInfo.type   = CPSS_INTERFACE_VID_E;
        interfaceInfo.vlanId = 100;

        st = cpssExMxPmPclProfileIdTblSet(dev, &interfaceInfo, profileId);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, interfaceInfoPtr->devPort.portNum = %d, ->type = %d, ->vlanId = %d",
                                    dev, interfaceInfo.devPort.portNum, interfaceInfo.type, interfaceInfo.vlanId);

        interfaceInfo.devPort.portNum = 0;

        /*
            1.6. CAll with interfaceInfo { type [CPSS_INTERFACE_VID_E],
                                           vlanId [100] },
                           profileId [31]
            Expected: GT_OK.
        */
        interfaceInfo.type = CPSS_INTERFACE_VID_E;
        interfaceInfo.vlanId = 100;
        profileId = 31;

        st = cpssExMxPmPclProfileIdTblSet(dev, &interfaceInfo, profileId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.7. Call with interfaceInfoPtr->vlanId [4096],
                           and other params from 1.6.
            Expected: NOT GT_OK.
        */
        interfaceInfo.vlanId = 4096;

        st = cpssExMxPmPclProfileIdTblSet(dev, &interfaceInfo, profileId);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, interfaceInfoPtr->vlanId = %d",
                                         dev, interfaceInfo.vlanId);

        interfaceInfo.vlanId = 100;

        /*
            1.9. Call with profileId [32] and other params from 1.6.
            Expected: NOT GT_OK.
        */
        profileId = 32;

        st = cpssExMxPmPclProfileIdTblSet(dev, &interfaceInfo, profileId);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, profileId = %d",
                                         dev, profileId);

        profileId = 31;

        /*
            1.10. Call with interfaceInfo->type[CPSS_INTERFACE_TRUNK_E] and other params from 1.6.
            Expected: NOT GT_OK.
        */
        interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;

        st = cpssExMxPmPclProfileIdTblSet(dev, &interfaceInfo, profileId);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, interfaceInfoPtr->type = %d",
                                         dev, interfaceInfo.type);

        interfaceInfo.type = CPSS_INTERFACE_PORT_E;

        /*
            1.11. Call with interfaceInfoPtr [NULL] and other params from 1.6.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclProfileIdTblSet(dev, NULL, profileId);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, interfaceInfoPtr = NULL", dev);
    }

    interfaceInfo.type = CPSS_INTERFACE_PORT_E;
    interfaceInfo.devPort.devNum = dev;
    interfaceInfo.devPort.portNum = 0;
    profileId = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclProfileIdTblSet(dev, &interfaceInfo, profileId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclProfileIdTblSet(dev, &interfaceInfo, profileId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclProfileIdTblGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_INTERFACE_INFO_STC           *interfaceInfoPtr,
    OUT GT_U32                            *profileIdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclProfileIdTblGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. CAll with interfaceInfo { type [CPSS_INTERFACE_PORT_E],
                                   devPort { devNum[dev], portNum [0]} },
                   not NULL profileIdPtr.
    Expected: GT_OK.
    1.2. Call with interfaceInfoPtr->type [0x5AAAAAA5] out of range
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with interfaceInfoPtr->devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] out of range
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with interfaceInfoPtr->devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] not relevant,
                   interfaceInfoPtr->type [CPSS_INTERFACE_VID_E],
                   interfaceInfoPtr->vlanId [100],
                   and other params from 1.1.
    Expected: GT_OK.
    1.5. Call with interfaceInfoPtr->type [CPSS_INTERFACE_VID_E],
                   interfaceInfoPtr->vlanId [100],
                   and other params from 1.1.
    Expected: GT_OK.
    1.6. Call with interfaceInfoPtr->type [CPSS_INTERFACE_VID_E],
                   interfaceInfoPtr->vlanId [4096],
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with interfaceInfo->type[CPSS_INTERFACE_TRUNK_E] and other params from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with interfaceInfoPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.9. Call with profileIdPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_INTERFACE_INFO_STC           interfaceInfo;
    GT_U32                            profileId    = 0;


    cpssOsBzero((GT_VOID*) &interfaceInfo, sizeof(interfaceInfo));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. CAll with interfaceInfo { type [CPSS_INTERFACE_PORT_E],
                                           devPort { devNum[dev], portNum [0]} },
                           not NULL profileIdPtr.
            Expected: GT_OK.
        */
        interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        interfaceInfo.devPort.devNum = dev;
        interfaceInfo.devPort.portNum = 0;

        st = cpssExMxPmPclProfileIdTblGet(dev, &interfaceInfo, &profileId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with interfaceInfoPtr->type [0x5AAAAAA5] out of range
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        interfaceInfo.type = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclProfileIdTblGet(dev, &interfaceInfo, &profileId);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, interfaceInfoPtr->type = %d",
                                    dev, interfaceInfo.type);

        interfaceInfo.type = CPSS_INTERFACE_PORT_E;

        /*
            1.3. Call with interfaceInfoPtr->devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] out of range
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        interfaceInfo.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmPclProfileIdTblGet(dev, &interfaceInfo, &profileId);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, interfaceInfoPtr->devPort.portNum = %d",
                                    dev, interfaceInfo.devPort.portNum);

        interfaceInfo.devPort.portNum = 0;

        /*
            1.4. Call with interfaceInfoPtr->devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] not relevant,
                           interfaceInfoPtr->type [CPSS_INTERFACE_VID_E],
                           interfaceInfoPtr->vlanId [100],
                           and other params from 1.1.
            Expected: GT_OK.
        */
        interfaceInfo.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        interfaceInfo.type   = CPSS_INTERFACE_VID_E;
        interfaceInfo.vlanId = 100;

        st = cpssExMxPmPclProfileIdTblGet(dev, &interfaceInfo, &profileId);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, interfaceInfoPtr->devPort.portNum = %d, ->type = %d, ->vlanId = %d",
                                    dev, interfaceInfo.devPort.portNum, interfaceInfo.type, interfaceInfo.vlanId);

        interfaceInfo.devPort.portNum = 0;

        /*
            1.5. Call with interfaceInfoPtr->type [CPSS_INTERFACE_VID_E],
                           interfaceInfoPtr->vlanId [100],
                           and other params from 1.1.
            Expected: GT_OK.
        */
        interfaceInfo.type   = CPSS_INTERFACE_VID_E;
        interfaceInfo.vlanId = 100;

        st = cpssExMxPmPclProfileIdTblGet(dev, &interfaceInfo, &profileId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.6. Call with interfaceInfoPtr->type [CPSS_INTERFACE_VID_E],
                           interfaceInfoPtr->vlanId [4096],
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        interfaceInfo.vlanId = 4096;

        st = cpssExMxPmPclProfileIdTblGet(dev, &interfaceInfo, &profileId);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, interfaceInfoPtr->type = %d, ->vlanId = %d",
                                    dev, interfaceInfo.type, interfaceInfo.vlanId);

        interfaceInfo.vlanId = 100;

        /*
            1.7. Call with interfaceInfo->type[CPSS_INTERFACE_TRUNK_E] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;

        st = cpssExMxPmPclProfileIdTblGet(dev, &interfaceInfo, &profileId);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, interfaceInfoPtr->type = %d",
                                    dev, interfaceInfo.type);

        interfaceInfo.type = CPSS_INTERFACE_PORT_E;

        /*
            1.8. Call with interfaceInfoPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclProfileIdTblGet(dev, NULL, &profileId);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, interfaceInfoPtr = NULL", dev);

        /*
            1.9. Call with profileIdPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclProfileIdTblGet(dev, &interfaceInfo, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, profileIdPtr = NULL", dev);
    }

    interfaceInfo.type = CPSS_INTERFACE_PORT_E;
    interfaceInfo.devPort.devNum = dev;
    interfaceInfo.devPort.portNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclProfileIdTblGet(dev, &interfaceInfo, &profileId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclProfileIdTblGet(dev, &interfaceInfo, &profileId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclEgressLookupTypeSet
(
    IN GT_U8     devNum,
    IN GT_BOOL   notIpPktLookupExternal,
    IN GT_BOOL   ipv4PktLookupExternal,
    IN GT_BOOL   ipv6PktLookupExternal
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclEgressLookupTypeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with notIpPktLookupExternal [GT_FALSE / GT_TRUE],
                   ipv4PktLookupExternal [GT_FALSE / GT_TRUE],
                   ipv6PktLookupExternal [GT_FALSE / GT_TRUE]
    Expected: GT_OK.
    1.2. Call cpssExMxPmPclEgressLookupTypeGet with not NULL pointers.
    Expected: GT_OK and the same params as was set.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL   notIpPktLookupExternal = GT_FALSE;
    GT_BOOL   ipv4PktLookupExternal  = GT_FALSE;
    GT_BOOL   ipv6PktLookupExternal  = GT_FALSE;

    GT_BOOL   notIpPktLookupExternalGet = GT_FALSE;
    GT_BOOL   ipv4PktLookupExternalGet  = GT_FALSE;
    GT_BOOL   ipv6PktLookupExternalGet  = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with notIpPktLookupExternal [GT_FALSE / GT_TRUE],
                           ipv4PktLookupExternal [GT_FALSE / GT_TRUE],
                           ipv6PktLookupExternal [GT_FALSE / GT_TRUE]
            Expected: GT_OK.
        */
        notIpPktLookupExternal = GT_FALSE;
        ipv4PktLookupExternal  = GT_FALSE;
        ipv6PktLookupExternal  = GT_FALSE;

        st = cpssExMxPmPclEgressLookupTypeSet(dev, notIpPktLookupExternal, ipv4PktLookupExternal, ipv6PktLookupExternal);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, notIpPktLookupExternal, ipv4PktLookupExternal, ipv6PktLookupExternal);

        /*
            1.2. Call cpssExMxPmPclEgressLookupTypeGet with not NULL pointers.
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmPclEgressLookupTypeGet(dev, &notIpPktLookupExternalGet, &ipv4PktLookupExternalGet, &ipv6PktLookupExternalGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPclEgressLookupTypeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(notIpPktLookupExternal, notIpPktLookupExternalGet,
                                    "got another notIpPktLookupExternal than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ipv4PktLookupExternal, ipv4PktLookupExternalGet,
                                    "got another ipv4PktLookupExternal than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ipv6PktLookupExternal, ipv6PktLookupExternalGet,
                                    "got another ipv6PktLookupExternal than was set: %d", dev);

        notIpPktLookupExternal = GT_TRUE;
        ipv4PktLookupExternal  = GT_TRUE;
        ipv6PktLookupExternal  = GT_TRUE;

        st = cpssExMxPmPclEgressLookupTypeSet(dev, notIpPktLookupExternal, ipv4PktLookupExternal, ipv6PktLookupExternal);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, notIpPktLookupExternal, ipv4PktLookupExternal, ipv6PktLookupExternal);

        /*
            1.2. Call cpssExMxPmPclEgressLookupTypeGet with not NULL pointers.
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmPclEgressLookupTypeGet(dev, &notIpPktLookupExternalGet, &ipv4PktLookupExternalGet, &ipv6PktLookupExternalGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPclEgressLookupTypeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(notIpPktLookupExternal, notIpPktLookupExternalGet,
                                    "got another notIpPktLookupExternal than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ipv4PktLookupExternal, ipv4PktLookupExternalGet,
                                    "got another ipv4PktLookupExternal than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ipv6PktLookupExternal, ipv6PktLookupExternalGet,
                                    "got another ipv6PktLookupExternal than was set: %d", dev);
    }

    notIpPktLookupExternal = GT_FALSE;
    ipv4PktLookupExternal  = GT_FALSE;
    ipv6PktLookupExternal  = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclEgressLookupTypeSet(dev, notIpPktLookupExternal, ipv4PktLookupExternal, ipv6PktLookupExternal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclEgressLookupTypeSet(dev, notIpPktLookupExternal, ipv4PktLookupExternal, ipv6PktLookupExternal);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclEgressLookupTypeGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *notIpPktLookupExternalPtr,
    OUT GT_BOOL   *ipv4PktLookupExternalPtr,
    OUT GT_BOOL   *ipv6PktLookupExternalPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclEgressLookupTypeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL pointers.
    Expected: GT_OK.
    1.2. Call with notIpPktLookupExternalPtr [NULL] and other params as in 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with ipv4PktLookupExternalPtr [NULL] and other params as in 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with ipv6PktLookupExternalPtr [NULL] and other params as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL   notIpPktLookupExternal = GT_FALSE;
    GT_BOOL   ipv4PktLookupExternal  = GT_FALSE;
    GT_BOOL   ipv6PktLookupExternal  = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL pointers.
            Expected: GT_OK.
        */
        st = cpssExMxPmPclEgressLookupTypeGet(dev, &notIpPktLookupExternal, &ipv4PktLookupExternal, &ipv6PktLookupExternal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with notIpPktLookupExternalPtr [NULL] and other params as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclEgressLookupTypeGet(dev, NULL, &ipv4PktLookupExternal, &ipv6PktLookupExternal);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, notIpPktLookupExternalPtr = NULL", dev);

        /*
            1.3. Call with ipv4PktLookupExternalPtr [NULL] and other params as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclEgressLookupTypeGet(dev, &notIpPktLookupExternal, NULL, &ipv6PktLookupExternal);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipv4PktLookupExternal = NULL", dev);

        /*
            1.4. Call with ipv6PktLookupExternalPtr [NULL] and other params as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclEgressLookupTypeGet(dev, &notIpPktLookupExternal, &ipv4PktLookupExternal, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipv6PktLookupExternal = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclEgressLookupTypeGet(dev, &notIpPktLookupExternal, &ipv4PktLookupExternal, &ipv6PktLookupExternal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclEgressLookupTypeGet(dev, &notIpPktLookupExternal, &ipv4PktLookupExternal, &ipv6PktLookupExternal);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclUserDefinedEtherTypeSet
(
    IN  GT_U8     devNum,
    IN  GT_U32    etherType
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclUserDefinedEtherTypeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with etherType [0 / 0xFFFF]
    Expected: GT_OK.
    1.2. Call cpssExMxPmPclUserDefinedEtherTypeGet with not NULL etherTypePtr.
    Expected: GT_OK and the same etherType as was set.
    1.3. Call with etherType [0x10000]
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32    etherType    = 0;
    GT_U32    etherTypeGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with etherType [0 / 0xFFFF]
            Expected: GT_OK.
        */
        /* iterate with etherType = 0 */
        etherType = 0;

        st = cpssExMxPmPclUserDefinedEtherTypeSet(dev, etherType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, etherType);

        /*
            1.2. Call cpssExMxPmPclUserDefinedEtherTypeGet with not NULL etherTypePtr.
            Expected: GT_OK and the same etherType as was set.
        */
        st = cpssExMxPmPclUserDefinedEtherTypeGet(dev, &etherTypeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPclUserDefinedEtherTypeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(etherType, etherTypeGet, "got another etherType than was set: %d", dev);

        /* iterate with etherType = 0xFFFF */
        etherType = 0xFFFF;

        st = cpssExMxPmPclUserDefinedEtherTypeSet(dev, etherType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, etherType);

        /*
            1.2. Call cpssExMxPmPclUserDefinedEtherTypeGet with not NULL etherTypePtr.
            Expected: GT_OK and the same etherType as was set.
        */
        st = cpssExMxPmPclUserDefinedEtherTypeGet(dev, &etherTypeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPclUserDefinedEtherTypeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(etherType, etherTypeGet, "got another etherType than was set: %d", dev);

        /*
            1.3. Call with etherType [0x10000]
            Expected: NOT GT_OK.
        */
        etherType = 0x10000;

        st = cpssExMxPmPclUserDefinedEtherTypeSet(dev, etherType);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, etherType);
    }

    etherType = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclUserDefinedEtherTypeSet(dev, etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclUserDefinedEtherTypeSet(dev, etherType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclUserDefinedEtherTypeGet
(
    IN  GT_U8     devNum,
    OUT GT_U32    *etherTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclUserDefinedEtherTypeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL etherTypePtr.
    Expected: GT_OK.
    1.2. Call with etherTypePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      etherType = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL etherTypePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmPclUserDefinedEtherTypeGet(dev, &etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with etherTypePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclUserDefinedEtherTypeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, etherTypePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclUserDefinedEtherTypeGet(dev, &etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclUserDefinedEtherTypeGet(dev, &etherType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclRuleStcToRawConvert
(
    IN     CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN     CPSS_EXMXPM_PCL_RULE_FORMAT_UNT      *stcDataPtr,
    INOUT  GT_U32                               *rawDataSizePtr,
    OUT    GT_U32                               *rawDataPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclRuleStcToRawConvert)
{
/*
1. Call with ruleFormat [CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E],
             stcData {ruleEgrStandardNotIp{
                                                commonEgr{  pclId [0],
                                                            isMpls [0],
                                                            srcPort [0],
                                                            isTagged [0],
                                                            vid [100],
                                                            up [7],
                                                            isIp [1] }
                                               commonEgrStd{isIpv4 [1],
                                                            isArp [0],
                                                            isL2Valid [0],
                                                            packetCmd [0],
                                                            trgOrCpuCode0_6OrCosOrSniff [0],
                                                            srcTrgOrTxMirror [0],
                                                            sourceId [0],
                                                            srcInfo [0],
                                                            srcIsTrunk [0],
                                                            isIpOrMplsRouted [0],
                                                            egrFilterRegOrCpuCode7 [0],
                                                            trgPort_Extrn [0],
                                                            srcDev8_7_Extrn [0],
                                                            exp_Extrn [0],
                                                            egrFilterReg_Extrn [0],
                                                            macSa_Extrn {arEther[10, 20, 30, 40, 50, 60]} }
                                                l2EncapType [0],
                                                ethType [0],
                                                macDa {arEther[10, 20, 30, 40, 50, 60]},
                                                macSa {arEther[10, 20, 30, 40, 50, 60]},
                                                isBc_Extrn [0] } },
             rawDataSize [10],
             not NULL rawDataPtr
Expected: GT_OK.
2. Call cpssExMxPmPclRuleRawToStcConvert with not NULL stcDataPtr and other params as in 1.
Expected: GT_OK anf the same params as was set.
3. Call with ruleFormat [0x5AAAAAA5] and other params as in 1.
Expected: GT_BAD_PARAM.
4. Call with stcDataPtr [NULL] and other params as in 1.
Expected: GT_BAD_PTR.
5. Call with rawDataSizePtr [NULL] and other params as in 1.
Expected: GT_BAD_PTR.
6. Call with rawDataPtr [NULL] and other params as in 1.
Expected: GT_BAD_PTR.
*/
    GT_STATUS   st      = GT_OK;
    GT_BOOL     isEqual = GT_FALSE;

    CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT ruleFormat;
    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT      stcData;
    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT      stcDataGet;
    GT_U32                               rawDataSize = 0;
    GT_U32                               rawData[10];


    cpssOsBzero((GT_VOID*) &stcData, sizeof(stcData));
    cpssOsBzero((GT_VOID*) &stcDataGet, sizeof(stcDataGet));

/*
    1. Call with ruleFormat [CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E],
                 stcData {ruleEgrStandardNotIp{
                                                    commonEgr{  pclId [0],
                                                                isMpls [0],
                                                                srcPort [0],
                                                                isTagged [0],
                                                                vid [100],
                                                                up [7],
                                                                isIp [1] }
                                                   commonEgrStd{isIpv4 [1],
                                                                isArp [0],
                                                                isL2Valid [0],
                                                                packetCmd [0],
                                                                trgOrCpuCode0_6OrCosOrSniff [0],
                                                                srcTrgOrTxMirror [0],
                                                                sourceId [0],
                                                                srcInfo [0],
                                                                srcIsTrunk [0],
                                                                isIpOrMplsRouted [0],
                                                                egrFilterRegOrCpuCode7 [0],
                                                                trgPort_Extrn [0],
                                                                srcDev8_7_Extrn [0],
                                                                exp_Extrn [0],
                                                                egrFilterReg_Extrn [0],
                                                                macSa_Extrn {arEther[10, 20, 30, 40, 50, 60]} }
                                                    l2EncapType [0],
                                                    ethType [0],
                                                    macDa {arEther[10, 20, 30, 40, 50, 60]},
                                                    macSa {arEther[10, 20, 30, 40, 50, 60]},
                                                    isBc_Extrn [0] } },
                 rawDataSize [10],
                 not NULL rawDataPtr
    Expected: GT_OK.
*/
    ruleFormat  = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E;
    rawDataSize = 10;

    stcData.ruleEgrStandardNotIp.commonEgr.pclId = 0;
    stcData.ruleEgrStandardNotIp.commonEgr.isMpls = 0;
    stcData.ruleEgrStandardNotIp.commonEgr.srcPort = 0;
    stcData.ruleEgrStandardNotIp.commonEgr.isTagged = 0;
    stcData.ruleEgrStandardNotIp.commonEgr.vid = 100;
    stcData.ruleEgrStandardNotIp.commonEgr.up = 7;
    stcData.ruleEgrStandardNotIp.commonEgr.isIp = 1;

    stcData.ruleEgrStandardNotIp.commonEgrStd.isIpv4 = 1;
    stcData.ruleEgrStandardNotIp.commonEgrStd.isArp = 0;
    stcData.ruleEgrStandardNotIp.commonEgrStd.isL2Valid = 0;
    stcData.ruleEgrStandardNotIp.commonEgrStd.packetCmd = 0;
    stcData.ruleEgrStandardNotIp.commonEgrStd.trgOrCpuCode0_6OrCosOrSniff = 0;
    stcData.ruleEgrStandardNotIp.commonEgrStd.srcTrgOrTxMirror = 0;
    stcData.ruleEgrStandardNotIp.commonEgrStd.sourceId = 0;
    stcData.ruleEgrStandardNotIp.commonEgrStd.srcInfo = 0;
    stcData.ruleEgrStandardNotIp.commonEgrStd.srcIsTrunk = 0;
    stcData.ruleEgrStandardNotIp.commonEgrStd.isIpOrMplsRouted = 0;
    stcData.ruleEgrStandardNotIp.commonEgrStd.egrFilterRegOrCpuCode7 = 0;
    stcData.ruleEgrStandardNotIp.commonEgrStd.trgPort_Extrn = 0;
    stcData.ruleEgrStandardNotIp.commonEgrStd.srcDev8_7_Extrn = 0;
    stcData.ruleEgrStandardNotIp.commonEgrStd.exp_Extrn = 0;
    stcData.ruleEgrStandardNotIp.commonEgrStd.egrFilterReg_Extrn = 0;

    stcData.ruleEgrStandardNotIp.commonEgrStd.macSa_Extrn.arEther[0] = 10;
    stcData.ruleEgrStandardNotIp.commonEgrStd.macSa_Extrn.arEther[1] = 20;
    stcData.ruleEgrStandardNotIp.commonEgrStd.macSa_Extrn.arEther[2] = 30;
    stcData.ruleEgrStandardNotIp.commonEgrStd.macSa_Extrn.arEther[3] = 40;
    stcData.ruleEgrStandardNotIp.commonEgrStd.macSa_Extrn.arEther[4] = 50;
    stcData.ruleEgrStandardNotIp.commonEgrStd.macSa_Extrn.arEther[5] = 60;

    stcData.ruleEgrStandardNotIp.l2EncapType = 0;
    stcData.ruleEgrStandardNotIp.ethType = 0;

    stcData.ruleEgrStandardNotIp.macDa.arEther[0] = 10;
    stcData.ruleEgrStandardNotIp.macDa.arEther[1] = 20;
    stcData.ruleEgrStandardNotIp.macDa.arEther[2] = 30;
    stcData.ruleEgrStandardNotIp.macDa.arEther[3] = 40;
    stcData.ruleEgrStandardNotIp.macDa.arEther[4] = 50;
    stcData.ruleEgrStandardNotIp.macDa.arEther[5] = 60;

    stcData.ruleEgrStandardNotIp.macSa.arEther[0] = 10;
    stcData.ruleEgrStandardNotIp.macSa.arEther[1] = 20;
    stcData.ruleEgrStandardNotIp.macSa.arEther[2] = 30;
    stcData.ruleEgrStandardNotIp.macSa.arEther[3] = 40;
    stcData.ruleEgrStandardNotIp.macSa.arEther[4] = 50;
    stcData.ruleEgrStandardNotIp.macSa.arEther[5] = 60;

    stcData.ruleEgrStandardNotIp.isBc_Extrn = 0;

    st = cpssExMxPmPclRuleStcToRawConvert(ruleFormat, &stcData, &rawDataSize, rawData);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, ruleFormat);

/*
    2. Call cpssExMxPmPclRuleRawToStcConvert with not NULL stcDataPtr and other params as in 1.
    Expected: GT_OK and the same params as was set.
*/
    st = cpssExMxPmPclRuleRawToStcConvert(ruleFormat, rawDataSize, rawData, &stcDataGet);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPclRuleRawToStcConvert: %d", ruleFormat);

    isEqual = (0 == cpssOsMemCmp((GT_VOID*) &stcData.ruleEgrStandardNotIp,
                                 (GT_VOID*) &stcDataGet.ruleEgrStandardNotIp,
                                 sizeof(stcData.ruleEgrStandardNotIp) )) ? GT_TRUE : GT_FALSE;
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another stcData.ruleEgrStandardNotIp than was set", ruleFormat);

/*
    3. Call with ruleFormat [0x5AAAAAA5] and other params as in 1.
    Expected: GT_BAD_PARAM.
*/
    ruleFormat = EXMXPM_PCL_INVALID_ENUM_CNS;

    st = cpssExMxPmPclRuleStcToRawConvert(ruleFormat, &stcData, &rawDataSize, rawData);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, ruleFormat);

    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E;

/*
    4. Call with stcDataPtr [NULL] and other params as in 1.
    Expected: GT_BAD_PTR.
*/
    st = cpssExMxPmPclRuleStcToRawConvert(ruleFormat, NULL, &rawDataSize, rawData);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, stcDataPtr = NULL", ruleFormat);

/*
    5. Call with rawDataSizePtr [NULL] and other params as in 1.
    Expected: GT_BAD_PTR.
*/
    st = cpssExMxPmPclRuleStcToRawConvert(ruleFormat, &stcData, NULL, rawData);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, rawDataSizePtr = NULL", ruleFormat);

/*
    6. Call with rawDataPtr [NULL] and other params as in 1.
    Expected: GT_BAD_PTR.
*/
    st = cpssExMxPmPclRuleStcToRawConvert(ruleFormat, &stcData, &rawDataSize, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, rawDataPtr = NULL", ruleFormat);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclRuleRawToStcConvert
(
    IN  CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN  GT_U32                               rawDataSize,
    IN  GT_U32                               *rawDataPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_UNT      *stcDataPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclRuleRawToStcConvert)
{
/*
1. Call with ruleFormat [CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E],
             rawDataSize [10],
             rawData [0 .. 0]
             not NULL stcDataPtr
Expected: GT_OK.
2. Call with ruleFormat [0x5AAAAAA5] and other params as in 1.
Expected: GT_BAD_PARAM.
3. Call with stcDataPtr [NULL] and other params as in 1.
Expected: GT_BAD_PTR.
4. Call with rawDataPtr [NULL] and other params as in 1.
Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT ruleFormat;
    GT_U32                               rawDataSize;
    GT_U32                               rawData[10];
    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT      stcData;

/*
    1. Call with ruleFormat [CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E],
                 rawDataSize [10],
                 rawData [0 .. 0]
                 not NULL stcDataPtr
    Expected: GT_OK.
*/
    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E;
    rawDataSize = 10;
    cpssOsBzero((GT_VOID*) &rawData, sizeof(rawData));

    st = cpssExMxPmPclRuleRawToStcConvert(ruleFormat, rawDataSize, rawData, &stcData);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, ruleFormat, rawDataSize);

/*
    2. Call with ruleFormat [0x5AAAAAA5] and other params as in 1.
    Expected: GT_BAD_PARAM.
*/
    ruleFormat = EXMXPM_PCL_INVALID_ENUM_CNS;

    st = cpssExMxPmPclRuleRawToStcConvert(ruleFormat, rawDataSize, rawData, &stcData);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, ruleFormat);

    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E;

/*
    3. Call with stcDataPtr [NULL] and other params as in 1.
    Expected: GT_BAD_PTR.
*/
    st = cpssExMxPmPclRuleRawToStcConvert(ruleFormat, rawDataSize, rawData, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, stcDataPtr = NULL", ruleFormat);

/*
    4. Call with rawDataPtr [NULL] and other params as in 1.
    Expected: GT_BAD_PTR.
*/
    st = cpssExMxPmPclRuleRawToStcConvert(ruleFormat, rawDataSize, NULL, &stcData);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, rawDataPtr = NULL", ruleFormat);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclRuleActionStcToRawConvert
(
    IN     CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT    actionFormat,
    IN     CPSS_EXMXPM_PCL_ACTION_UNT           *stcActionPtr,
    INOUT  GT_U32                               *rawDataSizePtr,
    OUT    GT_U32                               *rawActionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclRuleActionStcToRawConvert)
{
/*
1. Call with actionFormat [CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E],
             stcActionPtr{egress{
                                  pktCmd [CPSS_PACKET_CMD_DROP_HARD_E],
                                  actionStop [GT_TRUE],
                                  qos{ qosParamsModify{ modifyTc [GT_FALSE],
                                                        modifyUp [GT_FALSE],
                                                        modifyDp [GT_FALSE],
                                                        modifyDscp [GT_FALSE],
                                                        modifyExp [GT_FALSE] }
                                      qosParams{ tc [7],
                                                 dp [CPSS_DP_GREEN_E],
                                                 up [7],
                                                 dscp [63],
                                                 exp [7], }
                                      qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E] }
                                  vlan{ precedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E]
                                        nestedVlan [GT_FALSE]
                                        vlanCmd [CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E]
                                        vidOrInLif [0] }
                                  policer { activateCounter [GT_FALSE],
                                            activateMeter [GT_FALSE],
                                            policerPtr [0] } } },
             rawDataSizePtr [3],
             not NULL rawActionPtr
Expected: GT_OK.
2. Call cpssExMxPmPclRuleActionRawToStcConvert with not NULL stcActionPtr and other params as in 1.
Expected: GT_OK and the same stcActionPtr.
3. Call with actionFormat [0x5AAAAAA5]
Expected: GT_BAD_PARAM
4. Call with stcActionPtr[NULL] and other params from 1.
Expected: GT_BAD_PTR.
5. Call with rawDataSizePtr[NULL] and other params from 1.
Expected: GT_BAD_PTR.
6. Call with rawActionPtr[NULL] and other params from 1.
Expected: GT_BAD_PTR.
*/
    GT_STATUS   st      = GT_OK;
    GT_BOOL     isEqual = GT_FALSE;

    CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT    actionFormat;
    CPSS_EXMXPM_PCL_ACTION_UNT           stcAction;
    CPSS_EXMXPM_PCL_ACTION_UNT           stcActionGet;
    GT_U32                               rawDataSize = 3;
    GT_U32                               rawAction[20];

    cpssOsBzero((GT_VOID*) &stcAction, sizeof(stcAction));
    cpssOsBzero((GT_VOID*) &stcActionGet, sizeof(stcActionGet));
    cpssOsBzero((GT_VOID*) &rawAction, sizeof(rawAction));

/*
    1. Call with actionFormat [CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E],
                 stcActionPtr{egress{
                                      pktCmd [CPSS_PACKET_CMD_DROP_HARD_E],
                                      actionStop [GT_TRUE],
                                      qos{ qosParamsModify{ modifyTc [GT_FALSE],
                                                            modifyUp [GT_FALSE],
                                                            modifyDp [GT_FALSE],
                                                            modifyDscp [GT_FALSE],
                                                            modifyExp [GT_FALSE] }
                                          qosParams{ tc [7],
                                                     dp [CPSS_DP_GREEN_E],
                                                     up [7],
                                                     dscp [63],
                                                     exp [7], }
                                          qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E] }
                                      vlan{ precedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E]
                                            nestedVlan [GT_FALSE]
                                            vlanCmd [CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E]
                                            vidOrInLif [0] }
                                      policer { activateCounter [GT_FALSE],
                                                activateMeter [GT_FALSE],
                                                policerPtr [0] } } },
                 rawDataSizePtr [3],
                 not NULL rawActionPtr
    Expected: GT_OK.
*/
    actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E;

    stcAction.egress.pktCmd = CPSS_PACKET_CMD_DROP_HARD_E;
    stcAction.egress.actionStop = GT_TRUE;

    stcAction.egress.qos.qosParamsModify.modifyTc = GT_FALSE;
    stcAction.egress.qos.qosParamsModify.modifyUp = GT_FALSE;
    stcAction.egress.qos.qosParamsModify.modifyDp = GT_FALSE;
    stcAction.egress.qos.qosParamsModify.modifyDscp = GT_FALSE;
    stcAction.egress.qos.qosParamsModify.modifyExp = GT_FALSE;
    stcAction.egress.qos.qosParams.tc = 7;
    stcAction.egress.qos.qosParams.dp = CPSS_DP_GREEN_E;
    stcAction.egress.qos.qosParams.up = 7;
    stcAction.egress.qos.qosParams.dscp = 63;
    stcAction.egress.qos.qosParams.exp = 7;
    stcAction.egress.qos.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    stcAction.egress.vlan.precedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
    stcAction.egress.vlan.nestedVlan = GT_FALSE;
    stcAction.egress.vlan.vlanCmd = CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E;
    stcAction.egress.vlan.vidOrInLif = 0;

    stcAction.egress.policer.activateCounter = GT_FALSE;
    stcAction.egress.policer.activateMeter = GT_FALSE;
    stcAction.egress.policer.policerPtr = 0;
    rawDataSize = 3;

    st = cpssExMxPmPclRuleActionStcToRawConvert(actionFormat, &stcAction, &rawDataSize, rawAction);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, actionFormat);

/*
    2. Call cpssExMxPmPclRuleActionRawToStcConvert with not NULL stcActionPtr and other params as in 1.
    Expected: GT_OK and the same stcActionPtr.
*/
    st = cpssExMxPmPclRuleActionRawToStcConvert(actionFormat, rawDataSize, rawAction, &stcActionGet);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmPclRuleActionRawToStcConvert: %d", actionFormat);

    isEqual = (0 == cpssOsMemCmp((GT_VOID*) &stcAction.egress,
                                 (GT_VOID*) &stcActionGet.egress,
                                 sizeof(stcAction.egress) )) ? GT_TRUE : GT_FALSE;
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another stcAction.egress than was set", actionFormat);

/*
    3. Call with actionFormat [0x5AAAAAA5] and other params from 1.
    Expected: GT_BAD_PARAM
*/
    actionFormat = EXMXPM_PCL_INVALID_ENUM_CNS;

    st = cpssExMxPmPclRuleActionStcToRawConvert(actionFormat, &stcAction, &rawDataSize, rawAction);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, actionFormat);

    actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E;

/*
    4. Call with stcActionPtr[NULL] and other params from 1.
    Expected: GT_BAD_PTR.
*/
    st = cpssExMxPmPclRuleActionStcToRawConvert(actionFormat, NULL, &rawDataSize, rawAction);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, stcActionPtr = NULL", actionFormat);

/*
    5. Call with rawDataSizePtr[NULL] and other params from 1.
    Expected: GT_BAD_PTR.
*/
    st = cpssExMxPmPclRuleActionStcToRawConvert(actionFormat, &stcAction, NULL, rawAction);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, rawDataSizePtr = NULL", actionFormat);

/*
    6. Call with rawActionPtr[NULL] and other params from 1.
    Expected: GT_BAD_PTR.
*/
    st = cpssExMxPmPclRuleActionStcToRawConvert(actionFormat, &stcAction, &rawDataSize, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, rawActionPtr = NULL", actionFormat);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclRuleActionRawToStcConvert
(
    IN    CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT    actionFormat,
    IN    GT_U32                               rawDataSize,
    IN    GT_U32                               *rawActionPtr,
    OUT   CPSS_EXMXPM_PCL_ACTION_UNT           *stcActionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclRuleActionRawToStcConvert)
{
/*
1. Call with actionFormat [CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E],
             rawDataSize [3],
             rawActionPtr [0 .. 0]
             not NULL stcActionPtr
Expected: GT_OK.
2. Call with actionFormat [0x5AAAAAA5] and other params as in 1.
Expected: GT_BAD_PARAM.
3. Call with rawActionPtr [NULL] and other params as in 1.
Expected: GT_BAD_PTR.
4. Call with stcActionPtr [NULL] and other params as in 1.
Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT    actionFormat;
    GT_U32                               rawDataSize = 0;
    GT_U32                               rawAction[3];
    CPSS_EXMXPM_PCL_ACTION_UNT           stcAction;

    cpssOsBzero((GT_VOID*) &stcAction, sizeof(stcAction));
    cpssOsBzero((GT_VOID*) &rawAction, sizeof(rawAction));

/*
    1. Call with actionFormat [CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E],
                 rawDataSize [3],
                 rawActionPtr [0 .. 0]
                 not NULL stcActionPtr
    Expected: GT_OK.
*/
    actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E;
    rawDataSize = 3;

    st = cpssExMxPmPclRuleActionRawToStcConvert(actionFormat, rawDataSize, rawAction, &stcAction);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, actionFormat, rawDataSize);

/*
    2. Call with actionFormat [0x5AAAAAA5] and other params as in 1.
    Expected: GT_BAD_PARAM.
*/
    actionFormat = EXMXPM_PCL_INVALID_ENUM_CNS;

    st = cpssExMxPmPclRuleActionRawToStcConvert(actionFormat, rawDataSize, rawAction, &stcAction);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, actionFormat);

    actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E;

/*
    3. Call with rawActionPtr [NULL] and other params as in 1.
    Expected: GT_BAD_PTR.
*/
    st = cpssExMxPmPclRuleActionRawToStcConvert(actionFormat, rawDataSize, NULL, &stcAction);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, rawActionPtr = NULL", actionFormat);

/*
    4. Call with stcActionPtr [NULL] and other params as in 1.
    Expected: GT_BAD_PTR.
*/
    st = cpssExMxPmPclRuleActionRawToStcConvert(actionFormat, rawDataSize, rawAction, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, stcActionPtr = NULL", actionFormat);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclRuleSet
(
    IN GT_U8                                  devNum,
    IN CPSS_EXMXPM_TCAM_TYPE_ENT              tcamType,
    IN CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT  *ruleInfoPtr,
    IN CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT      actionFormat,
    IN GT_BOOL                                valid,
    IN CPSS_EXMXPM_PCL_RULE_FORMAT_UNT        *maskPtr,
    IN CPSS_EXMXPM_PCL_RULE_FORMAT_UNT        *patternPtr,
    IN CPSS_EXMXPM_PCL_ACTION_UNT             *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclRuleSet)
{
/*
ITERATE_DEVICES (ExMxPm)
1.1. Call with tcamType[CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E],
               ruleInfoPtr{ internalTcamRuleStartIndex [0] },
               ruleFormat [CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E,],
               actionFormat [CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E],
               valid [GT_TRUE],
               non-NULL maskPtr {  ruleStdNotIp { 0xFF, 0xFF, …0xFF} },
               non-NULL patternPtr { set ruleStdNotIp to default }
               and non-NULL actionPtr { set egres to default }.
Expected: GT_OK.

1.2. Call cpssExMxPmPclRuleGet with non-NULL maskPtr, non-NULL patternPtr, non-NULL actionPtr and other parameters the same as in 1.1.
Expected: GT_OK and the same maskPtr, patternPtr and actionPtr.
1.3. Call function with out of range tcamType [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.4. Call function with out of range ruleFormat [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.5. Call function with out of range actionFormat [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.6. Call function with ruleInfoPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
1.7. Call function with maskPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
1.8. Call function with patternPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
1.9. Call function with actionPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;

    GT_BOOL                 isEqual;

    CPSS_EXMXPM_TCAM_TYPE_ENT              tcamType;
    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT  ruleInfo;
    CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat;
    CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT      actionFormat;

    GT_BOOL                                valid;
    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT        mask;
    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT        pattern;
    CPSS_EXMXPM_PCL_ACTION_UNT             action;

    GT_BOOL                                validGet;
    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT        maskGet;
    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT        patternGet;
    CPSS_EXMXPM_PCL_ACTION_UNT             actionGet;


    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));

    cpssOsBzero((GT_VOID*) &maskGet, sizeof(maskGet));
    cpssOsBzero((GT_VOID*) &patternGet, sizeof(patternGet));
    cpssOsBzero((GT_VOID*) &actionGet, sizeof(actionGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tcamType[CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E],
                           ruleInfoPtr{ internalTcamRuleStartIndex [0] },
                           ruleFormat [CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E,],
                           actionFormat [CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E],
                           valid [GT_TRUE],
                           non-NULL maskPtr {  ruleStdNotIp { 0xFF, 0xFF, …0xFF} },
                           non-NULL patternPtr { set ruleStdNotIp to default }
                           and non-NULL actionPtr { set egres to default }.
            Expected: GT_OK.
        */
        tcamType     = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
        ruleFormat   = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E;
        actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E;
        valid        = GT_TRUE;

        ruleInfo.internalTcamRuleStartIndex = 0;
        pclRuleMaskPatternDefaultSet(&mask, &pattern);
        pclActionDefaultSet(&action);

        st = cpssExMxPmPclRuleSet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamType);

        /* 1.2. Call cpssExMxPmPclRuleGet with non-NULL maskPtr, non-NULL
         patternPtr, non-NULL actionPtr and other parameters the same as in 1.1.
          Expected: GT_OK and the same maskPtr, patternPtr and actionPtr. */

        cpssOsBzero((GT_VOID*) &maskGet, sizeof(maskGet));
        cpssOsBzero((GT_VOID*) &patternGet, sizeof(patternGet));
        cpssOsBzero((GT_VOID*) &actionGet, sizeof(actionGet));

        st = cpssExMxPmPclRuleGet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, &validGet, &maskGet, &patternGet, &actionGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPclRuleGet: %d, %d", dev, tcamType);

        if (GT_OK == st)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(valid, validGet, "get another valid than was set: %d, %d", dev, tcamType);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&mask.ruleEgrStandardNotIp, (GT_VOID*)&maskGet.ruleEgrStandardNotIp, sizeof (maskGet.ruleEgrStandardNotIp)))? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, "get another mask than was set: %d, %d", dev, tcamType);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&pattern.ruleEgrStandardNotIp, (GT_VOID*)&patternGet.ruleEgrStandardNotIp, sizeof (pattern.ruleEgrStandardNotIp)))? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, "get another pattern than was set: %d, %d, %d", dev, tcamType);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&action.egress, (GT_VOID*)&actionGet.egress, sizeof (action.egress)))? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,"get another action than was set: %d, %d", dev, tcamType);
        }

        /* 1.3. Call function with out of range tcamType [0x5AAAAAA5] and
           other parameters the same as in 1.1. Expected: GT_BAD_PARAM. */
        tcamType = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclRuleSet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, tcamType);

        tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;  /* restore */

        /* 1.4. Call function with out of range ruleFormat [0x5AAAAAA5] and
           other parameters the same as in 1.1. Expected: GT_BAD_PARAM. */
        ruleFormat = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclRuleSet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ruleFormat = %d", dev, ruleFormat);

        ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E;  /* restore */

        /* 1.5. Call function with out of range actionFormat [0x5AAAAAA5] and
        other parameters the same as in 1.1. Expected: GT_BAD_PARAM. */
        actionFormat = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclRuleSet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, actionFormat = %d", dev, actionFormat);

        actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E;  /* restore */

        /* 1.6. Call function with ruleInfoPtr [NULL] and other parameters the
           same as in 1.1. Expected: GT_BAD_PTR.    */

        st = cpssExMxPmPclRuleSet(dev, tcamType, NULL, ruleFormat, actionFormat, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ruleInfoPtr = NULL", dev);

        /* 1.7. Call function with maskPtr [NULL] and other parameters
           the same as in 1.1. Expected: GT_BAD_PTR. */

        st = cpssExMxPmPclRuleSet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, valid, NULL, &pattern, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskPtr = NULL", dev);

        /* 1.8. Call function with patternPtr [NULL] and other parameters
           the same as in 1.1. Expected: GT_BAD_PTR. */

        st = cpssExMxPmPclRuleSet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, valid, &mask, NULL, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, patternPtr = NULL", dev);

        /* 1.9. Call function with actionPtr [NULL] and other parameters
           the same as in 1.1. Expected: GT_BAD_PTR. */

        st = cpssExMxPmPclRuleSet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, valid, &mask, &pattern, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionPtr = NULL", dev);
    }

    tcamType     = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    ruleFormat   = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E;
    actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E;
    valid        = GT_TRUE;

    ruleInfo.internalTcamRuleStartIndex = 0;
    pclRuleMaskPatternDefaultSet(&mask, &pattern);
    pclActionDefaultSet(&action);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclRuleSet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclRuleSet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, valid, &mask, &pattern, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclRuleGet
(
    IN  GT_U8                                 devNum,
    IN CPSS_EXMXPM_TCAM_TYPE_ENT              tcamType,
    IN CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT  *ruleInfoPtr,
    IN  CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat,
    IN  CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT     actionFormat,
    OUT GT_BOOL                               *validPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_UNT       *maskPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_UNT       *patternPtr,
    OUT CPSS_EXMXPM_PCL_ACTION_UNT            *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclRuleGet)
{
/*
ITERATE_DEVICES (ExMxPm)
1.1. Call cpssExMxPmPclRuleSet with    tcamType[CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E],
                                       ruleInfoPtr{ internalTcamRuleStartIndex [0] },
                                       ruleFormat [CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E,],
                                       actionFormat [CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E],
                                       valid [GT_TRUE],
                                       non-NULL maskPtr {  ruleStdNotIp { 0xFF, 0xFF, …0xFF} },
                                       non-NULL patternPtr { set ruleStdNotIp to default }
                                       and non-NULL actionPtr { set egres to default }.
Expected: GT_OK.
1.2. Call with not NULL pointers and other parameters as in 1.1. Expected: GT_OK.
1.3. Call function with out of range tcamType [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.4. Call function with out of range ruleFormat [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.5. Call function with out of range actionFormat [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.6. Call function with ruleInfoPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
1.7. Call function with validPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
1.8. Call function with maskPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
1.9. Call function with patternPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
1.10. Call function with actionPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;

    CPSS_EXMXPM_TCAM_TYPE_ENT              tcamType;
    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT  ruleInfo;
    CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat;
    CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT      actionFormat;

    GT_BOOL                                valid;
    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT        mask;
    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT        pattern;
    CPSS_EXMXPM_PCL_ACTION_UNT             action;

    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT        maskGet;
    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT        patternGet;
    CPSS_EXMXPM_PCL_ACTION_UNT             actionGet;


    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));

    cpssOsBzero((GT_VOID*) &maskGet, sizeof(maskGet));
    cpssOsBzero((GT_VOID*) &patternGet, sizeof(patternGet));
    cpssOsBzero((GT_VOID*) &actionGet, sizeof(actionGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tcamType[CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E],
                           ruleInfoPtr{ internalTcamRuleStartIndex [0] },
                           ruleFormat [CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E,],
                           actionFormat [CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E],
                           valid [GT_TRUE],
                           non-NULL maskPtr {  ruleStdNotIp { 0xFF, 0xFF, …0xFF} },
                           non-NULL patternPtr { set ruleStdNotIp to default }
                           and non-NULL actionPtr { set egres to default }.
            Expected: GT_OK.
        */
        tcamType     = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
        ruleFormat   = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E;
        actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E;
        valid        = GT_TRUE;

        ruleInfo.internalTcamRuleStartIndex = 0;
        pclRuleMaskPatternDefaultSet(&mask, &pattern);
        pclActionDefaultSet(&action);

        st = cpssExMxPmPclRuleSet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPclRuleSet: %d, %d", dev, tcamType);

        /*
            1.2. Call with not NULL pointers and other parameters as in 1.1. Expected: GT_OK.
        */
        st = cpssExMxPmPclRuleGet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, &valid, &maskGet, &patternGet, &actionGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamType);

        /*
            1.3. Call function with out of range tcamType [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
        */
        tcamType = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclRuleGet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, &valid, &maskGet, &patternGet, &actionGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tcamType = %d", dev, tcamType);

        tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;

        /*
            1.4. Call function with out of range ruleFormat [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
        */
        ruleFormat = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclRuleGet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, &valid, &maskGet, &patternGet, &actionGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ruleFormat = %d", dev, ruleFormat);

        ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E;

        /*
            1.5. Call function with out of range actionFormat [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
        */
        actionFormat = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclRuleGet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, &valid, &maskGet, &patternGet, &actionGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, actionFormat = %d", dev, actionFormat);

        actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E;

        /*
            1.6. Call function with ruleInfoPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclRuleGet(dev, tcamType, NULL, ruleFormat, actionFormat, &valid, &maskGet, &patternGet, &actionGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ruleInfoPtr = NULL", dev);

        /*
            1.7. Call function with validPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclRuleGet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, NULL, &maskGet, &patternGet, &actionGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validPtr = NULL", dev);

        /*
            1.8. Call function with maskPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclRuleGet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, &valid, NULL, &patternGet, &actionGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskGetPtr = NULL", dev);

        /*
            1.9. Call function with patternPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclRuleGet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, &valid, &maskGet, NULL, &actionGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, patternGetPtr = NULL", dev);

        /*
            1.10. Call function with actionPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclRuleGet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, &valid, &maskGet, &patternGet, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionGetPtr = NULL", dev);
    }

    tcamType     = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    ruleFormat   = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E;
    actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E;
    valid        = GT_TRUE;

    ruleInfo.internalTcamRuleStartIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclRuleGet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, &valid, &maskGet, &patternGet, &actionGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclRuleGet(dev, tcamType, &ruleInfo, ruleFormat, actionFormat, &valid, &maskGet, &patternGet, &actionGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclRuleActionSet
(
    IN GT_U8                                  devNum,
    IN CPSS_EXMXPM_TCAM_TYPE_ENT              tcamType,
    IN CPSS_EXMXPM_TCAM_ACTION_INFO_UNT       *actionInfoPtr,
    IN CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT      actionFormat,
    IN CPSS_EXMXPM_PCL_ACTION_UNT             *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclRuleActionSet)
{
/*
ITERATE_DEVICES (ExMxPm)
1.1. Call with actionPtr { egress{
                                  pktCmd [CPSS_PACKET_CMD_DROP_SOFT_E],
                                  actionStop [GT_FALSE]}
Expected: GT_OK.
1.2. Call function with out of range tcamType [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.3. Call function with out of range actionFormat [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.4. Call function with actionPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
1.5. Call function with actionInfoPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_TCAM_TYPE_ENT              tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    CPSS_EXMXPM_TCAM_ACTION_INFO_UNT       actionInfo;
    CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT      actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E;
    CPSS_EXMXPM_PCL_ACTION_UNT             action;


    cpssOsBzero((GT_VOID*) &action, sizeof(action));
    cpssOsBzero((GT_VOID*) &actionInfo, sizeof(actionInfo));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with actionPtr { egress{
                                              pktCmd [CPSS_PACKET_CMD_DROP_SOFT_E],
                                              actionStop [GT_FALSE]}
            Expected: GT_OK.
        */
        tcamType     = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
        actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E;
        actionInfo.internalTcamRuleStartIndex = 0;

        pclActionDefaultSet(&action);
        action.egress.pktCmd     = CPSS_PACKET_CMD_DROP_SOFT_E;
        action.egress.actionStop = GT_FALSE;

        st = cpssExMxPmPclRuleActionSet(dev, tcamType, &actionInfo, actionFormat, &action);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamType);

        /*
            1.2. Call function with out of range tcamType [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
        */
        tcamType = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclRuleActionSet(dev, tcamType, &actionInfo, actionFormat, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tcamType = %d", dev, tcamType);

        tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;

        /*
            1.3. Call function with out of range actionFormat [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
        */
        actionFormat = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclRuleActionSet(dev, tcamType, &actionInfo, actionFormat, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, actionFormat = %d", dev, actionFormat);

        actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E;

        /*
            1.4. Call function with actionInfoPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclRuleActionSet(dev, tcamType, NULL, actionFormat, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionInfoPtr = %d", dev);

        /*
            1.5. Call function with actionPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclRuleActionSet(dev, tcamType, &actionInfo, actionFormat, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionPtr = %d", dev);
    }

    tcamType     = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E;
    actionInfo.internalTcamRuleStartIndex = 0;
    pclActionDefaultSet(&action);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclRuleActionSet(dev, tcamType, &actionInfo, actionFormat, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclRuleActionSet(dev, tcamType, &actionInfo, actionFormat, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPclRuleActionGet
(
    IN  GT_U8                                  devNum,
    IN  CPSS_EXMXPM_TCAM_TYPE_ENT              tcamType,
    IN  CPSS_EXMXPM_TCAM_ACTION_INFO_UNT       *actionInfoPtr,
    IN  CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT      actionFormat,
    OUT CPSS_EXMXPM_PCL_ACTION_UNT             *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclRuleActionGet)
{
/*
ITERATE_DEVICES (ExMxPm)
1.1. Call with not NULL pointers and other parameters as in 1.1. Expected: GT_OK.
1.2. Call function with out of range tcamType [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.3. Call function with out of range actionFormat [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.4. Call function with actionPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
1.5. Call function with actionInfoPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;

    CPSS_EXMXPM_TCAM_TYPE_ENT              tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    CPSS_EXMXPM_TCAM_ACTION_INFO_UNT       actionInfo;
    CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT      actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E;
    CPSS_EXMXPM_PCL_ACTION_UNT             action;


    cpssOsBzero((GT_VOID*) &action, sizeof(action));
    cpssOsBzero((GT_VOID*) &actionInfo, sizeof(actionInfo));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL pointers and other parameters as in 1.1. Expected: GT_OK.
        */
        tcamType     = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
        actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E;
        actionInfo.internalTcamRuleStartIndex = 0;

        pclActionDefaultSet(&action);
        actionInfo.internalTcamRuleStartIndex = 0;

        st = cpssExMxPmPclRuleActionGet(dev, tcamType, &actionInfo, actionFormat, &action);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamType);

        /*
            1.2. Call function with out of range tcamType [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
        */
        tcamType = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclRuleActionGet(dev, tcamType, &actionInfo, actionFormat, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tcamType = %d", dev, tcamType);

        tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;

        /*
            1.3. Call function with out of range actionFormat [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
        */
        actionFormat = EXMXPM_PCL_INVALID_ENUM_CNS;

        st = cpssExMxPmPclRuleActionGet(dev, tcamType, &actionInfo, actionFormat, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, actionFormat = %d", dev, actionFormat);

        actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E;

        /*
            1.4. Call function with actionInfoPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclRuleActionGet(dev, tcamType, NULL, actionFormat, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionInfoPtr = %d", dev);

        /*
            1.5. Call function with actionPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmPclRuleActionGet(dev, tcamType, &actionInfo, actionFormat, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionPtr = %d", dev);
    }

    tcamType     = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E;
    actionInfo.internalTcamRuleStartIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPclRuleActionGet(dev, tcamType, &actionInfo, actionFormat, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmPclRuleActionGet(dev, tcamType, &actionInfo, actionFormat, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill UDB CFG table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmPclFillUdbCfgTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in UDB CFG table.
         Call cpssExMxPmPclUdbTableEntrySet with entryIndex[0..numEntries - 1],
                                               udbCfgPtr {udbCfg[23]{valid [GT_TRUE],
                                                                     offsetType [CPSS_EXMXPM_PCL_OFFSET_L2_E],
                                                                     offset [0],
                                                                     tcpUdpComparatorsEnable [GT_FALSE] }
                                                          inlifIdEnable [GT_FALSE],
                                                          profileIdEnable [GT_FALSE] }
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmPclUdbTableEntrySet with entryIndex[numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in UDB CFG table and compare with original.
         Call cpssExMxPmPclUdbTableEntryGet with not NULL udbCfgPtr and other params from 1.2.
    Expected: GT_OK and the same udbCfgPtr as was set.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmPclUdbTableEntryGet with entryIndex[numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual    = GT_FALSE;
    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    CPSS_EXMXPM_PCL_UDB_TABLE_ENTRY_STC   udbCfg;
    CPSS_EXMXPM_PCL_UDB_TABLE_ENTRY_STC   udbCfgGet;


    cpssOsBzero((GT_VOID*) &udbCfg, sizeof(udbCfg));
    cpssOsBzero((GT_VOID*) &udbCfgGet, sizeof(udbCfgGet));

    /* Fill the entry for UDB CFG table */
    udbCfg.inlifIdEnable   = GT_FALSE;
    udbCfg.profileIdEnable = GT_FALSE;

    for(iTemp = 0; iTemp < 23; ++iTemp)
    {
        udbCfg.udbCfg[iTemp].valid = GT_TRUE;
        udbCfg.udbCfg[iTemp].offsetType = CPSS_EXMXPM_PCL_OFFSET_L2_E;
        udbCfg.udbCfg[iTemp].offset = iTemp % 128;
        udbCfg.udbCfg[iTemp].tcpUdpComparatorsEnable = GT_FALSE;
    }

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = PRV_CPSS_EXMXPM_DEV_TBLS_MAC(dev)->TTI.userDefinedBytesConfigMemory.numOfEntries;

        /* 1.2. Fill all entries in UDB CFG table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmPclUdbTableEntrySet(dev, iTemp, &udbCfg);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPclUdbTableEntrySet: %d, %d", dev, iTemp);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmPclUdbTableEntrySet(dev, numEntries, &udbCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPclUdbTableEntrySet: %d, %d", dev, iTemp);

        /* 1.4. Read all entries in UDB CFG table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmPclUdbTableEntryGet(dev, iTemp, &udbCfgGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPclUdbTableEntryGet: %d, %d", dev, iTemp);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &udbCfg, (GT_VOID*) &udbCfgGet, sizeof(udbCfg) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another udbCfg than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmPclUdbTableEntryGet(dev, numEntries, &udbCfgGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmPclUdbTableEntryGet: %d, %d", dev, iTemp);
    }
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssExMxPmPcl suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmPcl)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclPolicyEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclPolicyEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclPortIngressPolicyEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclPortIngressPolicyEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclPortLookupCfgTblAccessModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclPortLookupCfgTblAccessModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclCfgTblSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclCfgTblGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclInvalidUdbCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclInvalidUdbCmdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclTcpUdpPortComparatorSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclTcpUdpPortComparatorGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclProfileAssignmentModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclProfileAssignmentModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclUdbTableEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclUdbTableEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclIngressPolicyEngineCfgSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclIngressPolicyEngineCfgGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclProfileIdTblSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclProfileIdTblGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclEgressLookupTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclEgressLookupTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclUserDefinedEtherTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclUserDefinedEtherTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclRuleStcToRawConvert)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclRuleRawToStcConvert)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclRuleActionStcToRawConvert)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclRuleActionRawToStcConvert)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclRuleSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclRuleGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclRuleActionSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclRuleActionGet)
    /* Tests for Tables */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPclFillUdbCfgTable)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmPcl)


/*----------------------------------------------------------------------------*/

/*  Internal function. Is used for filling PCL action structure     */
/*  with default values which are used for most of all tests.       */
static void pclActionDefaultSet
(
    OUT CPSS_EXMXPM_PCL_ACTION_UNT     *actionPtr
)
{
/*
{egress{
      pktCmd [CPSS_PACKET_CMD_DROP_HARD_E],
      actionStop [GT_TRUE],
      qos{ qosParamsModify{ modifyTc [GT_FALSE],
                            modifyUp [GT_FALSE],
                            modifyDp [GT_FALSE],
                            modifyDscp [GT_FALSE],
                            modifyExp [GT_FALSE] }
          qosParams{ tc [7],
                     dp [CPSS_DP_GREEN_E],
                     up [7],
                     dscp [63],
                     exp [7], }
          qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E] }
      vlan{ precedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E]
            nestedVlan [GT_FALSE]
            vlanCmd [CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E]
            vidOrInLif [0] }
      policer { activateCounter [GT_FALSE],
                activateMeter [GT_FALSE],
                policerPtr [0] } } },
*/
    cpssOsBzero((GT_VOID*)actionPtr, sizeof(CPSS_EXMXPM_PCL_ACTION_UNT));

    actionPtr->egress.pktCmd = CPSS_PACKET_CMD_DROP_HARD_E;
    actionPtr->egress.actionStop = GT_TRUE;

    actionPtr->egress.qos.qosParamsModify.modifyTc = GT_FALSE;
    actionPtr->egress.qos.qosParamsModify.modifyUp = GT_FALSE;
    actionPtr->egress.qos.qosParamsModify.modifyDp = GT_FALSE;
    actionPtr->egress.qos.qosParamsModify.modifyDscp = GT_FALSE;
    actionPtr->egress.qos.qosParamsModify.modifyExp = GT_FALSE;
    actionPtr->egress.qos.qosParams.tc = 7;
    actionPtr->egress.qos.qosParams.dp = CPSS_DP_GREEN_E;
    actionPtr->egress.qos.qosParams.up = 7;
    actionPtr->egress.qos.qosParams.dscp = 63;
    actionPtr->egress.qos.qosParams.exp = 7;
    actionPtr->egress.qos.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    actionPtr->egress.vlan.precedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
    actionPtr->egress.vlan.nestedVlan = GT_FALSE;
    actionPtr->egress.vlan.vlanCmd = CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E;
    actionPtr->egress.vlan.vidOrInLif = 0;

    actionPtr->egress.policer.activateCounter = GT_FALSE;
    actionPtr->egress.policer.activateMeter = GT_FALSE;
    actionPtr->egress.policer.policerPtr = 0;
}

/*----------------------------------------------------------------------------*/

/*  Internal function. Is used for filling PCL mask & pattern structure     */
/*  with default values which are used for most of all tests.               */

static void pclRuleMaskPatternDefaultSet
(
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_UNT     *maskPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_UNT     *patternPtr
)
/*
ruleEgrStandardNotIp{
                    commonEgr{  pclId [0],
                                isMpls [0],
                                srcPort [0],
                                isTagged [0],
                                vid [100],
                                up [7],
                                isIp [1] }
                   commonEgrStd{isIpv4 [1],
                                isArp [0],
                                isL2Valid [0],
                                packetCmd [0],
                                trgOrCpuCode0_6OrCosOrSniff [0],
                                srcTrgOrTxMirror [0],
                                sourceId [0],
                                srcInfo [0],
                                srcIsTrunk [0],
                                isIpOrMplsRouted [0],
                                egrFilterRegOrCpuCode7 [0],
                                trgPort_Extrn [0],
                                srcDev8_7_Extrn [0],
                                exp_Extrn [0],
                                egrFilterReg_Extrn [0],
                                macSa_Extrn {arEther[10, 20, 30, 40, 50, 60]} }
                    l2EncapType [0],
                    ethType [0],
                    macDa {arEther[10, 20, 30, 40, 50, 60]},
                    macSa {arEther[10, 20, 30, 40, 50, 60]},
                    isBc_Extrn [0] } },
*/
{
    cpssOsMemSet((GT_VOID*)maskPtr, 0, sizeof(CPSS_EXMXPM_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet((GT_VOID*)patternPtr, 0, sizeof(CPSS_EXMXPM_PCL_RULE_FORMAT_UNT));

    /* CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E */
    patternPtr->ruleEgrStandardNotIp.commonEgr.pclId = 0;
    patternPtr->ruleEgrStandardNotIp.commonEgr.isMpls = 0;
    patternPtr->ruleEgrStandardNotIp.commonEgr.srcPort = 0;
    patternPtr->ruleEgrStandardNotIp.commonEgr.isTagged = 0;
    patternPtr->ruleEgrStandardNotIp.commonEgr.vid = 100;
    patternPtr->ruleEgrStandardNotIp.commonEgr.up = 7;
    patternPtr->ruleEgrStandardNotIp.commonEgr.isIp = 1;

    patternPtr->ruleEgrStandardNotIp.commonEgrStd.isIpv4 = 1;
    patternPtr->ruleEgrStandardNotIp.commonEgrStd.isArp = 0;
    patternPtr->ruleEgrStandardNotIp.commonEgrStd.isL2Valid = 0;
    patternPtr->ruleEgrStandardNotIp.commonEgrStd.packetCmd = 0;
    patternPtr->ruleEgrStandardNotIp.commonEgrStd.trgOrCpuCode0_6OrCosOrSniff = 0;
    patternPtr->ruleEgrStandardNotIp.commonEgrStd.srcTrgOrTxMirror = 0;
    patternPtr->ruleEgrStandardNotIp.commonEgrStd.sourceId = 0;
    patternPtr->ruleEgrStandardNotIp.commonEgrStd.srcInfo = 0;
    patternPtr->ruleEgrStandardNotIp.commonEgrStd.srcIsTrunk = 0;
    patternPtr->ruleEgrStandardNotIp.commonEgrStd.isIpOrMplsRouted = 0;
    patternPtr->ruleEgrStandardNotIp.commonEgrStd.egrFilterRegOrCpuCode7 = 0;


    patternPtr->ruleEgrStandardNotIp.l2EncapType = 0;
    patternPtr->ruleEgrStandardNotIp.ethType = 0;

    patternPtr->ruleEgrStandardNotIp.macDa.arEther[0] = 10;
    patternPtr->ruleEgrStandardNotIp.macDa.arEther[1] = 20;
    patternPtr->ruleEgrStandardNotIp.macDa.arEther[2] = 30;
    patternPtr->ruleEgrStandardNotIp.macDa.arEther[3] = 40;
    patternPtr->ruleEgrStandardNotIp.macDa.arEther[4] = 50;
    patternPtr->ruleEgrStandardNotIp.macDa.arEther[5] = 60;

    patternPtr->ruleEgrStandardNotIp.macSa.arEther[0] = 10;
    patternPtr->ruleEgrStandardNotIp.macSa.arEther[1] = 20;
    patternPtr->ruleEgrStandardNotIp.macSa.arEther[2] = 30;
    patternPtr->ruleEgrStandardNotIp.macSa.arEther[3] = 40;
    patternPtr->ruleEgrStandardNotIp.macSa.arEther[4] = 50;
    patternPtr->ruleEgrStandardNotIp.macSa.arEther[5] = 60;

    /* mask */
    maskPtr->ruleEgrStandardNotIp.commonEgr.pclId = 0x3FF;
    maskPtr->ruleEgrStandardNotIp.commonEgr.isMpls = 1;
    maskPtr->ruleEgrStandardNotIp.commonEgr.srcPort = 0x3F;
    maskPtr->ruleEgrStandardNotIp.commonEgr.isTagged = 1;
    maskPtr->ruleEgrStandardNotIp.commonEgr.vid = 0xFFF;
    maskPtr->ruleEgrStandardNotIp.commonEgr.up = 0x07;
    maskPtr->ruleEgrStandardNotIp.commonEgr.isIp = 1;

    maskPtr->ruleEgrStandardNotIp.commonEgrStd.isIpv4 = 1;
    maskPtr->ruleEgrStandardNotIp.commonEgrStd.isArp = 1;
    maskPtr->ruleEgrStandardNotIp.commonEgrStd.isL2Valid = 1;
    maskPtr->ruleEgrStandardNotIp.commonEgrStd.packetCmd = 0x3;
    maskPtr->ruleEgrStandardNotIp.commonEgrStd.trgOrCpuCode0_6OrCosOrSniff = 0x7F;
    maskPtr->ruleEgrStandardNotIp.commonEgrStd.srcTrgOrTxMirror = 1;
    maskPtr->ruleEgrStandardNotIp.commonEgrStd.sourceId = 0x1F;
    maskPtr->ruleEgrStandardNotIp.commonEgrStd.srcInfo = 0x7F;
    maskPtr->ruleEgrStandardNotIp.commonEgrStd.srcIsTrunk = 1;
    maskPtr->ruleEgrStandardNotIp.commonEgrStd.isIpOrMplsRouted = 1;
    maskPtr->ruleEgrStandardNotIp.commonEgrStd.egrFilterRegOrCpuCode7 = 1;

    maskPtr->ruleEgrStandardNotIp.l2EncapType = 1;
    maskPtr->ruleEgrStandardNotIp.ethType = 0xFFFF;

    maskPtr->ruleEgrStandardNotIp.macDa.arEther[0] = 0xFF;
    maskPtr->ruleEgrStandardNotIp.macDa.arEther[1] = 0xFF;
    maskPtr->ruleEgrStandardNotIp.macDa.arEther[2] = 0xFF;
    maskPtr->ruleEgrStandardNotIp.macDa.arEther[3] = 0xFF;
    maskPtr->ruleEgrStandardNotIp.macDa.arEther[4] = 0xFF;
    maskPtr->ruleEgrStandardNotIp.macDa.arEther[5] = 0xFF;

    maskPtr->ruleEgrStandardNotIp.macSa.arEther[0] = 0xFF;
    maskPtr->ruleEgrStandardNotIp.macSa.arEther[1] = 0xFF;
    maskPtr->ruleEgrStandardNotIp.macSa.arEther[2] = 0xFF;
    maskPtr->ruleEgrStandardNotIp.macSa.arEther[3] = 0xFF;
    maskPtr->ruleEgrStandardNotIp.macSa.arEther[4] = 0xFF;
    maskPtr->ruleEgrStandardNotIp.macSa.arEther[5] = 0xFF;

}

