/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxSalBrgGenUT.c
*
* DESCRIPTION:
*       Unit tests for CPSS Dx-Sal Bridge Generic APIs.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/dxSal/bridge/cpssDxSalBrgGen.h>

#include <cpss/generic/cos/cpssCosTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* From cpssDxSalBrgGen.c code!!!!           */
/* maximum value of rate limit               */
/* Be careful to keep them synchronized!     */
#define MAX_RATE_LIMIT_VALUE_CNS        0xffff

/* Invalid enum */
#define BRG_GEN_INVALID_ENUM_CNS        0x5AAAAAA5

/* Default valid value for physical port id */
#define BRG_GEN_VALID_PHY_PORT_CNS      0

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgGenBcMcRateLimitSet
(
    IN GT_U8        dev,
    IN GT_U32       rateValue,
    IN GT_U32       window
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgGenBcMcRateLimitSet)
{
/*
ITERATE_DEVICES 
1.1. Call function with rateValue [1000], window [100]. Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.
1.2. Check out-of-range rateValue. For Salsa devices call with rateValue[MAX_RATE_LIMIT_VALUE_CNS+1], window [100]. Expected: non GT_OK.
*/

    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_U32                      rateValue;
    GT_U32                      window;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with rateValue [1000], window [100]. Expected:*/
        /* GT_OK for Salsa devices and GT_BAD_PARAM for others.             */

        rateValue = 1000;
        window = 100;

        st = cpssDxSalBrgGenBcMcRateLimitSet(dev, rateValue, window);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "Salsa device: %d, %d, %d",
                                     dev, rateValue, window);

        /* 1.2. Check out-of-range rateValue. For Salsa devices call with rateValue           */
        /* [MAX_RATE_LIMIT_VALUE_CNS+1], window [100]. Expected: non GT_OK. */

        rateValue = MAX_RATE_LIMIT_VALUE_CNS+1;

        st = cpssDxSalBrgGenBcMcRateLimitSet(dev, rateValue, window);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, 
            "Salsa device: %d, rateValue=%d", dev, rateValue);

        rateValue = 1000;
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */
    rateValue = 1000;
    window = 100;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgGenBcMcRateLimitSet(dev, rateValue, window);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgGenBcMcRateLimitSet(dev, rateValue, window);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgGenBcRateLimitGranularitySet
(
    IN GT_U8                                    dev,
    IN CPSS_DXSAL_BRG_GEN_RL_GRANULARITY_ENT    granularity
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgGenBcRateLimitGranularitySet)
{
/*
ITERATE_DEVICES 
1.1. Call function with granularity [CPSS_DXSAL_BRG_GEN_RL_GRANULARITY_64K_PCKT_E / CPSS_DXSAL_BRG_GEN_RL_GRANULARITY_1_PCKT_E]. Expected: GT_OK for Salsa devices, GT_BAD_PARAM for others.
1.2. Check out-of-range enum. For Salsa devices call with granularity [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/

    GT_STATUS                                   st = GT_OK;
    GT_U8                                       dev;
    CPSS_DXSAL_BRG_GEN_RL_GRANULARITY_ENT       granularity;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with granularity 
           [CPSS_DXSAL_BRG_GEN_RL_GRANULARITY_64K_PCKT_E /
            CPSS_DXSAL_BRG_GEN_RL_GRANULARITY_1_PCKT_E]. Expected: GT_OK
           for Salsa devices, GT_BAD_PARAM for others. */

        granularity = CPSS_DXSAL_BRG_GEN_RL_GRANULARITY_64K_PCKT_E;

        st = cpssDxSalBrgGenBcRateLimitGranularitySet(dev, granularity);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, granularity);

        granularity = CPSS_DXSAL_BRG_GEN_RL_GRANULARITY_1_PCKT_E;

        st = cpssDxSalBrgGenBcRateLimitGranularitySet(dev, granularity);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, granularity);

        /* 1.2. Check out-of-range enum. For Salsa devices 
           call with granularity [0x5AAAAAA5].
           Expected: GT_BAD_PARAM.  */

        granularity = BRG_GEN_INVALID_ENUM_CNS;
        st = cpssDxSalBrgGenBcRateLimitGranularitySet(dev, granularity);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, 
            "Salsa device: %d, %d", dev, granularity);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    granularity = CPSS_DXSAL_BRG_GEN_RL_GRANULARITY_64K_PCKT_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgGenBcRateLimitGranularitySet(dev, granularity);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgGenBcRateLimitGranularitySet(dev, granularity);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgGenBcLimitEnable
(
    IN GT_U8        dev,
    IN GT_U8        port,
    IN GT_BOOL      enable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgGenBcLimitEnable)
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

            st = cpssDxSalBrgGenBcLimitEnable(dev, portNum, enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                              "Salsa device: %d, %d, %d",
                              dev, portNum, enable);

            enable = GT_TRUE;

            st = cpssDxSalBrgGenBcLimitEnable(dev, portNum, enable);
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
            st = cpssDxSalBrgGenBcLimitEnable(dev, portNum, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portNum);
        }

        /* 1.3. Call with out of range for Physical portNum.                       */
        /* Expected: GT_BAD_PARAM.                                                */

        portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalBrgGenBcLimitEnable(dev, portNum, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portNum);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        portNum = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalBrgGenBcLimitEnable(dev, portNum, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portNum);
    }

    portNum = BRG_GEN_VALID_PHY_PORT_CNS;
    enable = GT_FALSE;

    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices (dev). */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device (dev) and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */

        st = cpssDxSalBrgGenBcLimitEnable(dev, portNum, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id (dev).   */
    /* Expected: GT_BAD_PARAM.                                  */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* enable == GT_FALSE   */
    /* portNum == 0         */

    st = cpssDxSalBrgGenBcLimitEnable(dev, portNum, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgGenMcRateLimitEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgGenMcRateLimitEnable)
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

        st = cpssDxSalBrgGenMcRateLimitEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, enable);

        /* Call function with [enable==GT_TRUE] */
        enable = GT_TRUE;

        st = cpssDxSalBrgGenMcRateLimitEnable(dev, enable);
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
        st = cpssDxSalBrgGenMcRateLimitEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* enable == GT_TRUE */

    st = cpssDxSalBrgGenMcRateLimitEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgGenIgmpSnoopEnable
