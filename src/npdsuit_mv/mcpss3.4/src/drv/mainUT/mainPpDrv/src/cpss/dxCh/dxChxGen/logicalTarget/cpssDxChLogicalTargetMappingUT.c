/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChLogicalTargetMappingUT.c
*
* DESCRIPTION:
*       Unit tests for
*       Logical Target mapping.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/logicalTarget/cpssDxChLogicalTargetMapping.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* Tests use this vlan id for testing VLAN functions */
#define LTM_VLAN_TESTED_VLAN_ID_CNS     100

/* Tests use this port id for testing port functions */
#define LTM_TESTED_PORT_NUM_CNS           0

/* Invalid port id for testing port functions */
#define LTM_INVALID_PORT_NUM_CNS         65

/* Correct logical dev num value */
#define LTM_LOGICAL_DEV_NUM_CNS          24

/* Invalid logical dev num value */
#define LTM_INVALID_LOGICAL_DEV_NUM_CNS  23

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChLogicalTargetMappingDeviceEnableSet
(
    IN GT_U8      devNum,
    IN GT_U8      logicalDevNum,
    IN GT_BOOL    enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChLogicalTargetMappingDeviceEnableSet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call with logicalDevNum [LTM_LOGICAL_DEV_NUM_CNS]
              and enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssDxChLogicalTargetMappingDeviceEnableGet with the same params.
    Expected: GT_OK and the same value.
    1.3. Call with wrong logicalDevNum [LTM_INVALID_LOGICAL_DEV_NUM_CNS].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8       logicalDevNum = LTM_LOGICAL_DEV_NUM_CNS;
    GT_BOOL     enable    = GT_TRUE;
    GT_BOOL     enableGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with logicalDevNum [LTM_LOGICAL_DEV_NUM_CNS]
                      and enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        /*call with logicalDevNum [LTM_LOGICAL_DEV_NUM_CNS] and enable [GT_TRUE]*/
        logicalDevNum  = LTM_LOGICAL_DEV_NUM_CNS;
        enable = GT_TRUE;

        st = cpssDxChLogicalTargetMappingDeviceEnableSet(dev, logicalDevNum, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, logicalDevNum, enable);

        /*
            1.2. Call cpssDxChLogicalTargetMappingDeviceEnableGet with the same params.
            Expected: GT_OK and the same value.
        */

        st = cpssDxChLogicalTargetMappingDeviceEnableGet(dev, logicalDevNum, &enableGet);

        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, logicalDevNum, enable);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                                     "get another enable than was set: %d", dev);

        /*call with enable [GT_FALSE]*/
        enable = GT_FALSE;

        st = cpssDxChLogicalTargetMappingDeviceEnableSet(dev, logicalDevNum, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, logicalDevNum, enable);

        /*
            1.2. Call cpssDxChLogicalTargetMappingDeviceEnableGet with the same params.
            Expected: GT_OK and the same value.
        */

        st = cpssDxChLogicalTargetMappingDeviceEnableGet(dev, logicalDevNum, &enableGet);

        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, logicalDevNum, enable);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                                     "get another enable than was set: %d", dev);

        /*
            1.3. Call with wrong logicalDevNum [LTM_INVALID_LOGICAL_DEV_NUM_CNS].
            Expected: GT_BAD_PARAM.
        */
        logicalDevNum  = LTM_INVALID_LOGICAL_DEV_NUM_CNS;
        enable = GT_TRUE;

        st = cpssDxChLogicalTargetMappingDeviceEnableSet(dev, logicalDevNum, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, logicalDevNum, enable);

        logicalDevNum  = LTM_LOGICAL_DEV_NUM_CNS;
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChLogicalTargetMappingDeviceEnableSet(dev, logicalDevNum, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChLogicalTargetMappingDeviceEnableSet(dev, logicalDevNum, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChLogicalTargetMappingDeviceEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     logicalDevNum,
    OUT  GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChLogicalTargetMappingDeviceEnableGet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call with logicalDevNum [LTM_LOGICAL_DEV_NUM_CNS].
    Expected: GT_OK.
    1.2. Call with wrong logicalDevNum [LTM_INVALID_LOGICAL_DEV_NUM_CNS].
    Expected: GT_BAD_PARAM.
    1.3. Call with wrong enable pointer [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8       logicalDevNum = LTM_LOGICAL_DEV_NUM_CNS;
    GT_BOOL     enable    = GT_TRUE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with logicalDevNum [LTM_LOGICAL_DEV_NUM_CNS].
            Expected: GT_OK.
        */

        logicalDevNum  = LTM_LOGICAL_DEV_NUM_CNS;

        st = cpssDxChLogicalTargetMappingDeviceEnableGet(dev, logicalDevNum, &enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, logicalDevNum, enable);

        /*
            1.2. Call with wrong logicalDevNum [LTM_INVALID_LOGICAL_DEV_NUM_CNS].
            Expected: GT_BAD_PARAM.
        */
        logicalDevNum  = LTM_INVALID_LOGICAL_DEV_NUM_CNS;

        st = cpssDxChLogicalTargetMappingDeviceEnableGet(dev, logicalDevNum, &enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, logicalDevNum, enable);

        logicalDevNum  = LTM_LOGICAL_DEV_NUM_CNS;

        /*
            1.3. Call with wrong enable pointer [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChLogicalTargetMappingDeviceEnableGet(dev, logicalDevNum, NULL);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PTR, st, dev, logicalDevNum, enable);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChLogicalTargetMappingDeviceEnableGet(dev, logicalDevNum, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChLogicalTargetMappingDeviceEnableGet(dev, logicalDevNum, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChLogicalTargetMappingEnableSet
(
    IN GT_U8      devNum,
    IN GT_BOOL    enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChLogicalTargetMappingEnableSet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call with  enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssDxChLogicalTargetMappingEnableGet with the same params.
    Expected: GT_OK and the same value.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_TRUE;
    GT_BOOL     enableGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        /*call with enable [GT_TRUE]*/
        enable = GT_TRUE;

        st = cpssDxChLogicalTargetMappingEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssDxChLogicalTargetMappingEnableGet with the same params.
            Expected: GT_OK and the same value.
        */

        st = cpssDxChLogicalTargetMappingEnableGet(dev, &enableGet);

        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                                     "get another enable than was set: %d", dev);

        /*call with enable [GT_FALSE]*/
        enable = GT_FALSE;

        st = cpssDxChLogicalTargetMappingEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssDxChLogicalTargetMappingEnableGet with the same params.
            Expected: GT_OK and the same value.
        */

        st = cpssDxChLogicalTargetMappingEnableGet(dev, &enableGet);

        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                                     "get another enable than was set: %d", dev);

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChLogicalTargetMappingEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChLogicalTargetMappingEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChLogicalTargetMappingEnableGet
(
    IN   GT_U8     devNum,
    OUT  GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChLogicalTargetMappingEnableGet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call with non-NULL enable pointer.
    Expected: GT_OK.
    1.3. Call with wrong enable pointer [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_TRUE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL enable pointer.
            Expected: GT_OK.
        */

        st = cpssDxChLogicalTargetMappingEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call with wrong enable pointer [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChLogicalTargetMappingEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, dev, enable);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChLogicalTargetMappingEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChLogicalTargetMappingEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChLogicalTargetMappingTableEntrySet
(
    IN GT_U8                           devNum,
    IN GT_U8                           logicalDevNum,
    IN GT_U8                           logicalPortNum,
    IN CPSS_DXCH_OUTPUT_INTERFACE_STC  *logicalPortMappingTablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChLogicalTargetMappingTableEntrySet)
{
/*
    ITERATE_DEVICES (xCat and above)
   1.1. Call with logicalDevNum [LTM_LOGICAL_DEV_NUM_CNS],
                  logicalPortNum [LTM_TESTED_PORT_NUM_CNS]
                  and logicalPortMappingTable.isTunnelStart [GT_GT_TRUE],
                  logicalPortMappingTable.tunnelStartInfo.passengerPacketType [CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E],
                  logicalPortMappingTable.tunnelStartInfo.ptr [1].
    Expected: GT_OK.
    1.2. Call cpssDxChLogicalTargetMappingTableEntryGet with the same params.
    Expected: GT_OK and the same value.
    1.3. Call with logicalDevNum [LTM_LOGICAL_DEV_NUM_CNS],
                   logicalPortNum [LTM_TESTED_PORT_NUM_CNS]
                   and logicalPortMappingTable.isTunnelStart [GT_FALSE],
                   logicalPortMappingTable.tunnelStartInfo.passengerPacketType [wrong enum values],
                   logicalPortMappingTable.tunnelStartInfo.ptr [1].
    Expected: GT_OK
    1.4. Call with logicalDevNum [LTM_LOGICAL_DEV_NUM_CNS],
                   logicalPortNum [LTM_TESTED_PORT_NUM_CNS]
                   and logicalPortMappingTable.isTunnelStart [GT_TRUE],
                   logicalPortMappingTable.tunnelStartInfo.passengerPacketType [wrong enum values],
                   logicalPortMappingTable.tunnelStartInfo.ptr [1].
    Expected: GT_BAD_PARAM
    1.5. Call with wrong logicalDevNum [LTM_INVALID_LOGICAL_DEV_NUM_CNS].
    Expected: GT_BAD_PARAM.
    1.6. Call with wrong logicalPortNum [LTM_INVALID_PORT_NUM_CNS].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8       logicalDevNum = LTM_LOGICAL_DEV_NUM_CNS;
    GT_U8       logicalPortNum = LTM_TESTED_PORT_NUM_CNS;

    CPSS_DXCH_OUTPUT_INTERFACE_STC  logicalPortMappingTable;
    CPSS_DXCH_OUTPUT_INTERFACE_STC  logicalPortMappingTableGet;

    GT_BOOL     isEqual = GT_FALSE;

    cpssOsBzero((GT_VOID*)&logicalPortMappingTable, sizeof(logicalPortMappingTable));
    cpssOsBzero((GT_VOID*)&logicalPortMappingTableGet, sizeof(logicalPortMappingTableGet));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with logicalDevNum [LTM_LOGICAL_DEV_NUM_CNS],
                           logicalPortNum [LTM_TESTED_PORT_NUM_CNS]
                          and logicalPortMappingTable.isTunnelStart [GT_GT_TRUE],
                logicalPortMappingTable.tunnelStartInfo.passengerPacketType [CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E],
                logicalPortMappingTable.tunnelStartInfo.ptr [1].
.
            Expected: GT_OK.
        */

        logicalDevNum  = LTM_LOGICAL_DEV_NUM_CNS;
        logicalPortNum = LTM_TESTED_PORT_NUM_CNS;
        logicalPortMappingTable.isTunnelStart = GT_TRUE;
        logicalPortMappingTable.tunnelStartInfo.passengerPacketType = CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E;
        logicalPortMappingTable.tunnelStartInfo.ptr = 1;

        st = cpssDxChLogicalTargetMappingTableEntrySet(dev, logicalDevNum,
                                                  logicalPortNum, &logicalPortMappingTable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, logicalDevNum, logicalPortNum);

        /*
            1.2. Call cpssDxChLogicalTargetMappingTableEntryGet with the same params.
            Expected: GT_OK and the same value.
        */

        st = cpssDxChLogicalTargetMappingTableEntryGet(dev, logicalDevNum,
                                               logicalPortNum, &logicalPortMappingTableGet);

        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, logicalDevNum);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &(logicalPortMappingTable),
                                     (GT_VOID*) &(logicalPortMappingTableGet),
                                     sizeof(logicalPortMappingTable))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                  "get another logicalPortMappingTable than was set: %d", dev);


        /*
            1.3. Call with logicalDevNum [LTM_LOGICAL_DEV_NUM_CNS],
                           logicalPortNum [LTM_TESTED_PORT_NUM_CNS]
                           and logicalPortMappingTable.isTunnelStart [GT_FALSE],
                           logicalPortMappingTable.tunnelStartInfo.passengerPacketType [wrong enum values],
                           logicalPortMappingTable.tunnelStartInfo.ptr [1].
            Expected: GT_OK
        */

        logicalDevNum  = LTM_LOGICAL_DEV_NUM_CNS;
        logicalPortNum = LTM_TESTED_PORT_NUM_CNS;

        logicalPortMappingTable.isTunnelStart = GT_FALSE;
        logicalPortMappingTable.tunnelStartInfo.ptr = 1;

        for(enumsIndex = 0; enumsIndex < utfInvalidEnumArrSize; enumsIndex++) 
        {                                                                    
            logicalPortMappingTable.tunnelStartInfo.passengerPacketType 
                                                        = utfInvalidEnumArr[enumsIndex]; 
            st = cpssDxChLogicalTargetMappingTableEntrySet(dev, logicalDevNum,
                                        logicalPortNum, &logicalPortMappingTable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, logicalDevNum, logicalPortNum);
        }

        /*
            1.4. Call with logicalDevNum [LTM_LOGICAL_DEV_NUM_CNS],
                           logicalPortNum [LTM_TESTED_PORT_NUM_CNS]
                           and logicalPortMappingTable.isTunnelStart [GT_TRUE],
                           logicalPortMappingTable.tunnelStartInfo.passengerPacketType
                           [wrong enum values],
                           logicalPortMappingTable.tunnelStartInfo.ptr [1].
            Expected: GT_BAD_PARAM
        */
        logicalDevNum  = LTM_LOGICAL_DEV_NUM_CNS;
        logicalPortNum = LTM_TESTED_PORT_NUM_CNS;
        logicalPortMappingTable.isTunnelStart = GT_TRUE;
        logicalPortMappingTable.tunnelStartInfo.ptr = 1;

        UTF_ENUMS_CHECK_MAC(cpssDxChLogicalTargetMappingTableEntrySet
                            (dev, logicalDevNum, logicalPortNum, &logicalPortMappingTable),
                            logicalPortMappingTable.tunnelStartInfo.passengerPacketType);

        /*
            1.5. Call with wrong logicalDevNum [LTM_INVALID_LOGICAL_DEV_NUM_CNS].
            Expected: GT_BAD_PARAM.
        */
        logicalDevNum  = LTM_INVALID_LOGICAL_DEV_NUM_CNS;

        st = cpssDxChLogicalTargetMappingTableEntrySet(dev, logicalDevNum,
                                                  logicalPortNum, &logicalPortMappingTable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, logicalDevNum, logicalPortNum);

        logicalDevNum  = LTM_LOGICAL_DEV_NUM_CNS;

        /*
            1.6. Call with wrong logicalPortNum [LTM_INVALID_PORT_NUM_CNS].
            Expected: GT_BAD_PARAM.
        */
        logicalPortNum = LTM_INVALID_PORT_NUM_CNS;

        st = cpssDxChLogicalTargetMappingTableEntrySet(dev, logicalDevNum,
                                                  logicalPortNum, &logicalPortMappingTable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, logicalDevNum, logicalPortNum);

        logicalPortNum = LTM_TESTED_PORT_NUM_CNS;
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChLogicalTargetMappingTableEntrySet(dev, logicalDevNum,
                                                  logicalPortNum, &logicalPortMappingTable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChLogicalTargetMappingTableEntrySet(dev, logicalDevNum,
                                              logicalPortNum, &logicalPortMappingTable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChLogicalTargetMappingTableEntryGet
(
    IN GT_U8                            devNum,
    IN GT_U8                            logicalDevNum,
    IN GT_U8                            logicalPortNum,
    OUT CPSS_DXCH_OUTPUT_INTERFACE_STC  *logicalPortMappingTablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChLogicalTargetMappingTableEntryGet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call with logicalDevNum [LTM_LOGICAL_DEV_NUM_CNS],
                   logicalPortNum [LTM_TESTED_PORT_NUM_CNS]
               and logicalPortMappingTable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssDxChLogicalTargetMappingTableEntryGet with the same params.
    Expected: GT_OK and the same value.
    1.3. Call with wrong logicalDevNum [LTM_INVALID_LOGICAL_DEV_NUM_CNS].
    Expected: GT_BAD_PARAM.
    1.4. Call with wrong logicalPortNum [LTM_INVALID_PORT_NUM_CNS].
    Expected: GT_BAD_PARAM.
    1.5. Call with wrong logicalPortMappingTable pointer [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8       logicalDevNum = LTM_LOGICAL_DEV_NUM_CNS;
    GT_U8       logicalPortNum = LTM_TESTED_PORT_NUM_CNS;
    CPSS_DXCH_OUTPUT_INTERFACE_STC  logicalPortMappingTable;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with logicalDevNum [LTM_LOGICAL_DEV_NUM_CNS],
                           logicalPortNum [LTM_TESTED_PORT_NUM_CNS].
        */

        st = cpssDxChLogicalTargetMappingTableEntryGet(dev, logicalDevNum,
                                         logicalPortNum, &logicalPortMappingTable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, logicalDevNum, logicalPortNum);

        /*
            1.2. Call with wrong logicalDevNum [LTM_INVALID_LOGICAL_DEV_NUM_CNS].
            Expected: GT_BAD_PARAM.
        */
        logicalDevNum  = LTM_INVALID_LOGICAL_DEV_NUM_CNS;

        st = cpssDxChLogicalTargetMappingTableEntryGet(dev, logicalDevNum,
                                                logicalPortNum, &logicalPortMappingTable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, logicalDevNum, logicalPortNum);

        logicalDevNum  = LTM_LOGICAL_DEV_NUM_CNS;

        /*
            1.3. Call with wrong logicalPortNum [LTM_INVALID_PORT_NUM_CNS].
            Expected: GT_BAD_PARAM.
        */
        logicalPortNum = LTM_INVALID_PORT_NUM_CNS;

        st = cpssDxChLogicalTargetMappingTableEntrySet(dev, logicalDevNum,
                                                  logicalPortNum, &logicalPortMappingTable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, logicalDevNum, logicalPortNum);

        logicalPortNum = LTM_TESTED_PORT_NUM_CNS;

        /*
            1.4. Call with wrong logicalPortMappingTable pointer [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChLogicalTargetMappingTableEntryGet(dev, logicalDevNum, logicalPortNum, NULL);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PTR, st, dev, logicalDevNum, logicalPortNum);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChLogicalTargetMappingTableEntryGet(dev, logicalDevNum,
                                          logicalPortNum, &logicalPortMappingTable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChLogicalTargetMappingTableEntryGet(dev, logicalDevNum,
                                          logicalPortNum, &logicalPortMappingTable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssDxChLogicalTargetMapping suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChLogicalTargetMapping)

    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChLogicalTargetMappingDeviceEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChLogicalTargetMappingDeviceEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChLogicalTargetMappingEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChLogicalTargetMappingEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChLogicalTargetMappingTableEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChLogicalTargetMappingTableEntryGet)

UTF_SUIT_END_TESTS_MAC(cpssDxChLogicalTargetMapping)
