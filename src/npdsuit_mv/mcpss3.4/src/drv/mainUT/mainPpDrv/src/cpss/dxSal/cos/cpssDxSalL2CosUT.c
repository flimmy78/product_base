/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxSalL2CosUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxSalL2Cos, that provides
*       cpss Dx-Ch implementation for L2 bridging Class-Of-Service.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/dxSal/cos/cpssDxSalL2Cos.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for physical port id */
#define COS_VALID_PHY_PORT_CNS  0

/* Default valid value for virtual port id */
#define COS_VALID_VIRT_PORT_CNS 0

/* Invalid enum */
#define COS_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortDefaultTcSet
(
    IN GT_U8    dev,
    IN GT_U8    port,
    IN GT_U8    trfClass
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortDefaultTcSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS
    1.1.1. Call function with trfClass [0 / CPSS_4_TC_RANGE_CNS-1 = 3].
    Expected: GT_OK.
    1.1.2. Call function cpssDxSalPortDefaultTcGet with non-NULL trfClassPtr.
    Expected: GT_OK and same value as written.
    1.1.3. Call function with out of range trfClass [CPSS_4_TC_RANGE_CNS = 4],
                              other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8     dev; 
    GT_U8     port        = COS_VALID_PHY_PORT_CNS;
    GT_U8     trfClass    = 0;
    GT_U8     trfClassGet = 0;
    
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call function with trfClass [0 / CPSS_4_TC_RANGE_CNS-1 = 3].
                Expected: GT_OK.
            */

            /* Call with trfClass = 0 */
            trfClass = 0;

            st = cpssDxSalPortDefaultTcSet(dev, port, trfClass);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trfClass);

            /*
                1.1.2. Call function cpssDxSalPortDefaultTcGet with non-NULL trfClassPtr.
                Expected: GT_OK and same value as written.
            */
            st = cpssDxSalPortDefaultTcGet(dev, port, &trfClassGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalPortDefaultTcGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(trfClass, trfClassGet,
                       "get another trfClass than was set: %d, %d", dev, port);
            
            /* Call with trfClass = CPSS_4_TC_RANGE_CNS - 1 */
            trfClass = CPSS_4_TC_RANGE_CNS - 1;

            st = cpssDxSalPortDefaultTcSet(dev, port, trfClass);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trfClass);

            /*
                1.1.2. Call function cpssDxSalPortDefaultTcGet with non-NULL trfClassPtr.
                Expected: GT_OK and same value as written.
            */
            st = cpssDxSalPortDefaultTcGet(dev, port, &trfClassGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalPortDefaultTcGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(trfClass, trfClassGet,
                       "get another trfClass than was set: %d, %d", dev, port);

            /*
                1.1.3. Call function with out of range trfClass [CPSS_4_TC_RANGE_CNS = 4],
                                          other params same as in 1.1.
                Expected: GT_BAD_PARAM.
            */
            trfClass = CPSS_4_TC_RANGE_CNS;

            st = cpssDxSalPortDefaultTcSet(dev, port, trfClass);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, trfClass);

            trfClass = 0;
        }

        trfClass = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalPortDefaultTcSet(dev, port, trfClass);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortDefaultTcSet(dev, port, trfClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalPortDefaultTcSet(dev, port, trfClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    trfClass = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = COS_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortDefaultTcSet(dev, port, trfClass);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssDxSalPortDefaultTcSet(dev, port, trfClass);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortDefaultTcGet
(
    IN  GT_U8   dev,
    IN  GT_U8   port,
    OUT GT_U8   *trfClassPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortDefaultTcGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS
    1.1.1. Call function with non-NULL trfClassPtr.
    Expected: GT_OK.
    1.1.2. Call function trfClassPtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8     dev; 
    GT_U8     port        = COS_VALID_PHY_PORT_CNS;
    GT_U8     trfClass    = 0;
   
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call function with non-NULL trfClassPtr.
                Expected: GT_OK.
            */
            st = cpssDxSalPortDefaultTcGet(dev, port, &trfClass);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call function trfClassPtr[NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxSalPortDefaultTcGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, trfClassPtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalPortDefaultTcGet(dev, port, &trfClass);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortDefaultTcGet(dev, port, &trfClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalPortDefaultTcGet(dev, port, &trfClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    trfClass = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = COS_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortDefaultTcGet(dev, port, &trfClass);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssDxSalPortDefaultTcGet(dev, port, &trfClass);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalUserPrio2TcSet
(
    IN GT_U8    dev,
    IN GT_U8    up2TcTableIdx,
    IN GT_U8    userPrio,
    IN GT_U8    trfClass
)
*/
UTF_TEST_CASE_MAC(cpssDxSalUserPrio2TcSet)
{
/*
    ITERATE_DEVICES 
    1.1. Call function with up2TcTableIdx [0 / 1],
                            userPrio [0 / 7],
                            trfClass [0 / 3].
    Expected: GT_OK.
    1.2. Call cpssDxSalUserPrio2TcGet with non-null trfClassPtr, other params same as in 1.1.
    Expected: GT_OK and the same value as written.
    1.3. Call function with out of range up2TcTableIdx [2],
                            other params same as in 1.1.
    Expected: non GT_OK.
    1.4. Call function with out of range userPrio [CPSS_USER_PRIORITY_RANGE_CNS = 8],
                            other params same as in 1.1.
    Expected: non GT_OK.
    1.5. Call function with out of range trfClass [CPSS_4_TC_RANGE_CNS = 4],
                            other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st   = GT_OK;

    GT_U8     dev;
    GT_U8     up2TcTableIdx = 0;
    GT_U8     userPrio      = 0;
    GT_U8     trfClass      = 0;
    GT_U8     trfClassGet   = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with up2TcTableIdx [0 / 1],
                                    userPrio [0 / 7],
                                    trfClass [0 / 3].
            Expected: GT_OK.
        */

        /* Call with up2TcTableIdx[0], userPrio[0], trfClass[0] */
        up2TcTableIdx = 0;
        userPrio      = 0;
        trfClass      = 0;
        
        st = cpssDxSalUserPrio2TcSet(dev, up2TcTableIdx, userPrio, trfClass);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, up2TcTableIdx, userPrio, trfClass);

        /*
            1.2. Call cpssDxSalUserPrio2TcGet with non-null trfClassPtr, other params same as in 1.1.
            Expected: GT_OK and the same value as written.
        */
        st = cpssDxSalUserPrio2TcGet(dev, up2TcTableIdx, userPrio, &trfClassGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssDxSalUserPrio2TcGet: %d, %d, %d", dev, up2TcTableIdx, userPrio);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(trfClass, trfClassGet,
                       "get another trfClass than was set: %d", dev);

        /* Call with up2TcTableIdx[1], userPrio[7], trfClass[3] */
        up2TcTableIdx = 1;
        userPrio      = 7;
        trfClass      = 3;
        
        st = cpssDxSalUserPrio2TcSet(dev, up2TcTableIdx, userPrio, trfClass);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, up2TcTableIdx, userPrio, trfClass);

        /*
            1.2. Call cpssDxSalUserPrio2TcGet with non-null trfClassPtr, other params same as in 1.1.
            Expected: GT_OK and the same value as written.
        */
        st = cpssDxSalUserPrio2TcGet(dev, up2TcTableIdx, userPrio, &trfClassGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssDxSalUserPrio2TcGet: %d, %d, %d", dev, up2TcTableIdx, userPrio);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(trfClass, trfClassGet,
                       "get another trfClass than was set: %d", dev);

        /* 
            1.3. Call function with out of range up2TcTableIdx [2],
                                    other params same as in 1.1.
            Expected: non GT_OK.
        */
        up2TcTableIdx = 2;
        userPrio      = 0;
        trfClass      = 0;

        st = cpssDxSalUserPrio2TcSet(dev, up2TcTableIdx, userPrio, trfClass);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, up2TcTableIdx);

        up2TcTableIdx = 0;
        
        /* 
            1.4. Call function with out of range userPrio [CPSS_USER_PRIORITY_RANGE_CNS = 8],
                                    other params same as in 1.1.
            Expected: non GT_OK.
        */
        userPrio = CPSS_USER_PRIORITY_RANGE_CNS;

        st = cpssDxSalUserPrio2TcSet(dev, up2TcTableIdx, userPrio, trfClass);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, userPrio = %d", dev, userPrio);

        userPrio = 0;

        /* 
            1.5. Call function with out of range trfClass [CPSS_4_TC_RANGE_CNS = 4],
                                    other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        trfClass = CPSS_4_TC_RANGE_CNS;

        st = cpssDxSalUserPrio2TcSet(dev, up2TcTableIdx, userPrio, trfClass);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, trfClass = %d", dev, trfClass);

        trfClass = 0;
    }

    up2TcTableIdx = 0;
    userPrio      = 0;
    trfClass      = 0;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalUserPrio2TcSet(dev, up2TcTableIdx, userPrio, trfClass);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalUserPrio2TcSet(dev, up2TcTableIdx, userPrio, trfClass);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalUserPrio2TcGet
(
    IN  GT_U8   dev,
    IN  GT_U8   up2TcTableIdx,
    IN  GT_U8   userPrio,
    OUT GT_U8   *trfClassPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalUserPrio2TcGet)
{
/*
    ITERATE_DEVICES 
    1.1. Call function with up2TcTableIdx [0 / 1],
                       userPrio [0 / 7],
                       non-NULL trfClass.
    Expected: GT_OK.
    1.2. Call function with out of range up2TcTableIdx [2],
                            other params same as in 1.1.
    Expected: non GT_OK.
    1.3. Call function with out of range userPrio [CPSS_USER_PRIORITY_RANGE_CNS = 8],
                            other params same as in 1.1.
    Expected: non GT_OK.
    1.4. Call function with trfClassPtr [NULL],
                            other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st   = GT_OK;

    GT_U8     dev;
    GT_U8     up2TcTableIdx = 0;
    GT_U8     userPrio      = 0;
    GT_U8     trfClass      = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with up2TcTableIdx [0 / 1],
                                    userPrio [0 / 7],
                                    non-NULL trfClass.
            Expected: GT_OK.
        */

        /* Call with up2TcTableIdx[0], userPrio[0] */
        up2TcTableIdx = 0;
        userPrio      = 0;
        
        st = cpssDxSalUserPrio2TcGet(dev, up2TcTableIdx, userPrio, &trfClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, up2TcTableIdx, userPrio);

        /* Call with up2TcTableIdx[1], userPrio[7] */
        up2TcTableIdx = 1;
        userPrio      = 7;
        
        st = cpssDxSalUserPrio2TcGet(dev, up2TcTableIdx, userPrio, &trfClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, up2TcTableIdx, userPrio);

        /* 
            1.2. Call function with out of range up2TcTableIdx [2],
                                    other params same as in 1.1.
            Expected: non GT_OK.
        */
        up2TcTableIdx = 2;
        userPrio      = 0;

        st = cpssDxSalUserPrio2TcGet(dev, up2TcTableIdx, userPrio, &trfClass);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, up2TcTableIdx);

        up2TcTableIdx = 0;
        
        /* 
            1.3. Call function with out of range userPrio [CPSS_USER_PRIORITY_RANGE_CNS = 8],
                                    other params same as in 1.1.
            Expected: non GT_OK.
        */
        userPrio = CPSS_USER_PRIORITY_RANGE_CNS;

        st = cpssDxSalUserPrio2TcGet(dev, up2TcTableIdx, userPrio, &trfClass);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, userPrio = %d", dev, userPrio);

        userPrio = 0;

        /* 
            1.4. Call function with trfClassPtr [NULL],
                                    other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalUserPrio2TcGet(dev, up2TcTableIdx, userPrio, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, trfClassPtr = NULL", dev);
    }

    up2TcTableIdx = 0;
    userPrio      = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalUserPrio2TcGet(dev, up2TcTableIdx, userPrio, &trfClass);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalUserPrio2TcGet(dev, up2TcTableIdx, userPrio, &trfClass);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortUp2TcTableIdxSet
(
    IN GT_U8    dev,
    IN GT_U8    port,
    IN GT_U8    up2TcTableIdx
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortUp2TcTableIdxSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS
    1.1.1. Call with up2TcTableIdx [0 / 1].
    Expected: GT_OK.
    1.1.2. Call with cpssDxSalPortUp2TcTableIdxGet with non-NULL up2TcTableIdxPtr.
    Expected: GT_OK and same values as written.
    1.1.3. Call with out of range up2TcTableIdx [2].
    Expected: non GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8     dev; 
    GT_U8     port      = COS_VALID_VIRT_PORT_CNS;
    GT_U8     up2Idx    = 0;
    GT_U8     up2IdxGet = 0;
    
    
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
                1.1.1. Call with up2TcTableIdx [0 / 1].
                Expected: GT_OK.
            */

            /* Call with up2TcTableIdx = 0 */
            up2Idx = 0;

            st = cpssDxSalPortUp2TcTableIdxSet(dev, port, up2Idx);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, up2Idx);

            /*
                1.1.2. Call with cpssDxSalPortUp2TcTableIdxGet with non-NULL up2TcTableIdxPtr.
                Expected: GT_OK and same values as written.
            */
            st = cpssDxSalPortUp2TcTableIdxGet(dev, port, &up2IdxGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalPortUp2TcTableIdxGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(up2Idx, up2IdxGet,
                       "get another up2TcTableIdx than was set: %d, %d", dev, port);
            
            /* Call with up2TcTableIdx = 0 */
            up2Idx = 0;

            st = cpssDxSalPortUp2TcTableIdxSet(dev, port, up2Idx);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, up2Idx);

            /*
                1.1.2. Call with cpssDxSalPortUp2TcTableIdxGet with non-NULL up2TcTableIdxPtr.
                Expected: GT_OK and same values as written.
            */
            st = cpssDxSalPortUp2TcTableIdxGet(dev, port, &up2IdxGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalPortUp2TcTableIdxGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(up2Idx, up2IdxGet,
                       "get another up2TcTableIdx than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range up2TcTableIdx [2].
                Expected: non GT_OK.
            */
            up2Idx = 2;

            st = cpssDxSalPortUp2TcTableIdxSet(dev, port, up2Idx);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, up2Idx);

            up2Idx = 0;
        }

        up2Idx = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalPortUp2TcTableIdxSet(dev, port, up2Idx);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortUp2TcTableIdxSet(dev, port, up2Idx);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalPortUp2TcTableIdxSet(dev, port, up2Idx);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    up2Idx = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = COS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortUp2TcTableIdxSet(dev, port, up2Idx);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssDxSalPortUp2TcTableIdxSet(dev, port, up2Idx);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortUp2TcTableIdxGet
(
    IN  GT_U8    dev,
    IN  GT_U8    port,
    OUT GT_U8   *up2TcTableIdxPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortUp2TcTableIdxGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS
    1.1.1. Call with non-NULL up2TcTableIdxPtr.
    Expected: GT_OK.
    1.1.2. Call with up2TcTableIdxPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8     dev; 
    GT_U8     port   = COS_VALID_VIRT_PORT_CNS;
    GT_U8     up2Idx = 0;
    
    
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
                1.1.1. Call with non-NULL up2TcTableIdxPtr.
                Expected: GT_OK.
            */
            st = cpssDxSalPortUp2TcTableIdxGet(dev, port, &up2Idx);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with up2TcTableIdxPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxSalPortUp2TcTableIdxGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, up2TcTableIdxPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalPortUp2TcTableIdxGet(dev, port, &up2Idx);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortUp2TcTableIdxGet(dev, port, &up2Idx);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalPortUp2TcTableIdxGet(dev, port, &up2Idx);
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
        st = cpssDxSalPortUp2TcTableIdxGet(dev, port, &up2Idx);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssDxSalPortUp2TcTableIdxGet(dev, port, &up2Idx);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortUp2TcEnableSet
(
    IN GT_U8    dev,
    IN GT_U8    port,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortUp2TcEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS
    1.1.1. Call with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call with cpssDxSalPortUp2TcEnableGet with non-NULL enablePtr.
    Expected: GT_OK and same value as written.
*/
    GT_STATUS st = GT_OK;

    GT_U8     dev; 
    GT_U8     port      = COS_VALID_VIRT_PORT_CNS;
    GT_BOOL   enable    = GT_FALSE;
    GT_BOOL   enableGet = GT_FALSE;
    
    
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
                1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with enable = GT_FALSE */
            enable = GT_FALSE;

            st = cpssDxSalPortUp2TcEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call with cpssDxSalPortUp2TcEnableGet with non-NULL enablePtr.
                Expected: GT_OK and same value as written.
            */
            st = cpssDxSalPortUp2TcEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalPortUp2TcEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
            
            /* Call with enable = GT_TRUE */
            enable = GT_TRUE;

            st = cpssDxSalPortUp2TcEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call with cpssDxSalPortUp2TcEnableGet with non-NULL enablePtr.
                Expected: GT_OK and same value as written.
            */
            st = cpssDxSalPortUp2TcEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalPortUp2TcEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        enable = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalPortUp2TcEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortUp2TcEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalPortUp2TcEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = COS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortUp2TcEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssDxSalPortUp2TcEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortUp2TcEnableGet
(
    IN  GT_U8    dev,
    IN  GT_U8    port,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortUp2TcEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS
    1.1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8     dev; 
    GT_U8     port   = COS_VALID_VIRT_PORT_CNS;
    GT_BOOL   enable = GT_FALSE;
   
    
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
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssDxSalPortUp2TcEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxSalPortUp2TcEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalPortUp2TcEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortUp2TcEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalPortUp2TcEnableGet(dev, port, &enable);
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
        st = cpssDxSalPortUp2TcEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssDxSalPortUp2TcEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalTc2UserPrioSet
(
    IN GT_U8    dev,
    IN GT_U8    trfClass,
    IN GT_U8    userPrio
)
*/
UTF_TEST_CASE_MAC(cpssDxSalTc2UserPrioSet)
{
/*
    ITERATE_DEVICES 
    1.1. Call function with userPrio [0 / 7], 
                            trfClass [0 / CPSS_4_TC_RANGE_CNS-1 = 3].
    Expected: GT_OK.
    1.2. Call cpssDxSalTc2UserPrioGet with non-null userPrioPtr, other params same as in 1.1.
    Expected: GT_OK and the same value as written.
    1.3. Call function with out of range userPrio [CPSS_USER_PRIORITY_RANGE_CNS = 8],
                            other params same as in 1.1.
    Expected: non GT_OK.
    1.4. Call function with out of range trfClass [CPSS_4_TC_RANGE_CNS = 4],
                            other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st   = GT_OK;

    GT_U8     dev;
    GT_U8     trfClass    = 0;
    GT_U8     userPrio    = 0;
    GT_U8     userPrioGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with userPrio [0 / 7], 
                                    trfClass [0 / CPSS_4_TC_RANGE_CNS-1 = 3].
            Expected: GT_OK.
        */

        /* Call with userPrio[0], trfClass[0] */
        userPrio = 0;
        trfClass = 0;
        
        st = cpssDxSalTc2UserPrioSet(dev, trfClass, userPrio);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trfClass, userPrio);

        /*
            1.2. Call cpssDxSalTc2UserPrioGet with non-null userPrioPtr, other params same as in 1.1.
            Expected: GT_OK and the same value as written.
        */
        st = cpssDxSalTc2UserPrioGet(dev, trfClass, &userPrioGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssDxSalTc2UserPrioGet: %d, %d", dev, trfClass);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(userPrio, userPrioGet,
                       "get another userPrio than was set: %d", dev);

        /* Call with userPrio[7], trfClass[CPSS_4_TC_RANGE_CNS - 1] */
        userPrio = 7;
        trfClass = CPSS_4_TC_RANGE_CNS - 1;
        
        st = cpssDxSalTc2UserPrioSet(dev, trfClass, userPrio);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trfClass, userPrio);

        /*
            1.2. Call cpssDxSalTc2UserPrioGet with non-null userPrioPtr, other params same as in 1.1.
            Expected: GT_OK and the same value as written.
        */
        st = cpssDxSalTc2UserPrioGet(dev, trfClass, &userPrioGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssDxSalTc2UserPrioGet: %d, %d", dev, trfClass);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(userPrio, userPrioGet,
                       "get another userPrio than was set: %d", dev);

        /* 
            1.3. Call function with out of range userPrio [CPSS_USER_PRIORITY_RANGE_CNS = 8],
                                    other params same as in 1.1.
            Expected: non GT_OK.
        */
        userPrio = CPSS_USER_PRIORITY_RANGE_CNS;

        st = cpssDxSalTc2UserPrioSet(dev, trfClass, userPrio);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, userPrio = %d", dev, userPrio);

        userPrio = 0;

        /* 
            1.4. Call function with out of range trfClass [CPSS_4_TC_RANGE_CNS = 4],
                                    other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        trfClass = CPSS_4_TC_RANGE_CNS;

        st = cpssDxSalTc2UserPrioSet(dev, trfClass, userPrio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, trfClass);

        trfClass = 0;
    }

    userPrio = 0;
    trfClass = 0;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalTc2UserPrioSet(dev, trfClass, userPrio);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalTc2UserPrioSet(dev, trfClass, userPrio);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalTc2UserPrioGet
(
    IN  GT_U8   dev,
    IN  GT_U8   trfClass,
    OUT GT_U8   *userPrioPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalTc2UserPrioGet)
{
/*
    ITERATE_DEVICES 
    1.1. Call function with trfClass [0 / CPSS_4_TC_RANGE_CNS-1 = 3], non-NULL userPrioPtr. Expected: GT_OK.
    1.2. Call function with out of range trfClass [CPSS_4_TC_RANGE_CNS = 4], other params same as in 1.1. Expected: GT_BAD_PARAM.
    1.3. Call function with userPrioPtr [NULL], other params same as in 1.1. Expected: GT_BAD_PTR.
*/
    GT_STATUS st   = GT_OK;

    GT_U8     dev;
    GT_U8     trfClass = 0;
    GT_U8     userPrio = 0;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with trfClass [0 / CPSS_4_TC_RANGE_CNS-1 = 3],
                                    non-NULL userPrioPtr.
            Expected: GT_OK.
        */

        /* Call with trfClass[0] */
        trfClass = 0;
        
        st = cpssDxSalTc2UserPrioGet(dev, trfClass, &userPrio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trfClass);

        /* Call with trfClass[CPSS_4_TC_RANGE_CNS - 1] */
        trfClass = CPSS_4_TC_RANGE_CNS - 1;
        
        st = cpssDxSalTc2UserPrioGet(dev, trfClass, &userPrio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trfClass);

        /* 
            1.2. Call function with out of range trfClass [CPSS_4_TC_RANGE_CNS = 4],
                                    other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        trfClass = CPSS_4_TC_RANGE_CNS;

        st = cpssDxSalTc2UserPrioGet(dev, trfClass, &userPrio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, trfClass);

        trfClass = 0;

        /*
            1.3. Call function with userPrioPtr [NULL],
                                    other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalTc2UserPrioGet(dev, trfClass, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, userPrioPtr = NULL", dev, trfClass);
    }

    trfClass = 0;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalTc2UserPrioGet(dev, trfClass, &userPrio);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalTc2UserPrioGet(dev, trfClass, &userPrio);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalTCForCpuCodeSet
(
    IN GT_U8                    dev,
    IN CPSS_NET_RX_CPU_CODE_ENT cpuCode,
    IN GT_U8                    trfClass
)
*/
UTF_TEST_CASE_MAC(cpssDxSalTCForCpuCodeSet)
{
/*
    ITERATE_DEVICES 
    1.1. Call function with cpuCode [CPSS_NET_ETH_BRIDGED_LLT_E /
                                     CPSS_NET_CONTROL_E /
                                     CPSS_NET_CONTROL_BPDU_E /
                                     CPSS_NET_CONTROL_SRC_DST_MAC_TRAP_E /
                                     CPSS_NET_CONTROL_MAC_RANGE_TRAP_E /
                                     CPSS_NET_INTERVENTION_ARP_E /
                                     CPSS_NET_INTERVENTION_IGMP_E /
                                     CPSS_NET_INTERVENTION_SA_DA_E /
                                     CPSS_NET_INTERVENTION_PORT_LOCK_E /
                                     CPSS_NET_CLASS_KEY_MIRROR_E /
                                     CPSS_NET_LOCK_PORT_MIRROR_E /
                                     CPSS_NET_UNKNOWN_UC_E /
                                     CPSS_NET_UNREG_MC_E],
                            trfClass [0 / CPSS_4_TC_RANGE_CNS-1 = 3].
    Expected: GT_OK.
    1.2. Call cpssDxSalTCForCpuCodeGet with cpuCode [CPSS_NET_ETH_BRIDGED_LLT_E /
                                                     CPSS_NET_UNREG_MC_E], non-null trfClassPtr.
    Expected: GT_OK and the same value as written.
    1.3. Call function with unsupported cpuCode [CPSS_NET_INTERVENTION_SA_E /
                                                 CPSS_NET_IPV4_UNREGISTERED_MULTICAST_E /
                                                 CPSS_NET_ROUTED_PACKET_FORWARD_E],
                            other params same as in 1.1.
    Expected: non GT_OK.
    1.4. Call function with out of range cpuCode [0x5AAAAAA5],
                            other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call function with out of range trfClass [CPSS_4_TC_RANGE_CNS = 4],
                            cpuCode [CPSS_NET_CONTROL_E].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st   = GT_OK;

    GT_U8                    dev;
    CPSS_NET_RX_CPU_CODE_ENT cpuCode     = CPSS_NET_CONTROL_E;
    GT_U8                    trfClass    = 0;
    GT_U8                    trfClassGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with cpuCode [CPSS_NET_ETH_BRIDGED_LLT_E /
                                             CPSS_NET_CONTROL_E /
                                             CPSS_NET_CONTROL_BPDU_E /
                                             CPSS_NET_CONTROL_SRC_DST_MAC_TRAP_E /
                                             CPSS_NET_CONTROL_MAC_RANGE_TRAP_E /
                                             CPSS_NET_INTERVENTION_ARP_E /

                                             CPSS_NET_INTERVENTION_IGMP_E /
                                             CPSS_NET_INTERVENTION_SA_DA_E /
                                             CPSS_NET_INTERVENTION_PORT_LOCK_E /
                                             CPSS_NET_CLASS_KEY_MIRROR_E /
                                             CPSS_NET_LOCK_PORT_MIRROR_E /
                                             CPSS_NET_UNKNOWN_UC_E /
                                             CPSS_NET_UNREG_MC_E],
                                    trfClass [0 / CPSS_4_TC_RANGE_CNS-1 = 3].
            Expected: GT_OK.
        */

        /* Call with cpuCode [CPSS_NET_ETH_BRIDGED_LLT_E], trfClass[0] */
        cpuCode  = CPSS_NET_ETH_BRIDGED_LLT_E;
        trfClass = 0;
        
        st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cpuCode, trfClass);

        /* Call with cpuCode [CPSS_NET_CONTROL_E], trfClass[0] */
        cpuCode  = CPSS_NET_CONTROL_E;
        trfClass = 0;
        
        st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cpuCode, trfClass);

        /* Call with cpuCode [CPSS_NET_CONTROL_BPDU_E], trfClass[0] */
        cpuCode  = CPSS_NET_CONTROL_BPDU_E;
        trfClass = 0;
        
        st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cpuCode, trfClass);

        /* Call with cpuCode [CPSS_NET_CONTROL_SRC_DST_MAC_TRAP_E], trfClass[0] */
        cpuCode  = CPSS_NET_CONTROL_SRC_DST_MAC_TRAP_E;
        trfClass = 0;
        
        st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cpuCode, trfClass);

        /* Call with cpuCode [CPSS_NET_CONTROL_MAC_RANGE_TRAP_E], trfClass[0] */
        cpuCode  = CPSS_NET_CONTROL_MAC_RANGE_TRAP_E;
        trfClass = 0;
        
        st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cpuCode, trfClass);

        /* Call with cpuCode [CPSS_NET_INTERVENTION_ARP_E], trfClass[0] */
        cpuCode  = CPSS_NET_INTERVENTION_ARP_E;
        trfClass = 0;
        
        st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cpuCode, trfClass);

        /* Call with cpuCode [CPSS_NET_INTERVENTION_IGMP_E], trfClass[CPSS_4_TC_RANGE_CNS - 1] */
        cpuCode  = CPSS_NET_INTERVENTION_IGMP_E;
        trfClass = CPSS_4_TC_RANGE_CNS - 1;
        
        st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cpuCode, trfClass);

        /* Call with cpuCode [CPSS_NET_INTERVENTION_SA_DA_E], trfClass[CPSS_4_TC_RANGE_CNS - 1] */
        cpuCode  = CPSS_NET_INTERVENTION_SA_DA_E;
        trfClass = CPSS_4_TC_RANGE_CNS - 1;
        
        st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cpuCode, trfClass);

        /* Call with cpuCode [CPSS_NET_INTERVENTION_PORT_LOCK_E], trfClass[CPSS_4_TC_RANGE_CNS - 1] */
        cpuCode  = CPSS_NET_INTERVENTION_PORT_LOCK_E;
        trfClass = CPSS_4_TC_RANGE_CNS - 1;
        
        st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cpuCode, trfClass);

        /* Call with cpuCode [CPSS_NET_CLASS_KEY_MIRROR_E], trfClass[CPSS_4_TC_RANGE_CNS - 1] */
        cpuCode  = CPSS_NET_CLASS_KEY_MIRROR_E;
        trfClass = CPSS_4_TC_RANGE_CNS - 1;
        
        st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cpuCode, trfClass);

        /* Call with cpuCode [CPSS_NET_LOCK_PORT_MIRROR_E], trfClass[CPSS_4_TC_RANGE_CNS - 1] */
        cpuCode  = CPSS_NET_LOCK_PORT_MIRROR_E;
        trfClass = CPSS_4_TC_RANGE_CNS - 1;
        
        st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cpuCode, trfClass);

        /* Call with cpuCode [CPSS_NET_UNKNOWN_UC_E], trfClass[CPSS_4_TC_RANGE_CNS - 1] */
        cpuCode  = CPSS_NET_UNKNOWN_UC_E;
        trfClass = CPSS_4_TC_RANGE_CNS - 1;
        
        st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cpuCode, trfClass);

        /* Call with cpuCode [CPSS_NET_UNREG_MC_E], trfClass[CPSS_4_TC_RANGE_CNS - 1] */
        cpuCode  = CPSS_NET_UNREG_MC_E;
        trfClass = CPSS_4_TC_RANGE_CNS - 1;
        
        st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cpuCode, trfClass);

        /*
            1.2. Call cpssDxSalTCForCpuCodeGet with cpuCode [CPSS_NET_ETH_BRIDGED_LLT_E /
                                                             CPSS_NET_UNREG_MC_E],
                                                    non-null trfClassPtr.
            Expected: GT_OK and the same value as written.
        */

        /* Call with cpuCode [CPSS_NET_ETH_BRIDGED_LLT_E] */
        cpuCode  = CPSS_NET_ETH_BRIDGED_LLT_E;

        st = cpssDxSalTCForCpuCodeGet(dev, cpuCode, &trfClassGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssDxSalTCForCpuCodeGet: %d, %d", dev, cpuCode);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(trfClass, trfClassGet,
                       "get another trfClass than was set: %d", dev);

        /* Call with cpuCode [CPSS_NET_UNREG_MC_E] */
        cpuCode  = CPSS_NET_UNREG_MC_E;

        st = cpssDxSalTCForCpuCodeGet(dev, cpuCode, &trfClassGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssDxSalTCForCpuCodeGet: %d, %d", dev, cpuCode);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(trfClass, trfClassGet,
                       "get another trfClass than was set: %d", dev);

        /* 
            1.3. Call function with unsupported cpuCode [CPSS_NET_INTERVENTION_SA_E /
                                                         CPSS_NET_IPV4_UNREGISTERED_MULTICAST_E /
                                                         CPSS_NET_ROUTED_PACKET_FORWARD_E],
                                    other params same as in 1.1.
            Expected: non GT_OK.
        */

        /* Call with cpuCode [CPSS_NET_INTERVENTION_SA_E] */
        cpuCode = CPSS_NET_INTERVENTION_SA_E;

        st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cpuCode);

        cpuCode = CPSS_NET_ETH_BRIDGED_LLT_E;

        /* Call with cpuCode [CPSS_NET_IPV4_UNREGISTERED_MULTICAST_E] */
        cpuCode = CPSS_NET_IPV4_UNREGISTERED_MULTICAST_E;

        st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cpuCode);

        cpuCode = CPSS_NET_ETH_BRIDGED_LLT_E;

        /* Call with cpuCode [CPSS_NET_ROUTED_PACKET_FORWARD_E] */
        cpuCode = CPSS_NET_ROUTED_PACKET_FORWARD_E;

        st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cpuCode);

        cpuCode = CPSS_NET_ETH_BRIDGED_LLT_E;

        /*
            1.4. Call function with out of range cpuCode [0x5AAAAAA5],
                                    other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        cpuCode = COS_INVALID_ENUM_CNS;

        st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cpuCode);

        cpuCode = CPSS_NET_ETH_BRIDGED_LLT_E;

        /* 
            1.5. Call function with out of range trfClass [CPSS_4_TC_RANGE_CNS = 4],
                                    cpuCode [CPSS_NET_CONTROL_E].
            Expected: GT_BAD_PARAM.
        */
        trfClass = CPSS_4_TC_RANGE_CNS;

        st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, trfClass = %d", dev, trfClass);

        trfClass = 0;
    }

    cpuCode  = CPSS_NET_ETH_BRIDGED_LLT_E;
    trfClass = 0;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalTCForCpuCodeSet(dev, cpuCode, trfClass);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalTCForCpuCodeGet
