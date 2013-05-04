/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxSalPortBufMgUT.c
*
* DESCRIPTION:
*       Unit tests for Port configuration and control facility.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/dxSal/port/cpssDxSalPortBufMg.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* Invalid enum */
#define BUF_MG_INVALID_ENUM_CNS        0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortRxFcProfileSet
(
    IN GT_U8                            devNum,
    IN GT_U8                            portNum,
    IN CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortRxFcProfileSet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS (DxSal)
1.1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E / CPSS_PORT_RX_FC_PROFILE_2_E / CPSS_PORT_RX_FC_PROFILE_3_E / CPSS_PORT_RX_FC_PROFILE_4_E].
Expected: GT_OK.
1.1.2. Call cpssDxSalPortRxFcProfileGet
Expected: GT_OK and the same profileSet.
1.1.3. Call with out of range profileSet [0x5AAAAAA5]
Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSetRet;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E /
               CPSS_PORT_RX_FC_PROFILE_2_E / CPSS_PORT_RX_FC_PROFILE_3_E /
               CPSS_PORT_RX_FC_PROFILE_4_E]. Expected: GT_OK.   */

            profileSet = CPSS_PORT_RX_FC_PROFILE_1_E;

            st = cpssDxSalPortRxFcProfileSet(dev, port, profileSet);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, profileSet);

            profileSet = CPSS_PORT_RX_FC_PROFILE_2_E;

            st = cpssDxSalPortRxFcProfileSet(dev, port, profileSet);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, profileSet);

            profileSet = CPSS_PORT_RX_FC_PROFILE_3_E;

            st = cpssDxSalPortRxFcProfileSet(dev, port, profileSet);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, profileSet);

            profileSet = CPSS_PORT_RX_FC_PROFILE_4_E;

            st = cpssDxSalPortRxFcProfileSet(dev, port, profileSet);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, profileSet);

            /* 1.1.2. Call cpssDxSalPortRxFcProfileGet
                      Expected: GT_OK and the same profileSet.  */

            st = cpssDxSalPortRxFcProfileGet(dev, port, &profileSetRet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxSalPortRxFcProfileGet: %d", dev);

            if (GT_OK == st)
            {
                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(profileSet, profileSetRet,
                 "cpssDxSalPortRxFcProfileGet: get another profileSet than was set: dev = %d", dev);
            }

            /* 1.1.3. Call with out of range profileSet [0x5AAAAAA5]
                      Expected: GT_BAD_PARAM.   */

            profileSet = BUF_MG_INVALID_ENUM_CNS;

            st = cpssDxSalPortRxFcProfileSet(dev, port, profileSet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, profileSet = %d",
                                         dev, profileSet);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;
        profileSet = CPSS_PORT_RX_FC_PROFILE_1_E;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssDxSalPortRxFcProfileSet(dev, port, profileSet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssDxSalPortRxFcProfileSet(dev, port, profileSet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortRxFcProfileSet(dev, port, profileSet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = 0;
    profileSet = CPSS_PORT_RX_FC_PROFILE_1_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortRxFcProfileSet(dev, port, profileSet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortRxFcProfileSet(dev, port, profileSet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortRxFcProfileGet
(
    IN  GT_U8                            devNum,
    IN  GT_U8                            portNum,
    OUT CPSS_PORT_RX_FC_PROFILE_SET_ENT  *profileSetPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortRxFcProfileGet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS (DxSal)
1.1.1. Call with non-null profileSetPtr. 
Expected: GT_OK.
1.1.2. Call with profileSetPtr [NULL]. 
Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null profileSetPtr. Expected: GT_OK.*/

            st = cpssDxSalPortRxFcProfileGet(dev, port, &profileSet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with profileSetPtr [NULL]. Expected: GT_BAD_PTR.*/

            st = cpssDxSalPortRxFcProfileGet(dev, port, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                         "%d, profileSetPtr = NULL", dev);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssDxSalPortRxFcProfileGet(dev, port, &profileSet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssDxSalPortRxFcProfileGet(dev, port, &profileSet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortRxFcProfileGet(dev, port, &profileSet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortRxFcProfileGet(dev, port, &profileSet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortRxFcProfileGet(dev, port, &profileSet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortXonLimitSet
(
    IN GT_U8                            devNum,
    IN CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet,
    IN GT_U16                           xonLimit
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortXonLimitSet)
{
/*
ITERATE_DEVICES (DxSal)
1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E / CPSS_PORT_RX_FC_PROFILE_2_E] and xonLimit[5 / 1023].
Expected: GT_OK.
1.2. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_3_E / CPSS_PORT_RX_FC_PROFILE_4_E] (not valid values) and xonLimit[5].
Expected: NOT GT_OK.
1.3. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E] and xonLimit[1024] (out of range).
Expected: NOT GT_OK.
1.4. Call with out of range profileSet [0x5AAAAAA5] and xonLimit [5].
Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet;
    GT_U16                           xonLimit;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E /
           CPSS_PORT_RX_FC_PROFILE_2_E] and xonLimit[5 / 1023]. Expected: GT_OK.*/

        profileSet = CPSS_PORT_RX_FC_PROFILE_1_E;
        xonLimit = 1023;

        st = cpssDxSalPortXonLimitSet(dev, profileSet, xonLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileSet, xonLimit);

        profileSet = CPSS_PORT_RX_FC_PROFILE_2_E;
        xonLimit = 5;

        st = cpssDxSalPortXonLimitSet(dev, profileSet, xonLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileSet, xonLimit);

        /* 1.2. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_3_E /
           CPSS_PORT_RX_FC_PROFILE_4_E] (not valid values) and xonLimit[5].
           Expected: NOT GT_OK. */

        profileSet = CPSS_PORT_RX_FC_PROFILE_3_E;

        st = cpssDxSalPortXonLimitSet(dev, profileSet, xonLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileSet);

        profileSet = CPSS_PORT_RX_FC_PROFILE_4_E;

        st = cpssDxSalPortXonLimitSet(dev, profileSet, xonLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileSet);

        /* 1.3. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E] and
           xonLimit[1024] (out of range). Expected: NOT GT_OK.  */

        profileSet = CPSS_PORT_RX_FC_PROFILE_1_E;
        xonLimit = 1024;

        st = cpssDxSalPortXonLimitSet(dev, profileSet, xonLimit);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, xonLimit = %d", dev, xonLimit);

        xonLimit = 5;

        /* 1.4. Call with out of range profileSet [0x5AAAAAA5] and
           xonLimit [5]. Expected: GT_BAD_PARAM.    */

        profileSet = BUF_MG_INVALID_ENUM_CNS;

        st = cpssDxSalPortXonLimitSet(dev, profileSet, xonLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, profileSet);

    }

    profileSet = CPSS_PORT_RX_FC_PROFILE_1_E;
    xonLimit = 5;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortXonLimitSet(dev, profileSet, xonLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortXonLimitSet(dev, profileSet, xonLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortXoffLimitSet
(
    IN GT_U8                            devNum,
    IN CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet,
    IN GT_U16                           xoffLimit
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortXoffLimitSet)
{
/*
ITERATE_DEVICES (DxSal)
1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E / CPSS_PORT_RX_FC_PROFILE_2_E] and xoffLimit[5 / 1023].
Expected: GT_OK.
1.2. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_3_E / CPSS_PORT_RX_FC_PROFILE_4_E] (not valid values) and xoffLimit[5].
Expected: NOT GT_OK.
1.3. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E] and xoffLimit[1024] (out of range).
Expected: NOT GT_OK.
1.4. Call with out of range profileSet [0x5AAAAAA5] and xoffLimit [5].
Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet;
    GT_U16                           xoffLimit;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E /
           CPSS_PORT_RX_FC_PROFILE_2_E] and xoffLimit[5 / 1023]. Expected: GT_OK.*/

        profileSet = CPSS_PORT_RX_FC_PROFILE_1_E;
        xoffLimit = 1023;

        st = cpssDxSalPortXoffLimitSet(dev, profileSet, xoffLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileSet, xoffLimit);

        profileSet = CPSS_PORT_RX_FC_PROFILE_2_E;
        xoffLimit = 5;

        st = cpssDxSalPortXoffLimitSet(dev, profileSet, xoffLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileSet, xoffLimit);

        /* 1.2. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_3_E /
           CPSS_PORT_RX_FC_PROFILE_4_E] (not valid values) and xoffLimit[5].
           Expected: NOT GT_OK. */

        profileSet = CPSS_PORT_RX_FC_PROFILE_3_E;

        st = cpssDxSalPortXoffLimitSet(dev, profileSet, xoffLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileSet);

        profileSet = CPSS_PORT_RX_FC_PROFILE_4_E;

        st = cpssDxSalPortXoffLimitSet(dev, profileSet, xoffLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileSet);

        /* 1.3. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E] and
           xoffLimit[1024] (out of range). Expected: NOT GT_OK.  */

        profileSet = CPSS_PORT_RX_FC_PROFILE_1_E;
        xoffLimit = 1024;

        st = cpssDxSalPortXoffLimitSet(dev, profileSet, xoffLimit);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, xoffLimit = %d", dev, xoffLimit);

        xoffLimit = 5;

        /* 1.4. Call with out of range profileSet [0x5AAAAAA5] and
           xoffLimit [5]. Expected: GT_BAD_PARAM.    */

        profileSet = BUF_MG_INVALID_ENUM_CNS;

        st = cpssDxSalPortXoffLimitSet(dev, profileSet, xoffLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, profileSet);

    }

    profileSet = CPSS_PORT_RX_FC_PROFILE_1_E;
    xoffLimit = 5;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortXoffLimitSet(dev, profileSet, xoffLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortXoffLimitSet(dev, profileSet, xoffLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}



/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortRxBufLimitSet
(
    IN GT_U8    devNum,
    IN CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet,
    IN GT_U16   rxBufLimit
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortRxBufLimitSet)
{
/*
ITERATE_DEVICES (DxSal)
1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E / CPSS_PORT_RX_FC_PROFILE_2_E] and rxBufLimit[5 / 1023].
Expected: GT_OK.
1.2. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_3_E / CPSS_PORT_RX_FC_PROFILE_4_E] (not valid values) and rxBufLimit[5].
Expected: NOT GT_OK.
1.3. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E] and rxBufLimit[1024] (out of range).
Expected: NOT GT_OK.
1.4. Call with out of range profileSet [0x5AAAAAA5] and rxBufLimit [5].
Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet;
    GT_U16                           rxBufLimit;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E /
           CPSS_PORT_RX_FC_PROFILE_2_E] and rxBufLimit[5 / 1023]. Expected: GT_OK.*/

        profileSet = CPSS_PORT_RX_FC_PROFILE_1_E;
        rxBufLimit = 1023;

        st = cpssDxSalPortRxBufLimitSet(dev, profileSet, rxBufLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileSet, rxBufLimit);

        profileSet = CPSS_PORT_RX_FC_PROFILE_2_E;
        rxBufLimit = 5;

        st = cpssDxSalPortRxBufLimitSet(dev, profileSet, rxBufLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileSet, rxBufLimit);

        /* 1.2. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_3_E /
           CPSS_PORT_RX_FC_PROFILE_4_E] (not valid values) and rxBufLimit[5].
           Expected: NOT GT_OK. */

        profileSet = CPSS_PORT_RX_FC_PROFILE_3_E;

        st = cpssDxSalPortRxBufLimitSet(dev, profileSet, rxBufLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileSet);

        profileSet = CPSS_PORT_RX_FC_PROFILE_4_E;

        st = cpssDxSalPortRxBufLimitSet(dev, profileSet, rxBufLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileSet);

        /* 1.3. Call with profileSet [CPSS_PORT_RX_FC_PROFILE_1_E] and
           rxBufLimit[1024] (out of range). Expected: NOT GT_OK.  */

        profileSet = CPSS_PORT_RX_FC_PROFILE_1_E;
        rxBufLimit = 1024;

        st = cpssDxSalPortRxBufLimitSet(dev, profileSet, rxBufLimit);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, rxBufLimit = %d", dev, rxBufLimit);

        rxBufLimit = 5;

        /* 1.4. Call with out of range profileSet [0x5AAAAAA5] and
           rxBufLimit [5]. Expected: GT_BAD_PARAM.    */

        profileSet = BUF_MG_INVALID_ENUM_CNS;

        st = cpssDxSalPortRxBufLimitSet(dev, profileSet, rxBufLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, profileSet);

    }

    profileSet = CPSS_PORT_RX_FC_PROFILE_1_E;
    rxBufLimit = 5;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortRxBufLimitSet(dev, profileSet, rxBufLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortRxBufLimitSet(dev, profileSet, rxBufLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortCpuRxBufCountGet
(
    IN  GT_U8   devNum,
    OUT GT_U16  *cpuRxBufCntPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortCpuRxBufCountGet)
{
/*
ITERATE_DEVICES (DxSal)
1.1. Call with non-null cpuRxBufCntPtr. 
Expected: GT_OK.
1.2. Call with cpuRxBufCntPtr [NULL]. 
Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U16                  cpuRxBufCnt;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with non-null cpuRxBufCntPtr. Expected: GT_OK. */

        st = cpssDxSalPortCpuRxBufCountGet(dev, &cpuRxBufCnt);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call with cpuRxBufCntPtr [NULL]. Expected: GT_BAD_PTR.*/

        st = cpssDxSalPortCpuRxBufCountGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, cpuRxBufCntPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortCpuRxBufCountGet(dev, &cpuRxBufCnt);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortCpuRxBufCountGet(dev, &cpuRxBufCnt);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortFcHolSysModeSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXSAL_PORT_HOL_FC_ENT  modeFcHol
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortFcHolSysModeSet)
{
/*
ITERATE_DEVICES (DxSal)
1.1. Call with modeFcHol [CPSS_DXSAL_PORT_FC_E / CPSS_DXSAL_PORT_HOL_E / CPSS_DXSAL_PORT_HOL_CELL_E].
Expected: GT_OK.
1.2. Call with out of range modeFcHol [0x5AAAAAA5].
Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_DXSAL_PORT_HOL_FC_ENT  modeFcHol;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with modeFcHol [CPSS_DXSAL_PORT_FC_E /
         CPSS_DXSAL_PORT_HOL_E / CPSS_DXSAL_PORT_HOL_CELL_E].Expected: GT_OK.*/

        modeFcHol = CPSS_DXSAL_PORT_FC_E;

        st = cpssDxSalPortFcHolSysModeSet(dev, modeFcHol);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, modeFcHol);

        modeFcHol = CPSS_DXSAL_PORT_HOL_E;

        st = cpssDxSalPortFcHolSysModeSet(dev, modeFcHol);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, modeFcHol);

        modeFcHol = CPSS_DXSAL_PORT_HOL_CELL_E;

        st = cpssDxSalPortFcHolSysModeSet(dev, modeFcHol);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, modeFcHol);

        /*1.2. Call with out of range modeFcHol [0x5AAAAAA5].
                Expected: GT_BAD_PARAM. */

        modeFcHol = BUF_MG_INVALID_ENUM_CNS;

        st = cpssDxSalPortFcHolSysModeSet(dev, modeFcHol);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, modeFcHol);
    }

    modeFcHol = CPSS_DXSAL_PORT_FC_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortFcHolSysModeSet(dev, modeFcHol);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortFcHolSysModeSet(dev, modeFcHol);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssDxSalPortBufMg suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxSalPortBufMg)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortRxFcProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortRxFcProfileGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortXonLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortXoffLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortRxBufLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortCpuRxBufCountGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortFcHolSysModeSet)
UTF_SUIT_END_TESTS_MAC(cpssDxSalPortBufMg)
