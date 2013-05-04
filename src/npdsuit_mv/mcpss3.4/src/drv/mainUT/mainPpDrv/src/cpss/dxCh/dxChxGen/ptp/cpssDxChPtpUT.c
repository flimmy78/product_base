/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPtpUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxChPtp, that provides
*       CPSS DxCh Precision Time Protocol function implementations.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/ptp/cpssDxChPtp.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* Defines */

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpCpuCodeBaseGet
(
    IN  GT_U8                     dev,
    OUT CPSS_NET_RX_CPU_CODE_ENT  *cpuCodePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpCpuCodeBaseGet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with not null cpuCodePtr.
    Expected: GT_OK.
    1.2. Call api with wrong cpuCodePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                     dev;
    CPSS_NET_RX_CPU_CODE_ENT  cpuCode;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not null cpuCodePtr.
            Expected: GT_OK.
        */
        st = cpssDxChPtpCpuCodeBaseGet(dev, &cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call api with wrong cpuCodePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPtpCpuCodeBaseGet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* restore correct values */
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpCpuCodeBaseGet(dev, &cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpCpuCodeBaseGet(dev, &cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpCpuCodeBaseSet
(
    IN  GT_U8                    dev,
    IN  CPSS_NET_RX_CPU_CODE_ENT cpuCode
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpCpuCodeBaseSet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with cpuCode[CPSS_NET_CONTROL_E /
                           CPSS_NET_UN_KNOWN_UC_E /
                           CPSS_NET_UN_REGISTERD_MC_E /
                           CPSS_NET_CONTROL_BPDU_E /
                           CPSS_NET_CONTROL_DEST_MAC_TRAP_E],
    Expected: GT_OK.
    1.2. Call cpssDxChPtpCpuCodeBaseGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.3. Call api with wrong cpuCode [wrong enum values].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                    dev;
    CPSS_NET_RX_CPU_CODE_ENT cpuCode = CPSS_NET_CONTROL_E;
    CPSS_NET_RX_CPU_CODE_ENT cpuCodeGet = CPSS_NET_CONTROL_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cpuCode[CPSS_NET_CONTROL_E /
                                   CPSS_NET_UN_KNOWN_UC_E /
                                   CPSS_NET_UN_REGISTERD_MC_E /
                                   CPSS_NET_CONTROL_BPDU_E /
                                   CPSS_NET_CONTROL_DEST_MAC_TRAP_E],
            Expected: GT_OK.
        */
        /* call with cpuCode[CPSS_NET_CONTROL_E] */
        cpuCode = CPSS_NET_CONTROL_E;

        st = cpssDxChPtpCpuCodeBaseSet(dev, cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpCpuCodeBaseGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpCpuCodeBaseGet(dev, &cpuCodeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpCpuCodeBaseGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet, 
                       "got another cpuCode then was set: %d", dev);

        /* call with cpuCode[CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E] */
        cpuCode = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E;

        st = cpssDxChPtpCpuCodeBaseSet(dev, cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpCpuCodeBaseGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpCpuCodeBaseGet(dev, &cpuCodeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpCpuCodeBaseGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet, 
                       "got another cpuCode then was set: %d", dev);

        /* call with cpuCode[CPSS_NET_IPV6_HOP_BY_HOP_E] */
        cpuCode = CPSS_NET_IPV6_HOP_BY_HOP_E;

        st = cpssDxChPtpCpuCodeBaseSet(dev, cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpCpuCodeBaseGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpCpuCodeBaseGet(dev, &cpuCodeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpCpuCodeBaseGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet, 
                       "got another cpuCode then was set: %d", dev);

        /* call with cpuCode[CPSS_NET_CONTROL_BPDU_E] */
        cpuCode = CPSS_NET_CONTROL_BPDU_E;

        st = cpssDxChPtpCpuCodeBaseSet(dev, cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpCpuCodeBaseGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpCpuCodeBaseGet(dev, &cpuCodeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpCpuCodeBaseGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet, 
                       "got another cpuCode then was set: %d", dev);

        /* call with cpuCode[CPSS_NET_CLASS_KEY_MIRROR_E] */
        cpuCode = CPSS_NET_CLASS_KEY_MIRROR_E;

        st = cpssDxChPtpCpuCodeBaseSet(dev, cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpCpuCodeBaseGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpCpuCodeBaseGet(dev, &cpuCodeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpCpuCodeBaseGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet, 
                       "got another cpuCode then was set: %d", dev);

        /*
            1.3. Call api with wrong cpuCode [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpCpuCodeBaseSet
                            (dev, cpuCode),
                            cpuCode);
    }

    /* restore correct values */
    cpuCode = CPSS_NET_CONTROL_E;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpCpuCodeBaseSet(dev, cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpCpuCodeBaseSet(dev, cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpEtherTypeGet
(
    IN  GT_U8   dev,
    IN  GT_U32  etherTypeIndex,
    OUT GT_U32  *etherTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpEtherTypeGet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with etherTypeIndex[0 / 1],
    Expected: GT_OK.
    1.2. Call api with wrong etherTypeIndex [2].
    Expected: NOT GT_OK.
    1.3. Call api with wrong etherTypePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U32  etherTypeIndex = 0;
    GT_U32  etherType;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with etherTypeIndex[0 / 1],
            Expected: GT_OK.
        */
        /* call with etherTypeIndex[0] */
        etherTypeIndex = 0;

        st = cpssDxChPtpEtherTypeGet(dev, etherTypeIndex, &etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with etherTypeIndex[1] */
        etherTypeIndex = 1;

        st = cpssDxChPtpEtherTypeGet(dev, etherTypeIndex, &etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call api with wrong etherTypeIndex [2].
            Expected: NOT GT_OK.
        */
        etherTypeIndex = 2;

        st = cpssDxChPtpEtherTypeGet(dev, etherTypeIndex, &etherType);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        etherTypeIndex = 0;

        /*
            1.3. Call api with wrong etherTypePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPtpEtherTypeGet(dev, etherTypeIndex, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* restore correct values */
    etherTypeIndex = 0;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpEtherTypeGet(dev, etherTypeIndex, &etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpEtherTypeGet(dev, etherTypeIndex, &etherType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpEtherTypeSet
(
    IN  GT_U8  dev,
    IN  GT_U32 etherTypeIndex,
    IN  GT_U32 etherType
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpEtherTypeSet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with etherTypeIndex[0 / 1], etherType[0 / 0xFFFF],
    Expected: GT_OK.
    1.2. Call cpssDxChPtpEtherTypeGet with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.3. Call api with wrong etherTypeIndex [2].
    Expected: NOT GT_OK.
    1.4. Call api with wrong etherType [0xFFFF + 1].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8  dev;
    GT_U32 etherTypeIndex = 0;
    GT_U32 etherType = 0;
    GT_U32 etherTypeGet = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with etherTypeIndex[0 / 1], etherType[0 / 0xFFFF],
            Expected: GT_OK.
        */

        /* call with etherTypeIndex[0], etherType[0] */
        etherTypeIndex = 0;
        etherType = 0;

        st = cpssDxChPtpEtherTypeSet(dev, etherTypeIndex, etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpEtherTypeGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpEtherTypeGet(dev, etherTypeIndex, &etherTypeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpEtherTypeGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(etherType, etherTypeGet, 
                       "got another etherType then was set: %d", dev);

        /* call with etherTypeIndex[1], etherType[0xFFFF] */
        etherTypeIndex = 1;
        etherType = 0xFFFF;

        st = cpssDxChPtpEtherTypeSet(dev, etherTypeIndex, etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpEtherTypeGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpEtherTypeGet(dev, etherTypeIndex, &etherTypeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpEtherTypeGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(etherType, etherTypeGet, 
                       "got another etherType then was set: %d", dev);

        /*
            1.3. Call api with wrong etherTypeIndex [2].
            Expected: NOT GT_OK.
        */
        etherTypeIndex = 2;

        st = cpssDxChPtpEtherTypeSet(dev, etherTypeIndex, etherType);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        etherTypeIndex = 0;

        /*
            1.4. Call api with wrong etherType [0xFFFF + 1].
            Expected: NOT GT_OK.
        */
        etherType = 0xFFFF + 1;

        st = cpssDxChPtpEtherTypeSet(dev, etherTypeIndex, etherType);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        etherType = 0;
    }

    /* restore correct values */
    etherTypeIndex = 0;
    etherType = 0;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpEtherTypeSet(dev, etherTypeIndex, etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpEtherTypeSet(dev, etherTypeIndex, etherType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpEthernetTimeStampEnableGet
(
    IN  GT_U8                    dev,
    IN  CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_BOOL                  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpEthernetTimeStampEnableGet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E]
    Expected: GT_OK.
    1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
    Expected: NOT GT_OK.
    1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.4. Call api with wrong enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                    dev;
    CPSS_PORT_DIRECTION_ENT  direction = CPSS_PORT_DIRECTION_RX_E;
    GT_BOOL                  enable;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E]
            Expected: GT_OK.
        */

        /* call with direction[CPSS_PORT_DIRECTION_RX_E] */
        direction = CPSS_PORT_DIRECTION_RX_E;

        st = cpssDxChPtpEthernetTimeStampEnableGet(dev, direction, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with direction[CPSS_PORT_DIRECTION_TX_E] */
        direction = CPSS_PORT_DIRECTION_TX_E;

        st = cpssDxChPtpEthernetTimeStampEnableGet(dev, direction, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
            Expected: NOT GT_OK.
        */
        direction = CPSS_PORT_DIRECTION_BOTH_E;

        st = cpssDxChPtpEthernetTimeStampEnableGet(dev, direction, &enable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        direction = CPSS_PORT_DIRECTION_TX_E;

        /*
            1.3. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpEthernetTimeStampEnableGet
                            (dev, direction, &enable),
                            direction);

        /*
            1.4. Call api with wrong enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPtpEthernetTimeStampEnableGet(dev, direction, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpEthernetTimeStampEnableGet(dev, direction, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpEthernetTimeStampEnableGet(dev, direction, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpEthernetTimeStampEnableSet
(
    IN  GT_U8                   dev,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_BOOL                 enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpEthernetTimeStampEnableSet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E /
                             CPSS_PORT_DIRECTION_BOTH_E],
                   enable[GT_TRUE / GT_FALSE / GT_TRUE],
    Expected: GT_OK.
    1.2. Call cpssDxChPtpEthernetTimeStampEnableGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                   dev;
    CPSS_PORT_DIRECTION_ENT direction = CPSS_PORT_DIRECTION_RX_E;
    GT_BOOL                 enable = GT_FALSE;
    GT_BOOL                 enableGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E /
                                     CPSS_PORT_DIRECTION_BOTH_E],
                           enable[GT_TRUE / GT_FALSE / GT_TRUE],
            Expected: GT_OK.
        */
        /* call with direction[CPSS_PORT_DIRECTION_RX_E], enable[GT_TRUE] */
        direction = CPSS_PORT_DIRECTION_RX_E;
        enable = GT_TRUE;

        st = cpssDxChPtpEthernetTimeStampEnableSet(dev, direction, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpEthernetTimeStampEnableGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpEthernetTimeStampEnableGet(dev, direction, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpEthernetTimeStampEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        /* call with direction[CPSS_PORT_DIRECTION_TX_E], enable[GT_FALSE] */
        direction = CPSS_PORT_DIRECTION_TX_E;
        enable = GT_FALSE;

        st = cpssDxChPtpEthernetTimeStampEnableSet(dev, direction, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpEthernetTimeStampEnableGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpEthernetTimeStampEnableGet(dev, direction, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpEthernetTimeStampEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        /* call with direction[CPSS_PORT_DIRECTION_BOTH_E], enable[GT_TRUE] */
        direction = CPSS_PORT_DIRECTION_BOTH_E;
        enable = GT_TRUE;

        st = cpssDxChPtpEthernetTimeStampEnableSet(dev, direction, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpEthernetTimeStampEnableGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        /* We can't get both directions, they should be read separately */
        direction = CPSS_PORT_DIRECTION_RX_E;
        st = cpssDxChPtpEthernetTimeStampEnableGet(dev, direction, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpEthernetTimeStampEnableGet: %d ", dev);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        direction = CPSS_PORT_DIRECTION_TX_E;
        st = cpssDxChPtpEthernetTimeStampEnableGet(dev, direction, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpEthernetTimeStampEnableGet: %d ", dev);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        /*
            1.3. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpEthernetTimeStampEnableSet
                            (dev, direction, enable),
                            direction);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
    enable = GT_TRUE;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpEthernetTimeStampEnableSet(dev, direction, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpEthernetTimeStampEnableSet(dev, direction, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpMessageTypeCmdGet
(
    IN  GT_U8                dev,
    IN  GT_U32               messageType,
    OUT CPSS_PACKET_CMD_ENT  *commandPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpMessageTypeCmdGet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with messageType[0 / 5 / 15],
    Expected: GT_OK.
    1.2. Call api with wrong messageType [16].
    Expected: NOT GT_OK.
    1.3. Call api with wrong commandPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                dev;
    GT_U32               messageType = 0;
    CPSS_PACKET_CMD_ENT  command;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with messageType[0 / 5 / 15],
            Expected: GT_OK.
        */
        /* call with messageType[0] */
        messageType = 0;

        st = cpssDxChPtpMessageTypeCmdGet(dev, messageType, &command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with messageType[5] */
        messageType = 5;

        st = cpssDxChPtpMessageTypeCmdGet(dev, messageType, &command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with messageType[15] */
        messageType = 15;

        st = cpssDxChPtpMessageTypeCmdGet(dev, messageType, &command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call api with wrong messageType [16].
            Expected: NOT GT_OK.
        */
        messageType = 16;

        st = cpssDxChPtpMessageTypeCmdGet(dev, messageType, &command);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        messageType = 0;

        /*
            1.3. Call api with wrong commandPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPtpMessageTypeCmdGet(dev, messageType, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* restore correct values */
    messageType = 0;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpMessageTypeCmdGet(dev, messageType, &command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpMessageTypeCmdGet(dev, messageType, &command);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpMessageTypeCmdSet
(
    IN  GT_U8               dev,
    IN  GT_U32              messageType,
    IN  CPSS_PACKET_CMD_ENT command
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpMessageTypeCmdSet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with messageType[0 / 3 / 5 / 10 / 15],
                   command[CPSS_PACKET_CMD_FORWARD_E /
                           CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                           CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                           CPSS_PACKET_CMD_DROP_HARD_E /
                           CPSS_PACKET_CMD_DROP_SOFT_E],
    Expected: GT_OK.
    1.2. Call cpssDxChPtpMessageTypeCmdGet with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.3. Call api with wrong messageType [16].
    Expected: NOT GT_OK.
    1.4. Call api with wrong command [wrong enum values].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8               dev;
    GT_U32              messageType = 0;
    CPSS_PACKET_CMD_ENT command = CPSS_PACKET_CMD_FORWARD_E;
    CPSS_PACKET_CMD_ENT commandGet = CPSS_PACKET_CMD_FORWARD_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with messageType[0 / 3 / 5 / 10 / 15],
                           command[CPSS_PACKET_CMD_FORWARD_E /
                                   CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                                   CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                                   CPSS_PACKET_CMD_DROP_HARD_E /
                                   CPSS_PACKET_CMD_DROP_SOFT_E],
            Expected: GT_OK.
        */

        /* call with messageType[0], command[CPSS_PACKET_CMD_FORWARD_E] */
        messageType = 0;
        command = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssDxChPtpMessageTypeCmdSet(dev, messageType, command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpMessageTypeCmdGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpMessageTypeCmdGet(dev, messageType, &commandGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpMessageTypeCmdGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(command, commandGet, 
                       "got another command then was set: %d", dev);

        /* call with messageType[3], command[CPSS_PACKET_CMD_MIRROR_TO_CPU_E] */
        messageType = 3;
        command = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssDxChPtpMessageTypeCmdSet(dev, messageType, command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpMessageTypeCmdGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpMessageTypeCmdGet(dev, messageType, &commandGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpMessageTypeCmdGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(command, commandGet, 
                       "got another command then was set: %d", dev);

        /* call with messageType[5], command[CPSS_PACKET_CMD_TRAP_TO_CPU_E] */
        messageType = 5;
        command = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssDxChPtpMessageTypeCmdSet(dev, messageType, command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpMessageTypeCmdGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpMessageTypeCmdGet(dev, messageType, &commandGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpMessageTypeCmdGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(command, commandGet, 
                       "got another command then was set: %d", dev);

        /* call with messageType[10], command[CPSS_PACKET_CMD_DROP_HARD_E] */
        messageType = 10;
        command = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssDxChPtpMessageTypeCmdSet(dev, messageType, command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpMessageTypeCmdGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpMessageTypeCmdGet(dev, messageType, &commandGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpMessageTypeCmdGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(command, commandGet, 
                       "got another command then was set: %d", dev);

        /* call with messageType[15], command[CPSS_PACKET_CMD_DROP_SOFT_E] */
        messageType = 15;
        command = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssDxChPtpMessageTypeCmdSet(dev, messageType, command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpMessageTypeCmdGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpMessageTypeCmdGet(dev, messageType, &commandGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpMessageTypeCmdGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(command, commandGet, 
                       "got another command then was set: %d", dev);

        /*
            1.3. Call api with wrong messageType [16].
            Expected: NOT GT_OK.
        */
        messageType = 16;

        st = cpssDxChPtpMessageTypeCmdSet(dev, messageType, command);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        messageType = 0;

        /*
            1.4. Call api with wrong command [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpMessageTypeCmdSet
                            (dev, messageType, command),
                            command);
    }

    /* restore correct values */
    messageType = 0;
    command = CPSS_PACKET_CMD_FORWARD_E;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpMessageTypeCmdSet(dev, messageType, command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpMessageTypeCmdSet(dev, messageType, command);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpMessageTypeTimeStampEnableGet
(
    IN  GT_U8                    dev,
    IN  CPSS_PORT_DIRECTION_ENT  direction,
    IN  GT_U32                   messageType,
    OUT GT_BOOL                  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpMessageTypeTimeStampEnableGet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E]
                   messageType[0 / 15],
    Expected: GT_OK.
    1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
    Expected: NOT GT_OK.
    1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.4. Call api with wrong messageType [16].
    Expected: NOT GT_OK.
    1.5. Call api with wrong enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                    dev;
    CPSS_PORT_DIRECTION_ENT  direction = CPSS_PORT_DIRECTION_RX_E;
    GT_U32                   messageType = 0;
    GT_BOOL                  enable;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E]
                           messageType[0 / 15],
            Expected: GT_OK.
        */
        /* call with direction[CPSS_PORT_DIRECTION_RX_E], messageType[0] */
        direction = CPSS_PORT_DIRECTION_RX_E;
        messageType = 0;

        st = cpssDxChPtpMessageTypeTimeStampEnableGet(dev, direction, 
                                     messageType, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with direction[CPSS_PORT_DIRECTION_TX_E], messageType[15] */
        direction = CPSS_PORT_DIRECTION_TX_E;
        messageType = 15;

        st = cpssDxChPtpMessageTypeTimeStampEnableGet(dev, direction, 
                                     messageType, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
            Expected: NOT GT_OK.
        */
        direction = CPSS_PORT_DIRECTION_BOTH_E;

        st = cpssDxChPtpMessageTypeTimeStampEnableGet(dev, direction, 
                                     messageType, &enable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        direction = CPSS_PORT_DIRECTION_TX_E;

        /*
            1.3. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpMessageTypeTimeStampEnableGet
                            (dev, direction, messageType, &enable),
                            direction);

        /*
            1.4. Call api with wrong messageType [16].
            Expected: NOT GT_OK.
        */
        messageType = 16;

        st = cpssDxChPtpMessageTypeTimeStampEnableGet(dev, direction, 
                                     messageType, &enable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        messageType = 0;

        /*
            1.5. Call api with wrong enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPtpMessageTypeTimeStampEnableGet(dev, direction, 
                                     messageType, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
    messageType = 0;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpMessageTypeTimeStampEnableGet(dev, direction, 
                                     messageType, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpMessageTypeTimeStampEnableGet(dev, direction, 
                                     messageType, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpMessageTypeTimeStampEnableSet
(
    IN  GT_U8                   dev,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  messageType,
    IN  GT_BOOL                 enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpMessageTypeTimeStampEnableSet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E /
                             CPSS_PORT_DIRECTION_BOTH_E],
                   messageType[0 / 10 / 15],
                   enable[GT_TRUE /GT_FALSE /GT_TRUE],
    Expected: GT_OK.
    1.2. Call cpssDxChPtpMessageTypeTimeStampEnableGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.4. Call api with wrong messageType [16].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8                   dev;
    CPSS_PORT_DIRECTION_ENT direction = CPSS_PORT_DIRECTION_RX_E;
    GT_U32                  messageType = 0;
    GT_BOOL                 enable = GT_FALSE;
    GT_BOOL                 enableGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E /
                                     CPSS_PORT_DIRECTION_BOTH_E],
                           messageType[0 / 10 / 15],
                           enable[GT_TRUE /GT_FALSE /GT_TRUE],
            Expected: GT_OK.
        */
        /* call with direction[CPSS_PORT_DIRECTION_RX_E],
                     messageType[0], enable[GT_TRUE] */
        direction = CPSS_PORT_DIRECTION_RX_E;
        messageType = 0;
        enable = GT_TRUE;

        st = cpssDxChPtpMessageTypeTimeStampEnableSet(dev, direction, 
                                     messageType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpMessageTypeTimeStampEnableGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpMessageTypeTimeStampEnableGet(dev, direction, 
                                     messageType, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpMessageTypeTimeStampEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        /* call with direction[CPSS_PORT_DIRECTION_TX_E],
                     messageType[10], enable[GT_FALSE] */
        direction = CPSS_PORT_DIRECTION_TX_E;
        messageType = 10;
        enable = GT_FALSE;

        st = cpssDxChPtpMessageTypeTimeStampEnableSet(dev, direction, 
                                     messageType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpMessageTypeTimeStampEnableGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpMessageTypeTimeStampEnableGet(dev, direction, 
                                     messageType, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpMessageTypeTimeStampEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        /* call with direction[CPSS_PORT_DIRECTION_BOTH_E],
                     messageType[15], enable[GT_TRUE] */
        direction = CPSS_PORT_DIRECTION_BOTH_E;
        messageType = 15;
        enable = GT_TRUE;

        st = cpssDxChPtpMessageTypeTimeStampEnableSet(dev, direction, 
                                     messageType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpMessageTypeTimeStampEnableGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        direction = CPSS_PORT_DIRECTION_RX_E;
        st = cpssDxChPtpMessageTypeTimeStampEnableGet(dev, direction, 
                                     messageType, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpMessageTypeTimeStampEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        direction = CPSS_PORT_DIRECTION_TX_E;
        st = cpssDxChPtpMessageTypeTimeStampEnableGet(dev, direction, 
                                     messageType, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpMessageTypeTimeStampEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        /*
            1.3. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpMessageTypeTimeStampEnableSet
                            (dev, direction, messageType, enable),
                            direction);

        /*
            1.4. Call api with wrong messageType [16].
            Expected: NOT GT_OK.
        */
        messageType = 16;

        st = cpssDxChPtpMessageTypeTimeStampEnableSet(dev, direction, 
                                     messageType, enable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        messageType = 0;
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
    messageType = 0;
    enable = GT_TRUE;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpMessageTypeTimeStampEnableSet(dev, direction, 
                                     messageType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpMessageTypeTimeStampEnableSet(dev, direction, 
                                     messageType, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpPortGroupTodCounterFunctionGet
(
    IN  GT_U8                               dev,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  CPSS_PORT_DIRECTION_ENT             direction,
    OUT CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT  *functionPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpPortGroupTodCounterFunctionGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(Lion and above)
    1.1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                               CPSS_PORT_DIRECTION_TX_E]
    Expected: GT_OK.
    1.1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
    Expected: NOT GT_OK.
    1.1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.1.4. Call api with wrong functionPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;
    GT_U32    portGroupId;

    GT_U8                               dev;
    GT_PORT_GROUPS_BMP                  portGroupsBmp = 1;
    CPSS_PORT_DIRECTION_ENT             direction = CPSS_PORT_DIRECTION_RX_E;
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT  function;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);
            
            /*
                1.1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                           CPSS_PORT_DIRECTION_TX_E]
                Expected: GT_OK.
            */
            /* call with direction[CPSS_PORT_DIRECTION_RX_E] */
            direction = CPSS_PORT_DIRECTION_RX_E;

            st = cpssDxChPtpPortGroupTodCounterFunctionGet(dev, portGroupsBmp, 
                                     direction, &function);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call with direction[CPSS_PORT_DIRECTION_TX_E] */
            direction = CPSS_PORT_DIRECTION_TX_E;

            st = cpssDxChPtpPortGroupTodCounterFunctionGet(dev, portGroupsBmp, 
                                     direction, &function);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
                Expected: NOT GT_OK.
            */
            /* call with direction[CPSS_PORT_DIRECTION_BOTH_E] */
            direction = CPSS_PORT_DIRECTION_BOTH_E;

            st = cpssDxChPtpPortGroupTodCounterFunctionGet(dev, portGroupsBmp, 
                                     direction, &function);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            direction = CPSS_PORT_DIRECTION_TX_E;

            /*
                1.1.3. Call api with wrong direction [wrong enum values].
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPtpPortGroupTodCounterFunctionGet
                                (dev, portGroupsBmp, direction, &function),
                                direction);

            /*
                1.1.4. Call api with wrong functionPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPtpPortGroupTodCounterFunctionGet(dev, portGroupsBmp, 
                                     direction, NULL);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)
        
        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);
            
            st = cpssDxChPtpPortGroupTodCounterFunctionGet(dev, portGroupsBmp, 
                                     direction, &function);
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
        
        /* 1.3. For unaware port groups check that function returns GT_OK. */ 
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS; 
        
        st = cpssDxChPtpPortGroupTodCounterFunctionGet(dev, portGroupsBmp, 
                                     direction, &function);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpPortGroupTodCounterFunctionGet(dev, portGroupsBmp, 
                                     direction, &function);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpPortGroupTodCounterFunctionGet(dev, portGroupsBmp, 
                                     direction, &function);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpPortGroupTodCounterFunctionSet
(
    IN  GT_U8                              dev,
    IN  GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN  CPSS_PORT_DIRECTION_ENT            direction,
    IN  CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT function
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpPortGroupTodCounterFunctionSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(Lion and above)
    1.1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                               CPSS_PORT_DIRECTION_TX_E /
                               CPSS_PORT_DIRECTION_BOTH_E /
                               CPSS_PORT_DIRECTION_RX_E],
                   function[CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E /
                              CPSS_DXCH_PTP_TOD_COUNTER_FUNC_INCREMENT_E /
                              CPSS_DXCH_PTP_TOD_COUNTER_FUNC_CAPTURE_E /
                              CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GENERATE_E],
    Expected: GT_OK.
    1.1.2. Call cpssDxChPtpPortGroupTodCounterFunctionGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.1.4. Call api with wrong function [wrong enum values].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;
    GT_U32    portGroupId;

    GT_U8                              dev;
    GT_PORT_GROUPS_BMP                 portGroupsBmp = 1;
    CPSS_PORT_DIRECTION_ENT            direction = CPSS_PORT_DIRECTION_RX_E;
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT function = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E;
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT functionGet = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);
            
            /*
                1.1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                           CPSS_PORT_DIRECTION_TX_E /
                                           CPSS_PORT_DIRECTION_BOTH_E /
                                           CPSS_PORT_DIRECTION_RX_E],
                               function[CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E /
                                          CPSS_DXCH_PTP_TOD_COUNTER_FUNC_INCREMENT_E /
                                          CPSS_DXCH_PTP_TOD_COUNTER_FUNC_CAPTURE_E /
                                          CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GENERATE_E],
                Expected: GT_OK.
            */
            /* call with direction[CPSS_PORT_DIRECTION_RX_E],
                         function[CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E] */
            direction = CPSS_PORT_DIRECTION_RX_E;
            function = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E;

            st = cpssDxChPtpPortGroupTodCounterFunctionSet(dev, portGroupsBmp, 
                                     direction, function);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChPtpPortGroupTodCounterFunctionGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChPtpPortGroupTodCounterFunctionGet(dev, portGroupsBmp, 
                                     direction, &functionGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPtpPortGroupTodCounterFunctionGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(function, functionGet, 
                           "got another function then was set: %d", dev);

            /* call with direction[CPSS_PORT_DIRECTION_TX_E],
                         function[CPSS_DXCH_PTP_TOD_COUNTER_FUNC_INCREMENT_E] */
            direction = CPSS_PORT_DIRECTION_TX_E;
            function = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_INCREMENT_E;

            st = cpssDxChPtpPortGroupTodCounterFunctionSet(dev, portGroupsBmp, 
                                     direction, function);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChPtpPortGroupTodCounterFunctionGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChPtpPortGroupTodCounterFunctionGet(dev, portGroupsBmp, 
                                     direction, &functionGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPtpPortGroupTodCounterFunctionGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(function, functionGet, 
                           "got another function then was set: %d", dev);

            /* call with direction[CPSS_PORT_DIRECTION_BOTH_E],
                         function[CPSS_DXCH_PTP_TOD_COUNTER_FUNC_CAPTURE_E] */
            direction = CPSS_PORT_DIRECTION_BOTH_E;
            function = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_CAPTURE_E;

            st = cpssDxChPtpPortGroupTodCounterFunctionSet(dev, portGroupsBmp, 
                                     direction, function);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChPtpPortGroupTodCounterFunctionGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            direction = CPSS_PORT_DIRECTION_RX_E;
            st = cpssDxChPtpPortGroupTodCounterFunctionGet(dev, portGroupsBmp, 
                                     direction, &functionGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPtpPortGroupTodCounterFunctionGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(function, functionGet, 
                           "got another function then was set: %d", dev);

            direction = CPSS_PORT_DIRECTION_TX_E;
            st = cpssDxChPtpPortGroupTodCounterFunctionGet(dev, portGroupsBmp, 
                                     direction, &functionGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPtpPortGroupTodCounterFunctionGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(function, functionGet, 
                           "got another function then was set: %d", dev);

            /* call with direction[CPSS_PORT_DIRECTION_RX_E],
                         function[CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GENERATE_E] */
            direction = CPSS_PORT_DIRECTION_RX_E;
            function = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GENERATE_E;

            st = cpssDxChPtpPortGroupTodCounterFunctionSet(dev, portGroupsBmp, 
                                     direction, function);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChPtpPortGroupTodCounterFunctionGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChPtpPortGroupTodCounterFunctionGet(dev, portGroupsBmp, 
                                     direction, &functionGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPtpPortGroupTodCounterFunctionGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(function, functionGet, 
                           "got another function then was set: %d", dev);

            /*
                1.1.3. Call api with wrong direction [wrong enum values].
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPtpPortGroupTodCounterFunctionSet
                                (dev, portGroupsBmp, direction, function),
                                direction);

            /*
                1.1.4. Call api with wrong function [wrong enum values].
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPtpPortGroupTodCounterFunctionSet
                                (dev, portGroupsBmp, direction, function),
                                function);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)
        
        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);
            
            st = cpssDxChPtpPortGroupTodCounterFunctionSet(dev, portGroupsBmp, 
                                     direction, function);
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
        
        /* 1.3. For unaware port groups check that function returns GT_OK. */ 
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS; 
        
        st = cpssDxChPtpPortGroupTodCounterFunctionSet(dev, portGroupsBmp, 
                                     direction, function);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
    function = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpPortGroupTodCounterFunctionSet(dev, portGroupsBmp, 
                                     direction, function);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpPortGroupTodCounterFunctionSet(dev, portGroupsBmp, 
                                     direction, function);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpPortGroupTodCounterFunctionTriggerGet
(
    IN  GT_U8                    dev,
    IN  GT_PORT_GROUPS_BMP       portGroupsBmp,
    IN  CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_BOOL                  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpPortGroupTodCounterFunctionTriggerGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(Lion and above)
    1.1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                               CPSS_PORT_DIRECTION_TX_E]
    Expected: GT_OK.
    1.1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
    Expected: NOT GT_OK.
    1.1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.1.4. Call api with wrong enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;
    GT_U32    portGroupId;

    GT_U8                    dev;
    GT_PORT_GROUPS_BMP       portGroupsBmp = 1;
    CPSS_PORT_DIRECTION_ENT  direction = CPSS_PORT_DIRECTION_RX_E;
    GT_BOOL                  enable;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);
            
            /*
                1.1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                           CPSS_PORT_DIRECTION_TX_E]
                Expected: GT_OK.
            */
            /* call with direction[CPSS_PORT_DIRECTION_RX_E] */
            direction = CPSS_PORT_DIRECTION_RX_E;

            st = cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(dev, 
                            portGroupsBmp, direction, &enable);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call with direction[CPSS_PORT_DIRECTION_TX_E] */
            direction = CPSS_PORT_DIRECTION_TX_E;

            st = cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(dev, 
                            portGroupsBmp, direction, &enable);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
                Expected: NOT GT_OK.
            */
            direction = CPSS_PORT_DIRECTION_BOTH_E;

            st = cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(dev, 
                            portGroupsBmp, direction, &enable);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            direction = CPSS_PORT_DIRECTION_TX_E;

            /*
                1.1.3. Call api with wrong direction [wrong enum values].
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPtpPortGroupTodCounterFunctionTriggerGet
                                (dev, portGroupsBmp, direction, &enable),
                                direction);

            /*
                1.1.4. Call api with wrong enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(dev, 
                            portGroupsBmp, direction, NULL);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)
        
        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);
            
            st = cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(dev, 
                            portGroupsBmp, direction, &enable);
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
        
        /* 1.3. For unaware port groups check that function returns GT_OK. */ 
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS; 
        
        st = cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(dev, 
                            portGroupsBmp, direction, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(dev, 
                            portGroupsBmp, direction, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(dev, 
                            portGroupsBmp, direction, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpPortGroupTodCounterFunctionTriggerSet
(
    IN  GT_U8                   dev,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  CPSS_PORT_DIRECTION_ENT direction
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpPortGroupTodCounterFunctionTriggerSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(Lion and above)
    1.1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                               CPSS_PORT_DIRECTION_TX_E /
                               CPSS_PORT_DIRECTION_BOTH_E],
    Expected: GT_OK.
    1.1.2. Call cpssDxChPtpPortGroupTodCounterFunctionTriggerGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;
    GT_U32    portGroupId;

    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    CPSS_PORT_DIRECTION_ENT direction = CPSS_PORT_DIRECTION_RX_E;
    GT_BOOL                 enable = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);
            
            /*
                1.1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                           CPSS_PORT_DIRECTION_TX_E /
                                           CPSS_PORT_DIRECTION_BOTH_E],
                Expected: GT_OK.
            */

            /* call with direction[CPSS_PORT_DIRECTION_RX_E] */
            direction = CPSS_PORT_DIRECTION_RX_E;

            st = cpssDxChPtpPortGroupTodCounterFunctionTriggerSet(dev, 
                                     portGroupsBmp, direction);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChPtpPortGroupTodCounterFunctionTriggerGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(dev, 
                            portGroupsBmp, direction, &enable);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPtpPortGroupTodCounterFunctionTriggerGet: %d ", dev);

            /* Verifying values */

            /* call with direction[CPSS_PORT_DIRECTION_TX_E] */
            direction = CPSS_PORT_DIRECTION_TX_E;

            st = cpssDxChPtpPortGroupTodCounterFunctionTriggerSet(dev, 
                                     portGroupsBmp, direction);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChPtpPortGroupTodCounterFunctionTriggerGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(dev, 
                            portGroupsBmp, direction, &enable);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPtpPortGroupTodCounterFunctionTriggerGet: %d ", dev);

            /* Verifying values */

            /* call with direction[CPSS_PORT_DIRECTION_BOTH_E] */
            direction = CPSS_PORT_DIRECTION_BOTH_E;

            st = cpssDxChPtpPortGroupTodCounterFunctionTriggerSet(dev, 
                                     portGroupsBmp, direction);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChPtpPortGroupTodCounterFunctionTriggerGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            direction = CPSS_PORT_DIRECTION_RX_E;
            st = cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(dev, 
                            portGroupsBmp, direction, &enable);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPtpPortGroupTodCounterFunctionTriggerGet: %d ", dev);

            direction = CPSS_PORT_DIRECTION_TX_E;
            st = cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(dev, 
                            portGroupsBmp, direction, &enable);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPtpPortGroupTodCounterFunctionTriggerGet: %d ", dev);

            /* Verifying values */

            /*
                1.1.3. Call api with wrong direction [wrong enum values].
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPtpPortGroupTodCounterFunctionTriggerSet
                                (dev, portGroupsBmp, direction),
                                direction);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)
        
        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);
            
            st = cpssDxChPtpPortGroupTodCounterFunctionTriggerSet(dev, 
                                     portGroupsBmp, direction);
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
        
        /* 1.3. For unaware port groups check that function returns GT_OK. */ 
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS; 
        
        st = cpssDxChPtpPortGroupTodCounterFunctionTriggerSet(dev, 
                                     portGroupsBmp, direction);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpPortGroupTodCounterFunctionTriggerSet(dev, 
                                     portGroupsBmp, direction);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpPortGroupTodCounterFunctionTriggerSet(dev, 
                                     portGroupsBmp, direction);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpPortGroupTodCounterGet
(
    IN  GT_U8                        dev,
    IN  GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN  CPSS_PORT_DIRECTION_ENT      direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC  *todCounterPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpPortGroupTodCounterGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(Lion and above)
    1.1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                               CPSS_PORT_DIRECTION_TX_E]
    Expected: GT_OK.
    1.1.2. Call with direction[CPSS_PORT_DIRECTION_BOTH_E],
    Expected: NOT GT_OK.
    1.1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.1.4. Call api with wrong todCounterPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;
    GT_U32    portGroupId;

    GT_U8                        dev;
    GT_PORT_GROUPS_BMP           portGroupsBmp = 1;
    CPSS_PORT_DIRECTION_ENT      direction = CPSS_PORT_DIRECTION_RX_E;
    CPSS_DXCH_PTP_TOD_COUNT_STC  todCounter;

    cpssOsBzero((GT_VOID*) &todCounter, sizeof(todCounter));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);
            
            /*
                1.1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                           CPSS_PORT_DIRECTION_TX_E]
                Expected: GT_OK.
            */
            /* call with direction[CPSS_PORT_DIRECTION_RX_E] */
            direction = CPSS_PORT_DIRECTION_RX_E;

            st = cpssDxChPtpPortGroupTodCounterGet(dev, portGroupsBmp, 
                                     direction, &todCounter);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call with direction[CPSS_PORT_DIRECTION_TX_E] */
            direction = CPSS_PORT_DIRECTION_TX_E;

            st = cpssDxChPtpPortGroupTodCounterGet(dev, portGroupsBmp, 
                                     direction, &todCounter);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call with direction[CPSS_PORT_DIRECTION_BOTH_E],
                Expected: NOT GT_OK.
            */
            direction = CPSS_PORT_DIRECTION_BOTH_E;

            st = cpssDxChPtpPortGroupTodCounterGet(dev, portGroupsBmp, 
                                     direction, &todCounter);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            direction = CPSS_PORT_DIRECTION_TX_E;

            /*
                1.1.3. Call api with wrong direction [wrong enum values].
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPtpPortGroupTodCounterGet
                                (dev, portGroupsBmp, direction, &todCounter),
                                direction);

            /*
                1.1.4. Call api with wrong todCounterPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPtpPortGroupTodCounterGet(dev, portGroupsBmp, 
                                     direction, NULL);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)
        
        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);
            
            st = cpssDxChPtpPortGroupTodCounterGet(dev, portGroupsBmp, 
                                     direction, &todCounter);
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
        
        /* 1.3. For unaware port groups check that function returns GT_OK. */ 
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS; 
        
        st = cpssDxChPtpPortGroupTodCounterGet(dev, portGroupsBmp, 
                                     direction, &todCounter);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpPortGroupTodCounterGet(dev, portGroupsBmp, 
                                     direction, &todCounter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpPortGroupTodCounterGet(dev, portGroupsBmp, 
                                     direction, &todCounter);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpPortGroupTodCounterShadowGet
(
    IN  GT_U8                        dev,
    IN  GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN  CPSS_PORT_DIRECTION_ENT      direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC  *todCounterPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpPortGroupTodCounterShadowGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(Lion and above)
    1.1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                               CPSS_PORT_DIRECTION_TX_E]
    Expected: GT_OK.
    1.1.2. Call with direction[CPSS_PORT_DIRECTION_BOTH_E],
    Expected: NOT GT_OK.
    1.1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.1.4. Call api with wrong todCounterPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;
    GT_U32    portGroupId;

    GT_U8                        dev;
    GT_PORT_GROUPS_BMP           portGroupsBmp = 1;
    CPSS_PORT_DIRECTION_ENT      direction = CPSS_PORT_DIRECTION_RX_E;
    CPSS_DXCH_PTP_TOD_COUNT_STC  todCounter;

    cpssOsBzero((GT_VOID*) &todCounter, sizeof(todCounter));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);
            
            /*
                1.1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                           CPSS_PORT_DIRECTION_TX_E]
                Expected: GT_OK.
            */

            /* call with direction[CPSS_PORT_DIRECTION_RX_E] */
            direction = CPSS_PORT_DIRECTION_RX_E;

            st = cpssDxChPtpPortGroupTodCounterShadowGet(dev, portGroupsBmp, 
                                     direction, &todCounter);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call with direction[CPSS_PORT_DIRECTION_TX_E] */
            direction = CPSS_PORT_DIRECTION_TX_E;

            st = cpssDxChPtpPortGroupTodCounterShadowGet(dev, portGroupsBmp, 
                                     direction, &todCounter);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call with direction[CPSS_PORT_DIRECTION_BOTH_E],
                Expected: NOT GT_OK.
            */
            /* call with direction[CPSS_PORT_DIRECTION_BOTH_E] */
            direction = CPSS_PORT_DIRECTION_BOTH_E;

            st = cpssDxChPtpPortGroupTodCounterShadowGet(dev, portGroupsBmp, 
                                     direction, &todCounter);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            direction = CPSS_PORT_DIRECTION_TX_E;

            /*
                1.1.3. Call api with wrong direction [wrong enum values].
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPtpPortGroupTodCounterShadowGet
                                (dev, portGroupsBmp, direction, &todCounter),
                                direction);

            /*
                1.1.4. Call api with wrong todCounterPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPtpPortGroupTodCounterShadowGet(dev, portGroupsBmp, 
                                     direction, NULL);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)
        
        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);
            
            st = cpssDxChPtpPortGroupTodCounterShadowGet(dev, portGroupsBmp, 
                                     direction, &todCounter);
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
        
        /* 1.3. For unaware port groups check that function returns GT_OK. */ 
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS; 
        
        st = cpssDxChPtpPortGroupTodCounterShadowGet(dev, portGroupsBmp, 
                                     direction, &todCounter);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpPortGroupTodCounterShadowGet(dev, portGroupsBmp, 
                                     direction, &todCounter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpPortGroupTodCounterShadowGet(dev, portGroupsBmp, 
                                     direction, &todCounter);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpPortGroupTodCounterShadowSet
(
    IN  GT_U8                       dev,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  CPSS_PORT_DIRECTION_ENT     direction,
    IN  CPSS_DXCH_PTP_TOD_COUNT_STC todCounter
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpPortGroupTodCounterShadowSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(Lion and above)
    1.1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                               CPSS_PORT_DIRECTION_TX_E /
                               CPSS_PORT_DIRECTION_BOTH_E],
                    todCounter.nanoSeconds [0 / 100 / 1000]
                    todCounter.seconds     [0 / 100 / 1000];
    Expected: GT_OK.
    1.1.2. Call cpssDxChPtpPortGroupTodCounterShadowGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;
    GT_U32    portGroupId;
    GT_BOOL   isEqual = GT_FALSE;

    GT_U8                       dev;
    GT_PORT_GROUPS_BMP          portGroupsBmp = 1;
    CPSS_PORT_DIRECTION_ENT     direction = CPSS_PORT_DIRECTION_RX_E;
    CPSS_DXCH_PTP_TOD_COUNT_STC todCounter;
    CPSS_DXCH_PTP_TOD_COUNT_STC todCounterGet;

    cpssOsBzero((GT_VOID*) &todCounter, sizeof(todCounter));
    cpssOsBzero((GT_VOID*) &todCounter, sizeof(todCounter));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);
            
            /*
                1.1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                           CPSS_PORT_DIRECTION_TX_E /
                                           CPSS_PORT_DIRECTION_BOTH_E],
                                todCounter.nanoSeconds [0 / 100 / 1000]
                                todCounter.seconds     [0 / 100 / 1000];
                Expected: GT_OK.
            */
            /* call with direction[CPSS_PORT_DIRECTION_RX_E] */
            direction = CPSS_PORT_DIRECTION_RX_E;
            todCounter.nanoSeconds = 0;
            todCounter.seconds.l[0] = 0;
            todCounter.seconds.l[1] = 0;

            st = cpssDxChPtpPortGroupTodCounterShadowSet(dev, portGroupsBmp, 
                                     direction, &todCounter);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChPtpPortGroupTodCounterShadowGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChPtpPortGroupTodCounterShadowGet(dev, portGroupsBmp, 
                                     direction, &todCounterGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPtpPortGroupTodCounterShadowGet: %d ", dev);

            /* Verifying values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&todCounter, 
                                         (GT_VOID*)&todCounterGet,
                      sizeof (todCounter))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another todCounter than was set: %d", dev);

            /* call with direction[CPSS_PORT_DIRECTION_TX_E] */
            direction = CPSS_PORT_DIRECTION_TX_E;
            todCounter.nanoSeconds = 100;
            todCounter.seconds.l[0] = 100;
            todCounter.seconds.l[1] = 0;

            st = cpssDxChPtpPortGroupTodCounterShadowSet(dev, portGroupsBmp, 
                                     direction, &todCounter);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChPtpPortGroupTodCounterShadowGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChPtpPortGroupTodCounterShadowGet(dev, portGroupsBmp, 
                                     direction, &todCounterGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPtpPortGroupTodCounterShadowGet: %d ", dev);

            /* Verifying values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&todCounter, 
                                         (GT_VOID*)&todCounterGet,
                      sizeof (todCounter))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another todCounter than was set: %d", dev);

            /* call with direction[CPSS_PORT_DIRECTION_BOTH_E] */
            direction = CPSS_PORT_DIRECTION_BOTH_E;

            todCounter.nanoSeconds = 1000;
            todCounter.seconds.l[0] = 1000;
            todCounter.seconds.l[1] = 0;

            st = cpssDxChPtpPortGroupTodCounterShadowSet(dev, portGroupsBmp, 
                                     direction, &todCounter);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChPtpPortGroupTodCounterShadowGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            direction = CPSS_PORT_DIRECTION_RX_E;
            st = cpssDxChPtpPortGroupTodCounterShadowGet(dev, portGroupsBmp, 
                                     direction, &todCounterGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPtpPortGroupTodCounterShadowGet: %d ", dev);

            /* Verifying values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&todCounter, 
                                         (GT_VOID*)&todCounterGet,
                      sizeof (todCounter))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another todCounter than was set: %d", dev);

            direction = CPSS_PORT_DIRECTION_TX_E;
            st = cpssDxChPtpPortGroupTodCounterShadowGet(dev, portGroupsBmp, 
                                     direction, &todCounterGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPtpPortGroupTodCounterShadowGet: %d ", dev);

            /* Verifying values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&todCounter, 
                                         (GT_VOID*)&todCounterGet,
                      sizeof (todCounter))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another todCounter than was set: %d", dev);

            /* check if WA for nanoseconds is enabled */
            if (GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(dev, PRV_CPSS_DXCH_XCAT_TOD_NANO_SEC_SET_WA_E))
            {
                /* set nanosecond to value that is not a multiply of 20 */

                /* call with direction[CPSS_PORT_DIRECTION_BOTH_E] */
                direction = CPSS_PORT_DIRECTION_BOTH_E;

                todCounter.nanoSeconds = 1150;
                todCounter.seconds.l[0] = 200500;
                todCounter.seconds.l[1] = 500;

                st = cpssDxChPtpPortGroupTodCounterShadowSet(dev, portGroupsBmp, 
                                         direction, &todCounter);
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

                /* calculate nanosecons value */
                todCounter.nanoSeconds = (todCounter.nanoSeconds / 20);
                todCounter.nanoSeconds = (todCounter.nanoSeconds * 20);

                /*
                    1.1.2. Call cpssDxChPtpPortGroupTodCounterShadowGet
                           with the same parameters.
                    Expected: GT_OK and the same values than was set.
                */
                direction = CPSS_PORT_DIRECTION_RX_E;
                st = cpssDxChPtpPortGroupTodCounterShadowGet(dev, portGroupsBmp, 
                                         direction, &todCounterGet);
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                          "cpssDxChPtpPortGroupTodCounterShadowGet: %d ", dev);

                /* Verifying values */
                isEqual = (0 == cpssOsMemCmp((GT_VOID*)&todCounter, 
                                             (GT_VOID*)&todCounterGet,
                          sizeof (todCounter))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                           "get another todCounter than was set: %d", dev);

                direction = CPSS_PORT_DIRECTION_TX_E;
                st = cpssDxChPtpPortGroupTodCounterShadowGet(dev, portGroupsBmp, 
                                         direction, &todCounterGet);
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                          "cpssDxChPtpPortGroupTodCounterShadowGet: %d ", dev);

                /* Verifying values */
                isEqual = (0 == cpssOsMemCmp((GT_VOID*)&todCounter, 
                                             (GT_VOID*)&todCounterGet,
                          sizeof (todCounter))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                           "get another todCounter than was set: %d", dev);
            }



            /*
                1.1.3. Call api with wrong direction [wrong enum values].
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPtpPortGroupTodCounterShadowSet
                                (dev, portGroupsBmp, direction, &todCounter),
                                direction);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)
        
        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);
            
            st = cpssDxChPtpPortGroupTodCounterShadowSet(dev, portGroupsBmp, 
                                     direction, &todCounter);
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
        
        /* 1.3. For unaware port groups check that function returns GT_OK. */ 
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS; 
        
        st = cpssDxChPtpPortGroupTodCounterShadowSet(dev, portGroupsBmp, 
                                     direction, &todCounter);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
    todCounter.nanoSeconds = 0;
    todCounter.seconds.l[0] = 1000;
    todCounter.seconds.l[1] = 0;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpPortGroupTodCounterShadowSet(dev, portGroupsBmp, 
                                     direction, &todCounter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpPortGroupTodCounterShadowSet(dev, portGroupsBmp, 
                                     direction, &todCounter);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpPortTimeStampEnableGet
(
    IN  GT_U8                    dev,
    IN  CPSS_PORT_DIRECTION_ENT  direction,
    IN  GT_U8                    portNum,
    OUT GT_BOOL                  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpPortTimeStampEnableGet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E]
                   portNum[0 / 10],
    Expected: GT_OK.
    1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
    Expected: NOT GT_OK.
    1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.4. Call api with wrong portNum [128].
    Expected: NOT GT_OK.
    1.5. Call api with wrong enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                    dev;
    CPSS_PORT_DIRECTION_ENT  direction = CPSS_PORT_DIRECTION_RX_E;
    GT_U8                    portNum = 0;
    GT_BOOL                  enable;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E]
                           portNum[0 / 10],
            Expected: GT_OK.
        */

        /* call with direction[CPSS_PORT_DIRECTION_RX_E], portNum[0] */
        direction = CPSS_PORT_DIRECTION_RX_E;
        portNum = 0;

        st = cpssDxChPtpPortTimeStampEnableGet(dev, direction, portNum, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with direction[CPSS_PORT_DIRECTION_TX_E], portNum[10] */
        direction = CPSS_PORT_DIRECTION_TX_E;
        portNum = 10;

        st = cpssDxChPtpPortTimeStampEnableGet(dev, direction, portNum, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
            Expected: NOT GT_OK.
        */
        direction = CPSS_PORT_DIRECTION_BOTH_E;
        portNum = 0;

        st = cpssDxChPtpPortTimeStampEnableGet(dev, direction, portNum, &enable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        direction = CPSS_PORT_DIRECTION_TX_E;

        /*
            1.3. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpPortTimeStampEnableGet
                            (dev, direction, portNum, &enable),
                            direction);

        /*
            1.4. Call api with wrong portNum [128].
            Expected: NOT GT_OK.
        */
        portNum = 128;
        st = cpssDxChPtpPortTimeStampEnableGet(dev, direction, portNum, &enable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.5. Call api with wrong enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        portNum = 20;
        st = cpssDxChPtpPortTimeStampEnableGet(dev, direction, portNum, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
    portNum = 0;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpPortTimeStampEnableGet(dev, direction, portNum, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpPortTimeStampEnableGet(dev, direction, portNum, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpPortTimeStampEnableSet
(
    IN  GT_U8                   dev,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U8                   portNum,
    IN  GT_BOOL                 enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpPortTimeStampEnableSet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E /
                             CPSS_PORT_DIRECTION_BOTH_E],
                   portNum[0],
                   enable[GT_TRUE /GT_FALSE /GT_TRUE],
    Expected: GT_OK.
    1.2. Call cpssDxChPtpPortTimeStampEnableGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.2. Call cpssDxChPtpPortTimeStampEnableGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.2. Call cpssDxChPtpPortTimeStampEnableGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                   dev;
    CPSS_PORT_DIRECTION_ENT direction = CPSS_PORT_DIRECTION_RX_E;
    GT_U8                   portNum = 0;
    GT_BOOL                 enable = GT_FALSE;
    GT_BOOL                 enableGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E /
                                     CPSS_PORT_DIRECTION_BOTH_E],
                           portNum[0],
                           enable[GT_TRUE /GT_FALSE /GT_TRUE],
            Expected: GT_OK.
        */
        /* call with direction[CPSS_PORT_DIRECTION_RX_E],
                     portNum[0], enable[GT_TRUE] */
        direction = CPSS_PORT_DIRECTION_RX_E;
        portNum = 0;
        enable = GT_TRUE;

        st = cpssDxChPtpPortTimeStampEnableSet(dev, direction, portNum, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpPortTimeStampEnableGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpPortTimeStampEnableGet(dev, direction, portNum, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpPortTimeStampEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        /* call with direction[CPSS_PORT_DIRECTION_TX_E],
                     portNum[0], enable[GT_FALSE] */
        direction = CPSS_PORT_DIRECTION_TX_E;
        portNum = 0;
        enable = GT_FALSE;

        st = cpssDxChPtpPortTimeStampEnableSet(dev, direction, portNum, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpPortTimeStampEnableGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpPortTimeStampEnableGet(dev, direction, portNum, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpPortTimeStampEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        /* call with direction[CPSS_PORT_DIRECTION_BOTH_E],
                     portNum[0], enable[GT_TRUE] */
        direction = CPSS_PORT_DIRECTION_BOTH_E;
        portNum = 0;
        enable = GT_TRUE;

        st = cpssDxChPtpPortTimeStampEnableSet(dev, direction, portNum, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpPortTimeStampEnableGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        direction = CPSS_PORT_DIRECTION_RX_E;
        st = cpssDxChPtpPortTimeStampEnableGet(dev, direction, portNum, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpPortTimeStampEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        direction = CPSS_PORT_DIRECTION_TX_E;
        st = cpssDxChPtpPortTimeStampEnableGet(dev, direction, portNum, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpPortTimeStampEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        /*
            1.3. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpPortTimeStampEnableSet
                            (dev, direction, portNum, enable),
                            direction);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
    portNum = 0;
    enable = GT_TRUE;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpPortTimeStampEnableSet(dev, direction, portNum, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpPortTimeStampEnableSet(dev, direction, portNum, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpTimestampEntryGet
(
    IN  GT_U8                              dev,
    IN  CPSS_PORT_DIRECTION_ENT            direction,
    OUT CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC  *entryPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpTimestampEntryGet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E /
                             CPSS_PORT_DIRECTION_BOTH_E],
    Expected: GT_OK.
    1.2. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.3. Call api with wrong entryPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                              dev;
    CPSS_PORT_DIRECTION_ENT            direction = CPSS_PORT_DIRECTION_RX_E;
    CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC  entry;

    cpssOsBzero((GT_VOID*) &entry, sizeof(entry));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E],
            Expected: GT_OK.
        */
        /* call with direction[CPSS_PORT_DIRECTION_RX_E] */
        direction = CPSS_PORT_DIRECTION_RX_E;

        st = cpssDxChPtpTimestampEntryGet(dev, direction, &entry);
        if (st == GT_NO_MORE) 
        {
            st = GT_OK; 
        }
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with direction[CPSS_PORT_DIRECTION_TX_E] */
        direction = CPSS_PORT_DIRECTION_TX_E;

        st = cpssDxChPtpTimestampEntryGet(dev, direction, &entry);
        if (st == GT_NO_MORE) 
        {
            st = GT_OK; 
        }
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpTimestampEntryGet
                            (dev, direction, &entry),
                            direction);

        /*
            1.3. Call api with wrong entryPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPtpTimestampEntryGet(dev, direction, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpTimestampEntryGet(dev, direction, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpTimestampEntryGet(dev, direction, &entry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpTodCounterFunctionGet
(
    IN  GT_U8                               dev,
    IN  CPSS_PORT_DIRECTION_ENT             direction,
    OUT CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT  *functionPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpTodCounterFunctionGet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E /
                             CPSS_PORT_DIRECTION_BOTH_E],
    Expected: GT_OK.
    1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
    Expected: NOT GT_OK.
    1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.4. Call api with wrong functionPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                               dev;
    CPSS_PORT_DIRECTION_ENT             direction = CPSS_PORT_DIRECTION_RX_E;
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT  function;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E /
                                     CPSS_PORT_DIRECTION_BOTH_E],
            Expected: GT_OK.
        */

        /* call with direction[CPSS_PORT_DIRECTION_RX_E] */
        direction = CPSS_PORT_DIRECTION_RX_E;

        st = cpssDxChPtpTodCounterFunctionGet(dev, direction, &function);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with direction[CPSS_PORT_DIRECTION_TX_E] */
        direction = CPSS_PORT_DIRECTION_TX_E;

        st = cpssDxChPtpTodCounterFunctionGet(dev, direction, &function);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
            Expected: NOT GT_OK.
        */
        direction = CPSS_PORT_DIRECTION_BOTH_E;

        st = cpssDxChPtpTodCounterFunctionGet(dev, direction, &function);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        direction = CPSS_PORT_DIRECTION_TX_E;

        /*
            1.3. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpTodCounterFunctionGet
                            (dev, direction, &function),
                            direction);

        /*
            1.4. Call api with wrong functionPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPtpTodCounterFunctionGet(dev, direction, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpTodCounterFunctionGet(dev, direction, &function);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpTodCounterFunctionGet(dev, direction, &function);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpTodCounterFunctionSet
(
    IN  GT_U8                              dev,
    IN  CPSS_PORT_DIRECTION_ENT            direction,
    IN  CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT function
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpTodCounterFunctionSet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E /
                             CPSS_PORT_DIRECTION_BOTH_E /
                             CPSS_PORT_DIRECTION_RX_E],
                   function[CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E /
                            CPSS_DXCH_PTP_TOD_COUNTER_FUNC_INCREMENT_E /
                            CPSS_DXCH_PTP_TOD_COUNTER_FUNC_CAPTURE_E /
                            CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GENERATE_E],
    Expected: GT_OK.
    1.2. Call cpssDxChPtpTodCounterFunctionGet with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.4. Call api with wrong function [wrong enum values].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                              dev;
    CPSS_PORT_DIRECTION_ENT            direction = CPSS_PORT_DIRECTION_RX_E;
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT function = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E;
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT functionGet = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E /
                                     CPSS_PORT_DIRECTION_BOTH_E /
                                     CPSS_PORT_DIRECTION_RX_E],
                           function[CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E /
                                    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_INCREMENT_E /
                                    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_CAPTURE_E /
                                    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GENERATE_E],
            Expected: GT_OK.
        */

        /* call with direction[CPSS_PORT_DIRECTION_RX_E],
                     function[CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E] */
        direction = CPSS_PORT_DIRECTION_RX_E;
        function = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E;

        st = cpssDxChPtpTodCounterFunctionSet(dev, direction, function);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpTodCounterFunctionGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpTodCounterFunctionGet(dev, direction, &functionGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTodCounterFunctionGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(function, functionGet, 
                       "got another function then was set: %d", dev);

        /* call with direction[CPSS_PORT_DIRECTION_TX_E],
                     function[CPSS_DXCH_PTP_TOD_COUNTER_FUNC_INCREMENT_E] */
        direction = CPSS_PORT_DIRECTION_TX_E;
        function = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_INCREMENT_E;

        st = cpssDxChPtpTodCounterFunctionSet(dev, direction, function);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpTodCounterFunctionGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpTodCounterFunctionGet(dev, direction, &functionGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTodCounterFunctionGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(function, functionGet, 
                       "got another function then was set: %d", dev);

        /* call with direction[CPSS_PORT_DIRECTION_BOTH_E],
                     function[CPSS_DXCH_PTP_TOD_COUNTER_FUNC_CAPTURE_E] */
        direction = CPSS_PORT_DIRECTION_BOTH_E;
        function = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_CAPTURE_E;

        st = cpssDxChPtpTodCounterFunctionSet(dev, direction, function);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpTodCounterFunctionGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        direction = CPSS_PORT_DIRECTION_RX_E;
        st = cpssDxChPtpTodCounterFunctionGet(dev, direction, &functionGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTodCounterFunctionGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(function, functionGet, 
                       "got another function then was set: %d", dev);

        direction = CPSS_PORT_DIRECTION_TX_E;
        st = cpssDxChPtpTodCounterFunctionGet(dev, direction, &functionGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTodCounterFunctionGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(function, functionGet, 
                       "got another function then was set: %d", dev);

        /* call with direction[CPSS_PORT_DIRECTION_RX_E],
                     function[CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GENERATE_E] */
        direction = CPSS_PORT_DIRECTION_RX_E;
        function = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GENERATE_E;

        st = cpssDxChPtpTodCounterFunctionSet(dev, direction, function);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpTodCounterFunctionGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpTodCounterFunctionGet(dev, direction, &functionGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTodCounterFunctionGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(function, functionGet, 
                       "got another function then was set: %d", dev);

        /*
            1.3. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpTodCounterFunctionSet
                            (dev, direction, function),
                            direction);

        /*
            1.4. Call api with wrong function [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpTodCounterFunctionSet
                            (dev, direction, function),
                            function);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
    function = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpTodCounterFunctionSet(dev, direction, function);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpTodCounterFunctionSet(dev, direction, function);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpTodCounterFunctionTriggerGet
(
    IN  GT_U8                    dev,
    IN  CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_BOOL                  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpTodCounterFunctionTriggerGet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E]
    Expected: GT_OK.
    1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
    Expected: NOT GT_OK.
    1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.4. Call api with wrong enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                    dev;
    CPSS_PORT_DIRECTION_ENT  direction = CPSS_PORT_DIRECTION_RX_E;
    GT_BOOL                  enable;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E]
            Expected: GT_OK.
        */

        /* call with direction[CPSS_PORT_DIRECTION_RX_E] */
        direction = CPSS_PORT_DIRECTION_RX_E;

        st = cpssDxChPtpTodCounterFunctionTriggerGet(dev, direction, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with direction[CPSS_PORT_DIRECTION_TX_E] */
        direction = CPSS_PORT_DIRECTION_TX_E;

        st = cpssDxChPtpTodCounterFunctionTriggerGet(dev, direction, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
            Expected: NOT GT_OK.
        */
        direction = CPSS_PORT_DIRECTION_BOTH_E;

        st = cpssDxChPtpTodCounterFunctionTriggerGet(dev, direction, &enable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        direction = CPSS_PORT_DIRECTION_TX_E;

        /*
            1.3. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpTodCounterFunctionTriggerGet
                            (dev, direction, &enable),
                            direction);

        /*
            1.4. Call api with wrong enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPtpTodCounterFunctionTriggerGet(dev, direction, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpTodCounterFunctionTriggerGet(dev, direction, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpTodCounterFunctionTriggerGet(dev, direction, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpTodCounterFunctionTriggerSet
(
    IN  GT_U8                   dev,
    IN  CPSS_PORT_DIRECTION_ENT direction
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpTodCounterFunctionTriggerSet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E /
                             CPSS_PORT_DIRECTION_BOTH_E],
    Expected: GT_OK.
    1.2. Call cpssDxChPtpTodCounterFunctionTriggerGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                   dev;
    CPSS_PORT_DIRECTION_ENT direction = CPSS_PORT_DIRECTION_RX_E;
    GT_BOOL                 enable = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E /
                                     CPSS_PORT_DIRECTION_BOTH_E],
            Expected: GT_OK.
        */

        /* call with direction[CPSS_PORT_DIRECTION_RX_E] */
        direction = CPSS_PORT_DIRECTION_RX_E;

        st = cpssDxChPtpTodCounterFunctionTriggerSet(dev, direction);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpTodCounterFunctionTriggerGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpTodCounterFunctionTriggerGet(dev, direction, &enable);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTodCounterFunctionTriggerGet: %d ", dev);

        /* Verifying values */

        /* call with direction[CPSS_PORT_DIRECTION_TX_E] */
        direction = CPSS_PORT_DIRECTION_TX_E;

        st = cpssDxChPtpTodCounterFunctionTriggerSet(dev, direction);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpTodCounterFunctionTriggerGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpTodCounterFunctionTriggerGet(dev, direction, &enable);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTodCounterFunctionTriggerGet: %d ", dev);

        /* Verifying values */

        /* call with direction[CPSS_PORT_DIRECTION_BOTH_E] */
        direction = CPSS_PORT_DIRECTION_BOTH_E;

        st = cpssDxChPtpTodCounterFunctionTriggerSet(dev, direction);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpTodCounterFunctionTriggerGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        direction = CPSS_PORT_DIRECTION_RX_E;
        st = cpssDxChPtpTodCounterFunctionTriggerGet(dev, direction, &enable);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTodCounterFunctionTriggerGet: %d ", dev);

        direction = CPSS_PORT_DIRECTION_TX_E;
        st = cpssDxChPtpTodCounterFunctionTriggerGet(dev, direction, &enable);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTodCounterFunctionTriggerGet: %d ", dev);

        /*
            1.3. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpTodCounterFunctionTriggerSet
                            (dev, direction),
                            direction);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpTodCounterFunctionTriggerSet(dev, direction);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpTodCounterFunctionTriggerSet(dev, direction);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpTodCounterGet
(
    IN  GT_U8                        dev,
    IN  CPSS_PORT_DIRECTION_ENT      direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC  *todCounterPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpTodCounterGet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E /
                             CPSS_PORT_DIRECTION_BOTH_E],
    Expected: GT_OK.
    1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
    Expected: NOT GT_OK.
    1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.4. Call api with wrong todCounterPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                        dev;
    CPSS_PORT_DIRECTION_ENT      direction = CPSS_PORT_DIRECTION_RX_E;
    CPSS_DXCH_PTP_TOD_COUNT_STC  todCounter;

    cpssOsBzero((GT_VOID*) &todCounter, sizeof(todCounter));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E]
            Expected: GT_OK.
        */
        /* call with direction[CPSS_PORT_DIRECTION_RX_E] */
        direction = CPSS_PORT_DIRECTION_RX_E;

        st = cpssDxChPtpTodCounterGet(dev, direction, &todCounter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with direction[CPSS_PORT_DIRECTION_TX_E] */
        direction = CPSS_PORT_DIRECTION_TX_E;

        st = cpssDxChPtpTodCounterGet(dev, direction, &todCounter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
            Expected: NOT GT_OK.
        */
        direction = CPSS_PORT_DIRECTION_BOTH_E;

        st = cpssDxChPtpTodCounterGet(dev, direction, &todCounter);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        direction = CPSS_PORT_DIRECTION_TX_E;

        /*
            1.3. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpTodCounterGet
                            (dev, direction, &todCounter),
                            direction);

        /*
            1.4. Call api with wrong todCounterPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPtpTodCounterGet(dev, direction, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpTodCounterGet(dev, direction, &todCounter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpTodCounterGet(dev, direction, &todCounter);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpTodCounterShadowGet
(
    IN  GT_U8                        dev,
    IN  CPSS_PORT_DIRECTION_ENT      direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC  *todCounterPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpTodCounterShadowGet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E /
                             CPSS_PORT_DIRECTION_BOTH_E],
    Expected: GT_OK.
    1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
    Expected: NOT GT_OK.
    1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.4. Call api with wrong todCounterPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                        dev;
    CPSS_PORT_DIRECTION_ENT      direction = CPSS_PORT_DIRECTION_RX_E;
    CPSS_DXCH_PTP_TOD_COUNT_STC  todCounter;

    cpssOsBzero((GT_VOID*) &todCounter, sizeof(todCounter));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E]
            Expected: GT_OK.
        */
        /* call with direction[CPSS_PORT_DIRECTION_RX_E] */
        direction = CPSS_PORT_DIRECTION_RX_E;

        st = cpssDxChPtpTodCounterShadowGet(dev, direction, &todCounter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with direction[CPSS_PORT_DIRECTION_TX_E] */
        direction = CPSS_PORT_DIRECTION_TX_E;

        st = cpssDxChPtpTodCounterShadowGet(dev, direction, &todCounter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
            Expected: NOT GT_OK.
        */
        direction = CPSS_PORT_DIRECTION_BOTH_E;

        st = cpssDxChPtpTodCounterShadowGet(dev, direction, &todCounter);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        direction = CPSS_PORT_DIRECTION_TX_E;

        /*
            1.3. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpTodCounterShadowGet
                            (dev, direction, &todCounter),
                            direction);

        /*
            1.4. Call api with wrong todCounterPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPtpTodCounterShadowGet(dev, direction, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpTodCounterShadowGet(dev, direction, &todCounter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpTodCounterShadowGet(dev, direction, &todCounter);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpTodCounterShadowSet
(
    IN  GT_U8                       dev,
    IN  CPSS_PORT_DIRECTION_ENT     direction,
    IN  CPSS_DXCH_PTP_TOD_COUNT_STC todCounter
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpTodCounterShadowSet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E /
                             CPSS_PORT_DIRECTION_BOTH_E],
                    todCounter.seconds.l[0] = 1000;
                    todCounter.seconds.l[1] = 2000;
    Expected: GT_OK.
    1.2. Call cpssDxChPtpTodCounterShadowGet with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;
    GT_BOOL   isEqual = GT_FALSE;

    GT_U8                       dev;
    CPSS_PORT_DIRECTION_ENT     direction = CPSS_PORT_DIRECTION_RX_E;
    CPSS_DXCH_PTP_TOD_COUNT_STC todCounter;
    CPSS_DXCH_PTP_TOD_COUNT_STC todCounterGet;

    cpssOsBzero((GT_VOID*) &todCounter, sizeof(todCounter));
    cpssOsBzero((GT_VOID*) &todCounter, sizeof(todCounter));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E /
                                     CPSS_PORT_DIRECTION_BOTH_E],
                            todCounter.seconds.l[0] = 1000;
                            todCounter.seconds.l[1] = 2000;
            Expected: GT_OK.
        */

        /* call with direction[CPSS_PORT_DIRECTION_RX_E]*/
        direction = CPSS_PORT_DIRECTION_RX_E;
        todCounter.seconds.l[0] = 1000;
        todCounter.seconds.l[1] = 2000;

        st = cpssDxChPtpTodCounterShadowSet(dev, direction, &todCounter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpTodCounterShadowGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpTodCounterShadowGet(dev, direction, &todCounterGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTodCounterShadowGet: %d ", dev);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&todCounter, 
                                     (GT_VOID*)&todCounterGet,
                  sizeof (todCounter))) ? GT_TRUE : GT_FALSE;

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another todCounter than was set: %d", dev);

        /* call with direction[CPSS_PORT_DIRECTION_TX_E]*/
        direction = CPSS_PORT_DIRECTION_TX_E;
        todCounter.seconds.l[0] = 1000;
        todCounter.seconds.l[1] = 2000;

        st = cpssDxChPtpTodCounterShadowSet(dev, direction, &todCounter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpTodCounterShadowGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpTodCounterShadowGet(dev, direction, &todCounterGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTodCounterShadowGet: %d ", dev);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&todCounter, 
                                     (GT_VOID*)&todCounterGet,
                  sizeof (todCounter))) ? GT_TRUE : GT_FALSE;

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another todCounter than was set: %d", dev);

        /* call with direction[CPSS_PORT_DIRECTION_BOTH_E]*/
        direction = CPSS_PORT_DIRECTION_BOTH_E;
        todCounter.seconds.l[0] = 1000;
        todCounter.seconds.l[1] = 2000;

        st = cpssDxChPtpTodCounterShadowSet(dev, direction, &todCounter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpTodCounterShadowGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        direction = CPSS_PORT_DIRECTION_RX_E;
        st = cpssDxChPtpTodCounterShadowGet(dev, direction, &todCounterGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTodCounterShadowGet: %d ", dev);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&todCounter, 
                                     (GT_VOID*)&todCounterGet,
                  sizeof (todCounter))) ? GT_TRUE : GT_FALSE;

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another todCounter than was set: %d", dev);

        direction = CPSS_PORT_DIRECTION_TX_E;
        st = cpssDxChPtpTodCounterShadowGet(dev, direction, &todCounterGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTodCounterShadowGet: %d ", dev);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&todCounter, 
                                     (GT_VOID*)&todCounterGet,
                  sizeof (todCounter))) ? GT_TRUE : GT_FALSE;

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another todCounter than was set: %d", dev);


        /* check if WA for nanoseconds is enabled */
        if (GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(dev, PRV_CPSS_DXCH_XCAT_TOD_NANO_SEC_SET_WA_E))
        {
            /* set nanosecond to value that is not a multiply of 20 */

            /* call with direction[CPSS_PORT_DIRECTION_BOTH_E] */
            direction = CPSS_PORT_DIRECTION_BOTH_E;

            todCounter.nanoSeconds = 1150;
            todCounter.seconds.l[0] = 200500;
            todCounter.seconds.l[1] = 500;

            st = cpssDxChPtpTodCounterShadowSet(dev, direction, &todCounter);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* calculate nanosecons value */
            todCounter.nanoSeconds = (todCounter.nanoSeconds / 20);
            todCounter.nanoSeconds = (todCounter.nanoSeconds * 20);

            /*
                1.1.2. Call cpssDxChPtpPortGroupTodCounterShadowGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            direction = CPSS_PORT_DIRECTION_RX_E;
            st = cpssDxChPtpTodCounterShadowGet(dev, direction, &todCounterGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPtpPortGroupTodCounterShadowGet: %d ", dev);

            /* Verifying values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&todCounter, 
                                         (GT_VOID*)&todCounterGet,
                      sizeof (todCounter))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another todCounter than was set: %d", dev);

            direction = CPSS_PORT_DIRECTION_TX_E;
            st = cpssDxChPtpTodCounterShadowGet(dev, direction, &todCounterGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPtpPortGroupTodCounterShadowGet: %d ", dev);

            /* Verifying values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&todCounter, 
                                         (GT_VOID*)&todCounterGet,
                      sizeof (todCounter))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another todCounter than was set: %d", dev);
        }


        /*
            1.3. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpTodCounterShadowSet
                            (dev, direction, &todCounter),
                            direction);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
    todCounter.nanoSeconds = 0;
    todCounter.seconds.l[0] = 1000;
    todCounter.seconds.l[1] = 2000;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpTodCounterShadowSet(dev, direction, &todCounter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpTodCounterShadowSet(dev, direction, &todCounter);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpTransportSpecificCheckEnableGet
(
    IN  GT_U8                    dev,
    IN  CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_BOOL                  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpTransportSpecificCheckEnableGet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E]
    Expected: GT_OK.
    1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
    Expected: NOT GT_OK.
    1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.4. Call api with wrong enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                    dev;
    CPSS_PORT_DIRECTION_ENT  direction = CPSS_PORT_DIRECTION_RX_E;
    GT_BOOL                  enable;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E ]
            Expected: GT_OK.
        */

        /* call with direction[CPSS_PORT_DIRECTION_RX_E] */
        direction = CPSS_PORT_DIRECTION_RX_E;

        st = cpssDxChPtpTransportSpecificCheckEnableGet(dev, direction, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with direction[CPSS_PORT_DIRECTION_TX_E] */
        direction = CPSS_PORT_DIRECTION_TX_E;

        st = cpssDxChPtpTransportSpecificCheckEnableGet(dev, direction, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
            Expected: NOT GT_OK.
        */
        /* call with direction[CPSS_PORT_DIRECTION_BOTH_E] */
        direction = CPSS_PORT_DIRECTION_BOTH_E;

        st = cpssDxChPtpTransportSpecificCheckEnableGet(dev, direction, &enable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        direction = CPSS_PORT_DIRECTION_TX_E;

        /*
            1.3. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpTransportSpecificCheckEnableGet
                            (dev, direction, &enable),
                            direction);

        /*
            1.4. Call api with wrong enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPtpTransportSpecificCheckEnableGet(dev, direction, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpTransportSpecificCheckEnableGet(dev, direction, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpTransportSpecificCheckEnableGet(dev, direction, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpTransportSpecificCheckEnableSet
(
    IN  GT_U8                   dev,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_BOOL                 enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpTransportSpecificCheckEnableSet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E /
                             CPSS_PORT_DIRECTION_BOTH_E],
                   enable[GT_TRUE /GT_FALSE /GT_TRUE],
    Expected: GT_OK.
    1.2. Call cpssDxChPtpTransportSpecificCheckEnableGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                   dev;
    CPSS_PORT_DIRECTION_ENT direction = CPSS_PORT_DIRECTION_RX_E;
    GT_BOOL                 enable = GT_FALSE;
    GT_BOOL                 enableGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E /
                                     CPSS_PORT_DIRECTION_BOTH_E],
                           enable[GT_TRUE /GT_FALSE /GT_TRUE],
            Expected: GT_OK.
        */

        /* call with direction[CPSS_PORT_DIRECTION_RX_E], enable[GT_TRUE] */
        direction = CPSS_PORT_DIRECTION_RX_E;
        enable = GT_TRUE;

        st = cpssDxChPtpTransportSpecificCheckEnableSet(dev, direction, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpTransportSpecificCheckEnableGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpTransportSpecificCheckEnableGet(dev, direction, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTransportSpecificCheckEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        /* call with direction[CPSS_PORT_DIRECTION_TX_E], enable[GT_FALSE] */
        direction = CPSS_PORT_DIRECTION_TX_E;
        enable = GT_FALSE;

        st = cpssDxChPtpTransportSpecificCheckEnableSet(dev, direction, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpTransportSpecificCheckEnableGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpTransportSpecificCheckEnableGet(dev, direction, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTransportSpecificCheckEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        /* call with direction[CPSS_PORT_DIRECTION_BOTH_E], enable[GT_TRUE] */
        direction = CPSS_PORT_DIRECTION_BOTH_E;
        enable = GT_TRUE;

        st = cpssDxChPtpTransportSpecificCheckEnableSet(dev, direction, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpTransportSpecificCheckEnableGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        direction = CPSS_PORT_DIRECTION_RX_E;
        st = cpssDxChPtpTransportSpecificCheckEnableGet(dev, direction, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTransportSpecificCheckEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        direction = CPSS_PORT_DIRECTION_TX_E;
        st = cpssDxChPtpTransportSpecificCheckEnableGet(dev, direction, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTransportSpecificCheckEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        /*
            1.3. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpTransportSpecificCheckEnableSet
                            (dev, direction, enable),
                            direction);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
    enable = GT_TRUE;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpTransportSpecificCheckEnableSet(dev, direction, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpTransportSpecificCheckEnableSet(dev, direction, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpTransportSpecificTimeStampEnableGet
(
    IN  GT_U8                    dev,
    IN  CPSS_PORT_DIRECTION_ENT  direction,
    IN  GT_U32                   transportSpecificVal,
    OUT GT_BOOL                  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpTransportSpecificTimeStampEnableGet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E ]
                   transportSpecificVal[0 / 100],
    Expected: GT_OK.
    1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
    Expected: NOT GT_OK.
    1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.4. Call api with wrong enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                    dev;
    CPSS_PORT_DIRECTION_ENT  direction = CPSS_PORT_DIRECTION_RX_E;
    GT_U32                   transportSpecificVal = 0;
    GT_BOOL                  enable;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E ]
                           transportSpecificVal[0 / 10],
            Expected: GT_OK.
        */

        /* call with direction[CPSS_PORT_DIRECTION_RX_E],
                     transportSpecificVal[0] */
        direction = CPSS_PORT_DIRECTION_RX_E;
        transportSpecificVal = 0;

        st = cpssDxChPtpTransportSpecificTimeStampEnableGet(dev, 
                            direction, transportSpecificVal, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with direction[CPSS_PORT_DIRECTION_TX_E],
                     transportSpecificVal[10] */
        direction = CPSS_PORT_DIRECTION_TX_E;
        transportSpecificVal = 10;

        st = cpssDxChPtpTransportSpecificTimeStampEnableGet(dev, 
                            direction, transportSpecificVal, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
            Expected: NOT GT_OK.
        */
        direction = CPSS_PORT_DIRECTION_BOTH_E;

        st = cpssDxChPtpTransportSpecificTimeStampEnableGet(dev, 
                            direction, transportSpecificVal, &enable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        direction = CPSS_PORT_DIRECTION_TX_E;

        /*
            1.3. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpTransportSpecificTimeStampEnableGet
                            (dev, direction, transportSpecificVal, &enable),
                            direction);

        /*
            1.4. Call api with wrong enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPtpTransportSpecificTimeStampEnableGet(dev, 
                            direction, transportSpecificVal, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
    transportSpecificVal = 0;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpTransportSpecificTimeStampEnableGet(dev, 
                            direction, transportSpecificVal, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpTransportSpecificTimeStampEnableGet(dev, 
                            direction, transportSpecificVal, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpTransportSpecificTimeStampEnableSet
(
    IN  GT_U8                   dev,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  transportSpecificVal,
    IN  GT_BOOL                 enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpTransportSpecificTimeStampEnableSet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E /
                             CPSS_PORT_DIRECTION_BOTH_E],
                   transportSpecificVal[0 / 10 / 15],
                   enable[GT_TRUE /GT_FALSE /GT_TRUE],
    Expected: GT_OK.
    1.2. Call cpssDxChPtpTransportSpecificTimeStampEnableGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.3. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.4. Call api with wrong transportSpecificVal [16].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8                   dev;
    CPSS_PORT_DIRECTION_ENT direction = CPSS_PORT_DIRECTION_RX_E;
    GT_U32                  transportSpecificVal = 0;
    GT_BOOL                 enable = GT_FALSE;
    GT_BOOL                 enableGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E /
                                     CPSS_PORT_DIRECTION_BOTH_E],
                           transportSpecificVal[0 / 10 / 15],
                           enable[GT_TRUE /GT_FALSE /GT_TRUE],
            Expected: GT_OK.
        */
        /* call with direction[CPSS_PORT_DIRECTION_RX_E],
                     transportSpecificVal[0], enable[GT_TRUE] */
        direction = CPSS_PORT_DIRECTION_RX_E;
        transportSpecificVal = 0;
        enable = GT_TRUE;

        st = cpssDxChPtpTransportSpecificTimeStampEnableSet(dev, 
                            direction, transportSpecificVal, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpTransportSpecificTimeStampEnableGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpTransportSpecificTimeStampEnableGet(dev, 
                            direction, transportSpecificVal, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTransportSpecificTimeStampEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        /* call with direction[CPSS_PORT_DIRECTION_TX_E],
                     transportSpecificVal[10], enable[GT_FALSE] */
        direction = CPSS_PORT_DIRECTION_TX_E;
        transportSpecificVal = 10;
        enable = GT_FALSE;

        st = cpssDxChPtpTransportSpecificTimeStampEnableSet(dev, 
                            direction, transportSpecificVal, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpTransportSpecificTimeStampEnableGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpTransportSpecificTimeStampEnableGet(dev, 
                            direction, transportSpecificVal, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTransportSpecificTimeStampEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        /* call with direction[CPSS_PORT_DIRECTION_BOTH_E],
                     transportSpecificVal[15], enable[GT_TRUE] */
        direction = CPSS_PORT_DIRECTION_BOTH_E;
        transportSpecificVal = 15;
        enable = GT_TRUE;

        st = cpssDxChPtpTransportSpecificTimeStampEnableSet(dev, 
                            direction, transportSpecificVal, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpTransportSpecificTimeStampEnableGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        direction = CPSS_PORT_DIRECTION_RX_E;
        st = cpssDxChPtpTransportSpecificTimeStampEnableGet(dev, 
                            direction, transportSpecificVal, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTransportSpecificTimeStampEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        direction = CPSS_PORT_DIRECTION_TX_E;
        st = cpssDxChPtpTransportSpecificTimeStampEnableGet(dev, 
                            direction, transportSpecificVal, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpTransportSpecificTimeStampEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);


        /*
            1.3. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpTransportSpecificTimeStampEnableSet
                            (dev, direction, transportSpecificVal, enable),
                            direction);

        /*
            1.4. Call api with wrong transportSpecificVal [16].
            Expected: NOT GT_OK.
        */
        transportSpecificVal = 16;

        st = cpssDxChPtpTransportSpecificTimeStampEnableSet(dev, 
                            direction, transportSpecificVal, enable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        transportSpecificVal = 0;
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
    transportSpecificVal = 0;
    enable = GT_TRUE;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpTransportSpecificTimeStampEnableSet(dev, 
                            direction, transportSpecificVal, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpTransportSpecificTimeStampEnableSet(dev, 
                            direction, transportSpecificVal, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpUdpDestPortsGet
(
    IN  GT_U8   dev,
    IN  GT_U32  udpPortIndex,
    OUT GT_U32  *udpPortNumPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpUdpDestPortsGet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with udpPortIndex[0 / 1],
    Expected: GT_OK.
    1.2. Call api with wrong udpPortIndex [2].
    Expected: NOT GT_OK.
    1.3. Call api with wrong udpPortNumPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U32  udpPortIndex = 0;
    GT_U32  udpPortNum;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with udpPortIndex[0 / 1],
            Expected: GT_OK.
        */
        /* call with udpPortIndex[0] */
        udpPortIndex = 0;

        st = cpssDxChPtpUdpDestPortsGet(dev, udpPortIndex, &udpPortNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with udpPortIndex[1] */
        udpPortIndex = 1;

        st = cpssDxChPtpUdpDestPortsGet(dev, udpPortIndex, &udpPortNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call api with wrong udpPortIndex [2].
            Expected: NOT GT_OK.
        */
        udpPortIndex = 2;

        st = cpssDxChPtpUdpDestPortsGet(dev, udpPortIndex, &udpPortNum);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        udpPortIndex = 0;

        /*
            1.3. Call api with wrong udpPortNumPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPtpUdpDestPortsGet(dev, udpPortIndex, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* restore correct values */
    udpPortIndex = 0;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpUdpDestPortsGet(dev, udpPortIndex, &udpPortNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpUdpDestPortsGet(dev, udpPortIndex, &udpPortNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpUdpDestPortsSet
(
    IN  GT_U8  dev,
    IN  GT_U32 udpPortIndex,
    IN  GT_U32 udpPortNum
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpUdpDestPortsSet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with udpPortIndex[0 / 1], udpPortNum[0 / 0xFFFF],
    Expected: GT_OK.
    1.2. Call cpssDxChPtpUdpDestPortsGet with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.3. Call api with wrong udpPortIndex [2].
    Expected: NOT GT_OK.
    1.4. Call api with wrong udpPortNum [0xFFFF + 1].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8  dev;
    GT_U32 udpPortIndex = 0;
    GT_U32 udpPortNum = 0;
    GT_U32 udpPortNumGet = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with udpPortIndex[0 / 1], udpPortNum[0 / 0xFFFF],
            Expected: GT_OK.
        */
        /* call with udpPortIndex[0], udpPortNum[0] */
        udpPortIndex = 0;
        udpPortNum = 0;

        st = cpssDxChPtpUdpDestPortsSet(dev, udpPortIndex, udpPortNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpUdpDestPortsGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpUdpDestPortsGet(dev, udpPortIndex, &udpPortNumGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpUdpDestPortsGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(udpPortNum, udpPortNumGet, 
                       "got another udpPortNum then was set: %d", dev);

        /* call with udpPortIndex[1], udpPortNum[0xFFFF] */
        udpPortIndex = 1;
        udpPortNum = 0xFFFF;

        st = cpssDxChPtpUdpDestPortsSet(dev, udpPortIndex, udpPortNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpUdpDestPortsGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpUdpDestPortsGet(dev, udpPortIndex, &udpPortNumGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpUdpDestPortsGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(udpPortNum, udpPortNumGet, 
                       "got another udpPortNum then was set: %d", dev);


        /* call with udpPortIndex[0], udpPortNum[0x8F07] */
        udpPortIndex = 0;
        udpPortNum = 0x8F07;

        st = cpssDxChPtpUdpDestPortsSet(dev, udpPortIndex, udpPortNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpUdpDestPortsGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpUdpDestPortsGet(dev, udpPortIndex, &udpPortNumGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpUdpDestPortsGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(udpPortNum, udpPortNumGet, 
                       "got another udpPortNum then was set: %d", dev);

        /* call with udpPortIndex[1], udpPortNum[0xAA55] */
        udpPortIndex = 1;
        udpPortNum = 0xAA55;

        st = cpssDxChPtpUdpDestPortsSet(dev, udpPortIndex, udpPortNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpUdpDestPortsGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpUdpDestPortsGet(dev, udpPortIndex, &udpPortNumGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpUdpDestPortsGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(udpPortNum, udpPortNumGet, 
                       "got another udpPortNum then was set: %d", dev);


        /*
            1.3. Call api with wrong udpPortIndex [2].
            Expected: NOT GT_OK.
        */
        udpPortIndex = 2;

        st = cpssDxChPtpUdpDestPortsSet(dev, udpPortIndex, udpPortNum);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        udpPortIndex = 0;

        /*
            1.4. Call api with wrong udpPortNum [0xFFFF + 1].
            Expected: NOT GT_OK.
        */
        udpPortNum = 0xFFFF + 1;

        st = cpssDxChPtpUdpDestPortsSet(dev, udpPortIndex, udpPortNum);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        udpPortNum = 0;
    }

    /* restore correct values */
    udpPortIndex = 0;
    udpPortNum = 0;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpUdpDestPortsSet(dev, udpPortIndex, udpPortNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpUdpDestPortsSet(dev, udpPortIndex, udpPortNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpUdpTimeStampEnableGet
(
    IN  GT_U8                       dev,
    IN  CPSS_PORT_DIRECTION_ENT     direction,
    IN  CPSS_IP_PROTOCOL_STACK_ENT  protocolStack,
    OUT GT_BOOL                     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpUdpTimeStampEnableGet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E]
                   protocolStack[CPSS_IP_PROTOCOL_IPV4_E /
                                 CPSS_IP_PROTOCOL_IPV6_E]
    Expected: GT_OK.
    1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
    Expected: NOT GT_OK.
    1.3. Call with wrong protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
    Expected: NOT GT_OK.
    1.4. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.5. Call api with wrong protocolStack [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.6. Call api with wrong enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                       dev;
    CPSS_PORT_DIRECTION_ENT     direction = CPSS_PORT_DIRECTION_RX_E;
    CPSS_IP_PROTOCOL_STACK_ENT  protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    GT_BOOL                     enable;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E]
                           protocolStack[CPSS_IP_PROTOCOL_IPV4_E /
                                         CPSS_IP_PROTOCOL_IPV6_E]
            Expected: GT_OK.
        */
        /* call with direction[CPSS_PORT_DIRECTION_RX_E],
                     protocolStack[CPSS_IP_PROTOCOL_IPV4_E] */
        direction = CPSS_PORT_DIRECTION_RX_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        st = cpssDxChPtpUdpTimeStampEnableGet(dev, direction, protocolStack, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with direction[CPSS_PORT_DIRECTION_TX_E],
                     protocolStack[CPSS_IP_PROTOCOL_IPV6_E] */
        direction = CPSS_PORT_DIRECTION_TX_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssDxChPtpUdpTimeStampEnableGet(dev, direction, protocolStack, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong direction[CPSS_PORT_DIRECTION_BOTH_E],
            Expected: NOT GT_OK.
        */
        direction = CPSS_PORT_DIRECTION_BOTH_E;

        st = cpssDxChPtpUdpTimeStampEnableGet(dev, direction, protocolStack, &enable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        direction = CPSS_PORT_DIRECTION_TX_E;

        /*
            1.3. Call with wrong protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E].
            Expected: NOT GT_OK.
        */
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssDxChPtpUdpTimeStampEnableGet(dev, direction, protocolStack, &enable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;

        /*
            1.4. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpUdpTimeStampEnableGet
                            (dev, direction, protocolStack, &enable),
                            direction);

        /*
            1.5. Call api with wrong protocolStack [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpUdpTimeStampEnableGet
                            (dev, direction, protocolStack, &enable),
                            protocolStack);

        /*
            1.6. Call api with wrong enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPtpUdpTimeStampEnableGet(dev, direction, protocolStack, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpUdpTimeStampEnableGet(dev, direction, protocolStack, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpUdpTimeStampEnableGet(dev, direction, protocolStack, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPtpUdpTimeStampEnableSet
(
    IN  GT_U8                      dev,
    IN  CPSS_PORT_DIRECTION_ENT    direction,
    IN  CPSS_IP_PROTOCOL_STACK_ENT protocolStack,
    IN  GT_BOOL                    enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPtpUdpTimeStampEnableSet)
{
/*
    ITERATE_DEVICES(Lion and above)
    1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                             CPSS_PORT_DIRECTION_TX_E /
                             CPSS_PORT_DIRECTION_BOTH_E],
                   protocolStack[CPSS_IP_PROTOCOL_IPV4_E /
                                 CPSS_IP_PROTOCOL_IPV6_E]
                   enable[GT_TRUE /GT_FALSE /GT_TRUE],
    Expected: GT_OK.
    1.2. Call cpssDxChPtpUdpTimeStampEnableGet with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.3. Call with wrong protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: NOT GT_OK.
    1.4. Call api with wrong direction [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.5. Call api with wrong protocolStack [wrong enum values].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                      dev;
    CPSS_PORT_DIRECTION_ENT    direction = CPSS_PORT_DIRECTION_RX_E;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    GT_BOOL                    enable = GT_FALSE;
    GT_BOOL                    enableGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction[CPSS_PORT_DIRECTION_RX_E /
                                     CPSS_PORT_DIRECTION_TX_E /
                                     CPSS_PORT_DIRECTION_BOTH_E],
                           protocolStack[CPSS_IP_PROTOCOL_IPV4_E /
                                         CPSS_IP_PROTOCOL_IPV6_E]
                           enable[GT_TRUE /GT_FALSE /GT_TRUE],
            Expected: GT_OK.
        */
        /* call with direction[CPSS_PORT_DIRECTION_RX_E],
                     protocolStack[CPSS_IP_PROTOCOL_IPV4_E], enable[GT_TRUE] */
        direction = CPSS_PORT_DIRECTION_RX_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        enable = GT_TRUE;

        st = cpssDxChPtpUdpTimeStampEnableSet(dev, direction, protocolStack, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpUdpTimeStampEnableGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpUdpTimeStampEnableGet(dev, direction, protocolStack, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpUdpTimeStampEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        /* call with direction[CPSS_PORT_DIRECTION_TX_E],
                     protocolStack[CPSS_IP_PROTOCOL_IPV6_E], enable[GT_FALSE] */
        direction = CPSS_PORT_DIRECTION_TX_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        enable = GT_FALSE;

        st = cpssDxChPtpUdpTimeStampEnableSet(dev, direction, protocolStack, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpUdpTimeStampEnableGet with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPtpUdpTimeStampEnableGet(dev, direction, protocolStack, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpUdpTimeStampEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        /* call with direction[CPSS_PORT_DIRECTION_BOTH_E], enable[GT_TRUE] */
        direction = CPSS_PORT_DIRECTION_BOTH_E;
        enable = GT_TRUE;

        st = cpssDxChPtpUdpTimeStampEnableSet(dev, direction, protocolStack, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPtpUdpTimeStampEnableGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        direction = CPSS_PORT_DIRECTION_RX_E;
        st = cpssDxChPtpUdpTimeStampEnableGet(dev, direction, protocolStack, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpUdpTimeStampEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        direction = CPSS_PORT_DIRECTION_TX_E;
        st = cpssDxChPtpUdpTimeStampEnableGet(dev, direction, protocolStack, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPtpUdpTimeStampEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, 
                       "got another enable then was set: %d", dev);

        /*
            1.3. Call with correct protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E].
            Expected: GT_OK.
        */
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssDxChPtpUdpTimeStampEnableSet(dev, direction, protocolStack, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;

        /*
            1.4. Call api with wrong direction [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpUdpTimeStampEnableSet
                            (dev, direction, protocolStack, enable),
                            direction);

        /*
            1.5. Call api with wrong protocolStack [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPtpUdpTimeStampEnableSet
                            (dev, direction, protocolStack, enable),
                            protocolStack);
    }

    /* restore correct values */
    direction = CPSS_PORT_DIRECTION_RX_E;
    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    enable = GT_TRUE;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPtpUdpTimeStampEnableSet(dev, direction, protocolStack, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPtpUdpTimeStampEnableSet(dev, direction, protocolStack, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChPtp suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChPtp)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpCpuCodeBaseGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpCpuCodeBaseSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpEtherTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpEtherTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpEthernetTimeStampEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpEthernetTimeStampEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpMessageTypeCmdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpMessageTypeCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpMessageTypeTimeStampEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpMessageTypeTimeStampEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpPortGroupTodCounterFunctionGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpPortGroupTodCounterFunctionSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpPortGroupTodCounterFunctionTriggerGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpPortGroupTodCounterFunctionTriggerSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpPortGroupTodCounterGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpPortGroupTodCounterShadowGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpPortGroupTodCounterShadowSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpPortTimeStampEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpPortTimeStampEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpTimestampEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpTodCounterFunctionGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpTodCounterFunctionSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpTodCounterFunctionTriggerGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpTodCounterFunctionTriggerSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpTodCounterGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpTodCounterShadowGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpTodCounterShadowSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpTransportSpecificCheckEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpTransportSpecificCheckEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpTransportSpecificTimeStampEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpTransportSpecificTimeStampEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpUdpDestPortsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpUdpDestPortsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpUdpTimeStampEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPtpUdpTimeStampEnableSet)
UTF_SUIT_END_TESTS_MAC(cpssDxChPtp)



