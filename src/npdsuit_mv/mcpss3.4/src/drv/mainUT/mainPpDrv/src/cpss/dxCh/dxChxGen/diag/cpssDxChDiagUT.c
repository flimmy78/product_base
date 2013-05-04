/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChDiagUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxChDiagUT, that provides
*       CPSS DXCH Diagnostic API
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiag.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#include <cpss/generic/diag/private/prvCpssGenDiag.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <common/tgfCommon.h>

/* Valid port num value used for testing */
#define DIAG_VALID_PORTNUM_CNS       0

/* Invalid lane num used for testing */
#define DIAG_INVALID_LANENUM_CNS     4


#define IS_PORT_FE_E(devNum,portNum)    \
        (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType==PRV_CPSS_PORT_FE_E)

#define IS_PORT_GE_E(devNum,portNum)    \
        (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType==PRV_CPSS_PORT_GE_E)

#define IS_PORT_XG_E(devNum,portNum)    \
        (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType==PRV_CPSS_PORT_XG_E)

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagAllMemTest
(
    IN GT_U8                     devNum,
    OUT GT_BOOL                  *testStatusPtr,
    OUT GT_U32                   *addrPtr,
    OUT GT_U32                   *readValPtr,
    OUT GT_U32                   *writeValPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagAllMemTest)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-NULL pointers.
    Expected: GT_OK.
    1.2. Call with testStatusPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with addrPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with readValPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with writeValPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     testStatus;
    GT_U32      addr;
    GT_U32      readVal;
    GT_U32      writeVal;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL pointers.
            Expected: GT_OK.
        */
        st = cpssDxChDiagAllMemTest(dev, &testStatus, &addr, &readVal, &writeVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with testStatusPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChDiagAllMemTest(dev, NULL, &addr, &readVal, &writeVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.3. Call with addrPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChDiagAllMemTest(dev, &testStatus, NULL, &readVal, &writeVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.4. Call with readValPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChDiagAllMemTest(dev, &testStatus, &addr, NULL, &writeVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.5. Call with writeValPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChDiagAllMemTest(dev, &testStatus, &addr, &readVal, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagAllMemTest(dev, &testStatus, &addr, &readVal, &writeVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagAllMemTest(dev, &testStatus, &addr, &readVal, &writeVal);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagAllRegTest
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *testStatusPtr,
    OUT GT_U32  *badRegPtr,
    OUT GT_U32  *readValPtr,
    OUT GT_U32  *writeValPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagAllRegTest)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-NULL pointers.
    Expected: GT_OK.
    1.2. Call with testStatusPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with badRegPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with readValPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with writeValPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     testStatus;
    GT_U32      badReg;
    GT_U32      readVal;
    GT_U32      writeVal;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL pointers.
            Expected: GT_OK.
        */
        st = cpssDxChDiagAllRegTest(dev, &testStatus, &badReg, &readVal, &writeVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with testStatusPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChDiagAllRegTest(dev, NULL, &badReg, &readVal, &writeVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.3. Call with badRegPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChDiagAllRegTest(dev, &testStatus, NULL, &readVal, &writeVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.4. Call with readValPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChDiagAllRegTest(dev, &testStatus, &badReg, NULL, &writeVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.5. Call with writeValPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChDiagAllRegTest(dev, &testStatus, &badReg, &readVal, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagAllRegTest(dev, &testStatus, &badReg, &readVal, &writeVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagAllRegTest(dev, &testStatus, &badReg, &readVal, &writeVal);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagMemRead
(
    IN GT_U8                          devNum,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN GT_U32                         offset,
    OUT GT_U32                        *dataPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagMemRead)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with memType = [ CPSS_DIAG_PP_MEM_BUFFER_DRAM_E /
                               CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E ],
                  offset [0], and non-NULL dataPtr.
    Expected: GT_OK.
    1.2. Call with wrong enum values memType  and
         other params same as in 1.1.
    Expected: not GT_OK.
    1.3. Call with dataPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DIAG_PP_MEM_TYPE_ENT  memType = CPSS_DIAG_PP_MEM_BUFFER_DRAM_E;
    GT_U32                     offset = 0;
    GT_U32                     data;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with memType = [  CPSS_DIAG_PP_MEM_BUFFER_DRAM_E /
                                        CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E ],
                          offset [0], and non-NULL dataPtr.
            Expected: GT_OK.
        */
        memType = CPSS_DIAG_PP_MEM_BUFFER_DRAM_E;

        st = cpssDxChDiagMemRead(dev, memType, offset, &data);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);


        memType = CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E;

        st = cpssDxChDiagMemRead(dev, memType, offset, &data);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

        /*
            1.2. Call with wrong enum values memType and other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChDiagMemRead
                            (dev, memType, offset, &data),
                            memType);

        /*
            1.3. Call with dataPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChDiagMemRead(dev, memType, offset, NULL);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, dev, memType);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagMemRead(dev, memType, offset, &data);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagMemRead(dev, memType, offset, &data);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagMemTest
(
    IN GT_U8                          devNum,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN GT_U32                         startOffset,
    IN GT_U32                         size,
    IN CPSS_DIAG_TEST_PROFILE_ENT     profile,
    OUT GT_BOOL                       *testStatusPtr,
    OUT GT_U32                        *addrPtr,
    OUT GT_U32                        *readValPtr,
    OUT GT_U32                        *writeValPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagMemTest)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with  memType [CPSS_DIAG_PP_MEM_BUFFER_DRAM_E /
                             CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E ],
                        startOffset [0],
                        size [_8K],
                        profile [CPSS_DIAG_TEST_RANDOM_E]
                    and non-NULL pointers.
    Expected: GT_OK.
    1.2. Call with wrong enum values memType and other params same as in 1.1.
    Expected: not GT_OK.
    1.3. Call with wrong enum values profile and other params same as in 1.1.
    Expected: not GT_OK.
    1.4. Call with testStatusPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with addrPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.6. Call with readValPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.7. Call with writeValPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DIAG_PP_MEM_TYPE_ENT  memType = CPSS_DIAG_PP_MEM_BUFFER_DRAM_E;
    GT_U32                     startOffset = 0;
    GT_U32                     size = 0;
    CPSS_DIAG_TEST_PROFILE_ENT profile = CPSS_DIAG_TEST_RANDOM_E;
    GT_BOOL                    testStatus;
    GT_U32                     addr;
    GT_U32                     readVal;
    GT_U32                     writeVal;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with memType = [ CPSS_DIAG_PP_MEM_BUFFER_DRAM_E,
                                       CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E /
                                       CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E ],
                          offset [0], and non-NULL dataPtr.
            Expected: GT_OK.
        */

        /*call with memType = CPSS_DIAG_PP_MEM_BUFFER_DRAM_E*/
        memType = CPSS_DIAG_PP_MEM_BUFFER_DRAM_E;
        size = _8K;

        st = cpssDxChDiagMemTest(dev, memType, startOffset, size, profile,
                                 &testStatus, &addr, &readVal, &writeVal);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

        /*call with memType = CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E*/
        memType = CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E;
        size = _4K;

        st = cpssDxChDiagMemTest(dev, memType, startOffset, size, profile,
                                 &testStatus, &addr, &readVal, &writeVal);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

        /*call with memType = CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E*/
        memType = CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E;
        size = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.fdb;

        st = cpssDxChDiagMemTest(dev, memType, startOffset, size, profile,
                                 &testStatus, &addr, &readVal, &writeVal);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

        /*
            1.2. Call with wrong enum values memType and other params same as in 1.1.
            Expected: not GT_OK.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChDiagMemTest
                            (dev, memType, startOffset, size, profile,
                             &testStatus, &addr, &readVal, &writeVal),
                            memType);

        /*
            1.3. Call with wrong enum values profile and other params same as in 1.1.
            Expected: not GT_OK.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChDiagMemTest
                            (dev, memType, startOffset, size, profile,
                             &testStatus, &addr, &readVal, &writeVal),
                            profile);

        /*
            1.4. Call with testStatusPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChDiagMemTest(dev, memType, startOffset, size, profile,
                                 NULL, &addr, &readVal, &writeVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.5. Call with addrPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChDiagMemTest(dev, memType, startOffset, size, profile,
                                 &testStatus, NULL, &readVal, &writeVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.6. Call with readValPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChDiagMemTest(dev, memType, startOffset, size, profile,
                                 &testStatus, &addr, NULL, &writeVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.7. Call with writeValPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChDiagMemTest(dev, memType, startOffset, size, profile,
                                 &testStatus, &addr, &readVal, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagMemTest(dev, memType, startOffset, size, profile,
                                 &testStatus, &addr, &readVal, &writeVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagMemTest(dev, memType, startOffset, size, profile,
                             &testStatus, &addr, &readVal, &writeVal);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagMemWrite
(
    IN GT_U8                          devNum,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN GT_U32                         offset,
    IN GT_U32                         data
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagMemWrite)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with memType = [ CPSS_DIAG_PP_MEM_BUFFER_DRAM_E /
                               CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E ],
                  offset [0] and data [0].
    Expected: GT_OK.
    1.2. Call with wrong enum values memType  and
         other params same as in 1.1.
    Expected: not GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DIAG_PP_MEM_TYPE_ENT  memType = CPSS_DIAG_PP_MEM_BUFFER_DRAM_E;
    GT_U32                     offset = 0;
    GT_U32                     data = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with memType = [  CPSS_DIAG_PP_MEM_BUFFER_DRAM_E /
                                        CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E ],
                  offset [0] and data[0].
            Expected: GT_OK.
        */
        memType = CPSS_DIAG_PP_MEM_BUFFER_DRAM_E;

        st = cpssDxChDiagMemWrite(dev, memType, offset, data);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

        memType = CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E;

        st = cpssDxChDiagMemWrite(dev, memType, offset, data);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

        /*
            1.2. Call with wrong enum values memType  and other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChDiagMemWrite
                            (dev, memType, offset, data),
                            memType);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagMemWrite(dev, memType, offset, data);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagMemWrite(dev, memType, offset, data);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPrbsPortCheckEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_U32     laneNum,
    IN  GT_BOOL    enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPrbsPortCheckEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxChx)
    1.1.1. Call with enable[GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChDiagPrbsPortCheckEnableGet with not-NULL enablePtr.
    Expected: GT_OK and the same enable as was set.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = DIAG_VALID_PORTNUM_CNS;

    GT_U32      lane      = 0;
    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable[GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            /* iterate with enable - GT_FALSE */
            enable = GT_FALSE;
            st = cpssDxChDiagPrbsPortCheckEnableSet(dev, port, lane, enable);

            /* the feature supported for all Gig ports
               and XG ports of CH3 and above devices */
            if( IS_PORT_GE_E(dev, port) ||
                (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(dev) && IS_PORT_XG_E(dev, port)) )
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

                /*
                    1.1.2. Call cpssDxChDiagPrbsPortCheckEnableGet with not-NULL enablePtr.
                    Expected: GT_OK and the same enable as was set.
                */
                st = cpssDxChDiagPrbsPortCheckEnableGet(dev, port, lane, &enableGet);

                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                            "cpssDxChDiagPrbsPortCheckEnableGet: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                               "got another enable then was set: %d, %d", dev, port);

                /* iterate with enable - GT_TRUE */
                enable = GT_TRUE;

                st = cpssDxChDiagPrbsPortCheckEnableSet(dev, port, lane, enable);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

                /*
                    1.1.2. Call cpssDxChDiagPrbsPortCheckEnableGet with not-NULL enablePtr.
                    Expected: GT_OK and the same enable as was set.
                */
                st = cpssDxChDiagPrbsPortCheckEnableGet(dev, port, lane, &enableGet);

                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                            "cpssDxChDiagPrbsPortCheckEnableGet: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                               "got another enable then was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, enable);
            }
        }

        enable = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChDiagPrbsPortCheckEnableSet(dev, port, lane, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChDiagPrbsPortCheckEnableSet(dev, port, lane, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChDiagPrbsPortCheckEnableSet(dev, port, lane, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_FALSE;
    port   = DIAG_VALID_PORTNUM_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagPrbsPortCheckEnableSet(dev, port, lane, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagPrbsPortCheckEnableSet(dev, port, lane, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPrbsPortCheckEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_U32     laneNum,
    OUT GT_BOOL    *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPrbsPortCheckEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxChx)
    1.1.1. Call with not-NULL enable.
    Expected: GT_OK.
    1.1.2. Call enablePtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = DIAG_VALID_PORTNUM_CNS;

    GT_U32      lane      = 0;
    GT_BOOL     enable    = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with not-NULL enable.
                Expected: GT_OK.
            */
            st = cpssDxChDiagPrbsPortCheckEnableGet(dev, port, lane, &enable);

            /* the feature supported for all Gig ports
               and XG ports of CH3 and above devices */
            if( IS_PORT_GE_E(dev, port) ||
                (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(dev) && IS_PORT_XG_E(dev, port)) )
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. Call enablePtr[NULL].
                    Expected: GT_BAD_PTR.
                */
                st = cpssDxChDiagPrbsPortCheckEnableGet(dev, port, lane, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, enable);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChDiagPrbsPortCheckEnableGet(dev, port, lane, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChDiagPrbsPortCheckEnableGet(dev, port, lane, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChDiagPrbsPortCheckEnableGet(dev, port, lane, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = DIAG_VALID_PORTNUM_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagPrbsPortCheckEnableGet(dev, port, lane, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagPrbsPortCheckEnableGet(dev, port, lane, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPrbsPortCheckReadyGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    OUT GT_BOOL    *isReadyPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPrbsPortCheckReadyGet)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxChx)
    1.1.1. Call with not-NULL isReady.
    Expected: GT_OK.
    1.1.2. Call isReadyPtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = DIAG_VALID_PORTNUM_CNS;
    GT_BOOL     isReady    = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            if( IS_PORT_GE_E(dev, port) )
            {
                /*
                    1.1.1. Call with not-NULL isReady.
                    Expected: GT_OK.
                */
                st = cpssDxChDiagPrbsPortCheckReadyGet(dev, port, &isReady);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. Call isReadyPtr[NULL].
                    Expected: GT_BAD_PTR.
                */
                st = cpssDxChDiagPrbsPortCheckReadyGet(dev, port, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                             "%d, %d, isReadyPtr = NULL", dev, port);
            }
            else
            {
                /* not supported not GE_E ports*/
                st = cpssDxChDiagPrbsPortCheckReadyGet(dev, port, &isReady);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, isReady);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChDiagPrbsPortCheckReadyGet(dev, port, &isReady);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChDiagPrbsPortCheckReadyGet(dev, port, &isReady);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChDiagPrbsPortCheckReadyGet(dev, port, &isReady);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = DIAG_VALID_PORTNUM_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagPrbsPortCheckReadyGet(dev, port, &isReady);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagPrbsPortCheckReadyGet(dev, port, &isReady);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPrbsPortGenerateEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_U32     laneNum,
    IN  GT_BOOL    enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPrbsPortGenerateEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxChx)
    1.1.1. Call with enable[GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChDiagPrbsPortGenerateEnableGet with not-NULL enablePtr.
    Expected: GT_OK and the same enable as was set.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = DIAG_VALID_PORTNUM_CNS;

    GT_U32      lane      = 0;
    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable[GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            /* iterate with enable - GT_FALSE */
            enable = GT_FALSE;
            st = cpssDxChDiagPrbsPortGenerateEnableSet(dev, port, lane, enable);

            /* the feature supported for all Gig ports
               and XG ports of CH3 and above devices */
            if( IS_PORT_GE_E(dev, port) ||
                (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(dev) && IS_PORT_XG_E(dev, port)) )
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

                /*
                    1.1.2. Call cpssDxChDiagPrbsPortGenerateEnableGet with not-NULL enablePtr.
                    Expected: GT_OK and the same enable as was set.
                */
                st = cpssDxChDiagPrbsPortGenerateEnableGet(dev, port, lane, &enableGet);

                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                            "cpssDxChDiagPrbsPortGenerateEnableGet: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                               "got another enable then was set: %d, %d", dev, port);

                /* iterate with enable - GT_TRUE */
                enable = GT_TRUE;

                st = cpssDxChDiagPrbsPortGenerateEnableSet(dev, port, lane, enable);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

                /*
                    1.1.2. Call cpssDxChDiagPrbsPortGenerateEnableGet with not-NULL enablePtr.
                    Expected: GT_OK and the same enable as was set.
                */
                st = cpssDxChDiagPrbsPortGenerateEnableGet(dev, port, lane, &enableGet);

                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                            "cpssDxChDiagPrbsPortGenerateEnableGet: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                               "got another enable then was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, enable);
            }
        }

        enable = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChDiagPrbsPortGenerateEnableSet(dev, port, lane, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChDiagPrbsPortGenerateEnableSet(dev, port, lane, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChDiagPrbsPortGenerateEnableSet(dev, port, lane, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_FALSE;
    port   = DIAG_VALID_PORTNUM_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagPrbsPortGenerateEnableSet(dev, port, lane, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagPrbsPortGenerateEnableSet(dev, port, lane, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPrbsPortGenerateEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_U32     laneNum,
    OUT GT_BOOL    *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPrbsPortGenerateEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxChx)
    1.1.1. Call with not-NULL enable.
    Expected: GT_OK.
    1.1.2. Call enablePtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = DIAG_VALID_PORTNUM_CNS;

    GT_U32      lane      = 0;
    GT_BOOL     enable    = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with not-NULL enable.
                Expected: GT_OK.
            */
            st = cpssDxChDiagPrbsPortGenerateEnableGet(dev, port, lane, &enable);

            /* the feature supported for all Gig ports
               and XG ports of CH3 and above devices */
            if( IS_PORT_GE_E(dev, port) ||
                (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(dev) && IS_PORT_XG_E(dev, port)) )
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. Call enablePtr[NULL].
                    Expected: GT_BAD_PTR.
                */
                st = cpssDxChDiagPrbsPortGenerateEnableGet(dev, port, lane, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, enable);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChDiagPrbsPortGenerateEnableGet(dev, port, lane, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChDiagPrbsPortGenerateEnableGet(dev, port, lane, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChDiagPrbsPortGenerateEnableGet(dev, port, lane, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = DIAG_VALID_PORTNUM_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagPrbsPortGenerateEnableGet(dev, port, lane, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagPrbsPortGenerateEnableGet(dev, port, lane, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPrbsPortStatusGet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    OUT  GT_BOOL        *checkerLockedPtr,
    OUT  GT_U32         *errorCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPrbsPortStatusGet)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxChx)
    1.1.1. Call with not-NULL pointers.
    Expected: GT_OK.
    1.1.2. Call checkerLockedPtr[NULL].
    Expected: GT_BAD_PTR.
    1.1.3. Call errorCntrPtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = DIAG_VALID_PORTNUM_CNS;

    GT_U32      lane      = 0;
    GT_BOOL     checkerLocked;
    GT_U32      errorCntr;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with not-NULL pointers.
                Expected: GT_OK.
            */
            st = cpssDxChDiagPrbsPortStatusGet(dev, port, lane, &checkerLocked, &errorCntr);

            /* the feature supported for all Gig ports
               and XG ports of CH3 and above devices */
            if( IS_PORT_GE_E(dev, port) ||
                (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(dev) && IS_PORT_XG_E(dev, port)) )
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. Call checkerLockedPtr[NULL].
                    Expected: GT_BAD_PTR.
                */
                st = cpssDxChDiagPrbsPortStatusGet(dev, port, lane, NULL, &errorCntr);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                             "%d, %d, checkerLockedPtr = NULL", dev, port);

                /*
                    1.1.2. Call errorCntr[NULL].
                    Expected: GT_BAD_PTR.
                */
                st = cpssDxChDiagPrbsPortStatusGet(dev, port, lane, &checkerLocked, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                             "%d, %d, errorCntr = NULL", dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, checkerLocked);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChDiagPrbsPortStatusGet(dev, port, lane, &checkerLocked, &errorCntr);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChDiagPrbsPortStatusGet(dev, port, lane, &checkerLocked, &errorCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChDiagPrbsPortStatusGet(dev, port, lane, &checkerLocked, &errorCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = DIAG_VALID_PORTNUM_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagPrbsPortStatusGet(dev, port, lane, &checkerLocked, &errorCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagPrbsPortStatusGet(dev, port, lane, &checkerLocked, &errorCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPrbsPortTransmitModeSet
(
    IN   GT_U8                    devNum,
    IN   GT_U8                    portNum,
    IN   GT_U32                   laneNum,
    IN   CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT   mode
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPrbsPortTransmitModeSet)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxChx)
    1.1.1. Call with mode [CPSS_DXCH_DIAG_TRANSMIT_MODE_REGULAR_E].
    Expected: GT_OK.
    1.1.2. Call cpssDxChDiagPrbsPortTransmitModeGet with not-NULL modePtr.
    Expected: GT_OK and the same mode as was set.
    1.1.3. Call with wrong enum values mode.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = DIAG_VALID_PORTNUM_CNS;

    GT_U32      lane = 0;
    CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT mode = 0;
    CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT modeGet = 1;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with mode [CPSS_DXCH_DIAG_TRANSMIT_MODE_REGULAR_E].
                Expected: GT_OK.
            */
            mode = CPSS_DXCH_DIAG_TRANSMIT_MODE_REGULAR_E;

            st = cpssDxChDiagPrbsPortTransmitModeSet(dev, port, lane, mode);

            /* the feature supported for all Gig ports
               and XG ports of CH3 and above devices */
            if( IS_PORT_GE_E(dev, port) ||
                (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(dev) && IS_PORT_XG_E(dev, port)) )
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);

                /*
                    1.1.2. Call cpssDxChDiagPrbsPortTransmitModeGet with not-NULL modePtr.
                    Expected: GT_OK and the same mode as was set.
                */
                st = cpssDxChDiagPrbsPortTransmitModeGet(dev, port, lane, &modeGet);

                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                            "cpssDxChDiagPrbsPortTransmitModeGet: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(mode, modeGet,
                               "got another mode then was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, mode);
            }

            /*
                1.1.3. Call with wrong enum values mode.
                Expected: GT_BAD_PARAM.
            */

            /* the feature supported for all Gig ports
               and XG ports of CH3 and above devices */
            if( IS_PORT_GE_E(dev, port) ||
                (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(dev) && IS_PORT_XG_E(dev, port)) )
            {
                UTF_ENUMS_CHECK_MAC(cpssDxChDiagPrbsPortTransmitModeSet
                                    (dev, port, lane, mode),
                                    mode);
            }
        }

        mode = CPSS_DXCH_DIAG_TRANSMIT_MODE_REGULAR_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChDiagPrbsPortTransmitModeSet(dev, port, lane, mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChDiagPrbsPortTransmitModeSet(dev, port, lane, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChDiagPrbsPortTransmitModeSet(dev, port, lane, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port   = DIAG_VALID_PORTNUM_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagPrbsPortTransmitModeSet(dev, port, lane, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagPrbsPortTransmitModeSet(dev, port, lane, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPrbsPortTransmitModeGet
(
    IN   GT_U8                    devNum,
    IN   GT_U8                    portNum,
    IN   GT_U32                   laneNum,
    OUT  CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT   *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPrbsPortTransmitModeGet)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxChx)
    1.1.1. Call with not-NULL mode.
    Expected: GT_OK.
    1.1.2. Call modePtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = DIAG_VALID_PORTNUM_CNS;

    GT_U32      lane = 0;
    CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT mode;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with not-NULL mode.
                Expected: GT_OK.
            */
            st = cpssDxChDiagPrbsPortTransmitModeGet(dev, port, lane, &mode);

            /* the feature supported for all Gig ports
               and XG ports of CH3 and above devices */
            if( IS_PORT_GE_E(dev, port) ||
                (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(dev) && IS_PORT_XG_E(dev, port)) )
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. Call modePtr[NULL].
                    Expected: GT_BAD_PTR.
                */
                st = cpssDxChDiagPrbsPortTransmitModeGet(dev, port, lane, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                             "%d, %d, modePtr = NULL", dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, mode);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChDiagPrbsPortTransmitModeGet(dev, port, lane, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChDiagPrbsPortTransmitModeGet(dev, port, lane, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChDiagPrbsPortTransmitModeGet(dev, port, lane, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = DIAG_VALID_PORTNUM_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagPrbsPortTransmitModeGet(dev, port, lane, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagPrbsPortTransmitModeGet(dev, port, lane, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagRegTest
(
    IN GT_U8                          devNum,
    IN GT_U32                         regAddr,
    IN GT_U32                         regMask,
    IN CPSS_DIAG_TEST_PROFILE_ENT     profile,
    OUT GT_BOOL                       *testStatusPtr,
    OUT GT_U32                        *readValPtr,
    OUT GT_U32                        *writeValPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagRegTest)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1.1. Call with not-NULL pointers.
    Expected: GT_OK.
    1.1.2. Call wrong enum values profile.
    Expected: GT_BAD_PARAM.
    1.1.3. Call testStatusPtr[NULL].
    Expected: GT_BAD_PTR.
    1.1.4. Call readValPtr[NULL].
    Expected: GT_BAD_PTR.
    1.1.5. Call writeValPtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;

    GT_U32                         regAddr = 0;
    GT_U32                         regMask = 0;
    CPSS_DIAG_TEST_PROFILE_ENT     profile = CPSS_DIAG_TEST_RANDOM_E;
    GT_BOOL                        testStatus;
    GT_U32                         readVal;
    GT_U32                         writeVal;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1.1. Call with not-NULL pointers.
            Expected: GT_OK.
        */
        st = cpssDxChDiagRegTest(dev, regAddr, regMask, profile,
                                 &testStatus, &readVal, &writeVal);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /*
            1.1.2. Call wrong enum values profile.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChDiagRegTest
                            (dev, regAddr, regMask, profile, &testStatus, &readVal, &writeVal),
                            profile);

        /*
            1.1.3. Call testStatusPtr[NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChDiagRegTest(dev, regAddr, regMask, profile,
                                 NULL, &readVal, &writeVal);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

        /*
            1.1.4. Call readValPtr[NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChDiagRegTest(dev, regAddr, regMask, profile,
                                 &testStatus, NULL, &writeVal);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

        /*
            1.1.5. Call writeValPtr[NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChDiagRegTest(dev, regAddr, regMask, profile,
                                 &testStatus, &readVal, NULL);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagRegTest(dev, regAddr, regMask, profile,
                                 &testStatus, &readVal, &writeVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagRegTest(dev, regAddr, regMask, profile,
                             &testStatus, &readVal, &writeVal);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagRegsDump
(
    IN    GT_U8     devNum,
    INOUT GT_U32    *regsNumPtr,
    IN    GT_U32    offset,
    OUT   GT_U32    *regAddrPtr,
    OUT   GT_U32    *regDataPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagRegsDump)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1.1. Call with not-NULL pointers.
    Expected: GT_OK.
    1.1.2. Call regsNumPtr[NULL].
    Expected: GT_BAD_PTR.
    1.1.3. Call regAddrPtr[NULL].
    Expected: GT_BAD_PTR.
    1.1.4. Call regDataPtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;

    GT_U32    regsNum = 0;
    GT_U32    offset = 0;
    GT_U32    regAddr= 0;
    GT_U32    regData[1] = { 0 };


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1.1. Call with not-NULL pointers.
            Expected: GT_OK.
        */
        st = cpssDxChDiagRegsDump(dev, &regsNum, offset, &regAddr, regData);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /*
            1.1.2. Call regsNumPtr[NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChDiagRegsDump(dev, NULL, offset, &regAddr, regData);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

        /*
            1.1.3. Call regAddr[NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChDiagRegsDump(dev, &regsNum, offset, NULL, regData);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

        /*
            1.1.4. Call regData[NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChDiagRegsDump(dev, &regsNum, offset, &regAddr, NULL);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagRegsDump(dev, &regsNum, offset, &regAddr, regData);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagRegsDump(dev, &regsNum, offset, &regAddr, regData);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagRegsNumGet
(
    IN  GT_U8     devNum,
    OUT GT_U32    *regsNumPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagRegsNumGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1.1. Call with not-NULL pointers.
    Expected: GT_OK.
    1.1.2. Call regsNumPtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U32      regsNum;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1.1. Call with not-NULL pointers.
            Expected: GT_OK.
        */
        st = cpssDxChDiagRegsNumGet(dev, &regsNum);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /*
            1.1.2. Call regsNumPtr[NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChDiagRegsNumGet(dev, NULL);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagRegsNumGet(dev, &regsNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagRegsNumGet(dev, &regsNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPrbsCyclicDataSet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    IN   GT_U32         cyclicDataArr[4]
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPrbsCyclicDataSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORT (DxCh3 and above)
    1.1.1. Call with correct params.
    Expected: GT_OK.
    1.1.2. Call cpssDxChDiagPrbsCyclicDataGet with the same params.
    Expected: GT_OK and the same cyclicDataArr as was set.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       i = 0;
    GT_U8       port = DIAG_VALID_PORTNUM_CNS;

    GT_U32      lane = 0;
    GT_U32      cyclicDataArr[4] = {0,1,2,3};
    GT_U32      cyclicDataArrGet[4] = {1,2,3,4};

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with correct params.
                Expected: GT_OK.
            */

            st = cpssDxChDiagPrbsCyclicDataSet(dev, port, lane, cyclicDataArr);

            if( IS_PORT_XG_E(dev, port) )
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. Call cpssDxChDiagPrbsCyclicDataGet.
                    Expected: GT_OK and the same cyclicDataArr as was set.
                */
                st = cpssDxChDiagPrbsCyclicDataGet(dev, port, lane, cyclicDataArrGet);

                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                            "cpssDxChDiagPrbsCyclicDataGet: %d, %d", dev, port);

                for(i = 0; i < 4; i++)
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(cyclicDataArr[i], cyclicDataArrGet[i],
                          "got another cyclicDataArr then was set: %d, %d", dev, port);
                }
            }
            else
            {
                /* not supported not XG_E ports*/
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChDiagPrbsCyclicDataSet(dev, port, lane, cyclicDataArr);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChDiagPrbsCyclicDataSet(dev, port, lane, cyclicDataArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChDiagPrbsCyclicDataSet(dev, port, lane, cyclicDataArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port   = DIAG_VALID_PORTNUM_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagPrbsCyclicDataSet(dev, port, lane, cyclicDataArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagPrbsCyclicDataSet(dev, port, lane, cyclicDataArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPrbsCyclicDataGet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    OUT  GT_U32         cyclicDataArr[4]
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPrbsCyclicDataGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORT (DxCh3 and above)
    1.1.1. Call with correct params.
    Expected: GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = DIAG_VALID_PORTNUM_CNS;
    GT_U32      lane = 0;
    GT_U32      cyclicDataArr[4] = {0,1,2,3};

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with correct params.
                Expected: GT_OK.
            */
            st = cpssDxChDiagPrbsCyclicDataGet(dev, port, lane, cyclicDataArr);

            if( IS_PORT_XG_E(dev, port) )
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
            else
            {
                /* not supported other ports*/
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChDiagPrbsCyclicDataGet(dev, port, lane, cyclicDataArr);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChDiagPrbsCyclicDataGet(dev, port, lane, cyclicDataArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChDiagPrbsCyclicDataGet(dev, port, lane, cyclicDataArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port   = DIAG_VALID_PORTNUM_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagPrbsCyclicDataGet(dev, port, lane, cyclicDataArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagPrbsCyclicDataGet(dev, port, lane, cyclicDataArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPhyRegRead
(
    IN GT_U32                          baseAddr,
    IN CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel,
    IN GT_U32                          smiRegOffset,
    IN GT_U32                          phyAddr,
    IN GT_U32                          offset,
    OUT GT_U32                         *dataPtr,
    IN GT_BOOL                         doByteSwap
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPhyRegRead)
{
/*
    1.1.1. Call with data=NULL
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;

    GT_U32                          baseAddr = 0;
    CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel = 0;
    GT_U32                          smiRegOffset = 0;
    GT_U32                          phyAddr = 0;
    GT_U32                          offset = 0;
    GT_BOOL                         doByteSwap = GT_TRUE;

    st = cpssDxChDiagPhyRegRead(baseAddr, ifChannel, smiRegOffset,
                                  phyAddr, offset, NULL, doByteSwap);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPhyRegWrite
(
    IN GT_U32                          baseAddr,
    IN CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel,
    IN GT_U32                          smiRegOffset,
    IN GT_U32                          phyAddr,
    IN GT_U32                          offset,
    IN GT_U32                          data,
    IN GT_BOOL                         doByteSwap
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPhyRegWrite)
{
/*
    1.1.1. Call with phyAddr = 33
    Expected: GT_OK.
*/
    GT_STATUS   st   = GT_BAD_PARAM;

    GT_U32                          baseAddr = 0;
    CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel = 0;
    GT_U32                          smiRegOffset = 0;
    GT_U32                          phyAddr = 33;
    GT_U32                          offset = 0;
    GT_U32                          data = 0;
    GT_BOOL                         doByteSwap = GT_TRUE;


    st = cpssDxChDiagPhyRegWrite(baseAddr, ifChannel, smiRegOffset,
                                  phyAddr, offset, data, doByteSwap);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PARAM, st);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagRegRead
(
    IN GT_U32                          baseAddr,
    IN CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel,
    IN CPSS_DIAG_PP_REG_TYPE_ENT       regType,
    IN GT_U32                          offset,
    OUT GT_U32                         *dataPtr,
    IN GT_BOOL                         doByteSwap
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagRegRead)
{
/*
    1.1.1. Call with dataPtr == NULL
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;

    GT_U32                          baseAddr = 0;
    CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel = 0;
    CPSS_DIAG_PP_REG_TYPE_ENT       regType = 0;
    GT_U32                          offset = 0;
    GT_BOOL                         doByteSwap = GT_TRUE;

    st = cpssDxChDiagRegRead(baseAddr, ifChannel, regType, offset,
                             NULL, doByteSwap);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagRegWrite
(
    IN GT_U32                          baseAddr,
    IN CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel,
    IN CPSS_DIAG_PP_REG_TYPE_ENT       regType,
    IN GT_U32                          offset,
    IN GT_U32                          data,
    IN GT_BOOL                         doByteSwap
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagRegWrite)
{
/*
    1.1.1. Call with ifChannel not valid
    Expected: GT_OK.
*/
    GT_STATUS   st   = GT_BAD_PARAM;

    GT_U32                          baseAddr = 0;
    CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel = 0x55555;
    CPSS_DIAG_PP_REG_TYPE_ENT       regType = 0;
    GT_U32                          offset = 0;
    GT_U32                          data = 0;
    GT_BOOL                         doByteSwap = GT_TRUE;


    st = cpssDxChDiagRegWrite(baseAddr, ifChannel, regType, offset, data, doByteSwap);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PARAM, st);
}

static void prvUtfDxChPortSerdesNumGet(GT_U8 dev, GT_U8 port, GT_U32 *serdesNum)
{
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode = CPSS_PORT_INTERFACE_MODE_NA_E;
    GT_STATUS st;
    st = cpssDxChPortInterfaceModeGet(dev, port, &ifMode);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChPortInterfaceModeGet: %d, %d",
                                 dev, port);
    switch (ifMode)
    {
        case CPSS_PORT_INTERFACE_MODE_SGMII_E:
        case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
        case CPSS_PORT_INTERFACE_MODE_QX_E:
        case CPSS_PORT_INTERFACE_MODE_100BASE_FX_E:
        case CPSS_PORT_INTERFACE_MODE_QSGMII_E:
            if((PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) && 
               (PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(dev,port) == GT_TRUE))
            {   /* xCat Flex ports have 4 SERDESes even part of them are not used */
                *serdesNum = 4;
            }
            else
                *serdesNum = 1;
            break;
        case CPSS_PORT_INTERFACE_MODE_HX_E:
        case CPSS_PORT_INTERFACE_MODE_RXAUI_E:
            *serdesNum = 2;
            break;                     
        case CPSS_PORT_INTERFACE_MODE_XGMII_E:
            *serdesNum = 4;
            break;
        case CPSS_PORT_INTERFACE_MODE_XLG_E:
            *serdesNum = 8;
            break;
        default: 
            *serdesNum = 0;
            break;
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPrbsSerdesStatusGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    IN   GT_U32    laneNum,
    OUT  GT_BOOL  *lockedPtr,
    OUT  GT_U32   *errorCntrPtr,
    OUT  GT_U64   *patternCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPrbsSerdesStatusGet)
{
/*
    ITERATE_DEVICES_PHY_PORT (xCat and above)
    1.1.1. Call with laneNum [0 - 3] and not-NULL pointers.
    Expected: GT_OK.
    1.1.2. Call with wrong laneNum [DIAG_INVALID_LANENUM_CNS].
    Expected: NOT GT_OK for flexLink ports and GT_OK for other.
    1.1.3. Call  witn wrong checkerLockedPtr[NULL].
    Expected: GT_BAD_PTR.
    1.1.4. Call witn wrong errorCntrPtr[NULL].
    Expected: GT_BAD_PTR.
    1.1.5. Call witn wrong patternCntrPtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;

    GT_U8       dev;
    GT_U8       port = DIAG_VALID_PORTNUM_CNS;
    GT_U32      lane      = 0;
    GT_BOOL     checkerLocked;
    GT_U32      errorCntr;
    GT_U64      patternCntr;
    GT_U32      serdesNum;
    CPSS_PP_FAMILY_TYPE_ENT devFamily;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            prvUtfDxChPortSerdesNumGet(dev, port, &serdesNum);
            /*
                1.1.1. Call with laneNum [0 - 3] and not-NULL pointers.
                Expected: GT_OK.
            */
            for(lane = 0; lane < serdesNum; lane++)
            {
                st = cpssDxChDiagPrbsSerdesStatusGet(dev, port, lane,
                                        &checkerLocked, &errorCntr, &patternCntr);
                if(PRV_CPSS_DXCH_PORT_TYPE_MAC(dev,port) != PRV_CPSS_PORT_FE_E)
                {
                    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, lane);
                }
                else
                {
                    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, lane);
                }

            }

            /*
                1.1.2. Call with wrong laneNum [DIAG_INVALID_LANENUM_CNS].
                Expected: NOT GT_OK for flexLink ports and GT_OK for other.
            */
            lane = serdesNum;

            st = cpssDxChDiagPrbsSerdesStatusGet(dev, port, lane,
                                    &checkerLocked, &errorCntr, &patternCntr);

            if(PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(dev,port) == GT_TRUE)
            {
                if(CPSS_PP_FAMILY_DXCH_XCAT2_E != devFamily)
                    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
                else
                    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, lane);
            }
            else if(PRV_CPSS_DXCH_PORT_TYPE_MAC(dev,port) == PRV_CPSS_PORT_GE_E)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, lane);
            }
            else /*FE port */
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, lane);
            }

            lane = 0;

            /*
                1.1.3. Call  witn wrong checkerLockedPtr[NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChDiagPrbsSerdesStatusGet(dev, port, lane,
                                                 NULL, &errorCntr, &patternCntr);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                         "%d, %d, checkerLockedPtr = NULL", dev, port);

            /*
                1.1.4. Call witn wrong errorCntrPtr[NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChDiagPrbsSerdesStatusGet(dev, port, lane,
                                                 &checkerLocked, NULL, &patternCntr);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                         "%d, %d, errorCntr = NULL", dev, port);

            /*
                1.1.5. Call witn wrong patternCntr[NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChDiagPrbsSerdesStatusGet(dev, port, lane,
                                                 &checkerLocked, &errorCntr, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                         "%d, %d, patternCntr = NULL", dev, port);
        }

        lane = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChDiagPrbsSerdesStatusGet(dev, port, lane,
                                        &checkerLocked, &errorCntr, &patternCntr);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChDiagPrbsSerdesStatusGet(dev, port, lane,
                                    &checkerLocked, &errorCntr, &patternCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChDiagPrbsSerdesStatusGet(dev, port, lane,
                                    &checkerLocked, &errorCntr, &patternCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = DIAG_VALID_PORTNUM_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagPrbsSerdesStatusGet(dev, port, lane,
                                    &checkerLocked, &errorCntr, &patternCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagPrbsSerdesStatusGet(dev, port, lane,
                                   &checkerLocked, &errorCntr, &patternCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPrbsSerdesTestEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_U32     laneNum,
    IN  GT_BOOL    enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPrbsSerdesTestEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORT (xCat and above)
    1.1.1. Call with laneNum [0 - 3] enable[GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call with wrong laneNum [DIAG_INVALID_LANENUM_CNS].
    Expected: NOT GT_OK for flexLink ports and GT_OK for other.
    1.1.3. Call cpssDxChDiagPrbsSerdesTestEnableGet with not-NULL enablePtr.
    Expected: GT_OK and the same enable as was set.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = DIAG_VALID_PORTNUM_CNS;

    GT_U32      lane      = 0;
    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;
    GT_U32      serdesNum;

    CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT mode = CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {

            prvUtfDxChPortSerdesNumGet(dev, port, &serdesNum);
            /*
                1.1.1. Call with enable[GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            for(lane = 0; lane < serdesNum; lane++)
            {
                /* Transmit mode should be set before enabling test mode */
                st = cpssDxChDiagPrbsSerdesTransmitModeSet(dev, port, lane, mode);
                if(PRV_CPSS_DXCH_PORT_TYPE_MAC(dev,port) != PRV_CPSS_PORT_FE_E)
                {
                    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
                }
                else /* FE port */
                {
                    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, enable);
                }

                for(enable = GT_FALSE; enable <= GT_TRUE; enable++)
                {
                    st = cpssDxChDiagPrbsSerdesTestEnableSet(dev, port, lane, enable);

                    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(dev,port) != PRV_CPSS_PORT_FE_E)
                    {
                        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
                    }
                    else /* FE port */
                    {
                        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, enable);
                    }

                    /*
                        1.1.2. Call cpssDxChDiagPrbsSerdesTestEnableGet with not-NULL enablePtr.
                        Expected: GT_OK and the same enable as was set.
                    */
                    st = cpssDxChDiagPrbsSerdesTestEnableGet(dev, port, lane, &enableGet);

                    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(dev,port) != PRV_CPSS_PORT_FE_E)
                    {
                        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                    "cpssDxChDiagPrbsSerdesTestEnableGet: %d, %d", dev, port);
                        UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                                       "got another enable then was set: %d, %d", dev, port);
                    }
                    else /* FE port */
                    {
                        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, enable);
                    }
                }
        }
        }

        lane = 0;
        enable = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChDiagPrbsSerdesTestEnableSet(dev, port, lane, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChDiagPrbsSerdesTestEnableSet(dev, port, lane, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChDiagPrbsSerdesTestEnableSet(dev, port, lane, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    lane = 0;
    enable = GT_FALSE;
    port   = DIAG_VALID_PORTNUM_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagPrbsSerdesTestEnableSet(dev, port, lane, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagPrbsSerdesTestEnableSet(dev, port, lane, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPrbsSerdesTestEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_U32     laneNum,
    OUT GT_BOOL    *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPrbsSerdesTestEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORT (xCat and above)
    1.1.1. Call with lane [0 - 3] and not-NULL enable.
    Expected: GT_OK.
    1.1.2. Call enablePtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = DIAG_VALID_PORTNUM_CNS;

    GT_U32      lane      = 0;
    GT_BOOL     enable    = GT_FALSE;
    GT_U32      serdesNum;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            prvUtfDxChPortSerdesNumGet(dev, port, &serdesNum);
            /*
                1.1.1. Call with lane [0 - 3] and not-NULL enable.
                Expected: GT_OK.
            */
            for(lane = 0; lane < serdesNum; lane++)
            {
                st = cpssDxChDiagPrbsSerdesTestEnableGet(dev, port, lane, &enable);

                /* the feature supported for all Gig ports
                   and XG ports of CH3 and above devices */
                if( IS_PORT_GE_E(dev, port) ||
                    (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(dev) && IS_PORT_XG_E(dev, port)) )
                {
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                    /*
                        1.1.2. Call enablePtr[NULL].
                        Expected: GT_BAD_PTR.
                    */
                    st = cpssDxChDiagPrbsSerdesTestEnableGet(dev, port, lane, NULL);
                    UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
                }
                else
                {
                    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, enable);
                }
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChDiagPrbsSerdesTestEnableGet(dev, port, lane, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        lane = 0;
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChDiagPrbsSerdesTestEnableGet(dev, port, lane, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChDiagPrbsSerdesTestEnableGet(dev, port, lane, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = DIAG_VALID_PORTNUM_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagPrbsSerdesTestEnableGet(dev, port, lane, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagPrbsSerdesTestEnableGet(dev, port, lane, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPrbsSerdesTransmitModeSet
(
    IN   GT_U8                    devNum,
    IN   GT_U8                    portNum,
    IN   GT_U32                   laneNum,
    IN   CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT   mode
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPrbsSerdesTransmitModeSet)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxChx)
    1.1.1. Call with laneNum [0 - 3] and mode [CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E].
    Expected: GT_OK.
    1.1.2. Call cpssDxChDiagPrbsSerdesTransmitModeGet with not-NULL modePtr.
    Expected: GT_OK and the same mode as was set.
    1.1.3. Call with wrong laneNum [DIAG_INVALID_LANENUM_CNS].
    Expected: NOT GT_OK.
    1.1.4. Call with wrong mode.
    Expected: GT_BAD_PARAM.
    1.1.5. Call with wrong mode [CPSS_DXCH_DIAG_TRANSMIT_MODE_REGULAR_E].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = DIAG_VALID_PORTNUM_CNS;

    GT_U32      lane = 0;
    CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT mode = 0;
    CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT modeGet = 1;
    GT_U32      serdesNum;
    CPSS_PP_FAMILY_TYPE_ENT devFamily;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            prvUtfDxChPortSerdesNumGet(dev, port, &serdesNum);
            /*
                1.1.1. Call with mode [CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E].
                Expected: GT_OK.
            */
            mode = CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E;

            for(lane = 0; lane < serdesNum; lane++)
            {
                st = cpssDxChDiagPrbsSerdesTransmitModeSet(dev, port, lane, mode);

                if(PRV_CPSS_DXCH_PORT_TYPE_MAC(dev,port) != PRV_CPSS_PORT_FE_E)
                {
                    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);

                    /*
                        1.1.2. Call cpssDxChDiagPrbsSerdesTransmitModeGet with not-NULL modePtr.
                        Expected: GT_OK and the same mode as was set.
                    */
                    st = cpssDxChDiagPrbsSerdesTransmitModeGet(dev, port, lane, &modeGet);

                    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                "cpssDxChDiagPrbsSerdesTransmitModeGet: %d, %d", dev, port);
                    UTF_VERIFY_EQUAL2_STRING_MAC(mode, modeGet,
                                   "got another mode then was set: %d, %d", dev, port);
                }
                else
                {
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
                }

            }

            /*
                1.1.3. Call with wrong laneNum[DIAG_INVALID_LANENUM_CNS].
                Expected: NOT GT_OK for flexLink ports and GT_OK for others.
            */

            lane = serdesNum;

            st = cpssDxChDiagPrbsSerdesTransmitModeSet(dev, port, lane, mode);

            if(PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(dev,port) == GT_TRUE)
            {
                if(CPSS_PP_FAMILY_DXCH_XCAT2_E != devFamily)
                    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
                else
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
            else if(PRV_CPSS_DXCH_PORT_TYPE_MAC(dev,port) == PRV_CPSS_PORT_GE_E)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
            else /* FE Port */
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }

            lane = 0;


            /*
                1.1.5. Call with wrong mode, not CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E.
                Expected: NOT GT_OK.
            */

            for(mode = 0; mode < 7; mode++)
            {
                if(mode != CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E)
                {
                    st = cpssDxChDiagPrbsSerdesTransmitModeSet(dev, port, lane, mode);
                    UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);
                }

            }

            mode = CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E;
        }

        lane = 0;
        mode = CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChDiagPrbsSerdesTransmitModeSet(dev, port, lane, mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChDiagPrbsSerdesTransmitModeSet(dev, port, lane, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChDiagPrbsSerdesTransmitModeSet(dev, port, lane, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port   = DIAG_VALID_PORTNUM_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagPrbsSerdesTransmitModeSet(dev, port, lane, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagPrbsSerdesTransmitModeSet(dev, port, lane, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPrbsSerdesTransmitModeGet
(
    IN   GT_U8                    devNum,
    IN   GT_U8                    portNum,
    IN   GT_U32                   laneNum,
    OUT  CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT   *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPrbsSerdesTransmitModeGet)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxChx)
    1.1.1. Call with not-NULL mode.
    Expected: GT_OK.
    1.1.2. Call with wrong laneNum[DIAG_INVALID_LANENUM_CNS].
    Expected: NOT GT_OK for flexLink ports and GT_OK for others.
    1.1.3. Call modePtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = DIAG_VALID_PORTNUM_CNS;

    GT_U32      lane = 0;
    CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT mode;
    GT_U32      serdesNum;
    CPSS_PP_FAMILY_TYPE_ENT devFamily;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            prvUtfDxChPortSerdesNumGet(dev, port, &serdesNum);
            /*
                1.1.1. Call with not-NULL mode.
                Expected: GT_OK
            */
            for(lane = 0; lane < serdesNum; lane++)
            {
                st = cpssDxChDiagPrbsSerdesTransmitModeSet(dev, port, lane, CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E);
                if(PRV_CPSS_DXCH_PORT_TYPE_MAC(dev,port) != PRV_CPSS_PORT_FE_E)
                {
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
                }
                else /* FE Port */
                {
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
                }

                st = cpssDxChDiagPrbsSerdesTransmitModeGet(dev, port, lane, &mode);
                if(PRV_CPSS_DXCH_PORT_TYPE_MAC(dev,port) != PRV_CPSS_PORT_FE_E)
                {
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
                }
                else /* FE Port */
                {
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
                }

            }

            /*
                1.1.2. Call with wrong laneNum[DIAG_INVALID_LANENUM_CNS].
                Expected: NOT GT_OK for flexLink ports and GT_OK for others.
            */

            lane = serdesNum;
            st = cpssDxChDiagPrbsSerdesTransmitModeSet(dev, port, lane, CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E);
            if(PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(dev,port) == GT_TRUE)
            {
                if(CPSS_PP_FAMILY_DXCH_XCAT2_E != devFamily)
                    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
                else
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
            else if(PRV_CPSS_DXCH_PORT_TYPE_MAC(dev,port) == PRV_CPSS_PORT_GE_E)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }

            st = cpssDxChDiagPrbsSerdesTransmitModeGet(dev, port, lane, &mode);

            if(PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(dev,port) == GT_TRUE)
            {
                if(CPSS_PP_FAMILY_DXCH_XCAT2_E != devFamily)
                    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
                else
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
            else if(PRV_CPSS_DXCH_PORT_TYPE_MAC(dev,port) == PRV_CPSS_PORT_GE_E)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }

            lane = 0;

            /*
                1.1.2. Call with wrong modePtr[NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChDiagPrbsSerdesTransmitModeGet(dev, port, lane, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                         "%d, %d, modePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChDiagPrbsSerdesTransmitModeGet(dev, port, lane, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChDiagPrbsSerdesTransmitModeGet(dev, port, lane, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChDiagPrbsSerdesTransmitModeGet(dev, port, lane, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = DIAG_VALID_PORTNUM_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagPrbsSerdesTransmitModeGet(dev, port, lane, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagPrbsSerdesTransmitModeGet(dev, port, lane, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagMemoryBistBlockStatusGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DIAG_PP_MEM_BIST_TYPE_ENT  memBistType,
    IN  GT_U32                          blockIndex,
    OUT GT_BOOL                         *blockFixedPtr,
    OUT GT_U32                          *replacedIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagMemoryBistBlockStatusGet)
{
/*
    ITERATE_DEVICES (DxCh3)
    1.1. Call with memBistType [CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E /
                                CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E],
                                and blockIndex [0].
    Expected: GT_OK.
    1.2. Call with memBistType [CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E],
                                and blockIndex [13].
    Expected: GT_OK.
    1.3. Call with memBistType [CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E],
                                and blockIndex [19].
    Expected: GT_OK.
    1.4. Call with memBistType [CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E],
                                and  out of range blockIndex [14].
    Expected: NOT GT_OK.
    1.5. Call with memBistType [CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E],
                                and  out of range blockIndex [20].
    Expected: NOT GT_OK.
    1.6. Call with wrong enum values memBistType.
    Expected: GT_BAD_PARAM.
    1.7. Call with wrong blockFixedPtr [NULL].
    Expected: GT_BAD_PTR.
    1.8. Call with wrong replacedIndex [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;

    CPSS_DIAG_PP_MEM_BIST_TYPE_ENT  memBistType = CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E;
    GT_U32                          blockIndex = 0;
    GT_BOOL                         blockFixed;
    GT_U32                          replacedIndex;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with memBistType [CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E /
                                        CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E],
                                        and blockIndex [0].
            Expected: GT_OK
        */
        blockIndex  = 0;

        /*call with memBistType = CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E */
        memBistType = CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E;

        st = cpssDxChDiagMemoryBistBlockStatusGet(dev, memBistType,
                                    blockIndex, &blockFixed, &replacedIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memBistType);

        /*call with memBistType = CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E */
        memBistType = CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E;

        st = cpssDxChDiagMemoryBistBlockStatusGet(dev, memBistType,
                                    blockIndex, &blockFixed, &replacedIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memBistType);

        /*
            1.2. Call with memBistType [CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E],
                                        and blockIndex [13].
            Expected: GT_OK.
        */
        blockIndex  = 13;
        memBistType = CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E;

        st = cpssDxChDiagMemoryBistBlockStatusGet(dev, memBistType,
                                    blockIndex, &blockFixed, &replacedIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memBistType);

        blockIndex  = 0;

        /*
            1.3. Call with memBistType [CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E],
                                        and blockIndex [19].
            Expected: GT_OK.
        */
        blockIndex  = 19;
        memBistType = CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E;

        st = cpssDxChDiagMemoryBistBlockStatusGet(dev, memBistType,
                                    blockIndex, &blockFixed, &replacedIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memBistType);

        blockIndex  = 0;

        /*
            1.4. Call with memBistType [CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E],
                                        and  out of range blockIndex [14].
            Expected: NOT GT_OK.
        */
        blockIndex  = 14;
        memBistType = CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E;

        st = cpssDxChDiagMemoryBistBlockStatusGet(dev, memBistType,
                                    blockIndex, &blockFixed, &replacedIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, memBistType);

        blockIndex  = 0;

        /*
            1.5. Call with memBistType [CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E],
                                        and  out of range blockIndex [20].
            Expected: NOT GT_OK.
        */
        blockIndex  = 20;
        memBistType = CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E;

        st = cpssDxChDiagMemoryBistBlockStatusGet(dev, memBistType,
                                    blockIndex, &blockFixed, &replacedIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, memBistType);

        blockIndex  = 0;

        /*
            1.6. Call with wrong enum values memBistType.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChDiagMemoryBistBlockStatusGet
                            (dev, memBistType, blockIndex, &blockFixed, &replacedIndex),
                            memBistType);

        /*
            1.7. Call with wrong blockFixedPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChDiagMemoryBistBlockStatusGet(dev, memBistType,
                                    blockIndex, NULL, &replacedIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.8. Call with wrong replacedIndex [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChDiagMemoryBistBlockStatusGet(dev, memBistType,
                                    blockIndex, &blockFixed, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagMemoryBistBlockStatusGet(dev, memBistType,
                                    blockIndex, &blockFixed, &replacedIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagMemoryBistBlockStatusGet(dev, memBistType,
                                blockIndex, &blockFixed, &replacedIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagMemoryBistsRun
(
    IN  GT_U8                           devNum,
    IN  CPSS_DIAG_PP_MEM_BIST_TYPE_ENT  memBistType,
    IN  GT_U32                          timeOut,
    IN  GT_BOOL                         clearMemoryAfterTest
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagMemoryBistsRun)
{
/*
    ITERATE_DEVICES (DxCh3)
    1.1. Call with memBistType [CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E /
                                CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E],
                       timeOut [0 / 10]
                   and clearMemoryAfterTest[GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call with wrong enum values memBistType.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;

    CPSS_DIAG_PP_MEM_BIST_TYPE_ENT  memBistType = CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E;
    GT_U32                          timeOut = 0;
    GT_BOOL                         clearMemoryAfterTest = GT_TRUE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(
        &dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E
        | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with memBistType [CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E /
                                        CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E],
                               timeOut [0 / 10]
                               and clearMemoryAfterTest[GT_FALSE / GT_TRUE].
            Expected: GT_OK
        */

        /* call with memBistType = CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E */
        memBistType = CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E;
        timeOut = 0;
        clearMemoryAfterTest = GT_FALSE;

        st = cpssDxChDiagMemoryBistsRun(dev, memBistType, timeOut, clearMemoryAfterTest);
        st = (GT_TIMEOUT == st) ? GT_OK : st;
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memBistType);

        /* call with memBistType = CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E */
        memBistType = CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E;
        timeOut = 10;
        clearMemoryAfterTest = GT_TRUE;

        st = cpssDxChDiagMemoryBistsRun(dev, memBistType, timeOut, clearMemoryAfterTest);
        st = (GT_TIMEOUT == st) ? GT_OK : st;
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memBistType);

        /*
            1.2. Call with wrong enum values memBistType.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChDiagMemoryBistsRun
                            (dev, memBistType, timeOut, clearMemoryAfterTest),
                            memBistType);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT_E | UTF_XCAT2_E);

    timeOut = 10;

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagMemoryBistsRun(dev, memBistType, timeOut, clearMemoryAfterTest);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagMemoryBistsRun(dev, memBistType, timeOut, clearMemoryAfterTest);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPortGroupMemWrite
(
    IN GT_U8                          devNum,
    IN GT_PORT_GROUPS_BMP             portGroupsBmp,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN GT_U32                         offset,
    IN GT_U32                         data
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPortGroupMemWrite)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Write / Read memory word for each port group,
         Call with memType = [CPSS_DIAG_PP_MEM_BUFFER_DRAM_E /
                              CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E /
                              CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E ],
                  offset [0], and non-NULL dataPtr.
    Expected: GT_OK.
    1.2. Write memory word with CPSS_PORT_GROUP_UNAWARE_MODE_CNS
         and read for each port group.
    Expected: GT_OK and the same data.
    1.3. Write different memory words for each port groups. Read memory word with
        CPSS_PORT_GROUP_UNAWARE_MODE_CNS, it should be equal to one from
        port group 0.
    Expected: GT_OK and the same data.
    1.4. Write to specific port group and check that other port groups are not
    influenced.
    Expected: GT_OK and the same data.
    1.5. Call with wrong enum values memType and other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with dataPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DIAG_PP_MEM_TYPE_ENT  memType = CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E;
    GT_U32                     offset = 0;
    GT_U32                     data = 0;
    GT_U32                     readData = 0;
    GT_PORT_GROUPS_BMP         portGroupsBmp = 1;
    CPSS_DIAG_PP_MEM_TYPE_ENT  memTypesArray[3];
    GT_U32                     portGroupId;
    GT_U32                     memTypeId;
    GT_U32                     memTypeLen = 3;
    GT_U32                     i;
    CPSS_PP_FAMILY_TYPE_ENT    devFamily = CPSS_PP_FAMILY_CHEETAH_E;

    memTypesArray[0] = CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E;
    memTypesArray[1] = CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E;
    memTypesArray[2] = CPSS_DIAG_PP_MEM_BUFFER_DRAM_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);


    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /*
            1.1. Write / Read memory word for each port group,
                 Call with memType = [CPSS_DIAG_PP_MEM_BUFFER_DRAM_E /
                                      CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E /
                                      CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E ],
                          offset [0], and non-NULL dataPtr.
            Expected: GT_OK.
        */

#ifdef ASIC_SIMULATION
    if (devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
    {
        /* do not test CPSS_DIAG_PP_MEM_BUFFER_DRAM_E */
        memTypeLen = 2;
    }
#endif /* ASIC_SIMULATION*/

        for (memTypeId = 0; memTypeId < memTypeLen; memTypeId++)
        {
            memType = memTypesArray[memTypeId];

           PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev,portGroupId)
           {
               portGroupsBmp = (1 << portGroupId);

               /* In Lion Egress VLAN exist only in two port groups */
               if ((memType == CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E) &&
                   (devFamily == CPSS_PP_FAMILY_DXCH_LION_E))
               {
                   if ((0x3 & portGroupsBmp) == 0x0)
                   {
                       break;
                   }
               }

                data = 0xAAAA5555;
                offset = 0x0;

                st = cpssDxChDiagPortGroupMemWrite(dev, portGroupsBmp,
                                                   memType, offset, data);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

                st = cpssDxChDiagPortGroupMemRead(dev, portGroupsBmp,
                                                   memType, offset, &readData);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

                UTF_VERIFY_EQUAL2_STRING_MAC(data, readData,
                   "The read data differs from written data: offset = %d, portGroupId = %d",
                                             offset, portGroupId);

                data = 0xFFFFFFFF;
                offset = 0x40;

                st = cpssDxChDiagPortGroupMemWrite(dev, portGroupsBmp,
                                                   memType, offset, data);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

                st = cpssDxChDiagPortGroupMemRead(dev, portGroupsBmp,
                                                   memType, offset, &readData);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

                UTF_VERIFY_EQUAL2_STRING_MAC(data, readData,
                  "The read data differs from written data: offset = %d, portGroupId = %d",
                                             offset, portGroupId);
           }
           PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        }

        /*
            1.2. Write memory word with CPSS_PORT_GROUP_UNAWARE_MODE_CNS
                 and read for each port group.
            Expected: GT_OK and the same data.
        */

        for (memTypeId = 0; memTypeId < memTypeLen; memTypeId++)
        {
            memType = memTypesArray[memTypeId];
            data = 0xAAAA5555;
            offset = 0x80;

            st = cpssDxChDiagPortGroupMemWrite(dev, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               memType, offset, data);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

            /* read for each port group */
           PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev,portGroupId)
           {
                /* set next port */
                portGroupsBmp = (1 << portGroupId);

                /* In Lion Egress VLAN exist only in two port groups */
                if (devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
                {
                    if ((0x3 & portGroupsBmp) == 0x0)
                    {
                        break;
                    }
                }

                st = cpssDxChDiagPortGroupMemRead(dev, portGroupsBmp,
                                                  memType, offset, &readData);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

                UTF_VERIFY_EQUAL2_STRING_MAC(data, readData,
                 "The read data differs from written data: offset = %d, portGroupId = %d",
                                             offset, portGroupId);
           }
           PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)
        }


        /*
            1.3. Write different memory words for each port groups. Read memory word with
                CPSS_PORT_GROUP_UNAWARE_MODE_CNS, it should be equal to one from
                port group 0.
                Note: In Lion B0, the Egress VLAN table is duplicated in core 0 and 1.
                Expected: GT_OK and the same data.
        */

        for (memTypeId = 0; memTypeId < memTypeLen; memTypeId++)
        {
            memType = memTypesArray[memTypeId];
            data = 0xAAAAAAAA;
            offset = 0x40;

            /* write to each port group */
            PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev,portGroupId)
            {
                /* set next port */
                portGroupsBmp = (1 << portGroupId);

                /* In Lion Egress VLAN exist only in two port groups */
                if ((memType == CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E) &&
                    (devFamily == CPSS_PP_FAMILY_DXCH_LION_E))
                {
                    if ((0x3 & portGroupsBmp) == 0x0)
                    {
                        break;
                    }
                }

                st = cpssDxChDiagPortGroupMemWrite(dev, portGroupsBmp, memType, offset, data);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

                data |= (0x55 << (portGroupId * 4));
            }
            PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)


            /* Verify that the read data equals to the read data from the port group 0 */
            st = cpssDxChDiagPortGroupMemRead(dev,
                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS, memType, offset, &readData);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

            if ((memType == CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E) &&
                (devFamily == CPSS_PP_FAMILY_DXCH_LION_E))
            {
                /* This will be the data written to portGroup 0 and 1*/
                data = 0xAAAAAAFF;
            }
            else
            {
                data = 0xAAAAAAAA;
            }

            UTF_VERIFY_EQUAL2_STRING_MAC(data, readData,
             "The read data differs from written data: offset = %d, portGroupId = %d",
                                         offset, portGroupId);
        }

        /*
            1.4. Write to specific port group and check that other port groups are not
            influenced.
            Expected: GT_OK and the same data.
        */
        memType = CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E;
        offset  = 0x0;
        data    = 0x0;

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev,portGroupId)
        {
            data = 0x0;

            /* write 0 to all port groups */
            st = cpssDxChDiagPortGroupMemWrite(dev, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               memType, offset, data);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

            /* write memory word to specific port group id */

            portGroupsBmp = (1 << portGroupId);

            data = 0x5555AAAA;

            st = cpssDxChDiagPortGroupMemWrite(dev, portGroupsBmp,
                                               memType, offset, data);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

            /* read data from other port groups and check that it was not changed */
            PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev,i)
            {
                if (portGroupId != i)
                {
                    portGroupsBmp = (1 << i);

                    st = cpssDxChDiagPortGroupMemRead(dev, portGroupsBmp,
                                                      memType, offset, &data);
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

                    UTF_VERIFY_EQUAL2_STRING_MAC(0x0, data,
                     "The read data differs from written data: offset = %d, portGroupId = %d",
                                                 offset, i);
                }
            }
            PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,i)
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /*
            1.5. Call with wrong enum values memType and other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        data = 0xAAAAAAAA;
        UTF_ENUMS_CHECK_MAC(cpssDxChDiagPortGroupMemWrite
                            (dev, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, memType, offset, data),
                            memType);

        /*
            1.6. Call with dataPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChDiagPortGroupMemRead(dev, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                          memType, offset, NULL);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, dev, memType);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagPortGroupMemWrite(dev, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                          memType, offset, data);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    st = cpssDxChDiagPortGroupMemWrite(dev, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                      memType, offset, data);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPortGroupMemRead
(
    IN GT_U8                          devNum,
    IN GT_PORT_GROUPS_BMP             portGroupsBmp,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN GT_U32                         offset,
    OUT GT_U32                        *dataPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPortGroupMemRead)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxChx)
    1.1.1.  Call with memType = [ CPSS_DIAG_PP_MEM_BUFFER_DRAM_E /
                                CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E /
                                CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E ],
                  offset [0 / 100 / 1000], and non-NULL dataPtr.
    Expected: GT_OK.
    1.1.2. Call with wrong enum values memType and other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.1.3. Call with dataPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DIAG_PP_MEM_TYPE_ENT  memType = CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E;
    GT_U32                     offset = 0;
    GT_U32                     readData;
    GT_PORT_GROUPS_BMP         portGroupsBmp = 1;
    GT_U32                     portGroupId;
    CPSS_PP_FAMILY_TYPE_ENT    devFamily = CPSS_PP_FAMILY_CHEETAH_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1.  Call with memType = [CPSS_DIAG_PP_MEM_BUFFER_DRAM_E /
                                             CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E /
                                             CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E ],
                              offset [0 / 100 / 1000], and non-NULL dataPtr.
                Expected: GT_OK.
            */
            /* call with memType = CPSS_DIAG_PP_MEM_BUFFER_DRAM_E */
            memType = CPSS_DIAG_PP_MEM_BUFFER_DRAM_E;
            offset = 0;

            st = cpssDxChDiagPortGroupMemRead(dev, portGroupsBmp, memType, offset, &readData);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

            /*
                1.1.2. Call with wrong enum values memType and other params same as in 1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChDiagPortGroupMemRead
                                (dev, portGroupsBmp, memType, offset, &readData),
                                memType);

            /* call with memType = CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E */
            memType = CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E;
            offset = 100;

            st = cpssDxChDiagPortGroupMemRead(dev, portGroupsBmp, memType, offset, &readData);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

            /*
                1.1.2. Call with wrong enum values memType and other params same as in 1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChDiagPortGroupMemRead
                                (dev, portGroupsBmp, memType, offset, &readData),
                                memType);

            /* call with memType = CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E */
            memType = CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E;
            /* The offset inside the entry should be within 297 bits - 9 words
               For example offset 980 bytes is entry 15, with offset 20 bytes
               inside the entry */
            offset = 980;


            /* In Lion Egress VLAN exist only in two port groups */
            if (devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                if ((0x3 & portGroupsBmp) == 0x0)
                {
                    continue;
                }
            }

            st = cpssDxChDiagPortGroupMemRead(dev, portGroupsBmp, memType, offset, &readData);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, memType);

            /*
                1.1.2. Call with wrong enum values memType and other params same as in 1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChDiagPortGroupMemRead
                                (dev, portGroupsBmp, memType, offset, &readData),
                                memType);

            /*
                1.1.3. Call with dataPtr [NULL], other params same as in 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChDiagPortGroupMemRead(dev, portGroupsBmp,
                                              memType, offset, NULL);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, dev, memType);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChDiagPortGroupMemRead(dev, portGroupsBmp, memType, offset, &readData);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
    }

    portGroupsBmp = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagPortGroupMemRead(dev, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                          memType, offset, &readData);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagPortGroupMemRead(dev, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                      memType, offset, &readData);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPortGroupRegsNumGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *regsNumPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPortGroupRegsNumGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1.1  Call for each port group cpssDxChDiagPortGroupRegsNumGet and then
           cpssDxChPortGroupDiagRegsDump with not-NULL pointers.
    Expected: GT_OK.
    1.1.2 Call for CPSS_PORT_GROUP_UNAWARE_MODE_CNS cpssDxChDiagPortGroupRegsNumGet
          and then cpssDxChPortGroupDiagRegsDump with not-NULL pointers.
    Expected: GT_OK.
    1.1.3. Call cpssDxChDiagPortGroupRegsNumGet for CPSS_PORT_GROUP_UNAWARE_MODE_CNS
           with regsNumPtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st   = GT_OK;
    GT_U8                       dev;
    GT_U32                      regsNum;
    GT_PORT_GROUPS_BMP          portGroupsBmp = 1;
    GT_U32                      *regAddrPtr;
    GT_U32                      *regDataPtr;
    GT_U32                      portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1.1  Call for each port group cpssDxChDiagPortGroupRegsNumGet and then
            cpssDxChPortGroupDiagRegsDump with not-NULL pointers.
            Expected: GT_OK.
        */
        /* read for each port group */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next port */
            portGroupsBmp = (1 << portGroupId);

            /* read the registers number */
            st = cpssDxChDiagPortGroupRegsNumGet(dev, portGroupsBmp, &regsNum);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* allocate space for regAddress and regData */
            regAddrPtr = cpssOsMalloc(regsNum * sizeof(GT_U32));
            regDataPtr = cpssOsMalloc(regsNum * sizeof(GT_U32));

            UTF_VERIFY_NOT_EQUAL0_STRING_MAC((GT_U32)NULL, (GT_U32)regAddrPtr,
                                        "cpssOsMalloc: Memory allocation error.");
            UTF_VERIFY_NOT_EQUAL0_STRING_MAC((GT_U32)NULL, (GT_U32)regDataPtr,
                                        "cpssOsMalloc: Memory allocation error.");

            /* dump registers */
            st = cpssDxChDiagPortGroupRegsDump(dev,portGroupsBmp, &regsNum, 0,
                                               regAddrPtr, regDataPtr);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            if (regAddrPtr != NULL)
            {
                /* if memory wasn't allocated */
                cpssOsFree(regAddrPtr);
                regAddrPtr = (GT_U32*)NULL;
            }

            if (regDataPtr != NULL)
            {
                /* if memory wasn't allocated */
                cpssOsFree(regDataPtr);
                regDataPtr = (GT_U32*)NULL;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /*
            1.1.2 Call for CPSS_PORT_GROUP_UNAWARE_MODE_CNS cpssDxChDiagPortGroupRegsNumGet
            and then cpssDxChPortGroupDiagRegsDump with not-NULL pointers.
            Expected: GT_OK.
        */
        st = cpssDxChDiagPortGroupRegsNumGet(dev,
                                      CPSS_PORT_GROUP_UNAWARE_MODE_CNS, &regsNum);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* allocate space for regAddress and regData */
        regAddrPtr = cpssOsMalloc(regsNum * sizeof(GT_U32));
        regDataPtr = cpssOsMalloc(regsNum * sizeof(GT_U32));

        UTF_VERIFY_NOT_EQUAL0_STRING_MAC((GT_U32)NULL, (GT_U32)regAddrPtr,
                                        "cpssOsMalloc: Memory allocation error.");
        UTF_VERIFY_NOT_EQUAL0_STRING_MAC((GT_U32)NULL, (GT_U32)regDataPtr,
                                        "cpssOsMalloc: Memory allocation error.");

        st = cpssDxChDiagPortGroupRegsDump(dev, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                           &regsNum, 0, regAddrPtr, regDataPtr);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        if (regAddrPtr != NULL)
        {
            /* if memory wasn't allocated */
            cpssOsFree(regAddrPtr);
            regAddrPtr = (GT_U32*)NULL;
        }

        if (regDataPtr != NULL)
        {
            /* if memory wasn't allocated */
            cpssOsFree(regDataPtr);
            regDataPtr = (GT_U32*)NULL;
        }

        /*
            1.1.3. Call cpssDxChDiagPortGroupRegsNumGet for CPSS_PORT_GROUP_UNAWARE_MODE_CNS
            with regsNumPtr[NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChDiagPortGroupRegsNumGet(dev, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, NULL);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM. */

    /* allocate space for one register */
    regAddrPtr = cpssOsMalloc(sizeof(GT_U32));
    regDataPtr = cpssOsMalloc(sizeof(GT_U32));

    UTF_VERIFY_NOT_EQUAL0_STRING_MAC((GT_U32)NULL, (GT_U32)regAddrPtr,
                                    "cpssOsMalloc: Memory allocation error.");
    UTF_VERIFY_NOT_EQUAL0_STRING_MAC((GT_U32)NULL, (GT_U32)regDataPtr,
                                    "cpssOsMalloc: Memory allocation error.");

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagPortGroupRegsNumGet(dev,
                        CPSS_PORT_GROUP_UNAWARE_MODE_CNS, &regsNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagRegsNumGet(dev, &regsNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

    if (regAddrPtr != NULL)
    {
        /* if memory wasn't allocated */
        cpssOsFree(regAddrPtr);
        regAddrPtr = (GT_U32*)NULL;
    }

    if (regDataPtr != NULL)
    {
        /* if memory wasn't allocated */
        cpssOsFree(regDataPtr);
        regDataPtr = (GT_U32*)NULL;
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagPortGroupRegsDump
(
    IN    GT_U8                 devNum,
    IN    GT_PORT_GROUPS_BMP    portGroupsBmp,
    INOUT GT_U32                *regsNumPtr,
    IN    GT_U32                offset,
    OUT   GT_U32                *regAddrPtr,
    OUT   GT_U32                *regDataPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagPortGroupRegsDump)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxChx)
    1.1.1  Call with not null pointers.
    Expected: GT_OK.
    1.1.2. Call with regAddrPtr[NULL].
    Expected: GT_BAD_PTR.
    1.1.3. Call with regDataPtr[NULL].
    Expected: GT_BAD_PTR.
    1.1.4. Call with regsNumPtr[NULL].
    Expected: GT_BAD_PTR.
    1.1.5 Call for each port group cpssDxChDiagPortGroupRegsDump
          with regsNumPtr bigger then the real registers number.
    Expected: GT_BAD_PTR.

*/
    GT_STATUS           st = GT_OK;
    GT_U8               dev;
    GT_U32              regsNum;
    GT_PORT_GROUPS_BMP  portGroupsBmp = 1;
    GT_U32              *regAddrPtr = 0;
    GT_U32              *regDataPtr = 0;
    GT_U32              portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1  Call with not null pointers.
                Expected: GT_OK.
            */
            st = cpssDxChDiagPortGroupRegsDump(dev, portGroupsBmp,
                                               &regsNum, 0, NULL, regDataPtr);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

            /*
                1.1.2. Call with regAddrPtr[NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChDiagPortGroupRegsDump(dev, portGroupsBmp,
                                               &regsNum, 0, NULL, regDataPtr);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

            /*
                1.1.3. Call with regDataPtr[NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChDiagPortGroupRegsDump(dev, portGroupsBmp,
                                               &regsNum, 0, regAddrPtr, NULL);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

            /*
                1.1.4. Call with regsNumPtr[NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChDiagPortGroupRegsDump(dev, portGroupsBmp,
                                               NULL, 0, regAddrPtr, regDataPtr);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

            /*
                1.1.5 Call for each port group cpssDxChDiagPortGroupRegsDump
                with regsNumPtr bigger then the real registers number.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChDiagPortGroupRegsNumGet(dev, portGroupsBmp,
                                                 &regsNum);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* allocate space for regAddress and regData */
            regAddrPtr = cpssOsMalloc(regsNum * sizeof(GT_U32));
            regDataPtr = cpssOsMalloc(regsNum * sizeof(GT_U32));

            UTF_VERIFY_NOT_EQUAL0_STRING_MAC((GT_U32)NULL, (GT_U32)regAddrPtr,
                                    "cpssOsMalloc: Memory allocation error.");
            UTF_VERIFY_NOT_EQUAL0_STRING_MAC((GT_U32)NULL, (GT_U32)regDataPtr,
                                    "cpssOsMalloc: Memory allocation error.");

            regsNum += 1;

            st = cpssDxChDiagPortGroupRegsDump(dev, portGroupsBmp,
                                               &regsNum, 0, regAddrPtr, regDataPtr);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PARAM, st);

            if (regAddrPtr != NULL)
            {
                /* if memory wasn't allocated */
                cpssOsFree(regAddrPtr);
                regAddrPtr = (GT_U32*)NULL;
            }

            if (regDataPtr != NULL)
            {
                /* if memory wasn't allocated */
                cpssOsFree(regDataPtr);
                regDataPtr = (GT_U32*)NULL;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* restore port group */
        portGroupsBmp = 1;

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            st = cpssDxChDiagPortGroupRegsDump(dev, portGroupsBmp,
                                           &regsNum, 0, NULL, regDataPtr);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, dev, portGroupsBmp);
            }

            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
    }

    portGroupsBmp = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM. */

    /* allocate space for one register */
    regAddrPtr = cpssOsMalloc(sizeof(GT_U32));
    regDataPtr = cpssOsMalloc(sizeof(GT_U32));

    UTF_VERIFY_NOT_EQUAL0_STRING_MAC((GT_U32)NULL, (GT_U32)regAddrPtr,
                            "cpssOsMalloc: Memory allocation error.");
    UTF_VERIFY_NOT_EQUAL0_STRING_MAC((GT_U32)NULL, (GT_U32)regDataPtr,
                            "cpssOsMalloc: Memory allocation error.");

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        regsNum = 1;
        st = cpssDxChDiagPortGroupRegsDump(dev, portGroupsBmp,
                                           &regsNum, 0, regAddrPtr, regDataPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    regsNum = 1;
    st = cpssDxChDiagPortGroupRegsDump(dev, portGroupsBmp,
                                       &regsNum, 0, regAddrPtr, regDataPtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

    if (regAddrPtr != NULL)
    {
        /* if memory wasn't allocated */
        cpssOsFree(regAddrPtr);
        regAddrPtr = (GT_U32*)NULL;
    }

    if (regDataPtr != NULL)
    {
        /* if memory wasn't allocated */
        cpssOsFree(regDataPtr);
        regDataPtr = (GT_U32*)NULL;
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChDiagDeviceTemperatureGet
(
    IN  GT_U8  dev,
    OUT GT_32  *temperaturePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChDiagDeviceTemperatureGet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with not null temperaturePtr.
    Expected: GT_OK.
    1.2. Call api with wrong temperaturePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8  dev;
    GT_32  temperature;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not null temperaturePtr.
            Expected: GT_OK.
        */
        st = cpssDxChDiagDeviceTemperatureGet(dev, &temperature);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call api with wrong temperaturePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChDiagDeviceTemperatureGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                   "%d, temperaturePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChDiagDeviceTemperatureGet(dev, &temperature);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChDiagDeviceTemperatureGet(dev, &temperature);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChDiag suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChDiag)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagAllMemTest)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagAllRegTest)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagMemRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagMemTest)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagMemWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPrbsPortCheckEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPrbsPortCheckEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPrbsPortCheckReadyGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPrbsPortGenerateEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPrbsPortGenerateEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPrbsPortStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPrbsPortTransmitModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPrbsPortTransmitModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagRegTest)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagRegsDump)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagRegsNumGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPrbsCyclicDataSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPrbsCyclicDataGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPhyRegRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPhyRegWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagRegRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagRegWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPrbsSerdesStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPrbsSerdesTestEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPrbsSerdesTestEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPrbsSerdesTransmitModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPrbsSerdesTransmitModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagMemoryBistBlockStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagMemoryBistsRun)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPortGroupMemWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPortGroupMemRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPortGroupRegsNumGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagPortGroupRegsDump)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChDiagDeviceTemperatureGet)
UTF_SUIT_END_TESTS_MAC(cpssDxChDiag)

