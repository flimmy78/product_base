/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxIpCtrlUT.c
*
* DESCRIPTION:
*       Unit Tests for the CPSS EXMX Ip HW control registers APIs
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/exMx/exMxGen/ip/cpssExMxIpCtrl.h>

#include <cpss/generic/cos/cpssCosTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>


/* Invalid enumeration value used for testing */
#define IP_CTRL_INVALID_ENUM_CNS       0x5AAAAAA5

/*  Family sets constants */
#define UTF_TWISTD_FAMILY_SET_CNS                  UTF_CPSS_PP_FAMILY_TWISTD_CNS

#define UTF_TWISTD_TIGER_FAMILY_SET_CNS            (UTF_CPSS_PP_FAMILY_TWISTD_CNS | \
                                                   UTF_CPSS_PP_FAMILY_TIGER_CNS)

#define UTF_TWISTC_TWISTD_SAMBA_FAMILY_SET_CNS     (UTF_CPSS_PP_FAMILY_TWISTC_CNS | \
                                                   UTF_CPSS_PP_FAMILY_TWISTD_CNS |  \
                                                   UTF_CPSS_PP_FAMILY_SAMBA_CNS)

#define UTF_TWISTD_SAMBA_TIGER_FAMILY_SET_CNS      (UTF_CPSS_PP_FAMILY_TWISTD_CNS | \
                                                   UTF_CPSS_PP_FAMILY_SAMBA_CNS |  \
                                                   UTF_CPSS_PP_FAMILY_TIGER_CNS)


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpCtrlAgeRefreshEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpCtrlAgeRefreshEnable)
{
/*
ITERATE_DEVICES
1.1. Call function with enable [GT_FALSE and GT_TRUE]. Expected: GT_OK.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_BOOL                     enable;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with enable [GT_FALSE and GT_TRUE].   */
        /* Expected: GT_OK.                                         */
        enable = GT_FALSE;

        st = cpssExMxIpCtrlAgeRefreshEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_TRUE;

        st = cpssExMxIpCtrlAgeRefreshEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    enable = GT_TRUE;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxIpCtrlAgeRefreshEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpCtrlAgeRefreshEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpFlowRedirectRouteEntriesMSBBaseAddrSet
(
    IN GT_U8   devNum,
    IN GT_U32  flowRouteEntBaseAddr
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpFlowRedirectRouteEntriesMSBBaseAddrSet)
{
/*
ITERATE_DEVICES
1.1. Call function with flowRouteEntBaseAddr  [0 / 1000 / 0xFFFFFFFF]. Expected: GT_OK for TWIST, TWIST-D, SAMBA/RUMBA devices and GT_BAD_PARAM for others.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    IN GT_U32                   flowRouteEntBaseAddr;

    st = prvUtfNextDeviceReset(&dev, UTF_TWISTC_TWISTD_SAMBA_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with flowRouteEntBaseAddr [0/1000/0xFFFFFFFF].*/
        /* Expected: GT_OK for TWIST, TWIST-D, SAMBA/RUMBA devices          */
        /* and GT_BAD_PARAM for others.                                     */

        flowRouteEntBaseAddr = 0;
        st = cpssExMxIpFlowRedirectRouteEntriesMSBBaseAddrSet(dev,
                                                   flowRouteEntBaseAddr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
             "Non-Tiger (TWIST, TWIST-D, SAMBA/RUMBA) device: %d, %d",
                                     dev, flowRouteEntBaseAddr);

        flowRouteEntBaseAddr = 1000;
        st = cpssExMxIpFlowRedirectRouteEntriesMSBBaseAddrSet(dev,
                                                   flowRouteEntBaseAddr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
             "Non-Tiger (TWIST, TWIST-D, SAMBA/RUMBA) device: %d, %d",
                                     dev, flowRouteEntBaseAddr);

        flowRouteEntBaseAddr = 0xFFFFFFFF;
        st = cpssExMxIpFlowRedirectRouteEntriesMSBBaseAddrSet(dev,
                                                   flowRouteEntBaseAddr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
             "Non-Tiger (TWIST, TWIST-D, SAMBA/RUMBA) device: %d, %d",
                                     dev, flowRouteEntBaseAddr);
    }
    /* Set valid flowRouteEntBaseAddr */
    flowRouteEntBaseAddr = 1000;

    st = prvUtfNextDeviceReset(&dev, UTF_TWISTC_TWISTD_SAMBA_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxIpFlowRedirectRouteEntriesMSBBaseAddrSet(dev,
                                                   flowRouteEntBaseAddr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpFlowRedirectRouteEntriesMSBBaseAddrSet(dev,
                                               flowRouteEntBaseAddr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpMLLEntriesBaseAddrSet
(
    IN GT_U8   devNum,
    IN GT_U32  mllBaseAddr
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpMLLEntriesBaseAddrSet)
{
/*
ITERATE_DEVICES
1.1. Call function with mllBaseAddr  [0 / 1000]. Expected: GT_OK for TWIST, TWIST-D, SAMBA/RUMBA devices and GT_BAD_PARAM for others.
1.2. Call function with mllBaseAddr  [0x400000]. Expected: NOT GT_OK.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    IN GT_U32                   mllBaseAddr;

    st = prvUtfNextDeviceReset(&dev, UTF_TWISTC_TWISTD_SAMBA_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with mllBaseAddr [0/1000/0xFFFFFFFF].   */
        /* Expected: GT_OK for TWIST, TWIST-D, SAMBA/RUMBA devices          */
        /* and GT_BAD_PARAM for others.                                     */

        mllBaseAddr = 0;
        st = cpssExMxIpMLLEntriesBaseAddrSet(dev, mllBaseAddr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
             "Non-Tiger (TWIST, TWIST-D, SAMBA/RUMBA) device: %d, %d",
                                     dev, mllBaseAddr);

        mllBaseAddr = 1000;
        st = cpssExMxIpMLLEntriesBaseAddrSet(dev, mllBaseAddr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
             "Non-Tiger (TWIST, TWIST-D, SAMBA/RUMBA) device: %d, %d",
                                     dev, mllBaseAddr);

        /* 1.2. Call function with mllBaseAddr  [0x400000]. Expected: NOT GT_OK. */
        mllBaseAddr = 0x400000;
        st = cpssExMxIpMLLEntriesBaseAddrSet(dev, mllBaseAddr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
             "Non-Tiger (TWIST, TWIST-D, SAMBA/RUMBA) device: %d, %d",
                                     dev, mllBaseAddr);
    }
    /* Set valid mllBaseAddr */
    mllBaseAddr = 1000;

    st = prvUtfNextDeviceReset(&dev, UTF_TWISTC_TWISTD_SAMBA_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxIpMLLEntriesBaseAddrSet(dev, mllBaseAddr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpMLLEntriesBaseAddrSet(dev, mllBaseAddr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpVrBaseAddrSet
(
    IN GT_U8                              devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT         protocolStack,
    IN CPSS_EXMX_IP_VR_BASE_ADDR_TYPE_ENT baseAddrType,
    IN GT_U32                             vrBaseAddr
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpVrBaseAddrSet)
{
/*
ITERATE_DEVICES
1.1. Call function with protocolStack [CPSS_IP_PROTOCOL_IPV4_E], baseAddrType [CPSS_EXMX_IP_VR_UC_BASE_ADDR_TYPE_E/ CPSS_EXMX_IP_VR_MC_BASE_ADDR_TYPE_E], vrBaseAddr [0x01000000]. Expected: GT_OK for TIGER devices (since UC or MC baseAddrType supported only by this device) and GT_BAD_PARAM for others.
1.2. Call function with protocolStack [CPSS_IP_PROTOCOL_IPV4_E], baseAddrType [CPSS_EXMX_IP_VR_UC_MC_BASE_ADDR_TYPE_E], vrBaseAddr [0x01000000]. Expected: GT_OK for TWIST, TWIST-D, SAMBA devices (since UC & MC baseAddrType supported only by this devices) and GT_BAD_PARAM for others.
1.3. For Tiger devices call function with protocolStack [CPSS_IP_PROTOCOL_IPV6_E], baseAddrType [CPSS_EXMX_IP_VR_UC_BASE_ADDR_TYPE_E/ CPSS_EXMX_IP_VR_MC_BASE_ADDR_TYPE_E], vrBaseAddr [0x01000000]. Expected: GT_OK.
1.4. For Tiger devices call function with protocolStack [CPSS_IP_PROTOCOL_IPV4_E], baseAddrType [CPSS_EXMX_IP_VR_UC_BASE_ADDR_TYPE_E], vrBaseAddr [0x01000000]. Expected: GT_OK.
1.5. For TWIST, TWIST-D, SAMBA devices call function with protocolStack [CPSS_IP_PROTOCOL_IPV6_E], baseAddrType [CPSS_EXMX_IP_VR_UC_MC_BASE_ADDR_TYPE_E], vrBaseAddr [0x01000000]. Expected: NOT GT_OK (UC & MC baseAddrType supported only by this devices).
1.6. Check vrBaseAddr. For TWIST, TWIST-D, SAMBA devices call function with protocolStack [CPSS_IP_PROTOCOL_IPV4_E], baseAddrType [CPSS_EXMX_IP_VR_UC_MC_BASE_ADDR_TYPE_E], vrBaseAddr [0 / 0xFFFFFFFF / 1000]. Expected: GT_OK.
1.7. Call function with out of range protocolStack [0x5AAAAAA5] and baseAddrType [CPSS_EXMX_IP_VR_UC_BASE_ADDR_TYPE_E] for TIGER; baseAddrType [CPSS_EXMX_IP_VR_UC_MC_BASE_ADDR_TYPE_E] for SAMBA, TWIST, TWIST-D devices. Expected: GT_BAD_PARAM.
1.8. Call function with out of range baseAddrType [0x5AAAAAA5], other params same as in 1.1. Expected: GT_BAD_PARAM.
*/
    GT_STATUS                            st = GT_OK;
    GT_U8                                dev;
    CPSS_IP_PROTOCOL_STACK_ENT           protocolStack;
    CPSS_EXMX_IP_VR_BASE_ADDR_TYPE_ENT   baseAddrType;
    GT_U32                               vrBaseAddr;
    CPSS_PP_FAMILY_TYPE_ENT              devFamily;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d",
                                     dev);

        /* 1.1. Call function with protocolStack [CPSS_IP_PROTOCOL_IPV4_E], */
        /* baseAddrType [CPSS_EXMX_IP_VR_UC_BASE_ADDR_TYPE_E/               */
        /* CPSS_EXMX_IP_VR_MC_BASE_ADDR_TYPE_E], vrBaseAddr [0x01000000].   */
        /* Expected: GT_OK for TIGER devices (since UC or MC baseAddrType   */
        /* supported only by this device) and GT_BAD_PARAM for others.      */

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        baseAddrType = CPSS_EXMX_IP_VR_UC_BASE_ADDR_TYPE_E;
        vrBaseAddr = 0x01000000;

        st = cpssExMxIpVrBaseAddrSet(dev, protocolStack, baseAddrType, vrBaseAddr);
        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d, %d",
                              dev, protocolStack, baseAddrType, vrBaseAddr);
        }
        else
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
               "Non-Tiger device: %d, baseAddrType = %d", dev, baseAddrType);
        }

        baseAddrType = CPSS_EXMX_IP_VR_MC_BASE_ADDR_TYPE_E;

        st = cpssExMxIpVrBaseAddrSet(dev, protocolStack, baseAddrType, vrBaseAddr);
        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d, %d",
                              dev, protocolStack, baseAddrType, vrBaseAddr);
        }
        else
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
               "Non-Tiger device: %d, baseAddrType = %d", dev, baseAddrType);
        }

        /* 1.2. Call function with protocolStack [CPSS_IP_PROTOCOL_IPV4_E],  */
        /* baseAddrType [CPSS_EXMX_IP_VR_UC_MC_BASE_ADDR_TYPE_E], vrBaseAddr */
        /* [0x01000000]. Expected: GT_OK for TWIST, TWIST-D, SAMBA devices   */
        /* (since UC&MC baseAddrType supported only by this devices) and     */
        /* GT_BAD_PARAM for others.                                          */

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        baseAddrType = CPSS_EXMX_IP_VR_UC_MC_BASE_ADDR_TYPE_E;
        vrBaseAddr = 0x01000000;


        st = cpssExMxIpVrBaseAddrSet(dev, protocolStack, baseAddrType, vrBaseAddr);
        if (CPSS_PP_FAMILY_TIGER_E != devFamily)
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                              "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d",
                              dev, protocolStack, baseAddrType, vrBaseAddr);
        }
        else
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
               "Tiger device: %d, baseAddrType = %d", dev, baseAddrType);
        }

        /* 1.3. For Tiger devices call function with protocolStack
           [CPSS_IP_PROTOCOL_IPV6_E], baseAddrType
           [CPSS_EXMX_IP_VR_UC_BASE_ADDR_TYPE_E/
           CPSS_EXMX_IP_VR_MC_BASE_ADDR_TYPE_E], vrBaseAddr [0x01000000].
           Expected: GT_OK. */


        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
            baseAddrType = CPSS_EXMX_IP_VR_MC_BASE_ADDR_TYPE_E;
            vrBaseAddr = 0x01000000;

            st = cpssExMxIpVrBaseAddrSet(dev, protocolStack, baseAddrType, vrBaseAddr);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                              "Tiger device: %d, %d, %d, %d",
                              dev, protocolStack, baseAddrType, vrBaseAddr);

            baseAddrType = CPSS_EXMX_IP_VR_UC_BASE_ADDR_TYPE_E;

            st = cpssExMxIpVrBaseAddrSet(dev, protocolStack, baseAddrType, vrBaseAddr);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                              "Tiger device: %d, %d, %d, %d",
                              dev, protocolStack, baseAddrType, vrBaseAddr);

            /* 1.4. For Tiger devices call function with protocolStack
               [CPSS_IP_PROTOCOL_IPV4V6_E], baseAddrType
               [CPSS_EXMX_IP_VR_UC_BASE_ADDR_TYPE_E], vrBaseAddr
               [0x01000000]. Expected: GT_OK.   */

            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

            st = cpssExMxIpVrBaseAddrSet(dev, protocolStack, baseAddrType, vrBaseAddr);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                              "Tiger device: %d, %d, %d, %d",
                              dev, protocolStack, baseAddrType, vrBaseAddr);
        }

        /* 1.5. For TWIST, TWIST-D, SAMBA devices call function with
           protocolStack [CPSS_IP_PROTOCOL_IPV6_E], baseAddrType
           [CPSS_EXMX_IP_VR_UC_MC_BASE_ADDR_TYPE_E], vrBaseAddr [0x01000000].
           Expected: GT_OK (UC & MC baseAddrType supported only by this devices).   */

        if (CPSS_PP_FAMILY_TIGER_E != devFamily)
        {
            protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
            baseAddrType = CPSS_EXMX_IP_VR_UC_MC_BASE_ADDR_TYPE_E;
            vrBaseAddr = 0x01000000;

            st = cpssExMxIpVrBaseAddrSet(dev, protocolStack, baseAddrType, vrBaseAddr);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                              "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d",
                              dev, protocolStack, baseAddrType, vrBaseAddr);

            /* 1.6. Check vrBaseAddr. For TWIST, TWIST-D, SAMBA devices call
               function with protocolStack [CPSS_IP_PROTOCOL_IPV4_E], baseAddrType
               [CPSS_EXMX_IP_VR_UC_MC_BASE_ADDR_TYPE_E], vrBaseAddr
               [0 / 0xFFFFFFFF / 1000]. Expected: GT_OK.    */

            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            vrBaseAddr = 0;

            st = cpssExMxIpVrBaseAddrSet(dev, protocolStack, baseAddrType, vrBaseAddr);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                              "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d",
                              dev, protocolStack, baseAddrType, vrBaseAddr);
            vrBaseAddr = 0xFFFFFFFF;

            st = cpssExMxIpVrBaseAddrSet(dev, protocolStack, baseAddrType, vrBaseAddr);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                              "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d",
                              dev, protocolStack, baseAddrType, vrBaseAddr);
            vrBaseAddr = 1000;

            st = cpssExMxIpVrBaseAddrSet(dev, protocolStack, baseAddrType, vrBaseAddr);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                              "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d",
                              dev, protocolStack, baseAddrType, vrBaseAddr);
        }

        /* 1.7. Call function with out of range protocolStack [0x5AAAAAA5]  */
        /* and baseAddrType [CPSS_EXMX_IP_VR_UC_BASE_ADDR_TYPE_E] for TIGER;*/
        /* baseAddrType [CPSS_EXMX_IP_VR_UC_MC_BASE_ADDR_TYPE_E] for SAMBA, */
        /* TWIST, TWIST-D devices. Expected: GT_BAD_PARAM.                  */


        protocolStack = IP_CTRL_INVALID_ENUM_CNS;
        vrBaseAddr = 0x01000000;

        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            baseAddrType = CPSS_EXMX_IP_VR_UC_BASE_ADDR_TYPE_E;
        }
        else
        {
            baseAddrType = CPSS_EXMX_IP_VR_UC_MC_BASE_ADDR_TYPE_E;
        }
        st = cpssExMxIpVrBaseAddrSet(dev, protocolStack, baseAddrType, vrBaseAddr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, protocolStack);

        /* 1.8. Call function with out of range baseAddrType [0x5AAAAAA5],  */
        /* other params same as in 1.1. Expected: GT_BAD_PARAM.             */
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        baseAddrType = IP_CTRL_INVALID_ENUM_CNS;
        vrBaseAddr = 0x01000000;

        st = cpssExMxIpVrBaseAddrSet(dev, protocolStack, baseAddrType, vrBaseAddr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
           "%d, baseAddrType = %d", dev, baseAddrType);

    }

    /* Set valid params */
    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    baseAddrType = CPSS_EXMX_IP_VR_UC_BASE_ADDR_TYPE_E;
    vrBaseAddr = 0x01000000;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxIpVrBaseAddrSet(dev, protocolStack, baseAddrType, vrBaseAddr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpVrBaseAddrSet(dev, protocolStack, baseAddrType, vrBaseAddr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpCntSetGet
(
    IN  GT_U8                           dev,
    IN  CPSS_EXMX_IP_CNT_SET_ENT        cntSet,
    IN  CPSS_IP_PROTOCOL_STACK_ENT      protocolStack,
    OUT CPSS_EXMX_IP_COUNTERS_STC       *counters
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpCntSetGet)
{
/*
ITERATE_DEVICES
1.1. Call function with cntSet [CPSS_EXMX_IP_CNT_SET0_E / CPSS_EXMX_IP_CNT_SET2_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E] and non-NULL counters. Expected: GT_OK.
1.2. Call function with cntSet [CPSS_EXMX_IP_CNT_NO_SET_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E] and non-NULL counters. Expected: non GT_OK.
1.3. Check ipv6. Call function with cntSet [CPSS_EXMX_IP_CNT_SET0_E / CPSS_EXMX_IP_CNT_SET2_E], protocolStack [CPSS_IP_PROTOCOL_IPV6_E] and non-NULL counters. Expected: GT_OK.
1.4. Check dual protocol stack. Call function with cntSet [CPSS_EXMX_IP_CNT_SET2_E], protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] and non-NULL counters. Expected: NOT GT_OK.
1.5. Check for out of range enum. For all devices call function with out of range cntSet [0x5AAAAAA5], other params same as in 1.1. Expected: GT_BAD_PARAM.
1.6. Check for out of range enum. For all devices call function with out of range protocolStack [0x5AAAAAA5], cntSet [CPSS_EXMX_IP_CNT_SET0_E], non-NULL counters. Expected: GT_BAD_PARAM.
1.7. Check for NULL pointer. For all devices call function with cntSet [CPSS_EXMX_IP_CNT_SET0_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], counters [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    CPSS_EXMX_IP_CNT_SET_ENT    cntSet;
    CPSS_IP_PROTOCOL_STACK_ENT  protocolStack;
    CPSS_EXMX_IP_COUNTERS_STC   counters;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* initialize variable with zeros */
    cpssOsMemSet(&counters, 0, sizeof(counters));

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with cntSet [CPSS_EXMX_IP_CNT_SET0_E / CPSS_EXMX_IP_CNT_SET2_E],*/
        /* protocolStack [CPSS_IP_PROTOCOL_IPV4_E] and non-NULL     */
        /* counters. Expected: GT_OK.                               */
        cntSet = CPSS_EXMX_IP_CNT_SET0_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        st = cpssExMxIpCntSetGet(dev, cntSet, protocolStack, &counters);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntSet, protocolStack);

        cntSet = CPSS_EXMX_IP_CNT_SET2_E;

        st = cpssExMxIpCntSetGet(dev, cntSet, protocolStack, &counters);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntSet, protocolStack);

        /* 1.2. Call function with cntSet [CPSS_EXMX_IP_CNT_NO_SET_E],
           protocolStack [CPSS_IP_PROTOCOL_IPV4_E] and non-NULL counters.
           Expected: non GT_OK. */

        cntSet = CPSS_EXMX_IP_CNT_NO_SET_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        st = cpssExMxIpCntSetGet(dev, cntSet, protocolStack, &counters);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntSet);

        /* 1.3. Check ipv6. Call function with cntSet
           [CPSS_EXMX_IP_CNT_SET0_E / CPSS_EXMX_IP_CNT_SET2_E],
           protocolStack [CPSS_IP_PROTOCOL_IPV6_E] and non-NULL counters.
           Expected: GT_OK. */

        cntSet = CPSS_EXMX_IP_CNT_SET0_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssExMxIpCntSetGet(dev, cntSet, protocolStack, &counters);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntSet, protocolStack);

        cntSet = CPSS_EXMX_IP_CNT_SET2_E;

        st = cpssExMxIpCntSetGet(dev, cntSet, protocolStack, &counters);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntSet, protocolStack);

        /* 1.4. Check dual protocol stack. Call function with cntSet          */
        /* [CPSS_EXMX_IP_CNT_SET2_E], protocolStack                           */
        /* [CPSS_IP_PROTOCOL_IPV4V6_E] and non-NULL counters. Expected: NOT GT_OK.*/

        cntSet = CPSS_EXMX_IP_CNT_SET2_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssExMxIpCntSetGet(dev, cntSet, protocolStack, &counters);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntSet, protocolStack);

        /* 1.5. Check for out of range enum. For all devices call function  */
        /* with out of range cntSet [0x5AAAAAA5], other params same as      */
        /* in 1.1. Expected: GT_BAD_PARAM.                                  */
        cntSet = IP_CTRL_INVALID_ENUM_CNS;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        st = cpssExMxIpCntSetGet(dev, cntSet, protocolStack, &counters);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, cntSet, protocolStack);

        /* 1.6. Check for out of range enum. For all devices call function  */
        /* with out of range protocolStack [0x5AAAAAA5], cntSet             */
        /* [CPSS_EXMX_IP_CNT_SET0_E], non-NULL counters.                    */
        /* Expected: GT_BAD_PARAM.                                          */
        cntSet = CPSS_EXMX_IP_CNT_SET0_E;
        protocolStack = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxIpCntSetGet(dev, cntSet, protocolStack, &counters);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, cntSet, protocolStack);

        /* 1.7. Check for NULL pointer. For all devices call function with  */
        /* cntSet [CPSS_EXMX_IP_CNT_SET0_E], protocolStack                  */
        /* [CPSS_IP_PROTOCOL_IPV4_E], counters [NULL]. Expected: GT_BAD_PTR.*/

        cntSet = CPSS_EXMX_IP_CNT_SET0_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        st = cpssExMxIpCntSetGet(dev, cntSet, protocolStack, NULL);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, counters=NULL", dev);
    }

    /* Set valid params */
    cntSet = CPSS_EXMX_IP_CNT_SET0_E;
    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxIpCntSetGet(dev, cntSet, protocolStack, &counters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpCntSetGet(dev, cntSet, protocolStack, &counters);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpCntSetClear
(
    IN  GT_U8                      dev,
    IN  CPSS_EXMX_IP_CNT_SET_ENT   cntSet,
    IN  CPSS_IP_PROTOCOL_STACK_ENT protocolStack
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpCntSetClear)
{
/*
ITERATE_DEVICES
1.1. Call function with cntSet [CPSS_EXMX_IP_CNT_SET0_E / CPSS_EXMX_IP_CNT_SET2_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E]. Expected: GT_OK.
1.2. Call function with cntSet [CPSS_EXMX_IP_CNT_NO_SET_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E]. Expected: non GT_OK.
1.3. Check ipv6. Call function with cntSet [CPSS_EXMX_IP_CNT_SET0_E / CPSS_EXMX_IP_CNT_SET2_E], protocolStack [CPSS_IP_PROTOCOL_IPV6_E]. Expected: GT_OK.
1.4. Check dual protocol stack. Call function with cntSet [CPSS_EXMX_IP_CNT_SET2_E], protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E]. Expected: NOT GT_OK.
1.5. Check for out of range enum. For all devices call function with out of range cntSet [0x5AAAAAA5], other params same as in 1.1. Expected: GT_BAD_PARAM.
1.6. Check for out of range enum. For all devices call function with out of range protocolStack [0x5AAAAAA5], cntSet [CPSS_EXMX_IP_CNT_SET0_E]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    CPSS_EXMX_IP_CNT_SET_ENT    cntSet;
    CPSS_IP_PROTOCOL_STACK_ENT  protocolStack;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with cntSet [CPSS_EXMX_IP_CNT_SET0_E /         */
        /* CPSS_EXMX_IP_CNT_SET2_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E].*/
        /* Expected: GT_OK.   */
        cntSet = CPSS_EXMX_IP_CNT_SET0_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        st = cpssExMxIpCntSetClear(dev, cntSet, protocolStack);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntSet, protocolStack);

        cntSet = CPSS_EXMX_IP_CNT_SET2_E;

        st = cpssExMxIpCntSetClear(dev, cntSet, protocolStack);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntSet, protocolStack);

        /* 1.2. Call function with cntSet [CPSS_EXMX_IP_CNT_NO_SET_E],
           protocolStack [CPSS_IP_PROTOCOL_IPV4_E].
           Expected: non GT_OK. */

        cntSet = CPSS_EXMX_IP_CNT_NO_SET_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        st = cpssExMxIpCntSetClear(dev, cntSet, protocolStack);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntSet);

        /* 1.3. Check ipv6. Call function with cntSet
           [CPSS_EXMX_IP_CNT_SET0_E / CPSS_EXMX_IP_CNT_SET2_E],
           protocolStack [CPSS_IP_PROTOCOL_IPV6_E].
           Expected: GT_OK. */

        cntSet = CPSS_EXMX_IP_CNT_SET0_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssExMxIpCntSetClear(dev, cntSet, protocolStack);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntSet, protocolStack);

        cntSet = CPSS_EXMX_IP_CNT_SET2_E;

        st = cpssExMxIpCntSetClear(dev, cntSet, protocolStack);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntSet, protocolStack);

        /* 1.4. Check dual protocol stack. Call function with cntSet          */
        /* [CPSS_EXMX_IP_CNT_SET2_E], protocolStack                           */
        /* [CPSS_IP_PROTOCOL_IPV4V6_E]. Expected: NOT GT_OK.                  */

        cntSet = CPSS_EXMX_IP_CNT_SET2_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssExMxIpCntSetClear(dev, cntSet, protocolStack);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntSet, protocolStack);

        /* 1.5. Check for out of range enum. For all devices call function  */
        /* with out of range cntSet [0x5AAAAAA5], other params same as      */
        /* in 1.1. Expected: GT_BAD_PARAM.                                  */
        cntSet = IP_CTRL_INVALID_ENUM_CNS;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        st = cpssExMxIpCntSetClear(dev, cntSet, protocolStack);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, cntSet, protocolStack);

        /* 1.6. Check for out of range enum. For all devices call function  */
        /* with out of range protocolStack [0x5AAAAAA5], cntSet             */
        /* [CPSS_EXMX_IP_CNT_SET0_E].                                       */
        /* Expected: GT_BAD_PARAM.                                          */
        cntSet = CPSS_EXMX_IP_CNT_SET0_E;
        protocolStack = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxIpCntSetClear(dev, cntSet, protocolStack);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, cntSet, protocolStack);
    }

    /* Set valid params */
    cntSet = CPSS_EXMX_IP_CNT_SET0_E;
    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxIpCntSetClear(dev, cntSet, protocolStack);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpCntSetClear(dev, cntSet, protocolStack);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpCntGlblGet
