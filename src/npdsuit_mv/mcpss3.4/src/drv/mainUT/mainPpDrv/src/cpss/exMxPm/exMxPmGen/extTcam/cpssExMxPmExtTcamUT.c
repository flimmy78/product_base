/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmExternalTcamUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmExternalTcam, that provides
*       CPSS ExMxPm External TCAM API definitions.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */

#include <cpss/exMxPm/exMxPmGen/extTcam/cpssExMxPmExternalTcam.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define EXT_TCAM_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamRegSet
(
    IN  GT_U8                              devNum,
    IN  GT_U32                             tcamId,
    IN  GT_U32                             regAddr,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_REG      tcamReg
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamRegSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with tcamId [0],
                   regAddr [0x200 / 0x300],
                   tcamReg [2, 4, 8 / 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmExternalTcamRegGet with the same tcamId,
                                           regAddr
                                           and non-NULL tcamReg.
    Expected: GT_OK and the same tcamReg.
    1.3. Call with out of range tcamId [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with tcamReg [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                          tcamId  = 0;
    GT_U32                          regAddr = 0;
    CPSS_EXMXPM_EXTERNAL_TCAM_REG   tcamReg;
    CPSS_EXMXPM_EXTERNAL_TCAM_REG   tcamRegGet;
    GT_BOOL                         isEqual = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tcamId [0],
                           regAddr [0x200 / 0x300],
                           tcamReg [2, 4, 8 / 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF].
            Expected: GT_OK.
        */
        tcamId  = 0;
        regAddr = 0x200;

        tcamReg[0] = 2;
        tcamReg[1] = 4;
        tcamReg[2] = 8;

        st = cpssExMxPmExternalTcamRegSet(dev, tcamId, regAddr, tcamReg);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamId, regAddr);

        /*
            1.2. Call cpssExMxPmExternalTcamRegGet with the same tcamId,
                                                   regAddr
                                                   and non-NULL tcamReg.
            Expected: GT_OK and the same tcamReg.
        */
        st = cpssExMxPmExternalTcamRegGet(dev, tcamId, regAddr, tcamRegGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmExternalTcamRegGet: %d, %d, %d", dev, tcamId, regAddr);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) tcamReg,
                                     (GT_VOID*) tcamRegGet,
                                     sizeof(tcamReg[0] * 3))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another tcamReg than was set: %d", dev);

        /*
            1.1. Call with tcamId [0],
                           regAddr [0x200 / 0x300],
                           tcamReg [2, 4, 8 / 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF].
            Expected: GT_OK.
        */
        tcamId  = 0;
        regAddr = 0x300;

        tcamReg[0] = 0xFFFFFFFF;
        tcamReg[1] = 0xFFFFFFFF;
        tcamReg[2] = 0xFFFF;

        st = cpssExMxPmExternalTcamRegSet(dev, tcamId, regAddr, tcamReg);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamId, regAddr);

        /*
            1.2. Call cpssExMxPmExternalTcamRegGet with the same tcamId,
                                                   regAddr
                                                   and non-NULL tcamReg.
            Expected: GT_OK and the same tcamReg.
        */
        st = cpssExMxPmExternalTcamRegGet(dev, tcamId, regAddr, tcamRegGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmExternalTcamRegGet: %d, %d, %d", dev, tcamId, regAddr);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) tcamReg,
                                     (GT_VOID*) tcamRegGet,
                                     sizeof(tcamReg[0]*3))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another tcamReg than was set: %d", dev);

        /*
            1.3. Call   with out of range tcamId [4]
                        and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        tcamId = 4;

        st = cpssExMxPmExternalTcamRegSet(dev, tcamId, regAddr, tcamReg);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamId);

        tcamId = 0;
        regAddr = 0;

        /*
            1.4. Call with tcamReg [NULL]
                           and other parameters from 1.1
            Expected: GT_BAD_PTR
        */
        st = cpssExMxPmExternalTcamRegSet(dev, tcamId, regAddr, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, tcamReg = NULL", dev);
    }

    tcamId  = 0;
    regAddr = 0x200;

    tcamReg[0] = 2;
    tcamReg[1] = 4;
    tcamReg[2] = 8;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamRegSet(dev, tcamId, regAddr, tcamReg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamRegSet(dev, tcamId, regAddr, tcamReg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamRegGet
(
    IN  GT_U8                              devNum,
    IN  GT_U32                             tcamId,
    IN  GT_U32                             regAddr,
    OUT CPSS_EXMXPM_EXTERNAL_TCAM_REG      tcamReg
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamRegGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with tcamId [0],
                   regAddr [0x200 / 0x300]
                   and non-NULL tcamReg.
    Expected: GT_OK.
    1.2. Call with out of range tcamId [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with tcamReg [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                         tcamId  = 0;
    GT_U32                         regAddr = 0;
    CPSS_EXMXPM_EXTERNAL_TCAM_REG  tcamReg;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tcamId [0]
                           regAddr [0x200 / 0x300]
                           and non-NULL tcamReg.
            Expected: GT_OK.
        */

        /* call with tcamId = 0 */
        tcamId  = 0;
        regAddr = 0x200;

        st = cpssExMxPmExternalTcamRegGet(dev, tcamId, regAddr, tcamReg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with tcamId = 0 */
        tcamId  = 0;
        regAddr = 0x300;

        st = cpssExMxPmExternalTcamRegGet(dev, tcamId, regAddr, tcamReg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with out of range tcamId [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        tcamId = 4;

        st = cpssExMxPmExternalTcamRegGet(dev, tcamId, regAddr, tcamReg);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamId);

        tcamId = 0;
        regAddr = 0x200;

        /*
            1.3. Call with tcamReg [NULL]
                           and other parameters from 1.1
            Expected: GT_BAD_PTR
        */
        st = cpssExMxPmExternalTcamRegGet(dev, tcamId, regAddr, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, tcamReg = NULL", dev);
    }

    tcamId  = 0;
    regAddr = 0x200;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamRegGet(dev, tcamId, regAddr, tcamReg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamRegGet(dev, tcamId, regAddr, tcamReg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamRuleWrite
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      tcamId,
    IN  GT_U32                                      ruleStartIndex,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT     ruleSize,
    IN  GT_U32                                      dataSize,
    IN  GT_BOOL                                     valid,
    IN  GT_BOOL                                     ageEnable,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT   dataFormat,
    IN  GT_U32                                      patternOrXArr[],
    IN  GT_U32                                      maskOrYArr[]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamRuleWrite)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with tcamId [0],
                   ruleSize [CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E /
                             CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E],
                   ruleStartIndex [0 / 0xFFF],
                   dataSize [0 / 80],
                   valid [GT_FALSE / GT_TRUE],
                   ageEnable [GT_FALSE / GT_TRUE],
                   dataFormat [CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E / CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_DATA_MASK_AND_LOGIC_E],
                   patternOrXArr[0 / 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF],
                   maskOrYArr[0 / 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmExternalTcamRuleRead with the same
                     tcamId,
                     ruleSize,
                     ruleStartIndex
                     dataSize
                     dataFormat
                     and non-NULL
                        validPtr,
                        ageEnablePtr,
                        patternOrXArr,
                        maskOrYArr.
    Expected: GT_OK and the same validPtr, ageEnablePtr, patternOrXArr, maskOrYArr.
    1.3. Call with out of range tcamId [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4 Call with out of range ruleSize [0x5AAAAAA5]
                  and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5 Call with out of range dataFormat [0x5AAAAAA5]
                  and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with patternOrXArr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR
    1.7. Call with maskOrYArr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st      = GT_OK;
    GT_BOOL     isEqual = GT_FALSE;
    GT_U8       dev;

    GT_U32                                    tcamId       = 0;
    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT   ruleSize     = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E;
    GT_U32                                    ruleStartIndex = 0;
    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT dataFormat   = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E;
    GT_U32                                    dataSize     = 0;
    GT_BOOL                                   valid        = GT_FALSE;
    GT_BOOL                                   ageEnable    = GT_FALSE;
    GT_U32                                    patternOrXArr[3];
    GT_U32                                    maskOrYArr[3];

    GT_BOOL                                   validGet     = GT_FALSE;
    GT_BOOL                                   ageEnableGet = GT_FALSE;
    GT_U32                                    patternOrXArrGet[3];
    GT_U32                                    maskOrYArrGet[3];


    cpssOsBzero((GT_VOID*) maskOrYArr, sizeof(maskOrYArr));
    cpssOsBzero((GT_VOID*) patternOrXArr, sizeof(patternOrXArr));
    cpssOsBzero((GT_VOID*) maskOrYArrGet, sizeof(maskOrYArrGet));
    cpssOsBzero((GT_VOID*) patternOrXArrGet, sizeof(patternOrXArrGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tcamId [0],
                           ruleSize [CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E /
                                     CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E],
                           ruleStartIndex [0 / 0xFFF],
                           dataSize [0 / 80],
                           valid [GT_FALSE / GT_TRUE],
                           ageEnable [GT_FALSE / GT_TRUE],
                           dataFormat [CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E / CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_DATA_MASK_AND_LOGIC_E],
                           patternOrXArr[0 / 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF],
                           maskOrYArr[0 / 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF].
            Expected: GT_OK.
        */
        /* call with tcamId = 0 */
        tcamId         = 0;
        ruleSize       = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E;
        ruleStartIndex = 0;
        dataSize       = 0;
        valid          = GT_FALSE;
        ageEnable      = GT_FALSE;
        dataFormat     = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E;

        maskOrYArr[0] = 0;
        maskOrYArr[1] = 0;
        maskOrYArr[2] = 0;

        patternOrXArr[0] = 0;
        patternOrXArr[1] = 0;
        patternOrXArr[2] = 0;

        st = cpssExMxPmExternalTcamRuleWrite(dev, tcamId, ruleStartIndex, ruleSize,
                                             dataSize, valid, ageEnable, dataFormat,
                                             patternOrXArr, maskOrYArr);
        UTF_VERIFY_EQUAL7_PARAM_MAC(GT_OK, st, dev, tcamId, ruleStartIndex, ruleSize,
                                             dataSize, valid, ageEnable);

        /*
            1.2. Call cpssExMxPmExternalTcamRuleRead
                        with non-NULL validPtr,
                             patternOrXArr,
                             maskOrYArr
            Expected: GT_OK and the same validPtr, patternOrXArr, maskOrYArr
        */
        st = cpssExMxPmExternalTcamRuleRead(dev, tcamId, ruleStartIndex, ruleSize,
                                            dataSize, dataFormat, &validGet, &ageEnableGet,
                                            patternOrXArrGet, maskOrYArrGet);
        UTF_VERIFY_EQUAL6_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamRuleGet, %d, %d, %d, %d, %d, %d",
                                     dev, tcamId, ruleStartIndex, ruleSize, dataSize, dataFormat);

        /* Validating values */
        UTF_VERIFY_EQUAL1_STRING_MAC(valid, validGet, "got another valid than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ageEnable, ageEnableGet, "got another ageEnable than was set: %d", dev);


        isEqual = (0 == cpssOsMemCmp((GT_VOID*) patternOrXArr,(GT_VOID*) patternOrXArrGet,sizeof(patternOrXArr[0]*3))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,"get another patternOrXArr than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) maskOrYArr,(GT_VOID*) maskOrYArrGet,sizeof(maskOrYArr[0]*3))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,"get another maskOrYArr than was set: %d", dev);

        /*
            1.1. Call   with tcamId [0],
                        ruleSize [CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E /
                                  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E],
                        ruleStartIndex [0 / 0xFFF],
                        dataSize [0 / 80],
                        valid [GT_FALSE / GT_TRUE],
                        patternOrXArr[0 / 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF],
                        maskOrYArr[0 / 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF].
            Expected: GT_OK.
        */
        tcamId          = 0;
        ruleSize        = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E;
        ruleStartIndex  = 0xF;
        dataSize        = 80;
        valid           = GT_TRUE;
        ageEnable       = GT_TRUE;
        dataFormat      = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_DATA_MASK_AND_LOGIC_E;

        maskOrYArr[0] = 0xFFFFFFFF;
        maskOrYArr[1] = 0xFFFFFFFF;
        maskOrYArr[2] = 0xFFFF;

        patternOrXArr[0] = 0xFFFFFFFF;
        patternOrXArr[1] = 0xFFFFFFFF;
        patternOrXArr[2] = 0xFFFF;

        st = cpssExMxPmExternalTcamRuleWrite(dev, tcamId, ruleStartIndex, ruleSize,
                                             dataSize, valid, ageEnable, dataFormat,
                                             patternOrXArr, maskOrYArr);
        UTF_VERIFY_EQUAL7_PARAM_MAC(GT_OK, st, dev, tcamId, ruleStartIndex, ruleSize,
                                             dataSize, valid, ageEnable);

        /*
            1.2. Call cpssExMxPmExternalTcamRuleRead
                        with non-NULL validPtr,
                        patternOrXArr,
                        maskOrYArr
            Expected: GT_OK and the same validPtr, patternOrXArr, maskOrYArr
        */
        st = cpssExMxPmExternalTcamRuleRead(dev, tcamId, ruleStartIndex, ruleSize,
                                            dataSize, dataFormat, &validGet, &ageEnableGet,
                                            patternOrXArrGet, maskOrYArrGet);
        UTF_VERIFY_EQUAL6_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamRuleGet, %d, %d, %d, %d, %d, %d",
                                     dev, tcamId, ruleStartIndex, ruleSize, dataSize, dataFormat);

        /* Validating values */
        UTF_VERIFY_EQUAL1_STRING_MAC(valid, validGet, "got another valid than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) patternOrXArr,(GT_VOID*) patternOrXArrGet,sizeof(patternOrXArr[0]*3))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,"get another patternOrXArr than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) maskOrYArr,(GT_VOID*) maskOrYArrGet,sizeof(maskOrYArr[0]*3))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,"get another maskOrYArr than was set: %d", dev);

        /*
            1.3. Call   with out of range tcamId [4]
                        and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        tcamId = 4;

        st = cpssExMxPmExternalTcamRuleWrite(dev, tcamId, ruleStartIndex, ruleSize,
                                             dataSize, valid, ageEnable, dataFormat,
                                             patternOrXArr, maskOrYArr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamId);

        tcamId = 0;

        /*
            1.4 Call with out of range ruleSize [0x5AAAAAA5]
                          and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ruleSize = EXT_TCAM_INVALID_ENUM_CNS;

        st = cpssExMxPmExternalTcamRuleWrite(dev, tcamId, ruleStartIndex, ruleSize,
                                             dataSize, valid, ageEnable, dataFormat,
                                             patternOrXArr, maskOrYArr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ruleSize = %d", dev, ruleSize);

        ruleSize = 0;

        /*
            1.5 Call with out of range dataFormat [0x5AAAAAA5]
                          and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        dataFormat = EXT_TCAM_INVALID_ENUM_CNS;

        st = cpssExMxPmExternalTcamRuleWrite(dev, tcamId, ruleStartIndex, ruleSize,
                                             dataSize, valid, ageEnable, dataFormat,
                                             patternOrXArr, maskOrYArr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dataFormat = %d", dev, dataFormat);

        dataFormat = 0;

        /*
            1.6. Call with patternOrXArr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamRuleWrite(dev, tcamId, ruleStartIndex, ruleSize,
                                             dataSize, valid, ageEnable, dataFormat,
                                             NULL, maskOrYArr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, patternOrXArr = NULL", dev);

        /*
            1.7. Call with maskOrYArr [NULL]
                           and other parameters from 1.1
            Expected: GT_BAD_PTR
        */
        st = cpssExMxPmExternalTcamRuleWrite(dev, tcamId, ruleStartIndex, ruleSize,
                                             dataSize, valid, ageEnable, dataFormat,
                                             patternOrXArr, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskOrYArr = NULL", dev);
    }

    tcamId         = 0;
    ruleSize       = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E;
    ruleStartIndex = 0;
    dataSize       = 0;
    valid          = GT_FALSE;
    ageEnable      = GT_FALSE;
    dataFormat     = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E;

    maskOrYArr[0] = 0;
    maskOrYArr[1] = 0;
    maskOrYArr[2] = 0;

    patternOrXArr[0] = 0;
    patternOrXArr[1] = 0;
    patternOrXArr[2] = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamRuleWrite(dev, tcamId, ruleStartIndex, ruleSize,
                                             dataSize, valid, ageEnable, dataFormat,
                                             patternOrXArr, maskOrYArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmExternalTcamRuleWrite(dev, tcamId, ruleStartIndex, ruleSize,
                                             dataSize, valid, ageEnable, dataFormat,
                                             patternOrXArr, maskOrYArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamRuleRead
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      tcamId,
    IN  GT_U32                                      ruleStartIndex,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT     ruleSize,
    IN  GT_U32                                      dataSize,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT   dataFormat,
    OUT GT_BOOL                                     validPtr,
    OUT GT_BOOL                                     ageEnablePtr,
    OUT GT_U32                                      patternOrXArr[],
    OUT GT_U32                                      maskOrYArr[]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamRuleRead)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with tcamId [0],
                   ruleSize [CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E /
                             CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E],
                   ruleStartIndex [0/ 0xFFF],
                   dataSize [0 / 80],
                   dataFormat [CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E/
                               CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_DATA_MASK_E],
                   and non-NULL
                       validPtr,
                       ageEnablePtr,
                       patternOrXArr,
                       maskOrYArr.
    Expected: GT_OK.
    1.2. Call with out of range tcamId [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3 Call with out of range ruleSize [0x5AAAAAA5]
                  and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range dataFormat[0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with validPtr [NULL] and
                   other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.6. Call with ageEnablePtr [NULL] and
                   other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.7. Call with patternOrXArr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.8. Call with maskOrYArr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      tcamId         = 0;
    GT_U32      ruleStartIndex = 0;
    GT_U32      dataSize       = 0;
    GT_BOOL     valid          = GT_FALSE;
    GT_BOOL     ageEnable      = GT_FALSE;
    GT_U32      patternOrXArr[2];
    GT_U32      maskOrYArr[2];

    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT   ruleSize   = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E;
    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT dataFormat = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E;


    cpssOsBzero((GT_VOID*) maskOrYArr, sizeof(maskOrYArr));
    cpssOsBzero((GT_VOID*) patternOrXArr, sizeof(patternOrXArr));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tcamId [0],
                           ruleSize [CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E /
                                     CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E],
                           ruleStartIndex [0/ 0xFFF],
                           dataSize [0 / 80],
                           dataFormat [CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E/
                                       CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_DATA_MASK_E],
                           and non-NULL
                               validPtr,
                               ageEnablePtr,
                               patternOrXArr,
                               maskOrYArr.
            Expected: GT_OK.
        */

        /* Call with tcamId [0] */
        tcamId         = 0;
        ruleSize       = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E;
        ruleStartIndex = 0;
        dataSize       = 0;
        dataFormat     = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E;

        st = cpssExMxPmExternalTcamRuleRead(dev, tcamId, ruleStartIndex, ruleSize,
                                            dataSize, dataFormat, &valid, &ageEnable,
                                            patternOrXArr, maskOrYArr);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, tcamId, ruleStartIndex, ruleSize,
                                            dataSize, dataFormat);

        /* Call with tcamId [3] */
        tcamId     = 0;
        ruleSize   = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E;
        ruleStartIndex  = 0x0FFF;
        dataSize   = 80;
        dataFormat = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_DATA_MASK_AND_LOGIC_E;

        st = cpssExMxPmExternalTcamRuleRead(dev, tcamId, ruleStartIndex, ruleSize,
                                            dataSize, dataFormat, &valid, &ageEnable,
                                            patternOrXArr, maskOrYArr);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, tcamId, ruleStartIndex, ruleSize,
                                    dataSize, dataFormat);
        /*
            1.2. Call with out of range tcamId [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        tcamId = 4;

        st = cpssExMxPmExternalTcamRuleRead(dev, tcamId, ruleStartIndex, ruleSize,
                                            dataSize, dataFormat, &valid, &ageEnable,
                                            patternOrXArr, maskOrYArr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamId);

        tcamId = 0;

        /*
            1.3 Call with out of range ruleSize [0x5AAAAAA5]
                          and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ruleSize = 0x5AAAAAA5;

        st = cpssExMxPmExternalTcamRuleRead(dev, tcamId, ruleStartIndex, ruleSize,
                                            dataSize, dataFormat, &valid, &ageEnable,
                                            patternOrXArr, maskOrYArr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ruleSize = %d", dev, ruleSize);

        ruleSize = 0;

        /*
            1.4 Call with out of range dataFormat [0x5AAAAAA5]
                          and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        dataFormat = 0x5AAAAAA5;

        st = cpssExMxPmExternalTcamRuleRead(dev, tcamId, ruleStartIndex, ruleSize,
                                            dataSize, dataFormat, &valid, &ageEnable,
                                            patternOrXArr, maskOrYArr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, dataFormat = %d", dev, dataFormat);

        dataFormat = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E;

        /*
            1.5. Call with validPtr [NULL]
                           and other parameters from 1.1
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamRuleRead(dev, tcamId, ruleStartIndex, ruleSize,
                                            dataSize, dataFormat, NULL, &ageEnable,
                                            patternOrXArr, maskOrYArr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validPtr = NULL", dev);

        /*
            1.6. Call with ageEnablePtr [NULL]
                           and other parameters from 1.1
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamRuleRead(dev, tcamId, ruleStartIndex, ruleSize,
                                            dataSize, dataFormat, &valid, NULL,
                                            patternOrXArr, maskOrYArr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ageEnablePtr = NULL", dev);

        /*
            1.7. Call with patternOrXArr [NULL]
                           and other parameters from 1.1
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamRuleRead(dev, tcamId, ruleStartIndex, ruleSize,
                                            dataSize, dataFormat, &valid, &ageEnable,
                                            NULL, maskOrYArr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, patternOrXArr = NULL", dev);

        /*
            1.8. Call with maskOrYArr [NULL]
                           and other parameters from 1.1
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamRuleRead(dev, tcamId, ruleStartIndex, ruleSize,
                                            dataSize, dataFormat, &valid, &ageEnable,
                                            patternOrXArr, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskOrYArr = NULL", dev);
    }

    tcamId         = 0;
    ruleSize       = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E;
    ruleStartIndex = 0;
    dataSize       = 0;
    dataFormat     = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamRuleRead(dev, tcamId, ruleStartIndex, ruleSize,
                                            dataSize, dataFormat, &valid, &ageEnable,
                                            patternOrXArr, maskOrYArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamRuleRead(dev, tcamId, ruleStartIndex, ruleSize,
                                        dataSize, dataFormat, &valid, &ageEnable,
                                        patternOrXArr, maskOrYArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamGlobalWriteModeSet
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      tcamId,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT   ruleFormat
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamGlobalWriteModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with tcamId [0],
                   ruleFormat[CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E/
                               CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_DATA_MASK_AND_LOGIC_E],
    Expected: GT_OK.
    1.2. Call cpssExMxPmExternalTcamGlobalWriteModeGet with not NULL ruleFormatPtr
              and other params from 1.1.
    Expected: GT_OK and the same ruleFormatPtr as was set.
    1.3. Call with tcamId [4] and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with ruleFormat[0x5AAAAAA5] and other params from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32                                      tcamId        = 0;
    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT   ruleFormat    = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E;
    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT   ruleFormatGet = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E;
    CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_ENT          tcamType;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Get the device type of the external TCAM from CPSS DB. */
        st = cpssExMxPmExternalTcamTypeGet(dev, tcamId, &tcamType);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamTypeGet: %d, %d", dev, tcamId);
    
        /* global write mode setting is applicable only for IDT */ 
        if(tcamType == CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_IDT_E)
        {
            /*
                1.1. Call with tcamId [0],
                               ruleFormat[CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E/
                                           CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_DATA_MASK_AND_LOGIC_E],
                Expected: GT_OK.
            */
            /* iterate with tcamId = 0 */
            tcamId = 0;
            ruleFormat = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E;
    
            st = cpssExMxPmExternalTcamGlobalWriteModeSet(dev, tcamId, ruleFormat);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamId, ruleFormat);
    
            /*
                1.2. Call cpssExMxPmExternalTcamGlobalWriteModeGet with not NULL ruleFormatPtr
                          and other params from 1.1.
                Expected: GT_OK and the same ruleFormatPtr as was set.
            */
            st = cpssExMxPmExternalTcamGlobalWriteModeGet(dev, tcamId, &ruleFormatGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamGlobalWriteModeGet: %d, %d", dev, tcamId);
            UTF_VERIFY_EQUAL1_STRING_MAC(ruleFormat, ruleFormatGet, "got another ruleFormat than was set: %d", dev);
    
            /* iterate with tcamId = 3 */
            tcamId = 0;
            ruleFormat = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_DATA_MASK_AND_LOGIC_E;
    
            st = cpssExMxPmExternalTcamGlobalWriteModeSet(dev, tcamId, ruleFormat);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamId, ruleFormat);
    
            /*
                1.2. Call cpssExMxPmExternalTcamGlobalWriteModeGet with not NULL ruleFormatPtr
                          and other params from 1.1.
                Expected: GT_OK and the same ruleFormatPtr as was set.
            */
            st = cpssExMxPmExternalTcamGlobalWriteModeGet(dev, tcamId, &ruleFormatGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamGlobalWriteModeGet: %d, %d", dev, tcamId);
            UTF_VERIFY_EQUAL1_STRING_MAC(ruleFormat, ruleFormatGet, "got another ruleFormat than was set: %d", dev);
    
            /*
                1.3. Call with tcamId [4] and other params from 1.1.
                Expected: NOT GT_OK.
            */
            tcamId = 4;
    
            st = cpssExMxPmExternalTcamGlobalWriteModeSet(dev, tcamId, ruleFormat);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamId);
    
            tcamId = 0;
    
            /*
                1.4. Call with ruleFormat[0x5AAAAAA5] and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            ruleFormat = EXT_TCAM_INVALID_ENUM_CNS;
    
            st = cpssExMxPmExternalTcamGlobalWriteModeSet(dev, tcamId, ruleFormat);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ruleFormat = %d", dev, ruleFormat);
        }
        else
        {
            /* expected: NOT GT_OK */
            tcamId = 0;
            ruleFormat = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E;
    
            st = cpssExMxPmExternalTcamGlobalWriteModeSet(dev, tcamId, ruleFormat);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamId, ruleFormat);
        }
    }

    tcamId = 0;
    ruleFormat = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamGlobalWriteModeSet(dev, tcamId, ruleFormat);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamGlobalWriteModeSet(dev, tcamId, ruleFormat);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamGlobalWriteModeGet
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      tcamId,
    OUT CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT   *ruleFormatPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamGlobalWriteModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with tcamId [0],
                   not NULL ruleFormatPtr
    Expected: GT_OK.
    1.2. Call with tcamId [4] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with ruleFormatPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32                                      tcamId     = 0;
    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT   ruleFormat = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E;
    CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_ENT          tcamType;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Get the device type of the external TCAM from CPSS DB. */
        st = cpssExMxPmExternalTcamTypeGet(dev, tcamId, &tcamType);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamTypeGet: %d, %d", dev, tcamId);
    
        /* global write mode setting is applicable only for IDT */ 
        if(tcamType == CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_IDT_E)
        {
            /*
                1.1. Call with tcamId [0],
                               not NULL ruleFormatPtr
                Expected: GT_OK.
            */
            tcamId = 0;
    
            st = cpssExMxPmExternalTcamGlobalWriteModeGet(dev, tcamId, &ruleFormat);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamId);
    
            tcamId = 0;
    
            st = cpssExMxPmExternalTcamGlobalWriteModeGet(dev, tcamId, &ruleFormat);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamId);
    
            /*
                1.2. Call with tcamId [4] and other params from 1.1.
                Expected: NOT GT_OK.
            */
            tcamId = 4;
    
            st = cpssExMxPmExternalTcamGlobalWriteModeGet(dev, tcamId, &ruleFormat);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamId);
    
            tcamId = 0;
    
            /*
                1.3. Call with ruleFormatPtr [NULL] and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmExternalTcamGlobalWriteModeGet(dev, tcamId, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ruleFormatPtr = NULL", dev);
        }
        else
        {
            /* expected: NOT GT_OK */
            tcamId = 0;
    
            st = cpssExMxPmExternalTcamGlobalWriteModeGet(dev, tcamId, &ruleFormat);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamId, ruleFormat);
        }
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamGlobalWriteModeGet(dev, tcamId, &ruleFormat);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamGlobalWriteModeGet(dev, tcamId, &ruleFormat);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamTypeGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              tcamId,
    OUT CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_ENT *tcamTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamTypeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with tcamId [0],
                   not NULL ruleFormatPtr
    Expected: GT_OK.
    1.2. Call with tcamId [4] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with tcamTypePtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32                              tcamId   = 0;
    CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_ENT  tcamType = CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_IDT_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tcamId [0],
                           not NULL ruleFormatPtr
            Expected: GT_OK.
        */
        tcamId = 0;

        st = cpssExMxPmExternalTcamTypeGet(dev, tcamId, &tcamType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamId);

        tcamId = 0;

        st = cpssExMxPmExternalTcamTypeGet(dev, tcamId, &tcamType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamId);

        /*
            1.2. Call with tcamId [4] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        tcamId = 4;

        st = cpssExMxPmExternalTcamTypeGet(dev, tcamId, &tcamType);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamId);

        tcamId = 0;

        /*
            1.3. Call with tcamTypePtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamTypeGet(dev, tcamId, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, tcamTypePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamTypeGet(dev, tcamId, &tcamType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamTypeGet(dev, tcamId, &tcamType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamHitIndexConvertSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT            tcamClient,
    IN  GT_U32                                          clientKeyType,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_ENT       lookupType,
    IN  GT_U32                                          lookupIndex,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_HIT_INDEX_CONVERT_STC *indexConvertPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamHitIndexConvertSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with tcamClient [CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E / CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E],
                   clientKeyType [0 / 15],
                   lookupType [CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E / CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_DUAL_E]
                   lookupIndex [0 / 1],
                   indexConvertPtr {shiftOperation [CPSS_SHIFT_OPERATION_SHIFT_LEFT_E /
                                                    CPSS_SHIFT_OPERATION_SHIFT_RIGHT_E],
                                    shiftValue [0 / 3],
                                    baseAddrOperation [CPSS_ADJUST_OPERATION_ADD_E /
                                                    CPSS_ADJUST_OPERATION_SUBSTRUCT_E],
                                    baseAddrOffset [0 / 0xFFFFFF] }.
    Expected: GT_OK.
    1.2. Call cpssExMxPmExternalTcamHitIndexConvertGet
                with the same tcamClient,
                     lookupIndex
                     and non NULL indexConvertPtr.
    Expected: GT_OK and the same indexConvertPtr.
    1.3. Call with out of range tcamClient [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range lookupType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range lookupIndex [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range clientKeyType [16]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range indexConvertPtr->shiftOperation [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.8. Call with out of range indexConvertPtr->baseAddrOperation [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call with indexConvertPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT            tcamClient;
    GT_U32                                          clientKeyType;
    CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_ENT       lookupType;
    GT_U32                                          lookupIndex;
    CPSS_EXMXPM_EXTERNAL_TCAM_HIT_INDEX_CONVERT_STC indexConvert;
    CPSS_EXMXPM_EXTERNAL_TCAM_HIT_INDEX_CONVERT_STC indexConvertGet;

    cpssOsBzero((GT_VOID*) &indexConvert, sizeof(indexConvert));
    cpssOsBzero((GT_VOID*) &indexConvertGet, sizeof(indexConvertGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tcamClient [CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E / CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E],
                           clientKeyType [0 / 15],
                           lookupType [CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E / CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_DUAL_E]
                           lookupIndex [0 / 1],
                           indexConvertPtr {shiftOperation [CPSS_SHIFT_OPERATION_SHIFT_LEFT_E /
                                                            CPSS_SHIFT_OPERATION_SHIFT_RIGHT_E],
                                            shiftValue [0 / 3],
                                            baseAddrOperation [CPSS_ADJUST_OPERATION_ADD_E /
                                                            CPSS_ADJUST_OPERATION_SUBSTRUCT_E],
                                            baseAddrOffset [0 / 0xFFFFFF] }.
            Expected: GT_OK.
        */
        tcamClient    = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E;
        clientKeyType = 0;
        lookupType    = CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E;
        lookupIndex   = 0;

        indexConvert.shiftOperation    = CPSS_SHIFT_OPERATION_SHIFT_LEFT_E;
        indexConvert.shiftValue        = 0;
        indexConvert.baseAddrOperation = CPSS_ADJUST_OPERATION_ADD_E;
        indexConvert.baseAddrOffset    = 0;

        st = cpssExMxPmExternalTcamHitIndexConvertSet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, &indexConvert);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex);

        /*
            1.2. Call cpssExMxPmExternalTcamHitIndexConvertGet
                        with the same tcamClient,
                             lookupIndex
                             and non NULL indexConvertPtr.
            Expected: GT_OK and the same indexConvertPtr.
        */
        st = cpssExMxPmExternalTcamHitIndexConvertGet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, &indexConvertGet);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamHitIndexConvertGet: %d, %d, %d, %d, %d",
                                     dev, tcamClient, clientKeyType, lookupType, lookupIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(indexConvert.shiftOperation, indexConvertGet.shiftOperation,
                                     "get another indexConvertPtr->shiftOperation then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(indexConvert.shiftValue, indexConvertGet.shiftValue,
                                     "get another indexConvertPtr->shiftValue then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(indexConvert.baseAddrOperation, indexConvertGet.baseAddrOperation,
                                     "get another indexConvertPtr->baseAddrOperation then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(indexConvert.baseAddrOffset, indexConvertGet.baseAddrOffset,
                                     "get another indexConvertPtr->baseAddrOffset then was set: %d", dev);

        /*
            1.1. Call with tcamClient [CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E / CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E],
                           clientKeyType [0 / 15],
                           lookupType [CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E / CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_DUAL_E]
                           lookupIndex [0 / 1],
                           indexConvertPtr {shiftOperation [CPSS_SHIFT_OPERATION_SHIFT_LEFT_E /
                                                            CPSS_SHIFT_OPERATION_SHIFT_RIGHT_E],
                                            shiftValue [0 / 3],
                                            baseAddrOperation [CPSS_ADJUST_OPERATION_ADD_E /
                                                            CPSS_ADJUST_OPERATION_SUBSTRUCT_E],
                                            baseAddrOffset [0 / 0xFFFFFF] }.
            Expected: GT_OK.
        */
        tcamClient    = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E;
        clientKeyType = 15;
        lookupType    = CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_DUAL_E;
        lookupIndex   = 1;

        indexConvert.shiftOperation    = CPSS_SHIFT_OPERATION_SHIFT_RIGHT_E;
        indexConvert.shiftValue        = 3;
        indexConvert.baseAddrOperation = CPSS_ADJUST_OPERATION_SUBSTRUCT_E;
        indexConvert.baseAddrOffset    = 0xFFFFFF;

        st = cpssExMxPmExternalTcamHitIndexConvertSet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, &indexConvert);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex);

        /*
            1.2. Call cpssExMxPmExternalTcamHitIndexConvertGet
                        with the same tcamClient,
                             lookupIndex
                             and non NULL indexConvertPtr.
            Expected: GT_OK and the same indexConvertPtr.
        */
        st = cpssExMxPmExternalTcamHitIndexConvertGet(dev, tcamClient, clientKeyType,
                                              lookupType, lookupIndex, &indexConvertGet);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamHitIndexConvertGet: %d, %d, %d, %d, %d",
                                     dev, tcamClient, clientKeyType, lookupType, lookupIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(indexConvert.shiftOperation, indexConvertGet.shiftOperation,
                                     "get another indexConvertPtr->shiftOperation then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(indexConvert.shiftValue, indexConvertGet.shiftValue,
                                     "get another indexConvertPtr->shiftValue then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(indexConvert.baseAddrOperation, indexConvertGet.baseAddrOperation,
                                     "get another indexConvertPtr->baseAddrOperation then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(indexConvert.baseAddrOffset, indexConvertGet.baseAddrOffset,
                                     "get another indexConvertPtr->baseAddrOffset then was set: %d", dev);

        /*
            1.3. Call with out of range tcamClient [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        tcamClient = 0x5AAAAAA5;

        st = cpssExMxPmExternalTcamHitIndexConvertSet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, &indexConvert);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, tcamClient, lookupIndex, clientKeyType);

        tcamClient = 0;

        /*
            1.4. Call with out of range lookupType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        lookupType = EXT_TCAM_INVALID_ENUM_CNS;

        st = cpssExMxPmExternalTcamHitIndexConvertSet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, &indexConvert);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, tcamClient, lookupIndex);

        lookupType = CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_DUAL_E;

        /*
            1.5. Call with out of range lookupIndex [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        lookupIndex = 2;

        st = cpssExMxPmExternalTcamHitIndexConvertSet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, &indexConvert);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, lookupIndex = %d", dev, lookupIndex);

        lookupIndex = 0;

        /*
            1.6. Call with out of range clientKeyType [16]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        clientKeyType = 16;

        st = cpssExMxPmExternalTcamHitIndexConvertSet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, &indexConvert);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, clientKeyType = %d", dev, clientKeyType);

        clientKeyType = 0;

        /*
            1.7. Call with out of range indexConvertPtr->shiftOperation [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        indexConvert.shiftOperation = 0x5AAAAAA5;

        st = cpssExMxPmExternalTcamHitIndexConvertSet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, &indexConvert);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, indexConvertPtr->shiftOperation = %d",
                                     dev, indexConvert.shiftOperation);

        indexConvert.shiftOperation = CPSS_SHIFT_OPERATION_SHIFT_RIGHT_E;

        /*
            1.8. Call with out of range indexConvertPtr->baseAddrOperation [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        indexConvert.baseAddrOperation = 0x5AAAAAA5;

        st = cpssExMxPmExternalTcamHitIndexConvertSet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, &indexConvert);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, indexConvertPtr->baseAddrOperation = %d",
                                     dev, indexConvert.baseAddrOperation);

        indexConvert.baseAddrOperation = CPSS_ADJUST_OPERATION_SUBSTRUCT_E;

        /*
            1.9. Call with indexConvertPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamHitIndexConvertSet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, indexConvertPtr = NULL", dev);
    }

    tcamClient    = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E;
    clientKeyType = 0;
    lookupType    = CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E;
    lookupIndex   = 0;

    indexConvert.shiftOperation    = CPSS_SHIFT_OPERATION_SHIFT_LEFT_E;
    indexConvert.shiftValue        = 0;
    indexConvert.baseAddrOperation = CPSS_ADJUST_OPERATION_ADD_E;
    indexConvert.baseAddrOffset    = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamHitIndexConvertSet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, &indexConvert);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamHitIndexConvertSet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, &indexConvert);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamHitIndexConvertGet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT            tcamClient,
    IN  GT_U32                                          clientKeyType,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_ENT       lookupType,
    IN  GT_U32                                          lookupIndex,
    OUT CPSS_EXMXPM_EXTERNAL_TCAM_HIT_INDEX_CONVERT_STC *indexConvertPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamHitIndexConvertGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with tcamClient [CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E / 
                               CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E],
                   clientKeyType [0 / 15]
                   lookupType [CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E / 
                               CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_DUAL_E]
                   lookupIndex [0 / 1],
                   non-NULL indexConvertPtr.
    Expected: GT_OK
    1.2. Call with out of range tcamClient [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range lookupIndex [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range clientKeyType [16]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range lookupType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with indexConvertPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/

    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                     tcamClient  = 0;
    GT_U32                                     lookupIndex = 0;
    CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_ENT  lookupType;
    GT_U32                                     clientKeyType = 0;
    CPSS_EXMXPM_EXTERNAL_TCAM_HIT_INDEX_CONVERT_STC indexConvert;


    cpssOsBzero((GT_VOID*) &indexConvert, sizeof(indexConvert));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tcamClient [CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E / CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E],
                           clientKeyType [0 / 15]
                           lookupType [CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E / CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_DUAL_E]
                           lookupIndex [0 / 1],
                           non-NULL indexConvertPtr.
            Expected: GT_OK
        */
        /* call with tcamClient = 0 */
        tcamClient    = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E;
        clientKeyType = 0;
        lookupType    = CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E;
        lookupIndex   = 0;

        st = cpssExMxPmExternalTcamHitIndexConvertGet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, &indexConvert);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex);

        /* call with tcamClient = 3 */
        tcamClient    = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E;
        clientKeyType = 15;
        lookupType    = CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_DUAL_E;
        lookupIndex   = 1;

        st = cpssExMxPmExternalTcamHitIndexConvertGet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, &indexConvert);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex);

        /*
            1.2. Call with out of range tcamClient [EXT_TCAM_INVALID_ENUM_CNS]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        tcamClient = EXT_TCAM_INVALID_ENUM_CNS;

        st = cpssExMxPmExternalTcamHitIndexConvertGet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, &indexConvert);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamClient);

        tcamClient = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E;

        /*
            1.3. Call with out of range lookupIndex [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        lookupIndex = 2;

        st = cpssExMxPmExternalTcamHitIndexConvertGet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, &indexConvert);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, lookupIndex = %d", dev, lookupIndex);

        lookupIndex = 0;

        /*
            1.4. Call with out of range clientKeyType [16]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        clientKeyType = 16;

        st = cpssExMxPmExternalTcamHitIndexConvertGet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, &indexConvert);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, clientKeyType = %d", dev, clientKeyType);

        clientKeyType = 0;

        /*
            1.5. Call with out of range lookupType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        lookupType = EXT_TCAM_INVALID_ENUM_CNS;

        st = cpssExMxPmExternalTcamHitIndexConvertGet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, &indexConvert);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, lookupType = %d", dev, lookupType);

        lookupType = CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_DUAL_E;

        /*
            1.6. Call with indexConvertPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamHitIndexConvertGet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, indexConvertPtr = NULL", dev);
    }

    tcamClient    = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E;
    clientKeyType = 0;
    lookupType    = CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E;
    lookupIndex   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamHitIndexConvertGet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, &indexConvert);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamHitIndexConvertGet(dev, tcamClient, clientKeyType,
                                                      lookupType, lookupIndex, &indexConvert);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamClientConfigSet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT        tcamClient,
    IN  GT_U32                                      clientKeyType,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_ENT   lookupType,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_CONFIG_STC *clientConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamClientConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with tcamClient [CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_0_E /
                               CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_1_E /
                               CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_2_E],
                   clientKeyType [0 / 15],
                   lookupType [CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E / CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_DUAL_E],
                   clientConfigPtr {tcamDeviceSelectBmp [0 / 0xFF],
                                    globalMask [0 / 7],
                                    lookupSchemeIndex [0 / 31]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmExternalTcamClientConfigGet
                with the same tcamClient,
                     clientKeyType,
                     lookupType
                     and not NULL clientConfigPtr.
    Expected: GT_OK and the same clientConfigPtr.
    1.3. Call with out of range tcamClient [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range clientKeyType [16]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range lookupType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with clientConfigPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT        tcamClient    = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E;
    GT_U32                                      clientKeyType = 0;
    CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_ENT   lookupType    = CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E;
    CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_CONFIG_STC clientConfig;
    CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_CONFIG_STC clientConfigGet;


    cpssOsBzero((GT_VOID*) &clientConfig, sizeof(clientConfig));
    cpssOsBzero((GT_VOID*) &clientConfigGet, sizeof(clientConfigGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tcamClient [CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E /
                                        CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E /
                                        CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E],
                           clientKeyType [0 / 15],
                           lookupType  [CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E /
                                        CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_DUAL_E],
                           clientConfigPtr {tcamDeviceSelectBmp [0 / 0xFF],
                                            globalMask [0 / 7],
                                            lookupSchemeIndex [0 / 31]}.
        */

        /* call with tcamClient = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_0_E */
        tcamClient    = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E;
        clientKeyType = 0;
        lookupType    = CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E;

        clientConfig.tcamDeviceSelectBmp = 0;
        clientConfig.globalMask          = 0;
        clientConfig.lookupSchemeIndex   = 0;

        st = cpssExMxPmExternalTcamClientConfigSet(dev, tcamClient, clientKeyType, lookupType, &clientConfig);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, tcamClient, clientKeyType, lookupType);

        /*
            1.2. Call cpssExMxPmExternalTcamClientConfigGet
                        with the same tcamClient,
                             clientKeyType
                             and not NULL clientConfigPtr.
            Expected: GT_OK and the same clientConfigPtr.
        */
        st = cpssExMxPmExternalTcamClientConfigGet(dev, tcamClient, clientKeyType, lookupType, &clientConfigGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamClientConfigGet: %d, %d, %d, %d",
                                     dev, tcamClient, clientKeyType, lookupType);

        /* Validation */
        UTF_VERIFY_EQUAL1_STRING_MAC(clientConfig.tcamDeviceSelectBmp, clientConfigGet.tcamDeviceSelectBmp,
                                     "get another  clientConfigPtr->tcamDeviceSelectBmp then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(clientConfig.globalMask, clientConfigGet.globalMask,
                                     "get another  clientConfigPtr->globalMask then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(clientConfig.lookupSchemeIndex, clientConfigGet.lookupSchemeIndex,
                                     "get another  clientConfigPtr->lookupSchemeIndex then was set: %d", dev);

        /* call with tcamClient = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E */
        tcamClient    = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E;
        clientKeyType = 15;
        lookupType    = CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_DUAL_E;

        clientConfig.tcamDeviceSelectBmp = 0xFF;
        clientConfig.globalMask          = 7;
        clientConfig.lookupSchemeIndex   = 31;

        st = cpssExMxPmExternalTcamClientConfigSet(dev, tcamClient, clientKeyType, lookupType, &clientConfig);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, tcamClient, clientKeyType, lookupType);

        /*
            1.2. Call cpssExMxPmExternalTcamClientConfigGet
                        with the same tcamClient,
                             clientKeyType
                             and not NULL clientConfigPtr.
            Expected: GT_OK and the same clientConfigPtr.
        */
        st = cpssExMxPmExternalTcamClientConfigGet(dev, tcamClient, clientKeyType, lookupType, &clientConfigGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamClientConfigGet: %d, %d, %d, %d",
                                     dev, tcamClient, clientKeyType, lookupType);

        /* Validation */
        UTF_VERIFY_EQUAL1_STRING_MAC(clientConfig.tcamDeviceSelectBmp, clientConfigGet.tcamDeviceSelectBmp,
                                     "get another  clientConfigPtr->tcamDeviceSelectBmp then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(clientConfig.globalMask, clientConfigGet.globalMask,
                                     "get another  clientConfigPtr->globalMask then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(clientConfig.lookupSchemeIndex, clientConfigGet.lookupSchemeIndex,
                                     "get another  clientConfigPtr->lookupSchemeIndex then was set: %d", dev);

        /* call with tcamClient = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E */
        tcamClient    = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E;
        clientKeyType = 0;
        lookupType    = CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E;

        clientConfig.tcamDeviceSelectBmp = 0;
        clientConfig.globalMask          = 0;
        clientConfig.lookupSchemeIndex   = 0;

        st = cpssExMxPmExternalTcamClientConfigSet(dev, tcamClient, clientKeyType, lookupType, &clientConfig);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, tcamClient, clientKeyType, lookupType);

        /*
            1.2. Call cpssExMxPmExternalTcamClientConfigGet
                        with the same tcamClient,
                             clientKeyType
                             and not NULL clientConfigPtr.
            Expected: GT_OK and the same clientConfigPtr.
        */
        st = cpssExMxPmExternalTcamClientConfigGet(dev, tcamClient, clientKeyType, lookupType, &clientConfigGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamClientConfigGet: %d, %d, %d, %d",
                                     dev, tcamClient, clientKeyType, lookupType);

        /* Validation */
        UTF_VERIFY_EQUAL1_STRING_MAC(clientConfig.tcamDeviceSelectBmp, clientConfigGet.tcamDeviceSelectBmp,
                                     "get another  clientConfigPtr->tcamDeviceSelectBmp then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(clientConfig.globalMask, clientConfigGet.globalMask,
                                     "get another  clientConfigPtr->globalMask then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(clientConfig.lookupSchemeIndex, clientConfigGet.lookupSchemeIndex,
                                     "get another  clientConfigPtr->lookupSchemeIndex then was set: %d", dev);

        /*
            1.3. Call with out of range tcamClient [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        tcamClient = 0x5AAAAAA5;

        st = cpssExMxPmExternalTcamClientConfigSet(dev, tcamClient, clientKeyType, lookupType, &clientConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, tcamClient);

        tcamClient = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E;

        /*
            1.4. Call with out of range clientKeyType [16]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        clientKeyType = 16;

        st = cpssExMxPmExternalTcamClientConfigSet(dev, tcamClient, clientKeyType, lookupType, &clientConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, clientKeyType = %d", dev, clientKeyType);

        clientKeyType = 15;

        /*
            1.5. Call with out of range lookupType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        lookupType = EXT_TCAM_INVALID_ENUM_CNS;

        st = cpssExMxPmExternalTcamClientConfigSet(dev, tcamClient, clientKeyType, lookupType, &clientConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, lookupType);

        lookupType = CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_DUAL_E;

        /*
            1.6. Call with clientConfigPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamClientConfigSet(dev, tcamClient, clientKeyType, lookupType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, clientConfigPtr = NULL", dev);
    }

    tcamClient    = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E;
    clientKeyType = 0;
    lookupType    = CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E;

    clientConfig.tcamDeviceSelectBmp = 0;
    clientConfig.globalMask          = 0;
    clientConfig.lookupSchemeIndex   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamClientConfigSet(dev, tcamClient, clientKeyType, lookupType, &clientConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamClientConfigSet(dev, tcamClient, clientKeyType, lookupType, &clientConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamClientConfigGet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT        tcamClient,
    IN  GT_U32                                      clientKeyType,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_ENT   lookupType,
    OUT CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_CONFIG_STC *clientConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamClientConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with tcamClient [CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E /
                              CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E /
                              CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E],
                   clientKeyType [0 / 15]
                   lookupType [CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E / 
                               CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_DUAL_E],
                   and not-NULL clientConfigPtr.
    1.3. Call with out of range tcamClient [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM
    1.4. Call with out of range lookupType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range clientKeyType [16]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with clientConfigPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/

    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT        tcamClient  = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E;
    CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_ENT   lookupType;
    GT_U32                                      clientKeyType = 0;
    CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_CONFIG_STC clientConfig;


    cpssOsBzero((GT_VOID*) &clientConfig, sizeof(clientConfig));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tcamClient [CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E /
                                      CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E /
                                      CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E],
                           clientKeyType [0 / 15]
                           lookupType [CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E / CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_DUAL_E],
                           and not-NULL clientConfigPtr.
        */
        /* call with tcamClient = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E */
        tcamClient    = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E;
        clientKeyType = 0;
        lookupType    = CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E;

        st = cpssExMxPmExternalTcamClientConfigGet(dev, tcamClient, clientKeyType, lookupType, &clientConfig);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, tcamClient, clientKeyType, lookupType);

        /* call with tcamClient = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E */
        tcamClient = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E;
        lookupType = CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_DUAL_E;

        st = cpssExMxPmExternalTcamClientConfigGet(dev, tcamClient, clientKeyType, lookupType, &clientConfig);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, tcamClient, clientKeyType, lookupType);

        /* call with tcamClient = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E */
        tcamClient    = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E;
        clientKeyType = 15;
        lookupType    = CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E;

        st = cpssExMxPmExternalTcamClientConfigGet(dev, tcamClient, clientKeyType, lookupType, &clientConfig);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, tcamClient, clientKeyType, lookupType);

        /*
            1.3. Call with out of range tcamClient [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM
        */
        tcamClient = 0x5AAAAAA5;

        st = cpssExMxPmExternalTcamClientConfigGet(dev, tcamClient, clientKeyType, lookupType, &clientConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, tcamClient);

        tcamClient = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E;

        /*
            1.4. Call with out of range lookupType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        lookupType = EXT_TCAM_INVALID_ENUM_CNS;

        st = cpssExMxPmExternalTcamClientConfigGet(dev, tcamClient, clientKeyType, lookupType, &clientConfig);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, lookupType = %d", dev, lookupType);

        lookupType = CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_DUAL_E;

        /*
            1.5. Call with out of range clientKeyType [16]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        clientKeyType = 16;

        st = cpssExMxPmExternalTcamClientConfigGet(dev, tcamClient, clientKeyType, lookupType, &clientConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, clientKeyType = %d", dev, clientKeyType);

        clientKeyType = 15;

        /*
            1.6. Call with clientConfigPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamClientConfigGet(dev, tcamClient, clientKeyType, lookupType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, clientConfigPtr = NULL", dev);
    }

    tcamClient    = CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E;
    clientKeyType = 0;
    lookupType    = CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_SINGLE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamClientConfigGet(dev, tcamClient, clientKeyType, lookupType, &clientConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamClientConfigGet(dev, tcamClient, clientKeyType, lookupType, &clientConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamActionWrite
(
    IN  GT_U8                               devNum,
    IN  CPSS_EXMXPM_CONTROL_MEMORY_ENT      actionCsu,
    IN  GT_U32                              lineStartIndex,
    IN  GT_U32                              actionSize,
    IN  GT_U32                              *actionDataPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamActionWrite)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with actionCsu[CPSS_EXMXPM_CONTROL_MEMORY_0_E],
                   lineStartIndex [1],
                   actionSize [3*32],
                   actionData [10] {1,2,3}
    Expected: GT_OK.
    1.2. Call cpssExMxPmExternalTcamActionRead with not NULL actionDataPtr and other params from 1.1.
    Expected: GT_OK and the same actionDataPtr as was set.
    1.3. Call with actionCsu [CPSS_EXMXPM_CONTROL_MEMORY_1_E]
    Expected: NOT GT_OK.
    1.4. Call with actionData [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_BOOL     isEqual = GT_FALSE;
    CPSS_EXMXPM_CONTROL_MEMORY_ENT      actionCsu = CPSS_EXMXPM_CONTROL_MEMORY_0_E;
    GT_U32                              lineStartIndex = 0;
    GT_U32                              actionSize = 0;
    GT_U32                              actionData[7];
    GT_U32                              actionDataGet[7];


    cpssOsBzero((GT_VOID*) &actionData, sizeof(actionData));
    cpssOsBzero((GT_VOID*) &actionDataGet, sizeof(actionDataGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with actionCsu[CPSS_EXMXPM_CONTROL_MEMORY_0_E],
                           lineStartIndex [1],
                           actionSize [3],
                           actionData [10] {1,2,3}
            Expected: GT_OK.
        */
        actionCsu = CPSS_EXMXPM_CONTROL_MEMORY_0_E;
        lineStartIndex = 1;
        actionSize = 213;

        actionData[0] = 1;
        actionData[1] = 2;
        actionData[2] = 3;
        actionData[3] = 4;
        actionData[4] = 5;
        actionData[5] = 6;
        actionData[6] = 7;

        st = cpssExMxPmExternalTcamActionWrite(dev, actionCsu, lineStartIndex, actionSize, actionData);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, actionCsu, lineStartIndex, actionSize);

        /*
            1.2. Call cpssExMxPmExternalTcamActionRead with not NULL actionDataPtr and other params from 1.1.
            Expected: GT_OK and the same actionDataPtr as was set.
        */
        st = cpssExMxPmExternalTcamActionRead(dev, actionCsu, lineStartIndex, actionSize, actionDataGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamActionRead: %d, %d, %d, %d",
                                    dev, actionCsu, lineStartIndex, actionSize);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) actionData, (GT_VOID*) actionDataGet, sizeof(actionData) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another actionData than was set", dev);

        /*
            1.3. Call with actionCsu [CPSS_EXMXPM_CONTROL_MEMORY_1_E]
            Expected: NOT GT_OK.
        */
        actionCsu = CPSS_EXMXPM_CONTROL_MEMORY_1_E;

        st = cpssExMxPmExternalTcamActionWrite(dev, actionCsu, lineStartIndex, actionSize, actionData);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, actionCsu);

        actionCsu = CPSS_EXMXPM_CONTROL_MEMORY_0_E;

        /*
            1.4. Call with actionData [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamActionWrite(dev, actionCsu, lineStartIndex, actionSize, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionDataPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamActionWrite(dev, actionCsu, lineStartIndex, actionSize, actionData);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamActionWrite(dev, actionCsu, lineStartIndex, actionSize, actionData);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamActionRead
(
    IN  GT_U8                               devNum,
    IN  CPSS_EXMXPM_CONTROL_MEMORY_ENT      actionCsu,
    IN  GT_U32                              lineStartIndex,
    IN  GT_U32                              actionSize,
    OUT GT_U32                              *actionDataPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamActionRead)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with actionCsu[CPSS_EXMXPM_CONTROL_MEMORY_0_E],
                   lineStartIndex [1],
                   actionSize [3],
                   not NULL actionDataPtr.
    Expected: GT_OK.
    1.2. Call with actionCsu [CPSS_EXMXPM_CONTROL_MEMORY_1_E]
    Expected: NOT GT_OK.
    1.3. Call with actionData [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_CONTROL_MEMORY_ENT      actionCsu;
    GT_U32                              lineStartIndex;
    GT_U32                              actionSize;
    GT_U32                              actionData[3];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with actionCsu[CPSS_EXMXPM_CONTROL_MEMORY_0_E],
                           lineStartIndex [1],
                           actionSize [3],
                           not NULL actionDataPtr.
            Expected: GT_OK.
        */
        actionCsu = CPSS_EXMXPM_CONTROL_MEMORY_0_E;
        lineStartIndex = 1;
        actionSize = 3;

        st = cpssExMxPmExternalTcamActionRead(dev, actionCsu, lineStartIndex, actionSize, actionData);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, actionCsu, lineStartIndex, actionSize);

        /*
            1.2. Call with actionCsu [CPSS_EXMXPM_CONTROL_MEMORY_1_E]
            Expected: NOT GT_OK.
        */
        actionCsu = CPSS_EXMXPM_CONTROL_MEMORY_1_E;

        st = cpssExMxPmExternalTcamActionRead(dev, actionCsu, lineStartIndex, actionSize, actionData);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, actionCsu);

        actionCsu = CPSS_EXMXPM_CONTROL_MEMORY_0_E;

        /*
            1.3. Call with actionData [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamActionRead(dev, actionCsu, lineStartIndex, actionSize, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionDataPtr = NULL" ,dev);
    }

    actionCsu = CPSS_EXMXPM_CONTROL_MEMORY_0_E;
    lineStartIndex = 1;
    actionSize = 3;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamActionRead(dev, actionCsu, lineStartIndex, actionSize, actionData);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamActionRead(dev, actionCsu, lineStartIndex, actionSize, actionData);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamIndirectCmdStart
(
    IN  GT_U8   devNum
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamIndirectCmdStart)
{
/*
    ITERATE_DEVICES (ExMxPm)
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = cpssExMxPmExternalTcamIndirectCmdStart(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamIndirectCmdStart(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamIndirectCmdStart(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamIndirectCmdStop
(
    IN  GT_U8   devNum
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamIndirectCmdStop)
{
/*
    ITERATE_DEVICES (ExMxPm)
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = cpssExMxPmExternalTcamIndirectCmdStop(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamIndirectCmdStop(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamIndirectCmdStop(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamIndirectCmdStatusGet
(
    IN  GT_U8      devNum,
    OUT GT_BOOL    *completedPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamIndirectCmdStatusGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL completedPtr
    Expected: GT_OK.
    1.2. Call with completedPtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL    completed = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL completedPtr
            Expected: GT_OK.
        */
        st = cpssExMxPmExternalTcamIndirectCmdStatusGet(dev, &completed);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with completedPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamIndirectCmdStatusGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, completedPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamIndirectCmdStatusGet(dev, &completed);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamIndirectCmdStatusGet(dev, &completed);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamIndirectCmdParamsSet
(
    IN  GT_U8                                               devNum,
    IN  GT_U32                                              tcamId,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_OP_CODE_ENT               cmdOpCode,
    IN  GT_U32                                              lineStartIndexOrRegAddr,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_INDIRECT_WRITE_DATA_UNT   *writeCmdDataPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamIndirectCmdParamsSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with  tcamId [0 / 3],
                    cmdOpCode [CPSS_EXMXPM_EXTERNAL_TCAM_OP_CODE_WRITE_TCAM_ENTRY_E / CPSS_EXMXPM_EXTERNAL_TCAM_OP_CODE_READ_TCAM_ENTRY_E],
                    lineStartIndexOrRegAddr [0],
                    writeCmdData {tcamRegData [5] {1,2,3,4,5}
                                  tcamEntryInfo {   dataFormat [CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E];
                                                    valid [GT_FALSE],
                                                    ageEnable [GT_FALSE],
                                                    patternOrX [3] {1,2,3}
                                                    maskOrY [3] {1,2,3} } }
    Expected: GT_OK.
    1.2. Call cpssExMxPmExternalTcamIndirectCmdParamsGet with not NULL pointers
    Expected:GT_OK and the same params as was set.
    1.3. Call with tcamId [4]
    Expected: NOT GT_OK.
    1.4. Call with cmdOpCode [0x5AAAAAA5] (out of range)
    Expected: GT_BAD_PARAM.
    1.5. Call with writeCmdData->tcamEntryInfo.dataFormat [0x5AAAAAA5] (out of range)
    Expected: GT_BAD_PARAM.
    1.6. Call with writeCmdDataPtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    
    GT_U32                                              tcamId = 0;
    CPSS_EXMXPM_EXTERNAL_TCAM_OP_CODE_ENT               cmdOpCode = CPSS_EXMXPM_EXTERNAL_TCAM_OP_CODE_NOP_E;
    GT_U32                                              lineStartIndexOrRegAddr = 0;
    CPSS_EXMXPM_EXTERNAL_TCAM_INDIRECT_WRITE_DATA_UNT   writeCmdData;

    GT_U32                                              tcamIdGet = 0;
    CPSS_EXMXPM_EXTERNAL_TCAM_OP_CODE_ENT               cmdOpCodeGet = CPSS_EXMXPM_EXTERNAL_TCAM_OP_CODE_NOP_E;
    GT_U32                                              lineStartIndexOrRegAddrGet = 0;
    CPSS_EXMXPM_EXTERNAL_TCAM_INDIRECT_WRITE_DATA_UNT   writeCmdDataGet;


    cpssOsBzero((GT_VOID*) &writeCmdData, sizeof(writeCmdData));
    cpssOsBzero((GT_VOID*) &writeCmdDataGet, sizeof(writeCmdDataGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with  tcamId [0],
                            cmdOpCode [CPSS_EXMXPM_EXTERNAL_TCAM_OP_CODE_WRITE_TCAM_ENTRY_E / CPSS_EXMXPM_EXTERNAL_TCAM_OP_CODE_READ_TCAM_ENTRY_E],
                            lineStartIndexOrRegAddr [0],
                            writeCmdData { tcamRegData [3] {0,1,2}
                                           tcamEntryInfo {   dataFormat [CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E];
                                                            valid [GT_FALSE],
                                                            ageEnable [GT_FALSE],
                                                            patternOrX [3] {1,2,3}
                                                            maskOrY [3] {1,2,3} } }
            Expected: GT_OK.
        */
        /* iterate with tcamId = 0 */
        tcamId = 0;
        cmdOpCode = CPSS_EXMXPM_EXTERNAL_TCAM_OP_CODE_WRITE_TCAM_ENTRY_E;
        lineStartIndexOrRegAddr = 0;
        writeCmdData.tcamRegData[0] = 0;
        writeCmdData.tcamRegData[1] = 1;
        writeCmdData.tcamRegData[2] = 2;
        writeCmdData.tcamEntryInfo.dataFormat = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E;
        writeCmdData.tcamEntryInfo.valid = GT_TRUE;
        writeCmdData.tcamEntryInfo.ageEnable = GT_FALSE;
        writeCmdData.tcamEntryInfo.patternOrX[0] = 1;
        writeCmdData.tcamEntryInfo.patternOrX[1] = 2;
        writeCmdData.tcamEntryInfo.patternOrX[2] = 3;
        writeCmdData.tcamEntryInfo.maskOrY[0] = 1;
        writeCmdData.tcamEntryInfo.maskOrY[1] = 2;
        writeCmdData.tcamEntryInfo.maskOrY[2] = 3;

        st = cpssExMxPmExternalTcamIndirectCmdParamsSet(dev, tcamId, cmdOpCode, lineStartIndexOrRegAddr, &writeCmdData);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, tcamId, cmdOpCode, lineStartIndexOrRegAddr);

        /*
            1.2. Call cpssExMxPmExternalTcamIndirectCmdParamsGet with not NULL pointers
            Expected:GT_OK and the same params as was set.
        */
        st = cpssExMxPmExternalTcamIndirectCmdParamsGet(dev, &tcamIdGet, &cmdOpCodeGet, &lineStartIndexOrRegAddrGet, &writeCmdDataGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamIndirectCmdParamsGet: %d", dev);

        /* verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(tcamId, tcamIdGet, "got another tcamId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cmdOpCode, cmdOpCodeGet, "got another cmdOpCode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(lineStartIndexOrRegAddr, lineStartIndexOrRegAddrGet,
                                     "got another lineStartIndexOrRegAddr than was set: %d", dev);
        
        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamRegData[0], writeCmdDataGet.tcamRegData[0], 
                                     "got another writeCmdData.tcamRegData[0] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamRegData[1], writeCmdDataGet.tcamRegData[1], 
                                     "got another writeCmdData.tcamRegData[1] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamRegData[2], writeCmdDataGet.tcamRegData[2], 
                                     "got another writeCmdData.tcamRegData[2] than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamEntryInfo.dataFormat, writeCmdDataGet.tcamEntryInfo.dataFormat, 
                                     "got another writeCmdData.tcamEntryInfo.dataFormat than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamEntryInfo.valid, writeCmdDataGet.tcamEntryInfo.valid, 
                                     "got another writeCmdData.tcamEntryInfo.valid than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamEntryInfo.ageEnable, writeCmdDataGet.tcamEntryInfo.ageEnable, 
                                     "got another writeCmdData.tcamEntryInfo.ageEnable than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamEntryInfo.patternOrX[0], writeCmdDataGet.tcamEntryInfo.patternOrX[0], 
                                     "got another writeCmdData.tcamEntryInfo.patternOrX[0] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamEntryInfo.patternOrX[1], writeCmdDataGet.tcamEntryInfo.patternOrX[1], 
                                     "got another writeCmdData.tcamEntryInfo.patternOrX[1] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamEntryInfo.patternOrX[2], writeCmdDataGet.tcamEntryInfo.patternOrX[2], 
                                     "got another writeCmdData.tcamEntryInfo.patternOrX[2] than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamEntryInfo.maskOrY[0], writeCmdDataGet.tcamEntryInfo.maskOrY[0], 
                                     "got another writeCmdData.tcamEntryInfo.maskOrY[0] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamEntryInfo.maskOrY[1], writeCmdDataGet.tcamEntryInfo.maskOrY[1], 
                                     "got another writeCmdData.tcamEntryInfo.maskOrY[1] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamEntryInfo.maskOrY[2], writeCmdDataGet.tcamEntryInfo.maskOrY[2], 
                                     "got another writeCmdData.tcamEntryInfo.maskOrY[2] than was set: %d", dev);

        /* iterate with tcamId = 0 */
        tcamId = 0;
        cmdOpCode = CPSS_EXMXPM_EXTERNAL_TCAM_OP_CODE_WRITE_REG_E;

        writeCmdData.tcamEntryInfo.dataFormat = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_DATA_MASK_OR_LOGIC_E;
        writeCmdData.tcamEntryInfo.valid = GT_FALSE;
        writeCmdData.tcamEntryInfo.ageEnable = GT_TRUE;

        st = cpssExMxPmExternalTcamIndirectCmdParamsSet(dev, tcamId, cmdOpCode, lineStartIndexOrRegAddr, &writeCmdData);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, tcamId, cmdOpCode, lineStartIndexOrRegAddr);

        /*
            1.2. Call cpssExMxPmExternalTcamIndirectCmdParamsGet with not NULL pointers
            Expected:GT_OK and the same params as was set.
        */
        st = cpssExMxPmExternalTcamIndirectCmdParamsGet(dev, &tcamIdGet, &cmdOpCodeGet, &lineStartIndexOrRegAddrGet, &writeCmdDataGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamIndirectCmdParamsGet: %d", dev);

        /* verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(tcamId, tcamIdGet, "got another tcamId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cmdOpCode, cmdOpCodeGet, "got another cmdOpCode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(lineStartIndexOrRegAddr, lineStartIndexOrRegAddrGet,
                                     "got another lineStartIndexOrRegAddr than was set: %d", dev);
        
        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamRegData[0], writeCmdDataGet.tcamRegData[0], 
                                     "got another writeCmdData.tcamRegData[0] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamRegData[1], writeCmdDataGet.tcamRegData[1], 
                                     "got another writeCmdData.tcamRegData[1] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamRegData[2], writeCmdDataGet.tcamRegData[2], 
                                     "got another writeCmdData.tcamRegData[2] than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamEntryInfo.dataFormat, writeCmdDataGet.tcamEntryInfo.dataFormat, 
                                     "got another writeCmdData.tcamEntryInfo.dataFormat than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamEntryInfo.valid, writeCmdDataGet.tcamEntryInfo.valid, 
                                     "got another writeCmdData.tcamEntryInfo.valid than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamEntryInfo.ageEnable, writeCmdDataGet.tcamEntryInfo.ageEnable, 
                                     "got another writeCmdData.tcamEntryInfo.ageEnable than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamEntryInfo.patternOrX[0], writeCmdDataGet.tcamEntryInfo.patternOrX[0], 
                                     "got another writeCmdData.tcamEntryInfo.patternOrX[0] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamEntryInfo.patternOrX[1], writeCmdDataGet.tcamEntryInfo.patternOrX[1], 
                                     "got another writeCmdData.tcamEntryInfo.patternOrX[1] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamEntryInfo.patternOrX[2], writeCmdDataGet.tcamEntryInfo.patternOrX[2], 
                                     "got another writeCmdData.tcamEntryInfo.patternOrX[2] than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamEntryInfo.maskOrY[0], writeCmdDataGet.tcamEntryInfo.maskOrY[0], 
                                     "got another writeCmdData.tcamEntryInfo.maskOrY[0] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamEntryInfo.maskOrY[1], writeCmdDataGet.tcamEntryInfo.maskOrY[1], 
                                     "got another writeCmdData.tcamEntryInfo.maskOrY[1] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(writeCmdData.tcamEntryInfo.maskOrY[2], writeCmdDataGet.tcamEntryInfo.maskOrY[2], 
                                     "got another writeCmdData.tcamEntryInfo.maskOrY[2] than was set: %d", dev);

        /*
            1.3. Call with tcamId [4]
            Expected: NOT GT_OK.
        */
        tcamId = 4;

        st = cpssExMxPmExternalTcamIndirectCmdParamsSet(dev, tcamId, cmdOpCode, lineStartIndexOrRegAddr, &writeCmdData);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamId);

        tcamId = 0;

        /*
            1.4. Call with cmdOpCode [0x5AAAAAA5]
            Expected: GT_BAD_PARAM.
        */
        cmdOpCode = EXT_TCAM_INVALID_ENUM_CNS;

        st = cpssExMxPmExternalTcamIndirectCmdParamsSet(dev, tcamId, cmdOpCode, lineStartIndexOrRegAddr, &writeCmdData);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, cmdOpCode = %d", dev, cmdOpCode);

        cmdOpCode = CPSS_EXMXPM_EXTERNAL_TCAM_OP_CODE_WRITE_TCAM_ENTRY_E;

        /*
            1.5. Call with writeCmdData->tcamEntryInfo.dataFormat [0x5AAAAAA5] (out of range)
            Expected: GT_BAD_PARAM.
        */
        writeCmdData.tcamEntryInfo.dataFormat = EXT_TCAM_INVALID_ENUM_CNS;

        st = cpssExMxPmExternalTcamIndirectCmdParamsSet(dev, tcamId, cmdOpCode, lineStartIndexOrRegAddr, &writeCmdData);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, writeCmdData.tcamEntryInfo.dataFormat = %d", dev, writeCmdData.tcamEntryInfo.dataFormat);

        writeCmdData.tcamEntryInfo.dataFormat = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E;

        /*
            1.6. Call with writeCmdDataPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamIndirectCmdParamsSet(dev, tcamId, cmdOpCode, lineStartIndexOrRegAddr, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, writeCmdDataPtr = %d", dev);
    }

    tcamId = 0;
    cmdOpCode = CPSS_EXMXPM_EXTERNAL_TCAM_OP_CODE_WRITE_TCAM_ENTRY_E;
    lineStartIndexOrRegAddr = 0;
    writeCmdData.tcamRegData[0] = 0;
    writeCmdData.tcamRegData[1] = 1;
    writeCmdData.tcamRegData[2] = 2;
    writeCmdData.tcamEntryInfo.dataFormat = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E;
    writeCmdData.tcamEntryInfo.valid = GT_TRUE;
    writeCmdData.tcamEntryInfo.ageEnable = GT_FALSE;
    writeCmdData.tcamEntryInfo.patternOrX[0] = 1;
    writeCmdData.tcamEntryInfo.patternOrX[1] = 2;
    writeCmdData.tcamEntryInfo.patternOrX[2] = 3;
    writeCmdData.tcamEntryInfo.patternOrX[3] = 4;
    writeCmdData.tcamEntryInfo.maskOrY[0] = 1;
    writeCmdData.tcamEntryInfo.maskOrY[1] = 2;
    writeCmdData.tcamEntryInfo.maskOrY[2] = 3;
    writeCmdData.tcamEntryInfo.maskOrY[3] = 4;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamIndirectCmdParamsSet(dev, tcamId, cmdOpCode, lineStartIndexOrRegAddr, &writeCmdData);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamIndirectCmdParamsSet(dev, tcamId, cmdOpCode, lineStartIndexOrRegAddr, &writeCmdData);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamIndirectCmdParamsGet
(
    IN  GT_U8                                               devNum,
    OUT  GT_U32                                             *tcamIdPtr,
    OUT  CPSS_EXMXPM_EXTERNAL_TCAM_OP_CODE_ENT              *cmdOpCodePtr,
    OUT  GT_U32                                             *lineStartIndexOrRegAddrPtr,
    OUT  CPSS_EXMXPM_EXTERNAL_TCAM_INDIRECT_WRITE_DATA_UNT  *writeCmdDataPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamIndirectCmdParamsGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL pointers.
    Expected: GT_OK.
    1.2. Call with tcamIdPtr [NULL]
    Expected: GT_BAD_PTR.
    1.3. Call with cmdOpCodePtr [NULL]
    Expected: GT_BAD_PTR.
    1.4. Call with lineStartIndexOrRegAddrPtr [NULL]
    Expected: GT_BAD_PTR.
    1.5. Call with writeCmdDataPtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32                                             tcamId;
    CPSS_EXMXPM_EXTERNAL_TCAM_OP_CODE_ENT              cmdOpCode;
    GT_U32                                             lineStartIndexOrRegAddr;
    CPSS_EXMXPM_EXTERNAL_TCAM_INDIRECT_WRITE_DATA_UNT  writeCmdData;


    cpssOsBzero((GT_VOID*) &writeCmdData, sizeof(writeCmdData));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL pointers.
            Expected: GT_OK.
        */
        st = cpssExMxPmExternalTcamIndirectCmdParamsGet(dev, &tcamId, &cmdOpCode, &lineStartIndexOrRegAddr, &writeCmdData);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with tcamIdPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamIndirectCmdParamsGet(dev, NULL, &cmdOpCode, &lineStartIndexOrRegAddr, &writeCmdData);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, tcamIdPtr = NULL", dev);

        /*
            1.3. Call with cmdOpCodePtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamIndirectCmdParamsGet(dev, &tcamId, NULL, &lineStartIndexOrRegAddr, &writeCmdData);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cmdOpCodePtr = NULL", dev);

        /*
            1.4. Call with lineStartIndexOrRegAddrPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamIndirectCmdParamsGet(dev, &tcamId, &cmdOpCode, NULL, &writeCmdData);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, lineStartIndexOrRegAddrPtr = NULL", dev);

        /* Set command in write */
        tcamId = 0;

        cmdOpCode = CPSS_EXMXPM_EXTERNAL_TCAM_OP_CODE_WRITE_TCAM_ENTRY_E;
        
        st = cpssExMxPmExternalTcamIndirectCmdParamsSet(dev, tcamId, cmdOpCode, lineStartIndexOrRegAddr, &writeCmdData);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmExternalTcamIndirectCmdParamsSet %d, %d, %d", dev, tcamId, cmdOpCode);

        /*
            1.5. Call with writeCmdDataPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamIndirectCmdParamsGet(dev, &tcamId, &cmdOpCode, &lineStartIndexOrRegAddr, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, writeCmdDataPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamIndirectCmdParamsGet(dev, &tcamId, &cmdOpCode, &lineStartIndexOrRegAddr, &writeCmdData);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamIndirectCmdParamsGet(dev, &tcamId, &cmdOpCode, &lineStartIndexOrRegAddr, &writeCmdData);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamIndirectCmdGenericParamsSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_GENERIC_CMD_INST_UNT  *instructionsPtr,
    IN  GT_U32                                          numOfNops,
    IN  GT_U32                                          numOfTcamCyclesResult,
    IN  GT_U32                                          numOfMailboxCyclesResult,
    IN  GT_U32                                          dataLengthInTcamLines,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_REG                   *dataPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamIndirectCmdGenericParamsSet)
{
/*
    ITERATE_DEVICES (ExMxPm)

    1.1. Call cpssExMxPmExternalTcamTypeGet to get Tcam Type with tcamId [0].
    Expected: GT_OK and tcamTypePtr.
    1.2. Call with  for tcamType [CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_NL_E]
                        instructions {nlInstructionSet{dibus_low[0 / 0x1F],
                                                      dibus_high[0 / 0x1F],
                                                      dctx_init[0 / 0x3FF]} },
                    for tcamType [CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_IDT_E]
                        instructions {nlInstructionSet{ ltin0 [0 / 0x7],
                                                        inst0 [0 / 0x7],
                                                        ss_sel0 [0 / 0x1F],
                                                        ltin1 [0 / 0x7],
                                                        inst1 [0 / 0x7],
                                                        ss_sel1 [0 / 0x1F]} },
                    numOfNops [0 / 10],
                    numOfTcamCyclesResult [0 / 8],
                    numOfMailboxCyclesResult [0 / 4],
                    dataLengthInTcamLines [0 / 8],
                    data [3] {1,2,3},
    Expected: GT_OK.
    1.3. Call cpssExMxPmExternalTcamIndirectCmdGenericParamsGet with not NULL pointers
    Expected: GT_OK and the same params as was set.
    1.4. Call numOfTcamCyclesResult [9] (out of range)
    Expected: NOT GT_OK.
    1.5. Call numOfMailboxCyclesResult [5] (out of range)
    Expected: NOT GT_OK.
    1.6. Call dataLengthInTcamLines [9] (out of range)
    Expected: NOT GT_OK.
    1.7. Call with instructionsPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.8. Call with dataPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_BOOL     isEqual = GT_FALSE;

    CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_ENT              tcamType = CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_IDT_E;

    GT_U32                                          cmdDataLength = 0;
    CPSS_EXMXPM_EXTERNAL_TCAM_GENERIC_CMD_INST_UNT  instructions;
    GT_U32                                          numOfNops = 0;
    GT_U32                                          numOfTcamCyclesResult = 0;
    GT_U32                                          numOfMailboxCyclesResult = 0;
    GT_U32                                          dataLengthInTcamLines = 0;
    CPSS_EXMXPM_EXTERNAL_TCAM_REG                   data[10];

    CPSS_EXMXPM_EXTERNAL_TCAM_GENERIC_CMD_INST_UNT  instructionsGet;
    GT_U32                                          numOfNopsGet = 0;
    GT_U32                                          numOfTcamCyclesResultGet = 0;
    GT_U32                                          numOfMailboxCyclesResultGet = 0;
    GT_U32                                          dataLengthInTcamLinesGet = 0;
    CPSS_EXMXPM_EXTERNAL_TCAM_REG                   dataGet[10];


    cpssOsBzero((GT_VOID*) data, sizeof(data));
    cpssOsBzero((GT_VOID*) dataGet, sizeof(dataGet));
    cpssOsBzero((GT_VOID*) &instructions, sizeof(instructions));
    cpssOsBzero((GT_VOID*) &instructionsGet, sizeof(instructionsGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssExMxPmExternalTcamTypeGet to get Tcam Type with tcamId [0].
            Expected: GT_OK and tcamTypePtr.
        */
        st = cpssExMxPmExternalTcamTypeGet(dev, 0, &tcamType);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /*
            1.2. Call with  for tcamType [CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_NL_E]
                                instructions {nlInstructionSet{dibus_low[0 / 0x1F],
                                                              dibus_high[0 / 0x1F],
                                                              dctx_init[0 / 0x3FF]} },
                            for tcamType [CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_IDT_E]
                                instructions {nlInstructionSet{ ltin0 [0 / 0x7],
                                                                inst0 [0 / 0x7],
                                                                ss_sel0 [0 / 0x1F],
                                                                ltin1 [0 / 0x7],
                                                                inst1 [0 / 0x7],
                                                                ss_sel1 [0 / 0x1F]} },
                            numOfNops [0 / 10],
                            numOfTcamCyclesResult [0 / 8],
                            numOfMailboxCyclesResult [0 / 4],
                            dataLengthInTcamLines [0 / 8],
                            data [3] {1,2,3},
            Expected: GT_OK.
        */
        /* iterate with 0 */
        if (tcamType == CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_NL_E)
        {
            instructions.nlInstructionSet.dibus_low  = 0;
            instructions.nlInstructionSet.dibus_high = 0;
            instructions.nlInstructionSet.dctx_init  = 0;
        }
        else
        {
            instructions.idtInstructionSet.ltin0   = 0;
            instructions.idtInstructionSet.inst0   = 0;
            instructions.idtInstructionSet.ss_sel0 = 0;
            instructions.idtInstructionSet.ltin1   = 0;
            instructions.idtInstructionSet.inst1   = 0;
            instructions.idtInstructionSet.ss_sel1 = 0;
        }

        numOfNops = 0;
        numOfTcamCyclesResult = 0;
        numOfMailboxCyclesResult = 0;
        dataLengthInTcamLines = 3;

        data[0][0] = 1;
        data[0][1] = 2;
        data[0][2] = 3;

        st = cpssExMxPmExternalTcamIndirectCmdGenericParamsSet(dev, &instructions, numOfNops, numOfTcamCyclesResult,
                                                               numOfMailboxCyclesResult, dataLengthInTcamLines, data);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.3. Call cpssExMxPmExternalTcamIndirectCmdGenericParamsGet with not NULL pointers
            Expected: GT_OK and the same params as wa sset.
        */
        st = cpssExMxPmExternalTcamIndirectCmdGenericParamsGet(dev, &instructionsGet, &numOfNopsGet, &numOfTcamCyclesResultGet,
                                                               &numOfMailboxCyclesResultGet, &cmdDataLength,
                                                               &dataLengthInTcamLinesGet, dataGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamIndirectCmdGenericParamsGet: %d", dev);

        if (tcamType == CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_NL_E)
        {
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &instructions.nlInstructionSet, 
                                         (GT_VOID*) &instructionsGet.nlInstructionSet, 
                                         sizeof(instructions.nlInstructionSet) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another instructions.nlInstructionSet than was set: %d", dev);
        }
        else
        {
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &instructions.idtInstructionSet, 
                                         (GT_VOID*) &instructionsGet.idtInstructionSet, 
                                         sizeof(instructions.nlInstructionSet) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another instructions.idtInstructionSet than was set: %d", dev);
        }

        UTF_VERIFY_EQUAL1_STRING_MAC(numOfNops, numOfNopsGet,
                                     "got another numOfNops than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(numOfMailboxCyclesResult, numOfMailboxCyclesResultGet,
                                     "got another numOfMailboxCyclesResult than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(numOfTcamCyclesResult, numOfTcamCyclesResultGet,
                                     "got another numOfTcamCyclesResult than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(dataLengthInTcamLines, cmdDataLength,
                                     "got another dataLengthInTcamLines than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) data, (GT_VOID*) dataGet, dataLengthInTcamLinesGet*3 )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another data than was set: %d", dev);

        /* iterate with 0 */
        if (tcamType == CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_NL_E)
        {
            instructions.nlInstructionSet.dibus_low  = 0x1F;
            instructions.nlInstructionSet.dibus_high = 0x1F;
            instructions.nlInstructionSet.dctx_init  = 0x3FF;
        }
        else
        {
            instructions.idtInstructionSet.ltin0   = 0x7;
            instructions.idtInstructionSet.inst0   = 0x7;
            instructions.idtInstructionSet.ss_sel0 = 0x1F;
            instructions.idtInstructionSet.ltin1   = 0x7;
            instructions.idtInstructionSet.inst1   = 0x7;
            instructions.idtInstructionSet.ss_sel1 = 0x1F;
        }

        numOfNops = 10;
        numOfTcamCyclesResult = 8;
        numOfMailboxCyclesResult = 4;
        dataLengthInTcamLines = 3;

        data[0][0] = 1;
        data[0][1] = 2;
        data[0][2] = 3;

        st = cpssExMxPmExternalTcamIndirectCmdGenericParamsSet(dev, &instructions, numOfNops, numOfTcamCyclesResult,
                                                               numOfMailboxCyclesResult, dataLengthInTcamLines, data);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.3. Call cpssExMxPmExternalTcamIndirectCmdGenericParamsGet with not NULL pointers
            Expected: GT_OK and the same params as wa sset.
        */
        st = cpssExMxPmExternalTcamIndirectCmdGenericParamsGet(dev, &instructionsGet, &numOfNopsGet, &numOfTcamCyclesResultGet,
                                                               &numOfMailboxCyclesResultGet, &cmdDataLength,
                                                               &dataLengthInTcamLinesGet, dataGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamIndirectCmdGenericParamsGet: %d", dev);

        if (tcamType == CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_NL_E)
        {
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &instructions.nlInstructionSet, 
                                         (GT_VOID*) &instructionsGet.nlInstructionSet, 
                                         sizeof(instructions.nlInstructionSet) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another instructions.nlInstructionSet than was set: %d", dev);
        }
        else
        {
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &instructions.idtInstructionSet, 
                                         (GT_VOID*) &instructionsGet.idtInstructionSet, 
                                         sizeof(instructions.nlInstructionSet) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another instructions.idtInstructionSet than was set: %d", dev);
        }

        UTF_VERIFY_EQUAL1_STRING_MAC(numOfNops, numOfNopsGet,
                                     "got another numOfNops than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(numOfMailboxCyclesResult, numOfMailboxCyclesResultGet,
                                     "got another numOfMailboxCyclesResult than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(numOfTcamCyclesResult, numOfTcamCyclesResultGet,
                                     "got another numOfTcamCyclesResult than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(dataLengthInTcamLines, cmdDataLength,
                                     "got another dataLengthInTcamLines than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) data, (GT_VOID*) dataGet, dataLengthInTcamLinesGet*3 )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another data than was set: %d", dev);

        /*
            1.4. Call numOfTcamCyclesResult [9] (out of range)
            Expected: NOT GT_OK.
        */
        numOfTcamCyclesResult = 9;

        st = cpssExMxPmExternalTcamIndirectCmdGenericParamsSet(dev, &instructions, numOfNops, numOfTcamCyclesResult,
                                                               numOfMailboxCyclesResult, dataLengthInTcamLines, data);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, numOfTcamCyclesResult = %d", dev, numOfTcamCyclesResult);

        numOfTcamCyclesResult = 0;

        /*
            1.5. Call numOfMailboxCyclesResult [5] (out of range)
            Expected: NOT GT_OK.
        */
        numOfMailboxCyclesResult = 5;

        st = cpssExMxPmExternalTcamIndirectCmdGenericParamsSet(dev, &instructions, numOfNops, numOfTcamCyclesResult,
                                                               numOfMailboxCyclesResult, dataLengthInTcamLines, data);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, numOfMailboxCyclesResult = %d", dev, numOfMailboxCyclesResult);

        numOfMailboxCyclesResult = 0;

        /*
            1.6. Call dataLengthInTcamLines [9] (out of range)
            Expected: NOT GT_OK.
        */
        dataLengthInTcamLines = 9;

        st = cpssExMxPmExternalTcamIndirectCmdGenericParamsSet(dev, &instructions, numOfNops, numOfTcamCyclesResult,
                                                               numOfMailboxCyclesResult, dataLengthInTcamLines, data);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dataLengthInTcamLines = %d", dev, dataLengthInTcamLines);

        dataLengthInTcamLines = 1;

        /*
            1.7. Call with instructionsPtr[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamIndirectCmdGenericParamsSet(dev, NULL, numOfNops, numOfTcamCyclesResult,
                                                               numOfMailboxCyclesResult, dataLengthInTcamLines, data);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, instructionsPtr = NULL" ,dev);

        /*
            1.8. Call with dataPtr[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamIndirectCmdGenericParamsSet(dev, &instructions, numOfNops, numOfTcamCyclesResult,
                                                               numOfMailboxCyclesResult, dataLengthInTcamLines, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dataPtr = NULL" ,dev);
    }

    instructions.nlInstructionSet.dibus_low  = 0;
    instructions.nlInstructionSet.dibus_high = 0;
    instructions.nlInstructionSet.dctx_init  = 0;
    numOfNops = 0;
    numOfTcamCyclesResult = 0;
    numOfMailboxCyclesResult = 0;
    dataLengthInTcamLines = 0;

    data[0][0] = 1;
    data[0][1] = 2;
    data[0][2] = 3;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamIndirectCmdGenericParamsSet(dev, &instructions, numOfNops, numOfTcamCyclesResult,
                                                   numOfMailboxCyclesResult, dataLengthInTcamLines, data);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamIndirectCmdGenericParamsSet(dev, &instructions, numOfNops, numOfTcamCyclesResult,
                                                    numOfMailboxCyclesResult, dataLengthInTcamLines, data);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamIndirectCmdGenericParamsGet
(
    IN  GT_U8                                           devNum,
    OUT CPSS_EXMXPM_EXTERNAL_TCAM_GENERIC_CMD_INST_UNT  *instructionsPtr,
    OUT GT_U32                                          *numOfNopsPtr,
    OUT GT_U32                                          *numOfTcamCyclesResultPtr,
    OUT GT_U32                                          *numOfMailboxCyclesResultPtr,
    OUT GT_U32                                          *cmdDataLengthPtr,
    INOUT GT_U32                                        *dataLengthInTcamLinesPtr,
    OUT CPSS_EXMXPM_EXTERNAL_TCAM_REG                   *dataPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamIndirectCmdGenericParamsGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL pointers
    Expected: GT_OK.
    1.2. Call with instructionsPtr[NULL]
    Expected:GT_BAD_PTR.
    1.3. Call with numOfNopsPtr[NULL]
    Expected:GT_BAD_PTR.
    1.4. Call with numOfMailboxCyclesResultPtr[NULL]
    Expected:GT_BAD_PTR.
    1.5. Call with numOfTcamCyclesResultPtr[NULL]
    Expected:GT_BAD_PTR.
    1.6. Call with cmdDataLengthPtr[NULL]
    Expected:GT_BAD_PTR.
    1.7. Call with dataLengthInTcamLinesPtr[NULL]
    Expected:GT_BAD_PTR.
    1.8. Call with dataPtr[NULL]
    Expected:GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_EXTERNAL_TCAM_GENERIC_CMD_INST_UNT  instructions;
    GT_U32                                          numOfNops = 0;
    GT_U32                                          numOfTcamCyclesResult = 0;
    GT_U32                                          numOfMailboxCyclesResult = 0;
    GT_U32                                          cmdDataLength = 0;
    GT_U32                                          dataLengthInTcamLines = 3;
    CPSS_EXMXPM_EXTERNAL_TCAM_REG                   data[10];


    cpssOsBzero((GT_VOID*) data, sizeof(data));
    cpssOsBzero((GT_VOID*) &instructions, sizeof(instructions));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL pointers
            Expected: GT_OK.
        */
        dataLengthInTcamLines = 3;

        st = cpssExMxPmExternalTcamIndirectCmdGenericParamsGet(dev, &instructions, &numOfNops, &numOfMailboxCyclesResult,
                                                               &numOfTcamCyclesResult, &cmdDataLength,
                                                               &dataLengthInTcamLines, data);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with instructionsPtr[NULL]
            Expected:GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamIndirectCmdGenericParamsGet(dev, NULL, &numOfNops, &numOfMailboxCyclesResult,
                                                               &numOfTcamCyclesResult, &cmdDataLength,
                                                               &dataLengthInTcamLines, data);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, instructionsPtr = NULL", dev);

        /*
            1.3. Call with numOfNopsPtr[NULL]
            Expected:GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamIndirectCmdGenericParamsGet(dev, &instructions, NULL, &numOfMailboxCyclesResult,
                                                               &numOfTcamCyclesResult, &cmdDataLength,
                                                               &dataLengthInTcamLines, data);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numOfNopsPtr = NULL", dev);

        /*
            1.4. Call with numOfMailboxCyclesResultPtr[NULL]
            Expected:GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamIndirectCmdGenericParamsGet(dev, &instructions, &numOfNops, NULL,
                                                               &numOfTcamCyclesResult, &cmdDataLength,
                                                               &dataLengthInTcamLines, data);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numOfMailboxCyclesResultPtr = NULL", dev);

        /*
            1.5. Call with numOfTcamCyclesResultPtr[NULL]
            Expected:GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamIndirectCmdGenericParamsGet(dev, &instructions, &numOfNops, &numOfMailboxCyclesResult,
                                                               NULL, &cmdDataLength,
                                                               &dataLengthInTcamLines, data);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numOfTcamCyclesResultPtr = NULL", dev);

        /*
            1.6. Call with cmdDataLengthPtr[NULL]
            Expected:GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamIndirectCmdGenericParamsGet(dev, &instructions, &numOfNops, &numOfMailboxCyclesResult,
                                                               &numOfTcamCyclesResult, NULL,
                                                               &dataLengthInTcamLines, data);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cmdDataLengthPtr = NULL", dev);

        /*
            1.7. Call with dataLengthInTcamLinesPtr[NULL]
            Expected:GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamIndirectCmdGenericParamsGet(dev, &instructions, &numOfNops, &numOfMailboxCyclesResult,
                                                               &numOfTcamCyclesResult, &cmdDataLength,
                                                               NULL, data);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dataLengthInTcamLinesPtr = NULL", dev);

        /*
            1.8. Call with dataPtr[NULL]
            Expected:GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamIndirectCmdGenericParamsGet(dev, &instructions, &numOfNops, &numOfMailboxCyclesResult,
                                                               &numOfTcamCyclesResult, &cmdDataLength,
                                                               &dataLengthInTcamLines, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dataPtr = NULL", dev);
    }

    dataLengthInTcamLines = 3;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamIndirectCmdGenericParamsGet(dev, &instructions, &numOfNops, &numOfMailboxCyclesResult,
                                                       &numOfTcamCyclesResult, &cmdDataLength,
                                                       &dataLengthInTcamLines, data);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamIndirectCmdGenericParamsGet(dev, &instructions, &numOfNops, &numOfMailboxCyclesResult,
                                                       &numOfTcamCyclesResult, &cmdDataLength,
                                                       &dataLengthInTcamLines, data);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamIndirectCmdLoopParamsSet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_LOOP_PARAMS_STC   *loopParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamIndirectCmdLoopParamsSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with loopParamsPtr{ loopCount [0 / 10]
                                  loopInfinitely [GT_FALSE / GT_TRUE]
                                  incrementRuleIndexOrRegAddr [GT_FALSE / GT_TRUE]
                                  incrementData [GT_FALSE / GT_TRUE] }
    Expected: GT_OK.
    1.2. Call cpssExMxPmExternalTcamIndirectCmdLoopParamsGet with not NULL loopParamsPtr
    Expected: GT_OK and the same loopParamsPtr as was set
    1.3. Call with loopParamsPtr[NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_BOOL     isEqual = GT_FALSE;
    CPSS_EXMXPM_EXTERNAL_TCAM_LOOP_PARAMS_STC   loopParams;
    CPSS_EXMXPM_EXTERNAL_TCAM_LOOP_PARAMS_STC   loopParamsGet;


    cpssOsBzero((GT_VOID*) &loopParams, sizeof(loopParams));
    cpssOsBzero((GT_VOID*) &loopParamsGet, sizeof(loopParamsGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with loopParamsPtr{ loopCount [0 / 10]
                                          loopInfinitely [GT_FALSE / GT_TRUE]
                                          incrementRuleIndexOrRegAddr [GT_FALSE / GT_TRUE]
                                          incrementData [GT_FALSE / GT_TRUE] }
            Expected: GT_OK.
        */
        /* iterate with loopParams.loopCount = 0 */
        loopParams.loopCount = 0;
        loopParams.loopInfinitely = GT_TRUE;
        loopParams.incrementRuleIndexOrRegAddr = GT_FALSE;
        loopParams.incrementData = GT_FALSE;

        st = cpssExMxPmExternalTcamIndirectCmdLoopParamsSet(dev, &loopParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmExternalTcamIndirectCmdLoopParamsGet with not NULL loopParamsPtr
            Expected: GT_OK and the same loopParamsPtr as was set
        */
        st = cpssExMxPmExternalTcamIndirectCmdLoopParamsGet(dev, &loopParamsGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamIndirectCmdLoopParamsGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(loopParams.loopInfinitely, loopParamsGet.loopInfinitely,
                   "got anothe loopParamsPtr->loopInfinitely than was set", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(loopParams.incrementRuleIndexOrRegAddr, loopParamsGet.incrementRuleIndexOrRegAddr,
                   "got anothe loopParamsPtr->incrementRuleIndexOrRegAddr than was set", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(loopParams.incrementData, loopParamsGet.incrementData,
                   "got anothe loopParamsPtr->incrementData than was set", dev);

        /* iterate with loopParams.loopCount = 10 */
        loopParams.loopCount = 10;
        loopParams.loopInfinitely = GT_FALSE;
        loopParams.incrementRuleIndexOrRegAddr = GT_TRUE;
        loopParams.incrementData = GT_TRUE;

        st = cpssExMxPmExternalTcamIndirectCmdLoopParamsSet(dev, &loopParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmExternalTcamIndirectCmdLoopParamsGet with not NULL loopParamsPtr
            Expected: GT_OK and the same loopParamsPtr as was set
        */
        st = cpssExMxPmExternalTcamIndirectCmdLoopParamsGet(dev, &loopParamsGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamIndirectCmdLoopParamsGet: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &loopParams, (GT_VOID*) &loopParamsGet, sizeof(loopParams) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got anothe loopParams than was set", dev);


        /*
            1.3. Call with loopParamsPtr[NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamIndirectCmdLoopParamsSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, loopParamsPtr = NULL", dev);
    }

    loopParams.loopCount = 0;
    loopParams.loopInfinitely = GT_FALSE;
    loopParams.incrementRuleIndexOrRegAddr = GT_FALSE;
    loopParams.incrementData = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamIndirectCmdLoopParamsSet(dev, &loopParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamIndirectCmdLoopParamsSet(dev, &loopParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamIndirectCmdLoopParamsGet
(
    IN  GT_U8                                       devNum,
    OUT  CPSS_EXMXPM_EXTERNAL_TCAM_LOOP_PARAMS_STC  *loopParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamIndirectCmdLoopParamsGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with  not NULL loopParamsPtr
    Expected: GT_OK.
    1.2. Call with loopParamsPtr[NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_EXTERNAL_TCAM_LOOP_PARAMS_STC  loopParams;


    cpssOsBzero((GT_VOID*) &loopParams, sizeof(loopParams));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with  not NULL loopParamsPtr
            Expected: GT_OK.
        */
        st = cpssExMxPmExternalTcamIndirectCmdLoopParamsGet(dev, &loopParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with loopParamsPtr[NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamIndirectCmdLoopParamsGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, loopParamsPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamIndirectCmdLoopParamsGet(dev, &loopParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamIndirectCmdLoopParamsGet(dev, &loopParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamIndirectCmdResultGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              resultLinesNum,
    OUT CPSS_EXMXPM_EXTERNAL_MAILBOX_LINE   *resultPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamIndirectCmdResultGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with resultLinesNum [0 / 2] and not NULL resultPtr.
    Expected: GT_OK.
    1.2. Call with resultLinesNum [3] (out of range) and not NULL resultPtr.
    Expected: NOT GT_OK.
    1.3. Call with resultPtr[NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32                              resultLinesNum = 0;
    CPSS_EXMXPM_EXTERNAL_MAILBOX_LINE   result[3];


    cpssOsBzero((GT_VOID*) result, sizeof(result));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with resultLinesNum [0 / 2] and not NULL resultPtr.
            Expected: GT_OK.
        */
        /* iterate with resultLinesNum = 0 */
        resultLinesNum = 0;

        st = cpssExMxPmExternalTcamIndirectCmdResultGet(dev, resultLinesNum, result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, resultLinesNum);

        /* iterate with resultLinesNum = 2 */
        resultLinesNum = 2;

        st = cpssExMxPmExternalTcamIndirectCmdResultGet(dev, resultLinesNum, result);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, resultLinesNum);

        /*
            1.2. Call with resultLinesNum [3] (out of range) and not NULL resultPtr.
            Expected: NOT GT_OK.
        */
        resultLinesNum = 3;

        st = cpssExMxPmExternalTcamIndirectCmdResultGet(dev, resultLinesNum, result);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, resultLinesNum);

        resultLinesNum = 0;

        /*
            1.3. Call with resultPtr[NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamIndirectCmdResultGet(dev, resultLinesNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, resultPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamIndirectCmdResultGet(dev, resultLinesNum, result);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamIndirectCmdResultGet(dev, resultLinesNum, result);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamRxParityErrorCntrGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *cntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamRxParityErrorCntrGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with  not NULL cntrPtr
    Expected: GT_OK.
    1.2. Call with cntrPtr[NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      cntr = 0;



    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with  not NULL cntrPtr
            Expected: GT_OK.
        */
        st = cpssExMxPmExternalTcamRxParityErrorCntrGet(dev, &cntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with cntrPtr[NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamRxParityErrorCntrGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cntrPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamRxParityErrorCntrGet(dev, &cntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamRxParityErrorCntrGet(dev, &cntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmExternalTcamRxIoErrorCntrGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *cntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalTcamRxIoErrorCntrGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with  not NULL cntrPtr
    Expected: GT_OK.
    1.2. Call with cntrPtr[NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      cntr = 0;



    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with  not NULL cntrPtr
            Expected: GT_OK.
        */
        st = cpssExMxPmExternalTcamRxIoErrorCntrGet(dev, &cntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with cntrPtr[NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmExternalTcamRxIoErrorCntrGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cntrPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmExternalTcamRxIoErrorCntrGet(dev, &cntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmExternalTcamRxIoErrorCntrGet(dev, &cntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}



/*----------------------------------------------------------------------------*/
/*
    Test function to Fill TcamRule table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmExternalFillTcamRuleTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in TcamRule table.
         Call cpssExMxPmExternalTcamRuleWrite with tcamId [0 .. numEntries-1],
                                                   ruleSize [CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E],
                                                   ruleStartIndex [0],
                                                   dataSize [0],
                                                   valid [GT_FALSE],
                                                   ageEnable [GT_FALSE],
                                                   dataFormat [CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E],
                                                   patternOrXArr[0],
                                                   maskOrYArr[0].
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmExternalTcamRuleWrite with tcamId [numEntries] (out of range) and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in TcamRule table and compare with original.
         Call cpssExMxPmExternalTcamRuleRead with non-NULL validPtr, patternOrXArr, maskOrYArr
                                             and other params from 1.2.
    Expected: GT_OK and the same validPtr, patternOrXArr, maskOrYArr
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmExternalTcamRuleRead with tcamId [numEntries] and other params from 1.4.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual    = GT_FALSE;
    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT   ruleSize     = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E;
    GT_U32                                    ruleStartIndex = 0;
    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT dataFormat   = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E;
    CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_ENT        tcamType     = CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_IDT_E;
    GT_U32                                    dataSize     = 0;
    GT_BOOL                                   valid        = GT_FALSE;
    GT_BOOL                                   ageEnable    = GT_FALSE;
    GT_U32                                    patternOrXArr[3];
    GT_U32                                    maskOrYArr[3];

    GT_BOOL     validGet     = GT_FALSE;
    GT_BOOL     ageEnableGet = GT_FALSE;
    GT_U32      patternOrXArrGet[3];
    GT_U32      maskOrYArrGet[3];

    cpssOsBzero((GT_VOID*) maskOrYArr, sizeof(maskOrYArr));
    cpssOsBzero((GT_VOID*) patternOrXArr, sizeof(patternOrXArr));
    cpssOsBzero((GT_VOID*) maskOrYArrGet, sizeof(maskOrYArrGet));
    cpssOsBzero((GT_VOID*) patternOrXArrGet, sizeof(patternOrXArrGet));

    /* Fill the entry for TcamRule table */
    ruleSize       = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E;
    ruleStartIndex = 0;
    dataSize       = 0;
    valid          = GT_FALSE;
    ageEnable      = GT_FALSE;
    dataFormat     = CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E;

    maskOrYArr[0] = 0;
    maskOrYArr[1] = 0;
    maskOrYArr[2] = 0;

    patternOrXArr[0] = 0;
    patternOrXArr[1] = 0;
    patternOrXArr[2] = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = 4;

        /* 1.2. Fill all entries in TcamRule table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* get TCAM type*/
            st = cpssExMxPmExternalTcamTypeGet(dev, iTemp, &tcamType);
            if (CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_NL_E == tcamType && iTemp != 0)
            {
                continue;
            }
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmExternalTcamTypeGet: %d, %d", dev, iTemp);

            st = cpssExMxPmExternalTcamRuleWrite(dev, iTemp, ruleStartIndex, ruleSize,
                                                 dataSize, valid, ageEnable, dataFormat,
                                                 patternOrXArr, maskOrYArr);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamRuleWrite: %d, %d", dev, iTemp);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmExternalTcamRuleWrite(dev, numEntries, ruleStartIndex, ruleSize,
                                             dataSize, valid, ageEnable, dataFormat,
                                             patternOrXArr, maskOrYArr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgStpEntryWrite: %d, %d", dev, numEntries);

        /* 1.4. Read all entries in TcamRule table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* get TCAM type*/
            st = cpssExMxPmExternalTcamTypeGet(dev, iTemp, &tcamType);
            if (CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_NL_E == tcamType && iTemp != 0)
            {
                continue;
            }
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmExternalTcamTypeGet: %d, %d", dev, iTemp);

            st = cpssExMxPmExternalTcamRuleRead(dev, iTemp, ruleStartIndex, ruleSize,
                                                dataSize, dataFormat, &validGet, &ageEnableGet,
                                                patternOrXArrGet, maskOrYArrGet);
            UTF_VERIFY_EQUAL6_STRING_MAC(GT_OK, st, "cpssExMxPmExternalTcamRuleGet, %d, %d, %d, %d, %d, %d",
                                         dev, iTemp, ruleStartIndex, ruleSize, dataSize, dataFormat);
    
            /* Validating values */
            UTF_VERIFY_EQUAL1_STRING_MAC(valid, validGet, "got another valid than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(ageEnable, ageEnableGet, "got another ageEnable than was set: %d", dev);
    
    
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) patternOrXArr,(GT_VOID*) patternOrXArrGet,sizeof(patternOrXArr[0]*3))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,"get another patternOrXArr than was set: %d", dev);
    
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) maskOrYArr,(GT_VOID*) maskOrYArrGet,sizeof(maskOrYArr[0]*3))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,"get another maskOrYArr than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmExternalTcamRuleRead(dev, numEntries, ruleStartIndex, ruleSize,
                                            dataSize, dataFormat, &validGet, &ageEnableGet,
                                            patternOrXArrGet, maskOrYArrGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgStpEntryRead: %d, %d", dev, numEntries);

    }

}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmExternalTcam suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmExternalTcam)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamRegSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamRegGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamRuleWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamRuleRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamGlobalWriteModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamGlobalWriteModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamHitIndexConvertSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamHitIndexConvertGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamClientConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamClientConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamActionWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamActionRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamIndirectCmdStart)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamIndirectCmdStop)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamIndirectCmdStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamIndirectCmdParamsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamIndirectCmdParamsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamIndirectCmdGenericParamsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamIndirectCmdGenericParamsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamIndirectCmdLoopParamsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamIndirectCmdLoopParamsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamIndirectCmdResultGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamRxParityErrorCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalTcamRxIoErrorCntrGet)
    /* Test for table */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmExternalFillTcamRuleTable)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmExternalTcam)

