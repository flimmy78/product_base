/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxSalBrgEgrFltUT.c
*
* DESCRIPTION:
*       Unit tests for Egress filtering facility DxSal cpss implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/dxSal/bridge/cpssDxSalBrgEgrFlt.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* Invalid enum */
#define BRG_EGR_INVALID_ENUM_CNS    0x5AAAAAA5

/* Default valid value for physical port id */
#define BRG_EGR_VALID_PHY_PORT_CNS  0

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgPortEgrFltUnkEnable
(
    IN GT_U8    dev,
    IN GT_U8    port,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgPortEgrFltUnkEnable)
{
/*
ITERATE_DEVICES_PHY_PORTS (dev, port)
1.1.1. Call function for each port with enable [GT_FALSE / GT_TRUE]. Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.
*/
    GT_STATUS                   st = GT_OK;

    GT_BOOL                     enable;
    GT_U8                       portNum;
    GT_U8                       dev;

    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices (dev). */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&portNum, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For each active device (dev) id go over all active Physical portNums. */
        while (GT_OK == prvUtfNextPhyPortGet(&portNum, GT_TRUE))
        {
            /* 1.1.1. Call function for each port with enable [GT_FALSE /   */
            /* GT_TRUE]. Expected: GT_OK for Salsa devices and              */
            /* GT_BAD_PARAM for others.                                     */

            enable = GT_FALSE;

            st = cpssDxSalBrgPortEgrFltUnkEnable(dev, portNum, enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                              "Salsa device: %d, %d, %d",
                              dev, portNum, enable);

            enable = GT_TRUE;

            st = cpssDxSalBrgPortEgrFltUnkEnable(dev, portNum, enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                              "Salsa device: %d, %d, %d",
                              dev, portNum, enable);
        }

        /* Only for Salsa test non-active/out-of-range PhyPort */
        enable = GT_FALSE;

        st = prvUtfNextPhyPortReset(&portNum, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over all non active Physical portNums and active device id (dev). */
        while (GT_OK == prvUtfNextPhyPortGet(&portNum, GT_FALSE))
        {
            /* 1.2.1. <Call function for non active portNum and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                                 */
            st = cpssDxSalBrgPortEgrFltUnkEnable(dev, portNum, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portNum);
        }

        /* 1.3. Call with out of range for Physical portNum.                       */
        /* Expected: GT_BAD_PARAM.                                                */

        portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        /* enable == GT_FALSE   */

        st = cpssDxSalBrgPortEgrFltUnkEnable(dev, portNum, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portNum);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        portNum = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalBrgPortEgrFltUnkEnable(dev, portNum, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portNum);
    }

    portNum = BRG_EGR_VALID_PHY_PORT_CNS;
    enable = GT_FALSE;

    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices (dev). */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device (dev) and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */

        st = cpssDxSalBrgPortEgrFltUnkEnable(dev, portNum, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id (dev).   */
    /* Expected: GT_BAD_PARAM.                                  */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* enable == GT_FALSE   */
    /* portNum == 0         */

    st = cpssDxSalBrgPortEgrFltUnkEnable(dev, portNum, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgUnkUcEgressFilterCmdSet
(
    IN GT_U8              dev,
    IN CPSS_DXSAL_EGR_FILTER_CMD_ENT  egrCmd
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgUnkUcEgressFilterCmdSet)
{
/*
ITERATE_DEVICES 
1.1. Call function with egrCmd [CPSS_DXSAL_VLAN_FRWD_E / CPSS_DXSAL_NOT_CPU_FRWD_E]. Expected: GT_OK for Salsa devices, non GT_OK for others.
1.2. Check out-of-range enum. For Salsa devices call with egrCmd [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/

    GT_STATUS                       st = GT_OK;

    GT_U8                           dev;
    CPSS_DXSAL_EGR_FILTER_CMD_ENT   egrCmd;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with egrCmd [CPSS_DXSAL_VLAN_FRWD_E /         */
        /* CPSS_DXSAL_NOT_CPU_FRWD_E]. Expected: GT_OK for Salsa devices,   */
        /* non GT_OK for others.                                            */

        egrCmd = CPSS_DXSAL_VLAN_FRWD_E;

        st = cpssDxSalBrgUnkUcEgressFilterCmdSet(dev, egrCmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, egrCmd);

        egrCmd = CPSS_DXSAL_NOT_CPU_FRWD_E;

        st = cpssDxSalBrgUnkUcEgressFilterCmdSet(dev, egrCmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, egrCmd);

        /* 1.2. Check out-of-range enum. For Salsa devices call with    */
        /* egrCmd [0x5AAAAAA5]. Expected: GT_BAD_PARAM.                 */

        egrCmd = BRG_EGR_INVALID_ENUM_CNS;

        st = cpssDxSalBrgUnkUcEgressFilterCmdSet(dev, egrCmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, 
            "Salsa device: %d, %d", dev, egrCmd);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    egrCmd = CPSS_DXSAL_VLAN_FRWD_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgUnkUcEgressFilterCmdSet(dev, egrCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* egrCmd = CPSS_DXSAL_VLAN_FRWD_E */

    st = cpssDxSalBrgUnkUcEgressFilterCmdSet(dev, egrCmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgUnregMcEgressFilterCmdSet
(
    IN GT_U8              dev,
    IN CPSS_DXSAL_EGR_FILTER_CMD_ENT  egrCmd
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgUnregMcEgressFilterCmdSet)
{
/*
ITERATE_DEVICES 
1.1. Call function with egrCmd [CPSS_DXSAL_VLAN_FRWD_E / CPSS_DXSAL_NOT_CPU_FRWD_E]. Expected: GT_OK for Salsa devices, non GT_OK for others.
1.2. Check out-of-range enum. For Salsa devices call with egrCmd [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/

    GT_STATUS                       st = GT_OK;

    GT_U8                           dev;
    CPSS_DXSAL_EGR_FILTER_CMD_ENT   egrCmd;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with egrCmd [CPSS_DXSAL_VLAN_FRWD_E /         */
        /* CPSS_DXSAL_NOT_CPU_FRWD_E]. Expected: GT_OK for Salsa devices,   */
        /* non GT_OK for others.                                            */

        egrCmd = CPSS_DXSAL_VLAN_FRWD_E;

        st = cpssDxSalBrgUnregMcEgressFilterCmdSet(dev, egrCmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, egrCmd);

        egrCmd = CPSS_DXSAL_NOT_CPU_FRWD_E;

        st = cpssDxSalBrgUnregMcEgressFilterCmdSet(dev, egrCmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, egrCmd);

        /* 1.2. Check out-of-range enum. For Salsa devices call with    */
        /* egrCmd [0x5AAAAAA5]. Expected: GT_BAD_PARAM.                 */

        egrCmd = BRG_EGR_INVALID_ENUM_CNS;

        st = cpssDxSalBrgUnregMcEgressFilterCmdSet(dev, egrCmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, 
            "Salsa device: %d, %d", dev, egrCmd);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    egrCmd = CPSS_DXSAL_VLAN_FRWD_E;

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgUnregMcEgressFilterCmdSet(dev, egrCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* egrCmd = CPSS_DXSAL_VLAN_FRWD_E */

    st = cpssDxSalBrgUnregMcEgressFilterCmdSet(dev, egrCmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgCpuBcFilteringEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgCpuBcFilteringEnable)
{
/*
ITERATE_DEVICES 
1.1. Call function with enable [GT_TRUE / GT_FALSE]. Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.
*/

    GT_STATUS                   st = GT_OK;

    GT_U8                       dev;
    GT_BOOL                     enable;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with enable [GT_TRUE and GT_FALSE].           */
        /* Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.   */

        enable = GT_TRUE;

        st = cpssDxSalBrgCpuBcFilteringEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, enable);

        enable = GT_FALSE;

        st = cpssDxSalBrgCpuBcFilteringEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, enable);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    enable = GT_TRUE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgCpuBcFilteringEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* enable == GT_TRUE */

    st = cpssDxSalBrgCpuBcFilteringEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgVlanEgressFilteringEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgVlanEgressFilteringEnable)
{
/*
ITERATE_DEVICES 
1.1. Call function with enable [GT_FALSE / GT_TRUE]. Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.
*/

    GT_STATUS                   st = GT_OK;

    GT_U8                       dev;
    GT_BOOL                     enable;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with enable [GT_FALSE and GT_TRUE].           */
        /* Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.   */

        /* Call function with [enable==GT_FALSE] */
        enable = GT_FALSE;

        st = cpssDxSalBrgVlanEgressFilteringEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, enable);

        /* Call function with [enable==GT_TRUE] */
        enable = GT_TRUE;

        st = cpssDxSalBrgVlanEgressFilteringEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, enable);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    enable = GT_TRUE;

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgVlanEgressFilteringEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* enable == GT_TRUE */

    st = cpssDxSalBrgVlanEgressFilteringEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgCpuArpBcEgressFilteringEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgCpuArpBcEgressFilteringEnable)
{
/*
ITERATE_DEVICES 
1.1. Call function with enable [GT_TRUE / GT_FALSE]. Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.
*/

    GT_STATUS                   st = GT_OK;

    GT_U8                       dev;
    GT_BOOL                     enable;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with enable [GT_TRUE and GT_FALSE].           */
        /* Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.   */

        enable = GT_TRUE;

        st = cpssDxSalBrgCpuArpBcEgressFilteringEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, enable);

        enable = GT_FALSE;

        st = cpssDxSalBrgCpuArpBcEgressFilteringEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, enable);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    enable = GT_TRUE;

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgCpuArpBcEgressFilteringEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* enable == GT_TRUE */

    st = cpssDxSalBrgCpuArpBcEgressFilteringEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssDxSalBrgEgrFlt suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxSalBrgEgrFlt)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgPortEgrFltUnkEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgUnkUcEgressFilterCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgUnregMcEgressFilterCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgCpuBcFilteringEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgVlanEgressFilteringEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgCpuArpBcEgressFilteringEnable)
UTF_SUIT_END_TESTS_MAC(cpssDxSalBrgEgrFlt)