(
    IN  GT_U8                           dev,
    IN  CPSS_IP_PROTOCOL_STACK_ENT protocolStack,
    OUT CPSS_EXMX_IP_GLOBAL_COUNTERS_STC *counters
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpCntGlblGet)
{
/*
ITERATE_DEVICES
1.1. Call function with protocolStack [CPSS_IP_PROTOCOL_IPV4_E / CPSS_IP_PROTOCOL_IPV6_E] and non-NULL counters. Expected: GT_OK.
1.2. Check dual protocol stack. Call function with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] and non-NULL counters. Expected: NOT GT_OK.
1.3. Check for out of range enum. For all devices call function with out of range protocolStack [0x5AAAAAA5] and non-NULL counters. Expected: GT_BAD_PARAM.
1.4. Check for NULL pointer. For all devices call function with protocolStack [CPSS_IP_PROTOCOL_IPV4_E], counters [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS                          st = GT_OK;
    GT_U8                              dev;
    CPSS_IP_PROTOCOL_STACK_ENT         protocolStack;
    CPSS_EXMX_IP_GLOBAL_COUNTERS_STC   counters;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* initialize variable with zeros */
    cpssOsMemSet(&counters, 0, sizeof(counters));

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /     */
        /* CPSS_IP_PROTOCOL_IPV6_E] and non-NULL                                */
        /* counters. Expected: GT_OK.                                           */

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        st = cpssExMxIpCntGlblGet(dev, protocolStack, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, protocolStack);

        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssExMxIpCntGlblGet(dev, protocolStack, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, protocolStack);

        /* 1.2. Check dual protocol stack. Call function with */
        /* protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E]          */
        /* and non-NULL counters. Expected: NOT GT_OK.        */
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssExMxIpCntGlblGet(dev, protocolStack, &counters);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, protocolStack);

        /* 1.3. Check for out of range enum. For all devices call function
           with out of range protocolStack [0x5AAAAAA5] and non-NULL counters.
           Expected: GT_BAD_PARAM.  */

        protocolStack = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxIpCntGlblGet(dev, protocolStack, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, protocolStack);

        /* 1.4. Check for NULL pointer. For all devices call function with
           protocolStack [CPSS_IP_PROTOCOL_IPV4_E], counters [NULL].
           Expected: GT_BAD_PTR.    */

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        st = cpssExMxIpCntGlblGet(dev, protocolStack, NULL);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, counters=NULL", dev);
    }

    /* Set valid params */
    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxIpCntGlblGet(dev, protocolStack, &counters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpCntGlblGet(dev, protocolStack, &counters);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpCtrlExceptionCmdSet
(
    IN GT_U8                            devNum,
    IN CPSS_EXMX_IP_EXCP_TYPE_ENT       exceptionType,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    IN CPSS_EXMX_IP_EXCP_CMD_ENT        exceptionCmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpCtrlExceptionCmdSet)
{
/*
ITERATE_DEVICES

TWIST, TWIST-D, SAMBA/RUMBA

CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E
1.1. For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E / CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_OK.
1.2. Check unsupported commands. For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E / CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E / CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E / CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E / CPSS_EXMX_IP_EXCP_CMD_ROUTE_E]. Expected: non GT_OK.
1.3. Check unsupported protocol. For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E], protocolStack [CPSS_IP_PROTOCOL_IPV6_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E]. Expected: non GT_OK.

CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_1_E
1.4. Check unsupported commands (all). For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_1_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E / CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E / CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E / CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E / CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E / CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E / CPSS_EXMX_IP_EXCP_CMD_ROUTE_E]. Expected: non GT_OK.

CPSS_EXMX_IP_EXCP_TYPE_OPTIONS_HBH_E
1.5. For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_OPTIONS_HBH_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_ROUTE_E / CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_OK.
1.6. Check unsupported commands. For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_OPTIONS_HBH_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E / CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E / CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E / CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E / CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E]. Expected: non GT_OK.
1.7. Check unsupported protocol. For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_OPTIONS_HBH_E], protocolStack [CPSS_IP_PROTOCOL_IPV6_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_ROUTE_E]. Expected: non GT_OK.

CPSS_EXMX_IP_EXCP_TYPE_HEADER_ERROR_E
1.8. For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_HEADER_ERROR_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E / CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_OK.
1.9. Check unsupported commands. For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_HEADER_ERROR_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E / CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E / CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E / CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E / CPSS_EXMX_IP_EXCP_CMD_ROUTE_E]. Expected: non GT_OK.
1.10. Check unsupported protocol. For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_HEADER_ERROR_E], protocolStack [CPSS_IP_PROTOCOL_IPV6_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: non GT_OK.

CPSS_EXMX_IP_EXCP_TYPE_BAD_CHECKSUM_E
1.11. For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_BAD_CHECKSUM_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E / CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_OK.
1.12. Check unsupported commands. For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_BAD_CHECKSUM_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E / CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E / CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E / CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E / CPSS_EXMX_IP_EXCP_CMD_ROUTE_E]. Expected: non GT_OK.
1.13. Check unsupported protocol. For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_BAD_CHECKSUM_E], protocolStack [CPSS_IP_PROTOCOL_IPV6_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: non GT_OK.

CPSS_EXMX_IP_EXCP_TYPE_FAIL_RPF_E
1.14. For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_FAIL_RPF_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E / CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_OK.
1.15. Check unsupported commands. For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_FAIL_RPF_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E / CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E / CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E / CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E / CPSS_EXMX_IP_EXCP_CMD_ROUTE_E]. Expected: non GT_OK.
1.16. Check unsupported protocol. For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_FAIL_RPF_E], protocolStack [CPSS_IP_PROTOCOL_IPV6_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: non GT_OK.

CPSS_EXMX_IP_EXCP_TYPE_ILLEGAL_ADDRESS_E
1.17. Check unsupported commands (all). For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_ILLEGAL_ADDRESS_E], other params same as in 1.4. Expected: non GT_OK.

CPSS_EXMX_IP_EXCP_TYPE_MTU_EXCEED_E
1.18. Check unsupported commands (all). For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_MTU_EXCEED_E], other params same as in 1.4. Expected: non GT_OK.

CPSS_EXMX_IP_EXCP_TYPE_SPECADDR_EXT_HEADER_E
1.19. Check unsupported commands (all). For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_SPECADDR_EXT_HEADER_E], other params same as in 1.4. Expected: non GT_OK.

1.20. Check for out-of-range enum exceptionType for non-Tiger devices. For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [0x5AAAAAA5], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_BAD_PARAM.
1.21. Check for out-of-range enum protocolStack for non-Tiger devices. For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E], protocolStack [0x5AAAAAA5], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_BAD_PARAM.
1.22. Check for out-of-range enum exceptionCmd for non-Tiger devices. For TWIST, TWIST-D, SAMBA/RUMBA devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd [0x5AAAAAA5]. Expected: GT_BAD_PARAM.

TIGER

CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E / CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_1_E / CPSS_EXMX_IP_EXCP_TYPE_HEADER_ERROR_E / CPSS_EXMX_IP_EXCP_TYPE_ILLEGAL_ADDRESS_E / CPSS_EXMX_IP_EXCP_TYPE_MTU_EXCEED_E
1.23. For Tiger devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E / CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_1_E / CPSS_EXMX_IP_EXCP_TYPE_HEADER_ERROR_E / CPSS_EXMX_IP_EXCP_TYPE_ILLEGAL_ADDRESS_E / CPSS_EXMX_IP_EXCP_TYPE_MTU_EXCEED_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E / CPSS_IP_PROTOCOL_IPV6_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_OK.
1.24. Check unsupported commands. For Tiger devices call function with exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_ROUTE_E], other params same as in 1.23. Expected: non GT_OK.

CPSS_EXMX_IP_EXCP_TYPE_OPTIONS_HBH_E
1.25. For Tiger devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_OPTIONS_HBH_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E / CPSS_IP_PROTOCOL_IPV6_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_OK.
1.26. Check unsupported commands. For Tiger devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_OPTIONS_HBH_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E / CPSS_IP_PROTOCOL_IPV6_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E]. Expected: non GT_OK.

CPSS_EXMX_IP_EXCP_TYPE_BAD_CHECKSUM_E/ CPSS_EXMX_IP_EXCP_TYPE_FAIL_RPF_E
1.27. Check unsupported commands. For Tiger devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_BAD_CHECKSUM_E / CPSS_EXMX_IP_EXCP_TYPE_FAIL_RPF_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: non GT_OK.

CPSS_EXMX_IP_EXCP_TYPE_SPECADDR_EXT_HEADER_E
1.28. For Tiger devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_SPECADDR_EXT_HEADER_E], protocolStack [CPSS_IP_PROTOCOL_IPV6_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_ROUTE_E / CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E / CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E]. Expected: GT_OK.
1.29. Check unsupported commands. For Tiger devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_SPECADDR_EXT_HEADER_E], protocolStack [CPSS_IP_PROTOCOL_IPV6_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E / CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E / CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E / CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E]. Expected: non GT_OK.
1.30. For Tiger devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_SPECADDR_EXT_HEADER_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E / CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E / CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E]. Expected: GT_OK.
1.31. Check unsupported commands. For Tiger devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_SPECADDR_EXT_HEADER_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_ROUTE_E / CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E / CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E / CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E]. Expected: non GT_OK.

1.32. Check for out-of-range enum. For Tiger devices call function with exceptionType [0x5AAAAAA5], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_BAD_PARAM.
1.33. Check for out-of-range enum. For Tiger devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E], protocolStack [0x5AAAAAA5], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_BAD_PARAM.
1.34. Check for out-of-range enum. For Tiger devices call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd [0x5AAAAAA5]. Expected: GT_BAD_PARAM.

ALL DEVICES

1.35. Check Ipv4v6 unsupported protocol. Call function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E], protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E], exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: NOT GT_OK.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    CPSS_EXMX_IP_EXCP_TYPE_ENT  exceptionType;
    CPSS_IP_PROTOCOL_STACK_ENT  protocolStack;
    CPSS_EXMX_IP_EXCP_CMD_ENT   exceptionCmd;
    CPSS_PP_FAMILY_TYPE_ENT     devFamily;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d",
                                     dev);

        if (CPSS_PP_FAMILY_TIGER_E != devFamily)
        {   /* ----------------- TWIST, TWIST-D, SAMBA/RUMBA ---------------*/

            /* 1.1. For TWIST, TWIST-D, SAMBA/RUMBA devices call function
               with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E],
               protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd
               [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E /
               CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_OK.    */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);


            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.2. Check unsupported commands. For TWIST, TWIST-D,
               SAMBA/RUMBA devices call function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E], protocolStack
               [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd
               [CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E /
                CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E /
                CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E /
                CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E /
                CPSS_EXMX_IP_EXCP_CMD_ROUTE_E]. Expected: non GT_OK.        */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);


            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E;
            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);


            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E;
            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);


            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E;
            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);


            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_E;
            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.3. Check unsupported protocol. For TWIST, TWIST-D, SAMBA/RUMBA
               devices call function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E],
               protocolStack [CPSS_IP_PROTOCOL_IPV6_E], exceptionCmd
               [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E]. Expected: non GT_OK.      */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.4. Check unsupported commands (all). For TWIST, TWIST-D,
               SAMBA/RUMBA devices call function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_1_E], protocolStack
               [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd 
               [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E /
                CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E /
                CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E /
                CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E /
                CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E /
                CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E /
                CPSS_EXMX_IP_EXCP_CMD_ROUTE_E]. Expected: non GT_OK.    */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_1_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);


            /* 1.5. For TWIST, TWIST-D, SAMBA/RUMBA devices call function with
               exceptionType [CPSS_EXMX_IP_EXCP_TYPE_OPTIONS_HBH_E],
               protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd
               [CPSS_EXMX_IP_EXCP_CMD_ROUTE_E /
               CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_OK.    */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_OPTIONS_HBH_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.6. Check unsupported commands. For TWIST, TWIST-D,
               SAMBA/RUMBA devices call function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_OPTIONS_HBH_E], protocolStack
               [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd
               [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E /
               CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E /
               CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E /
               CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E /
               CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E]. Expected: non GT_OK.     */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_OPTIONS_HBH_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.7. Check unsupported protocol. For TWIST, TWIST-D,
               SAMBA/RUMBA devices call function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_OPTIONS_HBH_E], protocolStack
               [CPSS_IP_PROTOCOL_IPV6_E],
               exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_ROUTE_E].
               Expected: non GT_OK.     */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_OPTIONS_HBH_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.8. For TWIST, TWIST-D, SAMBA/RUMBA devices call function
               with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_HEADER_ERROR_E],
               protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd
               [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E /
                CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_OK.   */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_HEADER_ERROR_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.9. Check unsupported commands. For TWIST, TWIST-D,
               SAMBA/RUMBA devices call function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_HEADER_ERROR_E], protocolStack
               [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd
               [CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E /
                CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E /
                CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E /
                CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E /
                CPSS_EXMX_IP_EXCP_CMD_ROUTE_E]. Expected: non GT_OK.    */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_HEADER_ERROR_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);


            /* 1.10. Check unsupported protocol. For TWIST, TWIST-D,
               SAMBA/RUMBA devices call function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_HEADER_ERROR_E], protocolStack
               [CPSS_IP_PROTOCOL_IPV6_E],
               exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E].
               Expected: non GT_OK.     */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_HEADER_ERROR_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.11. For TWIST, TWIST-D, SAMBA/RUMBA devices call function
               with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_BAD_CHECKSUM_E],
               protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd 
               [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E /
                CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_OK.   */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_BAD_CHECKSUM_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.12. Check unsupported commands. For TWIST, TWIST-D,
              SAMBA/RUMBA devices call function with exceptionType 
              [CPSS_EXMX_IP_EXCP_TYPE_BAD_CHECKSUM_E], protocolStack
              [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd
              [CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E /
               CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E /
               CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E /
               CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E /
               CPSS_EXMX_IP_EXCP_CMD_ROUTE_E]. Expected: non GT_OK. */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_BAD_CHECKSUM_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.13. Check unsupported protocol. For TWIST, TWIST-D,
               SAMBA/RUMBA devices call function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_BAD_CHECKSUM_E], protocolStack
               [CPSS_IP_PROTOCOL_IPV6_E],
               exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E].
               Expected: non GT_OK. */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_BAD_CHECKSUM_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.14. For TWIST, TWIST-D, SAMBA/RUMBA devices call function
               with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_FAIL_RPF_E],
               protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd
               [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E /
                CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_OK.   */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_FAIL_RPF_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.15. Check unsupported commands. For TWIST, TWIST-D,
               SAMBA/RUMBA devices call function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_FAIL_RPF_E], protocolStack
               [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd
               [CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E /
                CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E /
                CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E /
                CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E /
                CPSS_EXMX_IP_EXCP_CMD_ROUTE_E]. Expected: non GT_OK.    */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_FAIL_RPF_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.16. Check unsupported protocol. For TWIST, TWIST-D,
               SAMBA/RUMBA devices call function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_FAIL_RPF_E], protocolStack
               [CPSS_IP_PROTOCOL_IPV6_E],
               exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E].
               Expected: non GT_OK.     */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_FAIL_RPF_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.17. Check unsupported commands (all). For TWIST, TWIST-D,
               SAMBA/RUMBA devices call function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_ILLEGAL_ADDRESS_E], other params
               same as in 1.4. Expected: non GT_OK.     */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_ILLEGAL_ADDRESS_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.18. Check unsupported commands (all). For TWIST, TWIST-D,
               SAMBA/RUMBA devices call function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_MTU_EXCEED_E], other params same
               as in 1.4. Expected: non GT_OK.  */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_MTU_EXCEED_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.19. Check unsupported commands (all). For TWIST, TWIST-D,
               SAMBA/RUMBA devices call function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_SPECADDR_EXT_HEADER_E], other params same
               as in 1.4. Expected: non GT_OK.      */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_SPECADDR_EXT_HEADER_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.20. Check for out-of-range enum exceptionType for non-Tiger
               devices. For TWIST, TWIST-D, SAMBA/RUMBA devices call function
               with exceptionType [0x5AAAAAA5], protocolStack
               [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd
               [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_BAD_PARAM.*/

            exceptionType = IP_CTRL_INVALID_ENUM_CNS;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, exceptionType=%d",
                 dev, exceptionType);

            /* 1.21. Check for out-of-range enum protocolStack for non-Tiger
               devices. For TWIST, TWIST-D, SAMBA/RUMBA devices call
               function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E], protocolStack
               [0x5AAAAAA5], exceptionCmd 
               [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E].
               Expected: GT_BAD_PARAM.    */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E;
            protocolStack = IP_CTRL_INVALID_ENUM_CNS;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                 "TWIST, TWIST-D, SAMBA/RUMBA device: %d, protocolStack=%d",
                 dev, protocolStack);

            /* 1.22. Check for out-of-range enum exceptionCmd for non-Tiger
               devices. For TWIST, TWIST-D, SAMBA/RUMBA devices call function
               with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E],
               protocolStack [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd
               [0x5AAAAAA5]. Expected: GT_BAD_PARAM.    */

                exceptionType = CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E;
                protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
                exceptionCmd = IP_CTRL_INVALID_ENUM_CNS;

                st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                                   protocolStack, exceptionCmd);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                     "TWIST, TWIST-D, SAMBA/RUMBA device: %d, exceptionCmd=%d",
                     dev, exceptionCmd);
            /* ----------------- TWIST, TWIST-D, SAMBA/RUMBA ---end---------*/            
        }
        else
        {   /* --------------------------- TIGER ---------------------------*/

            /* 1.23. For Tiger devices call function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E /
                CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_1_E /
                CPSS_EXMX_IP_EXCP_TYPE_HEADER_ERROR_E /
                CPSS_EXMX_IP_EXCP_TYPE_ILLEGAL_ADDRESS_E /
                CPSS_EXMX_IP_EXCP_TYPE_MTU_EXCEED_E],
                protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                CPSS_IP_PROTOCOL_IPV6_E], exceptionCmd
                [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_OK.  */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                  "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_1_E;
            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                  "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_HEADER_ERROR_E;
            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                  "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_ILLEGAL_ADDRESS_E;
            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                  "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_MTU_EXCEED_E;
            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                  "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* Ipv6 test */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                  "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_1_E;
            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                  "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_HEADER_ERROR_E;
            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                  "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_ILLEGAL_ADDRESS_E;
            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                  "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_MTU_EXCEED_E;
            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                  "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.24. Check unsupported commands. For Tiger devices call
               function with exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_ROUTE_E],
               other params same as in 1.23. Expected: non GT_OK.   */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_1_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_HEADER_ERROR_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_ILLEGAL_ADDRESS_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_MTU_EXCEED_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.25. For Tiger devices call function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_OPTIONS_HBH_E], protocolStack
               [CPSS_IP_PROTOCOL_IPV4_E / CPSS_IP_PROTOCOL_IPV6_E],exceptionCmd
               [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_OK.   */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_OPTIONS_HBH_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                  "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            protocolStack = CPSS_IP_PROTOCOL_IPV6_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                  "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.26. Check unsupported commands. For Tiger devices call
               function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_OPTIONS_HBH_E], protocolStack
               [CPSS_IP_PROTOCOL_IPV4_E / CPSS_IP_PROTOCOL_IPV6_E],exceptionCmd
               [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E]. Expected: non GT_OK.    */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_OPTIONS_HBH_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            protocolStack = CPSS_IP_PROTOCOL_IPV6_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.27. Check unsupported commands. For Tiger devices call
               function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_BAD_CHECKSUM_E /
                CPSS_EXMX_IP_EXCP_TYPE_FAIL_RPF_E],
                protocolStack [CPSS_IP_PROTOCOL_IPV4_E],
                exceptionCmd [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E].
                Expected: non GT_OK.    */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_BAD_CHECKSUM_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_FAIL_RPF_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.28. For Tiger devices call function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_SPECADDR_EXT_HEADER_E], protocolStack
               [CPSS_IP_PROTOCOL_IPV6_E], exceptionCmd
               [CPSS_EXMX_IP_EXCP_CMD_ROUTE_E /
                CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E /
                CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E].
                Expected: GT_OK.    */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_SPECADDR_EXT_HEADER_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                  "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                  "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                  "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.29. Check unsupported commands. For Tiger devices call
               function with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_SPECADDR_EXT_HEADER_E],
               protocolStack [CPSS_IP_PROTOCOL_IPV6_E], exceptionCmd
               [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E /
                CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E /
                CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E /
                CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E]. Expected: non GT_OK.    */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_SPECADDR_EXT_HEADER_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            /* 1.30. For Tiger devices call function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_SPECADDR_EXT_HEADER_E], protocolStack
               [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd
               [CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E /
                CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E /
                CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E].
               Expected: GT_OK. */
            
            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_SPECADDR_EXT_HEADER_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_UC_BRIDGE_AND_MIRROR_MC_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);


            /* 1.31. Check unsupported commands. For Tiger devices call
               function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_SPECADDR_EXT_HEADER_E], protocolStack
               [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd
               [CPSS_EXMX_IP_EXCP_CMD_ROUTE_E /
                CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E /
                CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E /
                CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E]. Expected: non GT_OK.    */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_SPECADDR_EXT_HEADER_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_ROUTE_AND_MIRROR_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_BRIDGE_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);

            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_SOFT_DROP_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                 "Tiger device: %d, %d, %d, %d", dev,
                  exceptionType, protocolStack, exceptionCmd);


            /* 1.32. Check for out-of-range enum. For Tiger devices call
               function with exceptionType [0x5AAAAAA5], protocolStack
               [CPSS_IP_PROTOCOL_IPV4_E], exceptionCmd
               [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E].
               Expected: GT_BAD_PARAM.  */

            exceptionType = IP_CTRL_INVALID_ENUM_CNS;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                 "Tiger device: %d, exceptionType=%d",
                 dev, exceptionType);

            /* 1.33. Check for out-of-range enum. For Tiger devices call
               function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E],
               protocolStack [0x5AAAAAA5], exceptionCmd
               [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: GT_BAD_PARAM. */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E;
            protocolStack = IP_CTRL_INVALID_ENUM_CNS;
            exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                 "Tiger device: %d, protocolStack=%d",
                 dev, protocolStack);

            /* 1.34. Check for out-of-range enum. For Tiger devices call
               function with exceptionType
               [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E],
               protocolStack [CPSS_IP_PROTOCOL_IPV4_E],
               exceptionCmd [0x5AAAAAA5]. Expected: GT_BAD_PARAM.   */

            exceptionType = CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E;
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
            exceptionCmd = IP_CTRL_INVALID_ENUM_CNS;

            st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                               protocolStack, exceptionCmd);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                 "Tiger device: %d, exceptionCmd=%d",
                 dev, exceptionCmd);
        }

        /* For ALL devices */

        /* 1.35. Check Ipv4v6 unsupported protocol. Call function
           with exceptionType [CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E],
           protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E], exceptionCmd
           [CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E]. Expected: NOT GT_OK.   */

        exceptionType = CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
        exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_TRAP_2_CPU_E;

        st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                           protocolStack, exceptionCmd);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
             "Tiger device: %d, %d, %d, %d", dev,
              exceptionType, protocolStack, exceptionCmd);
    }

    /* Set valid params for all devices */
    exceptionType = CPSS_EXMX_IP_EXCP_TYPE_TTL_HOP_LIMIT_0_E;
    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    exceptionCmd = CPSS_EXMX_IP_EXCP_CMD_HARD_DROP_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                           protocolStack, exceptionCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpCtrlExceptionCmdSet(dev, exceptionType,
                                       protocolStack, exceptionCmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpCtrlMcRpfCheckModeSet
(
    IN GT_U8                           devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT      protocolStack,
    IN CPSS_EXMX_IP_RPF_CHECK_MODE_ENT rpfCheckMode 
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpCtrlMcRpfCheckModeSet)
{
/*
ITERATE_DEVICES
1.1. For all devices call function with rpfCheckMode [CPSS_EXMX_RPF_CHECK_VLAN_MODE_E / CPSS_EXMX_RPF_CHECK_INLIF_MODE_E], protocolStack [CPSS_IP_PROTOCOL_IPV4_E]. Expected: GT_OK.
1.2. For all devices call function with rpfCheckMode [CPSS_EXMX_RPF_CHECK_VLAN_MODE_E / CPSS_EXMX_RPF_CHECK_INLIF_MODE_E], protocolStack [CPSS_IP_PROTOCOL_IPV6_E]. Expected: GT_OK.
1.3. For all devices call function with rpfCheckMode [CPSS_EXMX_RPF_CHECK_VLAN_MODE_E], protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E]. Expected: NOT GT_OK.
1.4. Check for out-of-range enum. For all devices call function with protocolStack [0x5AAAAAA5], rpfCheckMode [CPSS_EXMX_RPF_CHECK_VLAN_MODE_E]. Expected: GT_BAD_PARAM.
1.5. Check for out-of-range enum. For all devices call function with protocolStack [CPSS_IP_PROTOCOL_IPV4_E], rpfCheckMode [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS                           st = GT_OK;
    GT_U8                               dev;
    CPSS_IP_PROTOCOL_STACK_ENT          protocolStack;
    CPSS_EXMX_IP_RPF_CHECK_MODE_ENT     rpfCheckMode;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. For all devices call function with rpfCheckMode     */
        /* [CPSS_EXMX_RPF_CHECK_VLAN_MODE_E /                       */
        /* CPSS_EXMX_RPF_CHECK_INLIF_MODE_E], protocolStack         */
        /* [CPSS_IP_PROTOCOL_IPV4_E].                               */
        /* Expected: GT_OK.                                         */
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        rpfCheckMode = CPSS_EXMX_RPF_CHECK_VLAN_MODE_E;

        st = cpssExMxIpCtrlMcRpfCheckModeSet(dev, protocolStack, rpfCheckMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, rpfCheckMode);

        rpfCheckMode = CPSS_EXMX_RPF_CHECK_INLIF_MODE_E;

        st = cpssExMxIpCtrlMcRpfCheckModeSet(dev, protocolStack, rpfCheckMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, rpfCheckMode);


        /* 1.2. For all devices call function with rpfCheckMode     */
        /* [CPSS_EXMX_RPF_CHECK_VLAN_MODE_E /                       */
        /*  CPSS_EXMX_RPF_CHECK_INLIF_MODE_E], protocolStack        */
        /* [CPSS_IP_PROTOCOL_IPV6_E].                               */
        /* Expected: GT_OK.                                         */
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        rpfCheckMode = CPSS_EXMX_RPF_CHECK_VLAN_MODE_E;

        st = cpssExMxIpCtrlMcRpfCheckModeSet(dev, protocolStack, rpfCheckMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, rpfCheckMode);

        rpfCheckMode = CPSS_EXMX_RPF_CHECK_INLIF_MODE_E;

        st = cpssExMxIpCtrlMcRpfCheckModeSet(dev, protocolStack, rpfCheckMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, rpfCheckMode);

        /* 1.3. For all devices call function with rpfCheckMode */
        /* [CPSS_EXMX_RPF_CHECK_VLAN_MODE_E], protocolStack     */
        /* [CPSS_IP_PROTOCOL_IPV4V6_E]. Expected: NOT GT_OK.        */

        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
        rpfCheckMode = CPSS_EXMX_RPF_CHECK_VLAN_MODE_E;

        st = cpssExMxIpCtrlMcRpfCheckModeSet(dev, protocolStack, rpfCheckMode);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, rpfCheckMode);

        /* 1.4. Check for out-of-range enum. For all devices call function  */
        /* with protocolStack [0x5AAAAAA5], rpfCheckMode                    */
        /* [CPSS_EXMX_RPF_CHECK_VLAN_MODE_E]. Expected: GT_BAD_PARAM.       */
        protocolStack = IP_CTRL_INVALID_ENUM_CNS;
        rpfCheckMode = CPSS_EXMX_RPF_CHECK_VLAN_MODE_E;

        st = cpssExMxIpCtrlMcRpfCheckModeSet(dev, protocolStack, rpfCheckMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, protocolStack);

        /* 1.5. Check for out-of-range enum. For all devices call function  */
        /* with protocolStack [CPSS_IP_PROTOCOL_IPV4_E], rpfCheckMode       */
        /* [0x5AAAAAA5]. Expected: GT_BAD_PARAM.                            */
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        rpfCheckMode = IP_CTRL_INVALID_ENUM_CNS;

        st = cpssExMxIpCtrlMcRpfCheckModeSet(dev, protocolStack, rpfCheckMode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
            "%d, rpfCheckMode=%d", dev, rpfCheckMode);
    }

    /* Set valid params */
    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    rpfCheckMode = CPSS_EXMX_RPF_CHECK_VLAN_MODE_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxIpCtrlMcRpfCheckModeSet(dev, protocolStack, rpfCheckMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpCtrlMcRpfCheckModeSet(dev, protocolStack, rpfCheckMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpv4CtrlSpecialMcRouteEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable 
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpv4CtrlSpecialMcRouteEnable)
{
/*
ITERATE_DEVICES
1.1. For all devices call function with enable [GT_TRUE and GT_FALSE]. Expected: GT_OK for TWIST-D , SAMBA/RUMBA, TIGER devices and GT_BAD_PARAM for others.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_BOOL                     enable;

    st = prvUtfNextDeviceReset(&dev, UTF_TWISTD_SAMBA_TIGER_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. For all devices call function with enable [GT_TRUE  */
        /* and GT_FALSE]. Expected: GT_OK for TWIST-D , SAMBA/RUMBA,*/
        /* TIGER devices and GT_BAD_PARAM for others.               */
        enable = GT_TRUE;

        st = cpssExMxIpv4CtrlSpecialMcRouteEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
         "TWIST-D ,SAMBA/RUMBA, TIGER device: %d, %d", dev, enable);

        enable = GT_FALSE;

        st = cpssExMxIpv4CtrlSpecialMcRouteEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
         "TWIST-D ,SAMBA/RUMBA, TIGER device: %d, %d", dev, enable);
    }

    enable = GT_FALSE;

    st = prvUtfNextDeviceReset(&dev, UTF_TWISTD_SAMBA_TIGER_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxIpv4CtrlSpecialMcRouteEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpv4CtrlSpecialMcRouteEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpv4CtrlTrapPktTcSet
(
    IN  GT_U8 devNum,
    IN  GT_U8 tc
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpv4CtrlTrapPktTcSet)
{
/*
ITERATE_DEVICES
1.1. For all devices call function with tc [2]. Expected: GT_OK for TWIST, TWIST-D, SAMBA/RUMBA and non GT_OK for others(Tiger).
1.2. Check for out of range traffic class. For TWIST, TWIST-D , SAMBA/RUMBA devices call function with tc [CPSS_TC_RANGE_CNS = 8]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_U8                       tc;

    st = prvUtfNextDeviceReset(&dev, UTF_TWISTC_TWISTD_SAMBA_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. For all devices call function with tc [2].  */
        /* Expected: GT_OK for TWIST, TWIST-D, SAMBA/RUMBA  */
        /* and non GT_OK for others(Tiger).                 */
        tc = 2;

        st = cpssExMxIpv4CtrlTrapPktTcSet(dev, tc);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
         "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d", dev, tc);

        /* 1.2. Check for out of range traffic class. For TWIST, TWIST-D,    */
        /* SAMBA/RUMBA devices call function with tc [CPSS_TC_RANGE_CNS = 8].*/
        /* Expected: GT_BAD_PARAM.                                           */
        tc = CPSS_TC_RANGE_CNS;

        st = cpssExMxIpv4CtrlTrapPktTcSet(dev, tc);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
         "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d", dev, tc);
    }

    /* set valid params */
    tc = 2;

    st = prvUtfNextDeviceReset(&dev, UTF_TWISTC_TWISTD_SAMBA_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxIpv4CtrlTrapPktTcSet(dev, tc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpv4CtrlTrapPktTcSet(dev, tc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpv4AgeOverrunEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpv4AgeOverrunEnable)
{
/*
ITERATE_DEVICES
1.1. For all devices call function with enable [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_BOOL                     enable;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with enable [GT_TRUE and GT_FALSE].   */
        /* Expected: GT_OK.                                         */
        enable = GT_TRUE;

        st = cpssExMxIpv4AgeOverrunEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_FALSE;

        st = cpssExMxIpv4AgeOverrunEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    enable = GT_TRUE;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxIpv4AgeOverrunEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpv4AgeOverrunEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpv4McBridgeRpfFailureEnable
(
    IN GT_U8                devNum,
    IN GT_BOOL              enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpv4McBridgeRpfFailureEnable)
{
/*
ITERATE_DEVICES
1.1. For all devices call function with enable [GT_TRUE and GT_FALSE]. Expected: GT_OK  for TWIST-D devices and non GT_OK for others.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_BOOL                     enable;

    st = prvUtfNextDeviceReset(&dev, UTF_TWISTD_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. For all devices call function with enable [GT_TRUE  */
        /* and GT_FALSE]. Expected: GT_OK  for TWIST-D devices      */
        /* and non GT_OK for others.                                */
        enable = GT_TRUE;

        st = cpssExMxIpv4McBridgeRpfFailureEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
         "TWIST-D device: %d, %d", dev, enable);

        enable = GT_FALSE;

        st = cpssExMxIpv4McBridgeRpfFailureEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
         "TWIST-D device: %d, %d", dev, enable);
    }

    enable = GT_FALSE;

    st = prvUtfNextDeviceReset(&dev, UTF_TWISTD_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxIpv4McBridgeRpfFailureEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpv4McBridgeRpfFailureEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpDefaultMcRouteEntriesBaseAddrSet
(
    IN GT_U8  devNum,
    IN GT_U32 memAddr
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpDefaultMcRouteEntriesBaseAddrSet)
{
/*
ITERATE_DEVICES
1.1. For all devices call function with memAddr [1000]. Expected: GT_OK for TWIST, TWIST-D , SAMBA/RUMBA and GT_BAD_PARAM for others.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_U32                      memAddr;

    st = prvUtfNextDeviceReset(&dev, UTF_TWISTC_TWISTD_SAMBA_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. For all devices call function with memAddr          */
        /* [0 / 0xFFFFFFFE / 1000].                                 */
        /* Expected: GT_OK for TWIST, TWIST-D , SAMBA/RUMBA and     */
        /* non GT_OK for others.                                    */
        memAddr = 0;

        st = cpssExMxIpDefaultMcRouteEntriesBaseAddrSet(dev, memAddr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
         "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d", dev, memAddr);

        memAddr = 0xFFFFFFFE;

        st = cpssExMxIpDefaultMcRouteEntriesBaseAddrSet(dev, memAddr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
         "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d", dev, memAddr);

        memAddr = 1000;

        st = cpssExMxIpDefaultMcRouteEntriesBaseAddrSet(dev, memAddr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
         "TWIST, TWIST-D, SAMBA/RUMBA device: %d, %d", dev, memAddr);
    }

    /* set valid params */
    memAddr = 1000;

    st = prvUtfNextDeviceReset(&dev, UTF_TWISTC_TWISTD_SAMBA_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxIpDefaultMcRouteEntriesBaseAddrSet(dev, memAddr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpDefaultMcRouteEntriesBaseAddrSet(dev, memAddr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssExMxIpCtrl suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxIpCtrl)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpCtrlAgeRefreshEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpFlowRedirectRouteEntriesMSBBaseAddrSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpMLLEntriesBaseAddrSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpVrBaseAddrSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpCntSetGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpCntSetClear)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpCntGlblGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpCtrlExceptionCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpCtrlMcRpfCheckModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpv4CtrlSpecialMcRouteEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpv4CtrlTrapPktTcSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpv4AgeOverrunEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpv4McBridgeRpfFailureEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpDefaultMcRouteEntriesBaseAddrSet)
UTF_SUIT_END_TESTS_MAC(cpssExMxIpCtrl)
