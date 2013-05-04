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
*  gtFaApiRxReassemUT.c
*
* DESCRIPTION:
*       Unit tests for gtFaApiRxReassem, that provides 
*       Prestera API implemantation for Fabric Adapter Packet Cell Reassembly
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
/* includes */
#include <cpssFa/generic/rxReassem/gtFaRxReassem.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */
#define RX_REASSEM_INVALID_ENUM_CNS 0x5AAAAAA5

/*----------------------------------------------------------------------------*/
/*
faRxConfigCounterView
(
    IN  GT_U8                       devNum,
    IN  GT_FA_RX_COUNTER_SET        setNum,
    IN  GT_FA_RX_COUNTER_TYPE       counterType,
    IN  GT_FA_RX_CELL_TYPE_IN_VIEW  cellType,
    IN  GT_FA_RX_CELL_PRIO_IN_VIEW  cellPrio,
    IN  GT_U8                       srcDevNum
)
*/
UTF_TEST_CASE_MAC(faRxConfigCounterView)
{
/*
ITERATE_FA_DEVICES
1.1. Call function with setNum [GT_FA_RX_COUNT_SET0 / GT_FA_RX_COUNT_SET1 / 
GT_FA_RX_COUNT_SET2 / GT_FA_RX_COUNT_SET3], counterType [GT_FA_RX_CELL_COUNTER
/ GT_FA_RX_BAD_CELL_CRC_DROP_COUNTER / GT_FA_RX_BAD_PKT_LEN_DROP_COUNTER], 
cellType [GT_FA_RX_CELL_ALL / GT_FA_RX_CELL_UC / GT_FA_RX_CELL_MC], 
cellPrio [GT_FA_RX_CELL_PRIO_ALL = 0 /  GT_FA_RX_CELL_PRIO_0 /  
GT_FA_RX_CELL_PRIO_1 /  GT_FA_RX_CELL_PRIO_2 /  GT_FA_RX_CELL_PRIO_3] and
srcDevNum [0 / 32 / getMaxDevNum(dev) - 1 / 0xFF]. Expected: GT_OK.
1.2. Call function with out of range setNum [0x5AAAAAA5] and other parameters
the same as in 1.1. Expected: NOT GT_OK.
1.3. Call function with out of range counterType [0x5AAAAAA5] and other 
parameters the same as in 1.1. Expected: NOT GT_OK.
1.4. Call function with out of range cellType [0x5AAAAAA5] and other parameters
the same as in 1.1. Expected: NOT GT_OK.
1.5. Call function with out of range cellPrio [0x5AAAAAA5] and other parameters
the same as in 1.1. Expected: NOT GT_OK.
1.6. Call function with out of range srcDevNum [getMaxDevNum(dev)]
*/
    GT_STATUS                       st = GT_OK;
    GT_U8                           dev;
    GT_FA_RX_COUNTER_SET            setNum;
    GT_FA_RX_COUNTER_TYPE           counterType;
    GT_FA_RX_CELL_TYPE_IN_VIEW      cellType;
    GT_FA_RX_CELL_PRIO_IN_VIEW      cellPrio;
    GT_U8                           srcDevNum;
    GT_FA_MAX_NUM_DEV               maxDevNum;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* Getting max number of packet processors */
        st = prvUtfFaMaxNumPpGet(dev, &maxDevNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfFaMaxNumPpGet: %d", dev);

        /* 1.1. Call function with setNum [GT_FA_RX_COUNT_SET0 / GT_FA_RX_COUNT_SET1 / 
        GT_FA_RX_COUNT_SET2 / GT_FA_RX_COUNT_SET3], counterType [GT_FA_RX_CELL_COUNTER
        / GT_FA_RX_BAD_CELL_CRC_DROP_COUNTER / GT_FA_RX_BAD_PKT_LEN_DROP_COUNTER], 
        cellType [GT_FA_RX_CELL_ALL / GT_FA_RX_CELL_UC / GT_FA_RX_CELL_MC], 
        cellPrio [GT_FA_RX_CELL_PRIO_ALL = 0 /  GT_FA_RX_CELL_PRIO_0 /  
        GT_FA_RX_CELL_PRIO_1 /  GT_FA_RX_CELL_PRIO_2 /  GT_FA_RX_CELL_PRIO_3] and
        srcDevNum [0 / 32 / getMaxDevNum(dev) - 1 / 0xFF]. Expected: GT_OK. */
        setNum = GT_FA_RX_COUNT_SET0;
        counterType = GT_FA_RX_CELL_COUNTER;
        cellType = GT_FA_RX_CELL_ALL;
        cellPrio = GT_FA_RX_CELL_PRIO_ALL;
        srcDevNum = 0;

        st = faRxConfigCounterView(dev, setNum, counterType, cellType, cellPrio, srcDevNum);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, setNum, counterType, cellType, cellPrio, srcDevNum);

        setNum = GT_FA_RX_COUNT_SET1;
        counterType = GT_FA_RX_BAD_CELL_CRC_DROP_COUNTER;
        cellType = GT_FA_RX_CELL_UC;
        cellPrio = GT_FA_RX_CELL_PRIO_0;
        srcDevNum = 32;

        st = faRxConfigCounterView(dev, setNum, counterType, cellType, cellPrio, srcDevNum);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, setNum, counterType, cellType, cellPrio, srcDevNum);

        setNum = GT_FA_RX_COUNT_SET2;
        counterType = GT_FA_RX_BAD_PKT_LEN_DROP_COUNTER;
        cellType = GT_FA_RX_CELL_MC;
        cellPrio = GT_FA_RX_CELL_PRIO_1;
        srcDevNum = maxDevNum - 1;

        st = faRxConfigCounterView(dev, setNum, counterType, cellType, cellPrio, srcDevNum);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, setNum, counterType, cellType, cellPrio, srcDevNum);

        setNum = GT_FA_RX_COUNT_SET3;
        cellPrio = GT_FA_RX_CELL_PRIO_2;
        srcDevNum = 0xFF;

        st = faRxConfigCounterView(dev, setNum, counterType, cellType, cellPrio, srcDevNum);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, setNum, counterType, cellType, cellPrio, srcDevNum);

        srcDevNum = 0;
        cellPrio = GT_FA_RX_CELL_PRIO_3;

        st = faRxConfigCounterView(dev, setNum, counterType, cellType, cellPrio, srcDevNum);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, setNum, counterType, cellType, cellPrio, srcDevNum);

        /* 1.2. Call function with out of range setNum [0x5AAAAAA5] and other parameters
        the same as in 1.1. Expected: NOT GT_OK. */
        setNum = RX_REASSEM_INVALID_ENUM_CNS;

        st = faRxConfigCounterView(dev, setNum, counterType, cellType, cellPrio, srcDevNum);
        UTF_VERIFY_NOT_EQUAL6_PARAM_MAC(GT_OK, st, dev, setNum, counterType, cellType, cellPrio, srcDevNum);

        setNum = GT_FA_RX_COUNT_SET0;/* restore */

        /* 1.3. Call function with out of range counterType [0x5AAAAAA5] and other 
        parameters the same as in 1.1. Expected: NOT GT_OK. */
        counterType = RX_REASSEM_INVALID_ENUM_CNS;

        st = faRxConfigCounterView(dev, setNum, counterType, cellType, cellPrio, srcDevNum);
        UTF_VERIFY_NOT_EQUAL6_PARAM_MAC(GT_OK, st, dev, setNum, counterType, cellType, cellPrio, srcDevNum);

        /* restore */
        counterType = GT_FA_RX_CELL_COUNTER;

        /* 1.4. Call function with out of range cellType [0x5AAAAAA5] and other parameters
        the same as in 1.1. Expected: NOT GT_OK. */
        cellType = RX_REASSEM_INVALID_ENUM_CNS;

        st = faRxConfigCounterView(dev, setNum, counterType, cellType, cellPrio, srcDevNum);
        UTF_VERIFY_NOT_EQUAL6_PARAM_MAC(GT_OK, st, dev, setNum, counterType, cellType, cellPrio, srcDevNum);

        cellType = GT_FA_RX_CELL_ALL;/* restore*/

        /* 1.5. Call function with out of range cellPrio [0x5AAAAAA5] and other parameters
        the same as in 1.1. Expected: NOT GT_OK. */
        cellPrio = RX_REASSEM_INVALID_ENUM_CNS;

        st = faRxConfigCounterView(dev, setNum, counterType, cellType, cellPrio, srcDevNum);
        UTF_VERIFY_NOT_EQUAL6_PARAM_MAC(GT_OK, st, dev, setNum, counterType, cellType, cellPrio, srcDevNum);

        cellPrio = GT_FA_RX_CELL_PRIO_ALL;/* restore */

        /* 1.6. Call function with out of range srcDevNum [getMaxDevNum(dev)] and
        other parameters the same as in 1.1. Expected: NON GT_OK. */
        srcDevNum = maxDevNum;

        st = faRxConfigCounterView(dev, setNum, counterType, cellType, cellPrio, srcDevNum);
        UTF_VERIFY_NOT_EQUAL6_PARAM_MAC(GT_OK, st, dev, setNum, counterType, cellType, cellPrio, srcDevNum);
    }
    setNum = GT_FA_RX_COUNT_SET0;
    counterType = GT_FA_RX_CELL_COUNTER;
    cellType = GT_FA_RX_CELL_ALL;
    cellPrio = GT_FA_RX_CELL_PRIO_ALL;
    srcDevNum = 0;

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = faRxConfigCounterView(dev, setNum, counterType, cellType, cellPrio, srcDevNum);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = faRxConfigCounterView(dev, setNum, counterType, cellType, cellPrio, srcDevNum);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
faRxGetViewCounter
(
    IN  GT_U8                  devNum,
    IN  GT_FA_RX_COUNTER_TYPE  cntType,
    IN  GT_FA_RX_COUNTER_SET   cntSet,
    OUT GT_U32                *counterVal
)
*/
UTF_TEST_CASE_MAC(faRxGetViewCounter)
{
/*
ITERATE_FA_DEVICES
1.1. Call function with cntType [GT_FA_RX_CELL_COUNTER / 
GT_FA_RX_BAD_CELL_CRC_DROP_COUNTER / GT_FA_RX_BAD_PKT_LEN_DROP_COUNTER],
cntSet [GT_FA_RX_COUNT_SET0 / GT_FA_RX_COUNT_SET1 / GT_FA_RX_COUNT_SET2 /
GT_FA_RX_COUNT_SET3] and non-NULL counterVal. Expected: GT_OK.
1.2. Call function with out of range cntType [0x5AAAAAA5] and other parameters
the same as in 1.1. Expected: NOT GT_OK.
1.3. Call function with out of range cntSet [0x5AAAAAA5] and other parameters
the same as in 1.1. Expected: NOT GT_OK.
1.4. Call function with counterVal [NULL] and other parameters the same as 
in 1.1. Expected: NOT GT_OK.
*/
    GT_STATUS                st = GT_OK;
    GT_U8                    dev;
    GT_FA_RX_COUNTER_TYPE    cntType;
    GT_FA_RX_COUNTER_SET     cntSet;
    GT_U32                   retCounterVal;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /*1.1. Call function with cntType [GT_FA_RX_CELL_COUNTER / 
        GT_FA_RX_BAD_CELL_CRC_DROP_COUNTER / GT_FA_RX_BAD_PKT_LEN_DROP_COUNTER],
        cntSet [GT_FA_RX_COUNT_SET0 / GT_FA_RX_COUNT_SET1 / GT_FA_RX_COUNT_SET2 /
        GT_FA_RX_COUNT_SET3] and non-NULL counterVal. Expected: GT_OK. */
        cntType = GT_FA_RX_CELL_COUNTER;
        cntSet = GT_FA_RX_COUNT_SET0;

        st = faRxGetViewCounter(dev, cntType, cntSet, &retCounterVal);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntType, cntSet);

        cntType = GT_FA_RX_BAD_CELL_CRC_DROP_COUNTER;
        cntSet = GT_FA_RX_COUNT_SET1;

        st = faRxGetViewCounter(dev, cntType, cntSet, &retCounterVal);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntType, cntSet);

        cntType = GT_FA_RX_BAD_PKT_LEN_DROP_COUNTER;
        cntSet = GT_FA_RX_COUNT_SET2;

        st = faRxGetViewCounter(dev, cntType, cntSet, &retCounterVal);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntType, cntSet);

        cntSet = GT_FA_RX_COUNT_SET3;

        st = faRxGetViewCounter(dev, cntType, cntSet, &retCounterVal);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntType, cntSet);

        /* 1.2. Call function with out of range cntType [0x5AAAAAA5] and other parameters
        the same as in 1.1. Expected: NOT GT_OK. */
        cntType = RX_REASSEM_INVALID_ENUM_CNS;

        st = faRxGetViewCounter(dev, cntType, cntSet, &retCounterVal);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntType, cntSet);

        cntType = GT_FA_RX_CELL_COUNTER;/* restore */

        /* 1.3. Call function with out of range cntSet [0x5AAAAAA5] and other parameters
        the same as in 1.1. Expected: NOT GT_OK. */
        cntSet = RX_REASSEM_INVALID_ENUM_CNS;

        st = faRxGetViewCounter(dev, cntType, cntSet, &retCounterVal);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntType, cntSet);

        cntSet = GT_FA_RX_COUNT_SET0; /* restore */

        /* 1.4. Call function with counterVal [NULL] and other parameters the same as 
        in 1.1. Expected: NOT GT_OK. */

        st = faRxGetViewCounter(dev, cntType, cntSet, NULL);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, %d, NULL", dev, cntType, cntSet);
    }
    cntType = GT_FA_RX_CELL_COUNTER;
    cntSet = GT_FA_RX_COUNT_SET0;

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = faRxGetViewCounter(dev, cntType, cntSet, &retCounterVal);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = faRxGetViewCounter(dev, cntType, cntSet, &retCounterVal);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
faRxGetGlobalCounters
(
    IN  GT_U8                       devNum,
    OUT GT_FA_RX_GLOBAL_COUNTERS    *rxGlobalCounters
)
*/
UTF_TEST_CASE_MAC(faRxGetGlobalCounters)
{
/*
ITERATE_FA_DEVICES
1.1. Call function with non-NULL rxGlobalCounters. Expected: GT_OK.
1.2. Call function with rxGlobalCounters [NULL]. Expected: NOT GT_OK.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_FA_RX_GLOBAL_COUNTERS    retRxGlobalCounters;

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL rxGlobalCounters. Expected: GT_OK. */
        st = faRxGetGlobalCounters(dev, &retRxGlobalCounters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with rxGlobalCounters [NULL]. Expected: NOT GT_OK. */
        st = faRxGetGlobalCounters(dev, NULL);
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
        st = faRxGetGlobalCounters(dev, &retRxGlobalCounters);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = faRxGetGlobalCounters(dev, &retRxGlobalCounters);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of gtFaApiRxReassem suit
*/
UTF_SUIT_BEGIN_TESTS_MAC(gtFaApiRxReassem)
    UTF_SUIT_DECLARE_TEST_MAC(faRxConfigCounterView)
    UTF_SUIT_DECLARE_TEST_MAC(faRxGetViewCounter)
    UTF_SUIT_DECLARE_TEST_MAC(faRxGetGlobalCounters)
UTF_SUIT_END_TESTS_MAC(gtFaApiRxReassem)
