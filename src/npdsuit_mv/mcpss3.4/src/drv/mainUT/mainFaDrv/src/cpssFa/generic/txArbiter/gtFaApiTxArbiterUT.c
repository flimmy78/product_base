/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
*  gtFaApiTxArbiter
*
* DESCRIPTION:
*       Unit tests for gtFaApiTxArbiter, that provides 
*          restera API implementation for Fabric Adapter TX Arbiter
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
/* includes */
#include <cpssFa/generic/txArbiter/gtFaTxArbiter.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */
#define TX_ARBITER_INVALID_ENUM_CNS 0x5AAAAAA5
#define TX_ARBITER_VALID_FA_PORT_CNS    0

/*----------------------------------------------------------------------------*/
/*
faTxArbEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(faTxArbEnable)
{
/*
ITERATE_FA_DEVICES
1.1. Call function with enable [GT_FALSE/ GT_TRUE]. Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_BOOL     enable;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with enable [GT_FALSE/ GT_TRUE]. Expected: GT_OK. */
        enable = GT_TRUE;

        st = faTxArbEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_FALSE;

        st = faTxArbEnable(dev, enable);
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
        st = faTxArbEnable(dev, enable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = faTxArbEnable(dev, enable);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
faArbConfigWrrProfileSet
(
    IN GT_U8                    devNum,
    IN GT_FA_TX_ARB_SET         setNum,
    IN GT_U8                    fPrio,
    IN GT_FA_TX_ARB_WRR_WEIGHT  weight
)
*/
UTF_TEST_CASE_MAC(faArbConfigWrrProfileSet)
{
/*
ITERATE_FA_DEVICES
1.1. Call function with setNum [GT_FA_TX_ARB_SET0/ GT_FA_TX_ARB_SET1/
GT_FA_TX_ARB_SET2/ GT_FA_TX_ARB_SET3], fPrio [0/ 1/ 2/ 3] and 
weight [GT_FA_TX_ARB_1_CELL/ GT_FA_TX_ARB_3_CELL/ GT_FA_TX_ARB_5_CELL/ 
GT_FA_TX_ARB_STRICT]. Expected: GT_OK.
1.2. Call function with out of range setNum [0x5AAAAAA5] and fPrio [0],
weight [GT_FA_TX_ARB_1_CELL]. Expected: NON GT_OK.
1.3. Call function with out of range fPrio [NUM_FA_TC_QUEUES = 4] and 
setNum [GT_FA_TX_ARB_SET0], weight [GT_FA_TX_ARB_1_CELL]. Expected: NON GT_OK.
1.4. Call function with out of range weight [0x5AAAAAA5] and fPrio [0], 
setNum [GT_FA_TX_ARB_SET0]. Expected: NON GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_FA_TX_ARB_SET        setNum;
    GT_U8                   fPrio;
    GT_FA_TX_ARB_WRR_WEIGHT weight;


    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with setNum [GT_FA_TX_ARB_SET0/ GT_FA_TX_ARB_SET1/
        GT_FA_TX_ARB_SET2/ GT_FA_TX_ARB_SET3], fPrio [0/ 1/ 2/ 3] and 
        weight [GT_FA_TX_ARB_1_CELL/ GT_FA_TX_ARB_3_CELL/ GT_FA_TX_ARB_5_CELL/ 
        GT_FA_TX_ARB_STRICT]. Expected: GT_OK. */
        setNum = GT_FA_TX_ARB_SET0;
        fPrio = 0;
        weight = GT_FA_TX_ARB_1_CELL;

        st = faArbConfigWrrProfileSet(dev, setNum, fPrio, weight);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, setNum, fPrio, weight);

        setNum = GT_FA_TX_ARB_SET1;
        fPrio = 1;
        weight = GT_FA_TX_ARB_3_CELL;

        st = faArbConfigWrrProfileSet(dev, setNum, fPrio, weight);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, setNum, fPrio, weight);

        setNum = GT_FA_TX_ARB_SET3;
        fPrio = 2;
        weight = GT_FA_TX_ARB_5_CELL;

        st = faArbConfigWrrProfileSet(dev, setNum, fPrio, weight);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, setNum, fPrio, weight);

        fPrio = 3;
        weight = GT_FA_TX_ARB_STRICT;

        /* 1.2. Call function with out of range setNum [0x5AAAAAA5] and fPrio [0],
        weight [GT_FA_TX_ARB_1_CELL]. Expected: NON GT_OK. */
        setNum = TX_ARBITER_INVALID_ENUM_CNS;

        fPrio = 0;
        weight = GT_FA_TX_ARB_1_CELL;

        st = faArbConfigWrrProfileSet(dev, setNum, fPrio, weight);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, setNum, fPrio, weight);

        setNum = GT_FA_TX_ARB_SET0;/* restore */

        /* 1.3. Call function with out of range fPrio [NUM_FA_TC_QUEUES = 4] and 
        setNum [GT_FA_TX_ARB_SET0], weight [GT_FA_TX_ARB_1_CELL]. Expected: NON GT_OK. */
        fPrio = NUM_FA_TC_QUEUES;
        weight = GT_FA_TX_ARB_1_CELL;

        st = faArbConfigWrrProfileSet(dev, setNum, fPrio, weight);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, setNum, fPrio, weight);

        fPrio = 0;/* restore */

        /* 1.4. Call function with out of range weight [0x5AAAAAA5] and fPrio [0], 
        setNum [GT_FA_TX_ARB_SET0]. Expected: NON GT_OK. */
        weight = TX_ARBITER_INVALID_ENUM_CNS;
        setNum = GT_FA_TX_ARB_SET0;

        st = faArbConfigWrrProfileSet(dev, setNum, fPrio, weight);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, setNum, fPrio, weight);
    }
    setNum = GT_FA_TX_ARB_SET0;
    fPrio = 0;
    weight = GT_FA_TX_ARB_1_CELL;

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = faArbConfigWrrProfileSet(dev, setNum, fPrio, weight);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = faArbConfigWrrProfileSet(dev, setNum, fPrio, weight);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
faArbBindFport2WrrProfileSet
(
    IN GT_U8              devNum,
    IN GT_FA_TX_ARB_SET   setNum,
    IN GT_U8              fport
)
*/
UTF_TEST_CASE_MAC(faArbBindFport2WrrProfileSet)
{
/*
ITERATE_FA_DEVICES
1.1. Call function with fport [0], setNum [GT_FA_TX_ARB_SET0/ GT_FA_TX_ARB_SET3].
Expected: GT_OK.
1.2. Call function with out of range setNum [0x5AAAAAA5], fport [0]. Expected:
NON GT_OK.
1.3. Call function with out of range fport [numOfFports(dev)] and 
setNum [GT_FA_TX_ARB_SET0]. Expected: NON GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_FA_TX_ARB_SET        setNum;
    GT_U8                   fport;
    GT_U8                   maxFportsNum;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* Getting max number of fabric ports */
        st = prvUtfNumFaPortsGet(dev, &maxFportsNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfNumFaPortsGet: %d", dev);

        /* 1.1. Call function with fport [0], setNum [GT_FA_TX_ARB_SET0 / GT_FA_TX_ARB_SET3].
        Expected: GT_OK. */
        setNum = GT_FA_TX_ARB_SET0;
        fport = 0;

        st = faArbBindFport2WrrProfileSet(dev, setNum, fport);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, setNum, fport);

        setNum = GT_FA_TX_ARB_SET3;

        st = faArbBindFport2WrrProfileSet(dev, setNum, fport);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, setNum, fport);

        /* 1.2. Call function with out of range setNum [0x5AAAAAA5], fport [0]. Expected:
        NON GT_OK. */
        setNum = TX_ARBITER_INVALID_ENUM_CNS;

        st = faArbBindFport2WrrProfileSet(dev, setNum, fport);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, setNum, fport);

        setNum = GT_FA_TX_ARB_SET0;/* restore */

        /* 1.3. Call function with out of range fport [numOfFports(dev)] and 
        setNum [GT_FA_TX_ARB_SET0]. Expected: NON GT_OK. */
        fport = maxFportsNum;
        setNum = GT_FA_TX_ARB_SET0;

        st = faArbBindFport2WrrProfileSet(dev, setNum, fport);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, setNum, fport);
    }
    setNum = GT_FA_TX_ARB_SET0;
    fport = 0;

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = faArbBindFport2WrrProfileSet(dev, setNum, fport);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = faArbBindFport2WrrProfileSet(dev, setNum, fport);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
faArbBindMc2WrrProfileSet
(
    IN GT_U8              devNum,
    IN GT_FA_TX_ARB_SET   setNum
)
*/
UTF_TEST_CASE_MAC(faArbBindMc2WrrProfileSet)
{
/*
ITERATE_FA_DEVICES
1.1. Call function with setNum [GT_FA_TX_ARB_SET0/ GT_FA_TX_ARB_SET3]. Expected: GT_OK.
1.2. Call function with out of range setNum [0x5AAAAAA5]. Expected: NON GT_OK.
*/
    GT_STATUS          st = GT_OK;
    GT_U8              dev;
    GT_FA_TX_ARB_SET   setNum;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with setNum [GT_FA_TX_ARB_SET0/ GT_FA_TX_ARB_SET3].
        Expected: GT_OK. */
        setNum = GT_FA_TX_ARB_SET0;

        st = faArbBindMc2WrrProfileSet(dev, setNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, setNum);

        setNum = GT_FA_TX_ARB_SET3;

        st = faArbBindMc2WrrProfileSet(dev, setNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, setNum);

        /* 1.2. Call function with out of range setNum [0x5AAAAAA5].
        Expected: NON GT_OK. */
        setNum = TX_ARBITER_INVALID_ENUM_CNS;

        st = faArbBindMc2WrrProfileSet(dev, setNum);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, setNum);
    }
    setNum = GT_FA_TX_ARB_SET0;

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = faArbBindMc2WrrProfileSet(dev, setNum);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = faArbBindMc2WrrProfileSet(dev, setNum);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
faArbSetUnMcRatio
(
    IN GT_U8                     devNum,
    IN GT_FA_TX_ARB_UC_MC_RATIO ucMcRatio
)
*/
UTF_TEST_CASE_MAC(faArbSetUnMcRatio)
{
/*
ITERATE_FA_DEVICES
1.1. Call function with ucMcRatio [GT_FA_TX_ARB_1_1_RATIO/ 
GT_FA_TX_ARB_1_64_RATIO]. Expected: GT_OK.
1.2. Call function with out of range ucMcRatio [0x5AAAAAA5].
Expected: NON GT_OK.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_FA_TX_ARB_UC_MC_RATIO    ucMcRatio;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with ucMcRatio [GT_FA_TX_ARB_1_1_RATIO/ 
        GT_FA_TX_ARB_1_64_RATIO]. Expected: GT_OK. */
        ucMcRatio = GT_FA_TX_ARB_1_1_RATIO;

        st = faArbSetUnMcRatio(dev, ucMcRatio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ucMcRatio);

        ucMcRatio = GT_FA_TX_ARB_1_64_RATIO;

        st = faArbSetUnMcRatio(dev, ucMcRatio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ucMcRatio);

        /* 1.2. Call function with out of range ucMcRatio [0x5AAAAAA5].
        Expected: NON GT_OK. */
        ucMcRatio = TX_ARBITER_INVALID_ENUM_CNS;

        st = faArbSetUnMcRatio(dev, ucMcRatio);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ucMcRatio);
     }
    ucMcRatio = GT_FA_TX_ARB_1_1_RATIO;

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = faArbSetUnMcRatio(dev, ucMcRatio);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = faArbSetUnMcRatio(dev, ucMcRatio);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
faArbFportFlush
(
    IN GT_U8        devNum,
    IN GT_U8        fport
)
*/
UTF_TEST_CASE_MAC(faArbFportFlush)
{
/*
ITERATE_FA_PORTS
1.1. Call function. Expected: GT_OK.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U8           fport;

    GT_U8           maxFportsNum;

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
            /* 1.1. Call function. Expected: GT_OK. */
            st = faArbFportFlush(dev, fport);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);
        }

        /* prepare fabric port iterator */
        st = prvUtfNextFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non active fabric ports */
        while (GT_OK == prvUtfNextFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active fabric port */
            st = faArbFportFlush(dev, fport);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);
        }

        /* 1.3. For active device check that function returns NOT GT_OK    */
        /* for out of bound value for port number.                         */
        fport = maxFportsNum;

        st = faArbFportFlush(dev, fport);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = TX_ARBITER_VALID_FA_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = faArbFportFlush(dev, fport);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = faArbFportFlush(dev, fport);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
