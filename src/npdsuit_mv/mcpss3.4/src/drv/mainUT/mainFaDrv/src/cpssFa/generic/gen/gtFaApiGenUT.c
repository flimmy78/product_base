/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtFaApiGenUT.c
*
* DESCRIPTION:
*       Prestera API implementation for Prestera Fabric Adapter
*       General Global Control parameters.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpssFa/generic/gen/gtFaGen.h>
#include <cpssFaDriver/gtDriverFaDefs.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* Invalid enum */
#define FA_GEN_INVALID_ENUM_CNS        0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaSetActiveFport
(
    IN   GT_U8            devNum,
    IN   GT_FA_ACTIVE_FPORT  activeFport
)
*/
UTF_TEST_CASE_MAC(gtFaSetActiveFport)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with activeFport [GT_FA_ACTIVE_FPORT0 / GT_FA_ACTIVE_FPORT1].
    Expected: GT_OK.
    1.2. Call function with out of range activeFport [0x5AAAAAA5].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_FA_ACTIVE_FPORT activeFport = GT_FA_ACTIVE_FPORT0;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with activeFport [GT_FA_ACTIVE_FPORT0 / GT_FA_ACTIVE_FPORT1].
            Expected: GT_OK.
        */

        /* Call with activeFport [GT_FA_ACTIVE_FPORT0]. */
        activeFport = GT_FA_ACTIVE_FPORT0;
        
        st = gtFaSetActiveFport(dev, activeFport);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, activeFport);

        /* Call with activeFport [GT_FA_ACTIVE_FPORT1]. */
        activeFport = GT_FA_ACTIVE_FPORT1;
        
        st = gtFaSetActiveFport(dev, activeFport);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, activeFport);

        /*
            1.2. Call function with out of range activeFport [0x5AAAAAA5].
            Expected: NOT GT_OK.
        */
        activeFport = FA_GEN_INVALID_ENUM_CNS;
        
        st = gtFaSetActiveFport(dev, activeFport);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, activeFport);

        activeFport = GT_FA_ACTIVE_FPORT0;
    }

    activeFport = GT_FA_ACTIVE_FPORT0;
        
    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.         */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaSetActiveFport(dev, activeFport);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaSetActiveFport(dev, activeFport);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaUplinkReset
