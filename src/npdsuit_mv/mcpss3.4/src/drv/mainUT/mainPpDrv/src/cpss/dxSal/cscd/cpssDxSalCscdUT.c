/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxSalCscdUT.c
*
* DESCRIPTION:
*       Unit tests for Cascading facility core implementation -- Salsa.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/dxSal/cscd/cpssDxSalCscd.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* Invalid enum */
#define CSCD_INVALID_ENUM_CNS        0x5AAAAAA5

/* Max value for portNum in cpssDxSalCscdDevMapTableSet  */
#define CPSS_SAL_MAX_CASCADE_PORT_NUM_CNS   31

/* Max value for targetDevNum in cpssDxSalCscdDevMapTableSet  */
#define CPSS_SAL_MAX_CASCADE_TARGET_DEV_NUM_CNS   31

/* Default valid value for physical port id */
#define CSCD_VALID_PHY_PORT_CNS      0

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalCscdPortTypeSet
(
    IN GT_U8                          devNum,
    IN GT_U8                          portNum,
    IN CPSS_CSCD_PORT_TYPE_ENT        portType
)
*/
UTF_TEST_CASE_MAC(cpssDxSalCscdPortTypeSet)
{
/*
ITERATE_DEVICES_PHY_PORTS 
1.1.1. Call function with portType [CPSS_CSCD_PORT_DSA_MODE_REGULAR_E / CPSS_CSCD_PORT_NETWORK_E]. Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.
1.1.2. Check unsupported portType. For Salsa devices call function with portType [CPSS_CSCD_PORT_DSA_MODE_EXTEND_E]. Expected: non GT_OK.
1.1.3. Check out-of-range enum. For Salsa devices call with portType [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS                       st = GT_OK;

    GT_U8                           portNum;
    GT_U8                           dev;
    CPSS_CSCD_PORT_TYPE_ENT         portType;

    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices (dev). */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&portNum, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For each active device (dev) id go over all active Physical portNums. */
        while (GT_OK == prvUtfNextPhyPortGet(&portNum, GT_TRUE))
        {
            /* 1.1.1. Call function with portType                 */
            /* [CPSS_CSCD_PORT_DSA_MODE_REGULAR_E /               */
            /*  CPSS_CSCD_PORT_NETWORK_E]. Expected: GT_OK for    */
            /* Salsa devices and GT_BAD_PARAM for others.         */

            portType = CPSS_CSCD_PORT_DSA_MODE_REGULAR_E;

            st = cpssDxSalCscdPortTypeSet(dev, portNum, portType);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                              "Salsa device: %d, %d, %d",
                              dev, portNum, portType);

            portType = CPSS_CSCD_PORT_NETWORK_E;

            st = cpssDxSalCscdPortTypeSet(dev, portNum, portType);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                              "Salsa device: %d, %d, %d",
                              dev, portNum, portType);

            /* 1.1.2. For Salsa devices call function with portType   */
            /* [CPSS_CSCD_PORT_DSA_MODE_EXTEND_E]. Expected: non GT_OK.   */

            portType = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;

            st = cpssDxSalCscdPortTypeSet(dev, portNum, portType);

            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                              "Salsa device: %d, %d, %d",
                              dev, portNum, portType);

            /* 1.1.3. Check out-of-range enum. For Salsa devices call   */
            /* with portType [0x5AAAAAA5]. Expected: GT_BAD_PARAM.      */

            portType = CSCD_INVALID_ENUM_CNS;

            st = cpssDxSalCscdPortTypeSet(dev, portNum, portType);

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                              "Salsa device: %d, portType=%d",
                              dev, portType);
        }

        /* Only for Salsa test non-active/out-of-range PhyPort */
        portType = CPSS_CSCD_PORT_DSA_MODE_REGULAR_E;

        st = cpssDxSalCscdPortTypeSet(dev, portNum, portType);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over all non active Physical portNums and active device id (dev). */
        while (GT_OK == prvUtfNextPhyPortGet(&portNum, GT_FALSE))
        {
            /* 1.2.1. <Call function for non active portNum and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                                 */
            st = cpssDxSalCscdPortTypeSet(dev, portNum, portType);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portNum);
        }

        /* 1.3. Call with out of range for Physical portNum.                       */
        /* Expected: GT_BAD_PARAM.                                                */
        /* portType = CPSS_DXSAL_CSCD_PORT_DSA_MODE_REGULAR_E;  */

        portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalCscdPortTypeSet(dev, portNum, portType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portNum);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        portNum = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalCscdPortTypeSet(dev, portNum, portType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portNum);
    }

    /* set valid params */
    portNum = CSCD_VALID_PHY_PORT_CNS;
    portType = CPSS_CSCD_PORT_DSA_MODE_REGULAR_E;


    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices (dev). */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device (dev) and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */

        st = cpssDxSalCscdPortTypeSet(dev, portNum, portType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id (dev).   */
    /* Expected: GT_BAD_PARAM.                                  */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* portNum == 0         */

    st = cpssDxSalCscdPortTypeSet(dev, portNum, portType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalCscdDevMapTableSet
(
    IN GT_U8                    devNum,
    IN GT_U8                    targetDevNum,
    IN GT_U8                    portNum
)
*/
UTF_TEST_CASE_MAC(cpssDxSalCscdDevMapTableSet)
{
/*
ITERATE_DEVICES 
1.1. Call function with targetDevNum [0], portNum [0 / CPSS_SAL_MAX_CASCADE_PORT_NUM_CNS]. Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.
1.2. Check out-of-range targetDevNum. For Salsa devices call function with targetDevNum [CPSS_SAL_MAX_CASCADE_TARGET_DEV_NUM_CNS + 1], portNum [0]. Expected: GT_BAD_PARAM .
1.3. Check out-of-range portNum. For Salsa devices call function with targetDevNum [0], portNum [ CPSS_SAL_MAX_CASCADE_PORT_NUM_CNS +1 ]. Expected: non GT_OK.
*/

    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_U8                       targetDevNum;
    GT_U8                       portNum;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with targetDevNum [0], portNum [0 / CPSS_SAL_MAX_CASCADE_PORT_NUM_CNS]    */
        /* Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.*/

        targetDevNum = 0;
        portNum = 0;

        st = cpssDxSalCscdDevMapTableSet(dev, targetDevNum, portNum);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "Salsa device: %d, %d, %d",
                                     dev, targetDevNum, portNum);

        portNum = CPSS_SAL_MAX_CASCADE_PORT_NUM_CNS;

        st = cpssDxSalCscdDevMapTableSet(dev, targetDevNum, portNum);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "Salsa device: %d, %d, %d",
                                     dev, targetDevNum, portNum);

        /* 1.2. Check out-of-range targetDevNum. For Salsa devices call */
        /* function with targetDevNum [CPSS_SAL_MAX_CASCADE_TARGET_DEV_NUM_CNS+1],*/
        /* portNum [0]. Expected: GT_BAD_PARAM.                     */

        targetDevNum = CPSS_SAL_MAX_CASCADE_TARGET_DEV_NUM_CNS+1;
        portNum = 0;

        st = cpssDxSalCscdDevMapTableSet(dev, targetDevNum, portNum);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, 
            "Salsa device: %d, targetDevNum=%d", dev, targetDevNum);

        targetDevNum = 0;

        /* 1.3. Check out-of-range portNum. For Salsa devices call  */
        /* function with targetDevNum [0], portNum                  */
        /* [CPSS_SAL_MAX_CASCADE_PORT_NUM_CNS + 1]. Expected: non GT_OK.*/

        targetDevNum = 0;
        portNum = CPSS_SAL_MAX_CASCADE_PORT_NUM_CNS + 1;

        st = cpssDxSalCscdDevMapTableSet(dev, targetDevNum, portNum);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, 
            "Salsa device: %d, portNum=%d", dev, portNum);

        portNum = 0;
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* set valid params */
    targetDevNum = 0;
    portNum = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalCscdDevMapTableSet(dev, targetDevNum, portNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalCscdDevMapTableSet(dev, targetDevNum, portNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalCscdDsaSrcDevFilterSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enableOwnDevFltr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalCscdDsaSrcDevFilterSet)
{
/*
ITERATE_DEVICES
1.1. Call with enable [GT_TRUE and GT_FALSE]. Expected: GT_OK for Salsa devices and GT_BAD_PARAM for others.
*/

    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_BOOL                     enableOwnDevFltr;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with enable [GT_TRUE and GT_FALSE]. Expected:  */
        /* GT_OK for Salsa devices and GT_BAD_PARAM for others.     */

        enableOwnDevFltr = GT_TRUE;

        st = cpssDxSalCscdDsaSrcDevFilterSet(dev, enableOwnDevFltr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, enableOwnDevFltr);

        enableOwnDevFltr = GT_FALSE;

        st = cpssDxSalCscdDsaSrcDevFilterSet(dev, enableOwnDevFltr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Salsa device: %d, %d",
                                     dev, enableOwnDevFltr);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* set valid params */
    enableOwnDevFltr = GT_TRUE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalCscdDsaSrcDevFilterSet(dev, enableOwnDevFltr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalCscdDsaSrcDevFilterSet(dev, enableOwnDevFltr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssDxSalCscd suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxSalCscd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalCscdPortTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalCscdDevMapTableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalCscdDsaSrcDevFilterSet)
UTF_SUIT_END_TESTS_MAC(cpssDxSalCscd)
