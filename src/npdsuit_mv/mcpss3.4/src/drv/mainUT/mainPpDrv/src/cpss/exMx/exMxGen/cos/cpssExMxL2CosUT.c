/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxL2CosUT.c
*
* DESCRIPTION:
*       Unit Tests for L2 bridging Class-Of-Service configuration.
*
* DEPENDENCIES:
*
*       $Revision: 1.1.1.1 $
*******************************************************************************/
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/exMx/exMxGen/cos/cpssExMxL2Cos.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>


/* Invalid enum                     */
#define L2_COS_INVALID_ENUM_CNS             0x5AAAAAA5

/* Tested virtual port id. Test uses this value as default value port id.   */
#define L2_COS_VALID_VIRT_PORT_CNS          0


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosPortDefaultTcSet
(
    IN GT_U8    dev,
    IN GT_U8    port,
    IN GT_U8    trfClass
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosPortDefaultTcSet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call function with trfClass [3]. Expected: GT_OK.
1.1.2. Call function cpssExMxCosPortDefaultTcGet with non-Null trfClassPtr. Expected: GT_OK and trfClass [3].
1.1.3. Call function with out of range trfClass [CPSS_TC_RANGE_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       port;
    GT_U8       trfClass;


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
            GT_U8   trfClassWritten;

            /* 1.1.1. Call function with trfClass [3].                                      */
            /* Expected: GT_OK.                                                             */
            trfClass = 3;

            st = cpssExMxCosPortDefaultTcSet(dev, port, trfClass);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trfClass);

            /* 1.1.2. Call function cpssExMxCosPortDefaultTcGet with non-Null trfClassPtr.    */
            /* Expected: GT_OK and trfClass [3].                                            */
            trfClassWritten = 0;

            st = cpssExMxCosPortDefaultTcGet(dev, port, &trfClassWritten);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxCosPortDefaultTcGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(trfClass, trfClassWritten, dev, port, trfClass);

            /* 1.1.3. Call function with out of range trfClass [CPSS_TC_RANGE_CNS].         */
            /* Expected: GT_BAD_PARAM.                                                      */
            trfClass = CPSS_TC_RANGE_CNS;

            st = cpssExMxCosPortDefaultTcSet(dev, port, trfClass);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, trfClass);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        port = L2_COS_VALID_VIRT_PORT_CNS;
        trfClass = 3;

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            st = cpssExMxCosPortDefaultTcSet(dev, port, trfClass);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxCosPortDefaultTcSet(dev, port, trfClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = L2_COS_VALID_VIRT_PORT_CNS;
    trfClass = 3;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosPortDefaultTcSet(dev, port, trfClass);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosPortDefaultTcSet(dev, port, trfClass);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosPortDefaultTcGet
(
    IN  GT_U8   dev,
    IN  GT_U8   port,
    OUT GT_U8   *trfClassPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosPortDefaultTcGet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call function with non-NULL trfClassPtr. Expected: GT_OK.
1.1.2. Call function with trfClassPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       port;
    GT_U8       trfClass;


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
            /* 1.1.1. Call function with non-NULL trfClassPtr.  */
            /* Expected: GT_OK.                                 */
            st = cpssExMxCosPortDefaultTcGet(dev, port, &trfClass);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call function with trfClassPtr [NULL].    */
            /* Expected: GT_BAD_PTR.                            */
            st = cpssExMxCosPortDefaultTcGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        port = L2_COS_VALID_VIRT_PORT_CNS;

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            st = cpssExMxCosPortDefaultTcGet(dev, port, &trfClass);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxCosPortDefaultTcGet(dev, port, &trfClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = L2_COS_VALID_VIRT_PORT_CNS;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosPortDefaultTcGet(dev, port, &trfClass);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosPortDefaultTcGet(dev, port, &trfClass);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosUserPrio2TcSet
(
    IN GT_U8    dev,
    IN GT_U8    userPrio,
    IN GT_U8    trfClass
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosUserPrio2TcSet)
{
/*
ITERATE_DEVICES
1.1. Call function with userPrio [1] and trfClass [2]. Expected: GT_OK.
1.2. Call function cpssExMxCosUserPrio2TcGet with userPrio [1] and non-NULL trfClassPtr. Expected: GT_OK and trfClass [2].
1.3. Call function with out of range userPrio [CPSS_USER_PRIORITY_RANGE_CNS + 1]. Expected: GT_BAD_PARAM.
1.4. Call function with out of range trfClass [CPSS_TC_RANGE_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       userPrio;
    GT_U8       trfClass;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        GT_U8   trfClassWritten;

        /* 1.1. Call function with userPrio [1] and trfClass [2].                                   */
        /* Expected: GT_OK.                                                                         */
        userPrio = 1;
        trfClass = 2;

        st = cpssExMxCosUserPrio2TcSet(dev, userPrio, trfClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, userPrio, trfClass);

        /* 1.2. Call function cpssExMxCosUserPrio2TcGet with userPrio [1] and non-NULL trfClassPtr. */
        /* Expected: GT_OK and trfClass [2].                                                        */
        st = cpssExMxCosUserPrio2TcGet(dev, userPrio, &trfClassWritten);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxCosUserPrio2TcGet: %d, %d", dev, userPrio);
        UTF_VERIFY_EQUAL3_PARAM_MAC(trfClass, trfClassWritten, dev, userPrio, trfClass);

        /* 1.3. Call function with out of range userPrio [CPSS_USER_PRIORITY_RANGE_CNS + 1].        */
        /* Expected: GT_BAD_PARAM.                                                                  */
        userPrio = CPSS_USER_PRIORITY_RANGE_CNS + 1;

        st = cpssExMxCosUserPrio2TcSet(dev, userPrio, trfClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, userPrio, trfClass);

        /* 1.4. Call function with out of range trfClass [CPSS_TC_RANGE_CNS].                       */
        /* Expected: GT_BAD_PARAM.                                                                  */
        userPrio = 1;
        trfClass = CPSS_TC_RANGE_CNS;

        st = cpssExMxCosUserPrio2TcSet(dev, userPrio, trfClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, userPrio, trfClass);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    userPrio = 1;
    trfClass = 2;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosUserPrio2TcSet(dev, userPrio, trfClass);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosUserPrio2TcSet(dev, userPrio, trfClass);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosUserPrio2TcGet
(
    IN  GT_U8   dev,
    IN  GT_U8   userPrio,
    OUT GT_U8   *trfClassPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosUserPrio2TcGet)
{
/*
ITERATE_DEVICES
1.1. Call function with userPrio [1] and non-NULL trfClassPtr. Expected: GT_OK.
1.2. Call function with out of range userPrio [CPSS_USER_PRIORITY_RANGE_CNS + 1]. Expected: GT_BAD_PARAM.
1.3. Call function with trfClassPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       userPrio;
    GT_U8       trfClass;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with userPrio [1] and non-NULL trfClassPtr.                       */
        /* Expected: GT_OK.                                                                     */
        userPrio = 1;

        st = cpssExMxCosUserPrio2TcGet(dev, userPrio, &trfClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, userPrio);

        /* 1.2. Call function with out of range userPrio [CPSS_USER_PRIORITY_RANGE_CNS + 1].    */
        /* Expected: GT_BAD_PARAM.                                                              */
        userPrio = CPSS_USER_PRIORITY_RANGE_CNS + 1;

        st = cpssExMxCosUserPrio2TcGet(dev, userPrio, &trfClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, userPrio);

        /* 1.3. Call function with trfClassPtr [NULL].                                          */
        /* Expected: GT_BAD_PTR.                                                                */
        userPrio = 1;

        st = cpssExMxCosUserPrio2TcGet(dev, userPrio, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, userPrio);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    userPrio = 1;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosUserPrio2TcGet(dev, userPrio, &trfClass);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosUserPrio2TcGet(dev, userPrio, &trfClass);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosTc2UserPrioSet
(
    IN GT_U8    dev,
    IN GT_U8    trfClass,
    IN GT_U8    userPrio
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosTc2UserPrioSet)
{
/*
ITERATE_DEVICES
1.1. Call function with trfClass [1] and userPrio [3]. Expected: GT_OK.
1.2. Call function cpssExMxCosTc2UserPrioGet with trfClass [1] and non-NULL userPrioPtr. Expected: GT_OK and userPrio [3].
1.3. Call function with out of range trfClass [CPSS_TC_RANGE_CNS]. Expected: GT_BAD_PARAM.
1.4. Call function with out of range userPrio [CPSS_USER_PRIORITY_RANGE_CNS + 1]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       trfClass;
    GT_U8       userPrio;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        GT_U8   userPrioWritten;

        /* 1.1. Call function with trfClass [1] and userPrio [3].                                       */
        /* Expected: GT_OK.                                                                             */
        trfClass = 1;
        userPrio = 3;

        st = cpssExMxCosTc2UserPrioSet(dev, trfClass, userPrio);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trfClass, userPrio);

        /* 1.2. Call function cpssExMxCosTc2UserPrioGet with trfClass [1] and non-NULL userPrioPtr.     */
        /* Expected: GT_OK and userPrio [3].                                                            */
        userPrioWritten = 0;

        st = cpssExMxCosTc2UserPrioGet(dev, trfClass, &userPrioWritten);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxCosTc2UserPrioGet: %d, %d", dev, trfClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(userPrio, userPrioWritten, dev, trfClass, userPrio);

        /* 1.3. Call function with out of range trfClass [CPSS_TC_RANGE_CNS].                           */
        /* Expected: GT_BAD_PARAM.                                                                      */
        trfClass = CPSS_TC_RANGE_CNS;
        userPrio = 3;

        st = cpssExMxCosTc2UserPrioSet(dev, trfClass, userPrio);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, trfClass, userPrio);

        /* 1.4. Call function with out of range userPrio [CPSS_USER_PRIORITY_RANGE_CNS + 1].            */
        /* Expected: GT_BAD_PARAM.                                                                      */
        trfClass = 1;
        userPrio = CPSS_USER_PRIORITY_RANGE_CNS + 1;

        st = cpssExMxCosTc2UserPrioSet(dev, trfClass, userPrio);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, trfClass, userPrio);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    trfClass = 1;
    userPrio = 3;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosTc2UserPrioSet(dev, trfClass, userPrio);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosTc2UserPrioSet(dev, trfClass, userPrio);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosTc2UserPrioGet
(
    IN  GT_U8   dev,
    IN  GT_U8   trfClass,
    OUT GT_U8   *userPrioPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosTc2UserPrioGet)
{
/*
ITERATE_DEVICES
1.1. Call function with trfClass [1] and non-NULL userPrioPtr. Expected: GT_OK.
1.2. Call function with out of range trfClass [CPSS_TC_RANGE_CNS]. Expected: GT_BAD_PARAM.
1.3. Call function with userPrio [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       trfClass;
    GT_U8       userPrio;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with trfClass [1] and non-NULL userPrioPtr.       */
        /* Expected: GT_OK.                                                     */
        trfClass = 1;

        st = cpssExMxCosTc2UserPrioGet(dev, trfClass, &userPrio);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trfClass, userPrio);

        /* 1.2. Call function with out of range trfClass [CPSS_TC_RANGE_CNS].   */
        /* Expected: GT_BAD_PARAM.                                              */
        trfClass = CPSS_TC_RANGE_CNS;

        st = cpssExMxCosTc2UserPrioGet(dev, trfClass, &userPrio);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, trfClass, userPrio);

        /* 1.3. Call function with userPrio [NULL].                             */
        /* Expected: GT_BAD_PTR.                                                */
        trfClass = 1;

        st = cpssExMxCosTc2UserPrioGet(dev, trfClass, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, trfClass);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    trfClass = 1;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosTc2UserPrioGet(dev, trfClass, &userPrio);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosTc2UserPrioGet(dev, trfClass, &userPrio);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosUserPrioModeSet
(
    IN GT_U8                        devNum,
    IN GT_U8                                 port,
    IN CPSS_COS_USER_PRIO_MODE_ENT  userPrioMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosUserPrioModeSet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call function with userPrioMode [CPSS_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E]. Expected: GT_OK for devices from Tiger family and GT_NOT_SUPPORTED for other devices.
1.1.2. Call function cpssExMxCosUserPrioModeDebugCmdGet with non-NULL userPrioModePtr. Expected: GT_OK and userPrioMode [CPSS_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E] for devices from Tiger family and GT_NOT_SUPPORTED for other devices.
1.1.3. For devices from Tiger family call function with out of range userPrioMode [CPSS_COS_USER_PRIO_MODE_MAX_E]. Expected: GT_BAD_PARAM.
1.1.4. For devices from Tiger family call function with invalid userPrioMode [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS                       st = GT_OK;
    GT_U8                           dev;
    GT_U8                           port;
    CPSS_COS_USER_PRIO_MODE_ENT     userPrioMode;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For each active device id go over all active virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            CPSS_COS_USER_PRIO_MODE_ENT     userPrioModeWritten;

            /* 1.1.1. Call function with userPrioMode [CPSS_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E].   */
            /* Expected: GT_OK for devices from Tiger family and GT_NOT_SUPPORTED for other devices.        */
            userPrioMode = CPSS_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E;

            st = cpssExMxCosUserPrioModeSet(dev, port, userPrioMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, userPrioMode);

            /* 1.1.2. Call function cpssExMxCosUserPrioModeDebugCmdGet with non-NULL userPrioModePtr.       */
            /* Expected: GT_OK and userPrioMode [CPSS_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E]          */
            /* for devices from Tiger family and GT_NOT_SUPPORTED for other devices.                        */
            userPrioModeWritten = L2_COS_INVALID_ENUM_CNS;

            st = cpssExMxCosUserPrioModeDebugCmdGet(dev, port, &userPrioModeWritten);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxCosUserPrioModeDebugCmdGet: %d, %d, %d",
                                         dev, port, userPrioModeWritten);
            UTF_VERIFY_EQUAL3_PARAM_MAC(userPrioMode, userPrioModeWritten, dev, port, userPrioMode);

            /* 1.1.3. For devices from Tiger family call function with out of range                         */
            /* userPrioMode [CPSS_COS_USER_PRIO_MODE_MAX_E].                                                */
            /* Expected: GT_BAD_PARAM.                                                                      */
            userPrioMode = CPSS_COS_USER_PRIO_MODE_MAX_E;

            st = cpssExMxCosUserPrioModeSet(dev, port, userPrioMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, userPrioMode);

            /* 1.1.4. For devices from Tiger family call function with invalid userPrioMode [0x5AAAAAA5].   */
            /* Expected: GT_BAD_PARAM.                                                                      */
            userPrioMode = L2_COS_INVALID_ENUM_CNS;

            st = cpssExMxCosUserPrioModeSet(dev, port, userPrioMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, userPrioMode);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        port = L2_COS_VALID_VIRT_PORT_CNS;
        userPrioMode = CPSS_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E;

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            st = cpssExMxCosUserPrioModeSet(dev, port, userPrioMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxCosUserPrioModeSet(dev, port, userPrioMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = L2_COS_VALID_VIRT_PORT_CNS;
    userPrioMode = CPSS_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosUserPrioModeSet(dev, port, userPrioMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosUserPrioModeSet(dev, port, userPrioMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosUserPrioModeDebugCmdGet
(
    IN  GT_U8                       devNum,
    IN GT_U8                                 port,
    OUT CPSS_COS_USER_PRIO_MODE_ENT *userPrioModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosUserPrioModeDebugCmdGet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call function with non-NULL userPrioModePtr. Expected: GT_OK for devices from Tiger family and GT_NOT_SUPPORTED for other devices.
1.1.2. For devices from Tiger family call function with userPrioModePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS                       st = GT_OK;
    GT_U8                           dev;
    GT_U8                           port;
    CPSS_COS_USER_PRIO_MODE_ENT     userPrioMode;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For each active device id go over all active virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with non-NULL userPrioModePtr.                                      */
            /* Expected: GT_OK for devices from Tiger family and GT_NOT_SUPPORTED for other devices.    */
            st = cpssExMxCosUserPrioModeDebugCmdGet(dev, port, &userPrioMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, userPrioMode);

            /* 1.1.2. For devices from Tiger family call function with userPrioModePtr [NULL].          */
            /* Expected: GT_BAD_PTR.                                                                    */
            st = cpssExMxCosUserPrioModeDebugCmdGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        port = L2_COS_VALID_VIRT_PORT_CNS;

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            st = cpssExMxCosUserPrioModeDebugCmdGet(dev, port, &userPrioMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxCosUserPrioModeDebugCmdGet(dev, port, &userPrioMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = L2_COS_VALID_VIRT_PORT_CNS;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosUserPrioModeDebugCmdGet(dev, port, &userPrioMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosUserPrioModeDebugCmdGet(dev, port, &userPrioMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosPortDefaultUserPrioSet
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_U8    defaultUserPrio
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosPortDefaultUserPrioSet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call function with defaultUserPrio [2]. Expected: GT_OK for devices from Tiger family and GT_NOT_SUPPORTED for other devices.
1.1.2. Call function cpssExMxCosPortDefaultUserPrioDebugCmdGet with non-NULL defaultUserPrioPtr. Expected: GT_OK and defaultUserPrio [2] for devices from Tiger family and GT_NOT_SUPPORTED for other devices.
1.1.3. For devices from Tiger family call function with out of range defaultUserPrio [CPSS_USER_PRIORITY_RANGE_CNS + 1]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       port;
    GT_U8       defaultUserPrio;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For each active device id go over all active virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            GT_U8                       defaultUserPrioWritten;

            /* 1.1.1. Call function with defaultUserPrio [2].                                                   */
            /* Expected: GT_OK for devices from Tiger family and GT_NOT_SUPPORTED for other devices.            */
            defaultUserPrio = 2;

            st = cpssExMxCosPortDefaultUserPrioSet(dev, port, defaultUserPrio);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, defaultUserPrio);

            /* 1.1.2. Call function cpssExMxCosPortDefaultUserPrioDebugCmdGet with non-NULL defaultUserPrioPtr. */
            /* Expected: GT_OK and defaultUserPrio [2] for devices from Tiger family                            */
            /* and GT_NOT_SUPPORTED for other devices.                                                          */
            defaultUserPrioWritten = 0;

            st = cpssExMxCosPortDefaultUserPrioDebugCmdGet(dev, port, &defaultUserPrioWritten);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxCosPortDefaultUserPrioDebugCmdGet: %d, %d, %d",
                                         dev, port, defaultUserPrioWritten);
            UTF_VERIFY_EQUAL3_PARAM_MAC(defaultUserPrio, defaultUserPrioWritten, dev, port, defaultUserPrio);

            /* 1.1.3. For devices from Tiger family call function with out of range                             */
            /* defaultUserPrio [CPSS_USER_PRIORITY_RANGE_CNS + 1].                                              */
            /* Expected: GT_BAD_PARAM.                                                                          */
            defaultUserPrio = CPSS_USER_PRIORITY_RANGE_CNS + 1;

            st = cpssExMxCosPortDefaultUserPrioSet(dev, port, defaultUserPrio);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, defaultUserPrio);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        port = L2_COS_VALID_VIRT_PORT_CNS;
        defaultUserPrio = 2;

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            st = cpssExMxCosPortDefaultUserPrioSet(dev, port, defaultUserPrio);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxCosPortDefaultUserPrioSet(dev, port, defaultUserPrio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = L2_COS_VALID_VIRT_PORT_CNS;
    defaultUserPrio = 2;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosPortDefaultUserPrioSet(dev, port, defaultUserPrio);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosPortDefaultUserPrioSet(dev, port, defaultUserPrio);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosPortDefaultUserPrioDebugCmdGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    port,
    OUT GT_U8   *defaultUserPrioPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosPortDefaultUserPrioDebugCmdGet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call function with non-NULL defaultUserPrioPtr. Expected: GT_OK for devices from Tiger family and GT_NOT_SUPPORTED for other devices.
1.1.2. For devices from Tiger family call function with defaultUserPrioPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       port;
    GT_U8       defaultUserPrio;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For each active device id go over all active virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with non-NULL defaultUserPrioPtr.                                   */
            /* Expected: GT_OK for devices from Tiger family and GT_NOT_SUPPORTED for other devices.    */
            st  = cpssExMxCosPortDefaultUserPrioDebugCmdGet(dev, port, &defaultUserPrio);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, defaultUserPrio);

            /* 1.1.2. For devices from Tiger family call function with defaultUserPrioPtr [NULL].       */
            /* Expected: GT_BAD_PTR.                                                                    */
            st  = cpssExMxCosPortDefaultUserPrioDebugCmdGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        port = L2_COS_VALID_VIRT_PORT_CNS;

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            st = cpssExMxCosPortDefaultUserPrioDebugCmdGet(dev, port, &defaultUserPrio);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxCosPortDefaultUserPrioDebugCmdGet(dev, port, &defaultUserPrio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = L2_COS_VALID_VIRT_PORT_CNS;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosPortDefaultUserPrioDebugCmdGet(dev, port, &defaultUserPrio);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosPortDefaultUserPrioDebugCmdGet(dev, port, &defaultUserPrio);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosUserPrio2DpSet
(
    IN GT_U8        dev,
    IN GT_U8        userPrio,
    IN CPSS_DP_LEVEL_ENT  dpLevel
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosUserPrio2DpSet)
{
/*
ITERATE_DEVICES
1.1. Call function with userPrio [1] and dpLevel [CPSS_DP_YELLOW_E]. Expected: GT_OK.
1.2. Call function cpssExMxCosUserPrio2DpGet with userPrio [1] and non-NULL dpLevelPtr. Expected: GT_OK and dpLevel [CPSS_DP_YELLOW].
1.3. Call function with out of range userPrio [CPSS_USER_PRIORITY_RANGE_CNS + 1]. Expected: GT_BAD_PARAM.
1.4. Call function with out of range dpLevel [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS           st = GT_OK;
    GT_U8               dev;
    GT_U8               userPrio;
    CPSS_DP_LEVEL_ENT   dpLevel;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        CPSS_DP_LEVEL_ENT   dpLevelWritten;

        /* 1.1. Call function with userPrio [1] and dpLevel [CPSS_DP_YELLOW].                       */
        /* Expected: GT_OK.                                                                         */
        userPrio = 1;
        dpLevel = CPSS_DP_YELLOW_E;

        st = cpssExMxCosUserPrio2DpSet(dev, userPrio, dpLevel);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, userPrio, dpLevel);

        /* 1.2. Call function cpssExMxCosUserPrio2DpGet with userPrio [1] and non-NULL dpLevelPtr.  */
        /* Expected: GT_OK and dpLevel [CPSS_DP_YELLOW].                                            */
        dpLevelWritten = 0;

        st = cpssExMxCosUserPrio2DpGet(dev, userPrio, &dpLevelWritten);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxCosUserPrio2DpGet: %d, %d, %d",
                                     dev, userPrio, dpLevelWritten);
        UTF_VERIFY_EQUAL3_PARAM_MAC(dpLevel, dpLevelWritten, dev, userPrio, dpLevel);

        /* 1.3. Call function with out of range userPrio [CPSS_USER_PRIORITY_RANGE_CNS + 1].        */
        /* Expected: GT_BAD_PARAM.                                                                  */
        userPrio = CPSS_USER_PRIORITY_RANGE_CNS + 1;

        st = cpssExMxCosUserPrio2DpSet(dev, userPrio, dpLevel);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, userPrio, dpLevel);

        /* 1.4. Call function with out of range dpLevel [0x5AAAAAA5].                               */
        /* Expected: GT_BAD_PARAM.                                                                  */
        userPrio = 1;
        dpLevel = L2_COS_INVALID_ENUM_CNS;

        st = cpssExMxCosUserPrio2DpSet(dev, userPrio, dpLevel);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, userPrio, dpLevel);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    userPrio = 1;
    dpLevel = CPSS_DP_YELLOW_E;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosUserPrio2DpSet(dev, userPrio, dpLevel);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosUserPrio2DpSet(dev, userPrio, dpLevel);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosUserPrio2DpGet
(
    IN  GT_U8              dev,
    IN  GT_U8              userPrio,
    OUT CPSS_DP_LEVEL_ENT *dpLevelPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosUserPrio2DpGet)
{
/*
ITERATE_DEVICES
1.1. Call function with userPrio [1] and non-NULL dpLevelPtr. Expected: GT_OK.
1.2. Call function with out of range userPrio [CPSS_USER_PRIORITY_RANGE_CNS + 1]. Expected: GT_BAD_PARAM.
1.3. Call function with dpLevelPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS           st = GT_OK;
    GT_U8               dev;
    GT_U8               userPrio;
    CPSS_DP_LEVEL_ENT   dpLevel;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with userPrio [1] and non-NULL dpLevelPtr.                        */
        /* Expected: GT_OK.                                                                     */
        userPrio = 1;

        st = cpssExMxCosUserPrio2DpGet(dev, userPrio, &dpLevel);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, userPrio, dpLevel);

        /* 1.2. Call function with out of range userPrio [CPSS_USER_PRIORITY_RANGE_CNS + 1].    */
        /* Expected: GT_BAD_PARAM.                                                              */
        userPrio = CPSS_USER_PRIORITY_RANGE_CNS + 1;

        st = cpssExMxCosUserPrio2DpGet(dev, userPrio, &dpLevel);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, userPrio, dpLevel);

        /* 1.3. Call function with dpLevelPtr [NULL].                                           */
        /* Expected: GT_BAD_PTR.                                                                */
        userPrio = 1;

        st = cpssExMxCosUserPrio2DpGet(dev, userPrio, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, userPrio);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    userPrio = 1;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosUserPrio2DpGet(dev, userPrio, &dpLevel);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosUserPrio2DpGet(dev, userPrio, &dpLevel);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosMacEntryTcOverrideSet
(
    IN GT_U8    devNum,
    IN CPSS_COS_MAC_ADDR_TYPE_ENT  macEntryTcOverrideEnable
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosMacEntryTcOverrideSet)
{
/*
ITERATE_DEVICES
1.1. Call function with macEntryTcOverrideEnable [CPSS_COS_MAC_ADDR_TYPE_ALL_E]. Expected: GT_OK.
1.2. Call function with macEntryTcOverrideEnable [CPSS_COS_MAC_ADDR_TYPE_SA_ONLY_E and CPSS_COS_MAC_ADDR_TYPE_DA_ONLY_E]. Expected: NON GT_OK.
1.3. Call function with out of bound macEntryTcOverrideEnable [0x5AAAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    CPSS_COS_MAC_ADDR_TYPE_ENT  macEntryTcOverrideEnable;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with macEntryTcOverrideEnable [CPSS_COS_MAC_ADDR_TYPE_ALL_E].     */
        /* Expected: GT_OK.                                                                     */
        macEntryTcOverrideEnable = CPSS_COS_MAC_ADDR_TYPE_ALL_E;

        st = cpssExMxCosMacEntryTcOverrideSet(dev, macEntryTcOverrideEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, macEntryTcOverrideEnable);

        /* 1.2. Call function with macEntryTcOverrideEnable [CPSS_COS_MAC_ADDR_TYPE_SA_ONLY_E   */
        /* and CPSS_COS_MAC_ADDR_TYPE_DA_ONLY_E].                                               */
        /* Expected: NON GT_OK.                                                                 */
        macEntryTcOverrideEnable = CPSS_COS_MAC_ADDR_TYPE_SA_ONLY_E;

        st = cpssExMxCosMacEntryTcOverrideSet(dev, macEntryTcOverrideEnable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, macEntryTcOverrideEnable);

        macEntryTcOverrideEnable = CPSS_COS_MAC_ADDR_TYPE_DA_ONLY_E;

        st = cpssExMxCosMacEntryTcOverrideSet(dev, macEntryTcOverrideEnable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, macEntryTcOverrideEnable);

        /* 1.3. Call function with out of bound macEntryTcOverrideEnable [0x5AAAAAAA5].         */
        /* Expected: GT_BAD_PARAM.                                                              */
        macEntryTcOverrideEnable = L2_COS_INVALID_ENUM_CNS;

        st = cpssExMxCosMacEntryTcOverrideSet(dev, macEntryTcOverrideEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, macEntryTcOverrideEnable);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    macEntryTcOverrideEnable = CPSS_COS_MAC_ADDR_TYPE_ALL_E;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosMacEntryTcOverrideSet(dev, macEntryTcOverrideEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosMacEntryTcOverrideSet(dev, macEntryTcOverrideEnable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosPortOverrideTcEnable
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    IN  GT_BOOL   state
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosPortOverrideTcEnable)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call function with state [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     state;


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
            /* 1.1.1. Call function with state [GT_TRUE and GT_FALSE].  */
            /* Expected: GT_OK.                                         */
            state = GT_FALSE;

            st = cpssExMxCosPortOverrideTcEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            state = GT_TRUE;

            st = cpssExMxCosPortOverrideTcEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        port = L2_COS_VALID_VIRT_PORT_CNS;
        state = GT_TRUE;

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            st = cpssExMxCosPortOverrideTcEnable(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxCosPortOverrideTcEnable(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = L2_COS_VALID_VIRT_PORT_CNS;
    state = GT_TRUE;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosPortOverrideTcEnable(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosPortOverrideTcEnable(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/* function cpssExMxCosMap8To4TcSet moved to VB library !!!
   and changed name to cpssExMxVb8TcTo4TcMapSet
*/
#if 0

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosMap8To4TcSet
(
    IN GT_U8                    dev,
    IN GT_U8                    value8Tc,
    IN GT_U8                    value4Tc
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosMap8To4TcSet)
{
/*
ITERATE_DEVICES
1.1. Call function with value8Tc [5] and value4Tc [2]. Expected: GT_OK for Tiger family device and GT_NOT_SUPPORTED for other devices.
1.2. For Tiger family device call function with out of range value8Tc [CPSS_TC_RANGE_CNS]. Expected: GT_BAD_PARAM.
1.3. For Tiger family device call function with out of range value4Tc [CPSS_4_TC_RANGE_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       value8Tc;
    GT_U8       value4Tc;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        CPSS_PP_FAMILY_TYPE_ENT     devFamily;

        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. Call function with value8Tc [5] and value4Tc [2].                                           */
        /* Expected: GT_OK for Tiger family device and GT_NOT_SUPPORTED for other devices.                  */
        value8Tc = 5;
        value4Tc = 2;

        st = cpssExMxCosMap8To4TcSet(dev, value8Tc, value4Tc);
        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, value8Tc, value4Tc);
        }
        else
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, value8Tc, value4Tc);
        }

        /* 1.2. For Tiger family device call function with out of range value8Tc [CPSS_TC_RANGE_CNS].       */
        /* Expected: GT_BAD_PARAM.                                                                          */
        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            value8Tc = CPSS_TC_RANGE_CNS;
            value4Tc = 2;

            st = cpssExMxCosMap8To4TcSet(dev, value8Tc, value4Tc);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, value8Tc, value4Tc);
        }

        /* 1.3. For Tiger family device call function with out of range value4Tc [CPSS_4_TC_RANGE_CNS].     */
        /* Expected: GT_BAD_PARAM.                                                                          */
        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            value8Tc = 5;
            value4Tc = CPSS_4_TC_RANGE_CNS;

            st = cpssExMxCosMap8To4TcSet(dev, value8Tc, value4Tc);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, value8Tc, value4Tc);
        }
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    value8Tc = 5;
    value4Tc = 2;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosMap8To4TcSet(dev, value8Tc, value4Tc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosMap8To4TcSet(dev, value8Tc, value4Tc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

#endif /* 0 */


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssExMxL2Cos suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxL2Cos)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosPortDefaultTcSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosPortDefaultTcGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosUserPrio2TcSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosUserPrio2TcGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosTc2UserPrioSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosTc2UserPrioGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosUserPrioModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosUserPrioModeDebugCmdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosPortDefaultUserPrioSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosPortDefaultUserPrioDebugCmdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosUserPrio2DpSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosUserPrio2DpGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosMacEntryTcOverrideSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosPortOverrideTcEnable)
/*    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosMap8To4TcSet)*/
UTF_SUIT_END_TESTS_MAC(cpssExMxL2Cos)
