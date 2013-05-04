/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtFaVoqApiUT.c
*
* DESCRIPTION:
*       Prestera API implementation for Prestera Fabric Adapter
*       Virtual Output Queue (VOQ) unit.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpssFa/generic/voq/gtVoq.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* Invalid enum */
#define VOQ_INVALID_ENUM_CNS    0x5AAAAAA5

/* Default valid value for fabric port id */
#define VOQ_VALID_FA_PORT_CNS   0


/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqGetMaxHwFports
(
    IN  GT_U8     devNum,
    OUT GT_U32   *maxFports
)
*/
UTF_TEST_CASE_MAC(gtFaVoqGetMaxHwFports)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with non-NULL maxFports.
    Expected: GT_OK.
    1.2. Call function with maxFports [NULL].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32    maxFports = 0;
    
    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with non-NULL maxFports.
            Expected: GT_OK. 
        */
        st = gtFaVoqGetMaxHwFports(dev, &maxFports);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with maxFports [NULL].
            Expected: NOT GT_OK.
        */
        st = gtFaVoqGetMaxHwFports(dev, NULL);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, maxFports = NULL", dev);
    }

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqGetMaxHwFports(dev, &maxFports);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqGetMaxHwFports(dev, &maxFports);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqAutoFlushOnFportDisable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(gtFaVoqAutoFlushOnFportDisable)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_BOOL   enable = GT_FALSE;


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
        
        st = gtFaVoqAutoFlushOnFportDisable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;
        
        st = gtFaVoqAutoFlushOnFportDisable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    enable = GT_TRUE;
        
    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqAutoFlushOnFportDisable(dev, enable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqAutoFlushOnFportDisable(dev, enable);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqFportTxEnable
(
    IN GT_U8   devNum,
    IN GT_U8   fport,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(gtFaVoqFportTxEnable)
{
/*
    ITERATE_FA_PORTS
    1.1. Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;
    GT_U8     fport;

    GT_BOOL   enable       = GT_FALSE;
    GT_U8     maxFportsNum = 0;
    

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* Getting max number of fabric ports */
        st = prvUtfNumFaPortsGet(dev, &maxFportsNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfNumFaPortsGet: %d", dev);

        /* prepare fabric port iterator */
        st = prvUtfNextFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while (GT_OK == prvUtfNextFaPortGet(&fport, GT_TRUE))
        {
            /* 
                1.1. Call function with enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */
    
            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;
            
            st = gtFaVoqFportTxEnable(dev, fport, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, enable);
    
            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;
            
            st = gtFaVoqFportTxEnable(dev, fport, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, enable);
        }

        enable = GT_TRUE;

        /* prepare fabric port iterator */
        st = prvUtfNextFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non active fabric ports */
        while (GT_OK == prvUtfNextFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active fabric port */
            st = gtFaVoqFportTxEnable(dev, fport, enable);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);
        }

        /* 1.3. For active device check that function returns NOT GT_OK    */
        /* for out of bound value for port number.                         */
        fport = maxFportsNum;

        st = gtFaVoqFportTxEnable(dev, fport, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = VOQ_VALID_FA_PORT_CNS;
    
    /* prepare device iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqFportTxEnable(dev, fport, enable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqFportTxEnable(dev, fport, enable);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqMcTxEn
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(gtFaVoqMcTxEn)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_BOOL   enable = GT_FALSE;


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
        
        st = gtFaVoqMcTxEn(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;
        
        st = gtFaVoqMcTxEn(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    enable = GT_TRUE;
        
    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqMcTxEn(dev, enable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqMcTxEn(dev, enable);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqFlushAllTrigger
(
    IN GT_U8   devNum
)
*/
UTF_TEST_CASE_MAC(gtFaVoqFlushAllTrigger)
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
        st = gtFaVoqFlushAllTrigger(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqFlushAllTrigger(dev);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqFlushAllTrigger(dev);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqFportFlushTrigger
(
    IN GT_U8   devNum,
    IN GT_U8   fport
)
*/
UTF_TEST_CASE_MAC(gtFaVoqFportFlushTrigger)
{
/*
    ITERATE_FA_ PORTS
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;
    GT_U8     fport;

    GT_U8     maxFportsNum = 0;
    

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* Getting max number of fabric ports */
        st = prvUtfNumFaPortsGet(dev, &maxFportsNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfNumFaPortsGet: %d", dev);

        /* prepare fabric port iterator */
        st = prvUtfNextFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while (GT_OK == prvUtfNextFaPortGet(&fport, GT_TRUE))
        {
            st = gtFaVoqFportFlushTrigger(dev, fport);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);
        }

        /* prepare fabric port iterator */
        st = prvUtfNextFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non active fabric ports */
        while (GT_OK == prvUtfNextFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active fabric port */
            st = gtFaVoqFportFlushTrigger(dev, fport);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);
        }

        /* 1.3. For active device check that function returns NOT GT_OK */
        /* for out of bound value for port number.                      */
        fport = maxFportsNum;

        st = gtFaVoqFportFlushTrigger(dev, fport);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = VOQ_VALID_FA_PORT_CNS;
    
    /* prepare device iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqFportFlushTrigger(dev, fport);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqFportFlushTrigger(dev, fport);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqMcFlushTrigger
(
    IN GT_U8   devNum
)
*/
UTF_TEST_CASE_MAC(gtFaVoqMcFlushTrigger)
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
        st = gtFaVoqMcFlushTrigger(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqMcFlushTrigger(dev);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqMcFlushTrigger(dev);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqGetMaxHwVoqDescriptors
(
    IN  GT_U8   devNum,
    OUT GT_U32 *maxHwVoqDescriptors
)
*/
UTF_TEST_CASE_MAC(gtFaVoqGetMaxHwVoqDescriptors)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with non-NULL maxHwVoqDescriptors.
    Expected: GT_OK.
    1.2. Call function with maxHwVoqDescriptors [NULL].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32    maxHwVoqDescriptors = 0;
    

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with non-NULL maxHwVoqDescriptors.
            Expected: GT_OK.
        */
        st = gtFaVoqGetMaxHwVoqDescriptors(dev, &maxHwVoqDescriptors);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with maxHwVoqDescriptors [NULL].
            Expected: NOT GT_OK.
        */
        st = gtFaVoqGetMaxHwVoqDescriptors(dev, NULL);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, maxHwVoqDescriptors = NULL", dev);
    }

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqGetMaxHwVoqDescriptors(dev, &maxHwVoqDescriptors);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqGetMaxHwVoqDescriptors(dev, &maxHwVoqDescriptors);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqSetGlobalMaxDescriptorLimit
(
    IN GT_U8  devNum,
    IN GT_U16 maxDescriptors
)
*/
UTF_TEST_CASE_MAC(gtFaVoqSetGlobalMaxDescriptorLimit)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with maxDescriptors [0 / 100].
    Expected: GT_OK.
    1.2. Call function with maxDescriptors [0xFFFF] (no any constraints).
    Expected: GT_OK.
    1.3. Call gtFaVoqGetMaxHwVoqDescriptors with non-NULL maxHwVoqDescriptors.
    Expected: GT_OK and the same maxDescriptors.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U16    maxDescriptors    = 0;
    GT_U32    maxDescriptorsGet = 0;


    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with maxDescriptors [0 / 100].
            Expected: GT_OK.
        */

        /* Call with maxDescriptors [0] */
        maxDescriptors = 0;
        
        st = gtFaVoqSetGlobalMaxDescriptorLimit(dev, maxDescriptors);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, maxDescriptors);

        /* Call with maxDescriptors [100] */
        maxDescriptors = 100;
        
        st = gtFaVoqSetGlobalMaxDescriptorLimit(dev, maxDescriptors);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, maxDescriptors);

        /*
            1.3. Call gtFaVoqGetMaxHwVoqDescriptors with non-NULL maxHwVoqDescriptors.
            Expected: GT_OK and the same maxDescriptors.
        */
        st = gtFaVoqGetMaxHwVoqDescriptors(dev, &maxDescriptorsGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtFaVoqGetMaxHwVoqDescriptors: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(maxDescriptors - maxDescriptors % 32, maxDescriptorsGet,
                 "get another maxDescriptors than was set: %d", dev);

        /*
            1.2. Call function with maxDescriptors [0xFFFF] (no any constraints).
            Expected: GT_OK.
        */
        maxDescriptors = 0xFFFF;
        
        st = gtFaVoqSetGlobalMaxDescriptorLimit(dev, maxDescriptors);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, maxDescriptors);

        maxDescriptors = 0;
    }

    maxDescriptors = 0;
        
    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqSetGlobalMaxDescriptorLimit(dev, maxDescriptors);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqSetGlobalMaxDescriptorLimit(dev, maxDescriptors);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqBindFport2ConfigSet
(
    IN GT_U8   devNum,
    IN GT_U8   fport,
    IN GT_U8   setNum
)
*/
UTF_TEST_CASE_MAC(gtFaVoqBindFport2ConfigSet)
{
/*
    ITERATE_FA_PORTS
    1.1. Call function with setNum [0 / 2 / MAX_FA_DESC_SET  = 3].
    Expected: GT_OK.
    1.2. Call function with out of range setNum [MAX_FA_DESC_SET + 1].
    Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;
    GT_U8     fport;

    GT_U8     maxFportsNum = 0;
    GT_U8     setNum       = 0;
    

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* Getting max number of fabric ports */
        st = prvUtfNumFaPortsGet(dev, &maxFportsNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfNumFaPortsGet: %d", dev);

        /* prepare fabric port iterator */
        st = prvUtfNextFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while (GT_OK == prvUtfNextFaPortGet(&fport, GT_TRUE))
        {
            /* 
                1.1. Call function with setNum [0 / 2 / MAX_FA_DESC_SET  = 3].
                Expected: GT_OK.
            */
    
            /* Call with setNum [0] */
            setNum = 0;
            
            st = gtFaVoqBindFport2ConfigSet(dev, fport, setNum);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, setNum);
    
            /* Call with setNum [2] */
            setNum = 2;
            
            st = gtFaVoqBindFport2ConfigSet(dev, fport, setNum);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, setNum);

            /* Call with setNum [MAX_FA_DESC_SET] */
            setNum = MAX_FA_DESC_SET;
            
            st = gtFaVoqBindFport2ConfigSet(dev, fport, setNum);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, setNum);

            /*
                1.2. Call function with out of range setNum [MAX_FA_DESC_SET + 1].
                Expected: NON GT_OK.
            */
            setNum = MAX_FA_DESC_SET + 1;
            
            st = gtFaVoqBindFport2ConfigSet(dev, fport, setNum);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, setNum);
        }

        setNum = 0;

        /* prepare fabric port iterator */
        st = prvUtfNextFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non active fabric ports */
        while (GT_OK == prvUtfNextFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active fabric port */
            st = gtFaVoqBindFport2ConfigSet(dev, fport, setNum);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);
        }

        /* 1.3. For active device check that function returns NOT GT_OK */
        /* for out of bound value for port number.                      */
        fport = maxFportsNum;

        st = gtFaVoqBindFport2ConfigSet(dev, fport, setNum);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);
    }

    setNum = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = VOQ_VALID_FA_PORT_CNS;
    
    /* prepare device iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqBindFport2ConfigSet(dev, fport, setNum);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqBindFport2ConfigSet(dev, fport, setNum);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqBindMc2ConfigSet
(
    IN GT_U8   devNum,
    IN GT_U8   setNum
)
*/
UTF_TEST_CASE_MAC(gtFaVoqBindMc2ConfigSet)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with setNum [0 / 2 / MAX_FA_DESC_SET = 3].
    Expected: GT_OK.
    1.2. Call function with out of range setNum [MAX_FA_DESC_SET + 1].
    Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U8     setNum = 0;
    

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with setNum [0 / 2 / MAX_FA_DESC_SET  = 3].
            Expected: GT_OK.
        */
    
        /* Call with setNum [0] */
        setNum = 0;
            
        st = gtFaVoqBindMc2ConfigSet(dev, setNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, setNum);
    
        /* Call with setNum [2] */
        setNum = 2;
            
        st = gtFaVoqBindMc2ConfigSet(dev, setNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, setNum);

        /* Call with setNum [MAX_FA_DESC_SET] */
        setNum = MAX_FA_DESC_SET;
            
        st = gtFaVoqBindMc2ConfigSet(dev, setNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, setNum);

        /*
            1.2. Call function with out of range setNum [MAX_FA_DESC_SET + 1].
            Expected: NON GT_OK.
        */
        setNum = MAX_FA_DESC_SET + 1;
            
        st = gtFaVoqBindMc2ConfigSet(dev, setNum);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, setNum);
    }

    setNum = 0;
        
    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqBindMc2ConfigSet(dev, setNum);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqBindMc2ConfigSet(dev, setNum);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqFportConfigSetMaxDescriptorLimit
(
    IN GT_U8   devNum,
    IN GT_U8   setNum,
    IN GT_U32  maxDescriptors
)
*/
UTF_TEST_CASE_MAC(gtFaVoqFportConfigSetMaxDescriptorLimit)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with setNum [0 / MAX_FA_DESC_SET = 3] and maxDescriptors [0 / 100].
    Expected: GT_OK.
    1.2. Call function with setNum [2] and maxDescriptors [0xFFFF] (no any constraints).
    Expected: GT_OK.
    1.3. Call function with out of range setNum [MAX_FA_DESC_SET + 1] and maxDescriptors the same as in 1.1.
    Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U8     setNum         = 0;
    GT_U32    maxDescriptors = 0;
    

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with setNum [0 / MAX_FA_DESC_SET = 3] and maxDescriptors [0 / 100].
            Expected: GT_OK.
        */
    
        /* Call with setNum [0] and maxDescriptors [0] */
        setNum         = 0;
        maxDescriptors = 0;
            
        st = gtFaVoqFportConfigSetMaxDescriptorLimit(dev, setNum, maxDescriptors);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, setNum, maxDescriptors);
    
        /* Call with setNum [MAX_FA_DESC_SET] and maxDescriptors [100] */
        setNum         = MAX_FA_DESC_SET;
        maxDescriptors = 100;
            
        st = gtFaVoqFportConfigSetMaxDescriptorLimit(dev, setNum, maxDescriptors);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, setNum, maxDescriptors);

        /*
            1.2. Call function with setNum [2] and maxDescriptors [0xFFFF] (no any constraints).
            Expected: GT_OK.
        */
        setNum         = 2;
        maxDescriptors = 0xFFFF;
            
        st = gtFaVoqFportConfigSetMaxDescriptorLimit(dev, setNum, maxDescriptors);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, setNum, maxDescriptors);

        maxDescriptors = 0;

        /*
            1.3. Call function with out of range setNum [MAX_FA_DESC_SET + 1].
            Expected: NON GT_OK.
        */
        setNum = MAX_FA_DESC_SET + 1;
            
        st = gtFaVoqFportConfigSetMaxDescriptorLimit(dev, setNum, maxDescriptors);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, setNum);
    }

    setNum         = 0;
    maxDescriptors = 0;
        
    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqFportConfigSetMaxDescriptorLimit(dev, setNum, maxDescriptors);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqFportConfigSetMaxDescriptorLimit(dev, setNum, maxDescriptors);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqFportDpConfigSetMaxDescriptorLimit
(
    IN GT_U8   devNum,
    IN GT_U8   setNum,
    IN GT_U8   fpPrio,
    IN GT_U8   dp,
    IN GT_U32  maxDescriptors
)
*/
UTF_TEST_CASE_MAC(gtFaVoqFportDpConfigSetMaxDescriptorLimit)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with setNum [0 / MAX_FA_DESC_SET = 3],
                            fpPrio [0 / MAX_FA_FPORT_PRIO = 3],
                            dp [0 / 2]
                            and maxDescriptors [0 / 100].
    Expected: GT_OK.
    1.2. Call function with setNum [2],
                            fpPrio [2],
                            dp [2]
                            and  maxDescriptors [0xFFFF] (no any constraints).
    Expected: GT_OK.
    1.3. Call function with out of range setNum [MAX_FA_DESC_SET + 1]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.4. Call function with out of range fpPrio [MAX_FA_FPORT_PRIO + 1]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.5. Call function with out of range dp [3]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U8     setNum         = 0;
    GT_U8     fpPrio         = 0;
    GT_U8     dp             = 0;
    GT_U32    maxDescriptors = 0;
    

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with setNum [0 / MAX_FA_DESC_SET = 3],
                                    fpPrio [0 / MAX_FA_FPORT_PRIO = 3],
                                    dp [0 / 2]
                                    and maxDescriptors [0 / 100].
            Expected: GT_OK.
        */
    
        /* Call with setNum [0], fpPrio [0], dp [0], maxDescriptors [0] */
        setNum         = 0;
        fpPrio         = 0;
        dp             = 0;
        maxDescriptors = 0;
            
        st = gtFaVoqFportDpConfigSetMaxDescriptorLimit(dev, setNum, fpPrio, dp, maxDescriptors);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, setNum, fpPrio, dp, maxDescriptors);
    
        /* Call with setNum [MAX_FA_DESC_SET], fpPrio [MAX_FA_FPORT_PRIO], dp [2], maxDescriptors [100] */
        setNum         = MAX_FA_DESC_SET;
        fpPrio         = MAX_FA_FPORT_PRIO;
        dp             = 2;
        maxDescriptors = 100;
            
        st = gtFaVoqFportDpConfigSetMaxDescriptorLimit(dev, setNum, fpPrio, dp, maxDescriptors);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, setNum, fpPrio, dp, maxDescriptors);

        /*
            1.2. Call function with setNum [2],
                                    fpPrio [2],
                                    dp [2]
                                    and  maxDescriptors [0xFFFF] (no any constraints).
            Expected: GT_OK.
        */
        setNum         = 2;
        fpPrio         = 2;
        dp             = 2;
        maxDescriptors = 0xFFFF;
            
        st = gtFaVoqFportDpConfigSetMaxDescriptorLimit(dev, setNum, fpPrio, dp, maxDescriptors);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, maxDescriptors = %d", dev, maxDescriptors);

        /*
            1.3. Call function with out of range setNum [MAX_FA_DESC_SET + 1]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        fpPrio         = 0;
        dp             = 0;
        maxDescriptors = 0;
        setNum = MAX_FA_DESC_SET + 1;
            
        st = gtFaVoqFportDpConfigSetMaxDescriptorLimit(dev, setNum, fpPrio, dp, maxDescriptors);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, setNum);

        setNum = 0;

        /*
            1.4. Call function with out of range fpPrio [MAX_FA_FPORT_PRIO + 1]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        fpPrio = MAX_FA_FPORT_PRIO + 1;
            
        st = gtFaVoqFportDpConfigSetMaxDescriptorLimit(dev, setNum, fpPrio, dp, maxDescriptors);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fpPrio = %d", dev, fpPrio);

        fpPrio = 0;

        /*
            1.5. Call function with out of range dp [3]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        dp = 3;
            
        st = gtFaVoqFportDpConfigSetMaxDescriptorLimit(dev, setNum, fpPrio, dp, maxDescriptors);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dp = %d", dev, dp);
    }

    setNum         = 0;
    fpPrio         = 0;
    dp             = 0;
    maxDescriptors = 0;

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqFportDpConfigSetMaxDescriptorLimit(dev, setNum, fpPrio, dp, maxDescriptors);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqFportDpConfigSetMaxDescriptorLimit(dev, setNum, fpPrio, dp, maxDescriptors);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqTc2FprioMapSet
(
    IN GT_U8  devNum,
    IN GT_U8  trafClass,
    IN GT_U8  fPrio
)
*/
UTF_TEST_CASE_MAC(gtFaVoqTc2FprioMapSet)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function wiith trafClass [0 / 5 / CPSS_TC_RANGE_CNS - 1]
                             and fPrio [0 / 2 / MAX_FA_FPORT_PRIO = 3].
    Expected: GT_OK.
    1.2. Call function with out of range trafClass [CPSS_TC_RANGE_CNS]
                            and fPrio the same as in 1.1.
    Expected: NOT GT_OK.
    1.3. Call function with out of range fPrio [MAX_FA_FPORT_PRIO + 1]
                            and trafClass the same as in 1.1.
    Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U8     trafClass = 0;
    GT_U8     fPrio     = 0;
    

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function wiith trafClass [0 / 5 / CPSS_TC_RANGE_CNS - 1]
                                     and fPrio [0 / 2 / MAX_FA_FPORT_PRIO = 3].
            Expected: GT_OK.
        */
    
        /* Call with trafClass [0], fpPrio [0] */
        trafClass = 0;
        fPrio     = 0;
        
        st = gtFaVoqTc2FprioMapSet(dev, trafClass, fPrio);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trafClass, fPrio);
    
        /* Call with trafClass [5], fpPrio [2] */
        trafClass = 5;
        fPrio    = 2;
        
        st = gtFaVoqTc2FprioMapSet(dev, trafClass, fPrio);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trafClass, fPrio);

        /* Call with trafClass [CPSS_TC_RANGE_CNS - 1], fpPrio [MAX_FA_FPORT_PRIO] */
        trafClass = 7;
        fPrio     = MAX_FA_FPORT_PRIO;
        
        st = gtFaVoqTc2FprioMapSet(dev, trafClass, fPrio);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trafClass, fPrio);

        /*
            1.2. Call function with out of range trafClass [CPSS_TC_RANGE_CNS]
                                    and fPrio the same as in 1.1.
            Expected: NOT GT_OK.
        */
        trafClass = 8;
            
        st = gtFaVoqTc2FprioMapSet(dev, trafClass, fPrio);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trafClass);

        trafClass = 0;

        /*
            1.3. Call function with out of range fPrio [MAX_FA_FPORT_PRIO + 1]
                                    and trafClass the same as in 1.1.
            Expected: NON GT_OK. 
        */
        fPrio = MAX_FA_FPORT_PRIO + 1;
            
        st = gtFaVoqTc2FprioMapSet(dev, trafClass, fPrio);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fPrio = %d", dev, fPrio);
    }

    trafClass = 0;
    fPrio    = 0;

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqTc2FprioMapSet(dev, trafClass, fPrio);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqTc2FprioMapSet(dev, trafClass, fPrio);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqAuMcTcSet
(
    IN GT_U8  devNum,
    IN GT_U8  tc
)
*/
UTF_TEST_CASE_MAC(gtFaVoqAuMcTcSet)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with tc [0 / 5 / CPSS_TC_RANGE_CNS - 1].
    Expected: GT_Ok.
    1.2. Call function with out of range tc [CPSS_TC_RANGE_CNS].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U8     tc = 0;
    

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with tc [0 / 5 / CPSS_TC_RANGE_CNS - 1].
            Expected: GT_Ok.
        */
    
        /* Call with tc [0] */
        tc = 0;
        
        st = gtFaVoqAuMcTcSet(dev, tc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tc);
    
        /* Call with tc [5] */
        tc = 5;
        
        st = gtFaVoqAuMcTcSet(dev, tc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tc);

        /* Call with tc [CPSS_TC_RANGE_CNS - 1] */
        tc = 7;
        
        st = gtFaVoqAuMcTcSet(dev, tc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tc);

        /*
            1.2. Call function with out of range tc [CPSS_TC_RANGE_CNS].
            Expected: NOT GT_OK.
        */
        tc = 8;
            
        st = gtFaVoqAuMcTcSet(dev, tc);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tc);
    }

    tc = 0;
    
    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqAuMcTcSet(dev, tc);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqAuMcTcSet(dev, tc);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqDevToFportMap
(
    IN GT_U8    devNum,
    IN GT_U8    tarDev,
    IN GT_U8    fport[LBH_SIZE]
)
*/
UTF_TEST_CASE_MAC(gtFaVoqDevToFportMap)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with tarDev [0 / 32 / FA_64_DEV_LBH - 1]
                            and non-NULL fport [{0, 2, 4, getMaxPortNum(dev) - 1}].
    Expected: GT_OK.
    1.2. Call function with out of range tarDev [FA_64_DEV_LBH = 64]
                            and fport the same as in 1.1.
    Expected: NON GT_OK when Load Balancing is enabled and GT_OK when Load Balancing is disabled.
    1.3. Call function with out of range tarDev [FA_128_DEV_NON_LBH = 128]
                            and fport the same as in 1.1.
    Expected: NON GT_OK for both Load Balancing modes.
    1.4. Call function with fport [0, {getMaxPortNum(dev)}] (only fport[0] is relevant when Load Balancing is not used)
                            and tarDev [4].
    Expected: NON GT_OK when LBH mode enabled and GT_OK when LBH mode disabled.
    1.5. Call function with fport [{getMaxPortNum(dev)}]
                            and tarDev [2].
    Expected: NON GT_OK.
    1.6. Call function with fport [NULL]
                            and tarDev the same as in 1.1.
    Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U8             tarDev       = 0;
    GT_U8             fport[LBH_SIZE];
    GT_U8             maxFportsNum = 0;
    GT_FA_MAX_NUM_DEV maxDevNum    = FA_64_DEV_LBH;
    

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* Getting max number of fabric ports */
        st = prvUtfNumFaPortsGet(dev, &maxFportsNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfNumFaPortsGet: %d", dev);

        /* Getting max number of packet processors */
        st = prvUtfFaMaxNumPpGet(dev, &maxDevNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfFaMaxNumPpGet: %d", dev);

        /* 
            1.1. Call function with tarDev [0 / 32 / FA_64_DEV_LBH - 1]
                                    and non-NULL fport [{0, 2, 4, getMaxPortNum(dev) - 1}].
            Expected: GT_OK.
        */
        fport[0] = 0;
        fport[1] = 2;
        fport[2] = 4;
        fport[3] = maxFportsNum - 1;
    
        /* Call with tarDev [0] */
        tarDev = 0;
        
        st = gtFaVoqDevToFportMap(dev, tarDev, fport);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tarDev);
    
        /* Call with tarDev [32] */
        tarDev = 32;
        
        st = gtFaVoqDevToFportMap(dev, tarDev, fport);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tarDev);

        /* Call with tarDev [FA_64_DEV_LBH - 1] */
        tarDev = FA_64_DEV_LBH - 1;
        
        st = gtFaVoqDevToFportMap(dev, tarDev, fport);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tarDev);

        /*
            1.2. Call function with out of range tarDev [FA_64_DEV_LBH = 64]
                                    and fport the same as in 1.1.
            Expected: NON GT_OK when Load Balancing is enabled and GT_OK when Load Balancing is disabled.
        */
        tarDev = FA_64_DEV_LBH;
            
        st = gtFaVoqDevToFportMap(dev, tarDev, fport);
        if (FA_64_DEV_LBH == maxDevNum)
        {
            /* LBH mode enabled */
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tarDev);
        }
        else
        {
            /* LBH mode disabled */
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tarDev);
        }

        tarDev = 0;

        /*
            1.3. Call function with out of range tarDev [FA_128_DEV_NON_LBH = 128]
                                    and fport the same as in 1.1.
            Expected: NON GT_OK for both Load Balancing modes.
            
            1.5. Call function with fport [NULL]
                                    and tarDev the same as in 1.1.
            Expected: NON GT_OK.
        */
        tarDev = FA_128_DEV_NON_LBH;
            
        st = gtFaVoqDevToFportMap(dev, tarDev, fport);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tarDev);

        tarDev = 0;

        /*
            1.4. Call function with fport [0, {getMaxPortNum(dev)}] (only fport[0] is relevant when Load Balancing is not used)
                                    and tarDev [4].
            Expected: NON GT_OK when LBH mode enabled and GT_OK when LBH mode disabled.
        */
        fport[1] = maxFportsNum;

        st = gtFaVoqDevToFportMap(dev, tarDev, fport);
        if (FA_64_DEV_LBH == maxDevNum)
        {
            /* LBH mode enabled */
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport[1] = %d", dev, fport[1]);
        }
        else
        {
            /* LBH mode disabled */
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport[1] = %d", dev, fport[1]);
        }

        fport[1] = 2;

        /*
            1.5. Call function with fport [{getMaxPortNum(dev)}]
                                    and tarDev [2].
            Expected: NON GT_OK.
        */
        fport[0] = maxFportsNum;

        st = gtFaVoqDevToFportMap(dev, tarDev, fport);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport[0] = %d", dev, fport[0]);

        fport[0] = 0;

        /*
            1.6. Call function with fport [NULL]
                                    and tarDev the same as in 1.1.
            Expected: NON GT_OK.
        */
        st = gtFaVoqDevToFportMap(dev, tarDev, NULL);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, fport = NULL", dev);
    }

    fport[0] = 0;
    fport[1] = 2;
    fport[2] = 4;
    fport[3] = 0;
        
    tarDev = 0;
        
    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqDevToFportMap(dev, tarDev, fport);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqDevToFportMap(dev, tarDev, fport);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqSetDevTbl
(
    IN GT_U8    devNum,
    IN GT_U8    tarDev,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(gtFaVoqSetDevTbl)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with tarDev [0 / FA_64_DEV_LBH - 1] and enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call function with out of range tarDev [FA_128_DEV_NON_LBH = 128].
    Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U8     tarDev = 0;
    GT_BOOL   enable = GT_FALSE;
    

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with tarDev [0 / FA_64_DEV_LBH - 1]
                                    and enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */
        
        /* Call with tarDev [0] and enable [GT_FALSE] */
        tarDev = 0;
        enable = GT_FALSE;
        
        st = gtFaVoqSetDevTbl(dev, tarDev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    
        /* Call with tarDev [FA_64_DEV_LBH - 1] and enable [GT_TRUE] */
        tarDev = FA_64_DEV_LBH - 1;
        enable = GT_TRUE;
        
        st = gtFaVoqSetDevTbl(dev, tarDev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call function with out of range tarDev [FA_128_DEV_NON_LBH = 128].
            Expected: NON GT_OK.
        */
        tarDev = FA_128_DEV_NON_LBH;
            
        st = gtFaVoqSetDevTbl(dev, tarDev, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tarDev);
    }

    tarDev = 0;
    enable = GT_FALSE;
        
    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqSetDevTbl(dev, tarDev, enable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqSetDevTbl(dev, tarDev, enable);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqClearDevTbl
(
    IN GT_U8    devNum
)
*/
UTF_TEST_CASE_MAC(gtFaVoqClearDevTbl)
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
        st = gtFaVoqClearDevTbl(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqClearDevTbl(dev);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqClearDevTbl(dev);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqFportWatchdogEn
(
    IN GT_U8   devNum,
    IN GT_U8   fport,
    IN GT_BOOL enable,
    IN GT_U32  timeout
)
*/
UTF_TEST_CASE_MAC(gtFaVoqFportWatchdogEn)
{
/*
    ITERATE_FA_PORTS
    1.1. Call function wih enable [GT_TRUE / GT_FALSE]
                           and timeout [1 / 100 / 4095].
    Expected: GT_OK.
    1.2. Call function with out of range timeout [0 / 4096]
                            and enable [GT_TRUE].
    Expected: NON GT_OK.
    1.3. Call function with enable [GT_FALSE]
                            and timeout [0 / 4096] (is ignored).
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;
    GT_U8     fport;

    GT_BOOL   enable       = GT_FALSE;
    GT_U32    timeout      = 0;
    GT_U8     maxFportsNum = 0;
    

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* Getting max number of fabric ports */
        st = prvUtfNumFaPortsGet(dev, &maxFportsNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfNumFaPortsGet: %d", dev);

        /* prepare fabric port iterator */
        st = prvUtfNextFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while (GT_OK == prvUtfNextFaPortGet(&fport, GT_TRUE))
        {
            /* 
                1.1. Call function wih enable [GT_TRUE / GT_FALSE]
                                       and timeout [1 / 100 / 4095].
                Expected: GT_OK.
            */
    
            /* Call with enable [GT_FALSE] and timeout [4095] */
            timeout = 4095;
            
            st = gtFaVoqFportWatchdogEn(dev, fport, enable, timeout);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, enable, timeout);

            /* Call with enable [GT_TRUE] and timeout [1] */
            enable  = GT_TRUE;
            timeout = 1;
            
            st = gtFaVoqFportWatchdogEn(dev, fport, enable, timeout);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, enable, timeout);
    
            /* Call with enable [GT_TRUE] and timeout [100] */
            timeout = 100;
            
            st = gtFaVoqFportWatchdogEn(dev, fport, enable, timeout);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, enable, timeout);

            /* Call with enable [GT_TRUE] and timeout [4095] */
            timeout = 4095;
            
            st = gtFaVoqFportWatchdogEn(dev, fport, enable, timeout);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, enable, timeout);

            /*
                1.2. Call function with out of range timeout [0 / 4096]
                                        and enable [GT_TRUE].
                Expected: NON GT_OK.
            */
            enable = GT_TRUE;

            /* Call with timeout [0] */
            timeout = 0;
            
            st = gtFaVoqFportWatchdogEn(dev, fport, enable, timeout);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, enable, timeout);

            /* Call with timeout [4096] */
            timeout = 4096;
            
            st = gtFaVoqFportWatchdogEn(dev, fport, enable, timeout);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, enable, timeout);

            timeout = 1;

            /*
                1.3. Call function with enable [GT_FALSE]
                                        and timeout [0 / 4096] (is ignored).
                Expected: GT_OK.
            */
            enable = GT_FALSE;

            /* Call with timeout [0] */
            timeout = 0;
            
            st = gtFaVoqFportWatchdogEn(dev, fport, enable, timeout);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, enable, timeout);

            /* Call with timeout [4096] */
            timeout = 4096;
            
            st = gtFaVoqFportWatchdogEn(dev, fport, enable, timeout);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, enable, timeout);
        }

        enable  = GT_TRUE;
        timeout = 1;

        /* prepare fabric port iterator */
        st = prvUtfNextFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non active fabric ports */
        while (GT_OK == prvUtfNextFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active fabric port */
            st = gtFaVoqFportWatchdogEn(dev, fport, enable, timeout);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);
        }

        /* 1.3. For active device check that function returns NOT GT_OK    */
        /* for out of bound value for port number.                         */
        fport = maxFportsNum;

        st = gtFaVoqFportWatchdogEn(dev, fport, enable, timeout);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);
    }

    enable  = GT_TRUE;
    timeout = 1;

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = VOQ_VALID_FA_PORT_CNS;
    
    /* prepare device iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqFportWatchdogEn(dev, fport, enable, timeout);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqFportWatchdogEn(dev, fport, enable, timeout);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqMcWatchdogEn
(
    IN GT_U8   devNum,
    IN GT_BOOL enable,
    IN GT_U32  timeout
)
*/
UTF_TEST_CASE_MAC(gtFaVoqMcWatchdogEn)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function wih enable [GT_TRUE / GT_FALSE]
                           and timeout [1 / 100 / 4095].
    Expected: GT_OK.
    1.2. Call function with out of range timeout [0 / 4096]
                            and enable [GT_TRUE].
    Expected: NON GT_OK.
    1.3. Call function with enable [GT_FALSE]
                            and timeout [0 / 4096] (is ignored).
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_BOOL enable  = GT_FALSE;
    GT_U32  timeout = 0;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function wih enable [GT_TRUE / GT_FALSE]
                                   and timeout [1 / 100 / 4095].
            Expected: GT_OK.
        */
    
        /* Call with enable [GT_FALSE] and timeout [1] */
        enable  = GT_FALSE;
        timeout = 1;
            
        /* Call with enable [GT_TRUE] and timeout [1] */
        enable  = GT_TRUE;
        timeout = 1;
            
        st = gtFaVoqMcWatchdogEn(dev, enable, timeout);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, enable, timeout);
    
        /* Call with enable [GT_TRUE] and timeout [100] */
        timeout = 100;
            
        st = gtFaVoqMcWatchdogEn(dev, enable, timeout);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, enable, timeout);

        /* Call with enable [GT_TRUE] and timeout [4095] */
        timeout = 4095;
            
        st = gtFaVoqMcWatchdogEn(dev, enable, timeout);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, enable, timeout);

        /*
            1.2. Call function with out of range timeout [0 / 4096]
                                        and enable [GT_TRUE].
            Expected: NON GT_OK.
        */
        enable = GT_TRUE;

        /* Call with timeout [0] */
        timeout = 0;
            
        st = gtFaVoqMcWatchdogEn(dev, enable, timeout);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, enable, timeout);
        
        /* Call with timeout [4096] */
        timeout = 4096;
            
        st = gtFaVoqMcWatchdogEn(dev, enable, timeout);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, enable, timeout);

        timeout = 1;

        /*
            1.3. Call function with enable [GT_FALSE]
                                        and timeout [0 / 4096] (is ignored).
            Expected: GT_OK.
        */
        enable = GT_FALSE;

        /* Call with timeout [0] */
        timeout = 0;
            
        st = gtFaVoqMcWatchdogEn(dev, enable, timeout);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, enable, timeout);

        /* Call with timeout [4096] */
        timeout = 4096;
            
        st = gtFaVoqMcWatchdogEn(dev, enable, timeout);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, enable, timeout);
    }

    enable  = GT_FALSE;
    timeout = 1;

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqMcWatchdogEn(dev, enable, timeout);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqMcWatchdogEn(dev, enable, timeout);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqStatCounterView
(
    IN GT_U8          devNum,
    IN GT_U8          setViewNum,
    IN GT_BOOL        isDpInView,
    IN GT_U8          dpInView,
    IN GT_BOOL        isTcInView,
    IN GT_U8          tcInView,
    IN GT_BOOL        isTrgFpInView,
    IN GT_U8          trgFpInView,
    IN GT_BOOL        isTrgDevNumInView,
    IN GT_U8          trgDevNumInView
)
*/
UTF_TEST_CASE_MAC(gtFaVoqStatCounterView)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with setViewNum [0 / 2  / 3],
                            isDpInView [GT_TRUE],
                            dpInView [0 / 2 / 3],
                            isTcInView [GT_TRUE],
                            tcInView [0 / 4 / 7],
                            isTrgFpInView [GT_TRUE],
                            trgFpInView [0 / 23 / getMaxPortNum(dev) - 1],
                            isTrgDevNumInView [GT_TRUE]
                            and trgDevNumInView [0 / 32 / coreFaDevs[devNum]->maxNumDev -1].
    Expected: GT_OK.
    1.2. Call function with out of range setViewNum [4]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.3. Call function with out of range dpInView [4],
                            isDpInView [GT_TRUE]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.4. Call function with out of range dpInView [4] (not relevant),
                            isDpInView [GT_FALSE]
                            and other parameters the same as in 1.1.
    Expected: GT_OK.  
    1.5. Call function with out of range tcInView [8],
                            isTcInView [GT_TRUE]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.6. Call function with out of range tcInView [8] (not relevant),
                            isTcInView [GT_FALSE]
                            and other parameters the same as in 1.1.
    Expected: GT_OK.  
    1.7. Call function with out of range trgFpInView [getMaxPortNum(dev)],
                            isTrgFpInView [GT_TRUE]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.8. Call function with out of range trgFpInView [getMaxPortNum(dev)] (not relevant),
                            isTrgFpInView [GT_FALSE]
                            and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.9. Call function with out of range trgDevNumInView [getMaxDevNum(dev)],
                            isTrgDevNumInView [GT_TRUE]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.10. Call function with out of range trgDevNumInView [getMaxDevNum(dev)] (not relevant),
                             isTrgDevNumInView [GT_FALSE]
                             and other parameters the same as in 1.1.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U8     setViewNum        = 0;
    GT_BOOL   isDpInView        = GT_FALSE;
    GT_U8     dpInView          = 0;
    GT_BOOL   isTcInView        = GT_FALSE;
    GT_U8     tcInView          = 0;
    GT_BOOL   isTrgFpInView     = GT_FALSE;
    GT_U8     trgFpInView       = 0;
    GT_BOOL   isTrgDevNumInView = GT_FALSE;
    GT_U8     trgDevNumInView   = 0;
    GT_U8     maxFportsNum      = 0;
    GT_FA_MAX_NUM_DEV maxDevNum = FA_64_DEV_LBH;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* Getting max number of fabric ports */
        st = prvUtfNumFaPortsGet(dev, &maxFportsNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfNumFaPortsGet: %d", dev);

        /* Getting max number of packet processors */
        st = prvUtfFaMaxNumPpGet(dev, &maxDevNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfFaMaxNumPpGet: %d", dev);

        /* 
            1.1. Call function with setViewNum [0 / 2  / 3],
                                    isDpInView [GT_TRUE],
                                    dpInView [0 / 2 / 3],
                                    isTcInView [GT_TRUE],
                                    tcInView [0 / 4 / 7],
                                    isTrgFpInView [GT_TRUE],
                                    trgFpInView [0 / 23 / getMaxPortNum(dev) - 1],
                                    isTrgDevNumInView [GT_TRUE]
                                    and trgDevNumInView [0 / 32 / coreFaDevs[devNum]->maxNumDev -1].
            Expected: GT_OK.
        */
        isDpInView        = GT_TRUE;
        isTcInView        = GT_TRUE;
        isTrgFpInView     = GT_TRUE;
        isTrgDevNumInView = GT_TRUE;
    
        /* Call with setViewNum [0], dpInView [0], tcInView[0] */
        /* trgFpInView[0], trgDevNumInView[0]                  */
        setViewNum      = 0;
        dpInView        = 0;
        tcInView        = 0;
        trgFpInView     = 0;
        trgDevNumInView = 0;
            
        st = gtFaVoqStatCounterView(dev, setViewNum, isDpInView, dpInView, isTcInView, tcInView,
                                    isTrgFpInView, trgFpInView, isTrgDevNumInView, trgDevNumInView);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, setViewNum, dpInView, tcInView, trgFpInView, trgDevNumInView);

        /* Call with setViewNum [2], dpInView [2], tcInView[4] */
        /* trgFpInView[23], trgDevNumInView[32]                  */
        setViewNum      = 2;
        dpInView        = 2;
        tcInView        = 4;
        trgFpInView     = 23;
        trgDevNumInView = 32;
            
        st = gtFaVoqStatCounterView(dev, setViewNum, isDpInView, dpInView, isTcInView, tcInView,
                                    isTrgFpInView, trgFpInView, isTrgDevNumInView, trgDevNumInView);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, setViewNum, dpInView, tcInView, trgFpInView, trgDevNumInView);

        /* Call with setViewNum [3], dpInView [3], tcInView[7]                         */
        /* trgFpInView[getMaxPortNum(dev) - 1], trgDevNumInView[getMaxDevNum(dev) - 1] */
        setViewNum      = 3;
        dpInView        = 3;
        tcInView        = 7;
        trgFpInView     = maxFportsNum - 1;
        trgDevNumInView = maxDevNum - 1;
            
        st = gtFaVoqStatCounterView(dev, setViewNum, isDpInView, dpInView, isTcInView, tcInView,
                                    isTrgFpInView, trgFpInView, isTrgDevNumInView, trgDevNumInView);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, setViewNum, dpInView, tcInView, trgFpInView, trgDevNumInView);
    
        /*
            1.2. Call function with out of range setViewNum [4]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        setViewNum = 4;

        st = gtFaVoqStatCounterView(dev, setViewNum, isDpInView, dpInView, isTcInView, tcInView,
                                    isTrgFpInView, trgFpInView, isTrgDevNumInView, trgDevNumInView);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, setViewNum);

        setViewNum = 0;

        /*
            1.3. Call function with out of range dpInView [4],
                                    isDpInView [GT_TRUE]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        isDpInView = GT_TRUE;
        dpInView   = 4;
        
        st = gtFaVoqStatCounterView(dev, setViewNum, isDpInView, dpInView, isTcInView, tcInView,
                                    isTrgFpInView, trgFpInView, isTrgDevNumInView, trgDevNumInView);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, isDpInView = %d, dpInView = %d",
                                         dev, isDpInView, dpInView);

        dpInView = 0;

        /*
            1.4. Call function with out of range dpInView [4] (not relevant),
                                    isDpInView [GT_FALSE]
                                    and other parameters the same as in 1.1.
            Expected: GT_OK.
        */

        isDpInView = GT_FALSE;
        dpInView   = 4;
        
        st = gtFaVoqStatCounterView(dev, setViewNum, isDpInView, dpInView, isTcInView, tcInView,
                                    isTrgFpInView, trgFpInView, isTrgDevNumInView, trgDevNumInView);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, isDpInView = %d, dpInView = %d",
                                     dev, isDpInView, dpInView);

        dpInView   = 0;
        isDpInView = GT_TRUE;

        /*
            1.5. Call function with out of range tcInView [8],
                                    isTcInView [GT_TRUE]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        isTcInView = GT_TRUE;
        tcInView   = 8;
        
        st = gtFaVoqStatCounterView(dev, setViewNum, isDpInView, dpInView, isTcInView, tcInView,
                                    isTrgFpInView, trgFpInView, isTrgDevNumInView, trgDevNumInView);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, isTcInView = %d, tcInView = %d",
                                         dev, isTcInView, tcInView);

        tcInView = 0;

        /*
            1.6. Call function with out of range tcInView [8] (not relevant),
                                    isTcInView [GT_FALSE]
                                    and other parameters the same as in 1.1.
            Expected: GT_OK.
        */

        isTcInView = GT_FALSE;
        tcInView   = 4;
        
        st = gtFaVoqStatCounterView(dev, setViewNum, isDpInView, dpInView, isTcInView, tcInView,
                                    isTrgFpInView, trgFpInView, isTrgDevNumInView, trgDevNumInView);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, isTcInView = %d, tcInView = %d",
                                     dev, isTcInView, tcInView);

        tcInView   = 0;
        isTcInView = GT_TRUE;

        /*
            1.7. Call function with out of range trgFpInView [getMaxPortNum(dev)],
                                    isTrgFpInView [GT_TRUE]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        isTrgFpInView = GT_TRUE;
        trgFpInView   = maxFportsNum;
        
        st = gtFaVoqStatCounterView(dev, setViewNum, isDpInView, dpInView, isTcInView, tcInView,
                                    isTrgFpInView, trgFpInView, isTrgDevNumInView, trgDevNumInView);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, isTrgFpInView = %d, trgFpInView = %d",
                                         dev, isTcInView, trgFpInView);

        trgFpInView = 0;

        /*
            1.8. Call function with out of range trgFpInView [getMaxPortNum(dev)] (not relevant),
                                    isTrgFpInView [GT_FALSE]
                                    and other parameters the same as in 1.1.
            Expected: GT_OK.
        */

        isTrgFpInView = GT_FALSE;
        trgFpInView   = maxFportsNum;
        
        st = gtFaVoqStatCounterView(dev, setViewNum, isDpInView, dpInView, isTcInView, tcInView,
                                    isTrgFpInView, trgFpInView, isTrgDevNumInView, trgDevNumInView);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, isTrgFpInView = %d, tcInView = %d",
                                     dev, isTcInView, trgFpInView);

        trgFpInView   = 0;
        isTrgFpInView = GT_TRUE;

        /*
            1.9. Call function with out of range trgDevNumInView [getMaxDevNum(dev)],
                                    isTrgDevNumInView [GT_TRUE]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        isTrgDevNumInView = GT_TRUE;
        trgDevNumInView   = maxDevNum;
        
        st = gtFaVoqStatCounterView(dev, setViewNum, isDpInView, dpInView, isTcInView, tcInView,
                                    isTrgFpInView, trgFpInView, isTrgDevNumInView, trgDevNumInView);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, isTrgDevNumInView = %d, trgDevNumInView = %d",
                                         dev, isTrgDevNumInView, trgDevNumInView);

        trgFpInView = 0;

        /*
            1.10. Call function with out of range trgDevNumInView [getMaxDevNum(dev)] (not relevant),
                                     isTrgDevNumInView [GT_FALSE]
                                     and other parameters the same as in 1.1.
            Expected: GT_OK.
        */

        isTrgDevNumInView = GT_FALSE;
        trgDevNumInView   = maxDevNum;
        
        st = gtFaVoqStatCounterView(dev, setViewNum, isDpInView, dpInView, isTcInView, tcInView,
                                    isTrgFpInView, trgFpInView, isTrgDevNumInView, trgDevNumInView);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, isTrgDevNumInView = %d, trgDevNumInView = %d",
                                     dev, isTrgDevNumInView, trgDevNumInView);
    }

    isDpInView        = GT_TRUE;
    isTcInView        = GT_TRUE;
    isTrgFpInView     = GT_TRUE;
    isTrgDevNumInView = GT_TRUE;

    setViewNum      = 0;
    dpInView        = 0;
    tcInView        = 0;
    trgFpInView     = 0;
    trgDevNumInView = 0;

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqStatCounterView(dev, setViewNum, isDpInView, dpInView, isTcInView, tcInView,
                                    isTrgFpInView, trgFpInView, isTrgDevNumInView, trgDevNumInView);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqStatCounterView(dev, setViewNum, isDpInView, dpInView, isTcInView, tcInView,
                                    isTrgFpInView, trgFpInView, isTrgDevNumInView, trgDevNumInView);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqGetStatViewCounters
(
    IN   GT_U8    devNum,
    IN   GT_U8    setViewNum,
    OUT  GT_U32  *goodVoqRxPkts,
    OUT  GT_U32  *pktsDroppedOnVoqEnqueue,
    OUT  GT_U32  *pktsDroppedOnVoqDequeue
)
*/
UTF_TEST_CASE_MAC(gtFaVoqGetStatViewCounters)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with setViewNum [0 / 2  / 3],
                            non-NULL goodVoqRxPkts,
                            non-NULL pktsDroppedOnVoqEnqueue
                            and non-NULL pktsDroppedOnVoqDequeue.
    Expected: GT_OK.
    1.2. Call function with out of range setViewNum [4]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.3. Call function with goodVoqRxPkts [NULL]
                            and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.4. Call function with pktsDroppedOnVoqEnqueue [NULL] ]
                            and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.5. Call function with pktsDroppedOnVoqDequeue [NULL] ]
                            and other parameters the same as in 1.1.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U8     setViewNum          = 0;
    GT_U32    goodVoqRxPkts       = 0;
    GT_U32    pktsDrpOnVoqEnqueue = 0;
    GT_U32    pktsDrpOnVoqDequeue = 0;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with setViewNum [0 / 2  / 3],
                                    non-NULL goodVoqRxPkts,
                                    non-NULL pktsDroppedOnVoqEnqueue
                                    and non-NULL pktsDroppedOnVoqDequeue.
            Expected: GT_OK.
        */
    
        /* Call with setViewNum [0] */
        setViewNum = 0;
            
        st = gtFaVoqGetStatViewCounters(dev, setViewNum, &goodVoqRxPkts, &pktsDrpOnVoqEnqueue, &pktsDrpOnVoqDequeue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, setViewNum);
    
        /* Call with setViewNum [2] */
        setViewNum = 2;
            
        st = gtFaVoqGetStatViewCounters(dev, setViewNum, &goodVoqRxPkts, &pktsDrpOnVoqEnqueue, &pktsDrpOnVoqDequeue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, setViewNum);

        /* Call with setViewNum [3] */
        setViewNum = 3;
            
        st = gtFaVoqGetStatViewCounters(dev, setViewNum, &goodVoqRxPkts, &pktsDrpOnVoqEnqueue, &pktsDrpOnVoqDequeue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, setViewNum);

        /*
            1.2. Call function with out of range setViewNum [4]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        setViewNum = 4;
            
        st = gtFaVoqGetStatViewCounters(dev, setViewNum, &goodVoqRxPkts, &pktsDrpOnVoqEnqueue, &pktsDrpOnVoqDequeue);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, setViewNum);

        setViewNum = 0;

        /*
            1.3. Call function with goodVoqRxPkts [NULL]
                                    and other parameters the same as in 1.1.
            Expected: GT_OK.
        */
        st = gtFaVoqGetStatViewCounters(dev, setViewNum, NULL, &pktsDrpOnVoqEnqueue, &pktsDrpOnVoqDequeue);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, goodVoqRxPkts = NULL", dev);

        /*
            1.4. Call function with pktsDroppedOnVoqEnqueue [NULL] ]
                                    and other parameters the same as in 1.1.
            Expected: GT_OK.
        */
        st = gtFaVoqGetStatViewCounters(dev, setViewNum, &goodVoqRxPkts, NULL, &pktsDrpOnVoqDequeue);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, pktsDroppedOnVoqEnqueue = NULL", dev);

        /*
            1.5. Call function with pktsDroppedOnVoqDequeue [NULL] ]
                                    and other parameters the same as in 1.1.
            Expected: GT_OK.
        */
        st = gtFaVoqGetStatViewCounters(dev, setViewNum, &goodVoqRxPkts, &pktsDrpOnVoqEnqueue, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, pktsDroppedOnVoqDequeue = NULL", dev);
    }

    setViewNum = 0;

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqGetStatViewCounters(dev, setViewNum, &goodVoqRxPkts, &pktsDrpOnVoqEnqueue, &pktsDrpOnVoqDequeue);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqGetStatViewCounters(dev, setViewNum, &goodVoqRxPkts, &pktsDrpOnVoqEnqueue, &pktsDrpOnVoqDequeue);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqGetGlobalCounters
(
    IN   GT_U8                  devNum,
    OUT  GT_VOQ_GLOBAL_COUNTERS *voqGlobalCounters
)
*/
UTF_TEST_CASE_MAC(gtFaVoqGetGlobalCounters)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with non-NULL voqGlobalCounters.
    Expected: GT_OK.
    1.2. Call function with voqGlobalCounters [NULL].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_VOQ_GLOBAL_COUNTERS voqGlobalCounters;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with non-NULL voqGlobalCounters.
            Expected: GT_OK.
        */
        st = gtFaVoqGetGlobalCounters(dev, &voqGlobalCounters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    
        /*
            1.2. Call function with voqGlobalCounters [NULL].
            Expected: NOT GT_OK.
        */
        st = gtFaVoqGetGlobalCounters(dev, NULL);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, voqGlobalCounters = NULL", dev);
    }

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqGetGlobalCounters(dev, &voqGlobalCounters);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqGetGlobalCounters(dev, &voqGlobalCounters);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqGetDescriptorsInUseCounters
(
    IN   GT_U8                                devNum,
    OUT  GT_VOQ_DESCRIPTORS_IN_USE_COUNTERS  *voqDescInUseCounters
)
*/
UTF_TEST_CASE_MAC(gtFaVoqGetDescriptorsInUseCounters)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with non-NULL voqDescInUseCounters.
    Expected: GT_OK. 
    1.2. Call function with voqDescInUseCounters [NULL].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_VOQ_DESCRIPTORS_IN_USE_COUNTERS voqDescInUseCounters;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with non-NULL voqDescInUseCounters.
            Expected: GT_OK. 
        */
        st = gtFaVoqGetDescriptorsInUseCounters(dev, &voqDescInUseCounters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    
        /*
            1.2. Call function with voqDescInUseCounters [NULL].
            Expected: NOT GT_OK.
        */
        st = gtFaVoqGetDescriptorsInUseCounters(dev, NULL);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, voqDescInUseCounters = NULL", dev);
    }

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqGetDescriptorsInUseCounters(dev, &voqDescInUseCounters);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqGetDescriptorsInUseCounters(dev, &voqDescInUseCounters);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqControlTrafficQosAssign
(
    IN  GT_U8                           devNum,    
    IN  GT_BOOL                         ctrlQosAssignEnable,    
    IN  GT_VOQ_CTRL_TRAFFIC_QOS_STC     *ctrlQosParamsPtr
)
*/
UTF_TEST_CASE_MAC(gtFaVoqControlTrafficQosAssign)
{
/*
    ITERATE_FA_DEVICES (98FX915)
    1.1. Call function with ctrlQosAssignEnable [GT_TRUE]
                            and non-NULL ctrlQosParamsPtr {netPortToCpuTc [0 / 2 / 3],
                                                           netPortToCpuDp [0 / 2 / 3],
                                                           cpuToNetPortTc [0 / 2 / 3],
                                                           cpuToNetPortDp [0 / 2 / 3],
                                                           cpuToCpuTc [0 / 2 / 3],
                                                           cpuToCpuDp [0 / 2 / 3]}.
    Expected: GT_OK.
    1.2. Call gtFaVoqGetControlTrafficQosAssign with non-NULL ctrlQosAssignEnablePtr
                                                     and non-NULL ctrlQosParamsPtr.
    Expected: GT_OK and the same ctrlQosAssignEnable and ctrlQosParamsPtr.
    1.3. Call function with out of range  ctrlQosParamsPtr->netPortToCpuTc [4]
                            and other parameters the same as in 1.1.
    Expected: NOT GT_OK. 
    1.4. Call function with out of range ctrlQosParamsPtr->netPortToCpuDp [4]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.5. Call function with out of range ctrlQosParamsPtr->cpuToNetPortTc [4]
                            and other parameters the same as in 1.1.
    Expected: NOT GT_OK. 
    1.6. Call function with out of range ctrlQosParamsPtr->cpuToNetPortDp [4]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.7. Call function with out of range  ctrlQosParamsPtr->cpuToCpuTc [4]
                            and other parameters the same as in 1.1.
    Expected: NOT GT_OK. 
    1.8. Call function with out of range ctrlQosParamsPtr->cpuToCpuDp [4]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.9. Call function with out of range ctrlQosParamsPtr->cpuToCpuDp [4],
                            ctrlQosAssignEnable [GT_FALSE]
                            and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.10. Call function with ctrlQosParamsPtr [NULL]
                            and ctrlQosAssignEnable [GT_TRUE].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_BOOL                     ctrlQosAssignEnable    = GT_FALSE;
    GT_VOQ_CTRL_TRAFFIC_QOS_STC ctrlQosParams;
    GT_BOOL                     ctrlQosAssignEnableGet = GT_FALSE;
    GT_VOQ_CTRL_TRAFFIC_QOS_STC ctrlQosParamsGet;
    GT_BOOL                     isEqual                = GT_FALSE;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with ctrlQosAssignEnable [GT_TRUE]
                                    and non-NULL ctrlQosParamsPtr {netPortToCpuTc [0 / 2 / 3],
                                                                   netPortToCpuDp [0 / 2 / 3],
                                                                   cpuToNetPortTc [0 / 2 / 3],
                                                                   cpuToNetPortDp [0 / 2 / 3],
                                                                   cpuToCpuTc [0 / 2 / 3],
                                                                   cpuToCpuDp [0 / 2 / 3]}.
            Expected: GT_OK.
        */
        ctrlQosAssignEnable = GT_TRUE;
    
        /* Call with ctrlQosParamsPtr->... = 0 */
        ctrlQosParams.netPortToCpuTc = 0;
        ctrlQosParams.netPortToCpuDp = 0;
        ctrlQosParams.cpuToNetPortTc = 0;
        ctrlQosParams.cpuToNetPortDp = 0;
        ctrlQosParams.cpuToCpuTc     = 0;
        ctrlQosParams.cpuToCpuDp     = 0;
            
        st = gtFaVoqControlTrafficQosAssign(dev, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ctrlQosAssignEnable);

        /* Call with ctrlQosParamsPtr->... = 2 */
        ctrlQosParams.netPortToCpuTc = 2;
        ctrlQosParams.netPortToCpuDp = 2;
        ctrlQosParams.cpuToNetPortTc = 2;
        ctrlQosParams.cpuToNetPortDp = 2;
        ctrlQosParams.cpuToCpuTc     = 2;
        ctrlQosParams.cpuToCpuDp     = 2;
            
        st = gtFaVoqControlTrafficQosAssign(dev, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ctrlQosAssignEnable);

        /* Call with ctrlQosParamsPtr->... = 3 */
        ctrlQosParams.netPortToCpuTc = 3;
        ctrlQosParams.netPortToCpuDp = 3;
        ctrlQosParams.cpuToNetPortTc = 3;
        ctrlQosParams.cpuToNetPortDp = 3;
        ctrlQosParams.cpuToCpuTc     = 3;
        ctrlQosParams.cpuToCpuDp     = 3;
            
        st = gtFaVoqControlTrafficQosAssign(dev, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ctrlQosAssignEnable);

        /*
            1.2. Call gtFaVoqGetControlTrafficQosAssign with non-NULL ctrlQosAssignEnablePtr
                                                             and non-NULL ctrlQosParamsPtr.
            Expected: GT_OK and the same ctrlQosAssignEnable and ctrlQosParamsPtr.
        */
        st = gtFaVoqGetControlTrafficQosAssign(dev, &ctrlQosAssignEnableGet, &ctrlQosParamsGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtFaVoqGetControlTrafficQosAssign: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ctrlQosAssignEnable, ctrlQosAssignEnableGet,
                 "get another ctrlQosAssignEnable than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &ctrlQosParams, (GT_VOID*) &ctrlQosParamsGet, sizeof (ctrlQosParamsGet)))
                                      ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                     "get another ctrlQosParamsPtr than was set: %d", dev);

        /*
            1.3. Call function with out of range  ctrlQosParamsPtr->netPortToCpuTc [4]
                                    and other parameters the same as in 1.1.
            Expected: NOT GT_OK. 
        */
        ctrlQosParams.netPortToCpuTc = 4;

        st = gtFaVoqControlTrafficQosAssign(dev, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ctrlQosParamsPtr->netPortToCpuTc = %d",
                                         dev, ctrlQosParams.netPortToCpuTc);

        ctrlQosParams.netPortToCpuTc = 0;

        /*
            1.4. Call function with out of range ctrlQosParamsPtr->netPortToCpuDp [4]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        ctrlQosParams.netPortToCpuDp = 4;

        st = gtFaVoqControlTrafficQosAssign(dev, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ctrlQosParamsPtr->netPortToCpuDp = %d",
                                         dev, ctrlQosParams.netPortToCpuDp);

        ctrlQosParams.netPortToCpuDp = 0;

        /*
            1.5. Call function with out of range ctrlQosParamsPtr->cpuToNetPortTc [4]
                                    and other parameters the same as in 1.1.
            Expected: NOT GT_OK. 
        */
        ctrlQosParams.cpuToNetPortTc = 4;

        st = gtFaVoqControlTrafficQosAssign(dev, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ctrlQosParamsPtr->cpuToNetPortTc = %d",
                                     dev, ctrlQosParams.cpuToNetPortTc);

        ctrlQosParams.cpuToNetPortTc = 0;

        /*
            1.6. Call function with out of range ctrlQosParamsPtr->cpuToNetPortDp [4]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        ctrlQosParams.cpuToNetPortDp = 4;

        st = gtFaVoqControlTrafficQosAssign(dev, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ctrlQosParamsPtr->cpuToNetPortDp = %d",
                                         dev, ctrlQosParams.cpuToNetPortDp);

        ctrlQosParams.cpuToNetPortDp = 0;

        /*
            1.7. Call function with out of range  ctrlQosParamsPtr->cpuToCpuTc [4]
                                    and other parameters the same as in 1.1.
            Expected: NOT GT_OK. 
        */
        ctrlQosParams.cpuToCpuTc = 4;

        st = gtFaVoqControlTrafficQosAssign(dev, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ctrlQosParamsPtr->cpuToCpuTc = %d",
                                         dev, ctrlQosParams.cpuToCpuTc);

        ctrlQosParams.cpuToCpuTc = 0;

        /*
            1.8. Call function with out of range ctrlQosParamsPtr->cpuToCpuDp [4]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        ctrlQosParams.cpuToCpuDp = 4;

        st = gtFaVoqControlTrafficQosAssign(dev, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ctrlQosParamsPtr->cpuToCpuDp = %d",
                                         dev, ctrlQosParams.cpuToCpuDp);

        ctrlQosParams.cpuToCpuDp = 0;

        /*
            1.9. Call function with out of range ctrlQosParamsPtr->cpuToCpuDp [4],
                                    ctrlQosAssignEnable [GT_FALSE]
                                    and other parameters the same as in 1.1.
            Expected: GT_OK.
        */
        ctrlQosAssignEnable  = GT_FALSE;

        ctrlQosParams.cpuToCpuDp = 4;
        
        st = gtFaVoqControlTrafficQosAssign(dev, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ctrlQosAssignEnable = %d, ctrlQosParamsPtr->cpuToCpuDp = %d",
                                     dev, ctrlQosAssignEnable, ctrlQosParams.cpuToCpuDp);

        ctrlQosParams.cpuToCpuDp = 0;

        /*
            1.10. Call function with ctrlQosParamsPtr [NULL]
                                    and ctrlQosAssignEnable [GT_TRUE].
            Expected: NOT GT_OK.
        */
        st = gtFaVoqControlTrafficQosAssign(dev, ctrlQosAssignEnable, NULL);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, ctrlQosParamsPtr = NULL", dev);
    }

    ctrlQosAssignEnable = GT_TRUE;
    
    ctrlQosParams.netPortToCpuTc = 0;
    ctrlQosParams.netPortToCpuDp = 0;
    ctrlQosParams.cpuToNetPortTc = 0;
    ctrlQosParams.cpuToNetPortDp = 0;
    ctrlQosParams.cpuToCpuTc     = 0;
    ctrlQosParams.cpuToCpuDp     = 0;


    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqControlTrafficQosAssign(dev, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqControlTrafficQosAssign(dev, ctrlQosAssignEnable, &ctrlQosParams);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqGetControlTrafficQosAssign
(
    IN  GT_U8                           devNum,    
    OUT GT_BOOL                         *ctrlQosAssignEnablePtr,    
    OUT GT_VOQ_CTRL_TRAFFIC_QOS_STC     *ctrlQosParamsPtr
)
*/
UTF_TEST_CASE_MAC(gtFaVoqGetControlTrafficQosAssign)
{
/*
    ITERATE_FA_DEVICES (98FX915)
    1.1. Call function with non-NULL ctrlQosAssignEnablePtr
                            and non-NULL ctrlQosParamsPtr.
    Expected: GT_OK.
    1.2. Call function with ctrlQosAssignEnablePtr [NULL]
                            and non-NULL ctrlQosParamsPtr.
    Expected: NOT GT_OK.
    1.3. Call function with ctrlQosParamsPtr [NULL]
                            and non-NULL ctrlQosAssignEnablePtr.
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_BOOL                     ctrlQosAssignEnable = GT_FALSE;
    GT_VOQ_CTRL_TRAFFIC_QOS_STC ctrlQosParamsPtr;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with non-NULL ctrlQosAssignEnablePtr
                                    and non-NULL ctrlQosParamsPtr.
            Expected: GT_OK.
        */
        st = gtFaVoqGetControlTrafficQosAssign(dev, &ctrlQosAssignEnable, &ctrlQosParamsPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    
        /*
            1.2. Call function with ctrlQosAssignEnablePtr [NULL]
                                    and non-NULL ctrlQosParamsPtr.
            Expected: NOT GT_OK.
        */
        st = gtFaVoqGetControlTrafficQosAssign(dev, NULL, &ctrlQosParamsPtr);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, ctrlQosAssignEnablePtr = NULL", dev);

        /*
            1.3. Call function with ctrlQosParamsPtr [NULL]
                                    and non-NULL ctrlQosAssignEnablePtr.
            Expected: NOT GT_OK.
        */
        st = gtFaVoqGetControlTrafficQosAssign(dev, &ctrlQosAssignEnable, NULL);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, ctrlQosParamsPtr = NULL", dev);
    }

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqGetControlTrafficQosAssign(dev, &ctrlQosAssignEnable, &ctrlQosParamsPtr);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqGetControlTrafficQosAssign(dev, &ctrlQosAssignEnable, &ctrlQosParamsPtr);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqMcastTrafficUseVid
(
    IN  GT_U8       devNum,    
    IN  GT_BOOL     useVid
)
*/
UTF_TEST_CASE_MAC(gtFaVoqMcastTrafficUseVid)
{
/*
    ITERATE_FA_DEVICES (98FX915)
    1.1. Call function with useVid [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call gtFaVoqGetMcastTrafficUseVid with non-NULL useVidPtr.
    Expected: GT_OK and the same useVid.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_BOOL   useVid    = GT_FALSE;
    GT_BOOL   useVidGet = GT_FALSE;


    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with useVid [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        /* Call with useVid [GT_FALSE] */
        useVid = GT_FALSE;
        
        st = gtFaVoqMcastTrafficUseVid(dev, useVid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, useVid);

        /*
            1.2. Call gtFaVoqGetMcastTrafficUseVid with non-NULL useVidPtr.
            Expected: GT_OK and the same useVid.
        */
        st = gtFaVoqGetMcastTrafficUseVid(dev, &useVidGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtFaVoqGetMcastTrafficUseVid: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(useVid, useVidGet,
                 "get another useVid than was set: %d", dev);

        /* Call with useVid [GT_TRUE] */
        useVid = GT_TRUE;
        
        st = gtFaVoqMcastTrafficUseVid(dev, useVid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, useVid);

        /*
            1.2. Call gtFaVoqGetMcastTrafficUseVid with non-NULL useVidPtr.
            Expected: GT_OK and the same useVid.
        */
        st = gtFaVoqGetMcastTrafficUseVid(dev, &useVidGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtFaVoqGetMcastTrafficUseVid: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(useVid, useVidGet,
                 "get another useVid than was set: %d", dev);
    }

    useVid = GT_TRUE;
        
    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqMcastTrafficUseVid(dev, useVid);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqMcastTrafficUseVid(dev, useVid);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqGetMcastTrafficUseVid
(
    IN  GT_U8       devNum,    
    OUT GT_BOOL     *useVidPtr  
)
*/
UTF_TEST_CASE_MAC(gtFaVoqGetMcastTrafficUseVid)
{
/*
    ITERATE_FA_DEVICES (98FX915)
    1.1. Call function with non-NULL useVidPtr.
    Expected: GT_OK.
    1.2. Call function with useVidPtr [NULL].
    Expected: NOT GT_OK. 
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_BOOL   useVid = GT_FALSE;
    

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with non-NULL useVidPtr.
            Expected: GT_OK.
        */
        st = gtFaVoqGetMcastTrafficUseVid(dev, &useVid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with useVidPtr [NULL].
            Expected: NOT GT_OK. 
        */
        st = gtFaVoqGetMcastTrafficUseVid(dev, NULL);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, useVidPtr = NULL", dev);
    }

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqGetMcastTrafficUseVid(dev, &useVid);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqGetMcastTrafficUseVid(dev, &useVid);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaVoqStatCounterConfig
(
    IN GT_U8                        devNum,
    IN GT_U8                        setViewNum,
    IN GT_VOQ_STAT_COUNTER_SET_CFG  *counterSetConfigPtr
)
*/
UTF_TEST_CASE_MAC(gtFaVoqStatCounterConfig)
{
/*
    ITERATE_FA_DEVICES (98FX915)
    1.1. Call function with with setViewNum [0 / 2  / 3]
                                 and non-NULL counterSetConfigPtr {isDpInView [GT_TRUE],
                                                                   dpType [GT_VOQ_TC_DP_FROM_PP_DESC /
                                                                           GT_VOQ_TC_DP_FROM_FA_ASSIGN],
                                                                   dpInView [0 / 2 / 3],
                                                                   isTcInView [GT_TRUE],
                                                                   tcType [GT_VOQ_TC_DP_FROM_PP_DESC /
                                                                           GT_VOQ_TC_DP_FROM_FA_ASSIGN],
                                                                    tcInView [0 / 4 / 7],
                                                                    isTrgFpInView [GT_TRUE],
                                                                    trgFpInView [0 / 23 / getMaxPortNum(dev) - 1],
                                                                    isTrgDevNumInView [GT_TRUE]
                                                                    and trgDevNumInView [0 / 32 / coreFaDevs[devNum]->maxNumDev -1]}.
    Expected: GT_OK.
    1.2. Call function with out of range setViewNum [4]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.3. Call function with out of range counterSetConfigPtr->dpType [0x5AAAAAA5],
                            counterSetConfigPtr->isDpInView [GT_TRUE]
                            and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.4. Call function with out of range counterSetConfigPtr->dpType [0x5AAAAAA5],
                            counterSetConfigPtr->isDpInView [GT_FALSE]
                            and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.5. Call function with out of range counterSetConfigPtr->dpInView [4],
                            counterSetConfigPtr->isDpInView [GT_TRUE]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.6. Call function with out of range counterSetConfigPtr->dpInView [4],
                            counterSetConfigPtr-> isDpInView [GT_FALSE]
                            and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.7. Call function with out of range counterSetConfigPtr->tcType [0x5AAAAAA5],
                            counterSetConfigPtr->isDpInView [GT_TRUE]
                            and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.8. Call function with out of range counterSetConfigPtr->tcType [0x5AAAAAA5],
                            counterSetConfigPtr->isDpInView [GT_FALSE]
                            and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.9. Call function with out of range counterSetConfigPtr->tcInView [8],
                            counterSetConfigPtr->isTcInView [GT_TRUE]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.10. Call function with out of range counterSetConfigPtr->tcInView [8],
                             counterSetConfigPtr->isTcInView [GT_FALSE]
                             and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.11. Call function with out of range counterSetConfigPtr->trgFpInView [getMaxPortNum(dev)],
                             counterSetConfigPtr->isTrgFpInView [GT_TRUE]
                             and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.12. Call function with out of range counterSetConfigPtr->trgFpInView [getMaxPortNum(dev)],
                             counterSetConfigPtr->isTrgFpInView [GT_FALSE]
                             and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.13. Call function with out of range counterSetConfigPtr->trgDevNumInView [getMaxDevNum(dev)],
                             counterSetConfigPtr->isTrgDevNumInView [GT_TRUE]
                             and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.14. Call function with out of range counterSetConfigPtr->trgDevNumInView [getMaxDevNum(dev)],
                             counterSetConfigPtr->isTrgDevNumInView [GT_FALSE]
                             and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.15. Call function with counterSetConfigPtr [NULL]
                             and setViewNum [2].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U8                          setViewNum   = 0;
    IN GT_VOQ_STAT_COUNTER_SET_CFG counterSetConfig;
    GT_U8                          maxFportsNum = 0;
    GT_FA_MAX_NUM_DEV              maxDevNum    = FA_64_DEV_LBH;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* Getting max number of fabric ports */
        st = prvUtfNumFaPortsGet(dev, &maxFportsNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfNumFaPortsGet: %d", dev);

        /* Getting max number of packet processors */
        st = prvUtfFaMaxNumPpGet(dev, &maxDevNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfFaMaxNumPpGet: %d", dev);

        /* 
            1.1. Call function with with setViewNum [0 / 2  / 3]
                                         and non-NULL counterSetConfigPtr {isDpInView [GT_TRUE],
                                                                           dpType [GT_VOQ_TC_DP_FROM_PP_DESC /
                                                                                   GT_VOQ_TC_DP_FROM_FA_ASSIGN],
                                                                           dpInView [0 / 2 / 3],
                                                                           isTcInView [GT_TRUE],
                                                                           tcType [GT_VOQ_TC_DP_FROM_PP_DESC /
                                                                                   GT_VOQ_TC_DP_FROM_FA_ASSIGN],
                                                                            tcInView [0 / 4 / 7],
                                                                            isTrgFpInView [GT_TRUE],
                                                                            trgFpInView [0 / 23 / getMaxPortNum(dev) - 1],
                                                                            isTrgDevNumInView [GT_TRUE]
                                                                            and trgDevNumInView [0 / 32 / coreFaDevs[devNum]->maxNumDev -1]}.
            Expected: GT_OK.
        */
        counterSetConfig.isDpInView        = GT_TRUE;
        counterSetConfig.isTcInView        = GT_TRUE;
        counterSetConfig.isTrgFpInView     = GT_TRUE;
        counterSetConfig.isTrgDevNumInView = GT_TRUE;

        counterSetConfig.dpType = GT_VOQ_TC_DP_FROM_PP_DESC;
        counterSetConfig.tcType = GT_VOQ_TC_DP_FROM_FA_ASSIGN;

        /* Call with setViewNum [0], dpInView [0], tcInView[0] */
        /* trgFpInView[0], trgDevNumInView[0]                  */
        setViewNum = 0;

        counterSetConfig.dpInView        = 0;
        counterSetConfig.tcInView        = 0;
        counterSetConfig.trgFpInView     = 0;
        counterSetConfig.trgDevNumInView = 0;

        st = gtFaVoqStatCounterConfig(dev, setViewNum, &counterSetConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, setViewNum);

        /* Call with setViewNum [2], dpInView [2], tcInView[4] */
        /* trgFpInView[23], trgDevNumInView[32]                  */
        setViewNum = 2;

        counterSetConfig.dpInView        = 2;
        counterSetConfig.tcInView        = 4;
        counterSetConfig.trgFpInView     = 23;
        counterSetConfig.trgDevNumInView = 32;

        st = gtFaVoqStatCounterConfig(dev, setViewNum, &counterSetConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, setViewNum);

        /* Call with setViewNum [3], dpInView [3], tcInView[7]                         */
        /* trgFpInView[getMaxPortNum(dev) - 1], trgDevNumInView[getMaxDevNum(dev) - 1] */
        setViewNum = 3;

        counterSetConfig.dpType = GT_VOQ_TC_DP_FROM_FA_ASSIGN;
        counterSetConfig.tcType = GT_VOQ_TC_DP_FROM_PP_DESC;
        
        counterSetConfig.dpInView        = 3;
        counterSetConfig.tcInView        = 7;
        counterSetConfig.trgFpInView     = maxFportsNum - 1;
        counterSetConfig.trgDevNumInView = maxDevNum - 1;

        st = gtFaVoqStatCounterConfig(dev, setViewNum, &counterSetConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, setViewNum);

        /*
            1.2. Call function with out of range setViewNum [4]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        setViewNum = 4;

        st = gtFaVoqStatCounterConfig(dev, setViewNum, &counterSetConfig);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, setViewNum);

        setViewNum = 0;

        /*
            1.3. Call function with out of range counterSetConfigPtr->dpType [0x5AAAAAA5],
                                    counterSetConfigPtr->isDpInView [GT_TRUE]
                                    and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        counterSetConfig.isDpInView = GT_TRUE;
        counterSetConfig.dpType     = VOQ_INVALID_ENUM_CNS;

        st = gtFaVoqStatCounterConfig(dev, setViewNum, &counterSetConfig);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->isDpInView = %d, ->dpType = %d",
                                         dev, counterSetConfig.isDpInView, counterSetConfig.dpType);

        counterSetConfig.dpType = GT_VOQ_TC_DP_FROM_PP_DESC;

        /*
            1.4. Call function with out of range counterSetConfigPtr->dpType [0x5AAAAAA5],
                                    counterSetConfigPtr->isDpInView [GT_FALSE]
                                    and other parameters the same as in 1.1.
            Expected: GT_OK.
        */

        counterSetConfig.isDpInView = GT_FALSE;
        counterSetConfig.dpType     = VOQ_INVALID_ENUM_CNS;

        st = gtFaVoqStatCounterConfig(dev, setViewNum, &counterSetConfig);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->isDpInView = %d, ->dpType = %d",
                                     dev, counterSetConfig.isDpInView, counterSetConfig.dpType);

        counterSetConfig.dpType     = GT_VOQ_TC_DP_FROM_PP_DESC;
        counterSetConfig.isDpInView = GT_TRUE;

        /*
            1.5. Call function with out of range counterSetConfigPtr->dpInView [4],
                                    counterSetConfigPtr->isDpInView [GT_TRUE]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        counterSetConfig.isDpInView = GT_TRUE;
        counterSetConfig.dpInView   = 4;

        st = gtFaVoqStatCounterConfig(dev, setViewNum, &counterSetConfig);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->isDpInView = %d, ->dpInView = %d",
                                         dev, counterSetConfig.isDpInView, counterSetConfig.dpInView);

        counterSetConfig.dpInView = 0;

        /*
            1.6. Call function with out of range counterSetConfigPtr->dpInView [4],
                                    counterSetConfigPtr-> isDpInView [GT_FALSE]
                                    and other parameters the same as in 1.1.
            Expected: GT_OK.
        */

        counterSetConfig.isDpInView = GT_FALSE;
        counterSetConfig.dpInView   = 4;

        st = gtFaVoqStatCounterConfig(dev, setViewNum, &counterSetConfig);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->isDpInView = %d, ->dpInView = %d",
                                     dev, counterSetConfig.isDpInView, counterSetConfig.dpInView);

        counterSetConfig.dpInView   = 0;
        counterSetConfig.isDpInView = GT_TRUE;

        /*
            1.7. Call function with out of range counterSetConfigPtr->tcType [0x5AAAAAA5],
                                    counterSetConfigPtr->isDpInView [GT_TRUE]
                                    and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        counterSetConfig.isTcInView = GT_TRUE;
        counterSetConfig.tcType     = VOQ_INVALID_ENUM_CNS;

        st = gtFaVoqStatCounterConfig(dev, setViewNum, &counterSetConfig);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->isTcInView = %d, ->tcType = %d",
                                         dev, counterSetConfig.isTcInView, counterSetConfig.tcType);

        counterSetConfig.tcType = GT_VOQ_TC_DP_FROM_PP_DESC;

        /*
            1.8. Call function with out of range counterSetConfigPtr->tcType [0x5AAAAAA5],
                                    counterSetConfigPtr->isDpInView [GT_FALSE]
                                    and other parameters the same as in 1.1.
            Expected: GT_OK.
        */

        counterSetConfig.isTcInView = GT_FALSE;
        counterSetConfig.tcType     = VOQ_INVALID_ENUM_CNS;

        st = gtFaVoqStatCounterConfig(dev, setViewNum, &counterSetConfig);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->isTcInView = %d, ->tcType = %d",
                                     dev, counterSetConfig.isTcInView, counterSetConfig.tcType);

        counterSetConfig.tcType     = GT_VOQ_TC_DP_FROM_PP_DESC;
        counterSetConfig.isTcInView = GT_TRUE;

        /*
            1.9. Call function with out of range counterSetConfigPtr->tcInView [8],
                                    counterSetConfigPtr->isTcInView [GT_TRUE]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */

        counterSetConfig.isTcInView = GT_TRUE;
        counterSetConfig.tcInView   = 8;

        st = gtFaVoqStatCounterConfig(dev, setViewNum, &counterSetConfig);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->isTcInView = %d, ->tcInView = %d",
                                         dev, counterSetConfig.isTcInView, counterSetConfig.tcInView);

        counterSetConfig.tcInView = 0;

        /*
            1.10. Call function with out of range counterSetConfigPtr->tcInView [8],
                                     counterSetConfigPtr->isTcInView [GT_FALSE]
                                     and other parameters the same as in 1.1.
            Expected: GT_OK.
        */

        counterSetConfig.isTcInView = GT_FALSE;
        counterSetConfig.tcInView   = 4;

        st = gtFaVoqStatCounterConfig(dev, setViewNum, &counterSetConfig);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->isTcInView = %d, ->tcInView = %d",
                                     dev, counterSetConfig.isTcInView, counterSetConfig.tcInView);

        counterSetConfig.tcInView   = 0;
        counterSetConfig.isTcInView = GT_TRUE;

        /*
            1.11. Call function with out of range counterSetConfigPtr->trgFpInView [getMaxPortNum(dev)],
                                     counterSetConfigPtr->isTrgFpInView [GT_TRUE]
                                     and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        counterSetConfig.isTrgFpInView = GT_TRUE;
        counterSetConfig.trgFpInView   = maxFportsNum;

        st = gtFaVoqStatCounterConfig(dev, setViewNum, &counterSetConfig);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->isTrgFpInView = %d, ->trgFpInView = %d",
                                         dev, counterSetConfig.isTcInView, counterSetConfig.trgFpInView);

        counterSetConfig.trgFpInView = 0;

        /*
            1.12. Call function with out of range counterSetConfigPtr->trgFpInView [getMaxPortNum(dev)],
                                     counterSetConfigPtr->isTrgFpInView [GT_FALSE]
                                     and other parameters the same as in 1.1.
            Expected: GT_OK.
        */

        counterSetConfig.isTrgFpInView = GT_FALSE;
        counterSetConfig.trgFpInView   = maxFportsNum;

        st = gtFaVoqStatCounterConfig(dev, setViewNum, &counterSetConfig);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->isTrgFpInView = %d, ->tcInView = %d",
                                     dev, counterSetConfig.isTcInView, counterSetConfig.trgFpInView);

        counterSetConfig.trgFpInView   = 0;
        counterSetConfig.isTrgFpInView = GT_TRUE;

        /*
            1.13. Call function with out of range counterSetConfigPtr->trgDevNumInView [getMaxDevNum(dev)],
                                     counterSetConfigPtr->isTrgDevNumInView [GT_TRUE]
                                     and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        counterSetConfig.isTrgDevNumInView = GT_TRUE;
        counterSetConfig.trgDevNumInView   = maxDevNum;

        st = gtFaVoqStatCounterConfig(dev, setViewNum, &counterSetConfig);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->isTrgDevNumInView = %d, ->trgDevNumInView = %d",
                                         dev, counterSetConfig.isTrgDevNumInView, counterSetConfig.trgDevNumInView);

        counterSetConfig.trgFpInView = 0;

        /*
            1.14. Call function with out of range counterSetConfigPtr->trgDevNumInView [getMaxDevNum(dev)],
                                     counterSetConfigPtr->isTrgDevNumInView [GT_FALSE]
                                     and other parameters the same as in 1.1.
            Expected: GT_OK.
        */

        counterSetConfig.isTrgDevNumInView = GT_FALSE;
        counterSetConfig.trgDevNumInView   = maxDevNum;

        st = gtFaVoqStatCounterConfig(dev, setViewNum, &counterSetConfig);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->isTrgDevNumInView = %d, ->trgDevNumInView = %d",
                                     dev, counterSetConfig.isTrgDevNumInView, counterSetConfig.trgDevNumInView);

        counterSetConfig.isTrgDevNumInView = GT_TRUE;
        counterSetConfig.trgDevNumInView   = 0;

        /*
            1.15. Call function with counterSetConfigPtr [NULL]
                                     and setViewNum [2].
            Expected: NOT GT_OK.
        */
        setViewNum = 2;

        st = gtFaVoqStatCounterConfig(dev, setViewNum, NULL);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, counterSetConfigPtr = NULL", dev);
    }

    setViewNum = 0;

    counterSetConfig.isDpInView        = GT_TRUE;
    counterSetConfig.isTcInView        = GT_TRUE;
    counterSetConfig.isTrgFpInView     = GT_TRUE;
    counterSetConfig.isTrgDevNumInView = GT_TRUE;

    counterSetConfig.dpType = GT_VOQ_TC_DP_FROM_PP_DESC;
    counterSetConfig.tcType = GT_VOQ_TC_DP_FROM_FA_ASSIGN;

    counterSetConfig.dpInView        = 0;
    counterSetConfig.tcInView        = 0;
    counterSetConfig.trgFpInView     = 0;
    counterSetConfig.trgDevNumInView = 0;

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaVoqStatCounterConfig(dev, setViewNum, &counterSetConfig);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaVoqStatCounterConfig(dev, setViewNum, &counterSetConfig);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
* Configuration of gtVoqApi suit
*/
UTF_SUIT_BEGIN_TESTS_MAC(gtVoqApi)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqGetMaxHwFports)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqAutoFlushOnFportDisable)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqFportTxEnable)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqMcTxEn)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqFlushAllTrigger)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqFportFlushTrigger)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqMcFlushTrigger)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqGetMaxHwVoqDescriptors)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqSetGlobalMaxDescriptorLimit)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqBindFport2ConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqBindMc2ConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqFportConfigSetMaxDescriptorLimit)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqFportDpConfigSetMaxDescriptorLimit)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqTc2FprioMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqAuMcTcSet)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqDevToFportMap)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqSetDevTbl)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqClearDevTbl)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqFportWatchdogEn)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqMcWatchdogEn)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqStatCounterView)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqGetStatViewCounters)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqGetGlobalCounters)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqGetDescriptorsInUseCounters)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqControlTrafficQosAssign)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqGetControlTrafficQosAssign)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqMcastTrafficUseVid)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqGetMcastTrafficUseVid)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaVoqStatCounterConfig)
UTF_SUIT_END_TESTS_MAC(gtVoqApi)