faArbMcFlush
(
    IN GT_U8        devNum
)
*/
UTF_TEST_CASE_MAC(faArbMcFlush)
{
/*
ITERATE_FA_DEVICES
1.1. Call function. Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function. Expected: GT_OK. */
        st = faArbMcFlush(dev);
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
        st = faArbMcFlush(dev);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = faArbMcFlush(dev);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
faArbIsFportEmpty
(
    IN GT_U8         devNum,
    IN GT_U8         fport,
    OUT GT_BOOL      *isEmpty
)
*/
UTF_TEST_CASE_MAC(faArbIsFportEmpty)
{
/*
ITERATE_FA_PORTS
1.1. Call function with non-NULL isEmpty. Expected: GT_OK.
1.2. Call function with isEmpty [NULL]. Expected: NON GT_OK.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U8           fport;
    GT_BOOL         retIsEmpty;

    GT_U8           maxFportsNum;

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
            /* 1.1. Call function with non-NULL isEmpty. Expected: GT_OK. */

            st = faArbIsFportEmpty(dev, fport, &retIsEmpty);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

            /* 1.2. Call function with isEmpty [NULL]. Expected: NON GT_OK. */

            st = faArbIsFportEmpty(dev, fport, NULL);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,"%d, %d, NULL", dev, fport);
        }

        /* prepare fabric port iterator */
        st = prvUtfNextFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non active fabric ports */
        while (GT_OK == prvUtfNextFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active fabric port */
            st = faArbIsFportEmpty(dev, fport, &retIsEmpty);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);
        }

        /* 1.3. For active device check that function returns NOT GT_OK    */
        /* for out of bound value for port number.                         */
        fport = maxFportsNum;

        st = faArbIsFportEmpty(dev, fport, &retIsEmpty);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = TX_ARBITER_VALID_FA_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = faArbIsFportEmpty(dev, fport, &retIsEmpty);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = faArbIsFportEmpty(dev, fport, &retIsEmpty);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
