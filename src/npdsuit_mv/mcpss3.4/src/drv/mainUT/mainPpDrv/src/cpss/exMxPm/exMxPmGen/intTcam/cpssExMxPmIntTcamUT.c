/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmIntTcamUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmIntTcam, that provides
*       CPSS ExMxPm internal TCAM APIs.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/intTcam/cpssExMxPmIntTcam.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define INT_TCAM_INVALID_ENUM_CNS    0x5AAAAAA5

#define INT_TCAM_RULE_MAX_START_CNS  2048 * 4


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIntTcamWriteModeSet
(
    IN  GT_U8                                devNum,
    IN  CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_ENT writeDataFormat
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIntTcamWriteModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with writeDataFormat [CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E /
                                    CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIntTcamWriteModeGet with non-NULL writeDataFormatPtr.
    Expected: GT_OK and the same writeDataFormat.
    1.3. Call with out of range writeDataFormat [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;


    CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_ENT  writeDataFormat    = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E;
    CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_ENT  writeDataFormatGet = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with writeDataFormat [CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E /
                                            CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E].
            Expected: GT_OK.
        */

        /* Call with writeDataFormat [CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E] */
        writeDataFormat = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E;

        st = cpssExMxPmIntTcamWriteModeSet(dev, writeDataFormat);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, writeDataFormat);

        /*
            1.2. Call cpssExMxPmIntTcamWriteModeGet with non-NULL writeDataFormatPtr.
            Expected: GT_OK and the same writeDataFormat.
        */
        st = cpssExMxPmIntTcamWriteModeGet(dev, &writeDataFormatGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIntTcamWriteModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(writeDataFormat, writeDataFormatGet,
                       "get another writeDataFormat than was set: %d", dev);

        /* Call with writeDataFormat [CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E] */
        writeDataFormat = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E;

        st = cpssExMxPmIntTcamWriteModeSet(dev, writeDataFormat);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, writeDataFormat);

        /*
            1.2. Call cpssExMxPmIntTcamWriteModeGet with non-NULL writeDataFormatPtr.
            Expected: GT_OK and the same writeDataFormat.
        */
        st = cpssExMxPmIntTcamWriteModeGet(dev, &writeDataFormatGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIntTcamWriteModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(writeDataFormat, writeDataFormatGet,
                       "get another writeDataFormat than was set: %d", dev);

        /*
            1.3. Call with out of range writeDataFormat [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        writeDataFormat = INT_TCAM_INVALID_ENUM_CNS;

        st = cpssExMxPmIntTcamWriteModeSet(dev, writeDataFormat);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, writeDataFormat);
    }

    writeDataFormat = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIntTcamWriteModeSet(dev, writeDataFormat);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIntTcamWriteModeSet(dev, writeDataFormat);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIntTcamWriteModeGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_ENT *writeDataFormatPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIntTcamWriteModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null writeDataFormatPtr.
    Expected: GT_OK.
    1.2. Call with writeDataFormatPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_ENT  writeDataFormat = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null writeDataFormatPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmIntTcamWriteModeGet(dev, &writeDataFormat);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with writeDataFormatPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIntTcamWriteModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, writeDataFormatPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIntTcamWriteModeGet(dev, &writeDataFormat);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIntTcamWriteModeGet(dev, &writeDataFormat);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIntTcamRuleValidStatusSet
(
    IN GT_U8                              devNum,
    IN GT_U32                             ruleStartIndex,
    IN GT_BOOL                            valid
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIntTcamRuleValidStatusSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with ruleStartIndex [0 / 2048*4 - 1]
                   valid [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIntTcamRuleStateGet with the same ruleStartIndex
                                            and non-NULL sizePtr and validPtr.
    Expected: GT_OK and the same valid.
    1.3. Call with out of range ruleStartIndex [2048*4]
                   and other valid parameters.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      ruleStartIndex = 0;
    GT_BOOL     valid          = GT_FALSE;
    GT_BOOL     validGet       = GT_FALSE;
    CPSS_EXMXPM_INT_TCAM_RULE_SIZE_ENT size = CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E;
    GT_U32      ruleDataPattern[6]    = {0,0,0,0,0,0};
    GT_U32      ruleDataMask[6]       = {0,0,0,0,0,0};


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* write rule with ruleStartIndex 0 and                               */
        /* (INT_TCAM_RULE_MAX_START_CNS - 1)                                   */
        /* to cause cpssExMxPmIntTcamRuleStateGet to return GT_OK both for    */
        /* valid and invalid rules                                            */

        st = cpssExMxPmIntTcamRuleWrite(
            dev, 0 /*ruleStartIndex*/, GT_FALSE /*valid*/,
            CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E,
            192 /*dataSize*/,
            CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E,
            ruleDataPattern, ruleDataMask);
        if (st != GT_OK)
        {
            PRV_UTF_LOG0_MAC(
                "cpssExMxPmIntTcamRuleValidStatusSet.cpssExMxPmIntTcamRuleWrite failed\n");
        }

        st = cpssExMxPmIntTcamRuleWrite(
            dev, (INT_TCAM_RULE_MAX_START_CNS - 1) /*ruleStartIndex*/,
            GT_FALSE /*valid*/,
            CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E,
            192 /*dataSize*/,
            CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E,
            ruleDataPattern, ruleDataMask);
        if (st != GT_OK)
        {
            PRV_UTF_LOG0_MAC(
                "cpssExMxPmIntTcamRuleValidStatusSet.cpssExMxPmIntTcamRuleWrite failed\n");
        }


        /*
            1.1. Call with ruleStartIndex [0 / 2048*4 - 1]
                           valid [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with ruleStartIndex [0] */
        ruleStartIndex = 0;
        valid          = GT_FALSE;

        st = cpssExMxPmIntTcamRuleValidStatusSet(dev, ruleStartIndex, valid);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleStartIndex, valid);

        /*
            1.2. Call cpssExMxPmIntTcamRuleStateGet with the same ruleStartIndex
                                                    and non-NULL sizePtr and validPtr.
            Expected: GT_OK and the same valid.
        */
        st = cpssExMxPmIntTcamRuleStateGet(dev, ruleStartIndex, &size, &validGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIntTcamRuleStateGet: %d, %d", dev, ruleStartIndex);

        if (GT_OK == st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(valid, validGet,
                       "get another valid than was set: %d", dev);
        }

        /* Call with ruleStartIndex [2048*4 - 1] */
        ruleStartIndex = INT_TCAM_RULE_MAX_START_CNS - 1;
        valid          = GT_TRUE;

        st = cpssExMxPmIntTcamRuleValidStatusSet(dev, ruleStartIndex, valid);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleStartIndex, valid);

        /*
            1.2. Call cpssExMxPmIntTcamRuleStateGet with the same ruleStartIndex
                                                    and non-NULL sizePtr and validPtr.
            Expected: GT_OK and the same valid.
        */
        st = cpssExMxPmIntTcamRuleStateGet(dev, ruleStartIndex, &size, &validGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIntTcamRuleStateGet: %d, %d", dev, ruleStartIndex);

        if (GT_OK == st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(valid, validGet,
                       "get another valid than was set: %d", dev);
        }

        /*
            1.3. Call with out of range ruleStartIndex [2048*4]
                           and other valid parameters.
            Expected: NOT GT_OK.
        */
        ruleStartIndex = INT_TCAM_RULE_MAX_START_CNS;

        st = cpssExMxPmIntTcamRuleValidStatusSet(dev, ruleStartIndex, valid);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleStartIndex);
    }

    ruleStartIndex = 0;
    valid          = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIntTcamRuleValidStatusSet(dev, ruleStartIndex, valid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIntTcamRuleValidStatusSet(dev, ruleStartIndex, valid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIntTcamRuleStateGet
(
    IN  GT_U8                              devNum,
    IN  GT_U32                             ruleStartIndex,
    OUT CPSS_EXMXPM_INT_TCAM_RULE_SIZE_ENT *sizePtr,
    OUT GT_BOOL                            *validPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIntTcamRuleStateGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with ruleStartIndex [0 / 2048*4 - 1]
                   non NULL sizePtr, non NULL validPtr.
    Expected: GT_OK.
    1.2. Call with out of range ruleStartIndex [2048*4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with sizePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with validPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      ruleStartIndex = 0;
    GT_BOOL     valid          = GT_FALSE;
    CPSS_EXMXPM_INT_TCAM_RULE_SIZE_ENT size = CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E;
    GT_U32      ruleDataPattern[6]    = {0,0,0,0,0,0};
    GT_U32      ruleDataMask[6]       = {0,0,0,0,0,0};


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ruleStartIndex [0 / 2048*4 - 1]
                           non NULL sizePtr, non NULL validPtr.
            Expected: GT_OK.
        */

        /* write rule with ruleStartIndex 0 and                               */
        /* (INT_TCAM_RULE_MAX_START_CNS - 1)                                   */
        /* to cause cpssExMxPmIntTcamRuleStateGet to return GT_OK both for    */
        /* valid and invalid rules                                            */

        st = cpssExMxPmIntTcamRuleWrite(
            dev, 0 /*ruleStartIndex*/, GT_FALSE /*valid*/,
            CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E,
            192 /*dataSize*/,
            CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E,
            ruleDataPattern, ruleDataMask);
        if (st != GT_OK)
        {
            PRV_UTF_LOG0_MAC(
                "cpssExMxPmIntTcamRuleValidStatusSet.cpssExMxPmIntTcamRuleWrite failed\n");
        }

        st = cpssExMxPmIntTcamRuleWrite(
            dev, (INT_TCAM_RULE_MAX_START_CNS - 1) /*ruleStartIndex*/,
            GT_FALSE /*valid*/,
            CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E,
            192 /*dataSize*/,
            CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E,
            ruleDataPattern, ruleDataMask);
        if (st != GT_OK)
        {
            PRV_UTF_LOG0_MAC(
                "cpssExMxPmIntTcamRuleValidStatusSet.cpssExMxPmIntTcamRuleWrite failed\n");
        }

        /* Call with ruleStartIndex [0] */
        ruleStartIndex = 0;

        st = cpssExMxPmIntTcamRuleStateGet(dev, ruleStartIndex, &size, &valid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleStartIndex);

        /* Call with ruleStartIndex [2048*4 - 1] */
        ruleStartIndex = INT_TCAM_RULE_MAX_START_CNS - 1;

        st = cpssExMxPmIntTcamRuleStateGet(dev, ruleStartIndex, &size, &valid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleStartIndex);

        /*
            1.2. Call with out of range ruleStartIndex [2048*4]
                           and other valid parameters.
            Expected: NOT GT_OK.
        */
        ruleStartIndex = INT_TCAM_RULE_MAX_START_CNS;

        st = cpssExMxPmIntTcamRuleStateGet(dev, ruleStartIndex, &size, &valid);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleStartIndex);

        ruleStartIndex = 0;

        /*
            1.3. Call with sizePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIntTcamRuleStateGet(dev, ruleStartIndex, NULL, &valid);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, sizePtr = NULL", dev);

        /*
            1.4. Call with validPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIntTcamRuleStateGet(dev, ruleStartIndex, &size, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validPtr = NULL", dev);

    }

    ruleStartIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIntTcamRuleStateGet(dev, ruleStartIndex, &size, &valid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIntTcamRuleStateGet(dev, ruleStartIndex, &size, &valid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIntTcamRuleWrite
(
    IN GT_U8                                 devNum,
    IN GT_U32                                ruleStartIndex,
    IN GT_BOOL                               valid,
    IN CPSS_EXMXPM_INT_TCAM_RULE_SIZE_ENT    ruleSize,
    IN GT_U32                                dataSize,
    IN CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_ENT  dataFormat,
    IN GT_U32                                *rawPatternOrXPtr,
    IN GT_U32                                *rawMaskOrYPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIntTcamRuleWrite)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with ruleStartIndex [0 / 2048*4 - 1],
                   valid [GT_FALSE / GT_TRUE],
                   ruleSize [CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E /
                             CPSS_EXMXPM_INT_TCAM_RULE_SIZE_EXTENDED_E],
                   dataSize [0 / 5],
                   dataFormat [CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E /
                               CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E],
                   rawPatternOrXPtr [0] / [1],
                   rawMaskOrYPtr [0] / [1].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIntTcamRuleRead with the same ruleStartIndex,
                                                      dataFormat,
                                                      ruleSizePtr,
                                                      dataSizePtr
                                            and non-NULL validPtr, rawPatternOrXPtr and rawMaskOrYPtr.
    Expected: GT_OK and the same valid, rawPatternOrXPtr, rawMaskOrYPtr.
    1.3. Call with out of range ruleStartIndex [2048*4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range ruleSize [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range dataFormat [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with rawPatternOrXPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.7. Call with rawMaskOrYPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                ruleStartIndex = 0;
    GT_BOOL                               valid          = GT_FALSE;
    GT_BOOL                               validGet       = GT_FALSE;
    GT_BOOL                               isEqual        = GT_FALSE;
    CPSS_EXMXPM_INT_TCAM_RULE_SIZE_ENT    ruleSize       = CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E;
    CPSS_EXMXPM_INT_TCAM_RULE_SIZE_ENT    ruleSizeGet    = CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E;
    GT_U32                                dataSize       = 0;
    GT_U32                                dataSizeGet    = 0;
    CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_ENT  dataFormat     = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E;
    GT_U32                                rawPatternOrXPtr[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    GT_U32                                rawMaskOrYPtrPtr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    GT_U32                                rawPatternOrXGet[6] = {0x00, 0x00, 0x00, 0x00, 0x0, 0x0};
    GT_U32                                rawMaskOrYPtrGet[6] = {0x00, 0x00, 0x00, 0x00, 0x0, 0x0};


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ruleStartIndex [0 / 2048*4 - 1],
                           valid [GT_FALSE / GT_TRUE],
                           ruleSize [CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E /
                                     CPSS_EXMXPM_INT_TCAM_RULE_SIZE_EXTENDED_E],
                           dataSize [0 / 5],
                           dataFormat [CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E /
                                       CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E],
                           rawPatternOrXPtr [0] / [1],
                           rawMaskOrYPtr [0] / [1].
            Expected: GT_OK.
        */

        /* Call with ruleStartIndex [0] */
        ruleStartIndex = 0;
        valid          = GT_FALSE;
        ruleSize       = CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E;
        dataSize       = 192 /*0*/;
        dataFormat     = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E;

        st = cpssExMxPmIntTcamRuleWrite(dev, ruleStartIndex, valid, ruleSize, dataSize, dataFormat,
                                        (GT_U32*) rawPatternOrXPtr, (GT_U32*) rawMaskOrYPtrPtr);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, ruleStartIndex, valid, ruleSize, dataSize, dataFormat);

        /*
            1.2. Call cpssExMxPmIntTcamRuleRead with the same ruleStartIndex,
                                                              dataFormat,
                                                              ruleSizePtr,
                                                              dataSizePtr
                                                    and non-NULL validPtr, rawPatternOrXPtr and rawMaskOrYPtr.
            Expected: GT_OK and the same valid, rawPatternOrXPtr, rawMaskOrYPtr.
        */
        ruleSizeGet = ruleSize;
        dataSizeGet = dataSize;

        st = cpssExMxPmIntTcamRuleRead(dev, ruleStartIndex, dataFormat, &ruleSizeGet,
                                       &dataSizeGet, &validGet, (GT_U32*) rawPatternOrXGet, (GT_U32*) rawMaskOrYPtrGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIntTcamRuleRead: %d, %d, %d", dev, ruleStartIndex, dataFormat);

        if (GT_OK == st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(valid, validGet,
                       "get another valid than was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) rawPatternOrXPtr,
                                         (GT_VOID*) rawPatternOrXGet,
                                         sizeof(rawPatternOrXPtr[0]) * 5)) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another rawPatternOrXPtr than was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) rawMaskOrYPtrPtr,
                                         (GT_VOID*) rawMaskOrYPtrGet,
                                         sizeof(rawMaskOrYPtrPtr[0]) * 5)) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another rawMaskOrYPtr than was set: %d", dev);
        }

        /* Call with ruleStartIndex [2048*4 - 1] */
        ruleStartIndex = 4096;
        valid          = GT_TRUE;
        ruleSize       = CPSS_EXMXPM_INT_TCAM_RULE_SIZE_EXTENDED_E;
        dataSize       = 5;
        dataFormat     = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E;

        st = cpssExMxPmIntTcamRuleWrite(dev, ruleStartIndex, valid, ruleSize, dataSize, dataFormat,
                                        (GT_U32*) rawPatternOrXPtr, (GT_U32*) rawMaskOrYPtrPtr);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, ruleStartIndex, valid, ruleSize, dataSize, dataFormat);

        /*
            1.2. Call cpssExMxPmIntTcamRuleRead with the same ruleStartIndex,
                                                              dataFormat,
                                                              ruleSizePtr,
                                                              dataSizePtr
                                                    and non-NULL validPtr, rawPatternOrXPtr and rawMaskOrYPtr.
            Expected: GT_OK and the same valid, rawPatternOrXPtr, rawMaskOrYPtr.
        */
        ruleSizeGet = ruleSize;
        dataSizeGet = dataSize;

        st = cpssExMxPmIntTcamRuleRead(dev, ruleStartIndex, dataFormat, &ruleSizeGet,
                                       &dataSizeGet, &validGet, (GT_U32*) rawPatternOrXGet, (GT_U32*) rawMaskOrYPtrGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIntTcamRuleRead: %d, %d, %d", dev, ruleStartIndex, dataFormat);

        if (GT_OK == st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(valid, validGet,
                       "get another valid than was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) rawPatternOrXPtr,
                                         (GT_VOID*) rawPatternOrXGet,
                                         sizeof(rawPatternOrXPtr[0]) * 5)) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another rawPatternOrXPtr than was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) rawMaskOrYPtrPtr,
                                         (GT_VOID*) rawMaskOrYPtrGet,
                                         sizeof(rawMaskOrYPtrPtr[0]) * 5)) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another rawMaskOrYPtr than was set: %d", dev);
        }

        /*
            1.3. Call with out of range ruleStartIndex [2048*4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ruleStartIndex = INT_TCAM_RULE_MAX_START_CNS;

        st = cpssExMxPmIntTcamRuleWrite(dev, ruleStartIndex, valid, ruleSize, dataSize, dataFormat,
                                        (GT_U32*) rawPatternOrXPtr, (GT_U32*) rawMaskOrYPtrPtr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleStartIndex);

        ruleStartIndex = 1;


        /*
            1.4. Call with out of range ruleSize [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ruleSize = INT_TCAM_INVALID_ENUM_CNS;

        st = cpssExMxPmIntTcamRuleWrite(dev, ruleStartIndex, valid, ruleSize, dataSize, dataFormat,
                                        (GT_U32*) rawPatternOrXPtr, (GT_U32*) rawMaskOrYPtrPtr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,"%d, ruleSize = %d", dev, ruleSize);

        ruleSize = CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E;

        /*
            1.5. Call with out of range dataFormat [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        dataFormat = INT_TCAM_INVALID_ENUM_CNS;

        st = cpssExMxPmIntTcamRuleWrite(dev, ruleStartIndex, valid, ruleSize, dataSize, dataFormat,
                                        (GT_U32*) rawPatternOrXPtr, (GT_U32*) rawMaskOrYPtrPtr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,"%d, dataFormat = %d", dev, dataFormat);

        dataFormat = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E;

        /*
            1.6. Call with rawPatternOrXPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIntTcamRuleWrite(dev, ruleStartIndex, valid, ruleSize, dataSize, dataFormat,
                                        NULL, (GT_U32*) rawMaskOrYPtrPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,"%d, rawPatternOrXPtr = NULL", dev);

        /*
            1.7. Call with rawMaskOrYPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIntTcamRuleWrite(dev, ruleStartIndex, valid, ruleSize, dataSize, dataFormat,
                                        (GT_U32*) rawPatternOrXPtr, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,"%d, rawMaskOrYPtr = NULL", dev);
    }

    ruleStartIndex = 0;
    valid          = GT_FALSE;
    ruleSize       = CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E;
    dataSize       = 0;
    dataFormat     = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIntTcamRuleWrite(dev, ruleStartIndex, valid, ruleSize, dataSize, dataFormat,
                                        (GT_U32*) rawPatternOrXPtr, (GT_U32*) rawMaskOrYPtrPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIntTcamRuleWrite(dev, ruleStartIndex, valid, ruleSize, dataSize, dataFormat,
                                        (GT_U32*) rawPatternOrXPtr, (GT_U32*) rawMaskOrYPtrPtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIntTcamRuleRead
(
    IN    GT_U8                                devNum,
    IN    GT_U32                               ruleStartIndex,
    IN    CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_ENT dataFormat,
    INOUT CPSS_EXMXPM_INT_TCAM_RULE_SIZE_ENT   *ruleSizePtr,
    INOUT GT_U32                               *dataSizePtr,
    OUT   GT_BOOL                              *validPtr,
    OUT   GT_U32                               *rawPatternOrXPtr,
    OUT   GT_U32                               *rawMaskOrYPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIntTcamRuleRead)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with ruleStartIndex [0 / 2048*4 - 1],
                   dataFormat [CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E /
                               CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E],
                   ruleSize [CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E /
                             CPSS_EXMXPM_INT_TCAM_RULE_SIZE_EXTENDED_E],
                   dataSize [0],
                   non NULL validPtr,
                   non NULL rawPatternOrXPtr,
                   non NULL rawMaskOrYPtr,.
    Expected: GT_OK.
    1.2. Call with out of range ruleStartIndex [2048*4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range dataFormat [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with ruleSizePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with dataSizePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.6. Call with validPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.7. Call with rawPatternOrXPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.8. Call with rawMaskOrYPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                ruleStartIndex = 0;
    GT_BOOL                               valid          = GT_FALSE;
    CPSS_EXMXPM_INT_TCAM_RULE_SIZE_ENT    ruleSize       = CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E;
    GT_U32                                dataSize       = 0;
    CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_ENT  dataFormat     = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E;
    GT_U32                                rawPatternOrX  = 0;
    GT_U32                                rawMaskOrYPtr  = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ruleStartIndex [0 / 2048*4 - 1],
                           dataFormat [CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E /
                                       CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E],
                           ruleSize [CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E /
                                     CPSS_EXMXPM_INT_TCAM_RULE_SIZE_EXTENDED_E],
                           dataSize [0],
                           non NULL validPtr,
                           non NULL rawPatternOrXPtr,
                           non NULL rawMaskOrYPtr,.
            Expected: GT_OK.
        */

        /* Call with ruleStartIndex [0] */
        ruleStartIndex = 0;
        ruleSize       = CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E;
        dataSize       = 0;
        dataFormat     = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E;

        st = cpssExMxPmIntTcamRuleRead(dev, ruleStartIndex, dataFormat, &ruleSize, &dataSize,
                                       &valid, &rawPatternOrX, &rawMaskOrYPtr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleStartIndex, dataFormat);

        /* Call with ruleStartIndex [2048*4 - 1] */
        ruleStartIndex = INT_TCAM_RULE_MAX_START_CNS - 1;
        ruleSize       = CPSS_EXMXPM_INT_TCAM_RULE_SIZE_EXTENDED_E;
        dataFormat     = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E;

        st = cpssExMxPmIntTcamRuleRead(dev, ruleStartIndex, dataFormat, &ruleSize, &dataSize,
                                       &valid, &rawPatternOrX, &rawMaskOrYPtr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleStartIndex, dataFormat);

        /*
            1.2. Call with out of range ruleStartIndex [2048*4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ruleStartIndex = INT_TCAM_RULE_MAX_START_CNS;

        st = cpssExMxPmIntTcamRuleRead(dev, ruleStartIndex, dataFormat, &ruleSize, &dataSize,
                                       &valid, &rawPatternOrX, &rawMaskOrYPtr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleStartIndex);

        ruleStartIndex = 1;

        /*
            1.3. Call with out of range dataFormat [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        dataFormat = INT_TCAM_INVALID_ENUM_CNS;

        st = cpssExMxPmIntTcamRuleRead(dev, ruleStartIndex, dataFormat, &ruleSize, &dataSize,
                                       &valid, &rawPatternOrX, &rawMaskOrYPtr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,"%d, dataFormat = %d", dev, dataFormat);

        dataFormat = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E;

        /*
            1.4. Call with ruleSizePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIntTcamRuleRead(dev, ruleStartIndex, dataFormat, NULL, &dataSize,
                                       &valid, &rawPatternOrX, &rawMaskOrYPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,"%d, ruleSizePtr = NULL", dev);

        /*
            1.5. Call with dataSizePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIntTcamRuleRead(dev, ruleStartIndex, dataFormat, &ruleSize, NULL,
                                       &valid, &rawPatternOrX, &rawMaskOrYPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,"%d, dataSizePtr = NULL", dev);

        /*
            1.6. Call with validPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIntTcamRuleRead(dev, ruleStartIndex, dataFormat, &ruleSize, &dataSize,
                                       NULL, &rawPatternOrX, &rawMaskOrYPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,"%d, validPtr = NULL", dev);

        /*
            1.7. Call with rawPatternOrXPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIntTcamRuleRead(dev, ruleStartIndex, dataFormat, &ruleSize, &dataSize,
                                       &valid, NULL, &rawMaskOrYPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,"%d, rawPatternOrXPtr = NULL", dev);

        /*
            1.8. Call with rawMaskOrYPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIntTcamRuleRead(dev, ruleStartIndex, dataFormat, &ruleSize, &dataSize,
                                       &valid, &rawPatternOrX, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,"%d, rawMaskOrYPtr = NULL", dev);
    }

    ruleStartIndex = 0;
    ruleSize       = CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E;
    dataSize       = 0;
    dataFormat     = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIntTcamRuleRead(dev, ruleStartIndex, dataFormat, &ruleSize, &dataSize,
                                       &valid, &rawPatternOrX, &rawMaskOrYPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIntTcamRuleRead(dev, ruleStartIndex, dataFormat, &ruleSize, &dataSize,
                                       &valid, &rawPatternOrX, &rawMaskOrYPtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIntTcamRuleActionWrite
(
    IN GT_U8  devNum,
    IN GT_U32 ruleStartIndex,
    IN GT_U32 rawActionPtr[CPSS_EXMXPM_INT_TCAM_ACTION_SIZE_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIntTcamRuleActionWrite)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with ruleStartIndex [0 / 2048*4 - 1]
                   rawActionPtr [0xAA, 0xBB, 0, 0, 0xFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIntTcamRuleActionRead with the same ruleStartIndex
                                              and non-NULL sizePtr and validPtr.
    Expected: GT_OK and the same rawActionPtr.
    1.3. Call with out of range ruleStartIndex [2048*4]
                   and other valid parameters.
    Expected: NOT GT_OK.
    1.4. Call with rawActionPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      ruleStartIndex = 0;
    GT_BOOL     isEqual        = GT_FALSE;
    GT_U32      rawActionPtr   [CPSS_EXMXPM_INT_TCAM_ACTION_SIZE_CNS] = {0xAA, 0xBB, 0, 0, 0xFF};
    GT_U32      rawActionPtrGet[CPSS_EXMXPM_INT_TCAM_ACTION_SIZE_CNS];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ruleStartIndex [0 / 2048*4 - 1]
                           rawActionPtr [0xAA, 0xBB, 0, 0, 0xFF].
            Expected: GT_OK.
        */

        /* Call with ruleStartIndex [0] */
        ruleStartIndex = 0;

        st = cpssExMxPmIntTcamRuleActionWrite(dev, ruleStartIndex, rawActionPtr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleStartIndex);

        /*
            1.2. Call cpssExMxPmIntTcamRuleActionRead with the same ruleStartIndex
                                                      and non-NULL sizePtr and validPtr.
            Expected: GT_OK and the same rawActionPtr.
        */
        st = cpssExMxPmIntTcamRuleActionRead(dev, ruleStartIndex, rawActionPtrGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIntTcamRuleActionRead: %d, %d", dev, ruleStartIndex);

        if (GT_OK == st)
        {
            /* Verifying values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) rawActionPtr,
                                         (GT_VOID*) rawActionPtrGet,
                                         sizeof(rawActionPtr[0]) * 5)) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another rawActionPtr than was set: %d", dev);
        }

        /* Call with ruleStartIndex [2048*4 - 1] */
        ruleStartIndex = INT_TCAM_RULE_MAX_START_CNS - 1;

        st = cpssExMxPmIntTcamRuleActionWrite(dev, ruleStartIndex, rawActionPtr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleStartIndex);

        /*
            1.2. Call cpssExMxPmIntTcamRuleActionRead with the same ruleStartIndex
                                                      and non-NULL sizePtr and validPtr.
            Expected: GT_OK and the same rawActionPtr.
        */
        st = cpssExMxPmIntTcamRuleActionRead(dev, ruleStartIndex, rawActionPtrGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIntTcamRuleActionRead: %d, %d", dev, ruleStartIndex);

        if (GT_OK == st)
        {
            /* Verifying values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) rawActionPtr,
                                         (GT_VOID*) rawActionPtrGet,
                                         sizeof(rawActionPtr[0]) * 5)) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another rawActionPtr than was set: %d", dev);
        }

        /*
            1.3. Call with out of range ruleStartIndex [2048*4]
                           and other valid parameters.
            Expected: NOT GT_OK.
        */
        ruleStartIndex = INT_TCAM_RULE_MAX_START_CNS;

        st = cpssExMxPmIntTcamRuleActionWrite(dev, ruleStartIndex, rawActionPtr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleStartIndex);

        ruleStartIndex = 0;

        /*
            1.4. Call with rawActionPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIntTcamRuleActionWrite(dev, ruleStartIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,"%d, rawActionPtr = NULL", dev);
    }

    ruleStartIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIntTcamRuleActionWrite(dev, ruleStartIndex, rawActionPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIntTcamRuleActionWrite(dev, ruleStartIndex, rawActionPtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIntTcamRuleActionRead
(
    IN  GT_U8  devNum,
    IN  GT_U32 ruleStartIndex,
    OUT GT_U32 rawActionPtr[CPSS_EXMXPM_INT_TCAM_ACTION_SIZE_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIntTcamRuleActionRead)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with ruleStartIndex [0 / 2048*4 - 1]
                   non NULL rawActionPtr.
    Expected: GT_OK.
    1.2. Call with out of range ruleStartIndex [2048*4]
                   and other valid parameters.
    Expected: NOT GT_OK.
    1.3. Call with rawActionPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      ruleStartIndex = 0;
    GT_U32      rawActionPtr[CPSS_EXMXPM_INT_TCAM_ACTION_SIZE_CNS] = {0xAA, 0xBB, 0, 0, 0xFF};


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ruleStartIndex [0 / 2048*4 - 1]
                           rawActionPtr [0xAA, 0xBB, 0, 0, 0xFF].
            Expected: GT_OK.
        */

        /* Call with ruleStartIndex [0] */
        ruleStartIndex = 0;

        st = cpssExMxPmIntTcamRuleActionRead(dev, ruleStartIndex, rawActionPtr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleStartIndex);

        /* Call with ruleStartIndex [2048*4 - 1] */
        ruleStartIndex = INT_TCAM_RULE_MAX_START_CNS - 1;

        st = cpssExMxPmIntTcamRuleActionRead(dev, ruleStartIndex, rawActionPtr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleStartIndex);

        /*
            1.2. Call with out of range ruleStartIndex [2048*4]
                           and other valid parameters.
            Expected: NOT GT_OK.
        */
        ruleStartIndex = INT_TCAM_RULE_MAX_START_CNS;

        st = cpssExMxPmIntTcamRuleActionRead(dev, ruleStartIndex, rawActionPtr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleStartIndex);

        ruleStartIndex = 0;

        /*
            1.3. Call with rawActionPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIntTcamRuleActionRead(dev, ruleStartIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,"%d, rawActionPtr = NULL", dev);
    }

    ruleStartIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIntTcamRuleActionRead(dev, ruleStartIndex, rawActionPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIntTcamRuleActionRead(dev, ruleStartIndex, rawActionPtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIntTcamRuleAndActionCopy
(
    IN  GT_U8                              devNum,
    IN  CPSS_EXMXPM_INT_TCAM_RULE_SIZE_ENT ruleSize,
    IN  GT_U32                             srcRuleStartIndex,
    IN  GT_U32                             dstRuleStartIndex
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIntTcamRuleAndActionCopy)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with ruleSize [CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E /
                             CPSS_EXMXPM_INT_TCAM_RULE_SIZE_EXTENDED_E]
                   srcRuleStartIndex [0 / 2048*4 - 2]
                   dstRuleStartIndex [1 / 2048*4 - 1].
    Expected: GT_OK.
    1.2. Call with out of range ruleSize [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with out of range srcRuleStartIndex [2048*4]
                   and other valid parameters.
    Expected: NOT GT_OK.
    1.4. Call with out of range dstRuleStartIndex [2048*4]
                   and other valid parameters.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_INT_TCAM_RULE_SIZE_ENT ruleSize = CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E;
    GT_U32                             srcRuleStartIndex = 0;
    GT_U32                             dstRuleStartIndex = 0;
    GT_U32                             ruleDataPattern[6]    = {0,0,0,0,0,0};
    GT_U32                             ruleDataMask[6]       = {0,0,0,0,0,0};


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ruleSize [CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E /
                                     CPSS_EXMXPM_INT_TCAM_RULE_SIZE_EXTENDED_E]
                           srcRuleStartIndex [0 / 1]
                           dstRuleStartIndex [2048*4 - 2 / 2048*4 - 1].
            Expected: GT_OK.
        */

        /* Call with ruleSize [CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E] */
        ruleSize = CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E;

        srcRuleStartIndex = 0;
        dstRuleStartIndex = 2048*2;

        /* write rule with given  srcRuleStartIndex                                */
        /* to cause cpssExMxPmIntTcamRuleAndActionCopy to return GT_OK both for    */
        /* valid and invalid rules                                                 */

        st = cpssExMxPmIntTcamRuleWrite(
            dev, srcRuleStartIndex, GT_FALSE /*valid*/, ruleSize,
            192 /*dataSize*/,
            CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E,
            ruleDataPattern, ruleDataMask);
        if (st != GT_OK)
        {
            PRV_UTF_LOG0_MAC(
                "cpssExMxPmIntTcamRuleAndActionCopy.cpssExMxPmIntTcamRuleWrite failed\n");
        }

        st = cpssExMxPmIntTcamRuleAndActionCopy(dev, ruleSize, srcRuleStartIndex, dstRuleStartIndex);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ruleSize, srcRuleStartIndex, dstRuleStartIndex);

        /* Call with ruleSize [CPSS_EXMXPM_INT_TCAM_RULE_SIZE_EXTENDED_E] */
        ruleSize = CPSS_EXMXPM_INT_TCAM_RULE_SIZE_EXTENDED_E;

        srcRuleStartIndex = 2048;
        dstRuleStartIndex = 2048*3;

        st = cpssExMxPmIntTcamRuleWrite(
            dev, srcRuleStartIndex, GT_TRUE /*valid*/, ruleSize,
            192 /*dataSize*/,
            CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E,
            ruleDataPattern, ruleDataMask);
        if (st != GT_OK)
        {
            PRV_UTF_LOG0_MAC(
                "cpssExMxPmIntTcamRuleAndActionCopy.cpssExMxPmIntTcamRuleWrite failed\n");
        }

        st = cpssExMxPmIntTcamRuleAndActionCopy(dev, ruleSize, srcRuleStartIndex, dstRuleStartIndex);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ruleSize, srcRuleStartIndex, dstRuleStartIndex);

        /*
            1.2. Call with out of range ruleSize [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ruleSize = INT_TCAM_INVALID_ENUM_CNS;

        st = cpssExMxPmIntTcamRuleAndActionCopy(dev, ruleSize, srcRuleStartIndex, dstRuleStartIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, ruleSize);

        ruleSize = CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E;

        /*
            1.3. Call with out of range srcRuleStartIndex [2048*4]
                           and other valid parameters.
            Expected: NOT GT_OK.
        */
        srcRuleStartIndex = 2048*4;

        st = cpssExMxPmIntTcamRuleAndActionCopy(dev, ruleSize, srcRuleStartIndex, dstRuleStartIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,"%d, srcRuleStartIndex = %d", dev, srcRuleStartIndex);

        srcRuleStartIndex = 2;

        /*
            1.4. Call with out of range dstRuleStartIndex [2048*4]
                           and other valid parameters.
            Expected: NOT GT_OK.
        */
        dstRuleStartIndex = 2048*4;

        st = cpssExMxPmIntTcamRuleAndActionCopy(dev, ruleSize, srcRuleStartIndex, dstRuleStartIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,"%d, dstRuleStartIndex = %d", dev, dstRuleStartIndex);
    }

    ruleSize = CPSS_EXMXPM_INT_TCAM_RULE_SIZE_STANDARD_E;

    srcRuleStartIndex = 0;
    dstRuleStartIndex = 2048*2;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIntTcamRuleAndActionCopy(dev, ruleSize, srcRuleStartIndex, dstRuleStartIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIntTcamRuleAndActionCopy(dev, ruleSize, srcRuleStartIndex, dstRuleStartIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill Rule Action table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmIntTcamFillRuleActionTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in Rule Action table.
         Call cpssExMxPmIntTcamRuleActionWrite with ruleStartIndex [0..numEntries - 1]
                               rawActionPtr [0xAA, 0xBB, 0, 0, 0xFF].
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmIntTcamRuleActionWrite with ruleStartIndex [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in Rule Action table and compare with original.
         Call cpssExMxPmIntTcamRuleActionRead with the same ruleStartIndex
                                                          and non-NULL sizePtr and validPtr.
    Expected: GT_OK and the same rawActionPtr.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmIntTcamRuleActionRead with ruleStartIndex [numEntries] and non-NULL sizePtr and validPtr.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual    = GT_FALSE;
    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    GT_U32      rawActionPtr   [CPSS_EXMXPM_INT_TCAM_ACTION_SIZE_CNS] = {0xAA, 0xBB, 0, 0, 0xFF};
    GT_U32      rawActionPtrGet[CPSS_EXMXPM_INT_TCAM_ACTION_SIZE_CNS];


    /* Fill the entry for Rule Action table */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = 4 * 2048;

        /* 1.2. Fill all entries in Rule Action table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            rawActionPtr[0] = iTemp;
            rawActionPtr[1] = iTemp;
            rawActionPtr[2] = iTemp;

            st = cpssExMxPmIntTcamRuleActionWrite(dev, iTemp, rawActionPtr);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIntTcamRuleActionWrite: %d, %d", dev, iTemp);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmIntTcamRuleActionWrite(dev, numEntries, rawActionPtr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIntTcamRuleActionWrite: %d, %d", dev, iTemp);

        /* 1.4. Read all entries in Rule Action table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            rawActionPtr[0] = iTemp;
            rawActionPtr[1] = iTemp;
            rawActionPtr[2] = iTemp;

            st = cpssExMxPmIntTcamRuleActionRead(dev, iTemp, rawActionPtrGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmIntTcamRuleActionRead: %d, %d", dev, iTemp);

            if (GT_OK == st)
            {
                /* Verifying values */
                isEqual = (0 == cpssOsMemCmp((GT_VOID*) rawActionPtr,
                                             (GT_VOID*) rawActionPtrGet,
                                             sizeof(rawActionPtr[0]) * 5)) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                           "get another rawActionPtr than was set: %d", dev);
            }
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmIntTcamRuleActionRead(dev, numEntries, rawActionPtrGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIntTcamRuleActionRead: %d, %d", dev, iTemp);
    }
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmIntTcam suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmIntTcam)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIntTcamWriteModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIntTcamWriteModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIntTcamRuleValidStatusSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIntTcamRuleStateGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIntTcamRuleWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIntTcamRuleRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIntTcamRuleActionWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIntTcamRuleActionRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIntTcamRuleAndActionCopy)
    /* Test for Tables */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIntTcamFillRuleActionTable)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmIntTcam)