(
    IN GT_U8        dev,
    IN GT_U8        port,
    IN GT_BOOL      status
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgGenIgmpSnoopEnable)
{
/*
ITERATE_DEVICES_PHY_PORTS 
1.1.1. Call function for each port with status [GT_TRUE / GT_FALSE]. Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.
*/
    GT_STATUS                   st = GT_OK;

    GT_BOOL                     status;
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
            /* 1.1.1. Call function for each port with status [GT_TRUE /    */
            /* GT_FALSE]. Expected: GT_OK for Salsa devices and             */
            /* GT_BAD_PARAM for others.                                     */

            status = GT_TRUE;

            st = cpssDxSalBrgGenIgmpSnoopEnable(dev, portNum, status);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                              "Salsa device: %d, %d, %d",
                              dev, portNum, status);

            status = GT_FALSE;

            st = cpssDxSalBrgGenIgmpSnoopEnable(dev, portNum, status);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                              "Salsa device: %d, %d, %d",
                              dev, portNum, status);
        }

        /* Only for Salsa test non-active/out-of-range PhyPort */
        status = GT_FALSE;

        st = prvUtfNextPhyPortReset(&portNum, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over all non active Physical portNums and active device id (dev). */
        while (GT_OK == prvUtfNextPhyPortGet(&portNum, GT_FALSE))
        {
            /* 1.2.1. <Call function for non active portNum and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                                 */
            st = cpssDxSalBrgGenIgmpSnoopEnable(dev, portNum, status);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portNum);
        }

        /* 1.3. Call with out of range for Physical portNum.                       */
        /* Expected: GT_BAD_PARAM.                                                */

        portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalBrgGenIgmpSnoopEnable(dev, portNum, status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portNum);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        portNum = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalBrgGenIgmpSnoopEnable(dev, portNum, status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portNum);
    }

    portNum = BRG_GEN_VALID_PHY_PORT_CNS;
    status = GT_FALSE;

    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices (dev). */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device (dev) and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */

        st = cpssDxSalBrgGenIgmpSnoopEnable(dev, portNum, status);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id (dev).   */
    /* Expected: GT_BAD_PARAM.                                  */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* status == GT_FALSE   */
    /* portNum == 0         */

    st = cpssDxSalBrgGenIgmpSnoopEnable(dev, portNum, status);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgGenDropIpMcEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  state
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgGenDropIpMcEnable)
{
/*
ITERATE_DEVICES 
1.1. Call function with state [GT_TRUE / GT_FALSE]. Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.
*/

    GT_STATUS                   st = GT_OK;

    GT_U8                       dev;
    GT_BOOL                     state;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with state [GT_TRUE and GT_FALSE].            */
        /* Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.   */

        state = GT_TRUE;

        st = cpssDxSalBrgGenDropIpMcEnable(dev, state);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, state);

        state = GT_FALSE;

        st = cpssDxSalBrgGenDropIpMcEnable(dev, state);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, state);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    state = GT_TRUE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgGenDropIpMcEnable(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* state == GT_TRUE */

    st = cpssDxSalBrgGenDropIpMcEnable(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgGenDropNonIpMcEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  state
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgGenDropNonIpMcEnable)
{
/*
ITERATE_DEVICES 
1.1. Call function with state [GT_TRUE / GT_FALSE]. Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.
*/

    GT_STATUS                   st = GT_OK;

    GT_U8                       dev;
    GT_BOOL                     state;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with state [GT_TRUE and GT_FALSE].            */
        /* Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.   */

        state = GT_TRUE;

        st = cpssDxSalBrgGenDropNonIpMcEnable(dev, state);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, state);

        state = GT_FALSE;

        st = cpssDxSalBrgGenDropNonIpMcEnable(dev, state);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, state);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    state = GT_TRUE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgGenDropNonIpMcEnable(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* state == GT_TRUE */

    st = cpssDxSalBrgGenDropNonIpMcEnable(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgGenDropInvalidSaEnable(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgGenDropInvalidSaEnable)
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
        /* 1.1. Call function with enable [GT_TRUE and GT_FALSE].            */
        /* Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.   */

        enable = GT_TRUE;

        st = cpssDxSalBrgGenDropInvalidSaEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, enable);

        enable = GT_FALSE;

        st = cpssDxSalBrgGenDropInvalidSaEnable(dev, enable);
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
        st = cpssDxSalBrgGenDropInvalidSaEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* enable == GT_TRUE */

    st = cpssDxSalBrgGenDropInvalidSaEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgGenArpIgmpTrapTcSet
(
    IN GT_U8        dev,
    IN GT_U8        trClass
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgGenArpIgmpTrapTcSet)
{
/*
ITERATE_DEVICES 
1.1. Call with trClass [2]. Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.
1.2. For Salsa devices call function with out of bound value for trClass [CPSS_4_TC_RANGE_CNS]. Expected: GT_BAD_PARAM.
*/

    GT_STATUS                   st = GT_OK;

    GT_U8                       dev;
    GT_U8                       trClass;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with trClass [2]. Expected: GT_OK for Salsa devices    */
        /* and GT_BAD_PARAM for others.                                     */

        trClass = 2;

        st = cpssDxSalBrgGenArpIgmpTrapTcSet(dev, trClass);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, trClass);

        /* 1.2. For Salsa devices call function with out of bound value for */
        /* trClass [CPSS_4_TC_RANGE_CNS]. Expected: GT_BAD_PARAM.  */

        trClass = CPSS_4_TC_RANGE_CNS;

        st = cpssDxSalBrgGenArpIgmpTrapTcSet(dev, trClass);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "Salsa device: %d, %d",
                                     dev, trClass);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    trClass = 2;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgGenArpIgmpTrapTcSet(dev, trClass);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* trClass = 2 */

    st = cpssDxSalBrgGenArpIgmpTrapTcSet(dev, trClass);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgGenControlPktTcSet
(
    IN GT_U8        dev,
    IN GT_U8        trClass
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgGenControlPktTcSet)
{
/*
ITERATE_DEVICES 
1.1. Call with trClass [1]. Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.
1.2. For Salsa devices call function with out of bound value for trClass [CPSS_4_TC_RANGE_CNS]. Expected: GT_BAD_PARAM.
*/

    GT_STATUS                   st = GT_OK;

    GT_U8                       dev;
    GT_U8                       trClass;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with trClass [1]. Expected: GT_OK for Salsa devices    */
        /* and GT_BAD_PARAM for others.                                     */

        trClass = 1;

        st = cpssDxSalBrgGenControlPktTcSet(dev, trClass);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, trClass);

        /* 1.2. For Salsa devices call function with out of bound value for */
        /* trClass [CPSS_4_TC_RANGE_CNS]. Expected: GT_BAD_PARAM.  */

        trClass = CPSS_4_TC_RANGE_CNS;

        st = cpssDxSalBrgGenControlPktTcSet(dev, trClass);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "Salsa device: %d, %d",
                                     dev, trClass);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    trClass = 1;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgGenControlPktTcSet(dev, trClass);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* trClass = 1 */

    st = cpssDxSalBrgGenControlPktTcSet(dev, trClass);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgGenUnkRateLimitPpEnable
(
    IN GT_U8        dev,
    IN GT_U32       rateValue,
    IN GT_U32       window
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgGenUnkRateLimitPpEnable)
{
/*
ITERATE_DEVICES 
1.1. Call function with rateValue [1000], window [100]. Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.
1.2. Check out-of-range rateValue. For Salsa devices call with rateValue[MAX_RATE_LIMIT_VALUE_CNS+1], window [100]. Expected: non GT_OK.
*/

    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_U32                      rateValue;
    GT_U32                      window;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with rateValue [1000], window [100]. Expected:*/
        /* GT_OK for Salsa devices and GT_BAD_PARAM for others.             */

        rateValue = 1000;
        window = 100;

        st = cpssDxSalBrgGenUnkRateLimitPpEnable(dev, rateValue, window);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "Salsa device: %d, %d, %d",
                                     dev, rateValue, window);

        /* 1.2. Check out-of-range rateValue. For Salsa devices call with rateValue           */
        /* [MAX_RATE_LIMIT_VALUE_CNS+1], window [100]. Expected: non GT_OK. */

        rateValue = MAX_RATE_LIMIT_VALUE_CNS+1;

        st = cpssDxSalBrgGenUnkRateLimitPpEnable(dev, rateValue, window);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, 
            "Salsa device: %d, rateValue=%d", dev, rateValue);

        rateValue = 1000;
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */
    rateValue = 1000;
    window = 100;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgGenUnkRateLimitPpEnable(dev, rateValue, window);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgGenUnkRateLimitPpEnable(dev, rateValue, window);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgGenUnkRateLimitPpDisable
(
    IN GT_U8    dev
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgGenUnkRateLimitPpDisable)
{
/*
ITERATE_DEVICES 
1.1. Call function. Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.
*/

    GT_STATUS                   st = GT_OK;

    GT_U8                       dev;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function. Expected: GT_OK for Salsa devices    */
        /* and GT_BAD_PARAM for others.                             */

        st = cpssDxSalBrgGenUnkRateLimitPpDisable(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "Salsa device: %d",
                                     dev);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgGenUnkRateLimitPpDisable(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgGenUnkRateLimitPpDisable(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssDxSalBrgGen suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxSalBrgGen)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgGenBcMcRateLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgGenBcRateLimitGranularitySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgGenBcLimitEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgGenMcRateLimitEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgGenIgmpSnoopEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgGenDropIpMcEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgGenDropNonIpMcEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgGenDropInvalidSaEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgGenArpIgmpTrapTcSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgGenControlPktTcSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgGenUnkRateLimitPpEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgGenUnkRateLimitPpDisable)
UTF_SUIT_END_TESTS_MAC(cpssDxSalBrgGen)
