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
*  cpssDxSalBrgStpUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxSalBrgStp, that provides 
*   Spanning Tree protocol facility DxSal CPSS implementation
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
/* includes */
#include <cpss/dxSal/bridge/cpssDxSalBrgStp.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */
#define PRV_CPSS_DXSAL_STG_MAX_NUM_CNS  32

/*----------------------------------------------------------------------------*/
/*
cpssDxSalBrgStpEntryGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  stgId,
    OUT GT_U32  stgEntryPtr[CPSS_DXSAL_STG_ENTRY_SIZE_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgStpEntryGet)
{
/*
ITERATE_DEVICES(DxSal)
1.1. Call function with stgId [0 / 10 / PRV_CPSS_DXSAL_STG_MAX_NUM_CNS - 1] and non-
NULL stgEntryPtr. Expected: GT_OK.
1.2. Call function with out of range stgId 
[PRV_CPSS_DXSAL_STG_MAX_NUM_CNS] and non-NULL stgEntryPtr.
 Expected: NON GT_OK.
1.3. Call function with stgEntryPtr [NULL] and stgId [0].
 Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U32          stgId;
    GT_U32          stgEntryPtr[CPSS_DXSAL_STG_ENTRY_SIZE_CNS];

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with stgId [0 / 10 / PRV_CPSS_DXSAL_STG_MAX_NUM_CNS - 1] and non-
        NULL stgEntryPtr. Expected: GT_OK. */
        stgId = 0;

        st = cpssDxSalBrgStpEntryGet(dev, stgId, stgEntryPtr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, stgId);

        stgId = 10;

        st = cpssDxSalBrgStpEntryGet(dev, stgId, stgEntryPtr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, stgId);

        stgId = PRV_CPSS_DXSAL_STG_MAX_NUM_CNS - 1;

        st = cpssDxSalBrgStpEntryGet(dev, stgId, stgEntryPtr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, stgId);

        /* 1.2. Call function with out of range stgId 
        [PRV_CPSS_DXSAL_STG_MAX_NUM_CNS] and non-NULL stgEntryPtr.
        Expected: NON GT_OK. */
        stgId = PRV_CPSS_DXSAL_STG_MAX_NUM_CNS;

        st = cpssDxSalBrgStpEntryGet(dev, stgId, stgEntryPtr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, stgId);

        /* 1.3. Call function with stgEntryPtr [NULL] and stgId [0].
        Expected: GT_BAD_PTR. */
        stgId = 0;

        st = cpssDxSalBrgStpEntryGet(dev, stgId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, stgId);
    }
    stgId = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgStpEntryGet(dev, stgId, stgEntryPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgStpEntryGet(dev, stgId, stgEntryPtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
cpssDxSalBrgStpEntryWrite
(
    IN  GT_U8       devNum,
    IN  GT_U32      stgId,
    IN  GT_U32      stgEntryPtr[CPSS_DXSAL_STG_ENTRY_SIZE_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgStpEntryWrite)
{
/*
ITERATE_DEVICES(DxSal)
1.1. Call function with stgId [0 / 10 / PRV_CPSS_DXSAL_STG_MAX_NUM_CNS - 1] and non-NULL stgEntryPtr
 [{5,10}]. Expected: GT_OK.
1.2. Call cpssDxSalBrgStpEntryGet with parameters the same as in 1.1.
 Expected: GT_OK and the same stgEntryPtr.
1.3. Call function with out of range stgId [PRV_CPSS_DXSAL_STG_MAX_NUM_CNS]
 and non-NULL stgEntryPtr [{5,10}]. Expected: NON GT_OK.
1.4. Call function with stgEntryPtr [NULL] and stgId [0]. Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U32          stgId;
    GT_U32          stgEntryPtr[CPSS_DXSAL_STG_ENTRY_SIZE_CNS];
    GT_U32          retStgEntryPtr[CPSS_DXSAL_STG_ENTRY_SIZE_CNS];
    GT_BOOL         isEqual;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with stgId [0 / 10 / PRV_CPSS_DXSAL_STG_MAX_NUM_CNS - 1] and non-NULL stgEntryPtr
        [{5,10}]. Expected: GT_OK.
        1.2. Call cpssDxSalBrgStpEntryGet with parameters the same as in 1.1.
        Expected: GT_OK and the same stgEntryPtr. */
        stgId = 0;
        stgEntryPtr[0] = 5;
        stgEntryPtr[1] = 10;

        st = cpssDxSalBrgStpEntryWrite(dev, stgId, stgEntryPtr);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, stgId, stgEntryPtr[0],
                                     stgEntryPtr[1]);

        st = cpssDxSalBrgStpEntryGet(dev, stgId, retStgEntryPtr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgStpEntryGet: %d, %d",
                                     dev, stgId);
        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&stgEntryPtr, (GT_VOID*)&retStgEntryPtr, 
                                     sizeof(stgEntryPtr)))
                   ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, 
            "got another stgEntryPtr than was set: %d, %d", dev, stgId);

        stgId = 10;

        st = cpssDxSalBrgStpEntryWrite(dev, stgId, stgEntryPtr);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, stgId, stgEntryPtr[0],
                                     stgEntryPtr[1]);

        st = cpssDxSalBrgStpEntryGet(dev, stgId, retStgEntryPtr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgStpEntryGet: %d, %d",
                                     dev, stgId);
        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&stgEntryPtr, (GT_VOID*)&retStgEntryPtr, 
                                     sizeof(stgEntryPtr)))
                   ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, 
            "got another stgEntryPtr than was set: %d, %d", dev, stgId);

        stgId = PRV_CPSS_DXSAL_STG_MAX_NUM_CNS - 1;

        st = cpssDxSalBrgStpEntryWrite(dev, stgId, stgEntryPtr);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, stgId, stgEntryPtr[0],
                                     stgEntryPtr[1]);

        st = cpssDxSalBrgStpEntryGet(dev, stgId, retStgEntryPtr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgStpEntryGet: %d, %d",
                                     dev, stgId);
        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&stgEntryPtr, (GT_VOID*)&retStgEntryPtr, 
                                     sizeof(stgEntryPtr)))
                   ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, 
            "got another stgEntryPtr than was set: %d, %d", dev, stgId);

        /* 1.3. Call function with out of range stgId [PRV_CPSS_DXSAL_STG_MAX_NUM_CNS]
        and non-NULL stgEntryPtr [{5,10}]. Expected: NON GT_OK. */
        stgId = PRV_CPSS_DXSAL_STG_MAX_NUM_CNS;

        st = cpssDxSalBrgStpEntryWrite(dev, stgId, stgEntryPtr);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, stgId, stgEntryPtr[0],
                                     stgEntryPtr[1]);

        /* 1.4. Call function with stgEntryPtr [NULL] and stgId [0]. 
        Expected: GT_BAD_PTR. */
        stgId = 0;

        st = cpssDxSalBrgStpEntryWrite(dev, stgId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, stgId);
    }
    stgId = 0;
    stgEntryPtr[0] = 5;
    stgEntryPtr[1] = 10;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgStpEntryWrite(dev, stgId, stgEntryPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgStpEntryWrite(dev, stgId, stgEntryPtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxSalBrgStp suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxSalBrgStp)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgStpEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgStpEntryWrite)
UTF_SUIT_END_TESTS_MAC(cpssDxSalBrgStp)
