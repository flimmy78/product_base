/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxSalDscp2CosUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxSalDscp2Cos, that provides
*       cpss Dx-Sal implementation for DSCP Class-Of-Service configuration.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/dxSal/cos/cpssDxSalDscp2Cos.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define COS_VALID_VIRT_PORT_CNS 0

/* Invalid enum */
#define COS_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalDscpToCosSet
(
    IN GT_U8        devNum,
    IN GT_U8        dscp,
    IN CPSS_COS_STC *cosPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalDscpToCosSet)
{
/*
    ITERATE_DEVICES 
    1.1. Call function with dscp [0 / 31 / CPSS_DSCP_RANGE_CNS-1 = 63],
                            cosPtr {dropPrecedence [CPSS_DP_GREEN_E  / CPSS_DP_RED_E],
                                    userPriority [0/ CPSS_USER_PRIORITY_RANGE_CNS-1 = 7],
                                    trafficClass [0 / CPSS_4_TC_RANGE_CNS-1 = 3]}.
    Expected: GT_OK.
    1.2. Call cpssDxSalDscpToCosGet with dscp [0],
                                    non-null cosPtr.
    Expected: GT_OK and the same values as written.
    1.3. Call function with out of range dscp [CPSS_DSCP_RANGE_CNS = 64],
                            other params same as in 1.1.
    Expected: non GT_OK.
    1.4. Call function with out of range cosPtr->dropPrecedence [0x5AAAAAA5],
                            other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call function with out of range cosPtr->userPriority [CPSS_USER_PRIORITY_RANGE_CNS = 8],
                            other params same as in 1.1.
    Expected: non GT_OK.
    1.6. Call function with out of range cosPtr->trafficClass [CPSS_4_TC_RANGE_CNS = 4],
                            other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call function with dscp [0]
                            and cosPtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS    st   = GT_OK;

    GT_U8        dev;
    GT_U8        dscp = 0;
    CPSS_COS_STC cos;
    CPSS_COS_STC cosGet;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with dscp [0 / 31 / CPSS_DSCP_RANGE_CNS-1 = 63],
                                    cosPtr [dropPrecedence [CPSS_DP_GREEN_E  / CPSS_DP_RED_E],
                                            userPriority [0/ CPSS_USER_PRIORITY_RANGE_CNS-1 = 7],
                                            trafficClass [0 / CPSS_4_TC_RANGE_CNS-1 = 3] ].
            Expected: GT_OK.
        */

        /* Call with dscp[0], cosPtr{dropPrecedence[CPSS_DP_GREEN_E], userPriority[0], trafficClass[0]} */
        dscp = 0;

        cos.dropPrecedence = CPSS_DP_GREEN_E;
        cos.userPriority   = 0;
        cos.trafficClass   = 0;
        
        st = cpssDxSalDscpToCosSet(dev, dscp, &cos);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);

        /* Call with dscp[31], cosPtr{dropPrecedence[CPSS_DP_RED_E], userPriority[CPSS_USER_PRIORITY_RANGE_CNS - 1] */
        /* trafficClass[CPSS_4_TC_RANGE_CNS - 1]}                                                                   */
        dscp = 31;

        cos.dropPrecedence = CPSS_DP_RED_E;
        cos.userPriority   = CPSS_USER_PRIORITY_RANGE_CNS - 1;
        cos.trafficClass   = CPSS_4_TC_RANGE_CNS - 1;
        
        st = cpssDxSalDscpToCosSet(dev, dscp, &cos);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);

        /* Call with dscp[CPSS_DSCP_RANGE_CNS-1], cosPtr{dropPrecedence[CPSS_DP_GREEN_E], userPriority[0], trafficClass[0]} */
        dscp = CPSS_DSCP_RANGE_CNS - 1;

        cos.dropPrecedence = CPSS_DP_GREEN_E;
        cos.userPriority   = 0;
        cos.trafficClass   = 0;
        
        st = cpssDxSalDscpToCosSet(dev, dscp, &cos);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);

        /*
            1.2. Call cpssDxSalDscpToCosGet with dscp [0],
                                            non-null cosPtr.
            Expected: GT_OK and the same values as written.
        */
        dscp = 0;

        st = cpssDxSalDscpToCosGet(dev, dscp, &cosGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssDxSalDscpToCosGet: %d, %d", dev, dscp);
        
        /* Checking cos == cosGet */
        UTF_VERIFY_EQUAL1_STRING_MAC(cos.dropPrecedence, cosGet.dropPrecedence,
                       "get another cosPtr->dropPrecedence than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cos.userPriority, cosGet.userPriority,
                       "get another cosPtr->userPriority than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cos.trafficClass, cosGet.trafficClass,
                       "get another cosPtr->trafficClass than was set: %d", dev);
        
        /* 
            1.3. Call function with out of range dscp [CPSS_DSCP_RANGE_CNS = 64],
                                    other params same as in 1.1.
            Expected: non GT_OK.
        */
        dscp = CPSS_DSCP_RANGE_CNS;

        st = cpssDxSalDscpToCosSet(dev, dscp, &cos);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);

        dscp = 0;
        
        /* 
            1.4. Call function with out of range cosPtr->dropPrecedence [0x5AAAAAA5],
                                    other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        cos.dropPrecedence = COS_INVALID_ENUM_CNS;

        st = cpssDxSalDscpToCosSet(dev, dscp, &cos);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, cos->dropPrecedence = %d", dev, cos.dropPrecedence);

        cos.dropPrecedence = CPSS_DP_GREEN_E;

        /*
            1.5. Call function with out of range cosPtr->userPriority [CPSS_USER_PRIORITY_RANGE_CNS = 8],
                                    other params same as in 1.1.
            Expected: non GT_OK.
        */
        cos.userPriority = CPSS_USER_PRIORITY_RANGE_CNS;
        
        st = cpssDxSalDscpToCosSet(dev, dscp, &cos);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cosPtr->userPriority = %d", dev, cos.userPriority);

        cos.userPriority   = 0;

        /*
            1.6. Call function with out of range cosPtr->trafficClass [CPSS_4_TC_RANGE_CNS = 4],
                                    other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        cos.trafficClass = CPSS_4_TC_RANGE_CNS;

        st = cpssDxSalDscpToCosSet(dev, dscp, &cos);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, cos->trafficClass = %d", dev, cos.trafficClass);

        cos.trafficClass = 0;

        /*
            1.7. Call function with dscp [0]
                                    and cosPtr[NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalDscpToCosSet(dev, dscp, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, cosPtr = NULL", dev, dscp);
    }

    dscp = 0;

    cos.dropPrecedence = CPSS_DP_GREEN_E;
    cos.userPriority   = 0;
    cos.trafficClass   = 0;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalDscpToCosSet(dev, dscp, &cos);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalDscpToCosSet(dev, dscp, &cos);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalDscpToCosGet
(
    IN  GT_U8         dev,
    IN  GT_U8         dscp,
    OUT CPSS_COS_STC *cosPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalDscpToCosGet)
{
/*
    ITERATE_DEVICES 
    1.1. Call function with dscp [0 / 31 / CPSS_DSCP_RANGE_CNS-1 = 63],
                            non-NULL cosPtr.
    Expected: GT_OK.
    1.2. Call function with out of range dscp [CPSS_DSCP_RANGE_CNS = 64],
                            other params same as in 1.1.
    Expected: non GT_OK.
    1.3. Call function with cosPtr [NULL],
                            other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS    st   = GT_OK;

    GT_U8        dev;
    GT_U8        dscp = 0;
    CPSS_COS_STC cos;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with dscp [0 / 31 / CPSS_DSCP_RANGE_CNS-1 = 63],
                                    non-NULL cosPtr.
            Expected: GT_OK.
        */

        /* Call with dscp[0] */
        dscp = 0;

        st = cpssDxSalDscpToCosGet(dev, dscp, &cos);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);

        /* Call with dscp[31] */
        dscp = 31;
       
        st = cpssDxSalDscpToCosGet(dev, dscp, &cos);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);

        /* Call with dscp[CPSS_DSCP_RANGE_CNS-1] */
        dscp = CPSS_DSCP_RANGE_CNS - 1;

        st = cpssDxSalDscpToCosGet(dev, dscp, &cos);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);

        /* 
            1.2. Call function with out of range dscp [CPSS_DSCP_RANGE_CNS = 64],
                                    other params same as in 1.1.
            Expected: non GT_OK.
        */
        dscp = CPSS_DSCP_RANGE_CNS;

        st = cpssDxSalDscpToCosGet(dev, dscp, &cos);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);

        dscp = 0;
        
        /*
            1.3. Call function with cosPtr [NULL],
                                    other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalDscpToCosGet(dev, dscp, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, cosPtr = NULL", dev, dscp);
    }

    dscp = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalDscpToCosGet(dev, dscp, &cos);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalDscpToCosGet(dev, dscp, &cos);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalSetPortDscpMapEnable
(
    IN GT_U8    dev,
    IN GT_U8    port,
    IN GT_BOOL  mapDscp2TcPortEnable,
    IN GT_BOOL  mapDscp2UpPortEnable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalSetPortDscpMapEnable)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS
    1.1.1. Call with mapDscp2TcPortEnable [GT_FALSE / GT_TRUE],
                     mapDscp2UpPortEnable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxSalPortDscpMapGet with non-NULL mapDscp2TcPortEnable
                                             and mapDscp2UpPortEnable.
    Expected: GT_OK and same values as written.
*/
    GT_STATUS st = GT_OK;

    GT_U8     dev; 
    GT_U8     port               = COS_VALID_VIRT_PORT_CNS;
    GT_BOOL   mapTcPortEnable    = GT_FALSE;
    GT_BOOL   mapUpPortEnable    = GT_FALSE;
    GT_BOOL   mapTcPortEnableGet = GT_FALSE;
    GT_BOOL   mapUpPortEnableGet = GT_FALSE;
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with mapDscp2TcPortEnable [GT_FALSE / GT_TRUE],
                                 mapDscp2UpPortEnable [GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */

            /* mapDscp2TcPortEnable = GT_FALSE, mapDscp2UpPortEnable = GT_FALSE */
            mapTcPortEnable = GT_FALSE;
            mapUpPortEnable = GT_FALSE;

            st = cpssDxSalSetPortDscpMapEnable(dev, port, mapTcPortEnable, mapUpPortEnable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, mapTcPortEnable, mapUpPortEnable);

            /*
                1.1.2. Call cpssDxSalPortDscpMapGet with non-NULL mapDscp2TcPortEnable
                                                         and mapDscp2UpPortEnable.
                Expected: GT_OK and same values as written.
            */
            st = cpssDxSalPortDscpMapGet(dev, port, &mapTcPortEnableGet, &mapUpPortEnableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalPortDscpMapGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(mapTcPortEnable, mapTcPortEnableGet,
                       "get another mapDscp2TcPortEnable than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(mapUpPortEnable, mapUpPortEnableGet,
                       "get another mapDscp2UpPortEnable than was set: %d, %d", dev, port);


            /* mapDscp2TcPortEnable = GT_FALSE, mapDscp2UpPortEnable = GT_TRUE */
            mapTcPortEnable = GT_FALSE;
            mapUpPortEnable = GT_TRUE;

            st = cpssDxSalSetPortDscpMapEnable(dev, port, mapTcPortEnable, mapUpPortEnable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, mapTcPortEnable, mapUpPortEnable);

            /*
                1.1.2. Call cpssDxSalPortDscpMapGet with non-NULL mapDscp2TcPortEnable
                                                         and mapDscp2UpPortEnable.
                Expected: GT_OK and same values as written.
            */
            st = cpssDxSalPortDscpMapGet(dev, port, &mapTcPortEnableGet, &mapUpPortEnableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalPortDscpMapGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(mapTcPortEnable, mapTcPortEnableGet,
                       "get another mapDscp2TcPortEnable than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(mapUpPortEnable, mapUpPortEnableGet,
                       "get another mapDscp2UpPortEnable than was set: %d, %d", dev, port);


            /* mapDscp2TcPortEnable = GT_TRUE, mapDscp2UpPortEnable = GT_FALSE */
            mapTcPortEnable = GT_TRUE;
            mapUpPortEnable = GT_FALSE;

            st = cpssDxSalSetPortDscpMapEnable(dev, port, mapTcPortEnable, mapUpPortEnable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, mapTcPortEnable, mapUpPortEnable);

            /*
                1.1.2. Call cpssDxSalPortDscpMapGet with non-NULL mapDscp2TcPortEnable
                                                         and mapDscp2UpPortEnable.
                Expected: GT_OK and same values as written.
            */
            st = cpssDxSalPortDscpMapGet(dev, port, &mapTcPortEnableGet, &mapUpPortEnableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalPortDscpMapGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(mapTcPortEnable, mapTcPortEnableGet,
                       "get another mapDscp2TcPortEnable than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(mapUpPortEnable, mapUpPortEnableGet,
                       "get another mapDscp2UpPortEnable than was set: %d, %d", dev, port);


            /* mapDscp2TcPortEnable = GT_TRUE, mapDscp2UpPortEnable = GT_TRUE */
            mapTcPortEnable = GT_TRUE;
            mapUpPortEnable = GT_TRUE;

            st = cpssDxSalSetPortDscpMapEnable(dev, port, mapTcPortEnable, mapUpPortEnable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, mapTcPortEnable, mapUpPortEnable);

            /*
                1.1.2. Call cpssDxSalPortDscpMapGet with non-NULL mapDscp2TcPortEnable
                                                         and mapDscp2UpPortEnable.
                Expected: GT_OK and same values as written.
            */
            st = cpssDxSalPortDscpMapGet(dev, port, &mapTcPortEnableGet, &mapUpPortEnableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalPortDscpMapGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(mapTcPortEnable, mapTcPortEnableGet,
                       "get another mapDscp2TcPortEnable than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(mapUpPortEnable, mapUpPortEnableGet,
                       "get another mapDscp2UpPortEnable than was set: %d, %d", dev, port);
        }

        mapTcPortEnable = GT_TRUE;
        mapUpPortEnable = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalSetPortDscpMapEnable(dev, port, mapTcPortEnable, mapUpPortEnable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalSetPortDscpMapEnable(dev, port, mapTcPortEnable, mapUpPortEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalSetPortDscpMapEnable(dev, port, mapTcPortEnable, mapUpPortEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    mapTcPortEnable = GT_TRUE;
    mapUpPortEnable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = COS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalSetPortDscpMapEnable(dev, port, mapTcPortEnable, mapUpPortEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssDxSalSetPortDscpMapEnable(dev, port, mapTcPortEnable, mapUpPortEnable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortDscpMapGet
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    OUT GT_BOOL  *mapDscp2TcPortEnable,
    OUT GT_BOOL  *mapDscp2UpPortEnable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortDscpMapGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS
    1.1.1. Call with non-NULL mapDscp2TcPortEnable
                     and mapDscp2UpPortEnable.
    Expected: GT_OK.
    1.1.2. Call with mapDscp2TcPortEnable [NULL],
                     other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.1.3. Call with mapDscp2UpPortEnable [NULL],
                     other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8     dev; 
    GT_U8     port            = COS_VALID_VIRT_PORT_CNS;
    GT_BOOL   mapTcPortEnable = GT_FALSE;
    GT_BOOL   mapUpPortEnable = GT_FALSE;
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with non-NULL mapDscp2TcPortEnable
                                 and mapDscp2UpPortEnable.
                Expected: GT_OK.
            */
            st = cpssDxSalPortDscpMapGet(dev, port, &mapTcPortEnable, &mapUpPortEnable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with mapDscp2TcPortEnable [NULL],
                                 other params same as in 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxSalPortDscpMapGet(dev, port, NULL, &mapUpPortEnable);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, mapDscp2TcPortEnable = NULL", dev, port);

            /*
                1.1.3. Call with mapDscp2UpPortEnable [NULL],
                                 other params same as in 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxSalPortDscpMapGet(dev, port, &mapTcPortEnable, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, mapDscp2UpPortEnable = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalPortDscpMapGet(dev, port, &mapTcPortEnable, &mapUpPortEnable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortDscpMapGet(dev, port, &mapTcPortEnable, &mapUpPortEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalPortDscpMapGet(dev, port, &mapTcPortEnable, &mapUpPortEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = COS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortDscpMapGet(dev, port, &mapTcPortEnable, &mapUpPortEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssDxSalPortDscpMapGet(dev, port, &mapTcPortEnable, &mapUpPortEnable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssDxSalDscp2Cos suit
*/
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxSalDscp2Cos)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalDscpToCosSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalDscpToCosGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalSetPortDscpMapEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortDscpMapGet)
UTF_SUIT_END_TESTS_MAC(cpssDxSalDscp2Cos)

