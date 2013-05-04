/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxBrgStpUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxBrgStp.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>

#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgStp.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Max STP Group Id                             */
/* Its defined in cpssExMxBrgStp.c              */
/* Be careful to keep this values synchronized! */
#define MAX_NUMBER_OF_STG_CNS                   256


/* Invalid enum                                 */
#define BRG_STP_INVALID_ENUM_CNS                0x5AAAAAA5

/* STP index treated as valid in tests          */
#define BRG_STP_TESTED_STP_ID_CNS               0

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgStpModeSet
(
    IN GT_U8                    dev,
    IN CPSS_EXMX_STP_MODE_ENT   mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgStpModeSet)
{
/*
ITERATE_DEVICES
1.1. Call function with mode [CPSS_EXMX_SINGLE_STP_E]. 
    Expected: GT_OK.
1.2. Call function with out of range mode [0x5AAAAAA5]. 
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                           dev;
    CPSS_EXMX_STP_MODE_ENT          mode;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with mode [CPSS_EXMX_SINGLE_STP_E].  */
        /* Expected: GT_OK.                                        */
        mode = CPSS_EXMX_SINGLE_STP_E;

        st = cpssExMxBrgStpModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /* 1.2. Call with out of range mode [0x5AAAAAA5].     */
        /* Expected: GT_BAD_PARAM.                              */
        mode = BRG_STP_INVALID_ENUM_CNS;

        st = cpssExMxBrgStpModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_EXMX_SINGLE_STP_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgStpModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgStpModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgStpPortStateSet
(
    IN GT_U8                    dev,
    IN GT_U8                    port,
    IN CPSS_STP_STATE_ENT  state
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgStpPortStateSet)
{
/*
ITERATE_DEVICES_VIRT_PORT
1.1.Call function cpssExMxBrgStpModeSet with mode [CPSS_EXMX_SINGLE_STP_E]. Expected: GT_OK.
1.2.1. Call function with state 
    [CPSS_STP_BLCK_LSTN_E]. Expected: GT_OK.
1.2.2. Call function with out of range state 
    [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                       dev;
    GT_U8                       port;
    CPSS_STP_STATE_ENT          state;

    CPSS_EXMX_STP_MODE_ENT      mode = CPSS_EXMX_SINGLE_STP_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1.Call function cpssExMxBrgStpModeSet with mode
        [CPSS_EXMX_SINGLE_STP_E]. Expected: GT_OK. */
        st = cpssExMxBrgStpModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxBrgStpModeSet: %d, %d",
                                     dev, mode);

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For each active device id go over all active virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.2.1. Call function with state                  */
            /* [CPSS_EXMX_STP_BLCK_LSTN_E]. Expected: GT_OK.    */
            state = CPSS_STP_BLCK_LSTN_E;

            st = cpssExMxBrgStpPortStateSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /* 1.2.2. Call function with out of range state     */
            /* [0x5AAAAAA5]. Expected: GT_BAD_PARAM.            */
            state = BRG_STP_INVALID_ENUM_CNS;

            st = cpssExMxBrgStpPortStateSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, state);
        }

        state = CPSS_STP_BLCK_LSTN_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
             st = cpssExMxBrgStpPortStateSet(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        /* state == CPSS_EXMX_STP_BLCK_LSTN_E */

        st = cpssExMxBrgStpPortStateSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = 0;
    state = CPSS_STP_BLCK_LSTN_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */

        st = cpssExMxBrgStpPortStateSet(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgStpPortStateSet(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgStpStateSet
(
    IN GT_U8                    dev,
    IN GT_U8                    port,
    IN GT_U16                   stpId,
    IN CPSS_EXMX_STP_STATE_ENT  state
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgStpStateSet)
{
/*
ITERATE_DEVICES_VIRT_PORT
1.1.Call function cpssExMxBrgStpModeSet with mode [CPSS_EXMX_MULTIPLE_STP_E]. Expected: GT_OK.
1.2.1. Call function with stpId [0], 
    state [CPSS_EXMX_STP_LRN_E]. Expected: GT_OK.
1.2.2. Call function with out of range stpId
    [MAX_NUMBER_OF_STG_CNS]. Expected: NON GT_OK.
1.2.3. Call function with out of range 
    state [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                       dev;
    GT_U8                       port;
    GT_U16                      stpId;
    CPSS_STP_STATE_ENT          state;

    CPSS_EXMX_STP_MODE_ENT      mode = CPSS_EXMX_MULTIPLE_STP_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1.Call function cpssExMxBrgStpModeSet with mode
        [CPSS_EXMX_MULTIPLE_STP_E]. Expected: GT_OK. */
        st = cpssExMxBrgStpModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxBrgStpModeSet: %d, %d",
                                     dev, mode);

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For each active device id go over all active virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.2.1. Call function with stpId [0],             */
            /* state [CPSS_EXMX_STP_LRN_E]. Expected: GT_OK.    */
            stpId = BRG_STP_TESTED_STP_ID_CNS;
            state = CPSS_STP_LRN_E;

            st = cpssExMxBrgStpStateSet(dev, port, stpId, state);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, stpId, state);

            /* 1.2.2. Call function with out of range stpId     */
            /* [MAX_NUMBER_OF_STG_CNS]. Expected: NON GT_OK.    */
            stpId = MAX_NUMBER_OF_STG_CNS;

            st = cpssExMxBrgStpStateSet(dev, port, stpId, state);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, stpId, state);

            stpId = BRG_STP_TESTED_STP_ID_CNS; /* restore */

            /* 1.2.3. Call function with out of range state     */
            /* [0x5AAAAAA5]. Expected: GT_BAD_PARAM.            */
            state = BRG_STP_INVALID_ENUM_CNS;

            st = cpssExMxBrgStpStateSet(dev, port, stpId, state);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, stpId, state);
        }

        stpId = BRG_STP_TESTED_STP_ID_CNS;
        state = CPSS_STP_LRN_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
             st = cpssExMxBrgStpStateSet(dev, port, stpId, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        /* stpId == BRG_STP_TESTED_STP_ID_CNS   */
        /* state == CPSS_EXMX_STP_LRN_E         */  

        st = cpssExMxBrgStpStateSet(dev, port, stpId, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /*2. Go over all non active devices. */
    port = 0;
    stpId = BRG_STP_TESTED_STP_ID_CNS;
    state = CPSS_STP_LRN_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */

        st = cpssExMxBrgStpStateSet(dev, port, stpId, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgStpStateSet(dev, port, stpId, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgStpEntryGet
(
    IN  GT_U8       dev,
    IN  GT_U16      stpId,
    OUT GT_U32      stpEntryPtr[CPSS_EXMX_STG_ENTRY_SIZE_CNS]
)
*/

UTF_TEST_CASE_MAC(cpssExMxBrgStpEntryGet)
{
/*
ITERATE_DEVICES
1.1.Call function cpssExMxBrgStpModeSet with mode [CPSS_EXMX_MULTIPLE_STP_E]. Expected: GT_OK.
1.2. Call function with stpId [0] and 
    non-NULL stpEntrPtr. Expected: GT_OK.
1.3. Call function with out of range stpId 
    [MAX_NUMBER_OF_STG_CNS] and non-NULL stpEntrPtr. 
    Expected: NON GT_OK
*/
    GT_STATUS       st = GT_OK;

    GT_U8       dev;
    GT_U16      stpId;
    GT_U32      stpEntry[4];

    CPSS_EXMX_STP_MODE_ENT      mode = CPSS_EXMX_MULTIPLE_STP_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1.Call function cpssExMxBrgStpModeSet with mode
        [CPSS_EXMX_MULTIPLE_STP_E]. Expected: GT_OK. */
        st = cpssExMxBrgStpModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxBrgStpModeSet: %d, %d",
                                     dev, mode);

        /* 1.2. Call function with stpId [0] and    */
        /* non-NULL stpEntrPtr. Expected: GT_OK.    */
        stpId = BRG_STP_TESTED_STP_ID_CNS;

        st = cpssExMxBrgStpEntryGet(dev, stpId, stpEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, stpId);

        /* 1.3. Call function with out of range stpId       */
        /* [MAX_NUMBER_OF_STG_CNS] and non-NULL stpEntrPtr. */
        /* Expected: NON GT_OK                              */
        stpId = MAX_NUMBER_OF_STG_CNS;

        st = cpssExMxBrgStpEntryGet(dev, stpId, stpEntry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, stpId);

        stpId = BRG_STP_TESTED_STP_ID_CNS; /* restore */
    }

    stpId = BRG_STP_TESTED_STP_ID_CNS;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgStpEntryGet(dev, stpId, stpEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgStpEntryGet(dev, stpId, stpEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgStpEntryWrite
(
    IN  GT_U8       dev,
    IN  GT_U16      stpId,
    IN  GT_U32      stpEntryPtr[CPSS_EXMX_STG_ENTRY_SIZE_CNS]
)
*/
/*
* COMMENTS:
*       stpEntryPtr   - Pointer to 4 Words(16 bytes) allocated memory that
*                       includes the stp entry.
*/
UTF_TEST_CASE_MAC(cpssExMxBrgStpEntryWrite)
{
/*
ITERATE_DEVICES
1.1.Call function cpssExMxBrgStpModeSet with mode [CPSS_EXMX_MULTIPLE_STP_E]. Expected: GT_OK.
1.2. Call function with stpId [0] and 
    non-NULL stpEntrPtr [{1234, 0, 0, 5678}]. Expected: GT_OK.
1.3. Call cpssExMxBrgStpEntryGet with stpId [0] 
    and non-NULL stpEntryPtr. 
    Expected: GT_OK and stpEntryPtr [{1234, 0, 0, 5678}].
1.4. Call function with out of range stpId 
    [MAX_NUMBER_OF_STG_CNS] and non-NULL stpEntrPtr. 
    Expected: NON GT_OK.
*/
    GT_STATUS       st = GT_OK;

    GT_U8       dev;
    GT_U16      stpId;
    GT_U32      stpEntry[4];

    GT_U32      retStpEntry[4];
    GT_BOOL     failureWas;

    CPSS_EXMX_STP_MODE_ENT      mode = CPSS_EXMX_MULTIPLE_STP_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1.Call function cpssExMxBrgStpModeSet with mode
        [CPSS_EXMX_MULTIPLE_STP_E]. Expected: GT_OK. */
        st = cpssExMxBrgStpModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxBrgStpModeSet: %d, %d",
                                     dev, mode);

        /* 1.2. Call function with stpId [0] and    */
        /* non-NULL stpEntrPtr [{1234, 0, 0, 5678}]. Expected: GT_OK.    */
        stpId = BRG_STP_TESTED_STP_ID_CNS;

        cpssOsBzero((GT_VOID*)&stpEntry, sizeof(stpEntry[0]*4));
        stpEntry[0] = 1234;
        stpEntry[3] = 5678;
        /* stpEntry == {1234, 0, 0, 5678} */
  
        st = cpssExMxBrgStpEntryWrite(dev, stpId, stpEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, stpId);

        /* restore stpEntry in case function overwrite it */
        cpssOsBzero((GT_VOID*)&stpEntry, sizeof(stpEntry[0]*4));
        stpEntry[0] = 1234;
        stpEntry[3] = 5678;
        /* stpEntry == {1234, 0, 0, 5678} */

        /* 1.3. Call cpssExMxBrgStpEntryGet with stpId [0]      */
        /* and non-NULL stpEntryPtr.                            */
        /* Expected: GT_OK and stpEntryPtr [{1234, 0, 0, 5678}].*/

        st = cpssExMxBrgStpEntryGet(dev, stpId, retStpEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                     "cpssExMxBrgStpEntryGet: %d, %d",
                                     dev, stpId);
        if (GT_OK == st) 
        {

            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)stpEntry,
                                            (const GT_VOID*)retStpEntry, sizeof(stpEntry[0]*4))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                                         "get another stpEntry than has been written [dev = %d, stpId = %d]",
                                         dev, stpId);
        }

        /* 1.4. Call function with out of range stpId       */
        /* [MAX_NUMBER_OF_STG_CNS] and non-NULL stpEntrPtr. */
        /* Expected: NON GT_OK                              */
        stpId = MAX_NUMBER_OF_STG_CNS;

        st = cpssExMxBrgStpEntryWrite(dev, stpId, stpEntry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, stpId);

        stpId = BRG_STP_TESTED_STP_ID_CNS; /* restore */
    }

    stpId = BRG_STP_TESTED_STP_ID_CNS;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgStpEntryWrite(dev, stpId, stpEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgStpEntryWrite(dev, stpId, stpEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cppExMxBrgStp suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxBrgStp)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgStpModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgStpPortStateSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgStpStateSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgStpEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgStpEntryWrite)
UTF_SUIT_END_TESTS_MAC(cpssExMxBrgStp)
