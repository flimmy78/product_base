/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChCfgInitUT.c
*
* DESCRIPTION:
*       Unit Tests for CPSS DxCh initialization of PPs
*       and shadow data structures, and declarations of global variables.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

static GT_32 intKey;

/* Lock the interrupts, before re-number */
#define LOCK_INT_CNS cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);

/* Lock the interrupts, after re-number */
#define UNLOCK_INT_CNS cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCfgReNumberDevNum
(
    IN GT_U8   oldDevNum,
    IN GT_U8   newDevNum
)
*/
UTF_TEST_CASE_MAC(cpssDxChCfgReNumberDevNum)
{
/*
    ITERATE_DEVICES
    1.1. Call function with newDevNum [0 / 16 / 31].
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8   newDevNum = 0;
    GT_U8   oldDevNum = 0;


    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with newDevNum [0 / 16 / 31].
            Expected: GT_OK.
        */

        /* call with newDevNum = 0 */
        oldDevNum = dev;
        newDevNum = 0;

        LOCK_INT_CNS;
        st = cpssDxChCfgReNumberDevNum(oldDevNum, newDevNum);
        UNLOCK_INT_CNS;
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, oldDevNum, newDevNum);

        /* call with newDevNum = 16 */
        oldDevNum = dev;
        newDevNum = 16;

        LOCK_INT_CNS;
        st = cpssDxChCfgReNumberDevNum(oldDevNum, newDevNum);
        UNLOCK_INT_CNS;
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, oldDevNum, newDevNum);

        LOCK_INT_CNS;
        st = cpssDxChCfgReNumberDevNum(newDevNum, oldDevNum);
        UNLOCK_INT_CNS;
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, newDevNum, oldDevNum);

        /* call with newDevNum = 31 */
        oldDevNum = dev;
        newDevNum = 31;

        LOCK_INT_CNS;
        st = cpssDxChCfgReNumberDevNum(oldDevNum, newDevNum);
        UNLOCK_INT_CNS;
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, newDevNum);

        LOCK_INT_CNS;
        st = cpssDxChCfgReNumberDevNum(newDevNum, oldDevNum);
        UNLOCK_INT_CNS;
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, newDevNum, oldDevNum);
    }

    newDevNum = 0;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active/non applicable devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active/non applicable device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        LOCK_INT_CNS;
        st = cpssDxChCfgReNumberDevNum(dev, newDevNum);
        UNLOCK_INT_CNS;
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    LOCK_INT_CNS;
    st = cpssDxChCfgReNumberDevNum(dev, newDevNum);
    UNLOCK_INT_CNS;
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCfgDevEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChCfgDevEnable)
{
    /*
    ITERATE_DEVICES
    1.1. Call function with enable [GT_FALSE and GT_TRUE]. Expected: GT_OK for DxCh devices and GT_BAD_PARAM for others.
    */
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_BOOL                     enable;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with enable [GT_FALSE and GT_TRUE].           */
        /* Expected: GT_OK for DxCh devices and GT_BAD_PARAM for others.    */
        enable = GT_FALSE;

        st = cpssDxChCfgDevEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_TRUE;

        st = cpssDxChCfgDevEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    enable = GT_TRUE;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    /*2. Go over all non active/non applicable devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active/non applicable device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChCfgDevEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCfgDevEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_BOOL     modifyEnable
)
*/
UTF_TEST_CASE_MAC(cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet)
{
/*
ITERATE_DEVICES_PHY_PORTS (DxChx)
1.1.1. Call function with enable [GT_FALSE and GT_TRUE]. Expected: GT_OK.
*/

    GT_STATUS                   st = GT_OK;

    GT_U8                       dev;
    GT_U8                       port;
    GT_BOOL                     enable;

    /* 1. Go over all active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. For all active devices go over all available physical ports. */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while(GT_OK == prvUtfNextPhyPortGet(&port,GT_TRUE))
        {
            /* 1.1.1. Call function with enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK. */
            enable = GT_FALSE;

            st = cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            enable = GT_TRUE;

            st = cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
        }

        enable = GT_TRUE;

        /* 1.2. For all active devices go over all non available physical ports. */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;
    port = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCfgDsaTagSrcDevPortModifySet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     modifedDsaSrcDev
)
*/
UTF_TEST_CASE_MAC(cpssDxChCfgDsaTagSrcDevPortModifySet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call function with modifedDsaSrcDev [GT_FALSE and GT_TRUE]. Expected: GT_OK.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_BOOL                     enable;

    /* 1. Go over all active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with enable [GT_FALSE and GT_TRUE].   */
        /* Expected: GT_OK.                                         */
        enable = GT_FALSE;

        st = cpssDxChCfgDsaTagSrcDevPortModifySet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_TRUE;

        st = cpssDxChCfgDsaTagSrcDevPortModifySet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    enable = GT_TRUE;

    /*2. Go over all non active/non applicable devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active/non applicable device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChCfgDsaTagSrcDevPortModifySet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCfgDsaTagSrcDevPortModifySet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCfgTableNumEntriesGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_CFG_TABLES_ENT    table,
    OUT GT_U32                      *numEntriesPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCfgTableNumEntriesGet)
{
/*
    ITERATE_DEVICE (DxChx)
    1.1. Call with table [CPSS_DXCH_CFG_TABLE_VLAN_E /
                          CPSS_DXCH_CFG_TABLE_FDB_E /
                          CPSS_DXCH_CFG_TABLE_PCL_ACTION_E],
              not NULL numEntriesPtr.
    Expected: GT_OK.
    1.2. Call with wrong enum values table.
    Expected: GT_BAD_PARAM.
    1.3. Call with numEntriesPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_CFG_TABLES_ENT    table       = CPSS_DXCH_CFG_TABLE_VLAN_E;
    GT_U32                      numEntries  = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with table [CPSS_DXCH_CFG_TABLE_VLAN_E /
                                  CPSS_DXCH_CFG_TABLE_FDB_E /
                                  CPSS_DXCH_CFG_TABLE_PCL_ACTION_E],
                           not NULL numEntriesPtr.
            Expected: GT_OK.
        */
        /* iterate with table = CPSS_DXCH_CFG_TABLE_VLAN_E */
        table = CPSS_DXCH_CFG_TABLE_VLAN_E;

        st = cpssDxChCfgTableNumEntriesGet(dev, table, &numEntries);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, table);

        /* iterate with table = CPSS_DXCH_CFG_TABLE_FDB_E */
        table = CPSS_DXCH_CFG_TABLE_FDB_E;

        st = cpssDxChCfgTableNumEntriesGet(dev, table, &numEntries);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, table);

        /* iterate with table = CPSS_DXCH_CFG_TABLE_PCL_ACTION_E */
        table = CPSS_DXCH_CFG_TABLE_PCL_ACTION_E;

        st = cpssDxChCfgTableNumEntriesGet(dev, table, &numEntries);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, table);

        /*
            1.2. Call with wrong enum values table.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChCfgTableNumEntriesGet
                            (dev, table, &numEntries),
                            table);

        /*
            1.3. Call with numEntriesPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCfgTableNumEntriesGet(dev, table, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, numEntriesPtr = NULL", dev, table);
    }

    table = CPSS_DXCH_CFG_TABLE_VLAN_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCfgTableNumEntriesGet(dev, table, &numEntries);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCfgTableNumEntriesGet(dev, table, &numEntries);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCfgDevRemove
(
    IN  GT_U8                       devNum,
)
*/
UTF_TEST_CASE_MAC(cpssDxChCfgDevRemove)
{
/*
    ITERATE_DEVICE (DxChx)
    1.1. Call with correct dev.
    Expected: GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with correct dev.
            Expected: GT_OK.
        */

        /*st = cpssDxChCfgDevRemove(dev);*/
        /*UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);*/
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCfgDevRemove(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCfgDevRemove(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCfgPpLogicalInit
(
    IN GT_U8                           devNum,
    IN CPSS_DXCH_PP_CONFIG_INIT_STC    *ppConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCfgPpLogicalInit)
{
/*
    ITERATE_DEVICE (DxChx)
    1.1. Call with correct ppConfigPtr.
    Expected: GT_OK.
    1.2. Call with wrong ppConfigPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    CPSS_DXCH_PP_CONFIG_INIT_STC ppConfigPtr;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with correct ppConfigPtr.
            Expected: GT_OK.
        */
        ppConfigPtr.maxNumOfVirtualRouters = 10;
        ppConfigPtr.maxNumOfIpNextHop = 10;
        ppConfigPtr.maxNumOfIpv4Prefixes = 10;
        ppConfigPtr.maxNumOfMll = 10;
        ppConfigPtr.maxNumOfIpv4McEntries = 10;
        ppConfigPtr.maxNumOfIpv6Prefixes = 10;
        ppConfigPtr.maxNumOfTunnelEntries = 10;
        ppConfigPtr.maxNumOfIpv4TunnelTerms = 10;
        ppConfigPtr.routingMode = CPSS_DXCH_TCAM_ROUTER_BASED_E;

        st = cpssDxChCfgPpLogicalInit(dev, &ppConfigPtr);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /*
            1.2. Call with wrong ppConfigPtr [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChCfgPpLogicalInit(dev, NULL);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCfgPpLogicalInit(dev, &ppConfigPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCfgPpLogicalInit(dev, &ppConfigPtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCfgDevInfoGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_DXCH_CFG_DEV_INFO_STC   *devInfoPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCfgDevInfoGet)
{
/*
    ITERATE_DEVICE (DxChx)
    1.1. Call with not NULL devInfoPtr.
    Expected: GT_OK.
    1.2. Call with devInfoPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_CFG_DEV_INFO_STC   devInfo;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL devInfoPtr.
            Expected: GT_OK.
        */
        st = cpssDxChCfgDevInfoGet(dev, &devInfo);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with devInfoPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCfgDevInfoGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, %d, devInfoPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCfgDevInfoGet(dev, &devInfo);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCfgDevInfoGet(dev, &devInfo);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCfgBindPortPhymacObject
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN CPSS_MACDRV_OBJ_STC *macDrvObjPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCfgBindPortPhymacObject)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call function with not NULL macDrvObj pointer.
    Expected: GT_OK. 
    1.1.2. Call function with NULL macDrvObj pointer.
    Expected: GT_BAD_PTR. 
*/

    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_U8                       port;
    CPSS_MACDRV_OBJ_STC         macDrvObj;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. For all active devices go over all available physical ports. */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port,GT_TRUE))
        {
            /* 
                1.1.1. Call function with not NULL macDrvObj pointer.
                Expected: GT_OK. 
            */
            st = cpssDxChCfgBindPortPhymacObject(dev, port, &macDrvObj);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* clean object pointer to avoid failures of other tests */
            st = prvCpssPpConfigPortToPhymacObjectBind(dev, port, NULL);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 
                1.1.2. Call function with NULL macDrvObj pointer.
                Expected: GT_BAD_PTR. 
            */
            st = cpssDxChCfgBindPortPhymacObject(dev, port, NULL);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, dev, port);
        }

        /* 1.2. For all active devices go over all non available physical ports. */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChCfgBindPortPhymacObject(dev, port, &macDrvObj);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChCfgBindPortPhymacObject(dev, port, &macDrvObj);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChCfgBindPortPhymacObject(dev, port, &macDrvObj);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCfgBindPortPhymacObject(dev, port, &macDrvObj);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCfgBindPortPhymacObject(dev, port, &macDrvObj);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCfgIngressDropCntrSet
(
    IN  GT_U8       devNum,
    IN  GT_U32      counter
)
*/
UTF_TEST_CASE_MAC(cpssDxChCfgIngressDropCntrSet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call function with counter [0xFFFF]. Expected: GT_OK.
     Read the counter and get same value.
1.2. Call function with counter [0xFFFFFFFF]. Expected: GT_OK.
     Read the counter and get same value.
1.3. Call function with counter [0x0]. Expected: GT_OK.
     Read the counter and get same value.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_U32                      counter;
    GT_U32                      counterGet;

    /* 1. Go over all active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with counter [0xFFFF].   */
        /* Expected: GT_OK.                                         */
        counter = 0xFFFF;

        st = cpssDxChCfgIngressDropCntrSet(dev, counter);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counter);

        st = cpssDxChCfgIngressDropCntrGet(dev, &counterGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                     "cpssDxChCfgIngressDropCntrGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counter, counterGet,
                                     "get another ingress drop counter value than was set: %d", dev);

        /* 1.2. Call function with counter [0xFFFFFFFF].   */
        /* Expected: GT_OK.                                         */
        counter = 0xFFFFFFFF;

        st = cpssDxChCfgIngressDropCntrSet(dev, counter);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counter);

        st = cpssDxChCfgIngressDropCntrGet(dev, &counterGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                     "cpssDxChCfgIngressDropCntrGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counter, counterGet,
                                     "get another ingress drop counter value than was set: %d", dev);

        /* 1.3. Call function with counter [0x0].   */
        /* Expected: GT_OK.                                         */
        counter = 0x0;

        st = cpssDxChCfgIngressDropCntrSet(dev, counter);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counter);

        st = cpssDxChCfgIngressDropCntrGet(dev, &counterGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                     "cpssDxChCfgIngressDropCntrGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counter, counterGet,
                                     "get another ingress drop counter value than was set: %d", dev);
    }

    counter = 0x0;

    /*2. Go over all non active/non applicable devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active/non applicable device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChCfgIngressDropCntrSet(dev, counter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCfgIngressDropCntrSet(dev, counter);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCfgIngressDropCntrGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *counterPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCfgIngressDropCntrGet)
{
/*
    ITERATE_DEVICE (DxChx)
    1.1. Call with not NULL counterPtr.
    Expected: GT_OK.
    1.2. Call with counterPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U32      counter;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL devInfoPtr.
            Expected: GT_OK.
        */
        st = cpssDxChCfgIngressDropCntrGet(dev, &counter);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                     "cpssDxChCfgIngressDropCntrGet: %d", dev);

        /*
            1.2. Call with devInfoPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCfgIngressDropCntrGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, counterPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCfgIngressDropCntrGet(dev, &counter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCfgIngressDropCntrGet(dev, &counter);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChCfgInit suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChCfgInit)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCfgReNumberDevNum)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCfgDevEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCfgDsaTagSrcDevPortModifySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCfgTableNumEntriesGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCfgDevRemove)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCfgPpLogicalInit)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCfgDevInfoGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCfgBindPortPhymacObject)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCfgIngressDropCntrSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCfgIngressDropCntrGet)
UTF_SUIT_END_TESTS_MAC(cpssDxChCfgInit)