(
    IN  GT_U8                    dev,
    IN  CPSS_NET_RX_CPU_CODE_ENT cpuCode,
    OUT GT_U8                   *trfClassPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalTCForCpuCodeGet)
{
/*
    ITERATE_DEVICES 
    1.1. Call function with cpuCode [CPSS_NET_ETH_BRIDGED_LLT_E /
                                     CPSS_NET_LOCK_PORT_MIRROR_E /
                                     CPSS_NET_UNKNOWN_UC_E /
                                     CPSS_NET_UNREG_MC_E],
                            non-NULL trfClassPtr.
    Expected: GT_OK.
    1.2. Call function with unsupported cpuCode [CPSS_NET_INTERVENTION_SA_E /
                                                 CPSS_NET_IPV4_UNREGISTERED_MULTICAST_E /
                                                 CPSS_NET_ROUTED_PACKET_FORWARD_E],
                            other params same as in 1.1.
    Expected: non GT_OK.
    1.3. Call function with out of range cpuCode [0x5AAAAAA5],
                            other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call function with trfClassPtr [NULL],
                            cpuCode [CPSS_NET_ETH_BRIDGED_LLT_E].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st   = GT_OK;

    GT_U8                    dev;
    CPSS_NET_RX_CPU_CODE_ENT cpuCode  = CPSS_NET_CONTROL_E;
    GT_U8                    trfClass = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with cpuCode [CPSS_NET_ETH_BRIDGED_LLT_E /
                                             CPSS_NET_LOCK_PORT_MIRROR_E /
                                             CPSS_NET_UNKNOWN_UC_E /
                                             CPSS_NET_UNREG_MC_E],
                                    non-NULL trfClassPtr.
            Expected: GT_OK.
        */

        /* Call with cpuCode [CPSS_NET_ETH_BRIDGED_LLT_E] */
        cpuCode  = CPSS_NET_ETH_BRIDGED_LLT_E;
        
        st = cpssDxSalTCForCpuCodeGet(dev, cpuCode, &trfClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cpuCode);

        /* Call with cpuCode [CPSS_NET_LOCK_PORT_MIRROR_E] */
        cpuCode  = CPSS_NET_LOCK_PORT_MIRROR_E;
        
        st = cpssDxSalTCForCpuCodeGet(dev, cpuCode, &trfClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cpuCode);

        /* Call with cpuCode [CPSS_NET_UNKNOWN_UC_E] */
        cpuCode  = CPSS_NET_UNKNOWN_UC_E;
        
        st = cpssDxSalTCForCpuCodeGet(dev, cpuCode, &trfClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cpuCode);

        /* Call with cpuCode [CPSS_NET_UNREG_MC_E] */
        cpuCode  = CPSS_NET_UNREG_MC_E;
        
        st = cpssDxSalTCForCpuCodeGet(dev, cpuCode, &trfClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cpuCode);

        /* 
            1.2. Call function with unsupported cpuCode [CPSS_NET_INTERVENTION_SA_E /
                                                         CPSS_NET_IPV4_UNREGISTERED_MULTICAST_E /
                                                         CPSS_NET_ROUTED_PACKET_FORWARD_E],
                                    other params same as in 1.1.
            Expected: non GT_OK.
        */

        /* Call with cpuCode [CPSS_NET_INTERVENTION_SA_E] */
        cpuCode = CPSS_NET_INTERVENTION_SA_E;

        st = cpssDxSalTCForCpuCodeGet(dev, cpuCode, &trfClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cpuCode);

        cpuCode = CPSS_NET_ETH_BRIDGED_LLT_E;

        /* Call with cpuCode [CPSS_NET_IPV4_UNREGISTERED_MULTICAST_E] */
        cpuCode = CPSS_NET_IPV4_UNREGISTERED_MULTICAST_E;

        st = cpssDxSalTCForCpuCodeGet(dev, cpuCode, &trfClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cpuCode);

        cpuCode = CPSS_NET_ETH_BRIDGED_LLT_E;

        /* Call with cpuCode [CPSS_NET_ROUTED_PACKET_FORWARD_E] */
        cpuCode = CPSS_NET_ROUTED_PACKET_FORWARD_E;

        st = cpssDxSalTCForCpuCodeGet(dev, cpuCode, &trfClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cpuCode);

        cpuCode = CPSS_NET_ETH_BRIDGED_LLT_E;

        /*
            1.3. Call function with out of range cpuCode [0x5AAAAAA5],
                                    other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        cpuCode = COS_INVALID_ENUM_CNS;

        st = cpssDxSalTCForCpuCodeGet(dev, cpuCode, &trfClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cpuCode);

        cpuCode = CPSS_NET_ETH_BRIDGED_LLT_E;

        /* 
            1.4. Call function with trfClassPtr [NULL],
                                    cpuCode [CPSS_NET_ETH_BRIDGED_LLT_E].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalTCForCpuCodeGet(dev, cpuCode, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PARAM, st, "%d, trfClassPtr = NULL", dev);
    }

    cpuCode  = CPSS_NET_ETH_BRIDGED_LLT_E;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalTCForCpuCodeGet(dev, cpuCode, &trfClass);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalTCForCpuCodeGet(dev, cpuCode, &trfClass);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalUserPrioModeSet
(
    IN GT_U8                        dev,
    IN CPSS_COS_USER_PRIO_MODE_ENT  userPrioMode
)
*/
UTF_TEST_CASE_MAC(cpssDxSalUserPrioModeSet)
{
/*
    ITERATE_DEVICES 
    1.1. Call with userPrioMode [CPSS_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E /
                                 CPSS_COS_USER_PRIO_MODE_TC_PER_PORT_E].
    Expected: GT_OK.
    1.2. Call cpssDxSalUserPrioModeGet with non-NULL userPrioModePtr.
    Expected: GT_OK and same value as written.
    1.3. Call with out of range userPrioMode [CPSS_COS_USER_PRIO_MODE_MAX_E /
                                              0x5AAAAAA5],
                   other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st   = GT_OK;

    GT_U8                       dev;
    CPSS_COS_USER_PRIO_MODE_ENT prioMode    = CPSS_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E;
    CPSS_COS_USER_PRIO_MODE_ENT prioModeGet = CPSS_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with userPrioMode [CPSS_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E /
                                         CPSS_COS_USER_PRIO_MODE_TC_PER_PORT_E].
            Expected: GT_OK.
        */

        /* Call with userPrioMode [CPSS_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E] */
        prioMode = CPSS_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E;
        
        st = cpssDxSalUserPrioModeSet(dev, prioMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, prioMode);

        /*
            1.2. Call cpssDxSalUserPrioModeGet with non-NULL userPrioModePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssDxSalUserPrioModeGet(dev, &prioModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxSalUserPrioModeGet: %d", dev);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(prioMode, prioModeGet,
                       "get another userPrioMode than was set: %d", dev);

        /* Call with userPrioMode [CPSS_COS_USER_PRIO_MODE_TC_PER_PORT_E] */
        prioMode = CPSS_COS_USER_PRIO_MODE_TC_PER_PORT_E;
        
        st = cpssDxSalUserPrioModeSet(dev, prioMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, prioMode);

        /*
            1.2. Call cpssDxSalUserPrioModeGet with non-NULL userPrioModePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssDxSalUserPrioModeGet(dev, &prioModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxSalUserPrioModeGet: %d", dev);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(prioMode, prioModeGet,
                       "get another userPrioMode than was set: %d", dev);

        /*
            1.3. Call with out of range userPrioMode [CPSS_COS_USER_PRIO_MODE_MAX_E /
                                                      0x5AAAAAA5],
                           other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */

        /* Call with userPrioMode [CPSS_COS_USER_PRIO_MODE_MAX_E] */
        prioMode = CPSS_COS_USER_PRIO_MODE_MAX_E;

        st = cpssDxSalUserPrioModeSet(dev, prioMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, prioMode);

        /* Call with userPrioMode [0x5AAAAAA5] */
        prioMode = COS_INVALID_ENUM_CNS;

        st = cpssDxSalUserPrioModeSet(dev, prioMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, prioMode);

        prioMode = CPSS_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E;
    }

    prioMode = CPSS_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalUserPrioModeSet(dev, prioMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalUserPrioModeSet(dev, prioMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalUserPrioModeGet
(
    IN  GT_U8                        dev,
    OUT CPSS_COS_USER_PRIO_MODE_ENT *userPrioModePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalUserPrioModeGet)
{
/*
    ITERATE_DEVICES 
    1.1. Call with non-NULL userPrioModePtr.
    Expected: GT_OK.
    1.2. Call with userPrioModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st   = GT_OK;

    GT_U8                       dev;
    CPSS_COS_USER_PRIO_MODE_ENT prioMode = CPSS_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with non-NULL userPrioModePtr.
            Expected: GT_OK.
        */
        st = cpssDxSalUserPrioModeGet(dev, &prioMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with userPrioModePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalUserPrioModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PARAM, st, "%d, userPrioModePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalUserPrioModeGet(dev, &prioMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalUserPrioModeGet(dev, &prioMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortDefaultUserPrioSet
(
    IN GT_U8    dev,
    IN GT_U8    port,
    IN GT_U8    defaultUserPrio
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortDefaultUserPrioSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS
    1.1.1. Call function with defaultUserPrio [0 / CPSS_USER_PRIORITY_RANGE_CNS -1 = 7].
    Expected: GT_OK.
    1.1.2. Call function cpssDxSalPortDefaultUserPrioGet with non-NULL defaultUserPrioPtr.
    Expected: GT_OK and same value as written.
    1.1.3. Call function with out of range defaultUserPrio [CPSS_USER_PRIORITY_RANGE_CNS = 8],
                              other params same as in 1.1.
    Expected: non GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8     dev; 
    GT_U8     port           = COS_VALID_PHY_PORT_CNS;
    GT_U8     defUserPrio    = 0;
    GT_U8     defUserPrioGet = 0;
    
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call function with defaultUserPrio [0 / CPSS_USER_PRIORITY_RANGE_CNS -1 = 7].
                Expected: GT_OK.
            */

            /* Call with defaultUserPrio [0] */
            defUserPrio = 0;

            st = cpssDxSalPortDefaultUserPrioSet(dev, port, defUserPrio);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, defUserPrio);

            /*
                1.1.2. Call function cpssDxSalPortDefaultUserPrioGet with non-NULL defaultUserPrioPtr.
                Expected: GT_OK and same value as written.
            */
            st = cpssDxSalPortDefaultUserPrioGet(dev, port, &defUserPrioGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalPortDefaultUserPrioGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(defUserPrio, defUserPrioGet,
                       "get another defaultUserPrio than was set: %d, %d", dev, port);
            
            /* Call with defaultUserPrio [CPSS_USER_PRIORITY_RANGE_CNS - 1] */
            defUserPrio = CPSS_USER_PRIORITY_RANGE_CNS - 1;

            st = cpssDxSalPortDefaultUserPrioSet(dev, port, defUserPrio);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, defUserPrio);

            /*
                1.1.2. Call function cpssDxSalPortDefaultUserPrioGet with non-NULL defaultUserPrioPtr.
                Expected: GT_OK and same value as written.
            */
            st = cpssDxSalPortDefaultUserPrioGet(dev, port, &defUserPrioGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalPortDefaultUserPrioGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(defUserPrio, defUserPrioGet,
                       "get another defaultUserPrio than was set: %d, %d", dev, port);

            /*
                1.1.3. Call function with out of range defaultUserPrio [CPSS_USER_PRIORITY_RANGE_CNS = 8],
                                          other params same as in 1.1.
                Expected: non GT_OK.
            */
            defUserPrio = CPSS_USER_PRIORITY_RANGE_CNS;

            st = cpssDxSalPortDefaultUserPrioSet(dev, port, defUserPrio);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, defUserPrio);

            defUserPrio = 0;
        }

        defUserPrio = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalPortDefaultUserPrioSet(dev, port, defUserPrio);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortDefaultUserPrioSet(dev, port, defUserPrio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalPortDefaultUserPrioSet(dev, port, defUserPrio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    defUserPrio = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = COS_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortDefaultUserPrioSet(dev, port, defUserPrio);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssDxSalPortDefaultUserPrioSet(dev, port, defUserPrio);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortDefaultUserPrioGet
(
    IN  GT_U8    dev,
    IN  GT_U8    port,
    OUT GT_U8   *defaultUserPrioPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortDefaultUserPrioGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS
    1.1.1. Call function with non-NULL defaultUserPrioPtr.
    Expected: GT_OK.
    1.1.2. Call function defaultUserPrioPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8     dev; 
    GT_U8     port        = COS_VALID_PHY_PORT_CNS;
    GT_U8     defUserPrio = 0;
    
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call function with non-NULL defaultUserPrioPtr.
                Expected: GT_OK.
            */
            st = cpssDxSalPortDefaultUserPrioGet(dev, port, &defUserPrio);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call function defaultUserPrioPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxSalPortDefaultUserPrioGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, defaultUserPrioPtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalPortDefaultUserPrioGet(dev, port, &defUserPrio);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortDefaultUserPrioGet(dev, port, &defUserPrio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalPortDefaultUserPrioGet(dev, port, &defUserPrio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    defUserPrio = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = COS_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortDefaultUserPrioGet(dev, port, &defUserPrio);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssDxSalPortDefaultUserPrioGet(dev, port, &defUserPrio);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortOverrideTcEnableSet
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortOverrideTcEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS
    1.1.1. Call with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call with cpssDxSalPortUp2TcEnableGet with non-NULL enablePtr.
    Expected: GT_OK and same value as written.
*/
    GT_STATUS st = GT_OK;

    GT_U8     dev; 
    GT_U8     port      = COS_VALID_PHY_PORT_CNS;
    GT_BOOL   enable    = GT_FALSE;
    GT_BOOL   enableGet = GT_FALSE;
    
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssDxSalPortOverrideTcEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call with cpssDxSalPortUp2TcEnableGet with non-NULL enablePtr.
                Expected: GT_OK and same value as written.
            */
            st = cpssDxSalPortUp2TcEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalPortUp2TcEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
            
            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssDxSalPortOverrideTcEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call with cpssDxSalPortUp2TcEnableGet with non-NULL enablePtr.
                Expected: GT_OK and same value as written.
            */
            st = cpssDxSalPortUp2TcEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalPortUp2TcEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalPortOverrideTcEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortOverrideTcEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalPortOverrideTcEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = COS_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortOverrideTcEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssDxSalPortOverrideTcEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortOverrideTcEnableGet
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    IN  GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortOverrideTcEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS
    1.1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8     dev; 
    GT_U8     port   = COS_VALID_PHY_PORT_CNS;
    GT_BOOL   enable = GT_FALSE;
 
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssDxSalPortOverrideTcEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxSalPortUp2TcEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalPortOverrideTcEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortOverrideTcEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalPortOverrideTcEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = COS_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortOverrideTcEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssDxSalPortOverrideTcEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalMacEntryTcOverrideEnableSet
(
    IN GT_U8    dev,
    IN CPSS_COS_MAC_ADDR_TYPE_ENT  macEntryTcOverrideEnable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalMacEntryTcOverrideEnableSet)
{
/*
    ITERATE_DEVICES 
    1.1. Call with macEntryTcOverrideEnable [CPSS_COS_MAC_ADDR_TYPE_NONE_E /
                                             CPSS_COS_MAC_ADDR_TYPE_ALL_E].
    Expected: GT_OK.
    1.2. Call with macEntryTcOverrideEnable [CPSS_COS_MAC_ADDR_TYPE_SA_ONLY_E/
                                             CPSS_COS_MAC_ADDR_TYPE_DA_ONLY_E] (unsupported).
    Expected: non GT_OK.
    1.3. Call with out of range macEntryTcOverrideEnable [0x5AAAAAA5],
                   other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st   = GT_OK;

    GT_U8                       dev;
    CPSS_COS_MAC_ADDR_TYPE_ENT  macEntryEn    = CPSS_COS_MAC_ADDR_TYPE_NONE_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with macEntryTcOverrideEnable [CPSS_COS_MAC_ADDR_TYPE_NONE_E /
                                                     CPSS_COS_MAC_ADDR_TYPE_ALL_E].
            Expected: GT_OK.
        */

        /* Call with macEntryTcOverrideEnable [CPSS_COS_MAC_ADDR_TYPE_NONE_E] */
        macEntryEn = CPSS_COS_MAC_ADDR_TYPE_NONE_E;
        
        st = cpssDxSalMacEntryTcOverrideEnableSet(dev, macEntryEn);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, macEntryEn);

        /* Call with macEntryTcOverrideEnable [CPSS_COS_MAC_ADDR_TYPE_ALL_E] */
        macEntryEn = CPSS_COS_MAC_ADDR_TYPE_ALL_E;
        
        st = cpssDxSalMacEntryTcOverrideEnableSet(dev, macEntryEn);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, macEntryEn);

        /*
            1.2. Call with macEntryTcOverrideEnable [CPSS_COS_MAC_ADDR_TYPE_SA_ONLY_E/
                                                     CPSS_COS_MAC_ADDR_TYPE_DA_ONLY_E] (unsupported).
            Expected: non GT_OK.
        */

        /* Call with macEntryTcOverrideEnable [CPSS_COS_MAC_ADDR_TYPE_SA_ONLY_E] */
        macEntryEn = CPSS_COS_MAC_ADDR_TYPE_SA_ONLY_E;
        
        st = cpssDxSalMacEntryTcOverrideEnableSet(dev, macEntryEn);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, macEntryEn);

        macEntryEn = CPSS_COS_MAC_ADDR_TYPE_NONE_E;

        /* Call with macEntryTcOverrideEnable [CPSS_COS_MAC_ADDR_TYPE_DA_ONLY_E] */
        macEntryEn = CPSS_COS_MAC_ADDR_TYPE_DA_ONLY_E;
        
        st = cpssDxSalMacEntryTcOverrideEnableSet(dev, macEntryEn);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, macEntryEn);

        macEntryEn = CPSS_COS_MAC_ADDR_TYPE_NONE_E;

        /*
            1.3. Call with out of range macEntryTcOverrideEnable [0x5AAAAAA5],
                           other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        macEntryEn = COS_INVALID_ENUM_CNS;

        st = cpssDxSalMacEntryTcOverrideEnableSet(dev, macEntryEn);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, macEntryEn);

        macEntryEn = CPSS_COS_MAC_ADDR_TYPE_NONE_E;
    }

    macEntryEn = CPSS_COS_MAC_ADDR_TYPE_NONE_E;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalMacEntryTcOverrideEnableSet(dev, macEntryEn);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalMacEntryTcOverrideEnableSet(dev, macEntryEn);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalUpOverrideEnableSet
(
    IN GT_U8   dev,
    IN GT_BOOL upOverrideEnable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalUpOverrideEnableSet)
{
/*
    ITERATE_DEVICES
    1.1. Call with upOverrideEnable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call with cpssDxSalUpOverrideEnableGet with non-NULL upOverrideEnable.
    Expected: GT_OK and same value as written.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_BOOL   upOverrideEnable    = GT_FALSE;
    GT_BOOL   upOverrideEnableGet = GT_FALSE;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with upOverrideEnable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        /* Call with with upOverrideEnable [GT_FALSE] */
        upOverrideEnable = GT_FALSE;
        
        st = cpssDxSalUpOverrideEnableSet(dev, upOverrideEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, upOverrideEnable);

        /*
            1.2. Call with cpssDxSalUpOverrideEnableGet with non-NULL upOverrideEnable.
            Expected: GT_OK and same value as written.
        */
        st = cpssDxSalUpOverrideEnableGet(dev, &upOverrideEnableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxSalUpOverrideEnableGet: %d", dev);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(upOverrideEnable, upOverrideEnableGet,
                       "get another upOverrideEnable than was set: %d", dev);

        /* Call with with upOverrideEnable [GT_TRUE] */
        upOverrideEnable = GT_TRUE;
        
        st = cpssDxSalUpOverrideEnableSet(dev, upOverrideEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, upOverrideEnable);

        /*
            1.2. Call with cpssDxSalUpOverrideEnableGet with non-NULL upOverrideEnable.
            Expected: GT_OK and same value as written.
        */
        st = cpssDxSalUpOverrideEnableGet(dev, &upOverrideEnableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxSalUpOverrideEnableGet: %d", dev);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(upOverrideEnable, upOverrideEnableGet,
                       "get another upOverrideEnable than was set: %d", dev);
    }

    upOverrideEnable = GT_TRUE;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalUpOverrideEnableSet(dev, upOverrideEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalUpOverrideEnableSet(dev, upOverrideEnable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalUpOverrideEnableGet
(
    IN GT_U8     dev,
    OUT GT_BOOL *upOverrideEnable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalUpOverrideEnableGet)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-NULL upOverrideEnable.
    Expected: GT_OK.
    1.2. Call with upOverrideEnable [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_BOOL   upOverrideEnable = GT_FALSE;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with non-NULL upOverrideEnable.
            Expected: GT_OK.
        */
        st = cpssDxSalUpOverrideEnableGet(dev, &upOverrideEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with upOverrideEnable [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalUpOverrideEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, upOverrideEnable = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalUpOverrideEnableGet(dev, &upOverrideEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalUpOverrideEnableGet(dev, &upOverrideEnable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalForceL3CosValuesSet
(
    IN GT_U8 dev,
    IN GT_U8 up,
    IN GT_U8 tc,
    IN GT_U8 dscp
)
*/
UTF_TEST_CASE_MAC(cpssDxSalForceL3CosValuesSet)
{
/*
    ITERATE_DEVICES 
    1.1. Call function with up [0/ CPSS_USER_PRIORITY_RANGE_CNS-1 = 7],
                            tc [0 / CPSS_4_TC_RANGE_CNS-1 = 3],
                            dscp [0 / 31 / CPSS_DSCP_RANGE_CNS-1 = 63].
    Expected: GT_OK.
    1.2. Call cpssDxSalForceL3CosValuesGet with non-NULL pointers.
    Expected: GT_OK and the same values as written.
    1.3. Call function with out of range up [CPSS_USER_PRIORITY_RANGE_CNS = 8],
                            other params same as in 1.1.
    Expected: non GT_OK.
    1.4. Call function with out of range tc [CPSS_4_TC_RANGE_CNS = 4],
                            other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call function with out of range dscp [CPSS_DSCP_RANGE_CNS = 64],
                            other params same as in 1.1.
    Expected: non GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U8 up      = 0;
    GT_U8 tc      = 0;
    GT_U8 dscp    = 0;
    GT_U8 upGet   = 0;
    GT_U8 tcGet   = 0;
    GT_U8 dscpGet = 0;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with up [0/ CPSS_USER_PRIORITY_RANGE_CNS-1 = 7],
                                    tc [0 / CPSS_4_TC_RANGE_CNS-1 = 3],
                                    dscp [0 / 31 / CPSS_DSCP_RANGE_CNS-1 = 63].
            Expected: GT_OK.
        */

        /* Call with with up [0], tc [0], dscp [0] */
        up   = 0;
        tc   = 0;
        dscp = 0;
        
        st = cpssDxSalForceL3CosValuesSet(dev, up, tc, dscp);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, up, tc, dscp);

        /*
            1.2. Call cpssDxSalForceL3CosValuesGet with non-NULL pointers.
            Expected: GT_OK and the same values as written.
        */
        st = cpssDxSalForceL3CosValuesGet(dev, &upGet, &tcGet, &dscpGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxSalForceL3CosValuesGet: %d", dev);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(up, upGet,
                       "get another up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(tc, tcGet,
                       "get another tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(dev, dscpGet,
                       "get another dscp than was set: %d", dev);

        /* Call with with up [CPSS_USER_PRIORITY_RANGE_CNS-1], tc [CPSS_4_TC_RANGE_CNS-1], dscp [31] */
        up   = CPSS_USER_PRIORITY_RANGE_CNS - 1;
        tc   = CPSS_4_TC_RANGE_CNS - 1;
        dscp = 31;
        
        st = cpssDxSalForceL3CosValuesSet(dev, up, tc, dscp);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, up, tc, dscp);

        /*
            1.2. Call cpssDxSalForceL3CosValuesGet with non-NULL pointers.
            Expected: GT_OK and the same values as written.
        */
        st = cpssDxSalForceL3CosValuesGet(dev, &upGet, &tcGet, &dscpGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxSalForceL3CosValuesGet: %d", dev);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(up, upGet,
                       "get another up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(tc, tcGet,
                       "get another tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(dev, dscpGet,
                       "get another dscp than was set: %d", dev);

        /* Call with with up [CPSS_USER_PRIORITY_RANGE_CNS-1], tc [CPSS_4_TC_RANGE_CNS-1], dscp [CPSS_DSCP_RANGE_CNS-1] */
        up   = CPSS_USER_PRIORITY_RANGE_CNS - 1;
        tc   = CPSS_4_TC_RANGE_CNS - 1;
        dscp = CPSS_DSCP_RANGE_CNS - 1;
        
        st = cpssDxSalForceL3CosValuesSet(dev, up, tc, dscp);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, up, tc, dscp);

        /*
            1.2. Call cpssDxSalForceL3CosValuesGet with non-NULL pointers.
            Expected: GT_OK and the same values as written.
        */
        st = cpssDxSalForceL3CosValuesGet(dev, &upGet, &tcGet, &dscpGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxSalForceL3CosValuesGet: %d", dev);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(up, upGet,
                       "get another up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(tc, tcGet,
                       "get another tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(dev, dscpGet,
                       "get another dscp than was set: %d", dev);

        /*
            1.3. Call function with out of range up [CPSS_USER_PRIORITY_RANGE_CNS = 8],
                                    other params same as in 1.1.
            Expected: non GT_OK.
        */
        up   = CPSS_USER_PRIORITY_RANGE_CNS;
        tc   = 0;
        dscp = 0;
        
        st = cpssDxSalForceL3CosValuesSet(dev, up, tc, dscp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, up);

        up = 0;

        /*
            1.4. Call function with out of range tc [CPSS_4_TC_RANGE_CNS = 4],
                                    other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        tc = CPSS_4_TC_RANGE_CNS;
        
        st = cpssDxSalForceL3CosValuesSet(dev, up, tc, dscp);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tc = %d", dev, tc);

        tc = 0;

        /*
            1.5. Call function with out of range dscp [CPSS_DSCP_RANGE_CNS = 64],
                                    other params same as in 1.1.
            Expected: non GT_OK.
        */
        dscp = CPSS_DSCP_RANGE_CNS;
        
        st = cpssDxSalForceL3CosValuesSet(dev, up, tc, dscp);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dscp = %d", dev, dscp);

        dscp = 0;
    }

    up   = 0;
    tc   = 0;
    dscp = 0;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalForceL3CosValuesSet(dev, up, tc, dscp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalForceL3CosValuesSet(dev, up, tc, dscp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalForceL3CosValuesGet
(
    IN  GT_U8  dev,
    OUT GT_U8 *upPtr,
    OUT GT_U8 *tcPtr,
    OUT GT_U8 *dscpPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalForceL3CosValuesGet)
{
/*
    ITERATE_DEVICES 
    1.1. Call function with non-NULL pointers.
    Expected: GT_OK.
    1.2. Call function with upPtr [NULL],
                            other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call function with tcPtr [NULL],
                            other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call function with dscpPtr [NULL],
                            other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U8 up      = 0;
    GT_U8 tc      = 0;
    GT_U8 dscp    = 0;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with non-NULL pointers.
            Expected: GT_OK.
        */
        st = cpssDxSalForceL3CosValuesGet(dev, &up, &tc, &dscp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with upPtr [NULL],
                                    other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalForceL3CosValuesGet(dev, NULL, &tc, &dscp);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, upPtr = NULL", dev);

        /*
            1.3. Call function with tcPtr [NULL],
                                    other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalForceL3CosValuesGet(dev, &up, NULL, &dscp);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, tcPtr = NULL", dev);

        /*
            1.4. Call function with dscpPtr [NULL],
                                    other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalForceL3CosValuesGet(dev, &up, &tc, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dscpPtr = NULL", dev);
    }
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalForceL3CosValuesGet(dev, &up, &tc, &dscp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalForceL3CosValuesGet(dev, &up, &tc, &dscp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortQosTrustModeSet
(
    IN GT_U8                        dev,
    IN GT_U8                        port,
    IN CPSS_QOS_PORT_TRUST_MODE_ENT portQosTrustMode
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortQosTrustModeSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS
    1.1.1. Call with portQosTrustMode [CPSS_QOS_PORT_NO_TRUST_E /
                                       CPSS_QOS_PORT_TRUST_L2_E /
                                       CPSS_QOS_PORT_TRUST_L3_E /
                                       CPSS_QOS_PORT_TRUST_L2_L3_E].
    Expected: GT_OK.
    1.1.2. Call with out of range portQosTrustMode [0x5AAAAAA5],
                     other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev; 
    GT_U8     port = COS_VALID_VIRT_PORT_CNS;

    CPSS_QOS_PORT_TRUST_MODE_ENT trustMode = CPSS_QOS_PORT_NO_TRUST_E;
    
    
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
                1.1.1. Call with portQosTrustMode [CPSS_QOS_PORT_NO_TRUST_E /
                                                   CPSS_QOS_PORT_TRUST_L2_E /
                                                   CPSS_QOS_PORT_TRUST_L3_E /
                                                   CPSS_QOS_PORT_TRUST_L2_L3_E].
                Expected: GT_OK.
            */

            /* Call with portQosTrustMode [CPSS_QOS_PORT_NO_TRUST_E] */
            trustMode = CPSS_QOS_PORT_NO_TRUST_E;

            st = cpssDxSalPortQosTrustModeSet(dev, port, trustMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trustMode);

            /* Call with portQosTrustMode [CPSS_QOS_PORT_TRUST_L2_E] */
            trustMode = CPSS_QOS_PORT_TRUST_L2_E;

            st = cpssDxSalPortQosTrustModeSet(dev, port, trustMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trustMode);

            /* Call with portQosTrustMode [CPSS_QOS_PORT_TRUST_L3_E] */
            trustMode = CPSS_QOS_PORT_TRUST_L3_E;

            st = cpssDxSalPortQosTrustModeSet(dev, port, trustMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trustMode);

            /* Call with portQosTrustMode [CPSS_QOS_PORT_TRUST_L2_L3_E] */
            trustMode = CPSS_QOS_PORT_TRUST_L2_L3_E;

            st = cpssDxSalPortQosTrustModeSet(dev, port, trustMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trustMode);

            /*
                1.1.2. Call with out of range portQosTrustMode [0x5AAAAAA5],
                                 other params same as in 1.1.
                Expected: GT_BAD_PARAM.
            */
            trustMode = COS_INVALID_ENUM_CNS;

            st = cpssDxSalPortQosTrustModeSet(dev, port, trustMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, trustMode);

            trustMode = CPSS_QOS_PORT_NO_TRUST_E;
        }

        trustMode = CPSS_QOS_PORT_NO_TRUST_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalPortQosTrustModeSet(dev, port, trustMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortQosTrustModeSet(dev, port, trustMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalPortQosTrustModeSet(dev, port, trustMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    trustMode = CPSS_QOS_PORT_NO_TRUST_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = COS_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortQosTrustModeSet(dev, port, trustMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssDxSalPortQosTrustModeSet(dev, port, trustMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssDxSalL2Cos suit
*/
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxSalL2Cos)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortDefaultTcSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortDefaultTcGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalUserPrio2TcSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalUserPrio2TcGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortUp2TcTableIdxSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortUp2TcTableIdxGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortUp2TcEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortUp2TcEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalTc2UserPrioSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalTc2UserPrioGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalTCForCpuCodeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalTCForCpuCodeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalUserPrioModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalUserPrioModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortDefaultUserPrioSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortDefaultUserPrioGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortOverrideTcEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortOverrideTcEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalMacEntryTcOverrideEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalUpOverrideEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalUpOverrideEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalForceL3CosValuesSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalForceL3CosValuesGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortQosTrustModeSet)
UTF_SUIT_END_TESTS_MAC(cpssDxSalL2Cos)