(
    IN   GT_U8            devNum
)
*/
UTF_TEST_CASE_MAC(gtFaUplinkReset)
{
/*
    ITERATE_FA_DEVICES
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        st = gtFaUplinkReset(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.         */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaUplinkReset(dev);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaUplinkReset(dev);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaUplinkStatusGet
(
    IN  GT_U8       devNum,    
    OUT GT_BOOL     *wideUplinkFarEndRunningPtr,
    OUT GT_BOOL     *narrowUplinkFarEndRunningPtr
)
*/
UTF_TEST_CASE_MAC(gtFaUplinkStatusGet)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with non-NULL wideUplinkFarEndRunningPtr
                            and non-NULL narrowUplinkFarEndRunningPtr.
    Expected: GT_OK.
    1.2. Call function with wideUplinkFarEndRunningPtr [NULL]
                            and non-NULL narrowUplinkFarEndRunningPtr.
    Expected: NOT GT_OK.
    1.3. Call function with non-NULL wideUplinkFarEndRunningPtr
                            and narrowUplinkFarEndRunningPtr [NULL].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_BOOL   wideUplink   = GT_FALSE;
    GT_BOOL   narrowUplink = GT_FALSE;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with non-NULL wideUplinkFarEndRunningPtr
                                    and non-NULL narrowUplinkFarEndRunningPtr.
            Expected: GT_OK.
        */
        st = gtFaUplinkStatusGet(dev, &wideUplink, &narrowUplink);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with wideUplinkFarEndRunningPtr [NULL]
                                    and non-NULL narrowUplinkFarEndRunningPtr.
            Expected: NOT GT_OK.
        */
        st = gtFaUplinkStatusGet(dev, NULL, &narrowUplink);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, wideUplinkFarEndRunningPtr = NULL", dev);

        /*
            1.3. Call function with non-NULL wideUplinkFarEndRunningPtr
                                    and narrowUplinkFarEndRunningPtr [NULL].
            Expected: NOT GT_OK.
        */
        st = gtFaUplinkStatusGet(dev, &wideUplink, NULL);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, narrowUplinkFarEndRunningPtr = NULL", dev);
    }

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.         */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaUplinkStatusGet(dev, &wideUplink, &narrowUplink);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaUplinkStatusGet(dev, &wideUplink, &narrowUplink);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaUplinkEnable
(
    IN   GT_U8       devNum,
    IN   GT_BOOL     enable
)
*/
UTF_TEST_CASE_MAC(gtFaUplinkEnable)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call function gtFaUplinkStatusGet with non-NULL wideUplinkFarEndRunningPtr
                                                and narrowUplinkFarEndRunningPtr.
    Expected: GT_OK and the same status.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_BOOL   enable       = GT_FALSE;
    GT_BOOL   wideUplink   = GT_FALSE;
    GT_BOOL   narrowUplink = GT_FALSE;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        enable = GT_FALSE;
        
        st = gtFaUplinkEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call function gtFaUplinkStatusGet with non-NULL wideUplinkFarEndRunningPtr
                                                        and narrowUplinkFarEndRunningPtr.
            Expected: GT_OK and the same status.
        */
        st = gtFaUplinkStatusGet(dev, &wideUplink, &narrowUplink);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtFaUplinkStatusGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, wideUplink,
                 "get another wideUplinkFarEndRunningPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, narrowUplink,
                 "get another narrowUplinkFarEndRunningPtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;
        
        st = gtFaUplinkEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call function gtFaUplinkStatusGet with non-NULL wideUplinkFarEndRunningPtr
                                                        and narrowUplinkFarEndRunningPtr.
            Expected: GT_OK and the same status.
        */
        st = gtFaUplinkStatusGet(dev, &wideUplink, &narrowUplink);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtFaUplinkStatusGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, wideUplink,
                 "get another wideUplinkFarEndRunningPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, narrowUplink,
                 "get another narrowUplinkFarEndRunningPtr than was set: %d", dev);
    }

    enable = GT_TRUE;
        
    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.         */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaUplinkEnable(dev, enable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaUplinkEnable(dev, enable);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaSetBufferReclaim
(
    IN   GT_U8       devNum,
    IN   GT_BOOL     reclaimUcBuf,
    IN   GT_BOOL     reclaimMcBuf
)
*/
UTF_TEST_CASE_MAC(gtFaSetBufferReclaim)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with reclaimUcBuf [GT_TRUE / GT_FALSE]
                            and reclaimMcBuf [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_BOOL   reclaimUcBuf = GT_FALSE;
    GT_BOOL   reclaimMcBuf = GT_FALSE;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with reclaimUcBuf [GT_TRUE / GT_FALSE]
                                    and reclaimMcBuf [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        /* Call with reclaimUcBuf [GT_FALSE] and reclaimMcBuf [GT_FALSE] */
        reclaimUcBuf = GT_FALSE;
        reclaimMcBuf = GT_FALSE;
        
        st = gtFaSetBufferReclaim(dev, reclaimUcBuf, reclaimMcBuf);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, reclaimUcBuf, reclaimMcBuf);

        /* Call with reclaimUcBuf [GT_FALSE] and reclaimMcBuf [GT_TRUE] */
        reclaimMcBuf = GT_TRUE;
        
        st = gtFaSetBufferReclaim(dev, reclaimUcBuf, reclaimMcBuf);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, reclaimUcBuf, reclaimMcBuf);

        /* Call with reclaimUcBuf [GT_TRUE] and reclaimMcBuf [GT_FALSE] */
        reclaimUcBuf = GT_TRUE;
        reclaimMcBuf = GT_FALSE;
        
        st = gtFaSetBufferReclaim(dev, reclaimUcBuf, reclaimMcBuf);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, reclaimUcBuf, reclaimMcBuf);

        /* Call with reclaimUcBuf [GT_TRUE] and reclaimMcBuf [GT_TRUE] */
        reclaimMcBuf = GT_TRUE;
        
        st = gtFaSetBufferReclaim(dev, reclaimUcBuf, reclaimMcBuf);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, reclaimUcBuf, reclaimMcBuf);
    }

    reclaimUcBuf = GT_TRUE;
    reclaimMcBuf = GT_TRUE;
        
    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.         */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaSetBufferReclaim(dev, reclaimUcBuf, reclaimMcBuf);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaSetBufferReclaim(dev, reclaimUcBuf, reclaimMcBuf);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaEgrFCModeEn
(
    IN GT_U8    devNum,    
    IN GT_BOOL  fcModeEnable
)
*/
UTF_TEST_CASE_MAC(gtFaEgrFCModeEn)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with fcModeEnable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call gtFaGetEgrFCModeEn with non-NULL fcModeEnablePtr.
    Expected: GT_OK and the same fcModeEnable.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_BOOL   fcModeEn    = GT_FALSE;
    GT_BOOL   fcModeEnGet = GT_FALSE;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with fcModeEnable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        /* Call with fcModeEnable [GT_FALSE] */
        fcModeEn = GT_FALSE;
        
        st = gtFaEgrFCModeEn(dev, fcModeEn);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fcModeEn);

        /*
            1.2. Call gtFaGetEgrFCModeEn with non-NULL fcModeEnablePtr.
            Expected: GT_OK and the same fcModeEnable.
        */
        st = gtFaGetEgrFCModeEn(dev, &fcModeEnGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtFaGetEgrFCModeEn: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(fcModeEn, fcModeEnGet,
                 "get another fcModeEnable than was set: %d", dev);
        
        /* Call with fcModeEnable [GT_TRUE] */
        fcModeEn = GT_TRUE;
        
        st = gtFaEgrFCModeEn(dev, fcModeEn);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fcModeEn);

        /*
            1.2. Call gtFaGetEgrFCModeEn with non-NULL fcModeEnablePtr.
            Expected: GT_OK and the same fcModeEnable.
        */
        st = gtFaGetEgrFCModeEn(dev, &fcModeEnGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtFaGetEgrFCModeEn: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(fcModeEn, fcModeEnGet,
                 "get another fcModeEnable than was set: %d", dev);
    }

    fcModeEn = GT_TRUE;
        
    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.         */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaEgrFCModeEn(dev, fcModeEn);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaEgrFCModeEn(dev, fcModeEn);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaGetEgrFCModeEn
(
    IN  GT_U8    devNum,    
    OUT GT_BOOL  *fcModeEnablePtr
)
*/
UTF_TEST_CASE_MAC(gtFaGetEgrFCModeEn)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with non-NULL fcModeEnablePtr.
    Expected: GT_OK.
    1.2. Call function with fcModeEnablePtr [NULL].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_BOOL   fcModeEn = GT_FALSE;
    
    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with non-NULL fcModeEnablePtr.
            Expected: GT_OK.
        */
        st = gtFaGetEgrFCModeEn(dev, &fcModeEn);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with fcModeEnablePtr [NULL].
            Expected: NOT GT_OK.
        */
        st = gtFaGetEgrFCModeEn(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, fcModeEnablePtr = NULL", dev);
    }

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.         */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaGetEgrFCModeEn(dev, &fcModeEn);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaGetEgrFCModeEn(dev, &fcModeEn);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaMcastActiveActiveConfig
(
    IN  GT_U8                           devNum,    
    IN  GT_FA_MC_ACTIVE_ACTIVE_FPORT     mcTargetPort
)
*/
UTF_TEST_CASE_MAC(gtFaMcastActiveActiveConfig)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with mcTargetPort [GT_FA_MC_ACTIVE_ACTIVE_FPORT0 /
                                          GT_FA_MC_ACTIVE_ACTIVE_FPORT1 /
                                          GT_FA_MC_ACTIVE_ACTIVE_FPORT0_OR_FPORT1].
    Expected: GT_OK.
    1.2. Call gtFaGetMcastActiveActiveConfig with non-NULL mcTargetPortPtr.
    Expected: GT_OK and the same mcTargetPort.
    1.3. Call function with out of range mcTargetPort [0x5AAAAAA5].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_FA_MC_ACTIVE_ACTIVE_FPORT mcTargetPort    = GT_FA_MC_ACTIVE_ACTIVE_FPORT0;
    GT_FA_MC_ACTIVE_ACTIVE_FPORT mcTargetPortGet = GT_FA_MC_ACTIVE_ACTIVE_FPORT0;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with mcTargetPort [GT_FA_MC_ACTIVE_ACTIVE_FPORT0 /
                                                  GT_FA_MC_ACTIVE_ACTIVE_FPORT1 /
                                                  GT_FA_MC_ACTIVE_ACTIVE_FPORT0_OR_FPORT1].
            Expected: GT_OK.
        */

        /* Call with mcTargetPort [GT_FA_MC_ACTIVE_ACTIVE_FPORT0] */
        mcTargetPort = GT_FA_MC_ACTIVE_ACTIVE_FPORT0;
        
        st = gtFaMcastActiveActiveConfig(dev, mcTargetPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mcTargetPort);

        /*
            1.2. Call gtFaGetMcastActiveActiveConfig with non-NULL mcTargetPortPtr.
            Expected: GT_OK and the same mcTargetPort.
        */
        st = gtFaGetMcastActiveActiveConfig(dev, &mcTargetPortGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtFaGetMcastActiveActiveConfig: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mcTargetPort, mcTargetPortGet,
                 "get another mcTargetPort than was set: %d", dev);

        /* Call with mcTargetPort [GT_FA_MC_ACTIVE_ACTIVE_FPORT1] */
        mcTargetPort = GT_FA_MC_ACTIVE_ACTIVE_FPORT1;
        
        st = gtFaMcastActiveActiveConfig(dev, mcTargetPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mcTargetPort);

        /*
            1.2. Call gtFaGetMcastActiveActiveConfig with non-NULL mcTargetPortPtr.
            Expected: GT_OK and the same mcTargetPort.
        */
        st = gtFaGetMcastActiveActiveConfig(dev, &mcTargetPortGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtFaGetMcastActiveActiveConfig: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mcTargetPort, mcTargetPortGet,
                 "get another mcTargetPort than was set: %d", dev);

        /* Call with mcTargetPort [GT_FA_MC_ACTIVE_ACTIVE_FPORT0_OR_FPORT1] */
        mcTargetPort = GT_FA_MC_ACTIVE_ACTIVE_FPORT0_OR_FPORT1;
        
        st = gtFaMcastActiveActiveConfig(dev, mcTargetPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mcTargetPort);

        /*
            1.2. Call gtFaGetMcastActiveActiveConfig with non-NULL mcTargetPortPtr.
            Expected: GT_OK and the same mcTargetPort.
        */
        st = gtFaGetMcastActiveActiveConfig(dev, &mcTargetPortGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtFaGetMcastActiveActiveConfig: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mcTargetPort, mcTargetPortGet,
                 "get another mcTargetPort than was set: %d", dev);

        /*
            1.3. Call function with out of range mcTargetPort [0x5AAAAAA5].
            Expected: NOT GT_OK.
        */
        mcTargetPort = FA_GEN_INVALID_ENUM_CNS;

        st = gtFaMcastActiveActiveConfig(dev, mcTargetPort);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, mcTargetPort);

        mcTargetPort = GT_FA_MC_ACTIVE_ACTIVE_FPORT0;
    }

    mcTargetPort = GT_FA_MC_ACTIVE_ACTIVE_FPORT0;
        
    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.         */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaMcastActiveActiveConfig(dev, mcTargetPort);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaMcastActiveActiveConfig(dev, mcTargetPort);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaGetMcastActiveActiveConfig
(
    IN  GT_U8                           devNum,    
    OUT GT_FA_MC_ACTIVE_ACTIVE_FPORT    *mcTargetPortPtr
)
*/
UTF_TEST_CASE_MAC(gtFaGetMcastActiveActiveConfig)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with non-NULL mcTargetPortPtr.
    Expected: GT_OK.
    1.2. Call function with mcTargetPortPtr [NULL].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_FA_MC_ACTIVE_ACTIVE_FPORT mcTargetPort = GT_FA_MC_ACTIVE_ACTIVE_FPORT0;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with non-NULL mcTargetPortPtr.
            Expected: GT_OK.
        */
        st = gtFaGetMcastActiveActiveConfig(dev, &mcTargetPort);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with mcTargetPortPtr [NULL].
            Expected: NOT GT_OK.
        */
        st = gtFaGetMcastActiveActiveConfig(dev, NULL);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, mcTargetPortPtr = NULL", dev);
    }

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.         */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaGetMcastActiveActiveConfig(dev, &mcTargetPort);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaGetMcastActiveActiveConfig(dev, &mcTargetPort);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaConfigDevDataExport
(
    IN    GT_U8     devNum,
    INOUT void      *configDevDataBuffer,
    INOUT GT_U32    *configDevDataBufferSize 
)
*/
UTF_TEST_CASE_MAC(gtFaConfigDevDataExport)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with non-NULL configDevDataBuffer
                       and configDevDataBufferSize [0].
    Expected: NOT GT_OK.
    1.2. Call function with non-NULL configDevDataBuffer
                            and configDevDataBufferSize [sizeof (CPSS_FA_HA_CONFIG_DEV_DATA)].
    Expected: GT_OK configDevDataBufferSize = sizeof (CPSS_FA_HA_CONFIG_DEV_DATA).
    1.3. Call function with configDevDataBuffer [NULL]
                            and non-NULL configDevDataBufferSize.
    Expected: NOT GT_OK.
    1.4. Call function with non-NULL configDevDataBuffer
                            and configDevDataBufferSize [NULL].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    CPSS_FA_HA_CONFIG_DEV_DATA configDevDataBuffer;
    GT_U32                     configDevDataBufferSize = 0;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with non-NULL configDevDataBuffer
                                    and configDevDataBufferSize [0].
            Expected: NOT GT_OK.
        */
        configDevDataBufferSize = 0;
        
        st = gtFaConfigDevDataExport(dev, &configDevDataBuffer, &configDevDataBufferSize);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configDevDataBufferSize = %d",
                                         dev, configDevDataBufferSize);

        /*
            1.2. Call function with non-NULL configDevDataBuffer
                                    and configDevDataBufferSize [sizeof (CPSS_FA_HA_CONFIG_DEV_DATA)].
            Expected: GT_OK and the same device type and device revision as current; and configDevDataBufferSize = sizeof (CPSS_FA_HA_CONFIG_DEV_DATA).
        */
        configDevDataBufferSize = sizeof (CPSS_FA_HA_CONFIG_DEV_DATA);
        
        st = gtFaConfigDevDataExport(dev, &configDevDataBuffer, &configDevDataBufferSize);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, configDevDataBufferSize = %d", dev, configDevDataBufferSize);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(sizeof (CPSS_FA_HA_CONFIG_DEV_DATA), configDevDataBufferSize,
                 "get another configDevDataBufferSize than expected: %d", dev);

        /*
            1.3. Call function with configDevDataBuffer [NULL]
                                    and non-NULL configDevDataBufferSize.
            Expected: NOT GT_OK.
        */
        st = gtFaConfigDevDataExport(dev, NULL, &configDevDataBufferSize);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, configDevDataBuffer = NULL", dev);

        /*
            1.4. Call function with non-NULL configDevDataBuffer
                                    and configDevDataBufferSize [NULL].
            Expected: NOT GT_OK.
        */
        st = gtFaConfigDevDataExport(dev, &configDevDataBuffer, NULL);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, configDevDataBuffer = NULL", dev);
    }

    configDevDataBufferSize = sizeof (CPSS_FA_HA_CONFIG_DEV_DATA);
        
    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.         */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaConfigDevDataExport(dev, &configDevDataBuffer, &configDevDataBufferSize);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaConfigDevDataExport(dev, &configDevDataBuffer, &configDevDataBufferSize);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaConfigDevDataImport
(
    IN GT_U8     devNum,
    IN void      *configDevDataBuffer,
    IN GT_U32    configDevDataBufferSize
)
*/
UTF_TEST_CASE_MAC(gtFaConfigDevDataImport)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with non-NULL configDevDataBuffer [{current device settings}/ {GT_FA_98FX902A, 1}]
                            and configDevDataBufferSize [sizeof(CPSS_FA_HA_CONFIG_DEV_DATA).
    Expected: GT_OK.
    1.2. Call gtFaConfigDevDataExport with non-null configDevDataBuffer
                                           and the same configDevDataBufferSize.
    Expected: GT_OK and the same configDevDataBuffer and configDevDataBufferSize.
    1.3. Call function with configDevDataBuffer [NULL]
                            and configDevDataBufferSize the same as in 1.1.
    Expected: NOT GT_OK.
    1.4. Call function with non-NULL configDevDataBuffer
                            and configDevDataBufferSize [0].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    CPSS_FA_HA_CONFIG_DEV_DATA configDevDataBuffer;
    CPSS_FA_HA_CONFIG_DEV_DATA configDevDataBufferGet;
    GT_U32                     configDevDataBufferSize = 0;

    GT_FA_DEVICE               devType;
    GT_U32                     revision;
    
    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* Getting device type */
        st = prvUtfFaDeviceTypeGet(dev, &devType);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfFaDeviceTypeGet :%d", dev);

        /* Getting device revision */
        st = prvUtfFaDeviceRevisionGet(dev, &revision);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfFaDeviceRevisionGet :%d", dev);

        /* 
            1.1. Call function with non-NULL configDevDataBuffer {current device settings/ {GT_FA_98FX902A, 1}}
                                    and configDevDataBufferSize [sizeof(CPSS_FA_HA_CONFIG_DEV_DATA).
            Expected: GT_OK.
        */
        /*
            1.2. Call gtFaConfigDevDataExport with non-null configDevDataBuffer
                                                   and the same configDevDataBufferSize.
            Expected: GT_OK and the same configDevDataBuffer and configDevDataBufferSize.
        */

        /* 1.1. for configDevDataBuffer[{current device settings}] */
        configDevDataBuffer.faDevType  = devType;
        configDevDataBuffer.faRevision = (GT_U8) revision;

        configDevDataBufferSize = sizeof(CPSS_FA_HA_CONFIG_DEV_DATA);
        
        st = gtFaConfigDevDataImport(dev, &configDevDataBuffer, configDevDataBufferSize);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, configDevDataBufferSize = %d",
                                     dev, configDevDataBufferSize);
        /* 1.2. for configDevDataBuffer[{current device settings}] */
        st = gtFaConfigDevDataExport(dev, &configDevDataBufferGet, &configDevDataBufferSize);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtFaConfigDevDataExport: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(configDevDataBuffer.faDevType, configDevDataBufferGet.faDevType,
            "gtFaConfigDevDataExport: get another configDevDataBuffer->faDevType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(configDevDataBuffer.faRevision, configDevDataBufferGet.faRevision,
            "gtFaConfigDevDataExport: get another configDevDataBuffer->faRevision than was set: %d", dev);

        /* 1.1. configDevDataBuffer[{GT_FA_98FX902A, 1}] */
        configDevDataBuffer.faDevType  = GT_FA_98FX902A;
        configDevDataBuffer.faRevision = 1;

        configDevDataBufferSize = sizeof(CPSS_FA_HA_CONFIG_DEV_DATA);
        
        st = gtFaConfigDevDataImport(dev, &configDevDataBuffer, configDevDataBufferSize);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, configDevDataBufferSize = %d",
                                     dev, configDevDataBufferSize);
        /* 1.2. configDevDataBuffer[{GT_FA_98FX902A, 1}] */
        st = gtFaConfigDevDataExport(dev, &configDevDataBufferGet, &configDevDataBufferSize);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtFaConfigDevDataExport: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(configDevDataBuffer.faDevType, configDevDataBufferGet.faDevType,
            "gtFaConfigDevDataExport: get another configDevDataBuffer->faDevType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(configDevDataBuffer.faRevision, configDevDataBufferGet.faRevision,
            "gtFaConfigDevDataExport: get another configDevDataBuffer->faRevision than was set: %d", dev);

        /*
            1.3. Call function with configDevDataBuffer [NULL]
                                    and configDevDataBufferSize the same as in 1.1.
            Expected: NOT GT_OK.
        */
        st = gtFaConfigDevDataImport(dev, NULL, configDevDataBufferSize);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st,
                                         "%d, configDevDataBuffer = NULL", dev);

        /*
            1.4. Call function with non-NULL configDevDataBuffer
                                    and configDevDataBufferSize [0].
            Expected: NOT GT_OK.
        */
        configDevDataBufferSize = 0;

        st = gtFaConfigDevDataImport(dev, &configDevDataBuffer, configDevDataBufferSize);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
            "%d, configDevDataBufferSize = %d", dev, configDevDataBufferSize);
    }

    configDevDataBuffer.faDevType  = GT_FA_98FX900A;
    configDevDataBuffer.faRevision = 0;
    configDevDataBufferSize = sizeof (CPSS_FA_HA_CONFIG_DEV_DATA);
        
    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaConfigDevDataImport(dev, &configDevDataBuffer, configDevDataBufferSize);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaConfigDevDataImport(dev, &configDevDataBuffer, configDevDataBufferSize);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
* Configuration of gtFaApiGen suit
*/
UTF_SUIT_BEGIN_TESTS_MAC(gtFaApiGen)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaSetActiveFport)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaUplinkReset)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaUplinkStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaUplinkEnable)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaSetBufferReclaim)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaEgrFCModeEn)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaGetEgrFCModeEn)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaMcastActiveActiveConfig)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaGetMcastActiveActiveConfig)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaConfigDevDataExport)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaConfigDevDataImport)
UTF_SUIT_END_TESTS_MAC(gtFaApiGen)

