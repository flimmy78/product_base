/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmFabricSerdesUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmFabricSerdes, that provides
*       CPSS EXMXPM Fabric Connectivity SERDES API definitions.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricSerdes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define FABRIC_SERDES_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSerdesStatusGet
(
    IN  GT_U8                          devNum,
    IN  GT_FPORT                       fport,
    OUT CPSS_EXMXPM_FABRIC_SERDES_STATUS_STC  *serdesStatusPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSerdesStatusGet)
{
/*
    1.1. Call with fport [0 / 3]
                   and non-null serdesStatusPtr.
    Expected: GT_OK.
    1.3. Call with out of range fport [1]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with serdesStatusPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                              fport;
    CPSS_EXMXPM_FABRIC_SERDES_STATUS_STC  serdesStatus;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3]
                           and non-null serdesStatusPtr.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;

        st = cpssExMxPmSerdesStatusGet(dev, fport, &serdesStatus);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /* Call with fport [3] */
        fport   = 3;

        st = cpssExMxPmSerdesStatusGet(dev, fport, &serdesStatus);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.3. Call with out of range fport [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmSerdesStatusGet(dev, fport, &serdesStatus);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.4. Call with serdesStatusPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSerdesStatusGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, serdesStatusPtr = NULL", dev);
    }

    fport   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSerdesStatusGet(dev, fport, &serdesStatus);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSerdesStatusGet(dev, fport, &serdesStatus);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSerdesResetSet
(
    IN  GT_U8       devNum,
    IN  GT_FPORT    fport,
    IN  GT_BOOL     resetSerdesLaneArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSerdesResetSet)
{
/*
    1.1. Call with fport [0 / 3]
                   and resetSerdesLaneArr [GT_FALSE, …, GT_FALSE / GT_TRUE, …, GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSerdesResetGet with the same fport
                                            and non-NULL resetSerdesLaneArr.
    Expected: GT_OK and the same resetSerdesLaneArr.
    1.4. Call with out of range fport [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with resetSerdesLaneArr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT    fport   = 0;
    GT_BOOL     resetSerdesLaneArr   [CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];
    GT_BOOL     resetSerdesLaneArrGet[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];
    GT_U32      laneIter = 0;
    GT_BOOL     isEqual  = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3]
                           and resetSerdesLaneArr [GT_FALSE, …, GT_FALSE / GT_TRUE, …, GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;

        for (laneIter = 0; laneIter < CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS; laneIter++)
        {
            resetSerdesLaneArr[laneIter] = GT_FALSE;
        }

        st = cpssExMxPmSerdesResetSet(dev, fport, resetSerdesLaneArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmSerdesResetGet with the same fport
                                                    and non-NULL resetSerdesLaneArr.
            Expected: GT_OK and the same resetSerdesLaneArr.
        */
        st = cpssExMxPmSerdesResetGet(dev, fport, resetSerdesLaneArrGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSerdesResetGet: %d, %d", dev, fport);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) resetSerdesLaneArr,
                                     (GT_VOID*) resetSerdesLaneArrGet,
                                     sizeof(resetSerdesLaneArr[0]*CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another resetSerdesLaneArr than was set: %d", dev);

        /* Call with fport [1] */
        fport   = 3;

        for (laneIter = 0; laneIter < CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS; laneIter++)
        {
            resetSerdesLaneArr[laneIter] = GT_TRUE;
        }

        st = cpssExMxPmSerdesResetSet(dev, fport, resetSerdesLaneArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmSerdesResetGet with the same fport
                                                    and non-NULL resetSerdesLaneArr.
            Expected: GT_OK and the same resetSerdesLaneArr.
        */
        st = cpssExMxPmSerdesResetGet(dev, fport, resetSerdesLaneArrGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSerdesResetGet: %d, %d", dev, fport);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) resetSerdesLaneArr,
                                     (GT_VOID*) resetSerdesLaneArrGet,
                                     sizeof(resetSerdesLaneArr[0]*CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another resetSerdesLaneArr than was set: %d", dev);

        /*
            1.4. Call with out of range fport [4
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmSerdesResetSet(dev, fport, resetSerdesLaneArr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.5. Call with resetSerdesLaneArr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSerdesResetSet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, resetSerdesLaneArr = NULL", dev);
    }

    fport   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSerdesResetSet(dev, fport, resetSerdesLaneArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSerdesResetSet(dev, fport, resetSerdesLaneArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSerdesResetGet
(
    IN  GT_U8       devNum,
    IN  GT_FPORT    fport,
    OUT GT_BOOL     resetSerdesLaneArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSerdesResetGet)
{
/*
    1.1. Call with fport [0 / 3]
                   and non-NULL resetSerdesLaneArr.
    Expected: GT_OK.
    1.3. Call with out of range fport [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with resetSerdesLaneArr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT    fport   = 0;
    GT_BOOL     resetSerdesLaneArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3]
                           and non-NULL resetSerdesLaneArr.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport = 0;

        st = cpssExMxPmSerdesResetGet(dev, fport, resetSerdesLaneArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /* Call with fport [3] */
        fport = 3;

        st = cpssExMxPmSerdesResetGet(dev, fport, resetSerdesLaneArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.3. Call with out of range fport [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmSerdesResetGet(dev, fport, resetSerdesLaneArr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.4. Call with resetSerdesLaneArr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSerdesResetGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, resetSerdesLaneArr = NULL", dev);
    }

    fport = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSerdesResetGet(dev, fport, resetSerdesLaneArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSerdesResetGet(dev, fport, resetSerdesLaneArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSerdesPowerSet
(
    IN  GT_U8     devNum,
    IN  GT_FPORT  fport,
    IN  GT_BOOL   powerSerdesLaneArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSerdesPowerSet)
{
/*
    1.1. Call with fport [0 / 3],
                   powerSerdesLaneArr [GT_FALSE, …, GT_FALSE / GT_TRUE, …, GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSerdesPowerGet with the same fport
                                            and non-NULL powerSerdesRxLaneArr,
                                            powerSerdesTxLaneArr.
    Expected: GT_OK and the same powerSerdesLaneArr.
    1.4. Call with out of range fport [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with powerSerdesLaneArr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT    fport   = 0;
    GT_BOOL     powerSerdesLaneArr   [CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];
    GT_BOOL     powerSerdesLaneArrGet[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];
    GT_U32      laneIter = 0;
    GT_BOOL     isEqual  = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3],
                           powerSerdesLaneArr [GT_FALSE, …, GT_FALSE / GT_TRUE, …, GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;

        for (laneIter = 0; laneIter < CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS; laneIter++)
        {
            powerSerdesLaneArr[laneIter] = GT_FALSE;
        }

        st = cpssExMxPmSerdesPowerSet(dev, fport, powerSerdesLaneArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmSerdesPowerGet with the same fport
                                                    and non-NULL powerSerdesLaneArr.
            Expected: GT_OK and the same powerSerdesLaneArr.
        */
        st = cpssExMxPmSerdesPowerGet(dev, fport, powerSerdesLaneArrGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSerdesPowerGet: %d, %d", dev, fport);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) powerSerdesLaneArr,
                                     (GT_VOID*) powerSerdesLaneArrGet,
                                     sizeof(powerSerdesLaneArr[0]*CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another powerSerdesLaneArr than was set: %d", dev);

        /* Call with fport [3] */
        fport   = 3;

        for (laneIter = 0; laneIter < CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS; laneIter++)
        {
            powerSerdesLaneArr[laneIter] = GT_TRUE;
        }

        st = cpssExMxPmSerdesPowerSet(dev, fport, powerSerdesLaneArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmSerdesPowerGet with the same fport
                                                    and non-NULL powerSerdesLaneArr.
            Expected: GT_OK and the same powerSerdesLaneArr.
        */
        st = cpssExMxPmSerdesPowerGet(dev, fport, powerSerdesLaneArrGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSerdesPowerGet: %d, %d", dev, fport);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) powerSerdesLaneArr,
                                     (GT_VOID*) powerSerdesLaneArrGet,
                                     sizeof(powerSerdesLaneArr[0]*CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another powerSerdesLaneArr than was set: %d", dev);


        /*
            1.4. Call with out of range fport [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmSerdesPowerSet(dev, fport, powerSerdesLaneArr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.5. Call with powerSerdesLaneArr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSerdesPowerSet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, powerSerdesLaneArr = NULL", dev);
    }

    fport   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSerdesPowerSet(dev, fport, powerSerdesLaneArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSerdesPowerSet(dev, fport, powerSerdesLaneArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSerdesPowerGet
(
    IN  GT_U8     devNum,
    IN  GT_FPORT  fport,
    OUT GT_BOOL   powerSerdesLaneArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSerdesPowerGet)
{
/*
    1.1. Call with fport [0 / 3]
                   and non-NULL powerSerdesLaneArr.
    Expected: GT_OK.
    1.3. Call with out of range fport [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with powerSerdesLaneArr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT    fport   = 0;
    GT_BOOL     powerSerdesLaneArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3]
                           and non-NULL powerSerdesRxLaneArr,
                           powerSerdesTxLaneArr.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;

        st = cpssExMxPmSerdesPowerGet(dev, fport, powerSerdesLaneArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /* Call with fport [3] */
        fport   = 3;

        st = cpssExMxPmSerdesPowerGet(dev, fport, powerSerdesLaneArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.3. Call with out of range fport [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmSerdesPowerGet(dev, fport, powerSerdesLaneArr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.4. Call with powerSerdesLaneArr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSerdesPowerGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, powerSerdesLaneArr = NULL", dev);
    }

    fport   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSerdesPowerGet(dev, fport, powerSerdesLaneArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSerdesPowerGet(dev, fport, powerSerdesLaneArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSerdesLoopbackEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_FPORT  fport,
    IN  GT_BOOL   loopbackEnableArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSerdesLoopbackEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 3], loopbackEnableArr [GT_FALSE, GT_FALSE / GT_TRUE, GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSerdesLoopbackEnableGet with fport from 1.1. and NOT NULL pointer.
    Expected: GT_OK and the same loopbackEnableArr as was set.
    1.3. Call with fport [4] (out of range).
    Expected: GT_OK.
    1.4. Call with loopbackEnableArr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U32      iTemp;
    GT_BOOL     isEqual = GT_FALSE;

    GT_FPORT    fport = 0;
    GT_BOOL     loopbackEnableArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];
    GT_BOOL     loopbackEnableArrGet[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];


    cpssOsBzero((GT_VOID*) &loopbackEnableArr, sizeof(loopbackEnableArr));
    cpssOsBzero((GT_VOID*) &loopbackEnableArrGet, sizeof(loopbackEnableArrGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3], loopbackEnableArr [GT_FALSE, GT_FALSE / GT_TRUE, GT_TRUE].
            Expected: GT_OK.
        */
        /* iterate with fport = 0 */
        fport = 0;
        for(iTemp=0; iTemp<CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS; ++iTemp)
        {
            loopbackEnableArr[iTemp] = GT_FALSE;
        }

        st = cpssExMxPmSerdesLoopbackEnableSet(dev, fport, loopbackEnableArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmSerdesLoopbackEnableGet with fport from 1.1. and NOT NULL pointer.
            Expected: GT_OK and the same loopbackEnableArr as was set.
        */
        st = cpssExMxPmSerdesLoopbackEnableGet(dev, fport, loopbackEnableArrGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSerdesLoopbackEnableGet: %d, %d", dev, fport);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &loopbackEnableArr,
                                     (GT_VOID*) &loopbackEnableArrGet,
                                     sizeof(loopbackEnableArr))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another loopbackEnableArr than was set: %d", dev);

        /* iterate with fport = 3 */
        fport = 3;
        for(iTemp=0; iTemp<CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS; ++iTemp)
        {
            loopbackEnableArr[iTemp] = GT_TRUE;
        }

        st = cpssExMxPmSerdesLoopbackEnableSet(dev, fport, loopbackEnableArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmSerdesLoopbackEnableGet with fport from 1.1. and NOT NULL pointer.
            Expected: GT_OK and the same loopbackEnableArr as was set.
        */
        st = cpssExMxPmSerdesLoopbackEnableGet(dev, fport, loopbackEnableArrGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSerdesLoopbackEnableGet: %d, %d", dev, fport);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &loopbackEnableArr,
                                     (GT_VOID*) &loopbackEnableArrGet,
                                     sizeof(loopbackEnableArr))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another loopbackEnableArr than was set: %d", dev);

        /*
            1.3. Call with fport [4] (out of range).
            Expected: GT_OK.
        */
        fport = 4;

        st = cpssExMxPmSerdesLoopbackEnableSet(dev, fport, loopbackEnableArr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 0;

        /*
            1.4. Call with loopbackEnableArr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSerdesLoopbackEnableSet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, loopbackEnableArr = NULL", dev);
    }

    fport = 0;
    for(iTemp=0; iTemp<CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS; ++iTemp)
    {
        loopbackEnableArr[iTemp] = GT_FALSE;
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSerdesLoopbackEnableSet(dev, fport, loopbackEnableArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSerdesLoopbackEnableSet(dev, fport, loopbackEnableArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSerdesLoopbackEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_FPORT  fport,
    OUT GT_BOOL   loopbackEnableArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSerdesLoopbackEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 3], not NULL loopbackEnableArr.
    Expected: GT_OK.
    1.2. Call with fport [4] (out of range).
    Expected: GT_OK.
    1.3. Call with loopbackEnableArr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT  fport = 0;
    GT_BOOL   loopbackEnableArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3], not NULL loopbackEnableArr.
            Expected: GT_OK.
        */
        /* iterate with fport = 0 */
        fport = 0;

        st = cpssExMxPmSerdesLoopbackEnableGet(dev, fport, loopbackEnableArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /* iterate with fport = 3 */
        fport = 3;

        st = cpssExMxPmSerdesLoopbackEnableGet(dev, fport, loopbackEnableArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call with fport [4] (out of range).
            Expected: GT_OK.
        */
        fport = 4;

        st = cpssExMxPmSerdesLoopbackEnableGet(dev, fport, loopbackEnableArr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 0;

        /*
            1.3. Call with loopbackEnableArr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSerdesLoopbackEnableGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, loopbackEnableArr = NULL", dev);
    }

    fport = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSerdesLoopbackEnableGet(dev, fport, loopbackEnableArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSerdesLoopbackEnableGet(dev, fport, loopbackEnableArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSerdesTxVoltageSwingCfgSet
(
    IN  GT_U8     devNum,
    IN  GT_FPORT  fport,
    IN  GT_U32    laneNum,
    IN  CPSS_EXMXPM_FABRIC_SERDES_TX_VLTG_SWING_CFG_STC   *txVltgSwingCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSerdesTxVoltageSwingCfgSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 3],
                   laneNum [0 / 5],
                   txVltgSwingCfg{ txAmp [0 / 31],
                                   emphasisType [CPSS_EXMXPM_FABRIC_SERDES_EMPHASIS_TYPE_DE_EMPHASIS_E /
                                                 CPSS_EXMXPM_FABRIC_SERDES_EMPHASIS_TYPE_PRE_EMPHASIS_E]
                                   txEmphasisEnable [GT_TRUE / GT_FALSE],
                                   emphasisLevelAdjustEnable [GT_TRUE / GT_FALSE],
                                   txEmphasisAmp [0 / 15]}
    Expected: GT_OK.
    1.2. Call cpssExMxPmSerdesTxVoltageSwingCfgGet with not NULL pointer and other params from 1.1.
    Expected: GT_OK and the same txVltgSwingCfg as was set.
    1.3. Call with fport [4] (out of range).
    Expected: NOT GT_OK.
    1.4. Call with laneNum [6] (out of range).
    Expected: NOT GT_OK.
    1.5. Call with txVltgSwingCfg->emphasisType [0x5AAAAAA5] (out of range).
    Expected: GT_BAD_PARAM.
    1.6. Call with txVltgSwingCfgPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_BOOL     isEqual = GT_FALSE;

    GT_FPORT  fport   = 0;
    GT_U32    laneNum = 0;
    CPSS_EXMXPM_FABRIC_SERDES_TX_VLTG_SWING_CFG_STC txVltgSwingCfg;
    CPSS_EXMXPM_FABRIC_SERDES_TX_VLTG_SWING_CFG_STC txVltgSwingCfgGet;


    cpssOsBzero((GT_VOID*) &txVltgSwingCfg, sizeof(txVltgSwingCfg));
    cpssOsBzero((GT_VOID*) &txVltgSwingCfgGet, sizeof(txVltgSwingCfgGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3],
                           laneNum [0 / 5],
                           txVltgSwingCfg{ txAmp [0 / 31],
                                           emphasisType [CPSS_EXMXPM_FABRIC_SERDES_EMPHASIS_TYPE_DE_EMPHASIS_E /
                                                         CPSS_EXMXPM_FABRIC_SERDES_EMPHASIS_TYPE_PRE_EMPHASIS_E]
                                           txEmphasisEnable [GT_TRUE / GT_FALSE],
                                           emphasisLevelAdjustEnable [GT_TRUE / GT_FALSE],
                                           txEmphasisAmp [0 / 15]}
            Expected: GT_OK.
        */
        /* iterate with fport = 0 */
        fport = 0;
        laneNum = 0;
        txVltgSwingCfg.txAmp = 0;
        txVltgSwingCfg.emphasisType = CPSS_EXMXPM_FABRIC_SERDES_EMPHASIS_TYPE_DE_EMPHASIS_E;
        txVltgSwingCfg.txEmphasisEnable = GT_TRUE;
        txVltgSwingCfg.emphasisLevelAdjustEnable = GT_TRUE;
        txVltgSwingCfg.txEmphasisAmp = 0;

        st = cpssExMxPmSerdesTxVoltageSwingCfgSet(dev, fport, laneNum, &txVltgSwingCfg);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, laneNum);

        /*
            1.2. Call cpssExMxPmSerdesTxVoltageSwingCfgGet with not NULL pointer and other params from 1.1.
            Expected: GT_OK and the same txVltgSwingCfg as was set.
        */
        st = cpssExMxPmSerdesTxVoltageSwingCfgGet(dev, fport, laneNum, &txVltgSwingCfgGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmSerdesTxVoltageSwingCfgGet: %d, %d, %d", dev, fport, laneNum);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &txVltgSwingCfg,
                                     (GT_VOID*) &txVltgSwingCfgGet,
                                     sizeof(txVltgSwingCfg))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another txVltgSwingCfg than was set: %d", dev);

        /* iterate with fport = 3 */
        fport = 3;
        laneNum = 5;
        txVltgSwingCfg.txAmp = 31;
        txVltgSwingCfg.emphasisType = CPSS_EXMXPM_FABRIC_SERDES_EMPHASIS_TYPE_PRE_EMPHASIS_E;
        txVltgSwingCfg.txEmphasisEnable = GT_FALSE;
        txVltgSwingCfg.emphasisLevelAdjustEnable = GT_FALSE;
        txVltgSwingCfg.txEmphasisAmp = 15;

        st = cpssExMxPmSerdesTxVoltageSwingCfgSet(dev, fport, laneNum, &txVltgSwingCfg);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, laneNum);

        /*
            1.2. Call cpssExMxPmSerdesTxVoltageSwingCfgGet with not NULL pointer and other params from 1.1.
            Expected: GT_OK and the same txVltgSwingCfg as was set.
        */
        st = cpssExMxPmSerdesTxVoltageSwingCfgGet(dev, fport, laneNum, &txVltgSwingCfgGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmSerdesTxVoltageSwingCfgGet: %d, %d, %d", dev, fport, laneNum);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &txVltgSwingCfg,
                                     (GT_VOID*) &txVltgSwingCfgGet,
                                     sizeof(txVltgSwingCfg))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another txVltgSwingCfg than was set: %d", dev);

        /*
            1.3. Call with fport [4] (out of range).
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmSerdesTxVoltageSwingCfgSet(dev, fport, laneNum, &txVltgSwingCfg);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 0;

        /*
            1.4. Call with laneNum [6] (out of range).
            Expected: NOT GT_OK.
        */
        laneNum = 6;

        st = cpssExMxPmSerdesTxVoltageSwingCfgSet(dev, fport, laneNum, &txVltgSwingCfg);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, laneNum);

        laneNum = 0;

        /*
            1.5. Call with txVltgSwingCfg->emphasisType [0x5AAAAAA5] (out of range).
            Expected: GT_BAD_PARAM.
        */
        txVltgSwingCfg.emphasisType = FABRIC_SERDES_INVALID_ENUM_CNS;

        st = cpssExMxPmSerdesTxVoltageSwingCfgSet(dev, fport, laneNum, &txVltgSwingCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, txVltgSwingCfg.emphasisType = %d", dev, txVltgSwingCfg.emphasisType);

        txVltgSwingCfg.emphasisType = CPSS_EXMXPM_FABRIC_SERDES_EMPHASIS_TYPE_DE_EMPHASIS_E;

        /*
            1.6. Call with txVltgSwingCfgPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSerdesTxVoltageSwingCfgSet(dev, fport, laneNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, txVltgSwingCfg = NULL", dev);
    }

    fport = 0;
    laneNum = 0;
    txVltgSwingCfg.txAmp = 0;
    txVltgSwingCfg.emphasisType = CPSS_EXMXPM_FABRIC_SERDES_EMPHASIS_TYPE_DE_EMPHASIS_E;
    txVltgSwingCfg.txEmphasisEnable = GT_TRUE;
    txVltgSwingCfg.emphasisLevelAdjustEnable = GT_TRUE;
    txVltgSwingCfg.txEmphasisAmp = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSerdesTxVoltageSwingCfgSet(dev, fport, laneNum, &txVltgSwingCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSerdesTxVoltageSwingCfgSet(dev, fport, laneNum, &txVltgSwingCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSerdesTxVoltageSwingCfgGet
(
    IN  GT_U8     devNum,
    IN  GT_FPORT  fport,
    IN  GT_U32    laneNum,
    OUT  CPSS_EXMXPM_FABRIC_SERDES_TX_VLTG_SWING_CFG_STC   *txVltgSwingCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSerdesTxVoltageSwingCfgGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 3],
                   laneNum [0 / 5],
                   not NULL txVltgSwingCfg.
    Expected: GT_OK.
    1.2. Call with fport [4] (out of range).
    Expected: NOT GT_OK.
    1.3. Call with laneNum [6] (out of range).
    Expected: NOT GT_OK.
    1.4. Call with txVltgSwingCfgPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;


    GT_FPORT  fport = 0;
    GT_U32    laneNum = 0;
    CPSS_EXMXPM_FABRIC_SERDES_TX_VLTG_SWING_CFG_STC txVltgSwingCfg;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3],
                           laneNum [0 / 5],
                           not NULL txVltgSwingCfg.
            Expected: GT_OK.
        */
        /* iterate with fport = 0 */
        fport = 0;
        laneNum = 0;

        st = cpssExMxPmSerdesTxVoltageSwingCfgGet(dev, fport, laneNum, &txVltgSwingCfg);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, laneNum);

        /* iterate with fport = 3 */
        fport = 3;
        laneNum = 5;

        st = cpssExMxPmSerdesTxVoltageSwingCfgGet(dev, fport, laneNum, &txVltgSwingCfg);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, laneNum);

        /*
            1.2. Call with fport [4] (out of range).
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmSerdesTxVoltageSwingCfgGet(dev, fport, laneNum, &txVltgSwingCfg);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 0;

        /*
            1.3. Call with laneNum [6] (out of range).
            Expected: NOT GT_OK.
        */
        laneNum = 6;

        st = cpssExMxPmSerdesTxVoltageSwingCfgGet(dev, fport, laneNum, &txVltgSwingCfg);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, laneNum);

        laneNum = 0;

        /*
            1.4. Call with txVltgSwingCfgPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSerdesTxVoltageSwingCfgGet(dev, fport, laneNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, txVltgSwingCfg = NULL", dev);
    }

    fport = 0;
    laneNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSerdesTxVoltageSwingCfgGet(dev, fport, laneNum, &txVltgSwingCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSerdesTxVoltageSwingCfgGet(dev, fport, laneNum, &txVltgSwingCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSerdesTxSwpSelSet
(
    IN  GT_U8       devNum,
    IN  GT_FPORT    fport,
    IN  GT_U32      pcsTxLaneArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSerdesTxSwpSelSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 3],
                   pcsTxLaneArr [0..0 / 5..5].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSerdesTxSwpSelGet with not NULL pcsTxLaneArr and fport from 1.1.
    Expected: GT_OK and the same pcsTxLaneArr as was set.
    1.3. Call with fport [4] (out of range).
    Expected: NOT GT_OK.
    1.4. Call with pcsTxLaneArr [0] = 6 (out of range).
    Expected: NOT GT_OK.
    1.5. Call with pcsTxLaneArr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U32      iTemp;
    GT_BOOL     isEqual = GT_FALSE;

    GT_FPORT    fport = 0;
    GT_U32      pcsTxLaneArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];
    GT_U32      pcsTxLaneArrGet[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3],
                           pcsTxLaneArr [0..0 / 5..5].
            Expected: GT_OK.
        */
        /* iterate with fport = 0 */
        fport = 0;
        for(iTemp=0; iTemp<CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS; ++iTemp)
        {
            pcsTxLaneArr[iTemp] = 0;
        }

        st = cpssExMxPmSerdesTxSwpSelSet(dev, fport, pcsTxLaneArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmSerdesTxSwpSelGet with not NULL pcsTxLaneArr and fport from 1.1.
            Expected: GT_OK and the same pcsTxLaneArr as was set.
        */
        st = cpssExMxPmSerdesTxSwpSelGet(dev, fport, pcsTxLaneArrGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSerdesTxSwpSelGet: %d, %d", dev, fport);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pcsTxLaneArr,
                                     (GT_VOID*) &pcsTxLaneArrGet,
                                     sizeof(pcsTxLaneArr))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another pcsTxLaneArr than was set: %d", dev);

        /* iterate with fport = 3 */
        fport = 3;
        for(iTemp=0; iTemp<CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS; ++iTemp)
        {
            pcsTxLaneArr[iTemp] = 5;
        }

        st = cpssExMxPmSerdesTxSwpSelSet(dev, fport, pcsTxLaneArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmSerdesTxSwpSelGet with not NULL pcsTxLaneArr and fport from 1.1.
            Expected: GT_OK and the same pcsTxLaneArr as was set.
        */
        st = cpssExMxPmSerdesTxSwpSelGet(dev, fport, pcsTxLaneArrGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSerdesTxSwpSelGet: %d, %d", dev, fport);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pcsTxLaneArr,
                                     (GT_VOID*) &pcsTxLaneArrGet,
                                     sizeof(pcsTxLaneArr))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another pcsTxLaneArr than was set: %d", dev);


        /*
            1.3. Call with fport [4] (out of range).
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmSerdesTxSwpSelSet(dev, fport, pcsTxLaneArr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 0;

        /*
            1.4. Call with pcsTxLaneArr [0] = 6 (out of range).
            Expected: NOT GT_OK.
        */
        pcsTxLaneArr[0] = 6;

        st = cpssExMxPmSerdesTxSwpSelSet(dev, fport, pcsTxLaneArr);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, pcsTxLaneArr[0] = %d", dev, fport, pcsTxLaneArr[0]);

        pcsTxLaneArr[0] = 0;

        /*
            1.5. Call with pcsTxLaneArr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSerdesTxSwpSelSet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pcsTxLaneArr = NULL", dev);
    }

    fport = 0;
    for(iTemp=0; iTemp<CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS; ++iTemp)
    {
        pcsTxLaneArr[iTemp] = 0;
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSerdesTxSwpSelSet(dev, fport, pcsTxLaneArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSerdesTxSwpSelSet(dev, fport, pcsTxLaneArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSerdesTxSwpSelGet
(
    IN  GT_U8       devNum,
    IN  GT_FPORT    fport,
    OUT GT_U32      pcsTxLaneArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSerdesTxSwpSelGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 3],
                   not NULL pcsTxLaneArr.
    Expected: GT_OK.
    1.2. Call with fport [4] (out of range).
    Expected: NOT GT_OK.
    1.3. Call with pcsTxLaneArr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT    fport = 0;
    GT_U32      pcsTxLaneArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3],
                           not NULL pcsTxLaneArr.
            Expected: GT_OK.
        */
        /* iterate with fport = 0 */
        fport = 0;

        st = cpssExMxPmSerdesTxSwpSelGet(dev, fport, pcsTxLaneArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /* iterate with fport = 3 */
        fport = 3;

        st = cpssExMxPmSerdesTxSwpSelGet(dev, fport, pcsTxLaneArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call with fport [4] (out of range).
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmSerdesTxSwpSelGet(dev, fport, pcsTxLaneArr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 0;

        /*
            1.3. Call with pcsTxLaneArr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSerdesTxSwpSelGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pcsTxLaneArr = NULL", dev);
    }

    fport = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSerdesTxSwpSelGet(dev, fport, pcsTxLaneArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSerdesTxSwpSelGet(dev, fport, pcsTxLaneArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSerdesSpeedSet
(
    IN  GT_U8       devNum,
    IN  GT_FPORT    fport,
    IN  CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_TYPE_ENT refClk,
    IN  CPSS_EXMXPM_FABRIC_SERDES_SPEED_TYPE_ENT speed
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSerdesSpeedSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 3], refClk [CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_133M_E /
                                          CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_156M_E]
                   and speed [CPSS_EXMXPM_FABRIC_SERDES_SPEED_3125M_E /
                              CPSS_EXMXPM_FABRIC_SERDES_SPEED_3333M_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSerdesSpeedGet with not NULL speedPtr and other params fport from 1.1.
    Expected: GT_OK and the same speed as was set.
    1.3. Call with fport [4] (out of range).
    Expected: NOT GT_OK.
    1.4. Call with refClk [FABRIC_SERDES_INVALID_ENUM_CNS] (out of range).
    Expected: GT_BAD_PARAM.
    1.5. Call with speed [FABRIC_SERDES_INVALID_ENUM_CNS] (out of range).
    Expected: GT_BAD_PARAM.
*/

    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                           fport    = 0;
    CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_TYPE_ENT refClk   = CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_133M_E;
    CPSS_EXMXPM_FABRIC_SERDES_SPEED_TYPE_ENT           speed    = CPSS_EXMXPM_FABRIC_SERDES_SPEED_3125M_E;
    CPSS_EXMXPM_FABRIC_SERDES_SPEED_TYPE_ENT           speedGet = CPSS_EXMXPM_FABRIC_SERDES_SPEED_3125M_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /*-----------------
            1.1. Call with fport [0 / 3], refClk [CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_133M_E /
                                                  CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_156M_E]
                           and speed [CPSS_EXMXPM_FABRIC_SERDES_SPEED_3125M_E /
                                      CPSS_EXMXPM_FABRIC_SERDES_SPEED_3333M_E].
            Expected: GT_OK.
        */
            st = cpssExMxPmSerdesSpeedSet(dev, fport, refClk, speed);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmSerdesSpeedGet with not NULL speedPtr and other params fport from 1.1.
            Expected: GT_OK and the same speed as was set.
        */
            st = cpssExMxPmSerdesSpeedGet(dev, fport, refClk, &speedGet);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
            UTF_VERIFY_EQUAL2_STRING_MAC(speed, speedGet,
                                 "got another value than was set: %d, %d", speed, speedGet);


        /*-----------------
            1.1. Call with fport [0 / 3], refClk [CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_133M_E /
                                                  CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_156M_E]
                           and speed [CPSS_EXMXPM_FABRIC_SERDES_SPEED_3125M_E /
                                      CPSS_EXMXPM_FABRIC_SERDES_SPEED_3333M_E].
            Expected: GT_OK.
        */

            fport    = 3;
            refClk   = CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_156M_E;
            speed    = CPSS_EXMXPM_FABRIC_SERDES_SPEED_3333M_E;

            st = cpssExMxPmSerdesSpeedSet(dev, fport, refClk, speed);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmSerdesSpeedGet with not NULL speedPtr and other params fport from 1.1.
            Expected: GT_OK and the same speed as was set.
        */
            st = cpssExMxPmSerdesSpeedGet(dev, fport, refClk, &speedGet);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
            UTF_VERIFY_EQUAL2_STRING_MAC(speed, speedGet,
                             "got another value than was set: %d, %d", speed, speedGet);

        /*----------------
            1.1. Call with fport [0 / 3], refClk [CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_133M_E /
                                                  CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_156M_E]
                           and speed [CPSS_EXMXPM_FABRIC_SERDES_SPEED_3125M_E /
                                      CPSS_EXMXPM_FABRIC_SERDES_SPEED_3333M_E].
            Expected: GT_OK.
        */
            fport    = 1;
            refClk   = CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_156M_E;
            speed    = CPSS_EXMXPM_FABRIC_SERDES_SPEED_3125M_E;

            st = cpssExMxPmSerdesSpeedSet(dev, fport, refClk, speed);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmSerdesSpeedGet with not NULL speedPtr and other params fport from 1.1.
            Expected: GT_OK and the same speed as was set.
        */
            st = cpssExMxPmSerdesSpeedGet(dev, fport, refClk, &speedGet);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
            UTF_VERIFY_EQUAL2_STRING_MAC(speed, speedGet,
                                 "got another value than was set: %d, %d", speed, speedGet);


        /*---------------
            1.1. Call with fport [0 / 3], refClk [CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_133M_E /
                                                  CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_156M_E]
                           and speed [CPSS_EXMXPM_FABRIC_SERDES_SPEED_3125M_E /
                                      CPSS_EXMXPM_FABRIC_SERDES_SPEED_3333M_E].
            Expected: GT_OK.
        */
            fport    = 2;
            refClk   = CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_133M_E;
            speed    = CPSS_EXMXPM_FABRIC_SERDES_SPEED_3333M_E;

            st = cpssExMxPmSerdesSpeedSet(dev, fport, refClk, speed);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmSerdesSpeedGet with not NULL speedPtr and other params fport from 1.1.
            Expected: GT_OK and the same speed as was set.
        */
            st = cpssExMxPmSerdesSpeedGet(dev, fport, refClk, &speedGet);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
            UTF_VERIFY_EQUAL2_STRING_MAC(speed, speedGet,
                                 "got another value than was set: %d, %d", speed, speedGet);

        /*----------------
            1.3. Call with fport [4] (out of range).
            Expected: NOT GT_OK.
        */
            fport = 4;

            st = cpssExMxPmSerdesSpeedSet(dev, fport, refClk, speed);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            fport = 0;
        /*----------------
            1.4. Call with refClk [FABRIC_SERDES_INVALID_ENUM_CNS] (out of range).
            Expected: GT_BAD_PARAM.
        */
            refClk = FABRIC_SERDES_INVALID_ENUM_CNS;

            st = cpssExMxPmSerdesSpeedSet(dev, fport, refClk, speed);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

            refClk = CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_133M_E;
        /*----------------
            1.5. Call with speed [FABRIC_SERDES_INVALID_ENUM_CNS] (out of range).
            Expected: GT_BAD_PARAM.
        */
            speed = FABRIC_SERDES_INVALID_ENUM_CNS;

            st = cpssExMxPmSerdesSpeedSet(dev, fport, refClk, speed);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

            speed = CPSS_EXMXPM_FABRIC_SERDES_SPEED_3125M_E;

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSerdesSpeedSet(dev, fport, refClk, speed);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSerdesSpeedSet(dev, fport, refClk, speed);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSerdesSpeedGet
(
    IN  GT_U8       devNum,
    IN  GT_FPORT    fport,
    IN  CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_TYPE_ENT refClk,
    OUT CPSS_EXMXPM_FABRIC_SERDES_SPEED_TYPE_ENT *speedPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSerdesSpeedGet)
{
    /*
        ITERATE_DEVICES (ExMxPm)
        1.1. Call with fport [0 / 3], refClk [CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_133M_E /
                                              CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_156M_E].
        Expected: GT_OK.
        1.2. Call with fport [4] (out of range).
        Expected: NOT GT_OK.
        1.3. Call with refClk [FABRIC_SERDES_INVALID_ENUM_CNS] (out of range).
        Expected: GT_BAD_PARAM.
        1.4. Call with NULL speedPtr.
        Expected: GT_BAD_PTR.
    */

        GT_STATUS   st = GT_OK;
        GT_U8       dev;

        GT_FPORT                                           fport    = 0;
        CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_TYPE_ENT refClk   = CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_133M_E;
        CPSS_EXMXPM_FABRIC_SERDES_SPEED_TYPE_ENT           speed    = CPSS_EXMXPM_FABRIC_SERDES_SPEED_3125M_E;


        /* prepare device iterator */
        st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1. Go over all active devices. */
        while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
        {

        /*-----------------
            1.1. Call with fport [0 / 3], refClk [CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_133M_E /
                                                  CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_156M_E].
            Expected: GT_OK.
        */
            st = cpssExMxPmSerdesSpeedGet(dev, fport, refClk, &speed);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*-----------------
            1.1. Call with fport [0 / 3], refClk [CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_133M_E /
                                                  CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_156M_E].
            Expected: GT_OK.
        */
            fport    = 3;
            refClk   = CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_156M_E;

            st = cpssExMxPmSerdesSpeedGet(dev, fport, refClk, &speed);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*----------------
            1.1. Call with fport [0 / 3], refClk [CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_133M_E /
                                                  CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_156M_E].
            Expected: GT_OK.
        */

            fport    = 1;
            refClk   = CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_156M_E;
            speed    = CPSS_EXMXPM_FABRIC_SERDES_SPEED_3125M_E;

            st = cpssExMxPmSerdesSpeedGet(dev, fport, refClk, &speed);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*---------------
            1.1. Call with fport [0 / 3], refClk [CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_133M_E /
                                                  CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_156M_E].
            Expected: GT_OK.
        */
            fport    = 2;
            refClk   = CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_133M_E;
            speed    = CPSS_EXMXPM_FABRIC_SERDES_SPEED_3333M_E;

            st = cpssExMxPmSerdesSpeedGet(dev, fport, refClk, &speed);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*----------------
            1.2. Call with fport [4] (out of range).
            Expected: NOT GT_OK.
        */
            fport = 4;

            st = cpssExMxPmSerdesSpeedGet(dev, fport, refClk, &speed);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            fport = 0;
        /*----------------
            1.4. Call with refClk [FABRIC_SERDES_INVALID_ENUM_CNS] (out of range).
            Expected: GT_BAD_PARAM.
        */
            refClk = FABRIC_SERDES_INVALID_ENUM_CNS;

            st = cpssExMxPmSerdesSpeedGet(dev, fport, refClk, &speed);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

            refClk = CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_133M_E;
        /*----------------
            1.5. Call with speed [NULL] (Null pointer).
            Expected: GT_BAD_PTR.
        */
            st = cpssExMxPmSerdesSpeedGet(dev, fport, refClk, &speed);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        }

        /* 2. For not-active devices and devices from non-applicable family */
        /* check that function returns GT_BAD_PARAM.                        */

        /* prepare device iterator */
        st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* go over all non active devices */
        while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
        {
            st = cpssExMxPmSerdesSpeedGet(dev, fport, refClk, &speed);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }

        /* 3. Call function with out of bound value for device id.*/
        dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmSerdesSpeedGet(dev, fport, refClk, &speed);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmFabricSerdes suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmFabricSerdes)

    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSerdesStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSerdesResetSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSerdesResetGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSerdesPowerSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSerdesPowerGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSerdesLoopbackEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSerdesLoopbackEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSerdesTxVoltageSwingCfgSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSerdesTxVoltageSwingCfgGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSerdesTxSwpSelSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSerdesTxSwpSelGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSerdesSpeedSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSerdesSpeedGet)

UTF_SUIT_END_TESTS_MAC(cpssExMxPmFabricSerdes)