faArbIsMcEmpty
(
    IN  GT_U8         devNum,
    OUT GT_BOOL       *isEmpty
)
*/
UTF_TEST_CASE_MAC(faArbIsMcEmpty)
{
/*
ITERATE_FA_DEVICES
1.1. Call function with non-NULL isEmpty. Expected: GT_OK.
1.2. Call function with isEmpty [NULL]. Expected: NON GT_OK.
*/
    GT_STATUS           st = GT_OK;
    GT_U8               dev;
    GT_BOOL             retIsEmpty;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL isEmpty. Expected: GT_OK. */
        st = faArbIsMcEmpty(dev, &retIsEmpty);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with isEmpty [NULL]. Expected: NON GT_OK. */
        st = faArbIsMcEmpty(dev, NULL);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, NULL", dev);
    }

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = faArbIsMcEmpty(dev, &retIsEmpty);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = faArbIsMcEmpty(dev, &retIsEmpty);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
faArbConfigWrrProfileSetExt
(
    IN GT_U8                    devNum,
    IN GT_FA_TX_ARB_SET         setNum,
    IN GT_U8                    fPrio,
    IN GT_FA_TX_ARB_MODE        arbMode,
    IN GT_U32                   weight
)
*/
UTF_TEST_CASE_MAC(faArbConfigWrrProfileSetExt)
{
/*
ITERATE_FA_DEVICES(98Fx915)
1.1. Call function with setNum [GT_FA_TX_ARB_SET0/ GT_FA_TX_ARB_SET1/ 
GT_FA_TX_ARB_SET2/ GT_FA_TX_ARB_SET3], fPrio [0/ 1/ 2/ 3] and arbMode 
[GT_FA_TX_ARB_WRR/ GT_FA_TX_ARB_SP], weight [0/ 1/ 100/ 4095]. Expected: GT_OK.
1.2. Call function with out of range setNum [0x5AAAAAA5] and fPrio [0], arbMode
[GT_FA_TX_ARB_WRR], weight [0]. Expected: NON GT_OK.
1.3. Call function with out of range fPrio [NUM_FA_TC_QUEUES = 4] and setNum 
[GT_FA_TX_ARB_SET0], arbMode [GT_FA_TX_ARB_WRR], weight [0]. Expected: NON GT_OK.
1.4. Call function with out of range arbMode [0x5AAAAAA5] and fPrio [0], 
setNum [GT_FA_TX_ARB_SET0], weight [0]. Expected: NON GT_OK.
1.5. Call function with out of range weight [4096] and setNum 
[GT_FA_TX_ARB_SET0], arbMode [GT_FA_TX_ARB_WRR], fPrio [0]. Expected: NON GT_OK.
1.6. Call function with non-corresponding weight [4095] and arbMode 
[GT_FA_TX_ARB_WRR] (weight = 4095 corresponds to strict priority method), 
setNum [GT_FA_TX_ARB_SET0], fPrio [0]. Expected: NON GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_FA_TX_ARB_SET        setNum;
    GT_U8                   fPrio;
    GT_FA_TX_ARB_MODE       arbMode;
    GT_U32                  weight;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with setNum [GT_FA_TX_ARB_SET0/ GT_FA_TX_ARB_SET1/ 
        GT_FA_TX_ARB_SET2/ GT_FA_TX_ARB_SET3], fPrio [0/ 1/ 2/ 3] and arbMode 
        [GT_FA_TX_ARB_WRR/ GT_FA_TX_ARB_SP], weight [0/ 1/ 100/ 4095]. Expected: GT_OK. */
        setNum = GT_FA_TX_ARB_SET0;
        fPrio = 0;
        arbMode = GT_FA_TX_ARB_WRR;
        weight = 0;

        st = faArbConfigWrrProfileSetExt(dev, setNum, fPrio, arbMode, weight);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, setNum, fPrio, arbMode, weight);

        setNum = GT_FA_TX_ARB_SET1;
        fPrio = 1;
        arbMode = GT_FA_TX_ARB_SP;
        weight = 1;

        st = faArbConfigWrrProfileSetExt(dev, setNum, fPrio, arbMode, weight);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, setNum, fPrio, arbMode, weight);

        setNum = GT_FA_TX_ARB_SET2;
        fPrio = 2;
        weight = 100;

        st = faArbConfigWrrProfileSetExt(dev, setNum, fPrio, arbMode, weight);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, setNum, fPrio, arbMode, weight);

        setNum = GT_FA_TX_ARB_SET3;
        fPrio = 3;
        weight = 4095;

        st = faArbConfigWrrProfileSetExt(dev, setNum, fPrio, arbMode, weight);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, setNum, fPrio, arbMode, weight);

        /* 1.2. Call function with out of range setNum [0x5AAAAAA5] and fPrio [0], arbMode
        [GT_FA_TX_ARB_WRR], weight [0]. Expected: NON GT_OK. */
        setNum = TX_ARBITER_INVALID_ENUM_CNS;
        fPrio = 0;
        arbMode = GT_FA_TX_ARB_WRR;
        weight = 0;

        st = faArbConfigWrrProfileSetExt(dev, setNum, fPrio, arbMode, weight);
        UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, setNum, fPrio, arbMode, weight);

        setNum = GT_FA_TX_ARB_SET0;/* restore */

        /* 1.3. Call function with out of range fPrio [NUM_FA_TC_QUEUES = 4] and setNum 
        [GT_FA_TX_ARB_SET0], arbMode [GT_FA_TX_ARB_WRR], weight [0]. Expected: NON GT_OK. */
        fPrio = NUM_FA_TC_QUEUES;
        arbMode = GT_FA_TX_ARB_WRR;
        weight = 0;

        st = faArbConfigWrrProfileSetExt(dev, setNum, fPrio, arbMode, weight);
        UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, setNum, fPrio, arbMode, weight);

        fPrio = 0; /* restore */

        /* 1.4. Call function with out of range arbMode [0x5AAAAAA5] and fPrio [0], 
        setNum [GT_FA_TX_ARB_SET0], weight [0]. Expected: NON GT_OK. */
        arbMode = TX_ARBITER_INVALID_ENUM_CNS;
        setNum = GT_FA_TX_ARB_SET0;
        weight = 0;

        st = faArbConfigWrrProfileSetExt(dev, setNum, fPrio, arbMode, weight);
        UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, setNum, fPrio, arbMode, weight);

        arbMode = GT_FA_TX_ARB_WRR; /* restore */

        /* 1.5. Call function with out of range weight [4096] and setNum 
        [GT_FA_TX_ARB_SET0], arbMode [GT_FA_TX_ARB_WRR], fPrio [0]. Expected: NON GT_OK. */
        weight = 4096;
        setNum = GT_FA_TX_ARB_SET0;
        weight = 0;
        fPrio = 0;

        st = faArbConfigWrrProfileSetExt(dev, setNum, fPrio, arbMode, weight);
        UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, setNum, fPrio, arbMode, weight);

        /* 1.6. Call function with non-corresponding weight [4095] and arbMode 
        [GT_FA_TX_ARB_WRR] (weight = 4095 corresponds to strict priority method), 
        setNum [GT_FA_TX_ARB_SET0], fPrio [0]. Expected: NON GT_OK. */
        weight = 4095;
        arbMode = GT_FA_TX_ARB_WRR;
        setNum = GT_FA_TX_ARB_SET0;
        fPrio = 0;

        st = faArbConfigWrrProfileSetExt(dev, setNum, fPrio, arbMode, weight);
        UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, setNum, fPrio, arbMode, weight);
    }
    setNum = GT_FA_TX_ARB_SET0;
    fPrio = 0;
    arbMode = GT_FA_TX_ARB_WRR;
    weight = 0;

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_CPSS_FA_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = faArbConfigWrrProfileSetExt(dev, setNum, fPrio, arbMode, weight);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = faArbConfigWrrProfileSetExt(dev, setNum, fPrio, arbMode, weight);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of gtFaApiTxArbiter suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(gtFaApiTxArbiter)
    UTF_SUIT_DECLARE_TEST_MAC(faTxArbEnable)
    UTF_SUIT_DECLARE_TEST_MAC(faArbConfigWrrProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(faArbBindFport2WrrProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(faArbBindMc2WrrProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(faArbSetUnMcRatio)
    UTF_SUIT_DECLARE_TEST_MAC(faArbFportFlush)
    UTF_SUIT_DECLARE_TEST_MAC(faArbMcFlush)
    UTF_SUIT_DECLARE_TEST_MAC(faArbIsFportEmpty)
    UTF_SUIT_DECLARE_TEST_MAC(faArbIsMcEmpty)
    UTF_SUIT_DECLARE_TEST_MAC(faArbConfigWrrProfileSetExt)
UTF_SUIT_END_TESTS_MAC(gtFaApiTxArbiter)
