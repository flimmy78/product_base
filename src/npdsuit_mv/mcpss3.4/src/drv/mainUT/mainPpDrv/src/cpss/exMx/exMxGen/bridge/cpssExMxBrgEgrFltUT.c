/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxBrgEgrFltUT.c
*
* DESCRIPTION:
*       Unit test for cpssExMxBrgEgrFlt.c that provides Egress filtering 
*       facility ExMx cpss implementation
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>

#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgEgrFlt.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum                     */
#define BRG_EGR_FLT_INVALID_ENUM_CNS             0x5AAAAAA5
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgMcUplinkFilterSet
(
    IN GT_U8    dev,
    IN GT_BOOL  status
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgMcUplinkFilterSet)
{
/*
ITERATE_DEVICES
1.1. Call function with status [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_BOOL status;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with status = GT_TRUE and GT_FALSE.   */
        /* Must return GT_OK.                                           */
        status = GT_TRUE;

        st = cpssExMxBrgMcUplinkFilterSet(dev, status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, status);

        status = GT_FALSE;

        st = cpssExMxBrgMcUplinkFilterSet(dev, status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, status);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    status = GT_TRUE;

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgMcUplinkFilterSet(dev, status);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgMcUplinkFilterSet(dev, status);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgPortEgrFltUnkEnable
(
    IN GT_U8    dev,
    IN GT_U8    port,
    IN GT_BOOL  value
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgPortEgrFltUnkEnable)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call with value [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     value;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For each active device id go over all active virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with value [GT_TRUE and GT_FALSE].   */
            /* Expected: GT_OK.                                 */
            value = GT_FALSE;

            st = cpssExMxBrgPortEgrFltUnkEnable(dev, port, value);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, value);

            value = GT_TRUE;

            st = cpssExMxBrgPortEgrFltUnkEnable(dev, port, value);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, value);
        }

        value = GT_FALSE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
             st = cpssExMxBrgPortEgrFltUnkEnable(dev, port, value);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        /* value == GT_TRUE */

        st = cpssExMxBrgPortEgrFltUnkEnable(dev, port, value);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = 0;
    value = GT_TRUE;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */

        st = cpssExMxBrgPortEgrFltUnkEnable(dev, port, value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgPortEgrFltUnkEnable(dev, port, value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgUnkUcEgressFilterCmdSet
(
    IN GT_U8                            dev,
    IN CPSS_EXMX_EGR_FILTER_CMD_ENT     egrCmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgUnkUcEgressFilterCmdSet)
{
/*
ITERATE_DEVICES
1.1. Call function with egrCmd [CPSS_EXMX_VLAN_FRWD_E]. 
    Expected: GT_OK.
1.2. Call with out of range egrCmd [0x5AAAAAA5]. 
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                           dev;
    CPSS_EXMX_EGR_FILTER_CMD_ENT    egrCmd;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with egrCmd [CPSS_EXMX_VLAN_FRWD_E]. */
        /* Expected: GT_OK.                                        */
        egrCmd = CPSS_EXMX_VLAN_FRWD_E;

        st = cpssExMxBrgUnkUcEgressFilterCmdSet(dev, egrCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, egrCmd);

        /* 1.2. Call with out of range egrCmd [0x5AAAAAA5].     */
        /* Expected: GT_BAD_PARAM.                              */
        egrCmd = BRG_EGR_FLT_INVALID_ENUM_CNS;

        st = cpssExMxBrgUnkUcEgressFilterCmdSet(dev, egrCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, egrCmd);
    }

    egrCmd = CPSS_EXMX_VLAN_FRWD_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgUnkUcEgressFilterCmdSet(dev, egrCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgUnkUcEgressFilterCmdSet(dev, egrCmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgUnregMcEgressFilterCmdSet
(
    IN GT_U8              dev,
    IN CPSS_EXMX_EGR_FILTER_CMD_ENT  egrCmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgUnregMcEgressFilterCmdSet)
{
/*
ITERATE_DEVICES
1.1. Call function with egrCmd [CPSS_EXMX_VLAN_CPU_FRWD_E]. 
    Expected: GT_OK.
1.2. Call with out of range egrCmd [0x5AAAAAA5]. 
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                           dev;
    CPSS_EXMX_EGR_FILTER_CMD_ENT    egrCmd;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with egrCmd [CPSS_EXMX_VLAN_CPU_FRWD_E].  */
        /* Expected: GT_OK.                                             */
        egrCmd = CPSS_EXMX_VLAN_CPU_FRWD_E;

        st = cpssExMxBrgUnregMcEgressFilterCmdSet(dev, egrCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, egrCmd);

        /* 1.2. Call with out of range egrCmd [0x5AAAAAA5].     */
        /* Expected: GT_BAD_PARAM.                              */
        egrCmd = BRG_EGR_FLT_INVALID_ENUM_CNS;

        st = cpssExMxBrgUnregMcEgressFilterCmdSet(dev, egrCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, egrCmd);
    }

    egrCmd = CPSS_EXMX_VLAN_CPU_FRWD_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgUnregMcEgressFilterCmdSet(dev, egrCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgUnregMcEgressFilterCmdSet(dev, egrCmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgCpuBcFilteringEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  en
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgCpuBcFilteringEnable)
{
/*
ITERATE_DEVICES
1.1. Call function with en [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/

    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_BOOL en;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with en = GT_TRUE and GT_FALSE.   */
        /* Must return GT_OK.                                       */
        en = GT_TRUE;

        st = cpssExMxBrgCpuBcFilteringEnable(dev, en);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, en);

        en = GT_FALSE;

        st = cpssExMxBrgCpuBcFilteringEnable(dev, en);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, en);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    en = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgCpuBcFilteringEnable(dev, en);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* en == GT_FALSE */

    st = cpssExMxBrgCpuBcFilteringEnable(dev, en);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanEgressFilteringEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  en
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanEgressFilteringEnable)
{
/*
ITERATE_DEVICES
1.1. Call function with en [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/

    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_BOOL en;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with en = GT_TRUE and GT_FALSE.   */
        /* Must return GT_OK.                                       */
        en = GT_TRUE;

        st = cpssExMxBrgVlanEgressFilteringEnable(dev, en);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, en);

        en = GT_FALSE;

        st = cpssExMxBrgVlanEgressFilteringEnable(dev, en);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, en);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    en = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgVlanEgressFilteringEnable(dev, en);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* en == GT_FALSE */

    st = cpssExMxBrgVlanEgressFilteringEnable(dev, en);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgEgressFilteringForL3Enable
(
    IN GT_U8    dev,
    IN GT_BOOL  en
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgEgressFilteringForL3Enable)
{
/*
ITERATE_DEVICES
1.1. Call function with en [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/

    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_BOOL en;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with en = GT_TRUE and GT_FALSE.   */
        /* Must return GT_OK.                                       */
        en = GT_TRUE;

        st = cpssExMxBrgEgressFilteringForL3Enable(dev, en);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, en);

        en = GT_FALSE;

        st = cpssExMxBrgEgressFilteringForL3Enable(dev, en);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, en);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    en = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgEgressFilteringForL3Enable(dev, en);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* en == GT_FALSE */

    st = cpssExMxBrgEgressFilteringForL3Enable(dev, en);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cppExMxBrgEgrFlt suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxBrgEgrFlt)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgMcUplinkFilterSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgPortEgrFltUnkEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgUnkUcEgressFilterCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgUnregMcEgressFilterCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgCpuBcFilteringEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanEgressFilteringEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgEgressFilteringForL3Enable)
UTF_SUIT_END_TESTS_MAC(cpssExMxBrgEgrFlt)
