/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxCfgInitUT.c
*
* DESCRIPTION:
*       Unit Tests for CPSS ExMx initialization of PPs
*       and shadow data structures, and declarations of global variables.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpss/exMx/exMxGen/config/cpssExMxCfgInit.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>


/* Invalid enumeration value used for testing */
#define CFG_INIT_INVALID_ENUM_CNS       0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCfgDevRemove
(
    IN GT_U8   devNum
)
*/
UTF_TEST_CASE_MAC(cpssExMxCfgDevRemove)
{
/*
ITERATE_DEVICES 
1.1. Call function for each active device id. Must return GT_OK.
*/
    /* TODO: I'm not sure that we need to test this function at all */
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCfgDevEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxCfgDevEnable)
{
/*
ITERATE_DEVICES 
1.1. Call function for with enable GT_TRUE and GT_FALSE. Must return GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_BOOL     enable;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with enable GT_TRUE and GT_FALSE.     */
        /* Must return GT_OK.                                           */
        enable = GT_FALSE;

        st = cpssExMxCfgDevEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_TRUE;

        st = cpssExMxCfgDevEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    enable = GT_TRUE;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCfgDevEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCfgDevEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCfgDevMacAddrSet
(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *macPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxCfgDevMacAddrSet)
{
/*
ITERATE_DEVICES
1.1. Call with non-NULL macPtr [AB:00:00:12:FF:FF]. Expected: GT_OK.
1.2. Call with macPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_ETHERADDR    mac = {{0xAB, 0x00, 0x00, 0x12, 0xFF, 0xFF}};


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with non-NULL macPtr [AB:00:00:12:FF:FF].  */
        /* Expected: GT_OK.                                     */
        st = cpssExMxCfgDevMacAddrSet(dev, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call with macPtr [NULL].                        */
        /* Expected: GT_BAD_PTR.                                */
        st = cpssExMxCfgDevMacAddrSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCfgDevMacAddrSet(dev, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCfgDevMacAddrSet(dev, &mac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCfgDevMacAddrLsbModeSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_MAC_SA_LSB_MODE_ENT    saLsbMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxCfgDevMacAddrLsbModeSet)
{
/*
ITERATE_DEVICES
1.1. Call function with saLsbMode [CPSS_SA_LSB_PER_PORT_E]. Expected: GT_OK.
1.2. Call function with saLsbMode [CPSS_SA_LSB_PER_PKT_VID_E]. Expected: GT_OK.
1.3. Call function with saLsbMode [CPSS_SA_LSB_PER_VLAN_E]. Expected: GT_BAD_PARAM.
1.4. Call function with out of range saLsbMode [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    CPSS_MAC_SA_LSB_MODE_ENT    saLsbMode;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with saLsbMode [CPSS_SA_LSB_PER_PORT_E].    */
        /* Expected: GT_OK.                                               */
        saLsbMode = CPSS_SA_LSB_PER_PORT_E;

        st = cpssExMxCfgDevMacAddrLsbModeSet(dev, saLsbMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, saLsbMode);

        /* 1.2. Call function with saLsbMode [CPSS_SA_LSB_PER_PKT_VID_E]. */
        /* Expected: GT_OK.                                               */
        saLsbMode = CPSS_SA_LSB_PER_PKT_VID_E;

        st = cpssExMxCfgDevMacAddrLsbModeSet(dev, saLsbMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, saLsbMode);

        /* 1.3. Call function with saLsbMode [CPSS_SA_LSB_PER_VLAN_E].    */
        /* not supported in EXMX. Expected: GT_BAD_PARAM                  */
        saLsbMode = CPSS_SA_LSB_PER_VLAN_E;

        st = cpssExMxCfgDevMacAddrLsbModeSet(dev, saLsbMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, saLsbMode);

        /* 1.4. Call function with out of range saLsbMode [0x5AAAAAA5].   */
        /* Expected: GT_BAD_PARAM.                                        */
        saLsbMode = CFG_INIT_INVALID_ENUM_CNS;

        st = cpssExMxCfgDevMacAddrLsbModeSet(dev, saLsbMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, saLsbMode);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    saLsbMode = CPSS_SA_LSB_PER_PORT_E;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCfgDevMacAddrLsbModeSet(dev, saLsbMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCfgDevMacAddrLsbModeSet(dev, saLsbMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCfgIngressTrafficToFabricEnable
(
    IN      GT_U8                devNum,
    IN      GT_BOOL              enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxCfgIngressTrafficToFabricEnable)
{
/*
ITERATE_DEVICES 
1.1. Call function for with enable GT_TRUE and GT_FALSE. Must return GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_BOOL     enable;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with enable GT_TRUE and GT_FALSE. */
        /* Must return GT_OK.                                       */
        enable = GT_TRUE;

        st = cpssExMxCfgIngressTrafficToFabricEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_FALSE;

        st = cpssExMxCfgIngressTrafficToFabricEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    enable = GT_TRUE;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCfgIngressTrafficToFabricEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCfgIngressTrafficToFabricEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCfgTrapToFabricEnable
(
    IN GT_U8 devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxCfgTrapToFabricEnable)
{
/*
ITERATE_DEVICES 
1.1. Call function for with enable GT_TRUE and GT_FALSE. Must return GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_BOOL     enable;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with enable GT_TRUE and GT_FALSE. */
        /* Must return GT_OK.                                       */
        enable = GT_TRUE;

        st = cpssExMxCfgTrapToFabricEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_FALSE;

        st = cpssExMxCfgTrapToFabricEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    enable = GT_TRUE;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCfgTrapToFabricEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCfgTrapToFabricEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBuffMngStuckStatusGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *status
)
*/
UTF_TEST_CASE_MAC(cpssExMxBuffMngStuckStatusGet)
{
/*
ITERATE_DEVICES 
1.1. Call function for with non-NULL status. Expect: GT_OK for Twist-D devices of revision 1 and NON GT_OK for others.
1.2. For Twist-D devices of revision 1 call function status [NULL]. Expect: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8       dev;
    GT_BOOL     status;

    CPSS_PP_FAMILY_TYPE_ENT    devFamily;
    GT_U8                      devRevision;

    st = prvUtfNextDeviceReset(&dev, UTF_ALL_FAMILIES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* get device revision number */
        st = prvUtfDeviceRevisionGet(dev, &devRevision);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceRevisionGet: %d", dev);

        /* 1.1. Call function for with non-NULL status.
        Expect: GT_OK for Twist-D devices of revision 1 and NON GT_OK for others. */
        st = cpssExMxBuffMngStuckStatusGet(dev, &status);

        if ((CPSS_PP_FAMILY_TWISTD_E == devFamily) && (1 == devRevision))
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                         "Twist-D device, rev.1: %d", dev);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st,
                                         "not Twist-D rev.1 device: %d", dev);
        }
        /* 1.2. For Twist-D devices of revision 1 call function status [NULL].
        Expect: GT_BAD_PTR. */
        if ((CPSS_PP_FAMILY_TWISTD_E == devFamily) && (1 == devRevision))
        {
            st = cpssExMxBuffMngStuckStatusGet(dev, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
        }
    }

    st = prvUtfNextDeviceReset(&dev, UTF_ALL_FAMILIES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBuffMngStuckStatusGet(dev, &status);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBuffMngStuckStatusGet(dev, &status);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssExMxCfgInit suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxCfgInit)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCfgDevRemove)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCfgDevEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCfgDevMacAddrSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCfgDevMacAddrLsbModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCfgIngressTrafficToFabricEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCfgTrapToFabricEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBuffMngStuckStatusGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxCfgInit)
